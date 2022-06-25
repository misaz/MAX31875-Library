#ifndef MAX31875_H_
#define MAX31875_H_

#include <stdint.h>
#include <stddef.h>

#define MAX31875_I2C_7BIT_ADDRESS_DEFAULT			0x48
#define MAX31875_I2C_7BIT_ADDRESS_R1				0x49
#define MAX31875_I2C_7BIT_ADDRESS_R2				0x4A
#define MAX31875_I2C_7BIT_ADDRESS_R3				0x4B
#define MAX31875_I2C_7BIT_ADDRESS_R4				0x4C
#define MAX31875_I2C_7BIT_ADDRESS_R5				0x4D
#define MAX31875_I2C_7BIT_ADDRESS_R6				0x4E
#define MAX31875_I2C_7BIT_ADDRESS_R7				0x4F

#define MAX31875_I2C_READ_ADDRESS(dev)				(((dev)->i2cAddress << 1) | 0x01)
#define MAX31875_I2C_WRITE_ADDRESS(dev)				(((dev)->i2cAddress << 1) | 0x00)

#define MAX31875_REG_TEMPERATURE					0x00
#define MAX31875_REG_CONFIGURATION					0x01
#define MAX31875_REG_T_HYST							0x02
#define MAX31875_REG_T_OS							0x03

#define MAX31875_EXPAND_MACRO(content)				content
#define MAX31875_FIELD(NAME)						NAME##_MASK, NAME##_OFFSET
#define MAX31875_GET_FIELD_IMPL(FIELD_MASK,FIELD_OFFSET,value)	(((value) >> FIELD_OFFSET) & FIELD_MASK)
#define MAX31875_SET_FIELD_IMPL(FIELD_MASK,FIELD_OFFSET,value)	(((value) & FIELD_MASK) << FIELD_OFFSET)
#define MAX31875_GET_FIELD(...)						MAX31875_EXPAND_MACRO(MAX31875_GET_FIELD_IMPL(__VA_ARGS__))
#define MAX31875_SET_FIELD(...)						MAX31875_EXPAND_MACRO(MAX31875_SET_FIELD_IMPL(__VA_ARGS__))

#define MAX31875_CONFIG_ONE_SHOT_MASK				0x1
#define MAX31875_CONFIG_ONE_SHOT_OFFSET				0
#define MAX31875_CONFIG_ONE_SHOT_FIELD				MAX31875_FIELD(MAX31875_CONFIG_ONE_SHOT)
#define MAX31875_CONFIG_CONVERSION_RATE_MASK		0x3
#define MAX31875_CONFIG_CONVERSION_RATE_OFFSET		1
#define MAX31875_CONFIG_CONVERSION_RATE_FIELD		MAX31875_FIELD(MAX31875_CONFIG_CONVERSION_RATE)
#define MAX31875_CONFIG_PEC_MASK					0x1
#define MAX31875_CONFIG_PEC_OFFSET					3
#define MAX31875_CONFIG_PEC_FIELD					MAX31875_FIELD(MAX31875_CONFIG_PEC)
#define MAX31875_CONFIG_TIMEOUT_MASK				0x1
#define MAX31875_CONFIG_TIMEOUT_OFFSET				4
#define MAX31875_CONFIG_TIMEOUT_FIELD				MAX31875_FIELD(MAX31875_CONFIG_TIMEOUT)
#define MAX31875_CONFIG_RESOLUTION_MASK				0x3
#define MAX31875_CONFIG_RESOLUTION_OFFSET			5
#define MAX31875_CONFIG_RESOLUTION_FIELD			MAX31875_FIELD(MAX31875_CONFIG_RESOLUTION)
#define MAX31875_CONFIG_DATA_FORMAT_MASK			0x1
#define MAX31875_CONFIG_DATA_FORMAT_OFFSET			7
#define MAX31875_CONFIG_DATA_FORMAT_FIELD			MAX31875_FIELD(MAX31875_CONFIG_DATA_FORMAT)
#define MAX31875_CONFIG_SHUTDOWN_MASK				0x1
#define MAX31875_CONFIG_SHUTDOWN_OFFSET				8
#define MAX31875_CONFIG_SHUTDOWN_FIELD				MAX31875_FIELD(MAX31875_CONFIG_SHUTDOWN)
#define MAX31875_CONFIG_COMPARATOR_INTERRUPT_MASK	0x1
#define MAX31875_CONFIG_COMPARATOR_INTERRUPT_OFFSET 9
#define MAX31875_CONFIG_COMPARATOR_INTERRUPT_FIELD	MAX31875_FIELD(MAX31875_CONFIG_COMPARATOR_INTERRUPT)
#define MAX31875_CONFIG_FAULT_QUEUE_MASK			0x3
#define MAX31875_CONFIG_FAULT_QUEUE_OFFSET			11
#define MAX31875_CONFIG_FAULT_QUEUE_FIELD			MAX31875_FIELD(MAX31875_CONFIG_FAULT_QUEUE)
#define MAX31875_CONFIG_OVER_T_STATUS_MASK			0x1
#define MAX31875_CONFIG_OVER_T_STATUS_OFFSET		15
#define MAX31875_CONFIG_OVER_T_STATUS_FIELD			MAX31875_FIELD(MAX31875_CONFIG_OVER_T_STATUS)

