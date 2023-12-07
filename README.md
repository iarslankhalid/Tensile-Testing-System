# Tensile Testing System

## Overview

The Tensile Testing System project is designed to apply precise force to the machine jaws using motors, enabling the stretching or compression of materials and obtaining the stress-strain graph.

### Project Link

[Link to Project on Wokwi](https://wokwi.com/projects/376282440828275713) - Refer to the Main Menu section for operational instructions.

## Hardware Requirements

1. Arduino Mega 2660
2. Stepper Motors (NEMA 34)
3. Stepper Motor Drivers (HX-134 or A4988)
4. Load Cell
5. Keypad
6. LCD Screen (16 x 2)
7. Enclosure Box

## Hardware Implementation

The machine consists of two jaws, with the bottom jaw being stationary and the upper one movable. The load cell is attached to the moving jaw, controlled by two precisely aligned motors. The load cell, motor drivers, keypad, and screen are connected to the Arduino for control and sensor data acquisition.

## Code Implementation

1. Compile 'sketch.ino' on your Arduino.
2. Other files are used to run the project on the Wokwi website.

### Dependencies

Install the following libraries:

1. `keypad.h`
2. `HX711.h`
3. `LiquidCrystal.h`

### Calibration

Calibrate the load cell and update the value on the `calibrationFactor` variable in the code.

## Main Menu

1. Press '*' on the keypad to access the menu.
2. Different modes will be listed:
    - Adjust Motors
    - Normal Mode
    - Manual Mode
3. Each mode will be displayed for 3 seconds. Press 'C' to skip or 'D' to select.

### Modes

#### 1. Adjust Motors

- Use 'A' or 'B' to adjust the motors.
- Press '#' to stop the motor.
- Press '#' again to exit the mode.

#### 2. Normal Mode

- Place the material into the machine jaws.
- Enter the desired force and press '#' to select.
- Enter the number of iterations and press '#' to select.
- Motors will exert the specified force for the specified iterations.

#### 3. Manual Mode

- Place the material into the machine jaws.
- Use 'A' and 'B' to move the motors.
- Press '0' to stop the motors.
- The applied force will be displayed on the screen.
- Press the reset button to exit the manual mode.
