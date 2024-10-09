#include <FastLED.h>

// constants
const int NUM_LEDS = 80; // number of LEDS in the strip
const int MIN_SPEED_INTERVAL = 250; // 500ms per rotation; 10mph
const int MAX_SPEED_INTERVAL = 5000; // 5000ms per rotation; 1mph
const int DELAY_MS = 1; // number of ms to delay by
const int COLORVAL_PER_MPH = 76; // max colorVal (1530) / max mph (20)

// pins
const int DATA_PIN = 6;
const int MAGSENSOR_PIN = 11;

// variables
int magSensor = 0; // magnet sensor value
int rotateInterval = 0; // time in ms between each rotation
int prevColorVal = 1275;  // initialize previous color value
int colorVal = 1275; // initialize color value
double mph = 0; // variable for current mph, range: 1-20 mph
bool read = false; // avoid extra readings of magSensor in loop()
bool atMaxSpeed = false; // bool to determine if at highest speed or not; used to trigger rainbow() function

CRGB leds[NUM_LEDS]; // array of LEDS in strip

// find the interval between each rotation
int FindRotateInterval(int prevColorVal, int colorVal) {
  magSensor = digitalRead(MAGSENSOR_PIN); // read magnet sensor
  rotateInterval = 0; // reset rotation

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
  int colorVal = static_cast<int>(COLORVAL_PER_MPH * mph);

  // keep colorVal within its range
  if (colorVal < 0) colorval = 0;
  if (colorVal > 1530) colorVal = 1530;
  return colorVal;
}

// change color of leds based on value of colorVal
void ColorChange(int colorVal) {
  atMaxSpeed = false;
  //set color, colorVal = 0 is fastest; colorVal = 1275 is slowest
  if(colorVal <= 255) {
    fill_solid(leds, NUM_LEDS, CRGB(255 - colorVal, 0, 255)); // purple to blue
  } else if(colorVal <= 510) {
    fill_solid(leds, NUM_LEDS, CRGB(0, (colorVal - 255), 255)); // blue to teal
  } else if(colorVal <= 765) {
    fill_solid(leds, NUM_LEDS, CRGB(0, 255, 255 - (colorVal - 510))); // teal to green
  } else if(colorVal <= 1020) {
    fill_solid(leds, NUM_LEDS, CRGB((colorVal - 765), 255, 0)); // green to yellow
  } else if(colorVal <= 1275) {
    fill_solid(leds, NUM_LEDS, CRGB(255, 255 - (colorVal - 1020), 0)); // yellow to red
  } else if(colorVal <= 1530) {
    atMaxSpeed = true;
  }
  else {
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0)); // turn off LEDS
  }
  FastLED.show();
}

void Rainbow(int rotateInterval) {
  for(int i = 0; i < NUM_LEDS; i++) {
    if(i % 12 == rotateInterval % 12) {
      leds[i] = CRGB(255, 0, 0); // red
    } else if(i % 12 == (rotateInterval + 1) % 12) {
      leds[i] = CRGB(255,127,0); // orange
    } else if(i % 12 == (rotateInterval + 2) % 12) {
      leds[i] = CRGB(255, 255, 0); // yellow
    } else if(i % 12 == (rotateInterval + 3) % 12) {
      leds[i] = CRGB(127,255,0); // spring green?
    } else if(i % 12 == (rotateInterval + 4) % 12) {
      leds[i] = CRGB(0, 255, 0); // green
    } else if(i % 12 == (rotateInterval + 5) % 12) {
      leds[i] = CRGB(0,255,127); // turquoise
    } else if(i % 12 == (rotateInterval + 6) % 12) {
      leds[i] = CRGB(0, 255, 255); // cyan
    } else if(i % 12 == (rotateInterval + 7) % 12) {
      leds[i] = CRGB(0,127,255); // azure
    } else if(i % 12 == (rotateInterval + 8) % 12) {
      leds[i] = CRGB(0, 0, 255); // blue
    } else if(i % 12 == (rotateInterval + 9) % 12) {
      leds[i] = CRGB(127,0,255); // violet
    } else if(i % 12 == (rotateInterval + 10) % 12) {
      leds[i] = CRGB(255, 0, 255); // magenta
    } else if(i % 10 == (rotateInterval + 11) % 12) {
      leds[i] = CRGB(255,0,127); // rose
    }
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



