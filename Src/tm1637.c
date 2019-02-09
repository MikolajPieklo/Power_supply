#include "tm1637.h"


void _tm1637Start(void);
void _tm1637Stop(void);
void _tm1637ReadResult(void);
void _tm1637WriteByte(uint8_t b);
void _tm1637DelayUsec(uint32_t i);
void _tm1637ClkHigh(void);
void _tm1637ClkLow(void);
void _tm1637DioHigh(void);
void _tm1637DioLow(void);

// Configuration.



const char segmentMap[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0-7
    0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, // 8-9, A-F
    0x00
};


void tm1637Init(void)
{
	LL_APB2_GRP1_EnableClock	(LL_APB2_GRP1_PERIPH_GPIOB);

	LL_GPIO_SetPinOutputType	(GPIOB,LL_GPIO_PIN_0,LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinMode			(GPIOB,LL_GPIO_PIN_0,LL_GPIO_MODE_OUTPUT);
	//LL_GPIO_SetPinPull			(GPIOB,LL_GPIO_PIN_0,LL_GPIO_PULL_UP);
	LL_GPIO_SetPinSpeed			(GPIOB,LL_GPIO_PIN_0,LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetOutputPin		(GPIOB,LL_GPIO_PIN_0);

	LL_GPIO_SetPinOutputType	(GPIOB,LL_GPIO_PIN_1,LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinMode			(GPIOB,LL_GPIO_PIN_1,LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinPull			(GPIOB,LL_GPIO_PIN_1,LL_GPIO_PULL_UP);
	LL_GPIO_SetPinSpeed			(GPIOB,LL_GPIO_PIN_1,LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetOutputPin		(GPIOB,LL_GPIO_PIN_1);

    tm1637SetBrightness(8);
}

void tm1637DisplayDecimal(uint16_t v, uint8_t displaySeparator)
{
    uint8_t digitArr[4];
    for (uint8_t i = 0; i < 4; ++i) {
        digitArr[i] = segmentMap[v % 10];
        if (i == 2 && displaySeparator) {
            digitArr[i] |= 1 << 7;
        }
        v /= 10;
    }

    _tm1637Start();
    _tm1637WriteByte(0x40);
    _tm1637ReadResult();
    _tm1637Stop();

    _tm1637Start();
    _tm1637WriteByte(0xc0);
    _tm1637ReadResult();

    for (int i = 0; i < 4; ++i) {
        _tm1637WriteByte(digitArr[3 - i]);
        _tm1637ReadResult();
    }

    _tm1637Stop();
}

// Valid brightness values: 0 - 8.
// 0 = display off.
void tm1637SetBrightness(uint8_t brightness)
{
    // Brightness command:
    // 1000 0XXX = display off
    // 1000 1BBB = display on, brightness 0-7
    // X = don't care
    // B = brightness
    _tm1637Start();
    _tm1637WriteByte(0x87 + brightness);
    _tm1637ReadResult();
    _tm1637Stop();
}

void _tm1637Start(void)
{
    _tm1637ClkHigh();
    _tm1637DioHigh();
    _tm1637DelayUsec(2);
    _tm1637DioLow();
}

void _tm1637Stop(void)
{
    _tm1637ClkLow();
    _tm1637DelayUsec(2);
    _tm1637DioLow();
    _tm1637DelayUsec(2);
    _tm1637ClkHigh();
    _tm1637DelayUsec(2);
    _tm1637DioHigh();
}

void _tm1637ReadResult(void)
{
    _tm1637ClkLow();
    _tm1637DelayUsec(5);
    // while (dio); // We're cheating here and not actually reading back the response.
    _tm1637ClkHigh();
    _tm1637DelayUsec(2);
    _tm1637ClkLow();
}

void _tm1637WriteByte(uint8_t b)
{
    for (int i = 0; i < 8; ++i) {
        _tm1637ClkLow();
        if (b & 0x01) {
            _tm1637DioHigh();
        }
        else {
            _tm1637DioLow();
        }
        _tm1637DelayUsec(3);
        b >>= 1;
        _tm1637ClkHigh();
        _tm1637DelayUsec(3);
    }
}

void _tm1637DelayUsec(uint32_t i)
{
    for (; i>0; i--) {
        for (int j = 0; j < 10; ++j) {
            __asm__ __volatile__("nop\n\t":::"memory");
        }
    }
}

void _tm1637ClkHigh(void)
{
	LL_GPIO_SetOutputPin(GPIOB,LL_GPIO_PIN_0);
}

void _tm1637ClkLow(void)
{
	LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_0);
}

void _tm1637DioHigh(void)
{
	LL_GPIO_SetOutputPin(GPIOB,LL_GPIO_PIN_1);
}

void _tm1637DioLow(void)
{
	LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_1);
}
