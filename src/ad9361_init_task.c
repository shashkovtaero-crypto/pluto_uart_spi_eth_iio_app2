#include <stdint.h>
#include <string.h>

#include "ad9361_api.h"
#include "ad9361_ctrl.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "parameters.h"
#include "xilinx_gpio.h"
#include "xilinx_spi.h"
#include "xil_printf.h"
#include "xgpiops.h"
#include "sleep.h"
#include "ad9161_param.h"

struct ad9361_rf_phy *ad9361_phy = NULL;


static XGpioPs gpio_inst;

void gpio_init(void)
{
    XGpioPs_Config *cfg = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
    XGpioPs_CfgInitialize(&gpio_inst, cfg, cfg->BaseAddr);

    XGpioPs_SetDirectionPin(&gpio_inst, GPIO_RESET_PIN, 1);
    XGpioPs_SetOutputEnablePin(&gpio_inst, GPIO_RESET_PIN, 1);
    XGpioPs_WritePin(&gpio_inst, GPIO_RESET_PIN, 1);

    XGpioPs_SetDirectionPin(&gpio_inst, GPIO_LED_PIN, 1);
    XGpioPs_SetOutputEnablePin(&gpio_inst, GPIO_LED_PIN, 1);
    XGpioPs_WritePin(&gpio_inst, GPIO_LED_PIN, 0);

    XGpioPs_SetDirectionPin(&gpio_inst, GPIO_ENABLE_PIN, 1);
    XGpioPs_SetOutputEnablePin(&gpio_inst, GPIO_ENABLE_PIN, 1);
    XGpioPs_WritePin(&gpio_inst, GPIO_ENABLE_PIN, 0);

    XGpioPs_SetDirectionPin(&gpio_inst, GPIO_TXNRX_PIN, 1);
    XGpioPs_SetOutputEnablePin(&gpio_inst, GPIO_TXNRX_PIN, 1);
    XGpioPs_WritePin(&gpio_inst, GPIO_TXNRX_PIN, 0);
}


static void ad9361_fill_params(void)
{



	default_init_param.gpio_resetb.number = GPIO_RESET_PIN;
	default_init_param.gpio_resetb.platform_ops = &xil_gpio_ops;
	default_init_param.gpio_resetb.extra = &xil_gpio_param;

	default_init_param.spi_param.device_id = SPI_DEVICE_ID;
	default_init_param.spi_param.max_speed_hz = 10000000UL;
	default_init_param.spi_param.chip_select = 0;
	default_init_param.spi_param.mode = NO_OS_SPI_MODE_1;
	default_init_param.spi_param.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST;
	default_init_param.spi_param.platform_ops = &xil_spi_ops;
	default_init_param.spi_param.extra = &xil_spi_param;

	if (AD9364_DEVICE) {
		default_init_param.dev_sel = ID_AD9364;
		tx_dac_init.num_channels = 2;
		tx_dac_init.rate = 1;
		rx_adc_init.num_channels = 2;
		rx_adc_init.num_slave_channels = 0;
	} else {
		if (!default_init_param.two_rx_two_tx_mode_enable) {
			tx_dac_init.num_channels = 2;
			tx_dac_init.rate = 1;
			rx_adc_init.num_channels = 2;
			rx_adc_init.num_slave_channels = 0;
		}
	}
	if (AD9363A_DEVICE)
		default_init_param.dev_sel = ID_AD9363A;
}

void ad9361_no_os_init(void)
{
	/* Hard reset AD9361 */
	XGpioPs_WritePin(&gpio_inst, GPIO_ENABLE_PIN, 0);
	XGpioPs_WritePin(&gpio_inst, GPIO_TXNRX_PIN, 0);
	XGpioPs_WritePin(&gpio_inst, GPIO_RESET_PIN, 0);
	usleep(10000);
	XGpioPs_WritePin(&gpio_inst, GPIO_ENABLE_PIN, 1);
	usleep(1000);
	XGpioPs_WritePin(&gpio_inst, GPIO_RESET_PIN, 1);
	usleep(200000);
    ad9361_fill_params();

    int ret;
    ret=ad9361_init(&ad9361_phy, &default_init_param);
    if (ret<0)
        xil_printf("### ad9361_init error: %d\r\n", ret);

    ret=ad9361_set_tx_fir_config(&ad9361_phy, tx_fir_config);
    if (ret<0)
        xil_printf("### ad9361_set_tx_fir_config error: %d\r\n", ret);

    ret=ad9361_set_rx_fir_config(&ad9361_phy, rx_fir_config);
    if (ret<0)
        xil_printf("### ad9361_set_rx_fir_config error: %d\r\n", ret);
}
