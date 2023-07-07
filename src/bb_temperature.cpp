//
// BitBank Temperature Sensor Library
// Written by Larry Bank
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
#include "bb_temperature.h"

//
// Some sensors like the HTS221 can't read multiple bytes in 1 transaction
//
void BBTemp::readMultiple(int iRegister, uint8_t *pData, int iCount)
{   
int i;
    
    for (i=0; i<iCount; i++) { // must read each byte 1 at a time
        I2CReadRegister(&_bbi2c, _iAddr, iRegister + i, &pData[i], 1);
    }
} /* readMultiple() */ 

//
// Tell the sensor to start sampling
//
int BBTemp::start(void)
{
uint8_t ucTemp[4];
uint8_t ucCal[34];

   switch (_iType) {
      case BBT_TYPE_AHT20:
         I2CRead(&_bbi2c, _iAddr, ucTemp, 1); // first byte read is status byte
         if ((ucTemp[0] & AHT20_CALIBRATED) == 0) { // need to init
            ucTemp[0] = AHT20_REG_INIT; // initialize
            ucTemp[1] = 0x08;
            ucTemp[2] = 0x00;
            I2CWrite(&_bbi2c, _iAddr, ucTemp, 3);
         }
         break;

      case BBT_TYPE_SHT3X:
         ucTemp[0] = (uint8_t)(SHT3X_SOFTRESET >> 8);
         ucTemp[1] = (uint8_t)SHT3X_SOFTRESET;
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 2);
         break;

      case BBT_TYPE_BME280:
         // Read 24 bytes of calibration data
         I2CReadRegister(&_bbi2c, _iAddr, 0x88, ucCal, 24); // 24 from register 0x88
         I2CReadRegister(&_bbi2c, _iAddr, 0xa1, &ucCal[24], 1); // get humidity calibration byte
         I2CReadRegister(&_bbi2c, _iAddr, 0xe1, &ucCal[25], 7); // get 7 more humidity calibration bytes
         // Prepare temperature calibration data
         _calT1 = (uint32_t)ucCal[0] + ((uint32_t)ucCal[1] << 8);
         _calT2 = (uint32_t)ucCal[2] + ((uint32_t)ucCal[3] << 8);
         if (_calT2 > 32767L) _calT2 -= 65536L; // negative value
         _calT3 = (uint32_t)ucCal[4] + ((uint32_t)ucCal[5] << 8);
         if (_calT3 > 32767L) _calT3 -= 65536L;

         // Prepare pressure calibration data
         _calP1 = (uint32_t)ucCal[6] + ((uint32_t)ucCal[7] << 8);
         _calP2 =(uint32_t) ucCal[8] + ((uint32_t)ucCal[9] << 8);
         if (_calP2 > 32767L) _calP2 -= 65536L; // signed short
         _calP3 = (uint32_t)ucCal[10] + ((uint32_t)ucCal[11] << 8);
         if (_calP3 > 32767L) _calP3 -= 65536L;
         _calP4 = (uint32_t)ucCal[12] + ((uint32_t)ucCal[13] << 8);
         if (_calP4 > 32767L) _calP4 -= 65536L;
         _calP5 = (uint32_t)ucCal[14] + ((uint32_t)ucCal[15] << 8);
         if (_calP5 > 32767L) _calP5 -= 65536L;
         _calP6 = (uint32_t)ucCal[16] + ((uint32_t)ucCal[17] << 8);
         if (_calP6 > 32767L) _calP6 -= 65536L;
         _calP7 = (uint32_t)ucCal[18] + ((uint32_t)ucCal[19] << 8);
         if (_calP7 > 32767L) _calP7 -= 65536L;
         _calP8 = (uint32_t)ucCal[20] + ((uint32_t)ucCal[21] << 8);
         if (_calP8 > 32767L) _calP8 -= 65536L;
         _calP9 = (uint32_t)ucCal[22] + ((uint32_t)ucCal[23] << 8);
         if (_calP9 > 32767L) _calP9 -= 65536L;

         // Prepare humidity calibration data
         _calH1 = (uint32_t)ucCal[24];
         _calH2 = (uint32_t)ucCal[25] + ((uint32_t)ucCal[26] << 8);
         if (_calH2 > 32767L) _calH2 -= 65536L;
         _calH3 = (uint32_t)ucCal[27];
         _calH4 = ((uint32_t)ucCal[28] << 4) + ((uint32_t)ucCal[29] & 0xf);
         if (_calH4 > 2047L) _calH4 -= 4096L; // signed 12-bit
         _calH5 = ((uint32_t)ucCal[30] << 4) + ((uint32_t)ucCal[29] >> 4);
         if (_calH5 > 2047L) _calH5 -= 4096L;
         _calH6 = (uint32_t)ucCal[31];
         if (_calH6 > 127L) _calH6 -= 256L; // signed char

         // Must set config in sleep mode
         ucTemp[0] = BME280_REG_CONFIG;
         ucTemp[1] = 0xa0; // set stand by time to 1 second
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 2); // config
         ucTemp[0] = BME280_REG_CTRL_HUM;
         ucTemp[1] = BME280_OVERSAMPLE1; // humidity over sampling rate = 1
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 2); // control humidity register
         ucTemp[0] = BME280_REG_CTRL_MEAS;
         ucTemp[1] = BME280_NORMAL_MODE | (BME280_OVERSAMPLE1 << 2) | (BME280_OVERSAMPLE1 << 5); // normal mode, temp and pressure over sampling rate=1
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 2); // control measurement register
         break;

      case BBT_TYPE_HDC1080:
         ucTemp[0] = HDC_REG_CONFIG; // config register
         ucTemp[1] = 0x10; // temp+humidity, 14-bit resolution
         ucTemp[2] = 0x00;
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 3);
         delay(100); // give it time to power up
         break; // HDC1080

      case BBT_TYPE_HTS221:
         {
         uint8_t h0rH, h1rH;
         uint16_t t0degC, t1degC;
         int16_t h0t0Out, h1t0Out, t0Out, t1Out;

         // Read calibration data
         I2CReadRegister(&_bbi2c, _iAddr, HTS221_H0_rH_x2_REG, &h0rH, 1);
         I2CReadRegister(&_bbi2c, _iAddr, HTS221_H1_rH_x2_REG, &h1rH, 1);
         I2CReadRegister(&_bbi2c, _iAddr, HTS221_T0_degC_x8_REG, ucTemp, 1);
         I2CReadRegister(&_bbi2c, _iAddr, HTS221_T1_degC_x8_REG, &ucTemp[1], 1);
         I2CReadRegister(&_bbi2c, _iAddr, HTS221_T1_T0_MSB_REG, &ucTemp[2], 1);
         t0degC = ucTemp[0] | ((ucTemp[2] & 0x03) << 8);
         t1degC = ucTemp[1] | ((ucTemp[2] & 0x0c) << 6);
         readMultiple(HTS221_H0_T0_OUT_REG, ucTemp, 2);
         readMultiple(HTS221_H1_T0_OUT_REG, &ucTemp[2], 2);
         h0t0Out = ucTemp[0] | (ucTemp[1] << 8);
         h1t0Out = ucTemp[2] | (ucTemp[3] << 8);
         readMultiple(HTS221_T0_OUT_REG, ucTemp, 2);
         readMultiple(HTS221_T1_OUT_REG, &ucTemp[2], 2);
         t0Out = ucTemp[0] | (ucTemp[1] << 8);
         t1Out = ucTemp[2] | (ucTemp[3] << 8);

         // calculate slopes and 0 offset from calibration values,
         // for future calculations: value = a * X + b

         _hts221HumiditySlope = (h1rH - h0rH) / (2.0f * (h1t0Out - h0t0Out));
         _hts221HumidityZero = (h0rH / 2.0f) - _hts221HumiditySlope * h0t0Out;
         _hts221TemperatureSlope = (float)(t1degC - t0degC) / (8.0f * (float)(t1Out - t0Out));
         _hts221TemperatureZero = ((float)t0degC / 8.0f) - _hts221TemperatureSlope * (float)t0Out;

         // turn it on
         ucTemp[0] = HTS221_CTRL1_REG; // control register 1
         ucTemp[1] = 0x80;
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 2);
         }
         break; // HTS221

      default:
         return BBT_ERROR; // unsupported type
   } // switch
   return BBT_SUCCESS;
} /* start() */

