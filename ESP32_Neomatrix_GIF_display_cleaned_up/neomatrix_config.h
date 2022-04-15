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
//uint32_t tft_spi_speed;
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


CRGB *matrixleds;

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

//CRGB *matrixleds;

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

//CRGB *matrixleds;

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

//CRGB *matrixleds;

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

//CRGB *matrixleds;

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

//CRGB *matrixleds;

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

//CRGB *matrixleds;

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds,
    MATRIX_TILE_WIDTH,
    MATRIX_TILE_HEIGHT,
    MATRIX_TILE_H,
    MATRIX_TILE_V,
    NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_TILE_ROWS + NEO_TILE_PROGRESSIVE + NEO_TILE_BOTTOM + NEO_TILE_LEFT + NEO_TILE_PROGRESSIVE);

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


//CRGB *matrixleds;

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
//const uint16_t mw = MATRIX_WIDTH;
//const uint16_t mh = MATRIX_HEIGHT;
//#endif

// Used by some demos
//const uint32_t NUMMATRIX = mw * mh; MATRIX_WIDTH*MATRIX_HEIGHT
//const uint32_t NUM_LEDS = NUMMATRIX;

// Compat with SmartMatrix code that uses those variables
// (but don't redefine for SmartMatrix backend)
//#ifndef SMARTMATRIX
//const uint16_t kMatrixWidth = mw;
//const uint16_t kMatrixHeight = mh;
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

//uint8_t gHue = 0; // rotating "base color" used by many of the patterns
//uint16_t speed = 255;






















#endif // neomatrix_config_h
