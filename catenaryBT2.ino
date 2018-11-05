/* Simulate two balls in SHM
   WS2812 LED strip with HC-05 Bluetooth adaptor
   Hang the LED strip like a catenary
   Audio output from digital pin, connect to extnl amp 
   Bluetooth control viz:
   u - up the brightness
   d - dim the brightness
   S - toggle sound output
   f - faster
   s - slower
   
Geo - 05.11.2018
*/

#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11); // RXin, TXout

#include <FastLED.h>
#include <Math.h>
#define NUM_LEDS 120
#define DATA_PIN 5      // to LED strip control wire
#define tonepin 3       // connect via resistor chain to grnd
#define COLOR_ORDER GRB
#define mindel 3        // min value of DELAY
#define LED 13          // Pin 13 is connected to the Arduino LED
char rxChar = 0;        // rxChar holds the received command.
int j = 200;
int k = 200;
int m = 0;
int BRIGHTNESS = 4;
int DELAY = 10;         // extra delay at end of LED run

bool toggle = false;
bool stoploop = false;
bool debug = false;
bool sound = true;
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);   // Open serial port
  FastLED.addLeds<WS2812, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(0, 0, 0);
    FastLED.show();
    BTSerial.begin(9600);
    BTSerial.flush();
  }
}

void loop() {
  static int loopcnt = 0;
  while (!stoploop) {
    if (BTSerial.available() > 0 ) {        // Check receive buffer.
      rxChar = BTSerial.read();            // Save character received.
      BTSerial.flush();                    // Clear rx buffer
      Serial.println(rxChar);

      switch (rxChar) {
        case 'u':
          {
            if (BRIGHTNESS < 30) {
              BRIGHTNESS += 5;
              FastLED.setBrightness(  BRIGHTNESS );
              Serial.print("Brightness = ");
              Serial.println(BRIGHTNESS);
            }
          }
          break;
        case 'd':
          {
            if (BRIGHTNESS > 5) {
              BRIGHTNESS -= 5;
              FastLED.setBrightness(  BRIGHTNESS );
              Serial.print("Brightness = ");
              Serial.println(BRIGHTNESS);
            }
          }
          break;
        case 'S': {
            sound = !sound;
          }
          break;
        case 'f':
          {
            if (DELAY > 3) {
              DELAY -= 2;
              Serial.print("delay = ");
              Serial.println(DELAY);
            }
          }
          break;
        case 's':
          {
            if (DELAY < 20) {
              DELAY += 2;
              Serial.print("delay = ");
              Serial.println(DELAY);
            }
          }
          break;
      }
    }
    for (int i = loopcnt; i < NUM_LEDS / 2; i++) {  //in to centre
      leds[i].setRGB(j, k, m);                      //eg 0->59
      leds[NUM_LEDS - i - 1].setRGB(k, j, m);       //eg 119->59
      FastLED.show();
      leds[i] = CRGB::Black;
      leds[NUM_LEDS - i - 1] = CRGB::Black;

      int n = 100 * (2 * i) / NUM_LEDS;
      float value = DELAY * (1 - sin(n * PI / 200) );
      delay (mindel + int(value));

      if (debug) {
        Serial.print (" * ");
        Serial.print (loopcnt);
        Serial.print (" \t ");
        Serial.print(i);
        Serial.print (" \t ");
        Serial.println(NUM_LEDS - i - 1);
        //  Serial.print (" \t ");
        // Serial.println(value, 4);
      }
      if (sound) {
        blip(1);
      }
    }

    for (int i = (NUM_LEDS / 2); i > loopcnt; i--) {
      leds[i - 1].setRGB(k, j, m);          //eg 59->0
      leds[NUM_LEDS - i].setRGB(j, k, m); //eg. 59->119
      FastLED.show();
      leds[i - 1] = CRGB::Black;
      leds[NUM_LEDS - i] = CRGB::Black;

      int n = 100 * (2 * i) / NUM_LEDS;
      float value = DELAY * (1 - sin(n * PI / 200) );
      delay (mindel + int(value));

      if (debug) {
        Serial.print (" ** ");
        Serial.print (loopcnt);
        Serial.print (" \t ");
        Serial.print(i - 1);
        Serial.print (" \t ");
        Serial.println(NUM_LEDS - i);
      }
      if (sound) {
        blip(1);
      }
    }

    if (sound) {
      tone(tonepin, 200 + (loopcnt * 10), 150);
      delay(150);
    }

    toggle = !toggle;
    if (toggle == true) {
      j = 255;
      k = 0;
    }
    else {
      j = 0;
      k = 255;
    }
    loopcnt++;
    if (loopcnt == NUM_LEDS / 2) {
      loopcnt = 0;
      FastLED.show();
      if (sound) {
        for (int i = 0; i < 20; i++) {
          tone(tonepin, i * 70, 50);
          delay (50);
        }
        for (int i = 20; i > 0; i--) {
          tone(tonepin, i * 70, 50);
          delay (50);
        }
      }
      delay(2000);
    }

    if (debug && (loopcnt == 6)) {
      stoploop = true;
    }
  }
}

void blip(int t) {
  digitalWrite(tonepin, HIGH);
  delay(t);
  digitalWrite(tonepin, LOW);
}
