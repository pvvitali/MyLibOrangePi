#ifndef INC_LCD_1602_H_
#define INC_LCD_1602_H_

//#include "stm32f1xx_hal.h"

#define Lcd_1602_address	0x27    //0x4E    //0x70



void lcd_led_on();
void lcd_led_off();

int lcd_1602_write_8bits_comand(int fd);
int lcd_1602_write_4bits_comand(int fd);

int lcd_1602_write_comand(int fd, uint8_t command);

int Init_lcd_1602(int fd);

int Lcd_1602_Write_Data(int fd, uint8_t *pData);
int Lcd_1602_SetPos(int fd, uint8_t x, uint8_t y);	//x=offset y=column


#endif /* INC_LCD_1602_H_ */
