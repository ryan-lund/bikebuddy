#include <gpio.h>
#include <stdint.h>

typedef uint8_t i2c_addr_t;

// Initializes the I2C interface using these two pins
void i2c_init (GPIO_PIN SCL, GPIO_PIN SDA);

// Reads from a I2C device
void i2c_read (i2c_addr_t addr, char *bytes);
void i2c_read_byte (i2c_addr_t addr, char *byte);

// Reads from a register on a I2C device
void i2c_reg_read (i2c_addr_t addr, i2c_addr_t reg, char *bytes);
void i2c_reg_read_byte (i2c_addr_t addr, i2c_addr_t reg, char *byte);

//Writes to a I2C device
void i2c_write (i2c_addr_t addr, char *bytes);
void i2c_write_byte (i2c_addr_t addr, char *byte);

// Writes to a register on an I2C device
void i2c_reg_write (i2c_addr_t addr, i2c_addr_t reg, char *bytes);
void i2c_reg_write_byte (i2c_addr_t addr, i2c_addr_t reg, char *byte);

// Low level functionality
void i2c_send_start ();
void i2c_send_stop  ();
void i2c_send_read  (char *byte);
void i2c_send_write (char byte);

