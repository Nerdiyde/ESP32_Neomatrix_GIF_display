// when including in other code, allow disabling all those inits
// to avoid double inits.
/*#ifndef GIFANIM_INCLUDE
#include "animatedgif_config.h"
#endif*/

// Use NeoMatrix backend? Defined in main ino that calls sav_loop

#include "neomatrix_config.h"
#include "GifDecoder.h"

File file;
bool fileSeekCallback(unsigned long position) 
{
  return file.seek(position);
}
unsigned long filePositionCallback(void) 
{
  return file.position();
}
int fileReadCallback(void) 
{
  return file.read();
}
int fileReadBlockCallback(void * buffer, int numberOfBytes) 
{
  return file.read((uint8_t*)buffer, numberOfBytes);
}

GifDecoder<GIF_SIZE, GIF_SIZE, lzwMaxBits> decoder;

void screenClearCallback(void) 
{
  matrix->clear();
}

void updateScreenCallback(void) 
{
  matrix->show();
}

void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue)
{
  red   = matrix->gamma[red];
  green = matrix->gamma[green];
  blue  = matrix->gamma[blue];
  
#if DEBUGLINE

  if (y == DEBUGLINE) 
  {
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(">");
    Serial.print(red);
    Serial.print(",");
    Serial.print(green);
    Serial.print(",");
    Serial.print(blue);
    Serial.println("");
  }
  if (y > DEBUGLINE) return;
  
#endif

  CRGB color = CRGB(red, green, blue);

  uint16_t basex = x * FACTX / 10 + OFFSETX;
  uint16_t basey = y * FACTY / 10 + OFFSETY;

  matrix->drawPixel(basex, basey, color);

  if (FACTX > 10 && FACTY > 10) 
  {
    matrix->drawPixel(basex + 1, basey,   color);
    matrix->drawPixel(basex,   basey + 1, color);
    matrix->drawPixel(basex + 1, basey + 1, color);
  } else if (FACTY > 10) 
  {
    matrix->drawPixel(basex,   basey + 1, color);
  } else if (FACTX > 10) 
  {
    matrix->drawPixel(basex + 1, basey,   color);
  }
}

bool sav_newgif(const char *pathname)
{
  Serial.print(pathname);
  
  if (file) file.close();
  
#ifndef FSOFATFS

  file = FSO.open(pathname, "r");
  
#else

  file = FFat.open(pathname);
  
#endif //FSOFATFS

  if (!file)
  {
    Serial.println(": Error opening GIF file");
    return 1;
  }
  
  Serial.print(": Opened GIF file, start decoding (OFFSETX: ");
  Serial.print(OFFSETX);
  Serial.print(", OFFSETY: ");
  Serial.print(OFFSETY);
  Serial.print(", FACTX: ");
  Serial.print(FACTX);
  Serial.print(", FACTY: ");
  Serial.print(FACTY);
  Serial.println(")");
  
  decoder.startDecoding();
  
  return 0;
}

bool sav_loop()
{
  // ERROR_WAITING means it wasn't time to display the next frame and the display did
  // not get updated (this is important for a neopixel matrix where the display being
  // updated causes a pause in the code).
  if (decoder.decodeFrame() == ERROR_WAITING)
    return 1;
  return 0;
}
