#include <FastLED.h>

// constants
const int NUM_LEDS = 80; // number of LEDS in the strip
const int MIN_SPEED_INTERVAL = 500; // 500ms per rotation; 10mph
const int MAX_SPEED_INTERVAL = 5000; // 5000ms per rotation; 1mph
const int DELAY_MS = 1; // number of ms to delay by
const int COLORVAL_PER_MPH = 127; // max colorVal (1275) / max mph (10)

// pins
const int DATA_PIN = 6;
const int MAGSENSOR_PIN = 11;

// variables
int magSensor = 0; // magnet sensor value
int rotateInt = 0; // time in ms between each rotation
int prevColorVal = 1275;  // initialize previous color value
int colorVal = 1275; // initialize color value
double mph = 0; // variable for current mph, range: 1-20 mph
bool read = false; // avoid extra readings of magSensor in loop()

CRGB leds[NUM_LEDS]; // array of LEDS in strip

// find the interval between each rotation
int FindRotateInterval(int prevColorVal, int colorVal) {
  magSensor = digitalRead(MAGSENSOR_PIN); // read magnet sensor
  rotateInt = 0; // reset rotation

  int colorStep = (prevColorVal - colorVal) / 3; // interval for gradual color change

  // while the magnet sensor does not read a magnetic field, increases rotation interval
  while(magSensor == 1) {
    read = false;
    // change color gradually over first 150 ms; changes by x colorVal every 10 ms
    if (rotateInt <= 300 && rotateInt % 100 == 0) {
      ColorChange(prevColorVal - (colorStep * (rotateInt / 100)));
    }

    delay(DELAY_MS); // delay 1ms
    magSensor = digitalRead(MAGSENSOR_PIN); // read magnet sensor input
    // keep rotate int below MAX_SPEED_INTERVAL
    if (rotateInt <= MAX_SPEED_INTERVAL) {
      rotateInt++; // interval between each rotation, increases by 1 every 10 ms
    }
    
  }
  // keep rotateInt above MIN_SPEED_INTERVAL
  // MIN_SPEED_INTERVAL = Fastest mph speed (rotateInt and mph have an inverse relationship)
  if (rotateInt <= MIN_SPEED_INTERVAL) {
    rotateInt = MIN_SPEED_INTERVAL;
  }
  return(rotateInt);
} 

// change color of leds based on value of colorVal
void ColorChange(int colorVal) {
  //set color, colorVal = 0 is fastest; colorVal = 1275 is slowest
  if(colorVal <= 255) {
    fill_solid(leds, NUM_LEDS, CRGB(255 - colorVal, 0, 255)); // purple to blue
  } else if(colorVal <= 510) {
    fill_solid(leds, NUM_LEDS, CRGB(0, (colorVal - 255), 255)); // blue to teal
  } else if(colorVal <= 765) {
    fill_solid(leds, NUM_LEDS, CRGB(0, 255, 255 - (colorVal - 510))); // teal to green
  } else if(colorVal <= 1020) {
    fill_solid(leds, NUM_LEDS, CRGB((colorVal - 765), 255, 0)); // teal to green
  } else if(colorVal <= 1275) {
    fill_solid(leds, NUM_LEDS, CRGB(255, 255 - (colorVal - 1020), 0)); // green to red
  } else {
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0)); // turn off LEDS
  }
  FastLED.show();
}


void setup() {
  // define the leds in the strip
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);

  // setup serial monitor
  Serial.begin(9600);

  // defining pins
  pinMode(DATA_PIN, OUTPUT);
  pinMode(MAGSENSOR_PIN, INPUT);
}


void loop() {
  // find interval between rotation
  rotateInt = FindRotateInterval(prevColorVal, colorVal);
  
  if(rotateInt >= MIN_SPEED_INTERVAL && !read) {
    
    prevColorVal = colorVal; 

    mph = 1.0 / (rotateInt / 5000.0); // mph = reciprical of interval between each rotation, divided by 5000
    int nearestMPH = (int)mph; // nearest mph is rounding down mph to use as range for map
    Serial.println(mph);

    // mapping the mph to colorVal; using nearestMPH to make range more precise and accurate to the current speed aswell
    // as avoiding using rotateInt (range of 1 to 2 mph is 2500 ms, while 9 to 10 is only 55 ms)
    colorVal = map(mph, (nearestMPH - 1), nearestMPH, (COLORVAL_PER_MPH * (nearestMPH - 1)), (COLORVAL_PER_MPH * nearestMPH)); 
    Serial.println(colorVal);
    read = true;
  }
 
}



