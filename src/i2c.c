#include <stdint.h>
#include "tl_common.h"
#include "drivers.h"
#include "vendor/common/user_config.h"
#include "app_config.h"
#include "app.h"
#include "drivers/8258/gpio_8258.h"

_attribute_ram_code_ void init_i2c(void){
	i2c_gpio_set(I2C_GROUP); // I2C_GPIO_GROUP_C0C1, I2C_GPIO_GROUP_C2C3, I2C_GPIO_GROUP_B6D7, I2C_GPIO_GROUP_A3A4
#if (DEVICE_TYPE == DEVICE_CGDK2)
	reg_i2c_speed = (uint8_t)(CLOCK_SYS_CLOCK_HZ/(4*450000)); // 450 kHz
#elif (DEVICE_TYPE == DEVICE_LYWSD03MMC)
	if(cfg.hw_cfg.hwver == 3) // HW:B1.9
		reg_i2c_speed = (uint8_t)(CLOCK_SYS_CLOCK_HZ/(4*500000)); // 500 kHz
	else
		reg_i2c_speed = (uint8_t)(CLOCK_SYS_CLOCK_HZ/(4*700000)); // 700 kHz
#else
	reg_i2c_speed = (uint8_t)(CLOCK_SYS_CLOCK_HZ/(4*700000)); // 700 kHz
#endif
    //reg_i2c_id  = slave address
    reg_i2c_mode |= FLD_I2C_MASTER_EN; //enable master mode
	reg_i2c_mode &= ~FLD_I2C_HOLD_MASTER; // Disable clock stretching for Sensor

    reg_clk_en0 |= FLD_CLK0_I2C_EN;    //enable i2c clock
    reg_spi_sp  &= ~FLD_SPI_ENABLE;   //force PADs act as I2C; i2c and spi share the hardware of IC
}

int scan_i2c_addr(int address) {
	if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
		init_i2c();
	reg_i2c_id = (uint8_t) address;
	reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID | FLD_I2C_CMD_STOP;
	while (reg_i2c_status & FLD_I2C_CMD_BUSY);
	return ((reg_i2c_status & FLD_I2C_NAK)? 0 : address);
}
