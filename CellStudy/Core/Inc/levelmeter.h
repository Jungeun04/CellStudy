#include "stm32f4xx_hal.h"

uint8_t* ledState(uint16_t level, uint8_t *ledStates);
void ledDirectDrive(uint8_t *ledStates);
void shiftOut(uint8_t data);
uint8_t calculateLevel(uint32_t adcValue);
void displayLevel(uint8_t level);
