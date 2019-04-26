/*
 * ws2812b.cpp
 *
 *  Created on: Nov 12, 2018
 *      Author: 46sayu
 */

#include "ws2812b.hpp"

WS2812B::WS2812B(TIM_HandleTypeDef *htim, uint32_t channel, uint16_t num)
    : htim(htim),
      channel(channel),
      lednum(num),
      timClkFreq(20000000),  // Tclk = 50ns
      psc(0),
      period(25 - 1),  // 25 = 1250ns / 50ns (1250ns = 350ns + 900ns)
      bit0(7),     // 350ns / 50ns
      bit1(18)     // 900ns / 50ns
{
    colorBitBuf.resize(num * 24 + 1);   // num * (8bit * 3color) + 0bit
    for(int i = 0; i < num * 24; i++){
        colorBitBuf.push_back(0);
    }

}

WS2812B::~WS2812B() {
    // nothing to do
}

void WS2812B::init() {
    // make 20MHz -> 1clk = 50ns
    timClkFreq = HAL_RCC_GetPCLK1Freq() * 2;  // timer freq is faster 2times!
    if (timClkFreq > 20000000) {
        psc = (timClkFreq / 20000000);
    } else {
        psc = 1;  // no divide
    }
    __HAL_TIM_SET_PRESCALER(htim, psc - 1);  // 0 ~ n-1

    // set pwm ref value
    uint32_t t_1clk = 1000000000 / (timClkFreq / psc);  // time of 1 clock [ns]
    period = T_TOTAL / t_1clk;
    __HAL_TIM_SET_AUTORELOAD(htim, period - 1);
    bit0 = T_BIT_0 / t_1clk;
    bit1 = T_BIT_1 / t_1clk;
}

uint8_t WS2812B::setColors(RGB_t *colorary, uint16_t offset, uint16_t num) {
    if (offset + num > lednum) {
        return 0;  // too much number of led!
    }

    HAL_TIM_PWM_Stop_DMA(htim, channel);
    HAL_Delay(1);  // command reset time (optional)
    // set pwm datas
    for (uint16_t i = offset; i < num; i++) {
        // G->R->B (But color data is R->G->B)
        // G
        uint8_t val = colorary[i].cG;
        for (uint8_t j = 0; j < 8; j++) {  // 8 = bitnum of 1byte
            if (((val >> (7 - j)) & 1) == 0) {  // bit == 0
                colorBitBuf[(i * 24) + j] = bit0;  // 24 = 8 * 3colors
            } else {                            // bit == 1
                colorBitBuf[(i * 24) + j] = bit1;
            }
        }
        // R
        val = colorary[i].cR;
        for (uint8_t j = 0; j < 8; j++) {  // 8 = bitnum of 1byte
            if (((val >> (7 - j)) & 1) == 0) {  // bit == 0
                colorBitBuf[(i * 24) + 8 + j] = bit0;  // 8 = G data
            } else {                            // bit == 1
                colorBitBuf[(i * 24) + 8 + j] = bit1;
            }
        }
        // B
        val = colorary[i].cB;
        for (uint8_t j = 0; j < 8; j++) {  // 8 = bitnum of 1byte
            if (((val >> (7 - j)) & 1) == 0) {  // bit == 0
                colorBitBuf[(i * 24) + 16 + j] = bit0;  // 16 = G & R data
            } else {                            // bit == 1
                colorBitBuf[(i * 24) + 16 + j] = bit1;
            }
        }
    }
    HAL_TIM_PWM_Start_DMA(htim, channel, (uint32_t *)(colorBitBuf.data()), (uint16_t)(colorBitBuf.size()));
    return 1;
}
