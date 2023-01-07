#include "MAX31875.h"
#include "MAX31875_PlatformSpecific.h"

static int initializedDevices = 0;

static const uint8_t MAX31875_Crc8Table[] = { 0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D, 0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48,
		0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D, 0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD, 0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8,
		0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD, 0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA, 0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F,
		0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A, 0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A, 0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F,
		0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A, 0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4, 0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1,
		0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4, 0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44, 0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21,
		0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34, 0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63, 0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06,
		0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13, 0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83, 0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6,
		0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3 };

MAX31875_Status MAX31875_Init(MAX31875_Device *dev, uint8_t i2cAddress) {
	MAX31875_Status status;

	if (initializedDevices == 0) {
		status = MAX31875_PlatformSpecific_Init();
		if (status) {
			return status;
		}
	}

	dev->i2cAddress = i2cAddress;

	// at pwoerup we dont know if PEC is enabled or disabled. FOor preventing possible
	// fails in MAX31875_GetConfiguration, we will not expect them.
	dev->currentConfig.pecMode = 0;

	status = MAX31875_GetConfiguration(dev, &dev->currentConfig);
	if (status) {
		return status;
	}

	initializedDevices++;

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_Deinit(MAX31875_Device *dev) {
	(void)dev;

	initializedDevices--;

	if (initializedDevices == 0) {
		return MAX31875_PlatformSpecific_Deinit();
	}

	return MAX31875_Status_Ok;
}

static uint8_t MAX31875_CalculateCrc(uint8_t *buffer, size_t bufferSize) {
	uint8_t crc = 0;

	while (bufferSize--) {
		crc = MAX31875_Crc8Table[(*buffer++) ^ crc];
	}

	return crc;
}

static MAX31875_Status MAX31875_CheckCrc(uint8_t *buffer, size_t bufferSize, uint8_t receivedCrc) {
	if (MAX31875_CalculateCrc(buffer, bufferSize) != receivedCrc) {
		return MAX31875_Status_PacketErrorCheckFailed;
	} else {
		return MAX31875_Status_Ok;
	}
}

static MAX31875_Status MAX31875_ReadRegister(MAX31875_Device *dev, uint8_t registerAddress, uint16_t *registerValue) {
	MAX31875_Status status;

	// devAddr+WR (1 Byte) + registerAddress (1 Byte) + devAddr+RD (1 Byte) + registerValue (2 Bytes) + CRC (1 Byte)
	uint8_t crcBuffer[6];

	size_t readSize = sizeof(uint16_t);
	if (dev->currentConfig.pecMode == MAX31875_PacketErrorCheckingMode_Enabled) {
		readSize++;
	}

	crcBuffer[0] = MAX31875_I2C_WRITE_ADDRESS(dev);
	crcBuffer[1] = registerAddress;
	crcBuffer[2] = MAX31875_I2C_READ_ADDRESS(dev);

	status = MAX31875_PlatformSpecific_Read(dev, registerAddress, crcBuffer + 3, readSize);
	if (status) {
		return status;
	}

	if (dev->currentConfig.pecMode == MAX31875_PacketErrorCheckingMode_Enabled) {
		// checking input: devAddr+WR (1 Byte) + registerAddress (1 Byte) + devAddr+RD (1 Byte) + registerValue (2 Bytes)
		status = MAX31875_CheckCrc(crcBuffer, 5, crcBuffer[5]);
		if (status) {
			return status;
		}
	}

	*registerValue = (((uint16_t) crcBuffer[3]) << 8) | (((uint16_t) crcBuffer[4]) << 0);

	return MAX31875_Status_Ok;
}

static MAX31875_Status MAX31875_WriteRegister(MAX31875_Device *dev, uint8_t registerAddress, uint16_t registerValue) {
	MAX31875_Status status;
	uint8_t crcBuffer[5]; // devAddr+WR (1 Byte) + registerAddress (1 Byte) + registerValue (2 Bytes) + CRC (1 Byte)

	crcBuffer[0] = MAX31875_I2C_WRITE_ADDRESS(dev);
	crcBuffer[1] = registerAddress;
	crcBuffer[2] = (registerValue & 0xFF00) >> 8;
	crcBuffer[3] = registerValue & 0xFF;

	size_t writeSize = sizeof(uint16_t);

	if (dev->currentConfig.pecMode == MAX31875_PacketErrorCheckingMode_Enabled) {
		crcBuffer[4] = MAX31875_CalculateCrc(crcBuffer, 4);
		writeSize++;
	}

	status = MAX31875_PlatformSpecific_Write(dev, registerAddress, crcBuffer + 2, writeSize);
	if (status) {
		return status;
	}

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_GetConfiguration(MAX31875_Device *dev, MAX31875_Configuration *config) {
	MAX31875_Status status;
	uint16_t configRegVal;

	status = MAX31875_ReadRegister(dev, MAX31875_REG_CONFIGURATION, &configRegVal);
	if (status) {
		return status;
	}

	config->conversionRate = MAX31875_GET_FIELD(MAX31875_CONFIG_CONVERSION_RATE_FIELD, configRegVal);
	config->pecMode = MAX31875_GET_FIELD(MAX31875_CONFIG_PEC_FIELD, configRegVal);
	config->i2cTimeout = MAX31875_GET_FIELD(MAX31875_CONFIG_TIMEOUT_FIELD, configRegVal);
	config->resolution = MAX31875_GET_FIELD(MAX31875_CONFIG_RESOLUTION_FIELD, configRegVal);
	config->dataFormat = MAX31875_GET_FIELD(MAX31875_CONFIG_DATA_FORMAT_FIELD, configRegVal);
	config->shutdownMode = MAX31875_GET_FIELD(MAX31875_CONFIG_SHUTDOWN_FIELD, configRegVal);
	config->comparatorMode = MAX31875_GET_FIELD(MAX31875_CONFIG_COMPARATOR_INTERRUPT_FIELD, configRegVal);
	config->faultQueueLevel = MAX31875_GET_FIELD(MAX31875_CONFIG_FAULT_QUEUE_FIELD, configRegVal);

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_GetDefaultConfiguration(MAX31875_Configuration *config) {
	config->conversionRate = MAX31875_ConversionRate_0_25_PerSecond;
	config->pecMode = MAX31875_PacketErrorCheckingMode_Disabled;
	config->i2cTimeout = MAX31875_I2CTimeoutMode_Enabled;
	config->resolution = MAX31875_Resolution_10bit;
	config->dataFormat = MAX31875_DataFormat_Normal;
	config->shutdownMode = MAX31875_ShutdownMode_Disabled;
	config->comparatorMode = MAX31875_ComparatorMode_ComparatorMode;
	config->faultQueueLevel = MAX31875_FaultQueueLevel_1_Fault;

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_SetConfiguration(MAX31875_Device *dev, MAX31875_Configuration *config) {
	MAX31875_Status status;

	MAX31875_PacketErrorCheckingMode pecBackup = dev->currentConfig.pecMode;

	if (dev->currentConfig.pecMode == MAX31875_PacketErrorCheckingMode_Enabled || config->pecMode == MAX31875_PacketErrorCheckingMode_Enabled) {

		dev->currentConfig.pecMode = MAX31875_PacketErrorCheckingMode_Enabled;
	}

	uint16_t configRegVal =
	MAX31875_SET_FIELD(MAX31875_CONFIG_CONVERSION_RATE_FIELD, config->conversionRate) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_PEC_FIELD, config->pecMode) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_TIMEOUT_FIELD, config->i2cTimeout) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_RESOLUTION_FIELD, config->resolution) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_DATA_FORMAT_FIELD, config->dataFormat) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_SHUTDOWN_FIELD, config->shutdownMode) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_COMPARATOR_INTERRUPT_FIELD, config->comparatorMode) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_FAULT_QUEUE_FIELD, config->faultQueueLevel);

	status = MAX31875_WriteRegister(dev, MAX31875_REG_CONFIGURATION, configRegVal);
	if (status) {
		dev->currentConfig.pecMode = pecBackup;
		return status;
	}

	dev->currentConfig.conversionRate = config->conversionRate;
	dev->currentConfig.pecMode = config->pecMode;
	dev->currentConfig.i2cTimeout = config->i2cTimeout;
	dev->currentConfig.resolution = config->resolution;
	dev->currentConfig.dataFormat = config->dataFormat;
	dev->currentConfig.shutdownMode = config->shutdownMode;
	dev->currentConfig.comparatorMode = config->comparatorMode;
	dev->currentConfig.faultQueueLevel = config->faultQueueLevel;

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_TriggerSingleShotConversion(MAX31875_Device *dev) {
	uint16_t configRegVal =
	MAX31875_SET_FIELD(MAX31875_CONFIG_CONVERSION_RATE_FIELD, dev->currentConfig.conversionRate) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_PEC_FIELD, dev->currentConfig.pecMode) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_TIMEOUT_FIELD, dev->currentConfig.i2cTimeout) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_RESOLUTION_FIELD, dev->currentConfig.resolution) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_DATA_FORMAT_FIELD, dev->currentConfig.dataFormat) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_SHUTDOWN_FIELD, dev->currentConfig.shutdownMode) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_COMPARATOR_INTERRUPT_FIELD, dev->currentConfig.comparatorMode) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_FAULT_QUEUE_FIELD, dev->currentConfig.faultQueueLevel) |
	MAX31875_SET_FIELD(MAX31875_CONFIG_ONE_SHOT_FIELD, 1);

	return MAX31875_WriteRegister(dev, MAX31875_REG_CONFIGURATION, configRegVal);
}

