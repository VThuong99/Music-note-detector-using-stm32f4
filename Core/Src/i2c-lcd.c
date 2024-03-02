
/** Put this in the src folder **/

#include "i2c-lcd.h"
extern I2C_HandleTypeDef hi2c2;  // change your handler here accordingly

#define SLAVE_ADDRESS_LCD 0x4E // Dia chi dau

void lcd_send_cmd (char cmd)
{
  char data_u, data_l; //char 1 byte de luu tren 4 cao and 4 thap
	uint8_t data_t[4];
	data_u = (cmd&0xf0); // cmd and 0xf0 = 11110000 de tao 4 bit cao
	data_l = ((cmd<<4)&0xf0); // delete 4 bit cao
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit (&hi2c2, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100); // send qua i2c
}

void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	HAL_I2C_Master_Transmit (&hi2c2, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void lcd_clear (void) //clear lcd
{
	lcd_send_cmd (0x80);
	for (int i=0; i<70; i++)
	{
		lcd_send_data (' ');
	}
}

void lcd_put_cur(int row, int col)  // row hang col cot dat vi tri con tro
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
				case 2:
						col |= 0x94;
						break;
				case 3:
						col |= 0xD4;
						break;
    }

    lcd_send_cmd (col);
}


void lcd_init (void) // init lcd
{
	// 4 bit initialisation
	HAL_Delay(50);  // wait for >40ms
	lcd_send_cmd (0x30);
	HAL_Delay(5);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	HAL_Delay(1);  // wait for >100us
	lcd_send_cmd (0x30);
	HAL_Delay(10);
	lcd_send_cmd (0x20);  // 4bit mode
	HAL_Delay(10);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}

void send_int_to_lcd(int value) {
  char buffer[20];
  int i = 0;

  // xu ly cho am
  if (value < 0) {
    lcd_send_data('-');
    value = -value;
  }

  //chuyen tung so int thanh chuoi
  do {
    buffer[i++] = '0' + (value % 10);
    value /= 10;
  } while (value > 0);

  // dao chuoi ky tu
  for (int j = i - 1; j >= 0; j--) {
    lcd_send_data(buffer[j]);
  }
}

void send_float_to_lcd(float value) {
  char buffer[20];

  // float to string
  int integerPart = (int)value;
  int decimalPart = (int)((value - integerPart) * 100); // lay 2 so thap phan

  // nguyen to string
  int i = 0;
  do {
    buffer[i++] = '0' + (integerPart % 10);
    integerPart /= 10;
  } while (integerPart > 0);

  // inveter ky tu
  int j;
  for (j = 0; j < i / 2; j++) {
    char temp = buffer[j];
    buffer[j] = buffer[i - j - 1];
    buffer[i - j - 1] = temp;
  }

  // add dau cho float
  buffer[i++] = '.';
  buffer[i++] = '0' + (decimalPart / 10);
  buffer[i++] = '0' + (decimalPart % 10);
  buffer[i] = '\0'; // and chuoi

  lcd_send_string(buffer);
}

