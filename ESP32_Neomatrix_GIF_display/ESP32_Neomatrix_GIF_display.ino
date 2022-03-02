#ifndef ARDUINOONPC
#define BASICARDUINOFS
#endif

// Use NeoMatrix API, even if it may use the SmartMatrix backend depending on the CPU
#define NEOMATRIX
#define MATRIX_SIZE_16X16
#define gif_size 16
#define MATRIX_PIN 0
#define MATRIX_BRIGHTNESS 64

#include "GifAnim_Impl.h"

// If the matrix is a different size than the GIFs, allow panning through the GIF
// while displaying it, or bouncing it around if it's smaller than the display
int OFFSETX = 0;
int OFFSETY = 1;
int FACTX = 10;
int FACTY = 10;

// Setup method runs once, when the sketch starts
void setup()
{
#if defined(ARDUINOONPC)

  const char *pathname = FS_PREFIX "/gifs128x192/abstract_colorful.gif";

#elif defined(ESP8266)

  // 32x32 GIFs on 24x32 display, hence offset of -4
  OFFSETX = -4;
  const char *pathname = "/gifs/concentric_circles.gif";

#else

  const char *pathname = "/gifs/snake_16x16.gif";

#endif

  sav_setup();

  if (sav_newgif(pathname))
    delay(100000); // while 1 loop only triggers watchdog on ESP chips
}

void loop()
{
  sav_loop();
}
