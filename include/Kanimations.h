/*+===================================================================
  File:      Kanimations.h

  Summary:   Simple FastLED pixel animation testing.

  Kary Wall 2022.
===================================================================+*/
#include <Arduino.h>
#include <FastLED.h>

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

void clearLeds()
{
    FastLED.clear(true);
}

// prototypes

void randomDots2(CRGB leds[], int ledNum)
{
    currentLEDNum = random(ledNum);
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

void randomDots(CRGB leds[], int ledNum)
{
    currentLEDNum = random(ledNum);
    sLED currentLED;
    currentLED.index = currentLEDNum;
    currentLED.H = random(255);
    currentLED.S = random(255);
    currentLED.V = random(32, 255);
    leds[currentLEDNum] = CHSV(currentLED.H, currentLED.S, currentLED.V);
    FastLED.show();
    leds[currentLED.index] = CHSV(0, 0, 0);
}

void randomNoise(CRGB leds[], int ledNum)
{
    for (int i = 0; i < ledNum; i++)
    {
        leds[i] = CHSV(random(255), random(100, 255), random(1, 64));
    }
    FastLED.show();
    FastLED.clear();
    return;
}

void randomPurpleJumper(CRGB leds[], int ledNum)
{
    for (int i = 0; i < ledNum; i++)
    {
        leds[i] = CHSV(random(127, 250), random(100, 255), random(1, 64));
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
        leds[i] = CHSV(random(86, 172), random(100, 255), random(1, 64));
    }
    leds[random(ledNum)] = CRGB(255, 255, 255);
    FastLED.show();
    FastLED.clear();
    return;
}

void dotScrollRandomColor(CRGB leds[], int ledNum)
{
    for (int i = 0; i < ledNum; i++)
    {
        leds[i] = CHSV(random(0, 255), 255, 150);
        leds[random(ledNum)] = CHSV(128, 150, 100);
        FastLED.show();
        delay(22);
        FastLED.clear();
    }
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
    }
    FastLED.clear();
    FastLED.show();
    return;
}

void ltrDot(CRGB leds[], int ledNum, int rows, int cols)
{
    
    //----------------------------------------------------
    // Transform for LTR
    //----------------------------------------------------
    bool modVal = true;
    int mappedLeds[ledNum];
    int bigHop = (rows * 2) - 1;
    int smallHop = 1;
    int cCol = 0;
    int cRow = 0;
    int mappedVal = -1;

    // If a matrix instead of a strip, use the led count, rowcount and colcount 
    // to determine the mapping so looping through the array is left-to-right.
    if (rows > 0) 
    {
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
            mappedLeds[i] = mappedVal;
        }
    }
    else
    {
        *mappedLeds = *leds;
    }

    //----------------------------------------------------
    // Color and move the dot.
    // We could have done this in the mapping algo
    // but the leds would have to wait on the calcuation
    // at every interation.
    //----------------------------------------------------

    uint8_t randColor = random(0,255);
    for (int i = 0; i < ledNum; i++)
    {
        leds[mappedLeds[i]] = CHSV(randColor, 255, 255);
        FastLED.show();
        delay(1);
        FastLED.clear();
    }
}