//
// Tell the sensor to stop sampling
//
void BBTemp::stop(void)
{
uint8_t ucTemp[4];

   switch (_iType) {
      case BBT_TYPE_AHT20:
         ucTemp[0] = AHT20_REG_RESET; // reset
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 1);
         break;

      case BBT_TYPE_BME280:
         ucTemp[0] = BME280_REG_CTRL_MEAS;
         ucTemp[1] = BME280_SLEEP_MODE;
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 2); // control measurement register
         break;

      case BBT_TYPE_HDC1080:
         ucTemp[0] = HDC_REG_CONFIG; // config register
         ucTemp[1] = 0x80; // software reset
         ucTemp[2] = 0x00;
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 3);
         break;
   } // switch
} /* stop() */

//
// Initialize the library
// It only needs to initialize the I2C interface; the chip is ready
//
int BBTemp::init(int iSDA, int iSCL, bool bBitBang, uint32_t u32Speed)
{
uint8_t ucTemp[4];
int rc, iOff;

    _iType = -1;
    _bbi2c.bWire = !bBitBang; // use bit bang?
    _bbi2c.iSDA = iSDA;
    _bbi2c.iSCL = iSCL;
    I2CInit(&_bbi2c, u32Speed);

    for (iOff=0; iOff<8; iOff++) { // I2C address permutations
        if (I2CTest(&_bbi2c, BBT_ADDR_AHT20+iOff)) { // check for AHT20
           rc = I2CReadRegister(&_bbi2c, BBT_ADDR_AHT20+iOff, AHT20_REG_STATUS, ucTemp, 1);
           if (rc && ((ucTemp[0] & 0x7f) == 0x18 || (ucTemp[0] & 0x7f) == 0x1c)) { // yes
               _iAddr = BBT_ADDR_AHT20+iOff;
               _iType = BBT_TYPE_AHT20;
               _u32Caps = BBT_CAP_TEMPERATURE | BBT_CAP_HUMIDITY;
               return BBT_SUCCESS;
           }
        } // if AHT20

        if (I2CTest(&_bbi2c, BBT_ADDR_HTS221+iOff)) { // check for HTS221
           rc = I2CReadRegister(&_bbi2c, BBT_ADDR_HTS221+iOff, HTS221_WHO_AM_I_REG, ucTemp, 1);
           if (rc && ucTemp[0] == HTS221_WHO_AM_I_VAL) {
              _iAddr = BBT_ADDR_HTS221+iOff;
              _iType = BBT_TYPE_HTS221;
              _u32Caps = BBT_CAP_TEMPERATURE | BBT_CAP_HUMIDITY;
              return BBT_SUCCESS;
           }
        } // if HTS221

        if (I2CTest(&_bbi2c, BBT_ADDR_BME280+iOff)) { // check for Bosch BME280
           rc = I2CReadRegister(&_bbi2c, BBT_ADDR_BME280+iOff, BME280_REG_WHOAMI, ucTemp, 1);
           if (rc && ucTemp[0] == BME280_DEVICE_ID) { // yes
               _iAddr = BBT_ADDR_BME280+iOff;
               _iType = BBT_TYPE_BME280;
               _u32Caps = BBT_CAP_TEMPERATURE | BBT_CAP_HUMIDITY | BBT_CAP_PRESSURE;
               return BBT_SUCCESS;
           }
        } // if BME280

        if (I2CTest(&_bbi2c, BBT_ADDR_HDC1080+iOff)) {
           rc = I2CReadRegister(&_bbi2c, BBT_ADDR_HDC1080+iOff, HDC_REG_DEVICEID, ucTemp, 2);
           if (rc && ucTemp[0] == (HDC_VAL_DEVICEID >> 8) && ucTemp[1] == (HDC_VAL_DEVICEID & 0xff)) { // yes
               _iAddr = BBT_ADDR_HDC1080+iOff;
               _iType = BBT_TYPE_HDC1080;
               _u32Caps = BBT_CAP_TEMPERATURE | BBT_CAP_HUMIDITY;
               return BBT_SUCCESS;
           }
        } // if HDC1080

        if (I2CTest(&_bbi2c, BBT_ADDR_SHT3X+iOff)) {
           ucTemp[0] = 0xf3; ucTemp[1] = 0x2d; // read status command
           I2CWrite(&_bbi2c, BBT_ADDR_SHT3X+iOff, ucTemp, 2);
           I2CRead(&_bbi2c, BBT_ADDR_SHT3X+iOff, ucTemp, 3); // read status bits
           if ((ucTemp[1] & 0x10) == 0x10) { // yes, it's the SHT3X
               _iAddr = BBT_ADDR_SHT3X+iOff;
               _iType = BBT_TYPE_SHT3X;
               _u32Caps = BBT_CAP_TEMPERATURE | BBT_CAP_HUMIDITY;
               return BBT_SUCCESS;
           }
        } // if SHT3X

    } // for each address permutation
    return BBT_ERROR;
} /* init() */

