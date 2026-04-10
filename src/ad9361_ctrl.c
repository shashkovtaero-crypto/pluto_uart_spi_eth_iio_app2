#include <stdint.h>

#include "ad9361_api.h"
#include "ad9361_ctrl.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "xil_printf.h"

extern struct ad9361_rf_phy *ad9361_phy;

int ad9361_lowlevel_axi_init(void)
{
    AXI_AD9361_REG_RSTN = 0x00U;
    no_os_mdelay(1);
    AXI_AD9361_REG_RSTN = 0x03U;
    no_os_mdelay(1);
    AXI_AD9361_REG_CNTRL &= ~(1U << 3);
    return 0;
}

uint8_t ad9361_user_reg_read(uint16_t addr)
{
    if (!ad9361_phy)
        return 0xFF;

    return (uint8_t)ad9361_spi_read(ad9361_phy->spi, addr);
}

int ad9361_user_reg_write(uint16_t addr, uint8_t data)
{
    if (!ad9361_phy)
        return -EINVAL;

    return (int)ad9361_spi_write(ad9361_phy->spi, addr, data);
}

int ad9361_device_setup(void)
{
    extern int ad9361_no_os_init(void);
    int ret;
    uint8_t pid;
    uint8_t rev;

    ret = ad9361_lowlevel_axi_init();
    if (ret)
        return ret;

    ret = ad9361_no_os_init();
    if (ret) {
        xil_printf("ad9361_init failed: %d\r\n", ret);
        return ret;
    }

    pid = ad9361_user_reg_read(REG_PRODUCT_ID);
    rev = ad9361_user_reg_read(REG_PRODUCT_REV);

    xil_printf("AD9361: PID=0x%02X REV=0x%02X\r\n", pid, rev);
    return 0;
}
