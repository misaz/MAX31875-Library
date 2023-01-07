#include "MAX31875_PlatformSpecific.h"
#include "MAX31875.h"

MAX31875_Status MAX31875_PlatformSpecific_Init() {
	// this function must initialize I2C bus driver. It should not generate any 
	// transaction to the MAX31875 device. It is recommended to generate 9 clock  
	// cycles when SDA line is asserted as low and then generate stop sequence.

	return MAX31875_Status_NotImplemented;
}

MAX31875_Status MAX31875_PlatformSpecific_Deinit() {
	// this function must deinitialize I2C bus driver. It should not generate any 
	// transaction to the MAX31875 device.

	return MAX31875_Status_NotImplemented;
}

MAX31875_Status MAX31875_PlatformSpecific_Read(MAX31875_Device* dev, uint8_t registerAddress, uint8_t* buffer, size_t bufferSize) {
	// this function must generate following on the I2C bus:
	//
	// 1) Generate STRAT condition.
	// 2) Transmit I2C address. If you are using platform (library) which requires
	//    non-shifted 7bit address use dev->i2cAddress and write direction. If you need
	//    to provide 8bit shifted address then use MAX31875_I2C_WRITE_ADDRESS(dev)
	// 3) Check that device ACKed transaction. Otherwise generate STOP sequence and
	//    return MAX31875_Status_I2CNack.
	// 4) Transmit registerAddress byte.
	// 5) Check that device ACKed transaction. Otherwise generate STOP sequence and
	//    return MAX31875_Status_I2CNack.
	// 6) Generate REPEATED START sequence
	// 7) Transmit I2C address. If you are using platform (library) which requires
	//    non-shifted 7bit address use dev->i2cAddress and read direction. If you need
	//    to provide 8bit shifted address then use MAX31875_I2C_READ_ADDRESS(dev)
	// 8) Check that device ACKed transaction. Otherwise generate STOP sequence and
	//    return MAX31875_Status_I2CNack.
	// 9) Read buffer of size bufferSize. After receiving every byte except the last byte
	//    ACK the transaction. Generate NACK instead of ACK after receiving last byte.
	// 7) Generate STOP sequence and return MAX31875_Status_Ok.
	//
	// In all steps check for errors like arbitration lost error. Use return code 
	// MAX31875_Status_I2CNack for errors related to NACKing and MAX31875_Status_I2CError 
	// for other I2C errors. 
	//
	// Before returning any status code remember to generate STOP sequence.
	//
	// Prevent using loops which may end up in inifinete loop in case of unpredictable 
	// behaviour of I2C peripheral. Try implement platform specific timeout detection
	// and return MAX31875_Status_I2CTimeout when timeout occur. In case of timeout 
	// also generate STOP sequence and turn platform specific driver to the state that 
	// it can be reused for further communication again. (In the other words any I2C 
	// error should not make library stuck in error-generating state forever).
}

MAX31875_Status MAX31875_PlatformSpecific_Write(MAX31875_Device* dev, uint8_t registerAddress, uint8_t* buffer, size_t bufferSize) {
	// this function must generate following on the I2C bus:
	//
	// 1) Generate STRAT condition.
	// 2) Transmit I2C address. If you are using platform (library) which requires
	//    non-shifted 7bit address use dev->i2cAddress and write direction. If you need
	//    to provide 8bit shifted address then use MAX31875_I2C_WRITE_ADDRESS(dev)
	// 3) Check that device ACKed transaction. Otherwise generate STOP sequence and 
	//    return MAX31875_Status_I2CNack.
	// 4) transmit registerAddress byte.
	// 5) check that device ACKed transaction. Otherwise generate STOP sequence and
	//    return MAX31875_Status_I2CNack.
	// 6) transmit buffer of size bufferSize. After transmiting every byte check that 
	//    device ACKed transaction. Otherwise generate STOP sequence and return
	//    MAX31875_Status_I2CNack.
	// 7) generate STOP sequence and return MAX31875_Status_Ok.
	//
	// In all steps check for errors like arbitration lost error. Use return code 
	// MAX31875_Status_I2CNack for errors related to NACKing and MAX31875_Status_I2CError 
	// for other I2C errors. 
	//
	// Before returning any status code remember to generate STOP sequence.
	//
	// Prevent using loops which may end up in inifinete loop in case of unpredictable 
	// behaviour of I2C peripheral. Try implement platform specific timeout detection
	// and return MAX31875_Status_I2CTimeout when timeout occur. In case of timeout 
	// also generate STOP sequence and turn platform specific driver to the state that 
	// it can be reused for further communication again. (In the other words any I2C 
	// error should not make library stuck in error-generating state forever).

	return MAX31875_Status_NotImplemented;
}
