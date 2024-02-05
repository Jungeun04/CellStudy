#include "levelmeter.h"

GPIO_TypeDef* LED_DIRECT_PORTS[] = {GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOC, GPIOC, GPIOC, GPIOC, GPIOD};
uint16_t LED_DIRECT_PINS[] = {GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_3, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_2};

GPIO_TypeDef* LED_SHIFT_PORTS[] = {GPIOC, GPIOC, GPIOC};
uint16_t LED_SHIFT_PINS[] = {GPIO_PIN_9, GPIO_PIN_12, GPIO_PIN_11};	//DATA,CLK,storage CLK

// LED 상태 계산
uint8_t* ledState(uint16_t level, uint8_t* ledStates)
{
	for (int i = 0; i < 10; i++) {
		ledStates[i] = (i <= level) ? 1 : 0;
	}

	return ledStates;
}


// LED 제어
void ledDirectDrive(uint8_t* ledStates)
{

	for (int i = 0; i < 10; i++) {
	    HAL_GPIO_WritePin(LED_DIRECT_PORTS[i], LED_DIRECT_PINS[i], ledStates[i] ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}
}


void shiftOut(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        // DS 핀에 데이터 비트 설정 (MSB first)
        HAL_GPIO_WritePin(LED_SHIFT_PORTS[0], LED_SHIFT_PINS[0], (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        data <<= 1; // 다음 비트 준비

        // SH_CP 클록 핀에 상승 에지 제공
        HAL_GPIO_WritePin(LED_SHIFT_PORTS[1], LED_SHIFT_PINS[1], GPIO_PIN_RESET); // 클록을 LOW로
        HAL_GPIO_WritePin(LED_SHIFT_PORTS[1], LED_SHIFT_PINS[1], GPIO_PIN_SET);   // 클록을 HIGH로 (상승 에지)
        HAL_GPIO_WritePin(LED_SHIFT_PORTS[1], LED_SHIFT_PINS[1], GPIO_PIN_RESET); // 다음 사이클을 위해 클록을 다시 LOW로

        // 이 지점에서 소량의 지연을 추가할 수도 있지만, 대부분의 경우 필요하지 않습니다.
    }

    // ST_CP 클록 핀에 상승 에지 제공하여 저장 레지스터로 데이터 이동
    HAL_GPIO_WritePin(LED_SHIFT_PORTS[2], LED_SHIFT_PINS[2], GPIO_PIN_RESET); // 클록을 LOW로
    HAL_GPIO_WritePin(LED_SHIFT_PORTS[2], LED_SHIFT_PINS[2], GPIO_PIN_SET);   // 클록을 HIGH로 (상승 에지)
    HAL_GPIO_WritePin(LED_SHIFT_PORTS[2], LED_SHIFT_PINS[2], GPIO_PIN_RESET); // 다음 사이클을 위해 클록을 다시 LOW로
}


uint8_t calculateLevel(uint32_t adcValue) {
    // ADC 값에서 10단계 레벨을 계산
    return adcValue / 410; // 0부터 4095 범위를 10단계로 나눔
}


void displayLevel(uint8_t level) {
    uint8_t displayData = 0;
    // 레벨에 따라 켜야 할 LED 수를 계산하여 displayData에 설정
    for(int i = 0; i < level; i++) {
        displayData |= (1 << i); // i 번째 비트를 켬
    }
    // 74HC595로 데이터 전송
    shiftOut(displayData);
}



