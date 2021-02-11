/*
  Example Script: paj7620_cursor_mode.ino
  Package: RevEng_PAJ7620

  Copyright (c) 2020 Aaron S. Crandall
  Author       : Aaron S. Crandall <crandall@gonzaga.edu>
  Modified Time: December 2021

  Description: This example shows off "cursor mode" of the sensor
    This mode outputs an (X,Y) pair when an object is in view.
    It is effectively a "mouse mode" or "pointer mode"
    The intended use would be a low res non-contact pointer for input
    It would make a really cool game interface!
    This demo also enables the LED_BUILTIN - 
     when a cursor is in view the LED lights up.

  License: Same as package under MIT License (MIT)
*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 64

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Includes sensor driver object and interface
#include "RevEng_PAJ7620.h"

// Create gesture sensor driver object
RevEng_PAJ7620 sensor = RevEng_PAJ7620();

int led_vals[LED_COUNT];


#define LED_MAX_BRIGHTNESS 255
long long int last_refresh_millis = 0;
#define refresh_millis_interval 50
#define REFRESH_DECAY_RATE 0.90


// ***************************************************************************
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);     // Configure LED for output

  Serial.begin(115200);

  if( !sensor.begin() )             // return value of 1 == success
  {
    Serial.print("PAJ7620 init error -- device not found -- halting");
    while(true) {}
  }

  Serial.println("PAJ7620U2 init: OK.");

  sensor.setCursorMode();           // Put sensor into cursor mode
                                    //  stops gestures being detected

  Serial.println("Wave your hand over the sensor to see cursor coordinates.");
  Serial.println("When a cursor (object) is in view, ");
  Serial.println("the LED_BUILTIN should light up too.");

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(60); // Set BRIGHTNESS to about 1/5 (max = 255)

  for( int i = 0; i < LED_COUNT; i++ ) {
    led_vals[i] = 0;
  }
}


// ***************************************************************************
void loop()
{  
  if( sensor.isCursorInView() )
  {
    digitalWrite(LED_BUILTIN, HIGH);
    int cursor_x = sensor.getCursorX();
    int cursor_y = sensor.getCursorY();

    int axis_x = cursor_x / (3456 / 8);
    int axis_y = cursor_y / (3456 / 8);

    int calculated_led_index = calc_led_index(axis_x, axis_y);
    led_vals[calculated_led_index] = LED_MAX_BRIGHTNESS;

    for( int i = 0; i < LED_COUNT; i++ ) {
      strip.setPixelColor(i, strip.Color(0, 0, led_vals[i]));
    }
    strip.show();
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  if( (last_refresh_millis + refresh_millis_interval) < millis() ) {
    last_refresh_millis = millis();
    for( int i = 0; i < LED_COUNT; i++ ) {
      led_vals[i] *= REFRESH_DECAY_RATE;
      strip.setPixelColor(i, strip.Color(0, 0, led_vals[i]));
    }
    strip.show();
  }

  delay(20);
}


// ************************************************************************* //
int calc_led_index(int x, int y) {
  int ret_index = 0;
  ret_index = y * 8 + x;
  return ret_index;
}
