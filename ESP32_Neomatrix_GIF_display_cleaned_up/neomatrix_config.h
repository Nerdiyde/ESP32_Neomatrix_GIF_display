#ifndef neomatrix_config_h
#define neomatrix_config_h

/*
  Yes, this file is a bit complicated looking, that's because it supports lots
  of different hardware backends (listed below). A single define will let you
  switch from one backend to another one without changing your code.
  If you'd like to see this file (likely out of date somewhat) with a bunch of
  defines removed, so it looks more flat, see neomatrix_config_tftonly.h
  But this big file is the one I'll maintain instead of maintaining 5 or more
  separate files, one per backend, when most of the code/init can be shared
  between them.

  All backends end up using the same Framebuffer::GFX as the base class
  but SmartMatrix is sufficiently different to need its own exceptions and handling
  (for one, with SmartMatrix, the buffer is actually handled by SmartMatrix whereas
  the other libraries define their own FastLED CRGB buffer (RGB888) ).

  Backends you should choose from (define 1):
  - SMARTMATRIX (if you are using the old SMARTMATRIX3, also define SMARTMATRIXV3)
  - ILI9341
  - ST7735_128b160
  - ST7735_128b128
  - SSD1331 (96x64 TFT)
  - Everything below is NeoMatrix in different patterns:
  MATRIX_SIZE_24X24 MATRIX_SIZE_32X8_3_TIMES MATRIX_SIZE_16X16_4_TILES MATRIX_SIZE_64X64 are 4 examples of NEOMATRIX defines
  (3 tiled 32x8, 4 tiled 16x16, and a single zigzag 64x64 array)
  - ARDUINOONPC is auto defined by https://github.com/marcmerlin/ArduinoOnPc-FastLED-GFX-LEDMatrix
  - On ARM, we assume rPi and define RPIRGBPANEL
  - Elsewhere, we assume rendering on linux/X11
    - LINUX_RENDERER_X11 is the default with ArduinoOnPc-FastLED-GFX-LEDMatrix
    - LINUX_RENDERER_SDL can be defined in ArduinoOnPc-FastLED-GFX-LEDMatrix's Makefile

  // For TFTs, there is original support from adafruit, but https://github.com/moononournation/Arduino_GFX/
  // has better and faster support for many TFTs (ILI9341, ST7735*, SSD1331, etc...)
  // define ADAFRUIT_TFT if you'd rather have the Adafruit drivers. I don't recommend them anymore.

  LEDMATRIX is a separate define you'd set before including this file and
  adds the LEDMatrix API if you need it.
  The TL;DR is you shouldn't bother with it if you already have the GFX
  and FastLED APIs, unless you can use fancy wavy scrolling colored fonts or the
  sprite support in LEDMatrix.

  You should not need to modify this file at all unless you are adding new matrix
  definitions and/or changing pin mappings for TFT screens. To choose which backend
  to use, set the define before you include the file.
*/

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// README README README README README README README README README README  README README README README
// If you have never used FastLED::NeoMatrix before, please try these 2 examples first
// https://github.com/marcmerlin/FastLED_NeoMatrix/tree/master/examples/matrixtest
// https://github.com/marcmerlin/FastLED_NeoMatrix/tree/master/examples/MatrixGFXDemo
// For FastLED just uncomment this line below (define MATRIX_SIZE_24X24) and fix the matrix definition
// or use one of the other ones if they are closer ot your setup (MATRIX_SIZE_32X8_3_TIMES MATRIX_SIZE_16X16_4_TILES MATRIX_SIZE_64X64.
// For SmartMatrix, just use "#define SMARTMATRIX"
//
// Are all those defines confusing? Then look at neomatrix_config_tftonly.h with all the defines
// taken out and a single backend hardcoded.
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//#define MATRIX_SIZE_24X24

// If you did not define something above, right here ^^^ the code below will look at the
// chip and do a hardcoded define that works for me, but is unlikely to be what you are also
// using, so really you want to define your driver above, or one will be picked for you and
// it'll probably be the wrong one :)

#include <Adafruit_GFX.h>
bool init_done = 0;
uint32_t tft_spi_speed;
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// min/max are complicated. Arduino and ESP32 layers try to be helpful by using
// templates that take specific kinds of arguments, but those do not always work
// with mixed types:
// error: no matching function for call to 'max(byte&, int16_t&)'
// These defines get around this problem.
#define mmin(a,b) ((a<b)?(a):(b))
#define mmax(a,b) ((a>b)?(a):(b))

