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

struct ad9361_rf_phy *ad9361_phy = NULL;

static struct xil_spi_init_param xil_spi_param = {
    .type = SPI_PS,
    .flags = 0
};

static struct xil_gpio_init_param xil_gpio_param = {
    .type = GPIO_PS,
    .device_id = GPIO_DEVICE_ID
};

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

static AD9361_InitParam g_ad9361_init;

static void ad9361_fill_params(void)
{
    memset(&g_ad9361_init, 0, sizeof(g_ad9361_init));

    g_ad9361_init.reference_clk_rate = 40000000UL;

    g_ad9361_init.two_rx_two_tx_mode_enable = 0;
    g_ad9361_init.frequency_division_duplex_mode_enable = 1;

    g_ad9361_init.rx_synthesizer_frequency_hz = 2400000000ULL;
    g_ad9361_init.tx_synthesizer_frequency_hz = 2400000000ULL;

    g_ad9361_init.rf_rx_bandwidth_hz = 18000000UL;
    g_ad9361_init.rf_tx_bandwidth_hz = 18000000UL;

    g_ad9361_init.gc_rx1_mode = RF_GAIN_SLOWATTACK_AGC;
    g_ad9361_init.gc_rx2_mode = RF_GAIN_SLOWATTACK_AGC;

    g_ad9361_init.rx_path_clock_frequencies[0] = 983040000UL;
    g_ad9361_init.rx_path_clock_frequencies[1] = 245760000UL;
    g_ad9361_init.rx_path_clock_frequencies[2] = 122880000UL;
    g_ad9361_init.rx_path_clock_frequencies[3] = 61440000UL;
    g_ad9361_init.rx_path_clock_frequencies[4] = 30720000UL;
    g_ad9361_init.rx_path_clock_frequencies[5] = 30720000UL;

    g_ad9361_init.tx_path_clock_frequencies[0] = 983040000UL;
    g_ad9361_init.tx_path_clock_frequencies[1] = 245760000UL;
    g_ad9361_init.tx_path_clock_frequencies[2] = 122880000UL;
    g_ad9361_init.tx_path_clock_frequencies[3] = 61440000UL;
    g_ad9361_init.tx_path_clock_frequencies[4] = 30720000UL;
    g_ad9361_init.tx_path_clock_frequencies[5] = 30720000UL;

    g_ad9361_init.gpio_resetb.number = GPIO_RESET_PIN;
    g_ad9361_init.gpio_resetb.platform_ops = &xil_gpio_ops;
    g_ad9361_init.gpio_resetb.extra = &xil_gpio_param;

    g_ad9361_init.spi_param.device_id = SPI_DEVICE_ID;
    g_ad9361_init.spi_param.max_speed_hz = 10000000UL;
    g_ad9361_init.spi_param.chip_select = 0;
    g_ad9361_init.spi_param.mode = NO_OS_SPI_MODE_1;
    g_ad9361_init.spi_param.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST;
    g_ad9361_init.spi_param.platform_ops = &xil_spi_ops;
    g_ad9361_init.spi_param.extra = &xil_spi_param;
}

int ad9361_no_os_init(void)
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
    return ad9361_init(&ad9361_phy, &g_ad9361_init);
}
