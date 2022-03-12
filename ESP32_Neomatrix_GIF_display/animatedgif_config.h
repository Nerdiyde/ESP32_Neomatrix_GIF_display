#ifndef animatedgif_config
#define animatedgif_config

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

//#define NEOMATRIX // Switch to NEOMATRIX backend from native SMARTMATRIX backend
//#define NEOPIXEL_MATRIX  // If NEOMATRIX, use FastLED::NeoMatrix, not SmartMatrix_GFX

// if you want to display a file and display that one first
#define FIRSTINDEX 0
//#define DEBUGLINE 16

// Use Neomatrix API (which in turn could be using SmartMatrix driver)?
// This is defined in main ino that calls sav_loop

// This doesn't work due to variables being redefined. Sigh...
// instead it's included once from AnimatedGIFs.ino
#include "neomatrix_config.h"

// If the matrix is a different size than the GIFs, set the offset for the upper left corner
// (negative or positive is ok).
extern int OFFSETX;
extern int OFFSETY;
extern int FACTX;
extern int FACTY;

#endif
