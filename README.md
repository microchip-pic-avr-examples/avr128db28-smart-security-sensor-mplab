<!-- Please do not change this logo with link -->

[![MCHP](images/microchip.png)](https://www.microchip.com)

# Window Security Sensor and Room Temperature Monitor with AVR&reg; DB,  MLX90632 and MLX90392

The objective of this demo is to create a window security sensor with a room temperature sensor for a compact smart home solution. The AVR&reg; DB microcontroller was selected for this application in order to use Multi-Voltage I/O (MVIO) peripheral. To implement this solution, an MLX90632 and an MLX90392 sensor from Melexis were used.

## Related Documentation
- [MLX90632 (FIR Thermometer) Homepage](https://www.melexis.com/en/product/MLX90632/Miniature-SMD-Infrared-Thermometer-IC)
  - [MLX90632 Reference Software](https://github.com/melexis/mlx90632-library)
- [MLX90392 (3D Magnetometer) Homepage](https://www.melexis.com/en/product/MLX90392/3D-Magnetometer-micro-power-and-cost-conscious)

## Software Used

- [MPLAB&reg; X IDE v6.0.0 or newer](#)
- [MPLAB XC8 v2.35 or newer](#)
- [AVR-Dx_DFP v1.10.124](#)  
- Bluetooth Smart Data App
  - [For IOS](#)
  - [For Android](#)

## Hardware Used

Please consult the Bill of Materials (BOM) in the documentation.

## Table of Contents

- [Window Alarm Sensing](#window-alarm-sensing)
- [Temperature Measurements](#temperature-measurements)
- [Operating the Demo](#operating-the-demo)

## Window Alarm Sensing

Sensor: MLX90392-011 (&plusmn;50 mT version)

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
**Maximum Magnitude Monitoring - Vector Sum of Magnitude is Tracked for Step 3**

The objective of the data normalization to scale each of the 3 axes to be in similar scales to each other. Due to the direction of travel, one of the axes will have a much higher scale than the others.

The calibration algorithm in this step performs a similar action to the [Zero Calibration in Step 1](#step-1---zero-calibration). However, each averaged value is used to compute the scaling factor to compress the values to a 7-bit number. If the value is negative, then the scaling factor is negative.

| Input | Computed Scaling Factor | Normalized Value
| ----- | ----------------------- | ---------------
| 255   | 1 | 127
| -80   | -1 | 80
| -288  | -2 | 72

**Note: Ideally the values would remain 7-bit, but depending on the placement of the magnetometer, it is possible for the maximum intensity to occur at a slightly different position. The program limits the maximum intensity to 127.**

#### Step 3 - Threshold Set

User Directions: *Please open the window to the desired threshold, then press the button.*  
**Maximum Magnitude Monitoring - Vector Sum of Magnitude is Tracked**

In this step, the magnetometer is programmed to the threshold between open and closed. Unlike the reed switch, the user can set how far the window is open before triggering the alarm.

Additionally, at the conclusion of this stage, the program stores the highest magnitude of the field strength recorded during steps 2 and 3.

Magnitude<sup>2</sup> = X<sup>2</sup> + Y<sup>2</sup> + Z<sup>2</sup>

The maximum magnitude is used to protect against an attempt to tamper with the sensor with another magnet.

#### Step 4 - Return to Closed

User Directions: *Please close the window and press the button to finish.*  

This step ends the calibration sequence and saves the constants to the EEPROM of the microcontroller.

## Temperature Measurements

Sensor: MLX90632

### How FIR Sensors Work



## Operating the Demo

## Summary

<!-- Summarize what the example has shown -->
