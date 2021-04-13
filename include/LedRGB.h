#include <Adafruit_NeoPixel.h>
#define PIN_RGB 15
#define NUMPIXELS 30


Adafruit_NeoPixel pixels(NUMPIXELS, PIN_RGB, NEO_GRB + NEO_KHZ800);
void init_RGB
{
   pixels.begin();
}

void show_RGB(int distance)
{
   int k = (int) distance*1.25;
   for(int i=0; i<NUMPIXELS; i++)
   {
      pixels.setPixelColor(i, pixels.Color(255-k, k, 0));
      pixels.show();
   }
   \\delay(50);
}