#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "MAX31875.h"

int main(void) {
	cy_rslt_t cyStatus;
	MAX31875_Status mStatus;

	cyStatus = cybsp_init();
	CY_ASSERT(cyStatus == CY_RSLT_SUCCESS);

	cyStatus = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, 115200);
	CY_ASSERT(cyStatus == CY_RSLT_SUCCESS);

	MAX31875_Device m2;
	mStatus = MAX31875_Init(&m2, MAX31875_I2C_7BIT_ADDRESS_R2);
	if (mStatus) {
		printf("Error while initalizing MAX31875 temperature sensor. Error code: %d\r\n", mStatus);
		__BKPT(0);
		while (1) {
			__WFI();
		}
	}

	MAX31875_Configuration cfg;
	MAX31875_GetDefaultConfiguration(&m2, &cfg);
	cfg.conversionRate = MAX31875_ConversionRate_4_PerSecond;
	cfg.resolution = MAX31875_Resolution_12bit;
	cfg.dataFormat = MAX31875_DataFormat_Extended;
	cfg.pecMode = MAX31875_PacketErrorCheckingMode_Enabled;

	mStatus = MAX31875_SetConfiguration(&m2, &cfg);
	if (mStatus) {
		printf("Error while setting MAX31875 configuration. Error code: %d\r\n", mStatus);
		__BKPT(0);
		while (1) {
			__WFI();
		}
	}

	mStatus = MAX31875_SetOverTemperatureTreshold(&m2, 80);
	if (mStatus) {
		printf("Error while reading MAX31875 temperature. Error code: %d\r\n", mStatus);
		__BKPT(0);
		while (1) {
			__WFI();
		}
	}

	mStatus = MAX31875_SetHysteresisTreshold(&m2, 60);
	if (mStatus) {
		printf("Error while reading MAX31875 temperature. Error code: %d\r\n", mStatus);
		__BKPT(0);
		while (1) {
			__WFI();
		}
	}

	while (1) {
		float temperature;
		int isOverTemperature;

		mStatus = MAX31875_ReadTemperature(&m2, &temperature);
		if (mStatus) {
			printf("Error while reading MAX31875 temperature. Error code: %d\r\n", mStatus);
			continue;
		}

		mStatus = MAX31875_GetOverTemperatureStatus(&m2, &isOverTemperature);
		if (mStatus) {
			printf("Error while reading MAX31875 temperature. Error code: %d\r\n", mStatus);
			continue;
		}

        cyhal_system_delay_ms(250);

		printf("Temperature: %.4f%s\r\n", temperature, isOverTemperature ? " OVER-TEMPERATURE" : "");
	}
}