// The ESP32 FastLED defines below must be defined before FastLED.h is loaded
// They are not relevant if you don't actually use FastLED pixel output but cause
// no harm if we only include FastLED for its CRGB struct.
#ifdef ESP32
  // Allow infrared for old FastLED versions
  #define FASTLED_ALLOW_INTERRUPTS 1
  // Newer Samguyver ESP32 FastLED has a new I2S implementation that can be
  // better (or worse) than then default RMT which only supports 8 channels.
  #define FASTLED_ESP32_I2S
  #pragma message "Please use https://github.com/samguyer/FastLED.git if stock FastLED is unstable with ESP32"
#endif //ESP32

#include <FastLED.h>

//============================================================================
// Ok, if you're doing matrices of displays, there is also a reasonable chance
// you'll be using SPIFFS or FATFS on flash, or an sdcard, so let's define it
// here (NeoMatrix-FastLED-IR actually also uses this to read a config file)
//============================================================================

// Note, you can use an sdcard on ESP32 or ESP8266 if you really want,
// but if your data fits in built in flash, why not use it?
// Use built in flash via SPIFFS/FATFS
// esp8266com/esp8266/libraries/SD/src/File.cpp
// ESP8266: http://esp8266.github.io/Arduino/versions/2.3.0/doc/filesystem.html#uploading-files-to-file-system
// ESP32: https://github.com/me-no-dev/arduino-esp32fs-plugin
// https://github.com/marcmerlin/esp32_fatfsimage/blob/master/README.md

#if defined(ESP8266)

  //#define FS_PREFIX ""
  #include <FS.h>
  #define FSO SPIFFS
  #define FSOSPIFFS

  #if GIF_SIZE == 64
  
    #define GIF_DIRECTORY FS_PREFIX "/gifs64/"
  #elif GIF_SIZE == 32 //GIF_SIZE == 64
  
    #define GIF_DIRECTORY FS_PREFIX "/gifs32/"
  #elif GIF_SIZE == 16 //GIF_SIZE == 64
  
    #define GIF_DIRECTORY FS_PREFIX "/gifs16/"
  #else //GIF_SIZE == 64
  
    #define GIF_DIRECTORY FS_PREFIX "/gif/"
  #endif //GIF_SIZE == 64


extern "C"
{
#include "user_interface.h"
}

#elif defined(ESP32) //defined(ESP8266)

//#define FS_PREFIX ""

  #if ESP32FATFS

    #include "FFat.h"
    #define FSO FFat
    #define FSOFAT

  #else //ESP32FATFS
  
    // LittleFS is more memory efficient than FatFS
    #include "FS.h"
    #include <LITTLEFS.h>
    #define FSO LITTLEFS
    #define FSOLITTLEFS
  
  #endif //ESP32FATFS


  #if GIF_SIZE == 64
    #define GIF_DIRECTORY FS_PREFIX "/gifs64"  
  #elif GIF_SIZE == 32 //GIF_SIZE == 64
    #define GIF_DIRECTORY FS_PREFIX "/gifs"  
  #else //GIF_SIZE == 64  
    #define GIF_DIRECTORY FS_PREFIX "/"
  #endif // GIF_SIZE == 64
#else //defined(ESP8266)

  //#define FS_PREFIX ""
  #define FSO SD
  #define FSOSD

  #if defined (ARDUINO)
    #include <SD.h>
  
  #endif //defined (ARDUINO)

#endif 


//============================================================================
// Matrix defines (SMARTMATRIX vs NEOMATRIX and size)
// You should #define one and only one of them and if you need to edit it,
// edit both the block below and the 2nd block in setup() at the bottom of this file
//============================================================================
//

//----------------------------------------------------------------------------
#if defined(MATRIX_SIZE_16X16)

#include <FastLED_NeoMatrix.h>
#define FASTLED_NEOMATRIX

const uint8_t MATRIXPIN = MATRIX_PIN;

uint8_t matrix_brightness = MATRIX_BRIGHTNESS;
// Used by LEDMatrix
const uint16_t MATRIX_TILE_WIDTH = 16; // width of EACH NEOPIXEL MATRIX (not total display)
const uint16_t MATRIX_TILE_HEIGHT = 16; // height of each matrix
const uint8_t MATRIX_TILE_H     = 1;  // number of matrices arranged horizontally
const uint8_t MATRIX_TILE_V     = 1;  // number of matrices arranged vertically

