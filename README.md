# MAX31875 I2C Temperature Sensor Library
This repository contains sources, examples and description of library for Maxim Integrated MAX31875 temperature sensor connected to microcontroller using I2C bus. Library is designed as platform independent and is easily portable to other platforms. Library is ported to Cypress (Infineon) PSoC 6 platform and port is based on HAL library.

## Features
- Written in C. Supports both C and C++
- Dependencies only on stdint.h and stddef.h
- Support for reading templerature as float or RAW value from sensor
- Support for single-shot, continous and shutdown mode
- Support for configuring conversion rate (0.25 to 8 conversions per second)
- Support for configuring resolution (8, 9, 10 or 12 bits)
- Support for both normal and extended data formats
- Support for configuring over-temperature and hystherisis temperatures from both float or RAW input.
- Support for selecting comparator or interrupt over-temperature detection mode
- Support for configuring fault queue depth (1, 2, 4 or 6 faults before triggering over-temperature alarm)
- Support for calculating and checking CRC on both transmitted and received transactions (PEC)
- Multiple devices widht different addresses on the same bus are supported

## Instalaltion
Go to Release section and download prepared ZIP archive targeting your platform. Unpack the downloaded archieve and copy all 4 files to your project. Then add `#include "MAX31875.h"` at the befininf of your source code. See example below for more details.

If your platform is not supported, then download Generic variant and implement 4 functions in MAX31875_PlatformSpecific.c file according to comment description in these functions.

## Functions
Library provides following 17 functions to user:

```c
MAX31875_Status MAX31875_Init(MAX31875_Device* dev, uint8_t i2cAddress);
MAX31875_Status MAX31875_Deinit(MAX31875_Device* dev);

MAX31875_Status MAX31875_GetConfiguration(MAX31875_Device* dev, MAX31875_Configuration* config);
MAX31875_Status MAX31875_GetDefaultConfiguration(MAX31875_Device* dev, MAX31875_Configuration* config);
MAX31875_Status MAX31875_SetConfiguration(MAX31875_Device* dev, MAX31875_Configuration* config);

MAX31875_Status MAX31875_TriggerSingleShotConversion(MAX31875_Device* dev);
MAX31875_Status MAX31875_ReadRawTemperature(MAX31875_Device* dev, uint16_t* rawTemperature);
MAX31875_Status MAX31875_ReadTemperature(MAX31875_Device* dev, float* temperature);

MAX31875_Status MAX31875_GetOverTemperatureStatus(MAX31875_Device* dev, int* isOverTemperature);

MAX31875_Status MAX31875_GetHysteresisTreshold(MAX31875_Device* dev, float* temperatureHysteresis);
MAX31875_Status MAX31875_SetHysteresisTreshold(MAX31875_Device* dev, float temperatureHysteresis);
MAX31875_Status MAX31875_GetRawHysteresisTreshold(MAX31875_Device* dev, uint16_t* temperatureHysteresis);
MAX31875_Status MAX31875_SetRawHysteresisTreshold(MAX31875_Device* dev, uint16_t temperatureHysteresis);

MAX31875_Status MAX31875_GetOverTemperatureTreshold(MAX31875_Device* dev, float* overTemperatureLevel);
MAX31875_Status MAX31875_SetOverTemperatureTreshold(MAX31875_Device* dev, float overTemperatureLevel);
MAX31875_Status MAX31875_GetRawOverTemperatureTreshold(MAX31875_Device* dev, uint16_t* overTemperatureLevel);
MAX31875_Status MAX31875_SetRawOverTemperatureTreshold(MAX31875_Device* dev, uint16_t overTemperatureLevel);
```

## Example

In [examples folder](examples) there is implemented example for PSoC 62 platform showing possible configuration of sensor, changing resolution, conversion rate and data format. Example allows to demonstrate Packet Error Checking feature and confiogures over temperature and hystheresis thresholds. If you use different platforn, then [main.c](examples/01_overtemperature_psoc6/main.c) should be still interesting for you.

Following code show the most simple usage of library with MAX31875 sensor. If you are using vairant of sensor with different address, replace `MAX31875_I2C_7BIT_ADDRESS_DEFAULT` by `MAX31875_I2C_7BIT_ADDRESS_Rx` where `x` is the number after `R` in part name. For exmaple, if you use MAX31875**R3**TZS+, then use `MAX31875_I2C_7BIT_ADDRESS_R3`. If you use multiple devices on the bus, then instantiate `MAX31875_Device dev;` for each sensor and call `MAX31875_Init` for each sensor.

```c
#include "MAX31875.h"

int main(void) {
	MAX31875_Status mStatus;
	
	MAX31875_Device dev;
	mStatus = MAX31875_Init(&dev, MAX31875_I2C_7BIT_ADDRESS_DEFAULT);
	// check for error code in mStatus

	while (1) {
		float temperature;

		mStatus = MAX31875_ReadTemperature(&dev, &temperature);
		// check for error code in mStatus

		printf("Temperature: %.4f\r\n", temperature);
	}
}
```