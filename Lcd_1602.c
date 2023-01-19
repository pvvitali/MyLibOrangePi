
#include "Lcd_1602.h"



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>

#include "wiringPi.h"
#include "wiringPiI2C.h"

// I2C definitions

#define I2C_SLAVE	0x0703
#define I2C_SMBUS	0x0720	/* SMBus-level access */

#define I2C_SMBUS_READ	1
#define I2C_SMBUS_WRITE	0

// SMBus transaction types

#define I2C_SMBUS_QUICK		    0
#define I2C_SMBUS_BYTE		    1
#define I2C_SMBUS_BYTE_DATA	    2 
#define I2C_SMBUS_WORD_DATA	    3
#define I2C_SMBUS_PROC_CALL	    4
#define I2C_SMBUS_BLOCK_DATA	    5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7		/* SMBus 2.0 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

// SMBus messages

#define I2C_SMBUS_BLOCK_MAX	32	/* As specified in SMBus standard */	
#define I2C_SMBUS_I2C_BLOCK_MAX	32	/* Not specified but we use same structure */

// Structures used in the ioctl() calls

union i2c_smbus_data
{
  uint8_t  byte ;
  uint16_t word ;
  uint8_t  block [I2C_SMBUS_BLOCK_MAX + 2] ;	// block [0] is used for length + one more for PEC
} ;

struct i2c_smbus_ioctl_data
{
  char read_write ;
  uint8_t command ;
  int size ;
  union i2c_smbus_data *data ;
} ;

static inline int i2c_smbus_access (int fd, char rw, uint8_t command, int size, union i2c_smbus_data *data)
{
  struct i2c_smbus_ioctl_data args ;

  args.read_write = rw ;
  args.command    = command ;
  args.size       = size ;
  args.data       = data ;
  return ioctl (fd, I2C_SMBUS, &args) ;
}








unsigned int lcd_led = 0;

void lcd_led_on(){
	lcd_led = 1;
}

void lcd_led_off(){
	lcd_led = 0;
}






int lcd_1602_write_8bits_comand( int fd ) {
	uint8_t reg = 0x3C;
	union i2c_smbus_data data;
	data.block[0] = 1;		//size in data.block
	data.block[2] = 0x38;

	return i2c_smbus_access ( fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_I2C_BLOCK_DATA, &data);
}

// HAL_StatusTypeDef lcd_1602_write_8bits_comand(I2C_HandleTypeDef *i2c_handler){
// 	HAL_StatusTypeDef status;
// 	uint8_t data[2];
// 	data[0] = 0x3C;
// 	data[1] = 0x38;
// 	status = HAL_I2C_Master_Transmit(i2c_handler, Lcd_1602_address, data, 2, 500);
// 	return status;
// }




int lcd_1602_write_4bits_comand( int fd ) {
	uint8_t reg = 0x2C;
	union i2c_smbus_data data;
	data.block[0] = 1;		//size in data.block
	data.block[2] = 0x28;

	return i2c_smbus_access ( fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_I2C_BLOCK_DATA, &data);
}


// HAL_StatusTypeDef lcd_1602_write_4bits_comand(I2C_HandleTypeDef *i2c_handler){
// 	HAL_StatusTypeDef status;
// 	uint8_t data[2];
// 	data[0] = 0x2C;
// 	data[1] = 0x28;
// 	status = HAL_I2C_Master_Transmit(i2c_handler, Lcd_1602_address, data, 2, 500);
// 	return status;
// }



int lcd_1602_write_comand( int fd, uint8_t command ) {
	uint8_t reg = 0;
	reg |= (command & 0xF0) | lcd_led<<3 | 1<<2 ;

	union i2c_smbus_data data;

	data.block[0] = 3;		//size in data.block
	//
	data.block[1] = 0;
	data.block[2] = 0;
	data.block[3] = 0;
	//
	data.block[1] |= (command & 0xF0) | lcd_led<<3 | 0<<2 ;
	data.block[2] |= (command & 0x0F)<<4 | lcd_led<<3 | 1<<2 ;
	data.block[3] |= (command & 0x0F)<<4 | lcd_led<<3 | 0<<2 ;

	return i2c_smbus_access ( fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_I2C_BLOCK_DATA, &data);
}

