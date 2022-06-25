#include "MAX31875_PlatformSpecific.h"
#include "MAX31875.h"

static cyhal_i2c_t i2c;

MAX31875_Status MAX31875_PlatformSpecific_Init() {
	cy_rslt_t status;

	cyhal_gpio_configure(MAX31875_PORT_SCL, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG);
	for (int i = 0; i < 9; i++) {
		cyhal_gpio_write(MAX31875_PORT_SCL, 0);
		cyhal_system_delay_us(5);
		cyhal_gpio_write(MAX31875_PORT_SCL, 1);
		cyhal_system_delay_us(5);
	}

	status = cyhal_i2c_init(&i2c, MAX31875_PORT_SDA, MAX31875_PORT_SCL, NULL);
	if (status) {
		return MAX31875_Status_I2CError;
	}

	return MAX31875_Status_Ok;

}

MAX31875_Status MAX31875_PlatformSpecific_Deinit() {
	cyhal_i2c_free(&i2c);

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_PlatformSpecific_Read(MAX31875_Device* dev, uint8_t registerAddress, uint8_t* buffer, size_t bufferSize) {
	cy_rslt_t status;

	status = cyhal_i2c_master_mem_read(&i2c, dev->i2cAddress, registerAddress, 1, buffer, bufferSize, 250);
	if (status == CY_SCB_I2C_MASTER_MANUAL_TIMEOUT) {
		return MAX31875_Status_I2CTimeout;
	} else if (status == CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK || status == CY_SCB_I2C_MASTER_MANUAL_NAK) {
		return MAX31875_Status_I2CNack;
	} else if (status) {
		return MAX31875_Status_I2CError;
	}

	return MAX31875_Status_Ok;
}

MAX31875_Status MAX31875_PlatformSpecific_Write(MAX31875_Device* dev, uint8_t registerAddress, uint8_t* buffer, size_t bufferSize) {
	cy_rslt_t status;

	status = cyhal_i2c_master_mem_write(&i2c, dev->i2cAddress, registerAddress, 1, buffer, bufferSize, 250);
	if (status == CY_SCB_I2C_MASTER_MANUAL_TIMEOUT) {
		return MAX31875_Status_I2CTimeout;
	} else if (status == CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK || status == CY_SCB_I2C_MASTER_MANUAL_NAK) {
		return MAX31875_Status_I2CNack;
	} else if (status) {
		return MAX31875_Status_I2CError;
	}

	return MAX31875_Status_Ok;
}
