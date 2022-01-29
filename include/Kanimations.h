


/*+===================================================================
  File:      Kanimations.h

  Summary:   Simple FastLED pixel animation testing.

  Kary Wall 2022.
===================================================================+*/
#include <Arduino.h>
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// Structure for remebering a pixel's color.
// Had to name to sLED due to some conflict
struct sLED
{
    uint8_t index;
    uint8_t H;
    uint8_t S;
    uint8_t V;
};

sLED previousLED;
int currentLEDNum = 0;
const int ANALONG_PIN = 34;
int LED_COUNT;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
void clearLeds()
{
    FastLED.clear(true);
}

// prototypes
int *getLtrTransform(int leds[], int rows = 0, int cols = 0);
void rainbowWithGlitter();
void addGlitter(CRGB leds[], fract8 chanceOfGlitter);
void confetti(CRGB leds[]);
void sinelon(CRGB leds[]);
void bpm(CRGB leds[]);
void juggle(CRGB leds[]);

void setNumLeds(int numLeds){
    LED_COUNT = numLeds;
}

void randomDots2(CRGB leds[])
{
    currentLEDNum = random(LED_COUNT);
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
    FastLED.clear();
    return;
}

void randomDots(CRGB leds[])
{
    currentLEDNum = random(LED_COUNT);
    sLED currentLED;
    currentLED.index = currentLEDNum;
    currentLED.H = random(255);
    currentLED.S = random(255);
    currentLED.V = random(32, 255);
    leds[currentLEDNum] = CHSV(currentLED.H, currentLED.S, currentLED.V);
    FastLED.show();
    leds[currentLED.index] = CHSV(0, 0, 0);
}

void randomNoise(CRGB leds[])
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        leds[i] = CHSV(random(255), random(100, 255), random(1, 64));
    }
    FastLED.show();
    FastLED.clear();
    return;
}

void randomPurpleJumper(CRGB leds[])
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        leds[i] = CHSV(random(127, 250), random(100, 255), random(1, 64));
    }
    leds[random(LED_COUNT)] = CRGB(255, 255, 255);
    FastLED.show();
    FastLED.clear();
    return;
}

void randomBlueJumper(CRGB leds[])
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        leds[i] = CHSV(random(86, 172), random(100, 255), random(1, 64));
    }
    leds[random(LED_COUNT)] = CRGB(255, 255, 255);
    FastLED.show();
    FastLED.clear();
    return;
}

void dotScrollRandomColor(CRGB leds[])
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        leds[i] = CHSV(random(0, 255), 255, 150);
        leds[random(LED_COUNT)] = CHSV(128, 150, 100);
        FastLED.show();
        delay(22);
        FastLED.clear();
    }
    return;
}

void flashColor(CRGB leds[], int color)
{
    // forcing color random for now
    color = random(0, 255);
    EVERY_N_MILLISECONDS(200)
    {
        for (int i = 0; i < LED_COUNT; i++)
        {
            leds[i] = CHSV(color, 255, 255);
        }
        FastLED.show();
    }
    FastLED.clear();
    FastLED.show();
    return;
}

void ltrDot(CRGB leds[], int gTransform[])
{
    static int ledIndex;
    static uint8_t randomColor;

    EVERY_N_MILLISECONDS(30)
    {

        leds[gTransform[ledIndex]] = CHSV(randomColor, 255, 200);
        FastLED.show();
        ledIndex++;
        if (ledIndex == LED_COUNT)
        {
            ledIndex = 0;
        }
    }

    EVERY_N_MILLISECONDS(2)
    {
        fadeToBlackBy(leds, LED_COUNT, 30);
    }

    if (ledIndex == 0)
        randomColor = random(0, 255);
}

void modMation(CRGB leds[])
{
    EVERY_N_MILLISECONDS(100) // <-- not really needed.
    {
        for (int i = 0; i < LED_COUNT; i += 8)
        {
            leds[i] = CHSV(50, 255, 200);
            FastLED.show();
            delay(50);
            FastLED.clear();
        }

        for (int i = LED_COUNT; i > 0; i -= 6)
        {
            leds[i] = CHSV(130, 255, 200);
            FastLED.show();
            delay(50);
            FastLED.clear();
        }

        for (int i = 0; i < LED_COUNT; i += 4)
        {
            leds[i] = CHSV(180, 255, 200);
            FastLED.show();
            delay(50);
            FastLED.clear();
        }

        for (int i = LED_COUNT; i > 0; i -= 3)
        {
            leds[i] = CHSV(200, 255, 200);
            FastLED.show();
            delay(50);
            FastLED.clear();
        }

        for (int i = 0; i < LED_COUNT; i += 2)
        {
            leds[i] = CHSV(240, 255, 200);
            FastLED.show();
            delay(50);
            fadeToBlackBy(leds, LED_COUNT, 50);
        }

        for (int i = 0; i < LED_COUNT; i++)
        {
            leds[i] = CHSV(0, 255, 200);
            FastLED.show();
            delay(50);
            fadeToBlackBy(leds, LED_COUNT, 70);
        }
        FastLED.clear();
    }
}

// ---------------------------------------------------
//  FastLED Built-in Effects
// ---------------------------------------------------

void rainbow(CRGB leds[])
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, LED_COUNT, gHue, 7);
}

void rainbowWithGlitter(CRGB leds[])
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow(leds);
  addGlitter(leds, 80);
}

void addGlitter(CRGB leds[], fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(LED_COUNT) ] += CRGB::White;
  }
}

void confetti(CRGB leds[])
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, LED_COUNT, 10);
  int pos = random16(LED_COUNT);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon(CRGB leds[])
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, LED_COUNT, 20);
  int pos = beatsin16(13, 0, LED_COUNT);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm(CRGB leds[])
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < LED_COUNT; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle(CRGB leds[]) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, LED_COUNT, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, LED_COUNT)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

// ---------------------------------------------------
// Utility functions
// ---------------------------------------------------

// LTR Transform - Returns an int *array() of pixel locations mapped left to right
int *getLtrTransform(int leds[], int rows, int cols)
{

    // If rows and cols are not set then this is a strip, not a matrix.
    // So, populate the array with the default 1...255 and return.
    if (rows == 0 || cols == 0)
    {
        for (int i = 0; i < LED_COUNT; i++)
        {
            leds[i] = i;
        }
        return leds;
    }

    bool modVal = true;
    int bigHop = (rows * 2) - 1;
    int smallHop = 1;
    int cCol = 0;
    int cRow = 0;
    int mappedVal = -1;

    for (int i = 0; i < LED_COUNT; i++)
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
