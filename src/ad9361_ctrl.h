#ifndef AD9361_CTRL_H
#define AD9361_CTRL_H

#include <stdint.h>
#include "xparameters.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AXI_AD9361_BASE            0x79020000U
#define AXI_AD9361_REG(off)        (*(volatile uint32_t *)(AXI_AD9361_BASE + (off)))
#define AXI_AD9361_REG_RSTN        AXI_AD9361_REG(0x0040)
#define AXI_AD9361_REG_CNTRL       AXI_AD9361_REG(0x0044)

#define REG_PRODUCT_ID             0x037
#define REG_PRODUCT_REV            0x038

void     ad9361_lowlevel_axi_init(void);
void     ad9361_no_os_init(void);
void     ad9361_device_setup(void);
uint8_t ad9361_user_reg_read(uint16_t addr);
int     ad9361_user_reg_write(uint16_t addr, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
