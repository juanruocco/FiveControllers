#ifndef __GLOBALVARS_H__
#define __GLOBALVARS_H__


  const int I2C_SDA_ESP = 21;
  const int I2C_SCL_ESP = 22;
  const int I2C_SDA_ESPS3 = 21;
  const int I2C_SCL_ESPS3 = 47;

  //#define DEVICE_1_ENABLED   // 4 port esp32
  //#define DEVICE_2_ENABLED // 16 Port 
  #define DEVICE_3_ENABLED // 4 port esp32
  //#define DEVICE_4_ENABLED // 18 Port
  //#define DEVICE_5_ENABLED // 4 port esp32

  #define SLAVE_ADDRESS_P1 0x08
  #define SLAVE_ADDRESS_P3 0x10
  #define SLAVE_ADDRESS_P4 0x11
  #define SLAVE_ADDRESS_P5 0x12

  #ifdef DEVICE_1_ENABLED 
  #define DEVICE_ID 1
  const int I2C_SCL = I2C_SCL_ESP;
  const int I2C_SDA = I2C_SDA_ESP;
  #endif

  #ifdef DEVICE_2_ENABLED
  #define DEVICE_ID 2
  const int I2C_SCL = I2C_SCL_ESPS3;
  const int I2C_SDA = I2C_SDA_ESPS3;
  #endif

  #ifdef DEVICE_3_ENABLED
  #define DEVICE_ID 3
  const int I2C_SCL = I2C_SCL_ESP;//20;//22;
  const int I2C_SDA = I2C_SDA_ESP;//19;//21;
  #endif

  #ifdef DEVICE_4_ENABLED
  #define DEVICE_ID 4
  const int I2C_SCL = I2C_SCL_ESPS3;
  const int I2C_SDA = I2C_SDA_ESPS3;
  #endif

  #ifdef DEVICE_5_ENABLED
  #define DEVICE_ID 5
  const int I2C_SCL = I2C_SCL_ESP;
  const int I2C_SDA = I2C_SDA_ESP;
  #endif



#endif


