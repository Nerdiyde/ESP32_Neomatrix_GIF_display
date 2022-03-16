// Use NeoMatrix API, even if it may use the SmartMatrix backend depending on the CPU

#define FS_PREFIX ""

#define SD_CS // enter SD card CS pin here

//#define BOARD_HAS_PSRAM  //uncomment if board has PSRAM

#define MATRIX_GAMMA_CORRECTION 2.4 // higher number is darker, needed for Neomatrix more than SmartMatrix

#define MATRIX_PIN 16 // Gpio of the led matrix

#define MATRIX_BRIGHTNESS 10 //brightness of the led matrix. Possible values: 0-255

#define MATRIX_SIZE_16X16
#define GIF_SIZE 16

//#define MATRIX_SIZE_32X32_4_TILES
//#define GIF_SIZE 32

//#define PXLBLCK_32X32_FROM_4_16X16_TILES
//#define GIF_SIZE 32

// If the matrix is a different size than the GIFs, allow panning through the GIF
// while displaying it, or bouncing it around if it's smaller than the display
#define OFFSETX 0
#define OFFSETY 0
#define FACTX 10
#define FACTY 10

/* GifDecoder needs lzwMaxBits
   The lzwMaxBits value of 12 supports all GIFs, but uses 16kB RAM
   lzwMaxBits can be set to 10 or 11 for small displays, 12 for large displays
   All 32x32-pixel GIFs tested work with 11, most work with 10
*/

#ifdef ESP8266
const int lzwMaxBits = 11;
#else
const int lzwMaxBits = 12;
#endif

#include "GifAnim_Impl.h"






