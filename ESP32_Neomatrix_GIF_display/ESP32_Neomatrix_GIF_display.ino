/*
  #ifndef ARDUINOONPC
  #define BASICARDUINOFS
  #endif
*/

// Use NeoMatrix API, even if it may use the SmartMatrix backend depending on the CPU

#define FS_PREFIX ""

#define SD_CS // enter SD card CS pin here

//#define BOARD_HAS_PSRAM  //uncomment if board has PSRAM
#define NEOMATRIX
#define MATRIX_PIN 16
#define MATRIX_BRIGHTNESS 10

#define MATRIX_SIZE_16X16
#define GIF_SIZE 16

//#define MATRIX_SIZE_32X32_4_TILES
//#define GIF_SIZE 32

//#define PXLBLCK_32X32_FROM_4_16X16_TILES
//#define GIF_SIZE 32

#include "GifAnim_Impl.h"

// If the matrix is a different size than the GIFs, allow panning through the GIF
// while displaying it, or bouncing it around if it's smaller than the display
int OFFSETX = 0;
int OFFSETY = 0;
int FACTX = 10;
int FACTY = 10;

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

  //#ifdef NEOMATRIX
  matrix_setup();
  /*
    #else
    // neomatrix_config takes care of starting Serial, but we don't call it in the SMARTMATRIX code path
    Serial.begin(115200);
    Serial.println("Running on Native SmartMatrix Backend");
    // Initialize matrix
    matrix.addLayer(&backgroundLayer);
    matrix.setBrightness(defaultBrightness);

    // for large panels, may want to set the refresh rate lower to leave more CPU time to decoding GIFs (needed if GIFs are playing back slowly)
    //matrix.setRefreshRate(90);
    #if defined(ESP32)
    // for large panels on ESP32, may want to set the max percentage time dedicated to updating the refresh frames lower, to leave more CPU time to decoding GIFs (needed if GIFs are playing back slowly)
    //matrix.setMaxCalculationCpuPercentage(50);

    // alternatively, for large panels on ESP32, may want to set the calculation refresh rate divider lower to leave more CPU time to decoding GIFs (needed if GIFs are playing back slowly) - this has the same effect as matrix.setMaxCalculationCpuPercentage() but is set with a different parameter
    //matrix.setCalcRefreshRateDivider(4);
    #if defined(SPI_FFS)
    matrix.begin();
    #else //defined(SPI_FFS)
    // The ESP32 SD Card library is going to want to malloc about 28000 bytes of DMA-capable RAM
    // make sure at least that much is left free
    matrix.begin(28000);
    #endif //defined(SPI_FFS)


    #else // defined(ESP32)
    matrix.begin();
    #endif // defined(ESP32)

    #endif // NEOMATRIX
  */
#endif // GIFANIM_INCLUDE


  //#ifdef ARDUINOONPC

  //#elif !defined(FSOSD)
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
