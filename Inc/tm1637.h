#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx.h"

void tm1637Init(void);
void tm1637DisplayDecimal(uint16_t v, uint8_t displaySeparator);
void tm1637SetBrightness(uint8_t brightness);
