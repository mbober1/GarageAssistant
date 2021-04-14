#include <Adafruit_NeoPixel.h>
#define PIN_RGB 27
#define NUMPIXELS 30


Adafruit_NeoPixel pixels(NUMPIXELS, PIN_RGB, NEO_GRB + NEO_KHZ800);
void init_RGB()
{
   pixels.begin();
}

void show_RGB(int distance)
{
   if(distance < 0) return;

   if(distance >= 200)
   {
      for(int i=0; i<NUMPIXELS; i++)
      {
         pixels.setPixelColor(i, pixels.Color(0, 255/2, 0));
         pixels.show();
      }
   }
   else if(distance <= 2)
   {
      for(int i=0; i<NUMPIXELS; i++)
      {
         pixels.setPixelColor(i, pixels.Color(255/2, 0, 0));
         pixels.show();
      }
   }
   else
   {
      int k = (int) distance*1.25;
      for(int i=0; i<NUMPIXELS; i++)
      {
         pixels.setPixelColor(i, pixels.Color((255-k)/2, k/2, 0));
         pixels.show();
      }
   }
}