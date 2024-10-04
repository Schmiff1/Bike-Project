#include <FastLED.h>

// constants
const int NUM_LEDS = 80; // number of LEDS in the strip
const int MAX_SPEED = 50; // 250ms per rotation; 20mph
const int MIN_SPEED = 500; // 5000ms per rotation; 1mph

// pins
const int DATA_PIN = 6;
const int MAGSENSOR_PIN = 11;

// variables
int magSensor = 0; // magnet sensor value
int rotateInt = 0; // time in ms between each rotation
int prevColorVal = 1275;  // initialize previous color value
int colorVal = 1275; // initialize color value
double mph = 0; // variable for current mph, range: 1-20 mph
bool read = false;

CRGB leds[NUM_LEDS]; // array of LEDS in strip

// find the interval between each rotation
int FindRotateInterval(int prevColorVal, int colorVal) {
  magSensor = digitalRead(MAGSENSOR_PIN); // read magnet sensor
  int rotateInterval = 0; // reset rotation

  int x = (prevColorVal - colorVal) / 3; // interval for gradual color change

  // while the magnet sensor does not read a magnetic field, increases rotation interval
  while(magSensor == 1) {
    read = false;
    // change color gradually over first 40 ms; changes by x colorVal every 10 ms
    if (rotateInterval <= 15 & rotateInterval % 5 == 0) {
      ColorChange(prevColorVal - (x * (rotateInterval / 10)));
    }
    
    delay(10); // delay 10ms
    magSensor = digitalRead(MAGSENSOR_PIN); // read magnet sensor input
    
    rotateInterval++; // interval between each rotation, increases by 1 every 10 ms
    
  }
  if (rotateInterval >= MIN_SPEED) {
    rotateInterval = MIN_SPEED;
  } else if (rotateInterval <= MAX_SPEED) {
    rotateInterval = MAX_SPEED;
  }
  return(rotateInterval);
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
  

  if(rotateInt >= MAX_SPEED && !read) {
    Serial.println(rotateInt);
    prevColorVal = colorVal; 
    mph = 1.0 / (rotateInt / 500.0);
    int nearestMPH = (int)mph;
    colorVal = map(mph, nearestMPH, (nearestMPH + 1), (127 * nearestMPH), (127 * (nearestMPH + 1))); // convert rotateInt to colorVal
    //ColorChange(colorVal);
    Serial.println(mph);
    read = true;
  }
 
}



