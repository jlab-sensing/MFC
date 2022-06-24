This directory contains the arduino code to log data from the TEROS-12 sensors. 

# Getting started

The Arduino Uno is used to interface with the TEROS-12 sensors. The Arduino IDE
available at https://www.arduino.cc/en/main/software. To get started check out
the Arduino IDE Getting Started
(https://docs.arduino.cc/software/ide-v1/tutorials/arduino-ide-v1-basics). The
examples are a great resource.

## Connecting Sensors

The TEROS-12 sensors first need to be connected to the Arduino. The table below
shows the connection between the sensor and the Arduino. If multiple sensors
are used, they can wired in parallel. Simply put connect all the sensors to the
same data, power and ground. Only a maximum of two sensors have been tested to
work while being powered from the Arduino. For more sensors, current limits
could become an issue.

| Name   | TEROS-12          | Arduino |
|--------|-------------------|---------|
| Data   | Orange            | 13      |
| Power  | Brown             | 5V      |
| Ground | Silver/Unshielded | GND     |

![Data wire](https://raw.githubusercontent.com/lab11/soil-battery/master/arduino/images/20201023_172526.jpg)
![Power and GND](https://raw.githubusercontent.com/lab11/soil-battery/master/arduino/images/20201023_172538.jpg)

## Configuring Addresses

> Skip this step if only using a single sensor and the address of the sensor is
> known.

1. Open `AddressConfig.ino`
2. Change `new_addr` to single alphanumeric character. Numbers are preferred in
   the SDI protocol.
3. Upload sketch to Arduino
3. Check the responded address in the serial monitor. Ensure the baud rate is
   set to `9600`. The output should be similar to the following

```
TEROS-12 Address configurator, compiled on Jun 24 2022 10:09:26
Sensor active at 0
Changing to 1
Responded 1
```

## Setting up serial logger

1. Open `SerialLogger.ino`.
2. Configure `MEAS_INT` (period between measurements), `addrs`, `addrs_len`
   (addresses of connected sensors). Make sure the length of the array and
   defined length match. For example, a single sensor with default address of
   `0` would use

```
/** Array of address characters */
const char addrs[] = {'0'};
/** Length of @p addrs */
const unsigned int addrs_len = 1;
```

3. Upload sketch to Arduino
4. Verify output with serial monitor. Ensure the serial port is correctly set
   and baud rate is `1200`. It might take a few seconds for data to appear. The
   data outputted be similar to the following.

```
0+2639.13+21.8+312
0+2641.15+21.8+307
0+2639.81+21.8+316
```

For reference, the data if formatted as
`sensor_id+raw_soil_moisture+temp+electrical_conductivity`.

Congrats, you've successfully programmed the Arduino to read data from the TEROS-12 sensor!

# Preparing the Arduino for long-term deployment
Using the SDI-12 protocol, each Arduino can collect data from multiple sensors. To use this cabability, twist together the wires ends and plug them in as directed above. The wires can pull out of the Arduino pin holes rather easily, so I recommend putting the Arduino in a plastic case and puting the TEROS wires through the square hole opposite the USB port. Then you can secure the cable into place using hot glue or some similar material. Finally, tape the case together so it doesn't fall apart. These measures have worked for the Stanford TEROS setup, which has been carted all over the place and hasn't required any repairs/adjustment for more than a year.

![tape and glue](https://raw.githubusercontent.com/lab11/soil-battery/master/arduino/images/20201023_172453.jpg)
![sensor ready](https://raw.githubusercontent.com/lab11/soil-battery/master/arduino/images/20201023_172437.jpg)


Finally, plug the Arduino into a Shepherd or Rocketlogger. Shepherds/Rocketloggers contain an SD card, which is where the sensor data will be recorded.

# Additional info 

The `TEROS_Integrator_Guide.pdf` documents the serial protocol(s) the loggers use. We use the SDI-12 serial variant. 
