# STC3115 평가보드 테스트

제조사에서 API파일을 제공: [STSW-BAT001](https://www.st.com/en/embedded-software/stsw-bat001.html)  
github에서 찾을 수 있는 문서: [STC3115GenericDriver](https://github.com/st-sw/STC3115GenericDriver)
---
제조사 홈페이지에서 라이브러리를 제공합니다.  
같은 버전의 파일이 github에도 올라와 있으나 이슈 관리도 안되고 있고 코드도 약간 다릅니다.  
홈페이지에서 제공하는 파일이 더 최신인듯 합니다.  
github의 자료들은 참고만 하세요.  

## .ioc 파일 설정

저는 통신 프로세서로 stm32F4 MCU를 사용하고 있어 STM32CubeIDE 를 사용하여 프로그램을 작성하였습니다.  
![포트 및 핀셋팅]()


## stc3115_I2C.c 파일 수정

해당 파일은 사용하는 환경에 맞게 I2C 인테페이스를 수정하여야 합니다.  
저는 HAL Driver를 사용하여 구현하였습니다.

```C
#include "stc3115_I2C.h" 
#include "stc3115_Driver.h" 
#include "i2c.h"

int I2C_Write(int NumberOfBytes, int RegAddress , unsigned char *TxBuffer)
{
  int res = -1;
  HAL_StatusTypeDef checkError;
  checkError = HAL_I2C_Mem_Write(&hi2c1, STC3115_SLAVE_ADDRESS, RegAddress, I2C_MEMADD_SIZE_8BIT, TxBuffer, NumberOfBytes, 100);
  if(checkError==HAL_OK) res = 0;
  return(res);
}

int I2C_Read(int NumberOfBytes, int RegAddress , unsigned char *RxBuffer)
{
  int res = -1;
  HAL_StatusTypeDef checkError;
  checkError = HAL_I2C_Mem_Read(&hi2c1, STC3115_SLAVE_ADDRESS, RegAddress, I2C_MEMADD_SIZE_8BIT, RxBuffer, NumberOfBytes, 100);
  if(checkError==HAL_OK) res = 0;
	
  return(res);
}
```


## main.c 작성

I2C1을 통하여 읽어온 전압,방전시간,온도를 읽어와서 UART로 PC의 터미널에서 볼 수 있게 하였습니다.  
아직 배터리가 연결되어 있지 않아서 변화를 확인할 수 있는 값은 온도뿐이지만 통신이 정상적으로 되었음을 확인하였습니다.  

![터미널 캡처 사진](./)

```C
#include "stc3115_Driver.h"
#include <stdio.h>

#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

void SystemClock_Config(void);

int main(void)
{
	STC3115_ConfigData_TypeDef STC3115_ConfigData;
	STC3115_BatteryData_TypeDef STC3115_BatteryData;

  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();

  int8_t status1,status2;
  status1 = GasGauge_Initialization(&STC3115_ConfigData, &STC3115_BatteryData);
  HAL_UART_Transmit(&huart3, (uint8_t *)"Hello, PC!\r\n", 13, 10);
  char buffer[100]="AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

  while (1)
  {
	  HAL_Delay(5000);
	  status2 = GasGauge_Task(&STC3115_ConfigData, &STC3115_BatteryData);
	  sprintf(buffer," STATUS: %d %d, V: %d mV, time: %d s,Temp: %d °C\r\n",
			status1,
			status2,
			STC3115_BatteryData.Voltage,
			STC3115_BatteryData.RemTime,
			STC3115_BatteryData.Temperature/10
);
	  HAL_UART_Transmit(&huart3, (uint8_t*)buffer, sizeof(buffer), 10);
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

```