MAX31875_Status MAX31875_ReadRawTemperature(MAX31875_Device *dev, uint16_t *rawTemperature) {
	return MAX31875_ReadRegister(dev, MAX31875_REG_TEMPERATURE, rawTemperature);
}

static float MAX31875_ConvertRawTemperatureToFloat(MAX31875_Device *dev, uint16_t rawTemperature) {
	uint16_t sign = rawTemperature >> 15;
	float temperature;
	if (dev->currentConfig.dataFormat == MAX31875_DataFormat_Normal) {
		temperature = ((rawTemperature & 0x7FFF) >> 4);
	} else {
		temperature = ((rawTemperature & 0x7FFF) >> 3);
	}
	temperature *= 0.0625f;
	if (sign) {
		temperature = -temperature;
	}
	return temperature;
}

static uint16_t MAX31875_ConvertFloatTemperatureToRaw(MAX31875_Device *dev, float temperature) {
	int sign = temperature < 0;
	int temperatureLsbsLower = (int) (temperature / 0.0625);
	int temperatureLsbsUpper = temperatureLsbsLower + (sign ? -1 : 1);
	float temperatureLower = temperatureLsbsLower * 0.0625f;
	float temperatureUpper = temperatureLower + (sign ? -0.0625f : 0.0625f);
	float diffLower = temperature - temperatureLower;
	float diffUpper = temperatureUpper - temperature;
	int targetLsbs;

	if (diffLower < diffUpper) {
		targetLsbs = temperatureLsbsLower;
	} else {
		targetLsbs = temperatureLsbsUpper;
	}

	if (dev->currentConfig.dataFormat == MAX31875_DataFormat_Normal) {
		targetLsbs <<= 4;
	} else {
		targetLsbs <<= 3;
	}

	return (uint16_t) (sign << 15) | targetLsbs;
}

