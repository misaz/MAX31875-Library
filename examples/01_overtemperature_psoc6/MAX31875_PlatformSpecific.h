#ifndef MAX31875_PLATFORMSPECIFIC_H_
#define MAX31875_PLATFORMSPECIFIC_H_

#include "MAX31875.h"
#include <stdint.h>
#include <stddef.h>

#include <cyhal.h>

#define MAX31875_PORT_SDA		P6_1
#define MAX31875_PORT_SCL		P6_0

MAX31875_Status MAX31875_PlatformSpecific_Init();
MAX31875_Status MAX31875_PlatformSpecific_Deinit();
MAX31875_Status MAX31875_PlatformSpecific_Read(MAX31875_Device* dev, uint8_t registerAddress, uint8_t* buffer, size_t bufferSize);
MAX31875_Status MAX31875_PlatformSpecific_Write(MAX31875_Device* dev, uint8_t registerAddress, uint8_t* buffer, size_t bufferSize);

#endif
