// when including in other code, allow disabling all those inits
// to avoid double inits.
#ifndef GIFANIM_INCLUDE
#include "animatedgif_config.h"
#endif

// Use NeoMatrix backend? Defined in main ino that calls sav_loop

// select which NEOMATRIX config will be selected
//#define MATRIX_SIZE_16X16_4_TILES
#include "neomatrix_config.h"


#include "GifDecoder.h"

/*
// Used by SimpleGifAnimViewer2 to work without FilenameFunctions*
#ifdef UNIXFS

// https://www.programiz.com/c-programming/c-file-input-output
// https://www.gnu.org/software/libc/manual/html_node/I_002fO-on-Streams.html
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <cerrno>
FILE *file;
// This can be used by the caller to retrieve the name by index
char pathname[2048];

bool fileSeekCallback(unsigned long position) {
  return (fseek(file, position, SEEK_SET) != -1);
}
unsigned long filePositionCallback(void) {
  return ftell(file);
}
int fileReadCallback(void) {
  return getc(file);
}
int fileReadBlockCallback(void * buffer, int numberOfBytes) {
  return fread(buffer, 1, numberOfBytes, file);
}

// The functions below belong in FilenameFunctions_Impl.h, but I didn't want
// to fill them with linux ifdefs, so I just made separate versions here:
int enumerateGIFFiles(const char *directoryName, boolean displayFilenames) {
  int numberOfFiles = 0;
  Serial.print("Enumerate files in dir ");
  Serial.println(directoryName);
  DIR *d;
  struct dirent *dir;
  if (! (d = opendir(directoryName))) return 0;
  while ((dir = readdir(d)) != NULL) {
    if (dir->d_name[0] == '.') continue;
    if (!strstr(dir->d_name, ".gif")) continue;
    if (displayFilenames) {
      Serial.print(numberOfFiles);
      Serial.print(" : ");
      Serial.println(dir->d_name);
    }
    numberOfFiles++;
  }
  closedir(d);
  return numberOfFiles;
}

// Get the full path/filename of the GIF file with specified index
// This doesn't check if the files are gifs like the arduino version
void getGIFFilenameByIndex(const char *directoryName, int index, char *pnBuffer) {
  DIR *d;
  struct dirent *dir;
  if (! (d = opendir(directoryName))) return;
  dir = readdir(d);
  while (dir && (index >= 0)) {
    // Serial.print("Check file "); Serial.println(dir->d_name);
    // this doesn't check where in the string, but good enough
    if (!(dir->d_name[0] == '.') && strstr(dir->d_name, ".gif")) {
      index--;
      // Serial.print("index "); Serial.println(index);
      // buffer overflow, directory+file size is on you.
      strcpy(pnBuffer, directoryName);
      strcat(pnBuffer, "/");
      strcat(pnBuffer, dir->d_name);
    }
    dir = readdir(d);
  }
  Serial.print("Selected file ");
  Serial.println(pnBuffer);
}

int openGifFilenameByIndex(const char *directoryName, int index) 
{
  getGIFFilenameByIndex(directoryName, index, pathname);
  if (file) fclose(file);
  file = fopen(pathname, "r");
  return 0;
}


#elif defined(BASICARDUINOFS) //UNIXFS
*/

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
/*
#else //UNIXFS

extern File file;
#include "FilenameFunctions_Impl.h"

#endif //UNIXFS
*/

/* template parameters are maxGifWidth, maxGifHeight, lzwMaxBits
   defined in config.h
*/
GifDecoder<GIF_SIZE, GIF_SIZE, lzwMaxBits> decoder;

void screenClearCallback(void) 
{
//#ifdef NEOMATRIX
  matrix->clear();
/*#else
  backgroundLayer.fillScreen({0, 0, 0});
#endif //NEOMATRIX */
}

void updateScreenCallback(void) 
{
//#ifdef NEOMATRIX
  matrix->show();
/*#else
  backgroundLayer.swapBuffers();
#endif
*/
}

void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue)
{
  red   = matrix->gamma[red];
  green = matrix->gamma[green];
  blue  = matrix->gamma[blue];
#if DEBUGLINE
  if (y == DEBUGLINE) {
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

//#ifdef NEOMATRIX
  CRGB color = CRGB(red, green, blue);

  uint16_t basex = x * FACTX / 10 + OFFSETX;
  uint16_t basey = y * FACTY / 10 + OFFSETY;

  matrix->drawPixel(basex, basey, color);

  if (FACTX > 10 && FACTY > 10) {
    matrix->drawPixel(basex + 1, basey,   color);
    matrix->drawPixel(basex,   basey + 1, color);
    matrix->drawPixel(basex + 1, basey + 1, color);
  } else if (FACTY > 10) {
    matrix->drawPixel(basex,   basey + 1, color);
  } else if (FACTX > 10) {
    matrix->drawPixel(basex + 1, basey,   color);
  }
/*#else
  backgroundLayer.drawPixel(x, y, {red, green, blue});
#endif //NEOMATRIX */
}

bool sav_newgif(const char *pathname)
{
  Serial.print(pathname);
/*
#ifdef ARDUINOONPC
  if (file) fclose(file);
  file = fopen(pathname, "r");
#else
*/
  if (file) file.close();
#ifndef FSOFATFS
  file = FSO.open(pathname, "r");
#else
  file = FFat.open(pathname);
#endif //FSOFATFS
//#endif //ARDUINOONPC

  if (!file)
  {
    Serial.println(": Error opening GIF file");
/*#ifdef ARDUINOONPC
    Serial.println(strerror(errno));
#endif //ARDUINOONPC
*/
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
