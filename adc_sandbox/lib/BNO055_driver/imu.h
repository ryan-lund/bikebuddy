#ifndef HEADERS_IMU_H_
#define HEADERS_IMU_H_
#include "bno055.h"
#include "nrf_drv_twi.h"

#define COUNTER_TIMEOUT 	1000000UL

enum i2c_err_t{NO_ERROR = 0,BUSY_BUS, NO_TX_FLAG, NO_RX_FLAG, NO_TX_END };
extern enum i2c_err_t i2c_error;

#define IMU_I2C_ADD	0x28

extern struct bno055_t myIMU;

int imu_init(const nrf_drv_twi_t* m_twi_master);
void imu_reinit(void);
void I2C_Init(void);

/*	\Brief: The API is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register,
 *   will data is going to be read
 *	\param reg_data : This data read from the sensor,
 *   which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

/*	\Brief: The API is used as SPI bus write
 *	\Return : Status of the SPI write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register,
 *   will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *	will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

void BNO055_delay_msek(u32 msek);

int i2c_send_error(unsigned long* counter);

#endif /* HEADERS_IMU_H_ */