int BBTemp::type(void)
{
   return _iType;
} /* type() */

uint32_t BBTemp::caps(void)
{
   return _u32Caps;
} /* caps() */

int BBTemp::getSample(BBT_SAMPLE *pBS)
{
uint8_t ucTemp[8];
uint32_t ST, SRH;
int bReady;

   switch (_iType) {
      case BBT_TYPE_HTS221:
         {
         uint16_t t, h;
         float tf, hf;

         // trigger a sample
         ucTemp[0] = HTS221_CTRL2_REG;
         ucTemp[1] = 1;
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 2);
         // wait for completion
         bReady = 0;
         while (!bReady) {
           I2CReadRegister(&_bbi2c, _iAddr, HTS221_STATUS_REG, ucTemp, 1);
           bReady = ((ucTemp[0] & 3) == 3); // both temp & humidity samples ready
           delay(25);
         } // while
         // read ADC raw data for temp and humidity and 'fix' them
         readMultiple(HTS221_HUMIDITY_OUT_L_REG, ucTemp, 4);
         h = ucTemp[0] | ((uint16_t)ucTemp[1] << 8);
         t = ucTemp[2] | ((uint16_t)ucTemp[3] << 8);
         tf = (t * _hts221TemperatureSlope + _hts221TemperatureZero);
         hf = (h * _hts221HumiditySlope + _hts221HumidityZero);
         pBS->temperature = (int)(tf * 10.0f);
         pBS->humidity = 170 + (int)hf;
         }
         break;

      case BBT_TYPE_SHT3X:
         ucTemp[0] = (uint8_t)(SHT3X_MEAS_HIGHREP >> 8);
         ucTemp[1] = (uint8_t)SHT3X_MEAS_HIGHREP;
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 2);
         delay(50);
         I2CRead(&_bbi2c, _iAddr, ucTemp, 6);
         ST = (uint32_t)ucTemp[0] << 8;
         ST |= ucTemp[1];
         //if (ucTemp[2] != sht3x_crc8(ucTemp, 2))
         //   return SL_ERROR; // DEBUG
         SRH = (uint32_t)ucTemp[3] << 8;
         SRH |= ucTemp[4];
         //if (ucTemp[5] != sht3x_crc8(&ucTemp[3], 2))
         //   return SL_ERROR; // DEBUG
         ST *= 1750; // 10x temp
         ST >>= 16;
         ST -= 450;
         pBS->temperature = (int)ST;
         SRH *= 100; // RH
         SRH >>= 16;
         pBS->humidity = (int)SRH;
         break;

      case BBT_TYPE_AHT20:
         ucTemp[0] = AHT20_REG_MEASURE; // trigger a measurement
         ucTemp[1] = 0x33;
         ucTemp[2] = 0x00;
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 3);
         delay(75); // wait for measurement to complete
         I2CRead(&_bbi2c, _iAddr, ucTemp, 1); // first byte read is status byte
         if (ucTemp[0] & AHT20_BUSY)
            return BBT_NOT_READY;
         I2CRead(&_bbi2c, _iAddr, ucTemp, 6); // status + 5 data
         pBS->humidity = ucTemp[1] << 12;
         pBS->humidity |= ucTemp[2] << 4;
         pBS->humidity |= (ucTemp[3] >> 4);
         pBS->humidity = (pBS->humidity * 100) >> 20; // convert to integer percentage
         pBS->temperature = (ucTemp[3] & 0xf) << 16;
         pBS->temperature |= (ucTemp[4] << 8);
         pBS->temperature |= ucTemp[5];
         pBS->temperature >>= 10;
         pBS->temperature = ((pBS->temperature * 2000) >> 10) - 500; // get T in C x 10 for decimal place
         break;

      case BBT_TYPE_BME280:
         {
         uint8_t i, ucTemp[16];
         int32_t t, p, h; // raw sensor values
         int32_t var1,var2,t_fine;
         int64_t P_64;
         int64_t var1_64, var2_64;

         I2CReadRegister(&_bbi2c, _iAddr, 0xf7, ucTemp, 8); // start of data regs
         p = ((uint32_t)ucTemp[0] << 12) + ((uint32_t)ucTemp[1] << 4) + ((uint32_t)ucTemp[2] >> 4);
         t = ((uint32_t)ucTemp[3] << 12) + ((uint32_t)ucTemp[4] << 4) + ((uint32_t)ucTemp[5] >> 4);
         h = ((uint32_t)ucTemp[6] << 8) + (uint32_t)ucTemp[7];
         // Calculate calibrated temperature value
         // the value is 10x C (e.g. 260 = 26.0C)
         var1 = ((((t >> 3) - (_calT1 <<1))) * (_calT2)) >> 11;
         var2 = (((((t >> 4) - (_calT1)) * ((t>>4) - (_calT1))) >> 12) * (_calT3)) >> 14;
         t_fine = var1 + var2;
         t_fine = ((t_fine * 5 + 128) >> 8);
         pBS->temperature = (((uint32_t)t_fine*6553)>>16) - 15L; // for some reason, the reported temp is too high, subtract 1.5C

         // Calculate calibrated pressure value
         var1_64 = t_fine - 128000LL;
         var2_64 = var1_64 * var1_64 * (int64_t)_calP6;
         var2_64 = var2_64 + ((var1_64 * (int64_t)_calP5) << 17);
         var2_64 = var2_64 + (((int64_t)_calP4) << 35);
         var1_64 = ((var1_64 * var1_64 * (int64_t)_calP3)>>8) + ((var1_64 * (int64_t)_calP2)<<12);
         var1_64 = (((((int64_t)1)<<47)+var1_64))*((int64_t)_calP1)>>33;
         if (var1_64 == 0) {
            pBS->pressure = 0;
         } else {
            P_64 = 1048576LL - p;
            P_64 = (((P_64<<31)-var2_64)*3125LL)/var1_64;
            var1_64 = (((int64_t)_calP9) * (P_64>>13) * (P_64>>13)) >> 25;
            var2_64 = (((int64_t)_calP8) * P_64) >> 19;
            P_64 = ((P_64 + var1_64 + var2_64) >> 8) + (((int64_t)_calP7)<<4);
            pBS->pressure = (uint32_t)(P_64 / 100LL);
         }
         // Calculate calibrated humidity value
         var1 = (t_fine - 76800L);
         var1 = (((((h << 14) - ((_calH4) << 20) - ((_calH5) * var1)) +
                (16384L)) >> 15) * (((((((var1 * (_calH6)) >> 10) * (((var1 * (_calH3)) >> 11) + (32768L))) >> 10) + (2097152L)) * (_calH2) + 8192L) >> 14));
         var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * (_calH1)) >> 4));
         var1 = (var1 < 0? 0 : var1);
         var1 = ((uint32_t)var1 > 419430400UL ? 419430400UL : var1);
         var1 = (uint32_t)(var1 >> 12); // humidity * 1024;
         pBS->humidity = var1 >> 10; // humidity (0-100)
         }
         break;

      case BBT_TYPE_HDC1080:
         ucTemp[0] = HDC_REG_TEMPERATURE;
         I2CWrite(&_bbi2c, _iAddr, ucTemp, 1); // read temp register
         delay(15); // wait for conversion we just triggered
         I2CRead(&_bbi2c, _iAddr, ucTemp, 4); // read temp+humidity
         pBS->temperature = ((uint16_t)ucTemp[0]<<8) + ucTemp[1];
         pBS->humidity = ((uint16_t)ucTemp[2]<<8) + ucTemp[3];
         pBS->temperature = ((pBS->temperature * 1650) >> 16) - 400; // 10x temp
         pBS->temperature -= 84; // adjustment that seems to give a more accurate result
         break;

      default:
         return BBT_ERROR;
   } // switch
   return BBT_SUCCESS;
} /* getSample() */
