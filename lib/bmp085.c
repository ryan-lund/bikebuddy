/**
  * @file bmp085.h
  * @brief  A Driver for the BMP085 pressure sensor for the STM32-F4 ARM Discovery Board, modified to work wiht nrf52
  * @author Eason Smith modified by Ryan Lund
  * @date 12/23/2015
  *
  */
#include <stdio.h>
#include <math.h>

#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_twi_mngr.h" 
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "bmp_085.h"


//globals (mostly calibration stuff)
static short ac1 = 0xFFFF;
static short ac2 = 0xFFFF;
static short ac3 = 0xFFFF;
static unsigned short ac4 = 0xFFFF;
static unsigned short ac5 = 0xFFFF;
static unsigned short ac6 = 0xFFFF;
static short b1 = 0xFFFF;
static short b2 = 0xFFFF;
//static short mb = 0xFFFF;
static short mc = 0xFFFF;
static short md = 0xFFFF;
static long b5 =0x0;

const uint32_t PRESSURE_SEALEVEL = 102250;  //pressure at sealevel in your area

const nrf_twi_mngr_t* twi_mngr_instance;


/**
	* @struct BMP_READINGS
  * @brief  A struct which holds the most current bmp085 sensor readings.
  * @param  None
  * @retval None
  */
struct BMP_READINGS {
  float temperature; //in Cel
	uint32_t pressure; //in Pascals
  float altitude; //in meters
	uint16_t ut; 
	uint32_t up; 
	uint8_t hz; //sensor readings per second (Hz)
} bmp;

/**
  * @fn short bmp085ReadShort(uint8_t bmp_control_reg)
  * @brief  read two bytes of data and return as a short.
  * @param  bmp_control_reg The register to request data from
  * @retval Short containing data
  */
uint16_t bmp085ReadShort(uint8_t bmp_control_reg)
{	
	uint8_t data[2]; 	
	//contact the bmp
	nrf_twi_mngr_transfer_t const read_short_transfer[] = {
    	NRF_TWI_MNGR_WRITE(BMP085_ADDRESS, bmp_control_reg, 1, NRF_TWI_MNGR_NO_STOP),
    	NRF_TWI_MNGR_READ(BMP085_ADDRESS, (uint8_t*) data, 2, 0),
  	};

  	int error = nrf_twi_mngr_perform(twi_mngr_instance, NULL, read_short_ransfer, sizeof(read_short_transfer)/sizeof(read_short_transfer[0]), NULL);
  	APP_ERROR_CHECK(error);
	
	//construct short
	uint8_t msb, lsb; 
	msb = data[0]; //read first byte
	lsb = data[1]; //read second byte
	
	return (uint16_t) msb<<8 | lsb; //combine both bytes as short
}

/**
  * @fn void bmp085_requestUT()
  * @brief  requests uncompensated temperature from the bmp085
  * @param  None
  * @retval None
  */
void bmp085_requestUT(){
	uint8_t reg[2] = {BMP085_CONTROL_REGISTER, BMP085_TEMPERATURE};

  nrf_twi_mngr_transfer_t const requestUT_transfer[] = {
    NRF_TWI_MNGR_WRITE(BMP085_ADDRESS, reg, 2, 0),
  };

  int error = nrf_twi_mngr_perform(twi_mngr_instance, NULL, requestUT_transfer, 1, NULL);
  APP_ERROR_CHECK(error);
}

/**
  * @fn uint8_t bmp085Read(uint8_t bmp_control_reg)
  * @brief  Read 1 byte from a BMP085 control register
  * @param  bmp_control_reg The register to read data from
  * @retval A uint8_t containing data
  */
uint8_t bmp085Read(uint8_t bmp_control_reg){
	uint8_t* data; 	
	//contact the bmp
	nrf_twi_mngr_transfer_t const read_byte_transfer[] = {
    	NRF_TWI_MNGR_WRITE(BMP085_ADDRESS, bmp_control_reg, 1, NRF_TWI_MNGR_NO_STOP),
    	NRF_TWI_MNGR_READ(BMP085_ADDRESS, (uint8_t*) data, 1, 0),
  	};

  	int error = nrf_twi_mngr_perform(twi_mngr_instance, NULL, read_tshort_ransfer, sizeof(read_byte_transfer)/sizeof(read_byte_transfer[0]), NULL);
  	APP_ERROR_CHECK(error);
	
	return *data;
}

