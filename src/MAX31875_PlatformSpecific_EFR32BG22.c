#include "MAX31875_PlatformSpecific.h"
#include "MAX31875.h"

#include <string.h>

#include "em_i2c.h"
#include "em_cmu.h"

#include "FreeRTOS.h"
#include "task.h"

MAX31875_Status MAX31875_PlatformSpecific_Init() {
	CMU_ClockEnable(MAX31875_I2C_PERIPHERAL_CLOCK, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_HFRCO0, true);
	CMU_ClockEnable(cmuClock_LFXO, true);

	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

	GPIO_PinModeSet(MAX31875_I2C_SDA_PORT, MAX31875_I2C_SDA_PIN, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(MAX31875_I2C_SCL_PORT, MAX31875_I2C_SCL_PIN, gpioModeWiredAndPullUpFilter, 1);

	GPIO->I2CROUTE[MAX31875_I2C_INSTANCE_NO].SDAROUTE = MAX31875_I2C_SDA_PORT | (MAX31875_I2C_SDA_PIN << 16);
	GPIO->I2CROUTE[MAX31875_I2C_INSTANCE_NO].SCLROUTE = MAX31875_I2C_SCL_PORT | (MAX31875_I2C_SCL_PIN << 16);
	GPIO->I2CROUTE[MAX31875_I2C_INSTANCE_NO].ROUTEEN = 0x03;

	I2C_Reset(MAX31875_I2C_INSTANCE);
	I2C_Init_TypeDef config = I2C_INIT_DEFAULT;
	I2C_Init(MAX31875_I2C_INSTANCE, &config);

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_PlatformSpecific_Deinit() {
	I2C_Enable(MAX31875_I2C_INSTANCE, 0);
	return MAX31875_Status_Ok;
}

static MAX31875_Status MAX31875_PlatformSpecific_TransmitAndWait(I2C_TransferSeq_TypeDef* transferSequence) {
	I2C_TransferReturn_TypeDef tStatus;
	uint32_t end = xTaskGetTickCount() + pdMS_TO_TICKS(MAX31875_I2C_TIMEOUT_MS);

	tStatus = I2C_TransferInit(MAX31875_I2C_INSTANCE, transferSequence);
	if (tStatus != i2cTransferInProgress) {
		I2C_Reset(MAX31875_I2C_INSTANCE);
		return MAX31875_Status_I2CError;
	}

	do {
		tStatus = I2C_Transfer(MAX31875_I2C_INSTANCE);

		if (tStatus == i2cTransferInProgress) {
			taskYIELD();
		}
	} while (tStatus == i2cTransferInProgress && xTaskGetTickCount() < end);

	if (tStatus == i2cTransferInProgress) {
		I2C_Reset(MAX31875_I2C_INSTANCE);
		return MAX31875_Status_I2CTimeout;
	} else if (tStatus == i2cTransferNack) {
		I2C_Reset(MAX31875_I2C_INSTANCE);
		return MAX31875_Status_I2CNack;
	} else if (tStatus) {
		I2C_Reset(MAX31875_I2C_INSTANCE);
		return MAX31875_Status_I2CError;
	}

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_PlatformSpecific_Read(MAX31875_Device *dev, uint8_t registerAddress, uint8_t *buffer, size_t bufferSize) {
	I2C_TransferSeq_TypeDef seq;
	seq.addr = MAX31875_I2C_WRITE_ADDRESS(dev);
	seq.flags = I2C_FLAG_WRITE_READ;
	seq.buf[0].data = &registerAddress;
	seq.buf[0].len = sizeof(registerAddress);
	seq.buf[1].data = buffer;
	seq.buf[1].len = bufferSize;

	return MAX31875_PlatformSpecific_TransmitAndWait(&seq);
}

MAX31875_Status MAX31875_PlatformSpecific_Write(MAX31875_Device *dev, uint8_t registerAddress, uint8_t *buffer, size_t bufferSize) {
	uint8_t message[16];

	if (bufferSize > 3) {
		return MAX31875_Status_BadArg;
	}

	message[0] = registerAddress;
	memcpy(message + 1, buffer, bufferSize);

	I2C_TransferSeq_TypeDef seq;
	seq.addr = MAX31875_I2C_WRITE_ADDRESS(dev);
	seq.flags = I2C_FLAG_WRITE;
	seq.buf[0].data = message;
	seq.buf[0].len = bufferSize + 1;

	return MAX31875_PlatformSpecific_TransmitAndWait(&seq);
}