// HAL_StatusTypeDef lcd_1602_write_comand(I2C_HandleTypeDef *i2c_handler, uint8_t command){
// 	HAL_StatusTypeDef status;
// 	uint8_t data[4];
// 	data[0] = 0;
// 	data[1] = 0;
// 	data[2] = 0;
// 	data[3] = 0;
// 	data[0] |= (command & 0xF0) | lcd_led<<3 | 1<<2 ;
// 	data[1] |= (command & 0xF0) | lcd_led<<3 | 0<<2 ;
// 	data[2] |= (command & 0x0F)<<4 | lcd_led<<3 | 1<<2 ;
// 	data[3] |= (command & 0x0F)<<4 | lcd_led<<3 | 0<<2 ;
// 	status = HAL_I2C_Master_Transmit(i2c_handler, Lcd_1602_address, data, 4, 500);
// 	return status;
// }




int Init_lcd_1602(int fd){
	int status;

	delay(20);	//del 20ms
	status = lcd_1602_write_8bits_comand( fd);
	delay(5);	//del 5ms
	status = lcd_1602_write_8bits_comand( fd);
	delay(1);	//del 1ms
	status = lcd_1602_write_8bits_comand( fd);

	delay(1);	//del 1ms
	status = lcd_1602_write_4bits_comand( fd);
	delay(1);	//del 1ms
	status = lcd_1602_write_comand( fd, 0x2C);
	delay(1);	//del 1ms
	status = lcd_1602_write_comand( fd, 0x0C);
	delay(1);	//del 1ms
	status = lcd_1602_write_comand( fd, 0x01);
	delay(2);	//del 1ms
	status = lcd_1602_write_comand( fd, 0x02);
	delay(2);	//del 1ms
	status = lcd_1602_write_comand( fd, 0x06);
	delay(1);	//del 1ms

	return status;
}

int Lcd_1602_Write_Data( int fd, uint8_t *pData){
	int status;
	uint8_t reg = 0;
	uint8_t char_data=0;
	uint8_t count=0;

	union i2c_smbus_data data;

	while(*pData != '\0'){
		char_data = *pData;
		//
		//
		reg = 0;
		reg |= (char_data & 0xF0) | lcd_led<<3 | 1<<2 | 1<<0 ;

		data.block[0] = 3;		//size in data.block
		//
		data.block[1] = 0;
		data.block[2] = 0;
		data.block[3] = 0;
		//
		data.block[1] |= (char_data & 0xF0) | lcd_led<<3 | 0<<2 | 1<<0 ;
		data.block[2] |= (char_data & 0x0F)<<4 | lcd_led<<3 | 1<<2 | 1<<0 ;
		data.block[3] |= (char_data & 0x0F)<<4 | lcd_led<<3 | 0<<2 | 1<<0 ;

		status = i2c_smbus_access ( fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_I2C_BLOCK_DATA, &data);
		pData++;

		count++;
		if(count > 20) break;
	}
	return status;
}





int Lcd_1602_SetPos(int fd, uint8_t x, uint8_t y)	//x=offset y=column
{
	int status;
	switch(y)
	{
		case 0:
				status = lcd_1602_write_comand( fd, x|0x80);
				break;
		case 1:
				status = lcd_1602_write_comand( fd, (0x40+x)|0x80);
				break;
		case 2:
				status = lcd_1602_write_comand( fd, (0x14+x)|0x80);
				break;
		case 3:
				status = lcd_1602_write_comand( fd, (0x54+x)|0x80);
				break;
	}
	return status;
}