/**
  * @fn unsigned short bmp085_readUT()
  * @brief  reads a uncompensated temperature value from the bmp085
  * @param  None
  * @retval unsigned short of uncompensated temperature
  */
unsigned short bmp085_readUT(){
	
	bmp.ut = bmp085ReadShort(0xF6);
	return bmp085ReadShort(0xF6); 
}	

/**
  * @fn void bmp085_calibration()
  * @brief Calibrates bmp085. Called at startup.
  * @param  None
  * @retval None
  */
void bmp085_calibration()
{
	printf(" calibrating BMP085\n");
	ac1 = bmp085ReadShort(0xAA);
	ac2 = bmp085ReadShort(0xAC);
	ac3 = bmp085ReadShort(0xAE);
	ac4 = bmp085ReadShort(0xB0);
	ac5 = bmp085ReadShort(0xB2);
	ac6 = bmp085ReadShort(0xB4);
	b1 = bmp085ReadShort(0xB6);
	b2 = bmp085ReadShort(0xB8);
	//mb = bmp085ReadShort(0xBA);
	mc = bmp085ReadShort(0xBC);
	md = bmp085ReadShort(0xBE);
	printf(" calibration complete.\n");
}

/**
  * @fn float bmp085_calcT(unsigned short ut)
  * @brief Calculates temperature given UT. Value returned will be in units of deg C
  * @param  ut Uncompensated Temperature
  * @retval returns a float of the temperature in degrees Celcius
  */
float bmp085_calcT(unsigned short ut){

	long x1, x2;
	x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
	x2 = ((long)mc << 11)/(x1 + md);
	b5 = x1 + x2;

	return ((b5 + 8)>>4)*.1; //in celcius
}
	
/**
  * @fn float bmp085_Conv_to_F(float Temp_C)
  * @brief convert temp to farenheight
  * @param  Temp_C Temperature in degrees C
  * @retval returns a float of the temperature in degrees Farenheit
  */
float bmp085_Conv_to_F(float Temp_C){
return 1.8f*Temp_C + 32.0f; //convert to farenheit 
}	

/**
  * @fn void bmp085_requestUP(uint8_t OSS)
  * @brief request an uncompensated pressure reading.
  * @param  OSS The oversampling setting [0,1,2,or 3] see datasheet
  * @retval None
  */
void bmp085_requestUP(uint8_t OSS){
	uint8_t reg[2] = {BMP085_CONTROL_REGISTER, (0x34 + (OSS<<6))};

  	nrf_twi_mngr_transfer_t const requestUP_transfer[] = {
    	NRF_TWI_MNGR_WRITE(BMP085_ADDRESS, reg, 2, 0),
  	};

  	int error = nrf_twi_mngr_perform(twi_mngr_instance, NULL, requestUP_transfer, 1, NULL);
  	APP_ERROR_CHECK(error);

}

/**
  * @fn uint32_t bmp085_readUP(uint8_t bmp_control_reg, uint8_t OSS)
  * @brief Read 3 bytes from a BMP085 pressure control register, combine them into a short.
  * @param  bmp_control_reg The register to read data from.
  * @param  OSS The oversampling setting [0,1,2,or 3] see datasheet.
  * @retval unsigned 32bit uncompensated pressure reading.
  */
uint32_t bmp085_readUP(uint8_t bmp_control_reg, uint8_t OSS)
{
	
	uint8_t num_bytes = 3;
	uint8_t data[num_bytes];

	uint8_t data[2]; 	
	//contact the bmp
	nrf_twi_mngr_transfer_t const readUP_transfer[] = {
    	NRF_TWI_MNGR_WRITE(BMP085_ADDRESS, bmp_control_reg, 1, NRF_TWI_MNGR_NO_STOP),
    	NRF_TWI_MNGR_READ(BMP085_ADDRESS, (uint8_t*) data, num_bytes, 0),
  	};

  	int error = nrf_twi_mngr_perform(twi_mngr_instance, NULL, readUP_ransfer, sizeof(readUP_transfer)/sizeof(readUP_transfer[0]), NULL);
  	APP_ERROR_CHECK(error);

	
	//assemble and return
	uint8_t msb, lsb, xlsb; 
	msb = data[0]; 
	lsb = data[1];
	xlsb = data[2];
	return (((uint32_t) msb << 16) | ((uint32_t) lsb << 8) | (uint32_t) xlsb) >> (8-OSS);
}