void matrix_setup(bool initserial = true, int reservemem = 40000, int matrix_gamma = MATRIX_GAMMA_CORRECTION)
{
  reservemem = reservemem; // squelch compiler warning if var is unused.

  if (init_done)
  {
    Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> BUG: matrix_setup called twice");
    return;
  }
  init_done = 1;

  // It's bad to call Serial.begin twice, if your calling script runs it first, make sure
  // you request that it isn't called a 2nd time here.
  if (initserial)
  {
    Serial.begin(115200);
    Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Serial.begin");
  }

  //show_free_mem("Memory after setup() starts");

  // ESP32 has more memory available for allocation in setup than in global
  // (if this were a global array), so we use malloc here.
  // https://forum.arduino.cc/index.php?topic=647833
  matrixleds = (CRGB *) mallocordie("matrixleds", sizeof(CRGB) * NUMMATRIX, 1);
  // and then fix the until now NULL pointer in the object.
  matrix->newLedsPtr(matrixleds);
  //show_free_mem("After matrixleds malloc");

  //============================================================================================
  // Matrix Init Start
  //============================================================================================
#if defined(MATRIX_SIZE_16X16)

  FastLED.addLeds<NEOPIXEL, MATRIXPIN>(matrixleds, NUMMATRIX).setCorrection(TypicalLEDStrip);
  Serial.print("Neomatrix total LEDs: ");
  Serial.print(NUMMATRIX);
  Serial.print(" running on pin: ");
  Serial.println(MATRIXPIN);

#elif defined(MATRIX_SIZE_24X24) //defined(MATRIX_SIZE_16X16)
  FastLED.addLeds<NEOPIXEL, MATRIXPIN>(matrixleds, NUMMATRIX).setCorrection(TypicalLEDStrip);
  Serial.print("Neomatrix total LEDs: ");
  Serial.print(NUMMATRIX);
  Serial.print(" running on pin: ");
  Serial.println(MATRIXPIN);

  //============================================================================================
  // Example of parallel output
#elif defined(MATRIX_SIZE_32X8_3_TIMES) //defined(MATRIX_SIZE_16X16)
  // Init Matrix
  // Serialized, 768 pixels takes 26 seconds for 1000 updates or 26ms per refresh
  // FastLED.addLeds<NEOPIXEL,MATRIXPIN>(matrixleds, NUMMATRIX).setCorrection(TypicalLEDStrip);
  // https://github.com/FastLED/FastLED/wiki/Parallel-Output
  // WS2811_PORTA - pins 12, 13, 14 and 15 or pins 6,7,5 and 8 on the NodeMCU
  // This is much faster 1000 updates in 10sec
  // See  https://github.com/FastLED/FastLED/wiki/Parallel-Output for pin definitions
#ifdef ESP8266
  FastLED.addLeds<WS2811_PORTA, 3>(matrixleds, NUMMATRIX / MATRIX_TILE_H).setCorrection(TypicalLEDStrip);
#else //ESP8266
  FastLED.addLeds<WS2811_PORTD, 3>(matrixleds, NUMMATRIX / MATRIX_TILE_H).setCorrection(TypicalLEDStrip);
#endif //ESP8266
  Serial.print("Neomatrix parallel output, total LEDs: ");
  Serial.println(NUMMATRIX);

  //============================================================================================
  // Serialized (slow-ish) output
#elif defined(MATRIX_SIZE_16X16_4_TILES) //defined(MATRIX_SIZE_16X16)
  FastLED.addLeds<NEOPIXEL, MATRIXPIN>(matrixleds, NUMMATRIX).setCorrection(TypicalLEDStrip);
  Serial.print("Neomatrix total LEDs: ");
  Serial.print(NUMMATRIX);
  Serial.print(" running on pin: ");
  Serial.println(MATRIXPIN);

  //============================================================================================
  // Serialized (slow-ish) output
#elif defined(MATRIX_SIZE_32X32_4_TILES) //defined(MATRIX_SIZE_16X16)
  FastLED.addLeds<NEOPIXEL, MATRIXPIN>(matrixleds, NUMMATRIX).setCorrection(TypicalLEDStrip);
  Serial.print("Neomatrix total LEDs: ");
  Serial.print(NUMMATRIX);
  Serial.print(" running on pin: ");
  Serial.println(MATRIXPIN);
  //============================================================================================
  // Serialized (slow-ish) output
#elif defined(PXLBLCK_32X32_FROM_4_16X16_TILES) //defined(MATRIX_SIZE_16X16)
  FastLED.addLeds<NEOPIXEL, MATRIXPIN>(matrixleds, NUMMATRIX).setCorrection(TypicalLEDStrip);
  Serial.print("Neomatrix total LEDs: ");
  Serial.print(NUMMATRIX);
  Serial.print(" running on pin: ");
  Serial.println(MATRIXPIN);

  //============================================================================================
#elif defined(MATRIX_SIZE_64X64)  //defined(MATRIX_SIZE_16X16)
// 64x64 straight connection (no matrices)
  // https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples
  FastLED.addLeds<WS2812B, 23, GRB>(matrixleds, 0 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
#ifdef ESP32
  FastLED.addLeds<WS2812B, 22, GRB>(matrixleds, 1 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 27, GRB>(matrixleds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP); // was 3
  FastLED.addLeds<WS2812B, 21, GRB>(matrixleds, 3 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 19, GRB>(matrixleds, 4 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 18, GRB>(matrixleds, 5 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 5, GRB>(matrixleds, 6 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 4, GRB>(matrixleds, 7 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);

  FastLED.addLeds<WS2812B, 0, GRB>(matrixleds, 8 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 2, GRB>(matrixleds, 9 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 15, GRB>(matrixleds, 10 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 25, GRB>(matrixleds, 11 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 26, GRB>(matrixleds, 12 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 14, GRB>(matrixleds, 13 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 12, GRB>(matrixleds, 14 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, 13, GRB>(matrixleds, 15 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);

  Serial.print("Neomatrix 16 pin via RMT/I2S 16 way parallel output, total LEDs: ");
  Serial.println(NUMMATRIX);
#endif // ESP32

  //int matrix_gamma = MATRIX_GAMMA_CORRECTION; // higher number is darker, needed for Neomatrix more than SmartMatrix

  //============================================================================================
#else //defined(MATRIX_SIZE_16X16)
#error "Undefined Matrix"
#endif //defined(MATRIX_SIZE_16X16)


  //============================================================================================
  // Matrix Init End
  //============================================================================================

  //show_free_mem("Before matrix->begin");
  matrix->begin();
  //show_free_mem("After matrix->begin");

  Serial.print("Setting Brightness: ");
  Serial.println(matrix_brightness);

  matrix->setBrightness(matrix_brightness);

  //#endif
  Serial.print("Gamma Correction: ");
  Serial.println(matrix_gamma);
  // Gamma is used by AnimatedGIFs and others, as such:
  // CRGB color = CRGB(matrix->gamma[red], matrix->gamma[green], matrix->gamma[blue]);
  matrix->precal_gamma(matrix_gamma);

#ifndef DISABLE_SPEED_TEST
  uint32_t before;
  Serial.println("vvvvvvvvvvvvvvvvvvvvvvvvvv Speed vvvvvvvvvvvvvvvvvvvvvvvvvv");

  before = millis();
  for (uint16_t i = 0; i < 5; i++) {
    matrix->fillScreen(0xFC00);
    matrix->show();

    matrix->fillScreen(0x003F);
    matrix->show();

  }
  Serial.print("Framebuffer::GFX end to end speed test: ");
  Serial.print(millisdiff(before));
  Serial.print("ms, fps: ");
  Serial.println(10 * 1000 / (millisdiff(before)));
  // Arduino::GFX ILI9314
  //                     tft/gfx/bypass/copy
  // 40fhz, fps no PSRAM: 25/15/22/14       PSRAM: 25/11/21/8
  // 80fhz, fps no PSRAM: 42/19/33/18       PSRAM: 40/14/32/9 (Arduino_HWSPI)
  // 80fhz, fps no PSRAM: 53/21/38/20 Arduino_ESP32SPI
  // 80fhz, fps no PSRAM: 60/20/34/18 Arduino_ESP32SPI_DMA
  //
  // Adafruit ILI9314 (much slower)
  // 80Mhz: TFT 40fps, NO PSRAM: 32fps, PSRAM show: 12fps
  //
  // Old Adafruit numbers:
  // ST7735_128b160: 80Mhz: TFT153fps, NO PSRAM:104fps, PSRAM show: 45fps
  // ST7735_128b160: 40Mhz: TFT 68fps, NO PSRAM: 56fps, PSRAM show: 32fps
  // ST7735_128b160: 20Mhz: TFT 53fps, NO PSRAM: 45fps, PSRAM show: 29fps
  // ST7735_128b128: 40Mhz: TFT117fps, NO PSRAM: 90fps, PSRAM show: 48fps
  //
  // Arduino::GFX is a lot faster (over 100fps)
  // SSD1331: SWSPI: TFT  9fps, NO PSRAM:  9fps, PSRAM show:  8fps
  Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^ Speed ^^^^^^^^^^^^^^^^^^^^^^^^^^");
#endif // DISABLE_SPEED_TEST
  matrix->fillScreen(0x0000);
  matrix->show();

  // At least on teensy, due to some framework bug it seems, early
  // serial output gets looped back into serial input
  // Hence, flush input.
  while (Serial.available() > 0) {
    char t = Serial.read();
    t = t;
  }
  Serial.println("matrix_setup done");
}




// Like XY, but for a mirror image from the top (used by misconfigured code)
int XY2( int x, int y, bool wrap = false)
{
  wrap = wrap; // squelch compiler warning
  return matrix->XY(x, MATRIX_HEIGHT - 1 - y);
}

// FastLED::colorutils needs a signature with uint8_t
uint16_t XY( uint8_t x, uint8_t y)
{
  return matrix->XY(x, y);
}
// but x/y can be bigger than 256
uint16_t XY16( uint16_t x, uint16_t y)
{
  return matrix->XY(x, y);
}

int wrapX(int x)
{
  if (x < 0 ) return 0;
  if (x >= MATRIX_WIDTH) return (MATRIX_WIDTH - 1);
  return x;
}

void show_free_mem(const char *pre = NULL)
{
  Framebuffer_GFX::show_free_mem(pre);
}

void die(const char *mesg)
{
  Serial.println(mesg);
  while (1) delay((uint32_t)1); // while 1 loop only triggers watchdog on ESP chips
}

void *mallocordie(const char *varname, uint32_t req, bool psram = true)
{
  // If varname starts with @, show debug for the allocation
  void *mem;

#ifndef BOARD_HAS_PSRAM
  psram = false;
#endif //BOARD_HAS_PSRAM

  if (varname[0] == '@')
  {
    if (psram) Serial.print("PS");
    Serial.print("Malloc ");
    Serial.print(varname);
    Serial.print(" . Requested bytes: ");
    Serial.println(req);
  }

#ifdef ESP32
  if (psram)
  {
    mem = ps_malloc(req);
  } else
  {
    mem = malloc(req);
  }
#else //ESP32
  mem = malloc(req);
#endif //ESP32

  if (mem) 
  {
    return mem;
  } else 
  {
    //show_free_mem();
    Serial.println("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv");
    Serial.print("FATAL: ");
    if (psram) Serial.print("ps_");
    Serial.print("malloc failed for ");
    Serial.print(varname);
    Serial.print(" . Requested bytes: ");
    Serial.println(req);
    Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
    while (1); // delay(1);  Adding this seems to cause an ESP32 bug
  }
  return NULL;
}

uint32_t millisdiff(uint32_t before)
{
  return ((millis() - before) ? (millis() - before) : 1);
}









// Setup method runs once, when the sketch starts
void setup()
{

  const char *pathname = "/gifs16/corkscrew.gif";
  //const char *pathname = "/gifs32/corkscrew.gif";

  decoder.setScreenClearCallback(screenClearCallback);
  decoder.setUpdateScreenCallback(updateScreenCallback);
  decoder.setDrawPixelCallback(drawPixelCallback);

  decoder.setFileSeekCallback(fileSeekCallback);
  decoder.setFilePositionCallback(filePositionCallback);
  decoder.setFileReadCallback(fileReadCallback);
  decoder.setFileReadBlockCallback(fileReadBlockCallback);

  // when including in other code, allow disabling all those inits
  // to avoid double inits.
#ifndef GIFANIM_INCLUDE

  matrix_setup();

#endif // GIFANIM_INCLUDE

#if !defined(FSOSD)

  // SPIFFS Begin (can crash/conflict with IRRemote on ESP32)
#ifdef FSOFAT //!defined(FSOSD)

  // Limit Fat support to a single concurrent file to save RAM
  // 37248 KB are saved by using limiting to 2 file instead of 10

#ifdef GIFANIM_INCLUDE

  // 1 file for reading files, and one file for the web server
  if (!FFat.begin(0, "", 2)) die("Fat FS mount failed. Not enough RAM?");

#else //GIFANIM_INCLUDE

  // You might need 2 or 3.
  if (!FFat.begin(0, "", 2)) die("Fat FS mount failed. Not enough RAM?");

#endif //GIFANIM_INCLUDE

  Serial.println("FatFS Directory listing:");
#else //!defined(FSOSD)

  if (!FSO.begin()) die("FS mount failed");
  Serial.println("Directory listing:");

#endif //!defined(FSOSD)

  // ESP8266 SPIFFS uses special directory objects
#ifdef ESP8266

  Dir dir = SPIFFS.openDir("/");
  while (dir.next())
  {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
  }

#else // ESP8266

#if defined(FSOFAT)

  Serial.printf("Total space: %10u\n", FFat.totalBytes());
  Serial.printf("Free space:  %10u\n", FFat.freeBytes());
  File dir = FFat.open(GIF_DIRECTORY);
  if (!dir) die("Can't open  " GIF_DIRECTORY);
  if (!dir.isDirectory()) die( GIF_DIRECTORY ": not a directory");

#else //defined(FSOFAT)

  File dir = FSO.open("/");

#endif //defined(FSOFAT)

  while (File file = dir.openNextFile())
  {
    Serial.print("FS File: ");
    Serial.print(file.name());
    Serial.print(" Size: ");
    Serial.println(file.size());
    close(file);
  }
  close(dir);

#endif // ESP8266
  Serial.println();


#else // !defined(FSOSD)


  if (initSdCard(SD_CS) < 0)
  {

#if ENABLE_SCROLLING == 1
    scrollingLayer.start("No SD card", -1);
#endif //ENABLE_SCROLLING == 1
    die("No SD card");
  }

#endif // !defined(FSOSD)

  Serial.print("GifAnim Viewer enabled, lzwMaxBits: ");
  Serial.print(lzwMaxBits);
  Serial.print(", GIF_SIZE: ");
  Serial.println(GIF_SIZE);

  if (sav_newgif(pathname))
    delay(100000); // while 1 loop only triggers watchdog on ESP chips
}

void loop()
{
  sav_loop();
}
