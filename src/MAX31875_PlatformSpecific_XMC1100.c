#include "MAX31875_PlatformSpecific.h"
#include "MAX31875.h"

#include "XMC1100.h"
#include "xmc_i2c.h"

MAX31875_Status MAX31875_PlatformSpecific_Init() {
	// Initialization is supposed to be done using cyhal_init
	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_PlatformSpecific_Deinit() {
	// deinitialization is supposed to be done using cyhal
	return MAX31875_Status_NotImplemented;
}

static MAX31875_Status MAX31875_PlatformSpecific_WaitForAck() {
	uint32_t timeout = 250;

	uint32_t tragetFlags = XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED | XMC_I2C_CH_STATUS_FLAG_NACK_RECEIVED;

	while ((XMC_I2C_CH_GetStatusFlag(MAX31875_I2C_INTERFACE) & tragetFlags) == 0 && timeout > 0) {
		timeout--;
		XMC_Delay(1);
	}
	if (timeout == 0) {
		return MAX31875_Status_I2CTimeout;
	}
	uint32_t status = XMC_I2C_CH_GetStatusFlag(MAX31875_I2C_INTERFACE);
	if (status & XMC_I2C_CH_STATUS_FLAG_NACK_RECEIVED) {
		return MAX31875_Status_I2CNack;
	} else if ((status & XMC_I2C_CH_STATUS_FLAG_ACK_RECEIVED) == 0) {
		return MAX31875_Status_I2CError;
	}

	XMC_I2C_CH_ClearStatusFlag(MAX31875_I2C_INTERFACE, tragetFlags);

	return MAX31875_Status_Ok;
}

static MAX31875_Status MAX31875_PlatformSpecific_WaitForReception() {
	uint32_t timeout = 250;

	uint32_t tragetFlags = XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION;

	while ((XMC_I2C_CH_GetStatusFlag(MAX31875_I2C_INTERFACE) & tragetFlags) == 0 && timeout > 0) {
		timeout--;
		XMC_Delay(1);
	}
	if (timeout == 0) {
		return MAX31875_Status_I2CTimeout;
	}
	uint32_t status = XMC_I2C_CH_GetStatusFlag(MAX31875_I2C_INTERFACE);
	if (!(status & XMC_I2C_CH_STATUS_FLAG_RECEIVE_INDICATION) &&!(status & XMC_I2C_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION)) {
		return MAX31875_Status_I2CError;
	}

	XMC_I2C_CH_ClearStatusFlag(MAX31875_I2C_INTERFACE, tragetFlags);

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_PlatformSpecific_Read(MAX31875_Device *dev, uint8_t registerAddress, uint8_t *buffer, size_t bufferSize) {
	MAX31875_Status status;

	XMC_I2C_CH_ClearStatusFlag(MAX31875_I2C_INTERFACE, 0x1FFFF);

	XMC_I2C_CH_MasterStart(MAX31875_I2C_INTERFACE, MAX31875_I2C_WRITE_ADDRESS(dev), XMC_I2C_CH_CMD_WRITE);
	status = MAX31875_PlatformSpecific_WaitForAck();
	if (status) {
		XMC_I2C_CH_MasterStop(MAX31875_I2C_INTERFACE);
		return status;
	}

	XMC_I2C_CH_MasterTransmit(MAX31875_I2C_INTERFACE, registerAddress);
	status = MAX31875_PlatformSpecific_WaitForAck();
	if (status) {
		XMC_I2C_CH_MasterStop(MAX31875_I2C_INTERFACE);
		return status;
	}

	XMC_I2C_CH_MasterRepeatedStart(MAX31875_I2C_INTERFACE, MAX31875_I2C_READ_ADDRESS(dev), XMC_I2C_CH_CMD_READ);
	status = MAX31875_PlatformSpecific_WaitForAck();
	if (status) {
		XMC_I2C_CH_MasterStop(MAX31875_I2C_INTERFACE);
		return status;
	}

	while (bufferSize--) {
		if (bufferSize == 0) {
			XMC_I2C_CH_MasterReceiveNack(MAX31875_I2C_INTERFACE);
		} else {
			XMC_I2C_CH_MasterReceiveAck(MAX31875_I2C_INTERFACE);
		}
		status = MAX31875_PlatformSpecific_WaitForReception();
		if (status) {
			XMC_I2C_CH_MasterStop(MAX31875_I2C_INTERFACE);
			return status;
		}
		*buffer++ = XMC_I2C_CH_GetReceivedData(MAX31875_I2C_INTERFACE);
	}

	XMC_I2C_CH_MasterStop(MAX31875_I2C_INTERFACE);

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_PlatformSpecific_Write(MAX31875_Device *dev, uint8_t registerAddress, uint8_t *buffer, size_t bufferSize) {
	MAX31875_Status status;

	XMC_I2C_CH_ClearStatusFlag(MAX31875_I2C_INTERFACE, 0x1FFFF);
	XMC_I2C_CH_MasterStart(MAX31875_I2C_INTERFACE, MAX31875_I2C_WRITE_ADDRESS(dev), XMC_I2C_CH_CMD_WRITE);
	status = MAX31875_PlatformSpecific_WaitForAck();
	if (status) {
		XMC_I2C_CH_MasterStop(MAX31875_I2C_INTERFACE);
		return status;
	}

	XMC_I2C_CH_MasterTransmit(MAX31875_I2C_INTERFACE, registerAddress);
	status = MAX31875_PlatformSpecific_WaitForAck();
	if (status) {
		XMC_I2C_CH_MasterStop(MAX31875_I2C_INTERFACE);
		return status;
	}

	while (bufferSize--) {
		XMC_I2C_CH_MasterTransmit(MAX31875_I2C_INTERFACE, *buffer++);
		status = MAX31875_PlatformSpecific_WaitForAck();
		if (status) {
			XMC_I2C_CH_MasterStop(MAX31875_I2C_INTERFACE);
			return status;
		}
	}

	XMC_I2C_CH_MasterStop(MAX31875_I2C_INTERFACE);

	return MAX31875_Status_Ok;
}