MAX31875_Status MAX31875_ReadTemperature(MAX31875_Device *dev, float *temperature) {
	uint16_t rawTemperature;
	MAX31875_Status status;

	status = MAX31875_ReadRawTemperature(dev, &rawTemperature);
	if (status) {
		return status;
	}

	*temperature = MAX31875_ConvertRawTemperatureToFloat(dev, rawTemperature);

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_GetOverTemperatureStatus(MAX31875_Device *dev, int *isOverTemperature) {
	MAX31875_Status status;
	uint16_t configRegVal;

	status = MAX31875_ReadRegister(dev, MAX31875_REG_CONFIGURATION, &configRegVal);
	if (status) {
		return status;
	}

	*isOverTemperature = MAX31875_GET_FIELD(MAX31875_CONFIG_OVER_T_STATUS_FIELD, configRegVal);

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_GetHysteresisTreshold(MAX31875_Device *dev, float *temperatureHysteresis) {
	uint16_t rawTemperature;
	MAX31875_Status status;

	status = MAX31875_GetRawHysteresisTreshold(dev, &rawTemperature);
	if (status) {
		return status;
	}

	*temperatureHysteresis = MAX31875_ConvertRawTemperatureToFloat(dev, rawTemperature);

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_SetHysteresisTreshold(MAX31875_Device *dev, float temperatureHysteresis) {
	uint16_t rawTemperature = MAX31875_ConvertFloatTemperatureToRaw(dev, temperatureHysteresis);
	return MAX31875_SetRawHysteresisTreshold(dev, rawTemperature);
}

MAX31875_Status MAX31875_GetRawHysteresisTreshold(MAX31875_Device *dev, uint16_t *temperatureHysteresis) {
	return MAX31875_ReadRegister(dev, MAX31875_REG_T_HYST, temperatureHysteresis);
}

MAX31875_Status MAX31875_SetRawHysteresisTreshold(MAX31875_Device *dev, uint16_t temperatureHysteresis) {
	return MAX31875_WriteRegister(dev, MAX31875_REG_T_HYST, temperatureHysteresis);
}

MAX31875_Status MAX31875_GetOverTemperatureTreshold(MAX31875_Device *dev, float *overTemperatureLevel) {
	uint16_t rawTemperature;
	MAX31875_Status status;

	status = MAX31875_GetRawOverTemperatureTreshold(dev, &rawTemperature);
	if (status) {
		return status;
	}

	*overTemperatureLevel = MAX31875_ConvertRawTemperatureToFloat(dev, rawTemperature);

	return MAX31875_Status_Ok;

}

MAX31875_Status MAX31875_SetOverTemperatureTreshold(MAX31875_Device *dev, float overTemperatureLevel) {
	uint16_t rawTemperature = MAX31875_ConvertFloatTemperatureToRaw(dev, overTemperatureLevel);
	return MAX31875_SetRawOverTemperatureTreshold(dev, rawTemperature);
}

MAX31875_Status MAX31875_GetRawOverTemperatureTreshold(MAX31875_Device *dev, uint16_t *overTemperatureLevel) {
	return MAX31875_ReadRegister(dev, MAX31875_REG_T_OS, overTemperatureLevel);
}

MAX31875_Status MAX31875_SetRawOverTemperatureTreshold(MAX31875_Device *dev, uint16_t overTemperatureLevel) {
	return MAX31875_WriteRegister(dev, MAX31875_REG_T_OS, overTemperatureLevel);
}
