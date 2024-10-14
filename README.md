# LED Speedometer Bike Light

### Description
This project features an LED light strip integrated with a speedometer for a bike tire. Utilizing a magnet sensor, it 
detects the rotation of the wheel to calculate speed. As the bike accelerates, the light color transitions based on 
the current speed. This setup creates a very nice, dynamic visual element to the bike, as well as helping with visibility at nighttime.

### Background

I originally began working on this project for an assignment in high school, however starting this spiked my 
curiousity and when it ended up failing by the end of my senior year, I felt a need to finish it. I have been working on 
improving the project as much as possible since then, making it more stable, consistent, and adding new features
as my experience in the field expands.


## Showcase

## How It's Made

### Schematic

### Materials
- Arduino 
- A1440 hall effect magnetic sensor
- WS2812b led light strip
- AA Battery Pack

## Coding

### Libraries used
- FastLED: controlling the LED strip

### Key components

#### Loop function
- The loop function has 3 main parts:
    1. Finding the rotate interval
    2. Converting the rotate interval to miles per hour
    3. Converting the miles per hour to a color value 



```cpp
void loop() {
  rotateInterval = FindRotateInterval(prevColorVal, colorVal); // find interval between rotation
  
  if(rotateInterval >= MIN_SPEED_INTERVAL && !read) {
    prevColorVal = colorVal; 

    mph = calculateSpeed(rotateInterval); // nearest mph is rounding down mph to use as range for map
    colorVal = calculateColorVal(mph); // find color value with mph

    read = true;
  }
}
```

#### 1. Finding the interval between each rotation.
- This function reads the magnetic hall effect sensor and calculates the time between rotations of the wheel.
- In order to achieve a gradual color change, this function also changes the color of the lights for the first 300 ms
- This also is used to call the Rainbow function, requiring calls every 25ms in order to achieve the desired lighting effect.
``` cpp
int FindRotateInterval(int prevColorVal, int colorVal) {
  magSensor = digitalRead(MAGSENSOR_PIN); // read magnet sensor
  rotateInt = 0; // reset rotation

  // while the magnet sensor does not read a magnetic field, increases rotation interval
  while(magSensor == 1) {
    read = false;
    Serial.println(rotateInt);
    
    delay(DELAY_MS); // delay 1ms
    magSensor = digitalRead(MAGSENSOR_PIN); // read magnet sensor input

    // keep rotate int below MAX_SPEED_INTERVAL
    if (rotateInt < MAX_SPEED_INTERVAL) {
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
```

#### 2. Calculating speed
- This is just a one line function with the equation to convert the rotate interval to mph. The math for this function can be shown using dimensional analysis.
```cpp
double calculateSpeed(int rotateInterval) {
  return 1.0 / (rotateInterval / 5000.0); // mph = reciprical of interval between each rotation, divided by 5000ms
}
```

#### 3.1 Calculating the ColorVal

- This is again just a one line function, multiplying the miles per hour by the range of color values in one mile per hour(maxColorVal / maxMPH)
- the rest of the lines of this function are just to double check that the colorVal stays within its bounds.
```cpp
int calculateColorVal(double mph) {
  // finding the colorVal at an mph value
  return (int)(COLORVAL_PER_MPH * mph);
}
```

#### 3.2 Displaying the ColorVal
- The color value is a value in multiples of 255 in order to determine the RGB value for the LED strip. Currently, the max value for this function is 1530, giving us 6 ranges of color.
- This is then shown in the code as 6 if/else if statements, calculating the RGB color based on the value of colorVal

```cpp
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
  } else {
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0)); // turn off LEDS if value outside of colorVal range
  }
  FastLED.show();
}
```

- Whenever the value of colorVal is between 1275 and 1530 (__ to 20mph), atMaxSpeed becomes true, which starts running the Rainbow function.
  The Rainbow function contains a series of 12 if statements and colors that are constanly looped through during the FindRotateInterval function.
  This causes an interesting and very visually pleasing effect on the LED lights, running through each of the colors of the rainbow
```cpp
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
```


## Future Features
- Include multiple files for different lighting effects
- Install all parts onto a PCB board to replace breadboard
- Switch to a smaller microcontroller for a more compact, functional, and easier to implement design (e.g. arduino nano/nano-every/pro mini or ESP32-C3)
- Bluetooth/wi-fi capabilities