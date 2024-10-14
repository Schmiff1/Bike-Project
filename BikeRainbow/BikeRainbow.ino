#include <FastLED.h>

// constants
const int NUM_LEDS = 80; // number of LEDS in the strip
const int MIN_SPEED_INTERVAL = 250; // 250ms per rotation; 20mph
const int MAX_SPEED_INTERVAL = 5000; // 5000ms per rotation; 1mph
const int DELAY_MS = 1; // number of ms to delay by
const int COLORVAL_PER_MPH = 76; // max colorVal (1530) / max mph (20)
const int NUM_RAINBOW_COLORS = 12; // number of colors used in the rainbow function

// pins
const int DATA_PIN = 6;
const int MAGSENSOR_PIN = 11;

// variables
int magSensor = 0; // magnet sensor value
int rotateInterval = 0; // time in ms between each rotation
int prevColorVal = 1530;  // initialize previous color value
int colorVal = 1530; // initialize color value
double mph = 0; // variable for current mph, range: 1-20 mph
bool read = false; // avoid extra readings of magSensor in loop()
bool atMaxSpeed = false; // bool to determine if at highest speed or not; used to trigger rainbow() function

// array of colors used for Rainbow function
CRGB colors[] = {
  CRGB(255, 0, 0),     // Red
  CRGB(255, 127, 0),   // Orange
  CRGB(255, 255, 0),   // Yellow
  CRGB(127, 255, 0),   // Spring Green
  CRGB(0, 255, 0),     // Green
  CRGB(0, 255, 127),   // Turquoise
  CRGB(0, 255, 255),   // Cyan
  CRGB(0, 127, 255),   // Azure
  CRGB(0, 0, 255),     // Blue
  CRGB(127, 0, 255),   // Violet
  CRGB(255, 0, 255),   // Magenta
  CRGB(255, 0, 127)    // Rose
};

CRGB leds[NUM_LEDS]; // array of LEDS in strip

// change color base on value of colorVal
void ColorChange(int colorVal) {
  atMaxSpeed = false;

  int index = colorVal / 255;
  int subIndex = colorVal % 255;

  switch (index) {
    case 0:
      fill_solid(leds, NUM_LEDS, CRGB(255 - subIndex, 0, 255)); // purple to blue
      break;
    case 1:
      fill_solid(leds, NUM_LEDS, CRGB(0, subIndex, 255)); // blue to teal
      break;
    case 2:
      fill_solid(leds, NUM_LEDS, CRGB(0, 255, 255 - subIndex)); // teal to green
      break;
    case 3:
      fill_solid(leds, NUM_LEDS, CRGB(subIndex, 255, 0)); // green to yellow
      break;
    case 4:
      fill_solid(leds, NUM_LEDS, CRGB(255, 255 - subIndex, 0)); // yellow to red
      break;
    case 5:
      atMaxSpeed = true;
      break;
    default:
      fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0)); // turn off LEDS
      break;
  }

  FastLED.show();
}

// find the interval between each rotation
int FindRotateInterval(int prevColorVal, int colorVal) {
  magSensor = digitalRead(MAGSENSOR_PIN); // read magnet sensor

  // find rotate interval
  rotateInterval = 0;

  int colorStep = (prevColorVal - colorVal) / 3; // interval for gradual color change

  // while the magnet sensor does not read a magnetic field, increases rotation interval
  while(magSensor == 1) {
    read = false;
    
    // change color gradually over first 150 ms; changes by x colorVal every 10 ms
    if (rotateInterval <= 300 && rotateInterval % 100 == 0) {
      ColorChange(prevColorVal - (colorStep * (rotateInterval / 100)));
    }

    if(atMaxSpeed && rotateInterval % 25 == 0) {
      Rainbow(rotateInterval);
    }

    delay(DELAY_MS); // delay 1ms
    magSensor = digitalRead(MAGSENSOR_PIN); // read magnet sensor input
    // keep rotate int below MAX_SPEED_INTERVAL
    if (rotateInterval < MAX_SPEED_INTERVAL) {
      rotateInterval++; // interval between each rotation, increases by 1 every 10 ms
    }
    
  }
  // keep rotateInterval above MIN_SPEED_INTERVAL
  // MIN_SPEED_INTERVAL = Fastest mph speed (rotateInterval and mph have an inverse relationship)
  if (rotateInterval <= MIN_SPEED_INTERVAL) {
    rotateInterval = MIN_SPEED_INTERVAL;
  }
  return(rotateInterval);
} 

double calculateSpeed(int rotateInterval) {
  return 1.0 / (rotateInterval / 5000.0); // mph = reciprical of interval between each rotation, divided by 5000ms
}

int calculateColorVal(double mph) {
  // finding the colorVal at an mph value
  return (int)(COLORVAL_PER_MPH * mph);
}

void Rainbow(int rotateInterval) {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = colors[(i + rotateInterval) % NUM_RAINBOW_COLORS];
  }
  FastLED.show();
}


void setup() {
  // define the leds in the strip
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);

  // defining pins
  pinMode(DATA_PIN, OUTPUT);
  pinMode(MAGSENSOR_PIN, INPUT);
}

void loop() {
  
  rotateInterval = FindRotateInterval(prevColorVal, colorVal); // find interval between rotation
  
  if(rotateInterval >= MIN_SPEED_INTERVAL && !read) {
    prevColorVal = colorVal; 

    mph = calculateSpeed(rotateInterval); // nearest mph is rounding down mph to use as range for map
    colorVal = calculateColorVal(mph); // find color value with mph

    read = true;
  }
}