CRGB *matrixleds;

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT,
    NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT +
    NEO_TILE_ROWS + NEO_TILE_PROGRESSIVE );

//----------------------------------------------------------------------------
#elif defined(MATRIX_SIZE_24X24)

#include <FastLED_NeoMatrix.h>
#define FASTLED_NEOMATRIX

const uint8_t MATRIXPIN = MATRIX_PIN;

uint8_t matrix_brightness = MATRIX_BRIGHTNESS;
// Used by LEDMatrix
const uint16_t MATRIX_TILE_WIDTH = 24; // width of EACH NEOPIXEL MATRIX (not total display)
const uint16_t MATRIX_TILE_HEIGHT = 24; // height of each matrix
const uint8_t MATRIX_TILE_H     = 1;  // number of matrices arranged horizontally
const uint8_t MATRIX_TILE_V     = 1;  // number of matrices arranged vertically

CRGB *matrixleds;

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT,
    NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT +
    NEO_TILE_ROWS + NEO_TILE_PROGRESSIVE );

//----------------------------------------------------------------------------
#elif defined(MATRIX_SIZE_32X8_3_TIMES)

#include <FastLED_NeoMatrix.h>
#define FASTLED_NEOMATRIX

uint8_t matrix_brightness = MATRIX_BRIGHTNESS;
// Used by LEDMatrix
const uint16_t MATRIX_TILE_WIDTH = 8; // width of EACH NEOPIXEL MATRIX (not total display)
const uint16_t MATRIX_TILE_HEIGHT = 32; // height of each matrix
const uint8_t MATRIX_TILE_H     = 3;  // number of matrices arranged horizontally
const uint8_t MATRIX_TILE_V     = 1;  // number of matrices arranged vertically

CRGB *matrixleds;

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, MATRIX_TILE_H, MATRIX_TILE_V,
    NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
    NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG +
    NEO_TILE_TOP + NEO_TILE_LEFT +  NEO_TILE_PROGRESSIVE);

//----------------------------------------------------------------------------
#elif defined(MATRIX_SIZE_16X16_4_TILES)

#include <FastLED_NeoMatrix.h>
#define FASTLED_NEOMATRIX

uint8_t matrix_brightness = MATRIX_BRIGHTNESS;

const uint16_t MATRIX_TILE_WIDTH = 8; // width of EACH NEOPIXEL MATRIX (not total display)
const uint16_t MATRIX_TILE_HEIGHT = 8; // height of each matrix
const uint8_t MATRIX_TILE_H     = 2; // number of matrices arranged horizontally
const uint8_t MATRIX_TILE_V     = 2; // number of matrices arranged vertically

CRGB *matrixleds;

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, MATRIX_TILE_H, MATRIX_TILE_V,
    NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG +
    NEO_TILE_TOP + NEO_TILE_RIGHT +  NEO_TILE_PROGRESSIVE);

const uint8_t MATRIXPIN = MATRIX_PIN;

//----------------------------------------------------------------------------
#elif defined(MATRIX_SIZE_32X32_4_TILES)

#include <FastLED_NeoMatrix.h>
#define FASTLED_NEOMATRIX

uint8_t matrix_brightness = MATRIX_BRIGHTNESS;

const uint16_t MATRIX_TILE_WIDTH = 16; // width of EACH NEOPIXEL MATRIX (not total display)
const uint16_t MATRIX_TILE_HEIGHT = 16; // height of each matrix
const uint8_t MATRIX_TILE_H     = 2; // number of matrices arranged horizontally
const uint8_t MATRIX_TILE_V     = 2; // number of matrices arranged vertically

CRGB *matrixleds;

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, MATRIX_TILE_H, MATRIX_TILE_V,
    NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG +
    NEO_TILE_TOP + NEO_TILE_RIGHT +  NEO_TILE_PROGRESSIVE);

const uint8_t MATRIXPIN = MATRIX_PIN;

//----------------------------------------------------------------------------
#elif defined(PXLBLCK_32X32_FROM_4_16X16_TILES)

#include <FastLED_NeoMatrix.h>
#define FASTLED_NEOMATRIX

uint8_t matrix_brightness = MATRIX_BRIGHTNESS;

const uint16_t MATRIX_TILE_WIDTH = 16; // width of EACH NEOPIXEL MATRIX (not total display)
const uint16_t MATRIX_TILE_HEIGHT = 16; // height of each matrix
const uint8_t MATRIX_TILE_H     = 2; // number of matrices arranged horizontally
const uint8_t MATRIX_TILE_V     = 2; // number of matrices arranged vertically

