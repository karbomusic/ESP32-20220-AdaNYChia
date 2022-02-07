/*+===================================================================
  File:      Kanimations.h

  Summary:   Simple FastLED pixel animation testing.

  Kary Wall 2022.
===================================================================+*/
#include <Arduino.h>
#include <FastLED.h>

#define FRAMES_PER_SECOND 60
#define COOLING  70 // default: 55
#define SPARKING 120
#define NUM_LEDS 120

// Structure for remebering a pixel's color.
// Had to name to sLED due to some conflict
struct sLED
{
    uint8_t index;
    uint8_t H;
    uint8_t S;
    uint8_t V;
};

enum Mode
{
    Bright, Animation, SolidColor, Off
};

// prototypes
int *getLtrTransform(int leds[], int ledNum, int rows, int cols);

sLED previousLED;
int currentLEDNum = 0;
CRGBPalette16 gPal;
bool gReverseDirection = false;

void clearLeds()
{
    FastLED.clear(true);
}

/*--------------------------------------------------------------------
                         FASTLED ANIMATIONS
---------------------------------------------------------------------*/
void randomDots2(CRGB leds[], int ledNum)
{
    currentLEDNum = random(ledNum - 1);
    sLED currentLED;
    currentLED.index = currentLEDNum;
    currentLED.H = random(255);
    currentLED.S = random(255);
    currentLED.V = 120;
    leds[currentLED.index] = CRGB(currentLED.H, currentLED.S, currentLED.V);
    FastLED.show();
    delay(20);
    leds[currentLEDNum] = CRGB::CornflowerBlue;
    FastLED.show();
    leds[currentLED.index] = CHSV(0, 0, 0);
    fadeToBlackBy(leds, ledNum, 30);
}

void randomDots(CRGB leds[], int ledNum)
{
    currentLEDNum = random(ledNum - 1);
    sLED currentLED;
    currentLED.index = currentLEDNum;
    currentLED.H = random(255);
    currentLED.S = random(255);
    currentLED.V = random(32, 255);
    leds[currentLEDNum] = CHSV(currentLED.H, currentLED.S, currentLED.V);
    FastLED.show();
    leds[currentLED.index] = CHSV(0, 0, 0);
    FastLED.clear();
}

void randomNoise(CRGB leds[], int ledNum)
{
    for (int i = 0; i < ledNum; i++)
    {
        leds[i] = CHSV(random(255), random(120, 255), random(0, 255));
    }
    FastLED.show();
    FastLED.clear();
    return;
}

void randomPurpleJumper(CRGB leds[], int ledNum)
{
    for (int i = 0; i < ledNum; i++)
    {
        leds[i] = CHSV(random(127, 250), random(140, 255), random(1, 130));
    }
    leds[random(ledNum)] = CRGB(255, 255, 255);
    FastLED.show();
    FastLED.clear();
    return;
}

void randomBlueJumper(CRGB leds[], int ledNum)
{
    for (int i = 0; i < ledNum; i++)
    {
        leds[i] = CHSV(random(86, 172), random(140, 255), random(1, 130));
    }
    leds[random(ledNum)] = CRGB(255, 255, 255);
    FastLED.show();
    FastLED.clear();
    return;
}

void flashColor(CRGB leds[], int ledNum, int color)
{
    // forcing color random for now
    color = random(0, 255);
    EVERY_N_MILLISECONDS(200)
    {
        for (int i = 0; i < ledNum; i++)
        {
            leds[i] = CHSV(color, 255, 255);
        }
        FastLED.show();
        delay(10);
        FastLED.clear();
        FastLED.show();
    }
}

void dotScrollRandomColor(CRGB leds[], int gTransform[], int ledNum)
{
    for (int i = 0; i < ledNum; i += 3)
    {
        if (i < ledNum) // cuz 3
        {
            leds[gTransform[i]] = CHSV(random(0, 255), 255, 255);
            leds[random(ledNum)] = CHSV(128, 150, 100);
            FastLED.show();
            delay(22);
            FastLED.clear();
        }
    }
    return;
}

void ltrDot(CRGB leds[], int gTransform[], int ledNum)
{
    static int ledIndex;
    static uint8_t randomColor;

    EVERY_N_MILLISECONDS(30)
    {
        leds[gTransform[ledIndex]] = CHSV(randomColor, 255, 255);
        FastLED.show();
        ledIndex += 3;
        if (ledIndex >= ledNum)
        {
            ledIndex = 0;
        }
    }

    EVERY_N_MILLISECONDS(2)
    {
        fadeToBlackBy(leds, ledNum, 30);
    }

    if (ledIndex == 0)
        randomColor = random(0, 255);
}

void Fire2012WithPalette(CRGB leds[])
{
// Array of temperature readings at each simulation cell
// kw: Leaving some room at top for the darkness (20%) - this is wrong.
  static uint8_t heat[int(NUM_LEDS*0.7)];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      uint8_t colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }

    FastLED.delay(1000 / FRAMES_PER_SECOND);
}
/*--------------------------------------------------------------------
                         Utility functions
---------------------------------------------------------------------*/

/*--------------------------------------------------------------------
    Transpose Pixels - Some layouts are psuedo column-major order:

    0 15 16 31..................................................255
    1 14 17 30..................................................254
    2 13 18 29..................................................253
    3 12 19 28..................................................252
    4 11 20 27..................................................251
    5 10 21 26..................................................250
    6 09 22 25..................................................249
    7 08 23 24..................................................248

    This function transposes such an int *array() of pixels to
    a row-major order. Pass rows=1 if this is a strip instead 
    of a matrix. 
---------------------------------------------------------------------*/

int *getLtrTransform(int leds[], int ledNum, int rows, int cols)
{
    // LED Strip
    if (rows == 1) 
    {
        for (int i = 0; i < ledNum; i++)
        {
            leds[i] = i;
        }
        return leds;
    }

    // LED Matrix
    bool modVal = true;
    int bigHop = (rows * 2) - 1;
    int smallHop = 1;
    int cCol = 0;
    int cRow = 0;
    int mappedVal = -1;

    for (int i = 0; i < ledNum; i++)
    {
        if (cCol < cols)
        {
            mappedVal = (!modVal ? mappedVal + bigHop : mappedVal + smallHop);
        }
        else
        {
            cRow = cRow == rows ? 0 : cRow += 1;
            cCol = 0;
            mappedVal = cRow + cCol;
            bigHop -= 2;
            smallHop += 2;
        }
        modVal = !modVal;
        cCol++;
        leds[i] = mappedVal;
    }
    return leds;
}