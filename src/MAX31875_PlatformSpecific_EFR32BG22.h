#ifndef MAX31875_PLATFORMSPECIFIC_H_
#define MAX31875_PLATFORMSPECIFIC_H_

#include "MAX31875.h"
#include <stdint.h>
#include <stddef.h>

#define MAX31875_I2C_INSTANCE			I2C0
#define MAX31875_I2C_INSTANCE_NO		0
#define MAX31875_I2C_PERIPHERAL_CLOCK 	cmuClock_I2C0

#define MAX31875_I2C_SDA_PORT			gpioPortD
#define MAX31875_I2C_SDA_PIN			3

#define MAX31875_I2C_SCL_PORT			gpioPortD
#define MAX31875_I2C_SCL_PIN			2

#define MAX31875_I2C_TIMEOUT_MS			100

MAX31875_Status MAX31875_PlatformSpecific_Init();
MAX31875_Status MAX31875_PlatformSpecific_Deinit();
MAX31875_Status MAX31875_PlatformSpecific_Read(MAX31875_Device* dev, uint8_t registerAddress, uint8_t* buffer, size_t bufferSize);
MAX31875_Status MAX31875_PlatformSpecific_Write(MAX31875_Device* dev, uint8_t registerAddress, uint8_t* buffer, size_t bufferSize);

#endif
