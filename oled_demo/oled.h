#ifndef __OLED_H
#define __OLED_H

#include "main.h"

// SCL=SCLK
// SDA=MOSI
// DC=DC
// CS=CS1
// FS0=MISO
// CS2=CS2

#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, RESET)
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, SET)

#define OLED_CS_Clr() HAL_GPIO_WritePin(OLED_CS1_GPIO_Port, OLED_CS1_Pin, RESET)
#define OLED_CS_Set() HAL_GPIO_WritePin(OLED_CS1_GPIO_Port, OLED_CS1_Pin, SET)

#define OLED_ROM_CS_Clr() HAL_GPIO_WritePin(OLED_CS2_GPIO_Port, OLED_CS2_Pin, RESET)
#define OLED_ROM_CS_Set() HAL_GPIO_WritePin(OLED_CS2_GPIO_Port, OLED_CS2_Pin, SET)

#define OLED_CMD 0
#define OLED_DATA 1

void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void OLED_WR_Byte(uint8_t dat, uint8_t cmd);
void OLED_Clear(void);
void OLED_Address(uint8_t x, uint8_t y);
void OLED_Display_128x64(uint8_t *dp);
void OLED_Display_16x16(uint8_t x, uint8_t y, uint8_t *dp);
void OLED_Display_8x16(uint8_t x, uint8_t y, uint8_t *dp);
void OLED_Display_5x7(uint8_t x, uint8_t y, uint8_t *dp);
void Send_Command_to_ROM(uint8_t dat);
uint8_t Get_data_from_ROM(void);
void OLED_get_data_from_ROM(uint8_t addrHigh, uint8_t addrMid, uint8_t addrLow, uint8_t *pbuff, uint8_t DataLen);
void OLED_Display_GB2312_string(uint8_t x, uint8_t y, uint8_t *text);
void OLED_Display_string_5x7(uint8_t x, uint8_t y, uint8_t *text);
void OLED_Show_FloatNum(uint8_t x, uint8_t y, float number, uint8_t len);
void OLED_Init(void);

void OLED_Show_Char_8x16(uint8_t x, uint8_t y, char Char);
void OLED_Show_String_8x16(uint8_t x, uint8_t y, char *String);
uint32_t OLED_Pow(uint32_t X, uint32_t Y);
void OLED_Show_UnsignedNum_8x16(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length);
void OLED_Show_SignedNum_8x16(uint8_t X, uint8_t Y, int32_t Number, uint8_t Length);
void OLED_Show_HexNum_8x16(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length);
void OLED_Show_BinNum_8x16(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length);
void OLED_Show_BinNum_FixedLength_8x16(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length);

#endif
