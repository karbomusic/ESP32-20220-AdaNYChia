/*+===================================================================
  File:      Kanimations.h

  Summary:   Simple FastLED pixel animation testing.

  Kary Wall 2/20/22.
===================================================================+*/
//#define FASTLED_INTERRUPT_RETRY_COUNT 0 // required before #include
//#define FASTLED_ALLOW_INTERRUPTS 0

#include <Arduino.h>
#include <FastLED.h>

#define FRAMES_PER_SECOND 100
#define COOLING 70 // default: 55
#define SPARKING 120


// Structure for remebering a pixel's color.
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
int g_animationValue = -1;               // to inform loop which request was made (needs event).
uint8_t g_briteValue = 255;              // used to inform loop of new brightness value.
CHSV g_chsvColor(0, 0, 0);               // used to inform loop of new solid color.

// prototypes
int *getLtrTransform(int leds[], int rows, int cols);
void beatwave();
void inoise8_mover();

// locals
sLED previousLED;
int currentLEDNum = 0;
CRGBPalette16 gPal;
bool gReverseDirection = false;

// pallettes
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType currentBlending;

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
    leds[random(NUM_LEDS - 1)] = CRGB::Red;
    FastLED.show();
    leds[currentLED.index] = CHSV(0, 0, 0);
    fadeToBlackBy(leds, NUM_LEDS, 10);
}

int leds_done = 0;

void randomDots(CRGB leds[])
{
    EVERY_N_MILLIS(20)
    {
        CRGB Halloween_color;

        // shift pixels
        for (int i = NUM_LEDS - 1; i > 0; i--)
        {
            leds[i] = leds[i - 1];
        }

        for (int i = random(int(NUM_LEDS / 2)); i < random(int(NUM_LEDS / 2), NUM_LEDS); i++)
        {
            leds[i] = CHSV(random(128, 255), 255, random(0, 70));
        }
        FastLED.show();
        // reset?
        // EVERY_N_MILLIS_I(duration, 500)
        //  {
        // This initally defaults to 20 seconds, but then will change the run
        // period to a new random number of seconds from 10 and 30 seconds.
        // You can name "timingObj" whatever you want.
        // duration.setPeriod(random16(5, 10));
        //  leds_done = 0;
        //  }

        if (leds_done < NUM_LEDS)
        {
            Halloween_color = CRGB(random(20, 200), 0, random(255));
            leds[leds_done] = Halloween_color;
            leds_done = leds_done + 1;
        }
        else
        {
            leds[leds_done] = CRGB::Black;
            leds_done = 0;
        }

        EVERY_N_MILLISECONDS(random(100, 1000))
        {
            leds[random(NUM_LEDS - 1)] = CRGB::CornflowerBlue;
            FastLED.show();
        }

        EVERY_N_MILLISECONDS(random(223, 531))
        {
            leds[random(NUM_LEDS - 1)] = CRGB(random(255), random(255), random(255));
        }
    }
    leds[random(NUM_LEDS - 1)] = CRGB::Purple;
    fadeToBlackBy(leds, NUM_LEDS, 20);
    FastLED.show();
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

void redOcean(CRGB leds[])
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CHSV(random(232, 255), random(160, 255), random(1, 130));
    }
    leds[random(NUM_LEDS)] = CRGB(255, 255, 255);
    FastLED.show();
    FastLED.clear();
    return;
}

/*--------------------------------------------------------------------
   Moving Noise
---------------------------------------------------------------------*/

uint16_t xscale = 30;
uint16_t yscale = 30;
uint8_t maxChanges = 24;
static int16_t dist = random16(12345);

void inchWorm(CRGB leds[])
{
    static CRGBPalette16 cPalette = LavaColors_p;
    static CRGBPalette16 tPalette = OceanColors_p;

    EVERY_N_MILLISECONDS(10)
    {
        nblendPaletteTowardPalette(cPalette, tPalette, maxChanges); // AWESOME palette blending capability.
        inoise8_mover();                                            // Update the LED array with noise at the new location
        fadeToBlackBy(leds, NUM_LEDS, 4);
    }

    EVERY_N_SECONDS(5)
    { // Change the target palette to a random one every 5 seconds.
        tPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
    }

    EVERY_N_SECONDS(random(10, 35))
    {
        for (int i = 0; i < random(10, 30); i++)
        {
            leds[random(NUM_LEDS)] = CHSV(0, 0, 255);
            FastLED.delay(50);
        }
    }

    FastLED.show();
}

/*--------------------------------------------------------------------
   Color Strobe
---------------------------------------------------------------------*/

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

/*--------------------------------------------------------------------
   Start Twinkle (unhack me please)
---------------------------------------------------------------------*/

void starTwinkle(CRGB leds[])
{
    EVERY_N_MILLISECONDS(11)
    {
        static uint8_t position;
        static uint8_t direction;
        int rand = random(NUM_LEDS);
        leds[rand] = CHSV(0, 0, 255);

        if (rand % 3 == 0)
        {
            leds[rand] = CRGB::DarkOrange;
        }
        else if (rand == position)
        {
            leds[rand] = CRGB::Red;
        }

        leds[position] = CHSV(166, 255, 200);

        if (position == 0 && direction == 1)
        {
            direction = 0;
        }
        else if (position == NUM_LEDS - 1 && direction == 0)
        {
            direction = 1;
        }

        if (direction == 0)
        {
            position++;
        }
        else if (direction == 1)
        {
            position--;
        }

        EVERY_N_SECONDS(10)
        {
            leds[random(NUM_LEDS)] = CRGB::Red;
        }

        EVERY_N_MILLISECONDS_I(duration, 1000)
        {
            direction = !direction;
            duration.setPeriod(random16(100, 3000));
        }

        fadeToBlackBy(leds, NUM_LEDS, 8);
        FastLED.show();
    }
}
/*--------------------------------------------------------------------
   BeatWaver
---------------------------------------------------------------------*/
void beatWaver(CRGB leds[])
{
    currentBlending = LINEARBLEND;
    beatwave();

    EVERY_N_MILLISECONDS(100)
    {
        uint8_t maxChanges = 24;
        nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges); // AWESOME palette blending capability.
    }

    EVERY_N_SECONDS(5)
    { // Change the target palette to a random one every 5 seconds.
        targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
    }

    FastLED.show();
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

void beatwave()
{

    uint8_t wave1 = beatsin8(9, 0, 255); // That's the same as beatsin8(9);
    uint8_t wave2 = beatsin8(8, 0, 255);
    uint8_t wave3 = beatsin8(7, 0, 255);
    uint8_t wave4 = beatsin8(6, 0, 255);

    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = ColorFromPalette(currentPalette, i + wave1 + wave2 + wave3 + wave4, 255, currentBlending);
    }
}

void inoise8_mover()
{
    uint8_t locn = inoise8(xscale, dist + yscale) % 255;                       // Get a new pixel location from moving noise.
    uint8_t pixlen = map(locn, 0, 255, 0, NUM_LEDS);                           // Map that to the length of the strand.
    leds[pixlen] = ColorFromPalette(currentPalette, pixlen, 255, LINEARBLEND); // Use that value for both the location as well as the palette index colour for the pixel.
    dist += beatsin8(10, 1, 4);                                                // Moving along the distance (that random number we started out with). Vary it a bit with a sine wave.
}