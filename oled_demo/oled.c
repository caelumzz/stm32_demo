#include "oled.h"
#include "stdlib.h"

extern SPI_HandleTypeDef hspi1;

uint32_t fontaddr = 0;

// 反显函数
void OLED_ColorTurn(uint8_t i)
{
  if (i == 0)
  {
    OLED_WR_Byte(0xA6, OLED_CMD); // 正常显示
  }
  if (i == 1)
  {
    OLED_WR_Byte(0xA7, OLED_CMD); // 反色显示
  }
}

// 屏幕旋转180度
void OLED_DisplayTurn(uint8_t i)
{
  if (i == 0)
  {
    OLED_WR_Byte(0xC8, OLED_CMD); // 正常显示
    OLED_WR_Byte(0xA1, OLED_CMD);
  }
  if (i == 1)
  {
    OLED_WR_Byte(0xC0, OLED_CMD); // 反转显示
    OLED_WR_Byte(0xA0, OLED_CMD);
  }
}

// 开启OLED显示
void OLED_DisPlay_On(void)
{
  OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵使能
  OLED_WR_Byte(0x14, OLED_CMD); // 开启电荷泵
  OLED_WR_Byte(0xAF, OLED_CMD); // 点亮屏幕
}

// 关闭OLED显示
void OLED_DisPlay_Off(void)
{
  OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵使能
  OLED_WR_Byte(0x10, OLED_CMD); // 关闭电荷泵
  OLED_WR_Byte(0xAE, OLED_CMD); // 关闭屏幕
}

// 向SSD1306写入一个字节。
// mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{

  // uint8_t i;
  if (cmd)
    OLED_DC_Set();
  else
    OLED_DC_Clr();
  OLED_CS_Clr();
  HAL_SPI_Transmit(&hspi1, &dat, 1, 0xff);
  OLED_CS_Set();
  OLED_DC_Set();
}

// 清屏函数
void OLED_Clear(void)
{
  uint8_t i, n;
  for (i = 0; i < 8; i++)
  {
    OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置页地址
    OLED_WR_Byte(0x10, OLED_CMD);     // 设置列地址的高4位
    OLED_WR_Byte(0x00, OLED_CMD);     // 设置列地址的低4位
    for (n = 0; n < 128; n++)
    {
      OLED_WR_Byte(0x00, OLED_DATA); // 清除所有数据
    }
  }
}

// 设置起始地址
void OLED_Address(uint8_t x, uint8_t y)
{
  OLED_WR_Byte(0xb0 + y, OLED_CMD);                 // 设置页地址
  OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD); // 设置列地址的高4位
  OLED_WR_Byte((x & 0x0f), OLED_CMD);               // 设置列地址的低4位
}

// 显示128x64点阵图像
void OLED_Display_128x64(uint8_t *dp)
{
  uint8_t i, j;
  for (i = 0; i < 8; i++)
  {
    OLED_Address(0, i);
    for (j = 0; j < 128; j++)
    {
      OLED_WR_Byte(*dp, OLED_DATA); // 写数据到OLED,每写完一个8位的数据后列地址自动加1
      dp++;
    }
  }
}

// 显示16x16点阵图像、汉字、生僻字或16x16点阵的其他图标
void OLED_Display_16x16(uint8_t x, uint8_t y, uint8_t *dp)
{
  uint8_t i, j;
  for (j = 0; j < 2; j++)
  {
    OLED_Address(x, y);
    for (i = 0; i < 16; i++)
    {
      OLED_WR_Byte(*dp, OLED_DATA); // 写数据到OLED,每写完一个8位的数据后列地址自动加1
      dp++;
    }
    y++;
  }
}

// 显示8x16点阵图像、ASCII, 或8x16点阵的自造字符、其他图标
void OLED_Display_8x16(uint8_t x, uint8_t y, uint8_t *dp)
{
  uint8_t i, j;
  for (j = 0; j < 2; j++)
  {
    OLED_Address(x, y);
    for (i = 0; i < 8; i++)
    {
      OLED_WR_Byte(*dp, OLED_DATA); // 写数据到LCD,每写完一个8位的数据后列地址自动加1
      dp++;
    }
    y++;
  }
}

// 显示5*7点阵图像、ASCII, 或5x7点阵的自造字符、其他图标
void OLED_Display_5x7(uint8_t x, uint8_t y, uint8_t *dp)
{
  uint8_t i;
  OLED_Address(x, y);
  for (i = 0; i < 6; i++)
  {
    OLED_WR_Byte(*dp, OLED_DATA);
    dp++;
  }
}