CRGB *matrixleds;

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, MATRIX_TILE_H, MATRIX_TILE_V,
    NEO_MATRIX_BOTTOM  + NEO_MATRIX_LEFT +
    NEO_TILE_ROWS + NEO_TILE_PROGRESSIVE +
    NEO_TILE_BOTTOM + NEO_TILE_LEFT +  NEO_TILE_PROGRESSIVE);

const uint8_t MATRIXPIN = MATRIX_PIN;

//----------------------------------------------------------------------------
#elif defined(MATRIX_SIZE_64X64) // 64x64 straight connection (no matrices)

#include <FastLED_NeoMatrix.h>
#define FASTLED_NEOMATRIX

// http://marc.merlins.org/perso/arduino/post_2018-07-30_Building-a-64x64-Neopixel-Neomatrix-_4096-pixels_-running-NeoMatrix-FastLED-IR.html
uint8_t matrix_brightness = MATRIX_BRIGHTNESS;
//
// Used by LEDMatrix
const uint16_t MATRIX_TILE_WIDTH = 64; // width of EACH NEOPIXEL MATRIX (not total display)
const uint16_t MATRIX_TILE_HEIGHT = 64; // height of each matrix
const uint8_t MATRIX_TILE_H     = 1;  // number of matrices arranged horizontally
const uint8_t MATRIX_TILE_V     = 1;  // number of matrices arranged vertically

#define NUM_STRIPS 16
#define NUM_LEDS_PER_STRIP 256

CRGB *matrixleds;

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT,
    NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG );

//----------------------------------------------------------------------------

#else //defined(MATRIX_SIZE_16X16)

#error "Please write a matrix config or choose one of the definitions above. If you have a FastLED matrix, define MATRIX_SIZE_24X24 at the top of this file"

#endif //defined(MATRIX_SIZE_16X16)

//============================================================================
// End Matrix defines (SMARTMATRIX vs NEOMATRIX and size)
//============================================================================

// Compat for some other demos
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
const uint16_t MATRIX_WIDTH  = MATRIX_TILE_WIDTH *  MATRIX_TILE_H;
const uint16_t MATRIX_HEIGHT = MATRIX_TILE_HEIGHT * MATRIX_TILE_V;

// Used by NeoMatrix
const uint16_t mw = MATRIX_WIDTH;
const uint16_t mh = MATRIX_HEIGHT;
//#endif

// Used by some demos
const uint32_t NUMMATRIX = mw * mh;
const uint32_t NUM_LEDS = NUMMATRIX;

// Compat with SmartMatrix code that uses those variables
// (but don't redefine for SmartMatrix backend)
//#ifndef SMARTMATRIX
const uint16_t kMatrixWidth = mw;
const uint16_t kMatrixHeight = mh;
//#endif
#pragma GCC diagnostic pop

#ifdef ESP8266
// Turn off Wifi in setup()
// https://www.hackster.io/rayburne/esp8266-turn-off-wifi-reduce-current-big-time-1df8ae
//
#include "ESP8266WiFi.h"
extern "C"
{
#include "user_interface.h"
}
#endif // ESP8266

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint16_t speed = 255;

float matrix_gamma = 1; // higher number is darker, needed for Neomatrix more than SmartMatrix

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
    show_free_mem();
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



















void matrix_setup(bool initserial = true, int reservemem = 40000)
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

  show_free_mem("Memory after setup() starts");

  // ESP32 has more memory available for allocation in setup than in global
  // (if this were a global array), so we use malloc here.
  // https://forum.arduino.cc/index.php?topic=647833
  matrixleds = (CRGB *) mallocordie("matrixleds", sizeof(CRGB) * NUMMATRIX, 1);
  // and then fix the until now NULL pointer in the object.
  matrix->newLedsPtr(matrixleds);
  show_free_mem("After matrixleds malloc");

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

  matrix_gamma = 2.4; // higher number is darker, needed for Neomatrix more than SmartMatrix

  //============================================================================================
#else //defined(MATRIX_SIZE_16X16)
#error "Undefined Matrix"
#endif //defined(MATRIX_SIZE_16X16)


  //============================================================================================
  // Matrix Init End
  //============================================================================================

  show_free_mem("Before matrix->begin");
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

#endif // neomatrix_config_h
