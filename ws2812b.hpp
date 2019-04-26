/*
 * ws2812b.hpp
 *
 *  Created on: Nov 12, 2018
 *      Author: 46sayu
 */

#ifndef APPLICATION_USER_WS2812B_HPP_
#define APPLICATION_USER_WS2812B_HPP_

#include <vector>

#include "stm32f4xx_hal.h"

#define T_TOTAL 1250  // [ns]
#define T_BIT_0 350   // [ns]
#define T_BIT_1 900   // [ns]

struct RGB_t {
    uint8_t cR;
    uint8_t cG;
    uint8_t cB;
};

class WS2812B {
public:
    WS2812B(TIM_HandleTypeDef *htim,
            uint32_t channel,
            uint16_t num);
    virtual ~WS2812B();

    void init();

    uint8_t setColors(RGB_t *colorary, uint16_t offset, uint16_t num);

private:
    TIM_HandleTypeDef *htim;
    uint32_t channel;

    uint16_t lednum;
    std::vector<uint16_t> colorBitBuf;

    uint32_t timClkFreq;
    uint16_t psc;
    uint16_t period;
    uint16_t bit0;
    uint16_t bit1;

    void setBitBuf();
};

#endif /* APPLICATION_USER_WS2812B_HPP_ */
