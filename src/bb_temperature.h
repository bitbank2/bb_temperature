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

#include <Arduino.h>
#include <BitBang_I2C.h>

#ifndef __BB_TEMP__
#define __BB_TEMP__

#define BBT_SUCCESS 0
#define BBT_ERROR -1

enum {
  BBT_TYPE_AHT20 = 0,
  BBT_TYPE_BMP180,
  BBT_TYPE_BME280,
  BBT_TYPE_SHT3X,
  BBT_TYPE_HDC1080,
  BBT_TYPE_HTS221,
  BBT_TYPE_MCP9808,
  BBT_TYPE_COUNT
};

#define BBT_ADDR_MCP9808 0x18
#define BBT_ADDR_AHT20 0x38
#define BBT_ADDR_HDC1080 0x40
#define BBT_ADDR_HTS221 0x5f
#define BBT_ADDR_BME280 0x76
#define BBT_ADDR_BME680 0x76
#define BBT_ADDR_SHT3X 0x44

#define AHT20_REG_STATUS 0x71
#define AHT20_REG_RESET 0xBA
#define AHT20_REG_INIT 0xBE
#define AHT20_REG_MEASURE 0xAC

// AHT20 status bits
#define AHT20_CALIBRATED 0x08
#define AHT20_BUSY 0x80

typedef struct _tagbbtsample
{
   int iTemperature; // Temp in C * 10
   int iHumidity; // humidity in percent (0-100)
   int iPressure; // pressure in hpa
} BBTSAMPLE;

class BBTemp
{
public:
    BBTemp() {}
    ~BBTemp() {}

    int init(int iSDA, int iSCL, bool bBitBang, uint32_t u32Speed=400000);
    int getSamples(BBTSAMPLE *pBS);
 
private:
    int _iAddr;
    int _iType;
}; // class BBTemp
#endif // __BB_TEMP__