// 送指令到晶联讯字库IC
void Send_Command_to_ROM(uint8_t dat)
{
  HAL_SPI_Transmit(&hspi1, &dat, 1, 0xff);
}

// 从晶联讯字库IC中取汉字或字符数据（1个字节）
uint8_t Get_data_from_ROM(void)
{
  uint8_t read = 0;
  HAL_SPI_Receive(&hspi1, &read, 1, 0xff);
  return read;
}

// 从相关地址（addrHigh：地址高字节,addrMid：地址中字节,addrLow：地址低字节）中连续读出DataLen个字节的数据到 pbuff的地址
// 连续读取
void OLED_get_data_from_ROM(uint8_t addrHigh, uint8_t addrMid, uint8_t addrLow, uint8_t *pbuff, uint8_t DataLen)
{
  uint8_t i;
  OLED_ROM_CS_Clr();
  Send_Command_to_ROM(0x03);
  Send_Command_to_ROM(addrHigh);
  Send_Command_to_ROM(addrMid);
  Send_Command_to_ROM(addrLow);
  for (i = 0; i < DataLen; i++)
  {
    *(pbuff + i) = Get_data_from_ROM();
  }
  OLED_ROM_CS_Set();
}

void OLED_Display_GB2312_string(uint8_t x, uint8_t y, uint8_t *text)
{
  uint8_t i = 0;
  uint8_t addrHigh, addrMid, addrLow;
  uint8_t fontbuf[32];
  while (text[i] > 0x00)
  {
    if ((text[i] >= 0xb0) && (text[i] <= 0xf7) && (text[i + 1] >= 0xa1))
    {
      // 国标简体（GB2312）汉字在晶联讯字库IC中的地址由以下公式来计算：
      // Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
      // 由于担心8位单片机有乘法溢出问题，所以分三部取地址
      fontaddr = (text[i] - 0xb0) * 94;
      fontaddr += (text[i + 1] - 0xa1) + 846;
      fontaddr = fontaddr * 32;

      addrHigh = (fontaddr & 0xff0000) >> 16; // 地址的高8位,共24位
      addrMid = (fontaddr & 0xff00) >> 8;     // 地址的中8位,共24位
      addrLow = (fontaddr & 0xff);            // 地址的低8位,共24位

      OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 32);
      // 取32个字节的数据，存到"fontbuf[32]"
      OLED_Display_16x16(x, y, fontbuf);
      // 显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据
      x += 16;
      i += 2;
    }
    else if ((text[i] >= 0xa1) && (text[i] <= 0xa3) && (text[i + 1] >= 0xa1))
    {

      fontaddr = (text[i] - 0xa1) * 94;
      fontaddr += (text[i + 1] - 0xa1);
      fontaddr = fontaddr * 32;
      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = (fontaddr & 0xff);

      OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 32);
      OLED_Display_16x16(x, y, fontbuf);
      x += 16;
      i += 2;
    }
    else if ((text[i] >= 0x20) && (text[i] <= 0x7e))
    {
      unsigned char fontbuf[16];
      fontaddr = (text[i] - 0x20);
      fontaddr = (unsigned long)(fontaddr * 16);
      fontaddr = (unsigned long)(fontaddr + 0x3cf80);

      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = fontaddr & 0xff;
      OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 16);
      OLED_Display_8x16(x, y, fontbuf);
      x += 8;
      i += 1;
    }
    else
    {
      i++;
      printf("else\n");
    }
  }
}
/**
 * @brief 显示5*7宽度的数据函数
 * @param x
 * @param y
 * @param text
 */
void OLED_Display_string_5x7(uint8_t x, uint8_t y, uint8_t *text)
{
  uint8_t i = 0;
  uint8_t addrHigh, addrMid, addrLow;
  while (text[i] > 0x00)
  {
    if ((text[i] >= 0x20) && (text[i] <= 0x7e))
    {
      uint8_t fontbuf[8];
      fontaddr = (text[i] - 0x20);
      fontaddr = (unsigned long)(fontaddr * 8);
      fontaddr = (unsigned long)(fontaddr + 0x3bfc0);

      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = fontaddr & 0xff;
      OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 8);
      OLED_Display_5x7(x, y, fontbuf);
      x += 6;
      i += 1;
    }
    else
      i++;
  }
}

