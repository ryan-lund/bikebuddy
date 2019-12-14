#include "imu.h"
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "bno055.h"
#include <stdlib.h>
#include <string.h>
#include "nrf_delay.h"
#include "nrf_drv_twi.h"


const nrf_drv_twi_t _m_twi_master = NRF_DRV_TWI_INSTANCE(0);
struct bno055_t myIMU;
/*----------------------------------------------------------------------------*
 *  The following APIs are used for reading and writing of
 *	sensor data using I2C communication
*----------------------------------------------------------------------------*/


// nrf_drv_twi_t* _m_twi_master;
#define SSD1306_CONFIG_VDD_PIN 04
#define SSD1306_CONFIG_SCL_PIN_0 05
#define SSD1306_CONFIG_SDA_PIN_0 28
#define SSD1306_CONFIG_SCL_PIN_1 29
#define SSD1306_CONFIG_SDA_PIN_1 30
/**
 * @brief TWI initialization.
 */
void twi_init0 (void)
{
        ret_code_t err_code;

        const nrf_drv_twi_config_t twi_sensors_config = {
                .scl                = SSD1306_CONFIG_SCL_PIN_0,
                .sda                = SSD1306_CONFIG_SDA_PIN_0,
                .frequency          = NRF_TWI_FREQ_400K,
                .interrupt_priority = APP_IRQ_PRIORITY_LOWEST
        };

        //err_code = nrf_drv_twi_init(&m_twi_lis2dh12, &twi_lis2dh12_config, twi_handler, NULL);
        err_code = nrf_drv_twi_init(&_m_twi_master, &twi_sensors_config, NULL, NULL);    // twi in blocking mode.
        APP_ERROR_CHECK(err_code);

        nrf_drv_twi_enable(&_m_twi_master);
}
int imu_init(void){
	twi_init0();
	int com_error = 0;


	myIMU.bus_read = BNO055_I2C_bus_read;
	myIMU.bus_write = BNO055_I2C_bus_write;
	myIMU.delay_msec = BNO055_delay_msek;
	myIMU.dev_addr = BNO055_I2C_ADDR1;
;
	com_error += bno055_init(&myIMU);
	com_error += bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);
    com_error += bno055_set_power_mode(BNO055_POWER_MODE_NORMAL);
	return com_error;
}

/*	\Brief: The API is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register,
 *   will data is going to be read
 *	\param reg_data : This data read from the sensor,
 *   which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt){
	//
	// Wait until the STP bit is cleared from any previous master communication.
	// Clearing of this bit by the module is delayed until after the SCD bit is
	// set. If this bit is not checked prior to initiating a new message, the
	// I2C could get confused.
	//

	// s32 BNO055_iERROR = BNO055_INIT_VALUE;
	u8 array[cnt+1];

	/* Please take the below API as your reference
	 * for read the data using I2C communication
	 * add your I2C read API here.
	 * "BNO055_iERROR = I2C_WRITE_READ_STRING(DEV_ADDR,
	 * ARRAY, ARRAY, 1, CNT)"
	 * BNO055_iERROR is an return value of SPI write API
	 * Please select your valid return value
     * In the driver BNO055_SUCCESS defined as 0
     * and FAILURE defined as -1
	 */
	nrf_drv_twi_tx(&_m_twi_master, dev_addr, &reg_addr, cnt+1, false);
	nrf_drv_twi_rx(&_m_twi_master, dev_addr, array, cnt+1);
    NRF_LOG_INFO("READING: SENT ADDRESS WRITE");
    NRF_LOG_HEXDUMP_INFO(&array, 2);
    // NRF_LOG_INFO(&BNO055_iERROR);
	// for (stringpos = BNO055_INIT_VALUE; stringpos < cnt; stringpos++)
	// 	*(reg_data + stringpos) = array[stringpos];
	return 0;

}

/*	\Brief: The API is used as SPI bus write
 *	\Return : Status of the SPI write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register,
 *   will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *	will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt){
	//
	// Wait until the STP bit is cleared from any previous master communication.
	// Clearing of this bit by the module is delayed until after the SCD bit is
	// set. If this bit is not checked prior to initiating a new message, the
	// I2C could get confused.
	//
    u8 *tx_data = malloc(cnt+1);
    memcpy(tx_data+1, reg_data, cnt);
    tx_data[0] = reg_addr;

	NRF_LOG_INFO("A WRITE PERFORMED. Second value %x",  tx_data[1]);

	nrf_drv_twi_tx(&_m_twi_master, dev_addr, tx_data, cnt+1, false);

    free(tx_data);
	
	return BNO055_SUCCESS;
}

int i2c_send_error(unsigned long* counter){
	*counter = 0;
	return BNO055_ERROR;
}

void BNO055_delay_msek(u32 msek){
	nrf_delay_ms(msek);
}

void I2C_Init(void)
{
	return;
}