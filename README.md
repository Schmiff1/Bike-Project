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

#### 1. Loop function
- The loop function has 3 main parts:
    1. Finding the rotate interval
        - explained in more detail in the findRotateInterval function
    2. Converting the rotate interval to miles per hour and finding the nearest mile per hour
        - This is done by taking the recipricol of the rotate interval, and dividing that value by 5000.
    3. Mapping the miles per hour to a color value (in multiples of 255 to convert to RGB values for LED's)

```cpp
void loop() {
  // find interval between rotation
  rotateInt = FindRotateInterval(prevColorVal, colorVal);
  
  if(rotateInt >= MIN_SPEED_INTERVAL && !read) {
    
    prevColorVal = colorVal; 

    mph = 1.0 / (rotateInt / 5000.0); // mph = reciprical of interval between each rotation, divided by 5000
    int nearestMPH = (int)mph; // nearest mph is rounding down mph to use as range for map

    // mapping the mph to colorVal; using nearestMPH to make range more precise and accurate to the current speed aswell
    // as avoiding using rotateInt (range of 1 to 2 mph is 2500 ms, while 9 to 10 is only 55 ms)
    colorVal = map(mph, (nearestMPH - 1), nearestMPH, (COLORVAL_PER_MPH * (nearestMPH - 1)), (COLORVAL_PER_MPH * nearestMPH)); 
    read = true;
  }
}


```

#### 2. Finding the interval between each rotation.
- This function reads the magnetic hall effect sensor and calculates the time between rotations of the wheel.
- In order to achieve a gradual color change, this function also changes the color of the lights for the first 300 ms
    - This also goes for the Rainbow function, requiring 
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



## Future Features
- Include multiple files for different lighting effects
- Install all parts onto a PCB board to replace breadboard
- Switch to a smaller microcontroller for a more and easier to implement design (e.g. arduino nano/nano-every/pro mini or ESP32-C3)
- Bluetooth/wi-fi capabilities