/**
 * @brief 显示小数函数，小数位固定长度为2
 * @param x 横坐标位置
 * @param y 纵坐标位置
 * @param number 需要显示的小数
 * @param len 需要显示的小数数据长度
 */
void OLED_Show_FloatNum(uint8_t x, uint8_t y, float number, uint8_t len)
{
  uint8_t i;
  uint32_t t, temp_numbewr;
  x = x + len * 8 + 8;         // 要显示的小数最低位的横坐标
  temp_numbewr = number * 100; // 将小数左移两位并转化为整数
  // OLED_Display_GB2312_string(x - 24, y, "."); // 显示小数点
  OLED_Show_Char_8x16(x - 24, y, '.');
  for (i = 0; i < len; i++)
  {
    t = temp_numbewr % 10;            // 取个位数的数值
    temp_numbewr = temp_numbewr / 10; // 将整数右移一位
    x -= 8;
    if (i == 2)
    {
      x -= 8;
    } // 当显示出来两个小数之后，空出小数点的位置
    OLED_Show_UnsignedNum_8x16(x, y, t, 1);
  }
}

/**
 * @brief 显示一个字符函数
 * @param x 横坐标位置
 * @param y 纵坐标位置
 * @param Char 需要显示的一个字符
 */
void OLED_Show_Char_8x16(uint8_t x, uint8_t y, char Char)
{
  uint8_t AddrHight, AddrMid, AddrLow;

  if ((Char >= 0x20) && (Char <= 0x7E))
  {
    uint8_t fontbuf[16];
    fontaddr = (Char - 0x20);
    fontaddr = (unsigned long)(fontaddr * 16);
    fontaddr = (unsigned long)(fontaddr + 0x3b7c0);

    AddrHight = (fontaddr & 0xff0000) >> 16;
    AddrMid = (fontaddr & 0xff00) >> 8;
    AddrLow = fontaddr & 0xff;

    OLED_get_data_from_ROM(AddrHight, AddrMid, AddrLow, fontbuf, 16);
    OLED_Display_8x16(x, y, fontbuf);
  }
}
/**
 * @brief 显示字符串函数
 * @param x 横坐标位置
 * @param y 纵坐标位置
 * @param String 需要显示的字符串数据
 */
void OLED_Show_String_8x16(uint8_t x, uint8_t y, char *String)
{
  uint8_t i;
  for (i = 0; String[i] != '\0'; i++)
  {
    OLED_Show_Char_8x16(x + 8 * i, y, String[i]);
  }
}
/**
 * @brief oled的次方函数 即x的y次方
 * @param X
 * @param Y
 * @return 返回次方的结果
 */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
  uint32_t Result = 1;
  while (Y--)
  {
    Result *= X;
  }
  return Result;
}
/**
 * @brief 需要显示的无符号数据
 * @param X 横坐标位置
 * @param Y 纵坐标位置
 * @param Number 需要显示的数据
 * @param Length 需要显示数据的长度
 */
void OLED_Show_UnsignedNum_8x16(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length)
{
  uint8_t i, temp_number;
  // char *temp[Length];
  for (i = 0; i < Length; i++)
  {
    temp_number = Number / OLED_Pow(10, Length - i - 1) % 10 + '0';
    OLED_Show_Char_8x16(X + 8 * i, Y, temp_number);
  }
}
/**
 * @brief 显示带符号位的数据
 * @param X 横坐标位置
 * @param Y 纵坐标位置
 * @param Number 需要显示的数据
 * @param Length 需要显示数据的长度
 */
void OLED_Show_SignedNum_8x16(uint8_t X, uint8_t Y, int32_t Number, uint8_t Length)
{
  uint8_t i, temp_number;
  // char *temp[Length];
  uint32_t abs_number;
  if (Number >= 0)
  {
    OLED_Show_Char_8x16(X, Y, '+');
    abs_number = Number;
  }
  else
  {
    OLED_Show_Char_8x16(X, Y, '-');
    abs_number = -Number;
  }
  for (i = 0; i < Length; i++)
  {
    temp_number = (char)abs_number / OLED_Pow(10, Length - i - 1) % 10 + '0';
    OLED_Show_Char_8x16(X + 8 + 8 * i, Y, temp_number);
  }
}
/**
 * @brief 需要现实的十六进制数据
 * @param X 横坐标的位置
 * @param Y 纵坐标的位置
 * @param Number 需要现实的数据
 * @param Length 需要显示为十六进制数据的长度
 */
