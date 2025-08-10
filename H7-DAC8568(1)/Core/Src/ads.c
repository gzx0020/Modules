
#include "ads.h"
#include "main.h"
#include "gpio.h"
#include "spi.h"
#include <stdio.h>

void delay_us(uint32_t udelay)
{
	uint32_t Delay = udelay * 72/4;
	do
	{
		__NOP();
	}
	while (Delay --);
}

void ADS1256_Read(uint8_t reg_address, uint8_t size, uint8_t *recv_buf)
{
    if (size > 11) size = 11; // 限制最大读取字节数为11
    uint8_t send_data[2] = {
        0x10 | (reg_address & 0x0F), // 构造命令1: 0001 rrrr
        0x00 | ((size - 1) & 0x0F)   // 构造命令2: 0000 nnnn
    };

    // 等待 DRDY 为高（就绪）
    while(HAL_GPIO_ReadPin(DRDY_GPIO_Port, DRDY_Pin) == 0);

    // CS 拉低
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);

    // 发送命令
    HAL_SPI_Transmit(&hspi1, send_data, 2, 100);

    // 延时 t₆（5 μs） - 根据手册要求添加
    delay_us(10); // 需实现微秒级延时函数

    // 接收数据
    HAL_SPI_Receive(&hspi1, recv_buf, size, 100);

    // CS 拉高
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
}

void ADS1256_Write(uint8_t reg_address, uint8_t data)
{
    uint8_t send_data[3] = {
        0x50 | (reg_address & 0x0F), // 构造命令1: 0101 rrrr
        0x00,                        // 构造命令2: 0000 0000（写入1字节）
        data                         // 数据字节
    };

    // 等待 DRDY 为高
    while(HAL_GPIO_ReadPin(DRDY_GPIO_Port, DRDY_Pin) == 0);

    // CS 拉低
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);

    // 发送数据
    HAL_SPI_Transmit(&hspi1, send_data, 3, 100);

    // CS 拉高
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
}


void ADS1256_Read_ADC_data(uint8_t *recv_buf)
{
    uint8_t send_data[1] = {0X01};   //RDATA 命令（读取数据）
		
		// 等待 DRDY 为高（就绪）
		while(HAL_GPIO_ReadPin(DRDY_GPIO_Port,DRDY_Pin)==0);
		
		// CS 拉低
		HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
		
		// 发送 RDATA 命令
		HAL_SPI_Transmit(&hspi1, send_data, 1, 100);
		
		// 延时 t₆（5 μs）
		delay_us(5);
		
		// 接收 3 字节数据（24位 ADC 值）
		HAL_SPI_Receive(&hspi1, recv_buf, 0x03, 100);
				
		// CS 拉高
		HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

}


// 函数：验证 STATUS 寄存器的芯片 ID（BIT7~BIT4）
void ADS1256_Verify_ID(void)
{
    uint8_t status_register = 0; // 存储读取的寄存器值
    // 读取 STATUS 寄存器（地址 00h）
    ADS1256_Read(ADS1256_STATUS, 1, &status_register);
		uint8_t chip_id = (status_register >> 4) & 0x0F;
		printf("Status Register: 0x%02X, Chip ID: 0x%01X\n", status_register, chip_id);
}



/**
 * @brief 读取指定通道的 ADC 数据并打印电压
 * @param channel_addr: 通道对应的寄存器地址（例如 0x08 对应 AIN0）
 * @param channel_number: 通道编号（用于打印标识，例如 0, 1, 2）
 */
void Read_ADC_Channel(uint8_t channel_addr, uint8_t channel_number)
{
    uint8_t ADC_Read[3] = {0}; // 存储 ADC 数据（3 字节）
    float voltage = 0.0f;

    // 设置通道地址
    ADS1256_Write(0x01, channel_addr); // 写入 ADCON 寄存器（0x01）配置通道
    HAL_Delay(2); // 等待稳定

    // 读取 ADC 数据
    ADS1256_Read_ADC_data(ADC_Read);

    // 转换为电压
    voltage = Convert_ADC_to_Voltage(ADC_Read);

    // 打印结果
    printf("AIN%d = %.4fV\r\n", channel_number, voltage);
}