typedef enum {
	MAX31875_Status_Ok = 0,
	MAX31875_Status_I2CNack = -10,
	MAX31875_Status_I2CError = -11,
	MAX31875_Status_I2CTimeout = -12,
	MAX31875_Status_PacketErrorCheckFailed = -13,
	MAX31875_Status_NotImplemented = -20,
	MAX31875_Status_BadArg = -21,
	MAX31875_Status_InvalidOperation = -22,
	MAX31875_Status_NotSupported = -24,
} MAX31875_Status;

typedef enum {
	MAX31875_ConversionRate_0_25_PerSecond = 0,
	MAX31875_ConversionRate_1_PerSecond = 1,
	MAX31875_ConversionRate_4_PerSecond = 2,
	MAX31875_ConversionRate_8_PerSecond = 3,
} MAX31875_ConversionRate;

typedef enum {
	MAX31875_Resolution_8bit = 0,
	MAX31875_Resolution_9bit = 1,
	MAX31875_Resolution_10bit = 2,
	MAX31875_Resolution_12bit = 3,
} MAX31875_Resolution;

typedef enum {
	MAX31875_PacketErrorCheckingMode_Disabled = 0,
	MAX31875_PacketErrorCheckingMode_Enabled = 1,
} MAX31875_PacketErrorCheckingMode;

typedef enum {
	MAX31875_I2CTimeoutMode_Enabled = 0,
	MAX31875_I2CTimeoutMode_Disabled = 1,
} MAX31875_I2CTimeoutMode;

typedef enum {
	MAX31875_DataFormat_Normal = 0,
	MAX31875_DataFormat_Extended = 1,
} MAX31875_DataFormat;

typedef enum {
	MAX31875_ShutdownMode_Disabled = 0,
	MAX31875_ShutdownMode_Enabled = 1,
} MAX31875_ShutdownMode;

typedef enum {
	MAX31875_ComparatorMode_ComparatorMode = 0,
	MAX31875_ComparatorMode_InterruptMode = 1,
} MAX31875_ComparatorMode;

typedef enum {
	MAX31875_FaultQueueLevel_1_Fault = 0,
	MAX31875_FaultQueueLevel_2_Fault = 1,
	MAX31875_FaultQueueLevel_4_Fault = 2,
	MAX31875_FaultQueueLevel_8_Fault = 3,
} MAX31875_FaultQueueLevel;

typedef struct  {
	MAX31875_ConversionRate conversionRate;
	MAX31875_Resolution resolution;
	MAX31875_PacketErrorCheckingMode pecMode;
	MAX31875_I2CTimeoutMode i2cTimeout;
	MAX31875_DataFormat dataFormat;
	MAX31875_ShutdownMode shutdownMode;
	MAX31875_ComparatorMode comparatorMode;
	MAX31875_FaultQueueLevel faultQueueLevel;
} MAX31875_Configuration;

typedef struct  {
	uint8_t i2cAddress;
	MAX31875_Configuration currentConfig;
} MAX31875_Device;

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

#endif
