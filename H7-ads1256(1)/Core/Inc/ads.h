#ifndef __ADS_H__
#define __ADS_H__

#include "stm32h7xx_hal.h"
#include "main.h"
#include "gpio.h"
#include "spi.h"

// AIN0 - AINCOM (单端输入)
#define AIN0_ADDR 0x08 // 0x00 (AIN0) + 0x08 (AINCOM)
#define AIN1_ADDR 0x18 // 0x10 (AIN1) + 0x08 (AINCOM)
#define AIN2_ADDR 0x28 // 0x20 (AIN2) + 0x08 (AINCOM)
#define AIN3_ADDR 0x38 // 0x30 (AIN3) + 0x08 (AINCOM)
#define AIN4_ADDR 0x48 // 0x40 (AIN4) + 0x08 (AINCOM)
#define AIN5_ADDR 0x58 // 0x50 (AIN5) + 0x08 (AINCOM)
#define AIN6_ADDR 0x68 // 0x60 (AIN6) + 0x08 (AINCOM)
#define AIN7_ADDR 0x78 // 0x70 (AIN7) + 0x08 (AINCOM)

// 定义通道地址映射


// 定义电压转换系数（根据参考电压和分辨率计算）
#define VOLTAGE_SCALE 0.000000596 // 示例值，需根据实际硬件校准

//命令字定义
#define ADS1256_CMD_WAKEUP   0x00  // 唤醒命令：从待机模式唤醒设备
#define ADS1256_CMD_RDATA    0x01  // 读取单个数据（Read Data）
#define ADS1256_CMD_RDATAC   0x03  // 开启连续数据读取模式（Read Data Continuous）
#define ADS1256_CMD_SDATAC   0x0F  // 停止连续数据读取模式（Stop Data Continuous）
#define ADS1256_CMD_RREG     0x10  // 读取寄存器（Read Register）
#define ADS1256_CMD_WREG     0x50  // 写入寄存器（Write Register）
#define ADS1256_CMD_SELFCAL  0xF0  // 自动校准（Self-Calibration）
#define ADS1256_CMD_SELFOCAL 0xF1  // 自动偏移校准（Self-Offset Calibration）
#define ADS1256_CMD_SELFGCAL 0xF2  // 自动增益校准（Self-Gain Calibration）
#define ADS1256_CMD_SYSOCAL  0xF3  // 系统偏移校准（System Offset Calibration）
#define ADS1256_CMD_SYSGCAL  0xF4  // 系统增益校准（System Gain Calibration）
#define ADS1256_CMD_SYNC     0xFC  // 同步命令（Sync）
#define ADS1256_CMD_STANDBY  0xFD  // 进入待机模式（Standby）
#define ADS1256_CMD_REST     0xFE  // 复位命令（Reset Device）
 
//寄存器地址定义
#define ADS1256_STATUS       0x00  // 状态寄存器（Status Register）  
#define ADS1256_MUX          0x01  // 多路复用器控制寄存器（Multiplexer Control Register）  
#define ADS1256_ADCON        0x02  // ADC控制寄存器（ADC Control Register）  
#define ADS1256_DRATE        0x03  // 数据速率寄存器（Data Rate Register）  
#define ADS1256_IO           0x04  // I/O配置寄存器（I/O Configuration Register）  
#define ADS1256_OFC0         0x05  // 自动校准偏移寄存器0（Offset Calibration Register 0）  
#define ADS1256_OFC1         0x06  // 自动校准偏移寄存器1（Offset Calibration Register 1）  
#define ADS1256_OFC2         0x07  // 自动校准偏移寄存器2（Offset Calibration Register 2）  
#define ADS1256_FSC0         0x08  // 自动校准增益寄存器0（Full-Scale Calibration Register 0）  
#define ADS1256_FSC1         0x09  // 自动校准增益寄存器1（Full-Scale Calibration Register 1）  
#define ADS1256_FSC2         0x0A  // 自动校准增益寄存器2（Full-Scale Calibration Register 2）  
 
 
//多路复用器配置
// 正输入通道选择（MUXP_xxx）
#define ADS1256_MUXP_AIN0   0x00  // AIN0 作为正输入
#define ADS1256_MUXP_AIN1   0x10  // AIN1 作为正输入
#define ADS1256_MUXP_AIN2   0x20  // AIN2 作为正输入
#define ADS1256_MUXP_AIN3   0x30  // AIN3 作为正输入
#define ADS1256_MUXP_AIN4   0x40  // AIN4 作为正输入
#define ADS1256_MUXP_AIN5   0x50  // AIN5 作为正输入
#define ADS1256_MUXP_AIN6   0x60  // AIN6 作为正输入
#define ADS1256_MUXP_AIN7   0x70  // AIN7 作为正输入
#define ADS1256_MUXP_AINCOM 0x80  // AINCOM 作为正输入（参考地）