/**
 * @brief 将 ADC 原始数据转换为电压值
 * @param adc_data: 3 字节的 ADC 数据（MSB -> LSB）
 * @return 转换后的电压值（单位：V）
 */
float Convert_ADC_to_Voltage(uint8_t *adc_data)
{
    int32_t raw_value = 0; // 24 位有符号整数

    // 组合 3 字节数据（假设为 24 位补码格式）
    raw_value |= ((int32_t)adc_data[0] << 16);
    raw_value |= ((int32_t)adc_data[1] << 8);
    raw_value |= ((int32_t)adc_data[2]);

    // 处理符号扩展（如果最高位为 1，则扩展为负数）
    if (raw_value & 0x800000) {
        raw_value |= 0xFF000000; // 补码扩展为 32 位
    }

    // 计算电压
    return raw_value * VOLTAGE_SCALE;
}


//初始化ADS1256
void ADS1256_Init(void)
{
// 初始化 GPIO 和 SPI 外设
MX_GPIO_Init();              // 初始化 GPIO 引脚（包括 SPI 和复位引脚）
MX_SPI1_Init();              // 初始化 SPI1 外设（用于与 ADS1256 通信）

// 等待 5ms（启动延时）
HAL_Delay(5);

// 复位 ADS1256
HAL_GPIO_WritePin(SPI_RST_GPIO_Port, SPI_RST_Pin, GPIO_PIN_RESET); // 拉低复位引脚（低电平复位）
HAL_Delay(65);                                                     // 保持低电平至少 50ms
HAL_GPIO_WritePin(SPI_RST_GPIO_Port, SPI_RST_Pin, GPIO_PIN_SET);   // 拉高复位引脚（退出复位）
HAL_Delay(150);                                                     // 等待设备稳定
Verify_SPI_Connection();
// 验证芯片 ID（读取 STATUS 寄存器 BIT7~BIT4）
ADS1256_Verify_ID(); // 验证芯片 ID 是否为工厂烧录值（默认 0x1）

// 配置 STATUS 寄存器（地址 0x00）
ADS1256_Write(ADS1256_STATUS, 0x06); // 0b00000110
    // BIT7~BIT4: ID[3:0] = 0x0（保留，由芯片固定）
    // BIT3 (ORDER): 0 = MSB First（数据输出顺序为高位在前）
    // BIT2 (ACAL): 1 = Auto-Calibration Enabled（启用自动校准）
    // BIT1 (BUFEN): 1 = Analog Input Buffer Enabled（启用模拟输入缓冲）
    // BIT0 (DRDY): Read Only（状态位，无需设置）

// 注释掉的 MUX 配置（用于单端输入 AIN0 - AINCOM）
// ADS1256_Write(ADS1256_MUX, 0x08); // MUXP_AIN0 | MUXN_AINCOM（AIN0 为正输入，AINCOM 为负输入）

// 配置 ADCON 寄存器（地址 0x02）
ADS1256_Write(ADS1256_ADCON, 0x00); // 0b00000000
    // BIT7~BIT5: 保留（默认 0）
    // BIT4~BIT2: PGA[2:0] = 000 = Gain = 1（增益 1，输入范围 ±VREF）
    // BIT1~BIT0: 保留（默认 0）

// 配置 DRATE 寄存器（地址 0x03）
ADS1256_Write(ADS1256_DRATE, ADS1256_DRATE_1000SPS); // 设置采样率为 1000 SPS
    // ADS1256_DRATE_1000SPS = 0xA1（参考宏定义）
    // 实际采样率需根据增益和 DRATE 值查表（见手册表 3-1）

// 配置 IO 寄存器（地址 0x04）
ADS1256_Write(ADS1256_IO, 0x00); // 默认配置（所有 I/O 引脚为输入或保留功能）             

}



void Verify_SPI_Connection(void) {
    uint8_t test_data[4] = {0xAA, 0x55, 0xAA, 0x55};
    uint8_t recv_data[4] = {0};
    
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, test_data, recv_data, 4, 100);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
    
    printf("SPI Test: Send %02X %02X %02X %02X | Recv %02X %02X %02X %02X\n",
           test_data[0], test_data[1], test_data[2], test_data[3],
           recv_data[0], recv_data[1], recv_data[2], recv_data[3]);
}

