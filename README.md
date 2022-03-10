<!-- Please do not change this logo with link -->

[![MCHP](images/microchip.png)](https://www.microchip.com)

# Window Security Sensor and Room Temperature Monitor with AVR&reg; DB,  MLX90632 and MLX90392

The objective of this application is to create a window security sensor with room temperature monitoring for a compact smart home solution. The AVR&reg; DB microcontroller was selected for this application in order to use Multi-Voltage I/O (MVIO) peripheral to communicate with the sensors. To implement this solution, an MLX90632 and an MLX90392 sensor were used.

This application was co-developed with Melexis N.V., a manufacturer of digital sensors.

[![Melexis Logo](./images/melexis.png)](https://www.melexis.com/)

## Related Documentation
- [MLX90632 (FIR Thermometer) Homepage](https://www.melexis.com/en/product/MLX90632/Miniature-SMD-Infrared-Thermometer-IC)
  - [MLX90632 Reference Software Library (from Melexis)](https://github.com/melexis/mlx90632-library)
- [MLX90392 (3D Magnetometer) Homepage](https://www.melexis.com/en/product/MLX90392/3D-Magnetometer-micro-power-and-cost-conscious)

## Software Used

- [MPLAB&reg; X IDE v6.0.0 or newer](#)
- [MPLAB XC8 v2.35 or newer](#)
- [AVR-Dx_DFP v1.10.124](#)  
- Bluetooth Smart Data App by Microchip
  - [For IOS](https://apps.apple.com/us/app/bluetooth-smart-data/id1004033562)
  - [For Android](https://play.google.com/store/apps/details?id=com.microchip.bluetooth.data&hl=en_US&gl=US)

## Hardware Used

Please consult the Bill of Materials (BOM) in the documentation.

## Table of Contents

- [Window Alarm Sensing](#window-alarm-sensing)
- [Temperature Measurements](#temperature-measurements)
- [Operating the Demo](#operating-the-demo)

## Window Alarm Sensing

Sensor Version: MLX90392-011 (&plusmn;50 mT version)

### The Advantage of a Magnetometer

One of the most common ways to detect whether a window or door has been opened/closed by an unknown person is to use a magnetic reed switch. When a magnet is placed near the switch, the two contacts inside either open or close. An external alarm circuit can monitor each circuit to know the current state of the system.

While reed switches are simple and easy to use, they come with the drawback that they only have 2 binary states. If an external magnet is placed close to the switch, the field from the magnet can hold the contacts in position while the window is opened.

A magnetometer avoids this problem by measuring the intensity of the magnetic field. If the intensity changes beyond a set range, then the magnetometer can detect the attempt to bypass the alarm.

### Calibrating the Magnetometer

Before using the magnetometer in system, it must be calibrated. Small variations in installation and position are more than enough to cause the alarm to trigger. After some internal testing, we found a 4-step calibration routine worked best.

1. Zero Calibration
2. Data Normalization
3. Threshold Set
4. Return to Closed

#### Step 1 - Zero Calibration

User Directions: *Open the window and press the button to start.*  

To zero the sensor, the microcontroller collects a set number of samples from the magnetometer with the magnet on the window in the farthest position from the sensor. After the set number of samples are acquired, the microcontroller computes the (whole number) average from the totals in each dimension and uses this as the offset.

#### Step 2 - Data Normalization

User Directions: *Please close the window and the press the button to continue.*  

The objective of the data normalization to scale each of the 3 axes to be in similar scales to each other. Due to the direction of travel, one of the axes will have a much higher scale than the others.

The calibration algorithm in this step performs a similar action to the [Zero Calibration in Step 1](#step-1---zero-calibration). However, each averaged value is used to compute the scaling factor to compress the values to a signed 8-bit number. If the value is negative, then the scaling factor will be negative.

| Input | Computed Scaling Factor | Normalized Value
| ----- | ----------------------- | ---------------
| 255   | 1 | 127
| -80   | -1 | 80
| -288  | -2 | 72

**Note: Ideally the values would remain 7-bit, but depending on the placement of the magnetometer, it is possible for the maximum intensity to occur in a slightly different position. The program limits the maximum intensity to 127. For total magnitude, the full normalized value is used.**

#### Step 3 - Threshold Set

User Directions: *Please open the window to the desired threshold, then press the button.*  

In this step, the magnetometer is programmed to the threshold between open and closed. Unlike the reed switch, the user can set how far the window is open before triggering the alarm. Additionally, at the conclusion of this step, the program stores the highest magnitude of the field strength recorded between (the end of) step 2 and 3.

Magnitude<sup>2</sup> = X<sup>2</sup> + Y<sup>2</sup> + Z<sup>2</sup>

The maximum magnitude is used to protect against attempts to tamper with the sensor with another magnet.

#### Step 4 - Return to Closed

User Directions: *Please close the window and press the button to finish.*  

This step validates the calibration while the user closes the window. If the alarm triggers during this, the calibration is considered bad and returns to step 1. If the window is closed and the alarm did not trigger, then the constants are saved to the EEPROM of the microcontroller.

## Temperature Measurements

Sensor: MLX90632

### Timing

The MLX90632 sensor takes 500ms to get one half of a sample (see sensor datasheet), and/or 1s for a full conversion. This demo only uses the full conversion timing. Due to the acquisition time delay, the microcontroller starts the temperature acquisition early to ensure the results are ready when the application prints the status. 

### Constant Caching
To improve the start-up time of the application, the factory calibration constants stored on the MLX90632 are converted into the required floating point values and are stored in the microcontroller's EEPROM memory.

On startup, the microcontroller looks at the EEPROM to:

- Match the serial number of the sensor to the value in memory
- Match the XOR of the serial number words

If either of these checks fail, then the memory is considered invalid, and the constants will be reacquired and reprogrammed into EEPROM. Other constants related to the temperature sensor will also be reset.

Constant Caching can be disabled by commenting out the macro `CACHE_CONSTANTS` in **MLX90632.h**. Booting the application in safe mode will also ignore the EEPROM values.

## Advanced Features

### Safe Mode
To enter safe mode:
- Hold the "Cal" button
- Power-up the demo

In safe mode, the application ignores stored values in EEPROM and resets all user settings to the compile-time defaults. Constants for the MLX90632 (temp. sensor) will be reacquired and cached and the magnetometer will need to be recalibrated before use.

**Note: If the application is powered-off before recalibrating the magnetometer, the magnetometer will load the old calibration on the next start-up.**

## Operating the Demo


## User Commands

| User Command | Format | Example | Description
| ------------ | ------ | ------- | ------------
| HELP | HELP | HELP | Returns list of commands
| DEMO | DEMO | DEMO | Prints information about this demo
| BAT  | BAT  | BAT | Prints current battery voltage
| STATUS | STATUS | STATUS | Prints the current system status
| STU  | STU,<C/F/K> | STU,F | Sets the Temperature Unit (Celsius (*default*), Fahrenheit, Kelvin)
| STWH | STWH,\<TEMP\> | STWH,40.1 | Sets the High Temperature Alarm Point. Units are inherited from STU.
| STWL | STWL,\<TEMP\> | STWL,10.0 | Sets the High Temperature Alarm Point. Units are inherited from STU.
| STSR | STSR, \<FAST/NORM/SLOW\> | STSR,FAST | Sets the sampling rate of the temp sensor to 3s, 15s, or 30s intervals.
| RST   | RST   | RST   | Resets the microcontroller. Communications will be reset.
| PWDWN | PWDWN | PWDWN | Electrically disconnects the Bluetooth radio, stopping communications.

## Summary

<!-- Summarize what the example has shown -->