void OLED_Show_HexNum_8x16(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length)
{
  uint8_t i, temp_number;
  // char *temp[Length];
  for (i = 0; i < Length; i++)
  {
    temp_number = Number / OLED_Pow(16, Length - i - 1) % 16;
    if (temp_number < 10)
    {
      temp_number += '0';
      OLED_Show_Char_8x16(X + 8 * i, Y, temp_number);
    }
    else
    {
      temp_number = temp_number - 10 + 'A';
      OLED_Show_Char_8x16(X + 8 * i, Y, temp_number);
    }
  }
}
/**
 * @brief 显示二进制数据
 * @param X 横坐标的位置
 * @param Y 纵坐标的位置
 * @param Number 需要显示为二进制的数据
 * @param Length 需要显示的二进制数据长度
 */
void OLED_Show_BinNum_8x16(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length)
{
  uint8_t i, temp_number;
  for (i = 0; i < Length; i++)
  {
    temp_number = Number / OLED_Pow(2, Length - i - 1) % 2 + '0';
    OLED_Show_Char_8x16(X + 8 * i, Y, temp_number);
  }
}
/**
 * @brief Display fixed length binary data
 * @param X Horizontal coordinate position (0~127)
 * @param Y Vertical coordinate position  (0~7)
 * @param Number Data to be displayed
 * @param Length The length of data to be displayed
 */
void OLED_Show_BinNum_FixedLength_8x16(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length)
{
  uint8_t i, temp_number;
  for (i = 0; i < Length; i++)
  {

    if (i < 4)
    {
      temp_number = Number / OLED_Pow(2, Length - i - 2) % 2 + '0';
      OLED_Show_Char_8x16(X + 8 * i, Y, temp_number);
    }
    if (i == 4)
    {
      OLED_Show_Char_8x16(X + 8 * i, Y, ' ');
    }
    if (i > 4)
    {
      temp_number = Number / OLED_Pow(2, Length - i - 1) % 2 + '0';
      OLED_Show_Char_8x16(X + 8 * i, Y, temp_number);
    }
  }
}
/**
 * @brief oled初始化函数
 *
 */
void OLED_Init(void)
{
  // OLED_DC_Set();
  OLED_CS_Set();
  OLED_ROM_CS_Set();

  HAL_Delay(20);

  OLED_WR_Byte(0xAE, OLED_CMD); //--turn off oled panel

  OLED_WR_Byte(0x00, OLED_CMD); //---set low column address
  OLED_WR_Byte(0x10, OLED_CMD); //---set high column address

  OLED_WR_Byte(0x40, OLED_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)

  OLED_WR_Byte(0x81, OLED_CMD); //--set contrast control register
  OLED_WR_Byte(0xCF, OLED_CMD); // Set SEG Output Current Brightness

  OLED_WR_Byte(0xA1, OLED_CMD); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常

  OLED_WR_Byte(0xC8, OLED_CMD); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常

  OLED_WR_Byte(0xA6, OLED_CMD); //--set normal display

  OLED_WR_Byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
  OLED_WR_Byte(0x3f, OLED_CMD); //--1/64 duty

  OLED_WR_Byte(0xD3, OLED_CMD); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WR_Byte(0x00, OLED_CMD); //-not offset

  OLED_WR_Byte(0xd5, OLED_CMD); //--set display clock divide ratio/oscillator frequency
  OLED_WR_Byte(0x80, OLED_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec

  OLED_WR_Byte(0xD9, OLED_CMD); //--set pre-charge period
  OLED_WR_Byte(0xF1, OLED_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock

  OLED_WR_Byte(0xDA, OLED_CMD); //--set com pins hardware configuration
  OLED_WR_Byte(0x12, OLED_CMD);

  OLED_WR_Byte(0xDB, OLED_CMD); //--set vcomh
  OLED_WR_Byte(0x40, OLED_CMD); // Set VCOM Deselect Level

  OLED_WR_Byte(0x20, OLED_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WR_Byte(0x02, OLED_CMD); //

  OLED_WR_Byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
  OLED_WR_Byte(0x14, OLED_CMD); //--set(0x10) disable

  OLED_WR_Byte(0xA4, OLED_CMD); // Disable Entire Display On (0xa4/0xa5)

  OLED_WR_Byte(0xA6, OLED_CMD); // Disable Inverse Display On (0xa6/a7)

  OLED_Clear();
  OLED_WR_Byte(0xAF, OLED_CMD); /*display ON*/
}
