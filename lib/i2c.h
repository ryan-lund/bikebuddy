#include <gpio.h>
#include <stdint.h>

extern const nrf_drv_twi_t m_twi;
extern volatile bool m_xfer_done;

void i2c_init (uint32_t slc, uint32_t sca);

