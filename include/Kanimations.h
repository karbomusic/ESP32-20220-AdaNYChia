/*+===================================================================
  File:      Kanimations.h

  Summary:   Simple FastLED pixel animation testing.

  Kary Wall 2022.
===================================================================+*/
#include <Arduino.h>
#include <FastLED.h>

#define FRAMES_PER_SECOND 100
#define COOLING 70 // default: 55
#define SPARKING 120
#define NUM_LEDS 300

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
    Bright,
    Animation,
    SolidColor,
    Off
};

// globals
CRGB leds[NUM_LEDS];
int gLeds[NUM_LEDS];
uint8_t g_animationValue = 0; // to inform loop which request was made (needs event).
uint8_t g_briteValue = 255;   // used to inform loop of new brightness value.
CHSV g_chsvColor(0, 0, 0);    // used to inform loop of new solid color.

// prototypes
int *getLtrTransform(int leds[], int rows, int cols);

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
void randomDots2(CRGB leds[])
{
    currentLEDNum = random(NUM_LEDS - 1);
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
    fadeToBlackBy(leds, NUM_LEDS, 30);
}

int leds_done = 0;
void randomDots(CRGB leds[])
{
    //     currentLEDNum = random(NUM_LEDS - 1);
    //     sLED currentLED;
    //     currentLED.index = currentLEDNum;
    //     currentLED.H = random(255);
    //     currentLED.S = random(255);
    //     currentLED.V = random(32, 255);
    //     leds[currentLEDNum] = CHSV(currentLED.H, currentLED.S, currentLED.V);
    //     FastLED.show();
    //     leds[currentLED.index] = CHSV(0, 0, 0);
    //     FastLED.clear();

    EVERY_N_MILLIS(100)
    {
        CRGB Halloween_color = CRGB::Red;
        // Pick a random color - skew toward red/orange/yellow part of the spectrum for extra creepyness
        int m_red = random8(150, 255);
        int m_blue = 0;
        int m_green = random8(100);

        int r = map(m_red, 0, 255, 0, g_briteValue);
        int g = map(m_green, 0, 255, 0, g_briteValue);
        int b = map(m_blue, 0, 255, 0, g_briteValue);

        Halloween_color = CRGB(r, g, b);

        // shift pixels
        for (int i = NUM_LEDS - 1; i > 0; i--)
        {
            leds[i] = leds[i - 1];
        }

        // reset?
        EVERY_N_MILLIS_I(Dot_time, 500)
        {
            // This initally defaults to 20 seconds, but then will change the run
            // period to a new random number of seconds from 10 and 30 seconds.
            // You can name "timingObj" whatever you want.
            Dot_time.setPeriod(random16(10, 30));
            leds_done = 0;
        }

        if (leds_done < NUM_LEDS)
        {

            leds[0] = Halloween_color;
            leds_done = leds_done + 1;
            // i=i+1; if (i>=3){i=0;}
        }
        else
        {
            leds[0] = CRGB::Black;
        }

        FastLED.show(g_briteValue);
    }
}

void randomNoise(CRGB leds[])
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CHSV(random(255), random(120, 255), random(0, 255));
    }
    FastLED.show();
    FastLED.clear();
    return;
}

void randomPurpleJumper(CRGB leds[])
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CHSV(random(127, 250), random(140, 255), random(1, 130));
    }
    leds[random(NUM_LEDS)] = CRGB(255, 255, 255);
    FastLED.show();
    FastLED.clear();
    return;
}

void randomBlueJumper(CRGB leds[])
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CHSV(random(86, 172), random(140, 255), random(1, 130));
    }
    leds[random(NUM_LEDS)] = CRGB(255, 255, 255);
    FastLED.show();
    FastLED.clear();
    return;
}

void flashColor(CRGB leds[], int color)
{
    // forcing color random for now
    color = random(0, 255);
    EVERY_N_MILLISECONDS(200)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CHSV(color, 255, 255);
        }
        FastLED.show();
        delay(10);
        FastLED.clear();
        FastLED.show();
    }
}

void dotScrollRandomColor(CRGB leds[], int gTransform[])
{
    for (int i = 0; i < NUM_LEDS; i += 3)
    {
        if (i < NUM_LEDS) // cuz 3
        {
            leds[gTransform[i]] = CHSV(random(0, 255), 255, 255);
            leds[random(NUM_LEDS)] = CHSV(128, 150, 100);
            FastLED.show();
            delay(22);
            FastLED.clear();
        }
    }
    return;
}

void ltrDot(CRGB leds[], int gTransform[])
{
    static int ledIndex;
    static uint8_t randomColor;

    EVERY_N_MILLISECONDS(30)
    {
        leds[gTransform[ledIndex]] = CHSV(randomColor, 255, 255);
        FastLED.show();
        ledIndex += 3;
        if (ledIndex >= NUM_LEDS)
        {
            ledIndex = 0;
        }
    }

    EVERY_N_MILLISECONDS(2)
    {
        fadeToBlackBy(leds, NUM_LEDS, 10);
    }

    if (ledIndex == 0)
        randomColor = random(0, 255);
}

void Fire2012WithPalette(CRGB leds[])
{
    // Array of temperature readings at each simulation cell
    static uint8_t heat[int(NUM_LEDS)];

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++)
    {
        heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = NUM_LEDS - 1; k >= 2; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING)
    {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < NUM_LEDS; j++)
    {
        // Scale the heat value from 0-255 down to 0-240
        // for best results with color palettes.
        uint8_t colorindex = scale8(heat[j], 240);
        CRGB color = ColorFromPalette(gPal, colorindex);
        int pixelnumber;
        if (gReverseDirection)
        {
            pixelnumber = (NUM_LEDS - 1) - j;
        }
        else
        {
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

int *getLtrTransform(int leds[], int rows, int cols)
{
    // LED Strip
    if (rows == 1)
    {
        for (int i = 0; i < NUM_LEDS; i++)
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

    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (cCol < cols)
        {
            mappedVal = (!modVal ? mappedVal + bigHop : mappedVal + smallHop);
        }
        else
        {
            if (cRow == rows)
            {
                cRow = 0;
            }
            else
            {
                cRow += 1;
            }

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