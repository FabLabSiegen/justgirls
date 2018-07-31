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


// #include <EEPROM.h>

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

#define NUM_PIXELS 60 // Number of LEDs in strip

#define NUM_MODES 1
#define NUM_STRIPES 4
#define STRIPE_WIDTH 14

// Here's how to control the LEDs from any two pins:
#define DATAPIN    6
#define BUTTONPIN 4
static uint8_t glut[256];
int buttonState;

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

void setup()
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

  pinMode(BUTTONPIN, INPUT);

}


void render_rings (const uint16_t t)
{
  static uint16_t positions[NUM_STRIPES];
  static uint8_t colors[NUM_STRIPES * 3];
  static int8_t speeds[NUM_STRIPES];
  uint8_t currentStripe, j, col[3];

  for (currentStripe = 0; currentStripe < NUM_STRIPES; currentStripe++)
    {

      if (speeds[currentStripe] != 0){
        //move stripe, depending on the speed
        positions[currentStripe] += speeds[i];
      }
      if (positions[currentStripe] < 0 || positions[currentStripe] >= NUM_PIXELS + 2 * STRIPE_WIDTH){
        //stripe is out of range, so stop it
        speeds[currentStripe] = 0;
      }

      //this stripe ended, so create a new one or not
      if (speeds[currentStripe] == 0 && random (10) == 0)
        {
          //create new stripe
          uint8_t basecolor;
          //in which direction should the stripe be animated?
          if (random (2) == 0)
            {
              //forward
              positions[currentStripe] = 0;
              speeds[currentStripe] = random(2);//random (1);
            }
          else
            {
              //backwards
              positions[currentStripe] = NUM_PIXELS + 2 * STRIPE_WIDTH - 1;
              speeds[currentStripe] = -random(2); //random (3);
            }

          //decide which basecolor, red, green or blue
          basecolor = random (3);

          colors[currentStripe*3 + (basecolor + 0) % 3] = random (150);
          colors[currentStripe*3 + (basecolor + 1) % 3] = random (150);
          colors[currentStripe*3 + (basecolor + 2) % 3] = random (15);
        }
    }

  for (i = STRIPE_WIDTH; i < NUM_PIXELS + STRIPE_WIDTH; i++)
    {
      col[0] = col[1] = col[2] = 0;

      for (currentStripe = 0; currentStripe < NUM_STRIPES; currentStripe++)
        {
          if (speeds[currentStripe] == 0)
            continue;

          col[0] = MIN (255, MAX (i, positions[j]) - MIN (i, positions[j]) < STRIPE_WIDTH ? col[0] + colors[currentStripe*3+0] : col[0]);
          col[1] = MIN (255, MAX (i, positions[j]) - MIN (i, positions[j]) < STRIPE_WIDTH ? col[1] + colors[currentStripe*3+1] : col[1]);
          col[2] = MIN (255, MAX (i, positions[j]) - MIN (i, positions[j]) < STRIPE_WIDTH ? col[2] + colors[currentStripe*3+2] : col[2]);
        }

      pixels.setPixelColor (i - STRIPE_WIDTH, glut[col[0]], glut[col[1]], glut[col[2]]);
    }
}

void loop()
{
    static uint16_t t = 0xffff;


    buttonState = digitalRead(BUTTONPIN);
    if (buttonState == HIGH) {

    }
    else {
      render_rings (t);
      delay (40);
    }

  // Time-Tick. Needed for moving stripes
  t--;

  // update Pixels
  pixels.show ();
}
