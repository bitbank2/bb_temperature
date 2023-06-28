//
// BitBank Capacitive Touch Sensor Library
// written by Larry Bank
//
// Copyright 2023 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================

//
// Written for the many variants of ESP32 + Capacitive touch LCDs on the market
//
#include <Arduino.h>
#include <Wire.h>

#ifndef __BB_CAPTOUCH__
#define __BB_CAPTOUCH__

#define CT_SUCCESS 0
#define CT_ERROR -1

enum {
  CT_TYPE_FT6X36 = 0,
  CT_TYPE_GT911
};

#define GT911_ADDR1 0x5D
#define GT911_ADDR2 0x14
#define FT6X36_ADDR 0x38

// GT911 registers
#define GT911_POINT_INFO 0x814E
#define GT911_POINT_1    0x814F
#define GT911_CONFIG_FRESH 0x8100
#define GT911_CONFIG_SIZE 0xb9
#define GT911_CONFIG_START 0x8047

// FT6x36 registers
#define TOUCH_REG_STATUS 0x02
#define TOUCH_REG_XH 0x03
#define TOUCH_REG_XL 0x04
#define TOUCH_REG_YH 0x05
#define TOUCH_REG_YL 0x06
#define TOUCH_REG_WEIGHT 0x07
#define TOUCH_REG_AREA 0x08
// register offset to info for the second touch point
#define PT2_OFFSET 6
typedef struct _fttouchinfo
{
  int count;
  uint16_t x[5], y[5];
  uint8_t pressure[5], area[5];
} TOUCHINFO;

class BBCapTouch
{
public:
    BBCapTouch() {}
    ~BBCapTouch() { Wire.end(); }

    int init(int iSDA, int iSCL, int iRST=-1, int iINT=-1, uint32_t u32Speed=400000);
    int getSamples(TOUCHINFO *pTI);
 
private:
    int _iAddr;
    int _iType;
    bool I2CTest(uint8_t u8Addr);
    int I2CRead(uint8_t u8Addr, uint8_t *pData, int iLen);
    int I2CReadRegister(uint8_t u8Addr, uint8_t u8Register, uint8_t *pData, int iLen);
    int I2CReadRegister16(uint8_t u8Addr, uint16_t u16Register, uint8_t *pData, int iLen);
    int I2CWrite(uint8_t u8Addr, uint8_t *pData, int iLen);
}; // class BBCapTouch
#endif // __BB_CAPTOUCH__