/**
  * @fn uint32_t bmp085_calcP(uint32_t up, uint8_t OSS)
  * @brief calculates pressure from UP. returns pascals. most of this is taken from the datasheet.
  * @param  up Uncompensated pressure reading.
  * @param  OSS The oversampling setting [0,1,2,or 3] see datasheet.
  * @retval unsigned 32bit pressure reading in Pascals.
  */
uint32_t bmp085_calcP(uint32_t up, uint8_t OSS){

	int32_t x1, x2, x3, b3, b6, p;
	uint32_t b4, b7;
	
	//Clac B6 using global variable B5
	b6 = b5 - 4000;
	
	// Calculate B3
	x1 = (b2 * (b6 * b6)>>12)>>11;
	x2 = (ac2 * b6)>>11;
	x3 = x1 + x2;
	b3 = (((((int32_t)ac1)*4 + x3)<<OSS) + 2)>>2;
	
	// Calculate B4
	x1 = (ac3 * b6)>>13;
	x2 = (b1 * ((b6 * b6)>>12))>>16;
	x3 = ((x1 + x2) + 2)>>2;
	b4 = (ac4 * (uint32_t)(x3 + 32768))>>15;
	
	// Calculate B7
	b7 = ((uint32_t)(up - b3) * (50000>>OSS));
	
	if (b7 < 0x80000000)
		p = (b7<<1)/b4;
	else
		p = (b7/b4)<<1;
	x1 = (p>>8) * (p>>8);
	x1 = (x1 * 3038)>>16;
	x2 = (-7357 * p)>>16;
	p += (x1 + x2 + 3791)>>4;
	
	return p;
}

/**
  * @fn float bmp085_calcAlt(void)
  * @brief calculate altitude in meters (see datasheet)
  * @param  None
  * @retval Float of the current altitude in meters.
  */
float bmp085_calcAlt(void){
	
	float x =  pow(bmp.pressure/PRESSURE_SEALEVEL, 0.190295F);
	float y = 1.0F-x;
	float altitude = 44330.0F * y;
		
	return 44330 * (1 - pow(((float) bmp.pressure/PRESSURE_SEALEVEL), 0.190295));
}

/**
  * @fn void bmp085_init()
  * @brief initializes bmp085 
  * @param  None
  * @retval None
  */
void bmp085_init(const nrf_twi_mngr_t* instance){
	twi_mngr_instance = instance
	bmp085_calibration();	
}

/**
  * @fn float bmp085_getTemperature(void)
  * @brief returns most recent temperature reading in Celsius.
  * @param  None
  * @retval Float of temperature reading in Celsius
  */
float bmp085_getTemperature(void){ 
	bmp085_requestUT();
	nrf_delay_ms(6); // 4.5 ms delay required
	bmp085_readUT();
	bmp.temperature = bmp085_calcT(bmp.ut);
	return bmp.temperature;
}

/**
  * @fn uint32_t bmp085_getPressure(void)
  * @brief returns most recent pressure reading in Pascals.
  * @param  None
  * @retval uint32 of pressure reading in Pascals
  */
uint32_t bmp085_getPressure(void){ 
	bmp085_requestUP(OSS);
	switch(OSS) {
		case 0:
			nrf_delay_ms(6); // 4.5 ms required
			break;
		case 1:
			nrf_delay_ms(9); // 7.5 ms required
			break;
		case 2:
			nrf_delay_ms(15); //13.5 ms required
			break;
		case 3:
			nrf_delay_ms(27); //25.5 ms required
			break;
	}
	
	bmp.up = bmp085_readUP(0xF6, OSS);
	bmp.pressure = bmp085_calcP(bmp.up, OSS);
	return bmp.pressure;
}

/**
  * @fn float bmp085_get_altitude_meters(void)
  * @brief returns most recent altitude reading in meters.
  * @param  None
  * @retval float of altitude reading in meters
  */
float bmp085_get_altitude_meters(void){
	bmp085_getPressure();
	bmp.altitude = bmp085_calcAlt();
	return bmp.altitude; 
}

/**
  * @fn float bmp085_get_altitude_feet(void)
  * @brief returns most recent altitude reading in feet.
  * @param  None
  * @retval float of altitude reading in feet
  */
float bmp085_get_altitude_feet(void){ 
	bmp085_get_altitude_meters();
	return bmp.altitude*3.28084F; 
}