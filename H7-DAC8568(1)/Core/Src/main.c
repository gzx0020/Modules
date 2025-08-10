/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "memorymap.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dac8568.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
		// 在main.c中添加测试代码
uint8_t test_data[4] = {0x12,0x34,0x56,0x78};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void SPI_Test(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	__HAL_SPI_ENABLE(&hspi1);
//	SPI_Test();
#if DAC8568_VERSION
  /* ��ȡ�������� ��10V���Ҳ� */
  double data_wave[500] = {0};
  double temp_f = 2 * 3.1415926535 / (sizeof(data_wave) / sizeof(double));
  for (int i = 0; i < sizeof(data_wave) / sizeof(double); i++) {
    data_wave[i] = MAX_VOLTAGE * sin(temp_f * i);
  }
#else
  /* ��ȡ�������� ��5V���Ҳ� */
  double data_wave[500] = {0};
  double temp_f = 2 * 3.1415926535 / (sizeof(data_wave) / sizeof(double));
  for (int i = 0; i < sizeof(data_wave) / sizeof(double); i++) {
    data_wave[i] = MAX_VOLTAGE * sin(temp_f * i);
  }
#endif
  DAC8568_Init();
  DAC8568_Reset();
  DAC8568_Internal_Reference_Config(true);
#if DAC8568_VERSION
  /* ͨ��A���10V��ѹ */
  DAC8568_Set_Direct_Current(DAC_Channel_A, MAX_VOLTAGE);
#else
  /* ͨ��A���5V��ѹ */
  DAC8568_Set_Direct_Current(DAC_Channel_A, MAX_VOLTAGE);
			   
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
DAC8568_Set_Channel_Wave(DAC_Channel_B, data_wave,
                             sizeof(data_wave) / sizeof(double));

//  HAL_GPIO_WritePin(DAC8568_SYNC_GPIO_Port, DAC8568_SYNC_Pin, GPIO_PIN_RESET);
//  HAL_SPI_Transmit(&hspi1, test_data, 4, 100);
//  HAL_GPIO_WritePin(DAC8568_SYNC_GPIO_Port, DAC8568_SYNC_Pin, GPIO_PIN_SET);
//  HAL_Delay(1); // 添加可见间隔


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 12;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV4;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//void SPI_Test(void) 
//{
//    uint8_t test_data[4] = {0xAA, 0x55, 0xAA, 0x55};
//    
//    printf("--- SPI Test Start ---\r\n");
//    printf("SPI1->CR1: 0x%08lX\r\n", SPI1->CR1);
//    printf("SPI1->SR: 0x%08lX\r\n", SPI1->SR);
//    
//    while(1) {
//        // 手动控制SYNC（片选）
//        HAL_GPIO_WritePin(DAC8568_SYNC_GPIO_Port, DAC8568_SYNC_Pin, GPIO_PIN_RESET);
//        
//        // 检查SPI状态
//        if (hspi1.State != HAL_SPI_STATE_READY) {
//            printf("SPI Busy! State: %d\r\n", hspi1.State);
//        }
//        
//        // 发送测试数据
//        HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, test_data, 4, 100);
//        
//        if (status != HAL_OK) {
//            printf("SPI Error: %d, ErrorCode: 0x%08lX\r\n", status, hspi1.ErrorCode);
//        }
//        
//        // 释放片选
//        HAL_GPIO_WritePin(DAC8568_SYNC_GPIO_Port, DAC8568_SYNC_Pin, GPIO_PIN_SET);
//        
//        // 延时便于观察
//        HAL_Delay(10);
//        
//        // 显示传输计数
//        static uint32_t counter = 0;
//        if ((counter++ % 100) == 0) {
//            printf("Transfers: %lu, SR: 0x%08lX\r\n", counter, SPI1->SR);
//        }
//    }
//}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
