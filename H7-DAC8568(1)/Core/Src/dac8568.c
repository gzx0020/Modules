#include "dac8568.h"
#include "gpio.h"
#include "spi.h"

/**
 * @brief DAC8568写入寄存器
 *
 * @param data uint32_t 数据
 */
static void DAC8568_Write_Register(uint32_t data) {
  uint8_t write_data[4] = {0};
  write_data[0] = (data >> 24) & 0xFF;
  write_data[1] = (data >> 16) & 0xFF;
  write_data[2] = (data >> 8) & 0xFF;
  write_data[3] = data & 0xFF;
	

	
	
  HAL_GPIO_WritePin(DAC8568_SYNC_GPIO_Port, DAC8568_SYNC_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, write_data, sizeof(write_data), 1000);
  HAL_GPIO_WritePin(DAC8568_SYNC_GPIO_Port, DAC8568_SYNC_Pin, GPIO_PIN_SET);
	

  HAL_GPIO_WritePin(DAC8568_LDAC_GPIO_Port, DAC8568_LDAC_Pin, GPIO_PIN_RESET);
//	asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"); // 插入4个空周期(84MHz下约47ns)
  HAL_GPIO_WritePin(DAC8568_LDAC_GPIO_Port, DAC8568_LDAC_Pin, GPIO_PIN_SET);
}

/**
 * @brief DAC8568设置通道输出电压
 *
 * @param channel uint8_t 通道
 * @param voltage double  电压
 */
static void DAC8568_Set_Voltage(uint8_t channel, double voltage) {
#if DAC8568_VERSION
  double real_voltage = voltage / 4.0 + 2.5;
  uint16_t binary_voltage =
      (uint16_t)(real_voltage / 2.0 * 1000.0 / VREF * 0xFFFF);
#else
  double real_voltage = voltage / 4.0 + 1.25;
  uint16_t binary_voltage = (uint16_t)(real_voltage * 1000.0 / VREF * 0xFFFF);
#endif
  uint32_t write_data = ((uint32_t)Write_Update_Respective_DAC_Reg << 24) |
                        ((uint32_t)channel << 20) | (binary_voltage << 4);
  DAC8568_Write_Register(write_data);
}

/**
 * @brief DAC8568设置直流输出
 *
 * @param channel uint8_t 通道
 * @param voltage double  电压
 */
void DAC8568_Set_Direct_Current(uint8_t channel, double voltage) {
  DAC8568_Set_Voltage(channel, voltage);
}

/**
 * @brief DAC8568设置通道波形输出
 *
 * @param channel uint8_t 通道
 * @param wave_data double  波形数据
 * @param data_size uint16_t  数据大小
 */
void DAC8568_Set_Channel_Wave(uint8_t channel, double *wave_data,
                              uint16_t data_size) {
  for (uint16_t i = 0; i < data_size; i++, wave_data++) {
    DAC8568_Set_Voltage(channel, *wave_data);
  }
}

/**
 * @brief DAC8568内部基准电压配置
 *
 * @param state bool ture:内部2.5V基准电压  false:外部基准电压
 */
void DAC8568_Internal_Reference_Config(bool state) {
  uint32_t write_data = ((uint32_t)Internal_Reference << 24) | (uint8_t)state;
  DAC8568_Write_Register(write_data);
}

/**
 * @brief DAC8568复位
 *
 */
void DAC8568_Reset(void) {
  uint32_t write_data = (uint32_t)Software_Reset << 24;
  DAC8568_Write_Register(write_data);
}

/**
 * @brief DAC8568初始化
 *
 */
void DAC8568_Init(void) {
  /* 初始化GPIO */
  MX_GPIO_Init();
  /* 初始化硬件SPI */
  MX_SPI1_Init();

  HAL_GPIO_WritePin(DAC8568_CLR_GPIO_Port, DAC8568_CLR_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DAC8568_SYNC_GPIO_Port, DAC8568_SYNC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DAC8568_LDAC_GPIO_Port, DAC8568_LDAC_Pin, GPIO_PIN_SET);
}
