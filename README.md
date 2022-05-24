<!-- Please do not change this logo with link -->

[![MCHP](images/microchip.png)](https://www.microchip.com)

# Window Security Sensor and Room Temperature Monitor with AVR&reg; DB,  MLX90632 and MLX90392

The objective of this application is to create a window security sensor with room temperature monitoring for a compact smart home solution. The AVR&reg; DB microcontroller was selected for this application in order to use [Multi-Voltage I/O (MVIO) peripheral](#) to communicate with the sensors. To implement this solution, an [MLX90632](https://www.melexis.com/en/product/MLX90632) and an [MLX90392 sensor](https://www.melexis.com/en/product/MLX90392) were used.

This application was co-developed with Melexis, a global supplier of microelectronic semiconductor solutions.

[![Melexis Logo](./images/melexis.png)](https://www.melexis.com/)

## Related Documentation
- [MLX90632 (FIR Thermometer) Homepage](https://www.melexis.com/en/product/MLX90632/Miniature-SMD-Infrared-Thermometer-IC)
  - [MLX90632 Reference Software Library (from Melexis)](https://github.com/melexis/mlx90632-library)
- [MLX90392 (3D Magnetometer) Homepage](https://www.melexis.com/en/product/MLX90392/3D-Magnetometer-micro-power-and-cost-conscious)
- [MVIO Peripheral Homepage](#)

## Software Used

- [MPLAB&reg; X IDE v6.0.0 or newer](#)
- [MPLAB XC8 v2.35 or newer](#)
- [AVR-Dx_DFP v2.0.151](#)  
- Bluetooth&reg; Smart Data App by Microchip
  - [For iOS](https://apps.apple.com/us/app/bluetooth-smart-data/id1004033562)
  - [For Android](https://play.google.com/store/apps/details?id=com.microchip.bluetooth.data&hl=en_US&gl=US)

## Hardware Used

Please consult the Bill of Materials (BOM) in the documentation.

## Estimated Power Consumption

Important: **These values are provided for reference only.** Data collected from 1 unit on the bench.

Values were captured with the system powered from a DC power supply at 4.5V through a calibrated precision digital multimeter (with a sampling rate of 10 ksps - 1,000,000 samples per run).

Update rates determine how often new temperature measurements are collected and how often system status reported (either via Bluetooth or the flashed LED, depending on the operating mode).

Magnetometer samples at a constant rate in all modes, except calibration mode. In calibration mode, a higher sampling rate is used.

Software Configuration: *XC8_Free*

### Active Modes (Paired)
| Operating Mode | Average Current |
| -------------- | -----------------
| Active Mode | 4.5741 mA
| Calibration Mode | 5.9015 mA

Power consumption in active mode is not expected to vary significantly.  

### Slow (30s) Update Rate
| Operating Mode | Average Current |
| -------------- | -----------------
| Low-Power Mode (Temp not Monitored)| 222.69 &micro;A
| Low-Power Mode (Temp Monitored)| 264.59 &micro;A

### Normal (15s) Update Rate
| Operating Mode | Average Current |
| -------------- | -----------------
| Low-Power Mode (Temp not Monitored)| 223.10 &micro;A
| Low-Power Mode (Temp Monitored)| 303.20 &micro;A

### Fast (3s) Update Rate
| Operating Mode | Average Current |
| -------------- | -----------------
| Low-Power Mode (Temp not Monitored)| 224.12 &micro;A
| Low-Power Mode (Temp Monitored)| 591.91 &micro;A

## Table of Contents

- [Window Alarm Sensing](#window-alarm-sensing)
  - [The Advantage of a Magnetometer](#the-advantage-of-a-magnetometer)
  - [Calibrating the Magnetometer](#calibrating-the-magnetometer)
    - [Step 1 - Zero Calibration](#step-1---zero-calibration)
    - [Step 2 - Data Normalization](#step-2---data-normalization)
    - [Step 3 - Threshold Set](#step-3---threshold-set)
    - [Step 4 - Return to Closed](#step-4---return-to-closed)
- [Temperature Measurements](#temperature-measurements)
  - [Timing](#timing)
  - [Constant Caching](#constant-caching)
- [Advanced Features](#advanced-features)
  - [Safe Mode](#safe-mode)
- [Operating the Demo](#operating-the-demo)
- [User Commands](#user-commands)

## Window Alarm Sensing

Sensor Version: MLX90392-011 (&plusmn;50 mT version)

### The Advantage of a Magnetometer

One of the most common ways to detect whether a window or door has been opened/closed by an unknown person is to use a magnetic reed switch. When a magnet is placed near the switch, the two contacts inside either open or close. An external alarm circuit can monitor each circuit to know the current state of the system.

While reed switches are simple and easy to use, they come with the drawback that they only have 2 binary states. If an external magnet is placed close to the switch, the field from the added magnet can hold the contacts in position while the window is opened.

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

For ease of demonstrability, an [RN4870 Bluetooth module](https://www.microchip.com/en-us/product/RN4870) was used for wireless communication. In a production setting, we'd recommend switching to a different wireless communication method, like [Sub-GHz Radio](https://www.microchip.com/en-us/products/wireless-connectivity/sub-ghz).

### Pairing

Instructions, System Conditions, and other messages are sent via Bluetooth to the user's smartphone.

1. Open the Microchip Smart Data App
  1. Download links are provided in the [Software Used](#software-used) section.
2. Power-On the Demo Board.
3. Connect to the Bluetooth Device **MCHP-MLX_UXYZ**, where UXYZ is the S/N of the RN4870.
4. After connecting, a status message should appear in the app's terminal window.

### Sending Commands

Commands can be sent to the demo board to get more information or changing operating parameters / user settings.

All commands must be sent in the following format: **!\<Command,Parameter\>!**, with the ! acting as the deliminator. In cases where the parameter is not used (ex: HELP), it should be omitted.

Command Examples:  
**!HELP!** - Prints Help  
**!STU,F!** - Sets Temperature Units to Fahrenheit

### User Command List

| User Command | Format | Example | Description
| ------------ | ------ | ------- | ------------
| HELP | HELP | HELP | Get help with this demo.
| DEMO | DEMO | DEMO | Prints information about this demo.
| USER | USER | USER | Prints current user-defined settings
| CAL | CAL | CAL | Prints current calibration constants (for developers only)
| MAGRAW | MAGRAW | MAGRAW | Requests the next magnetometer result to be displayed on the screen. This usually occurs immediately after sending this command.
| VBAT  | VBAT  | VBAT | Prints current battery voltage.
| RECAL | RECAL | RECAL | Triggers a new calibration cycle of the demo.
| STATUS | STATUS | STATUS | Prints the current system status.
| RESET | RESET | RESET | Resets the user settings to the defaults. The demo will also be reset to calibration mode. If power is cycled before recalibration is completed, the old calibration values will be used on the next power-up.
| REBOOT | REBOOT | REBOOT | Reboots the microcontroller, but does NOT reset the settings.
| STU  | STU,<C/F/K> | STU,F | Sets the Temperature Unit (Celsius (*default*), Fahrenheit, Kelvin).
| STWH | STWH,\<TEMP\> | STWH,40.1 | Sets the High Temperature Alarm Point. Units are inherited from STU.
| STWL | STWL,\<TEMP\> | STWL,10.0 | Sets the High Temperature Alarm Point. Units are inherited from STU.
| STSR | STSR, \<FAST/NORM/SLOW\> | STSR,FAST | Sets the sampling rate of the temp sensor to 3s, 15s, or 30s intervals.
| PWDWN | PWDWN | PWDWN | Electrically disconnects the Bluetooth radio, stopping communications. Calibration must be valid and alarm is inactive to enter this state. See [Low Power Mode](#low-power-mode) for more information. Also aliased as PWRDWN.
| BTIDLEOFF | BTIDLEOFF,\<TRUE/FALSE\> | BTIDLEOFF,false | Enables (true) or disables (false) idle Bluetooth power-down. If enabled, after 30s where the system is not connected to a device, calibration is OK, and alarm is inactive, the system will auto-switch to [Low Power Mode](#low-power-mode).
| TEMPSLEEP | TEMPSLEEP,\<TRUE/FALSE\> | TEMPSLEEP, true | Enables temp monitoring while in sleep. If enabled, the system will measure the temperature via the FIR sensor. If the temperature is out of the expected ranges, the system will wake-up and restart the Bluetooth radio. **This mode will increase power consumption. It is recommended to use NORMAL or SLOW speed update rates to minimize power usage.**

### LED Status Indicator

The RGB LED on the board is used to indicate the following statuses:

Red LED - Alarm Tripped
Green LED - Alarm Good
Blue LED - Connected (and able to send) UART to the Smartphone

If both Red and Green are active, the device is in calibration mode.

### Calibration Mode

To enter calibration mode, press the Cal. button when NOT in Low Power mode or by sending the command "RECAL".

**Note: If in calibration mode and the board is powered off before completed the calibration previous calibration settings will be used. (Calibration values in progress are NOT stored in EEPROM until the cycle has finished). If the stored values are invalid, the system will request calibration on power being restored.**

### Low Power Mode

For longer battery life, the demo can be placed into Low Power Mode. In this mode, the Bluetooth radio is electrically disconnected from the power-supply. If the alarm is triggered in low power mode, the microcontroller will restart the Bluetooth module and wake-up. While in low power mode, commands from the user are not accepted. **On startup, the device does NOT start in Low Power Mode.**

To enter Low Power Mode, send the "PWDWN" command while the calibration is valid and the alarm is inactive.   
To exit Low Power Mode, press the Cal. button on the demo.

Note: Debug UART will remain active in Low Power Mode.

### Debug UART

A second UART is used on the device to print errors and technical information. A standard off-the-shelf USB-UART bridge can be connected to this pin header to view this output. Debug UART remains active in all modes, including Low Power Mode.

UART Settings:
- 115,200 Baud
- 8 bits, no parity, 1 stop bit
- Transmit Only (receiver not enabled)

## Summary

This demo has shown how MVIO can be used to interface with sensors that run at a different voltage level than the rest of the device. Special thanks again to Melexis for development assistance with this demo.
