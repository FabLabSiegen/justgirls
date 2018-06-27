//Made by Simon Budig

//Bootloader: Catrina Micro
//Evtl Bootloader separat flashen
// AVR-Programmer

// Simple strand test for Adafruit Dot Star RGB LED strip.
// This is a basic diagnostic tool, NOT a graphics demo...helps confirm
// correct wiring and tests each pixel's ability to display red, green
// and blue and to forward data down the line.  By limiting the number
// and color of LEDs, it's reasonably safe to power a couple meters off
// the Arduino's 5V pin.  DON'T try that with other code!

#include <EEPROM.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//
// // Because conditional #includes don't work w/Arduino sketches...
// #include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
// //#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

#define MAX(x,y) ((x) >= (y) ? (x) : (y))
#define MIN(x,y) ((x) <= (y) ? (x) : (y))

#define NUM_PIXELS 100 // Number of LEDs in strip

#define NUM_MODES 1
#define NUM_RINGS 1
#define RING_RESOLUTION 1
#define RING_WIDTH 10

// Here's how to control the LEDs from any two pins:
#define DATAPIN    6
#define CLOCKPIN   3

static uint8_t glut[256];


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, DATAPIN, NEO_GRB + NEO_KHZ800);
// Adafruit_DotStar pixels = Adafruit_DotStar(
//   NUM_PIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);

void
setup()
{
  uint16_t i;
  float rf;

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  // calculate Gamma-Table
  for (i = 0; i < 256; i++)
    {
      rf = i / 255.0;
      rf = pow (rf, 2.2);
      glut[i] = 255.0 * rf;
    }

  pixels.begin (); // Initialize pins for output
  pixels.show ();  // Turn all LEDs off ASAP
}


void
render_rings (const uint16_t t)
{
  static uint16_t positions[NUM_RINGS];
  static uint8_t colors[NUM_RINGS * 3];
  static int8_t speeds[NUM_RINGS];
  uint8_t i, j, col[3];

  for (i = 0; i < NUM_RINGS; i++)
    {
      if (speeds[i] != 0)
        positions[i] += speeds[i];
      if (positions[i] < 0 || positions[i] >= NUM_PIXELS + 2 * RING_WIDTH)
        speeds[i] = 0;

      if (speeds[i] == 0 && random (10) == 0)
        {
          uint8_t basecolor;

          if (random (2) == 0)
            {
              positions[i] = 0;
              speeds[i] = random (3);
            }
          else
            {
              positions[i] = NUM_PIXELS + 2 * RING_WIDTH - 1;
              speeds[i] = - random (3);
            }

          basecolor = random (3);

          colors[i*3 + (basecolor + 0) % 3] = random (256);
          colors[i*3 + (basecolor + 1) % 3] = random (256);
          colors[i*3 + (basecolor + 2) % 3] = random (32);
        }
    }

  for (i = RING_WIDTH; i < NUM_PIXELS + RING_WIDTH; i++)
    {
      col[0] = col[1] = col[2] = 0;

      for (j = 0; j < NUM_RINGS; j++)
        {
          if (speeds[j] == 0)
            continue;

          col[0] = MIN (255, MAX (i, positions[j]) - MIN (i, positions[j]) < RING_WIDTH ? col[0] + colors[j*3+0] : col[0]);
          col[1] = MIN (255, MAX (i, positions[j]) - MIN (i, positions[j]) < RING_WIDTH ? col[1] + colors[j*3+1] : col[1]);
          col[2] = MIN (255, MAX (i, positions[j]) - MIN (i, positions[j]) < RING_WIDTH ? col[2] + colors[j*3+2] : col[2]);
        }

      pixels.setPixelColor (i - RING_WIDTH, glut[col[0]], glut[col[1]], glut[col[2]]);
    }
}

void
render_redwhitegreen (const uint16_t t)
{
  uint16_t i;
  uint8_t pos;

  for (i = 0; i < NUM_PIXELS; i++)
    {
      pos = (t + i) % 149;
      if (pos < 51)
        pixels.setPixelColor (i, 0, glut[pos * 5], 0);
      else if (pos < 66)
        pixels.setPixelColor (i, glut[(pos - 51) * 17], 255, glut[(pos - 51) * 17]);
      else if (pos < 81)
        pixels.setPixelColor (i, 255, glut[255 - (pos - 66) * 17], glut[255 - (pos - 66) * 17]);
      else if (pos < 132)
        pixels.setPixelColor (i, glut[255 - (pos - 81) * 5], 0, 0);
      else
        pixels.setPixelColor (i, 0, 0, 0);
    }
}



void
loop()
{
  static uint16_t t = 0xffff;
  static uint8_t state = 0xff;

  if (state >= NUM_MODES)
    state = EEPROM.read(0);
  if (state >= NUM_MODES)
    state = 0;

  switch (state)
    {
      case 0:
        render_rings (t);
        delay (50);
        break;
      case 1:
      default:
        render_redwhitegreen (t);
        delay (50);
        break;

    }

  // Time-Tick. Needed for moving stripes
  t--;

  // update Pixels
  pixels.show ();
}