// 负输入通道选择（MUXN_xxx）
#define ADS1256_MUXN_AIN0   0x00  // AIN0 作为负输入
#define ADS1256_MUXN_AIN1   0x01  // AIN1 作为负输入
#define ADS1256_MUXN_AIN2   0x02  // AIN2 作为负输入
#define ADS1256_MUXN_AIN3   0x03  // AIN3 作为负输入
#define ADS1256_MUXN_AIN4   0x04  // AIN4 作为负输入
#define ADS1256_MUXN_AIN5   0x05  // AIN5 作为负输入
#define ADS1256_MUXN_AIN6   0x06  // AIN6 作为负输入
#define ADS1256_MUXN_AIN7   0x07  // AIN7 作为负输入
#define ADS1256_MUXN_AINCOM 0x08  // AINCOM 作为负输入（参考地） 
 
 
//增益配置
#define ADS1256_GAIN_1      0x00  // 增益 1（输入范围 ±VREF）
#define ADS1256_GAIN_2      0x01  // 增益 2（输入范围 ±VREF/2）
#define ADS1256_GAIN_4      0x02  // 增益 4（输入范围 ±VREF/4）
#define ADS1256_GAIN_8      0x03  // 增益 8（输入范围 ±VREF/8）
#define ADS1256_GAIN_16     0x04  // 增益 16（输入范围 ±VREF/16）
#define ADS1256_GAIN_32     0x05  // 增益 32（输入范围 ±VREF/32）
#define ADS1256_GAIN_64     0x06  // 增益 64（输入范围 ±VREF/64）
 
//数据速率配置
#define ADS1256_DRATE_30000SPS   0xF0  // 30,000 SPS
#define ADS1256_DRATE_15000SPS   0xE0  // 15,000 SPS
#define ADS1256_DRATE_7500SPS    0xD0  // 7,500 SPS
#define ADS1256_DRATE_3750SPS    0xC0  // 3,750 SPS
#define ADS1256_DRATE_2000SPS    0xB0  // 2,000 SPS
#define ADS1256_DRATE_1000SPS    0xA1  // 1,000 SPS
#define ADS1256_DRATE_500SPS     0x92  // 500 SPS
#define ADS1256_DRATE_100SPS     0x82  // 100 SPS
#define ADS1256_DRATE_60SPS      0x72  // 60 SPS
#define ADS1256_DRATE_50SPS      0x63  // 50 SPS
#define ADS1256_DRATE_30SPS      0x53  // 30 SPS
#define ADS1256_DRATE_25SPS      0x43  // 25 SPS
#define ADS1256_DRATE_15SPS      0x33  // 15 SPS
#define ADS1256_DRATE_10SPS      0x23  // 10 SPS
#define ADS1256_DRATE_5SPS       0x13  // 5 SPS
#define ADS1256_DRATE_2_5SPS     0x03  // 2.5 SPS

extern SPI_HandleTypeDef hspi1;

void ADS1256_Init(void);   //初始化ADS1256

void ADS1256_Read(uint8_t address, uint8_t size,uint8_t *recv_buf );


void ADS1256_Write(uint8_t address,uint8_t number);
void ADS1256_Read_ADC_data(uint8_t *recv_buf);

void Read_ADC_Channel(uint8_t channel_addr, uint8_t channel_number);
float Convert_ADC_to_Voltage(uint8_t *adc_data);
void ADS1256_Verify_ID(void);
void Verify_SPI_Connection(void);
void ADS1256_SendCommand(uint8_t command);
void ADS1256_Read_MultiChannels(uint8_t *channels, uint8_t num_channels, uint32_t sample_count, float *samples);

#endif /* __ADS_H__ */

