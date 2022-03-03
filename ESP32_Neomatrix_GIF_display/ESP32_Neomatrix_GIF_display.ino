#ifndef ARDUINOONPC
#define BASICARDUINOFS
#endif

// Use NeoMatrix API, even if it may use the SmartMatrix backend depending on the CPU

#define NEOMATRIX
#define MATRIX_PIN 0
#define MATRIX_BRIGHTNESS 10

//#define MATRIX_SIZE_16X16
//#define GIF_SIZE 16

//#define MATRIX_SIZE_32X32_4_TILES
//#define GIF_SIZE 32

#define PXLBLCK_32X32_FROM_4_16X16_TILES
#define GIF_SIZE 32

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

    //const char *pathname = "/gifs16/corkscrew.gif";
    const char *pathname = "/gifs32/corkscrew.gif";

  sav_setup();

  if (sav_newgif(pathname))
    delay(100000); // while 1 loop only triggers watchdog on ESP chips
}

void loop()
{
  sav_loop();
}
