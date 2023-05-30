#ifndef MAX31875_PLATFORMSPECIFIC_H_
#define MAX31875_PLATFORMSPECIFIC_H_

#include "MAX31875.h"

#include "cybsp.h"

#include <stdint.h>
#include <stddef.h>

#define MAX31875_I2C_INTERFACE		usic_0_ch_1_HW

MAX31875_Status MAX31875_PlatformSpecific_Init();
MAX31875_Status MAX31875_PlatformSpecific_Deinit();
MAX31875_Status MAX31875_PlatformSpecific_Read(MAX31875_Device* dev, uint8_t registerAddress, uint8_t* buffer, size_t bufferSize);
MAX31875_Status MAX31875_PlatformSpecific_Write(MAX31875_Device* dev, uint8_t registerAddress, uint8_t* buffer, size_t bufferSize);

#endif
