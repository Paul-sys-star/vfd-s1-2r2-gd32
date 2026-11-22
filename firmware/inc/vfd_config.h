/*
 * vfd_config.h  —  VFD-S1-2R2 编译配置 / 引脚映射 / 硬件标定常数
 * 平台：GD32F103C8T6 @72MHz
 */
#ifndef VFD_CONFIG_H
#define VFD_CONFIG_H

#include "gd32f103_regs.h"

/* ----------------------------- 系统时钟 ----------------------------- */
#define SYSCLK_HZ            72000UL * 1000UL   /* 72 MHz */
#define APB1_HZ              (SYSCLK_HZ / 2)     /* 36 MHz */

/* ----------------------------- 功率级参数 ----------------------------- */
#define PWM_FREQ_HZ          8000U                       /* 载波 8kHz */
#define PWM_PERIOD           (SYSCLK_HZ / (2U * PWM_FREQ_HZ))  /* 中心对齐 4500 */
#define PWM_DEADTIME_DTG     0x88U                       /* ≈2.0µs @72MHz */
#define PWM_MIN_PULSE_NS     1200U                       /* 最小脉宽 1.2µs */

#define VBUS_NOMINAL_V       311.0f   /* 220VAC 整流后母线额定 */
#define VBUS_OV_TRIP_V       400.0f
#define VBUS_OV_RESET_V      380.0f
#define VBUS_UV_TRIP_V       200.0f
#define VBUS_UV_RESET_V      215.0f

#define MOTOR_V_RATED        220.0f   /* 电机额定线电压 */
#define MOTOR_F_BASE_MIN     10.0f    /* 基频下限 */
#define MOTOR_F_BASE_MAX     400.0f   /* 基频上限 */
#define MOTOR_F_MAX          400.0f   /* 最大输出频率 */
#define BOOST_VOLT_DEFAULT   12.0f    /* 0Hz 转矩提升电压 */
#define BOOST_RATIO_DEFAULT  6.0f     /* 转矩提升百分比（占额定电压） */

/* ----------------------------- 采样标定 ----------------------------- */
#define ADC_FULL_SCALE       4095.0f
#define ADC_VREF_MV          3300.0f

/* 母线电压：750kΩ / 5.1kΩ 分压，1 LSB = 0.806mV × (755.1/5.1) ≈ 0.119V */
#define VBUS_DIV_RATIO       (755100.0f / 5100.0f)
#define VBUS_ADC_OFFSET      0.0f

/* 母线电流：R_sh 2mΩ + 差分放大 ×25 → 50mV/A，中点 1.65V，量程 ±33A */
#define IDC_AMP_GAIN         25.0f
#define IDC_SHUNT_OHM        0.002f
#define IDC_V_PER_AMP        (IDC_SHUNT_OHM * IDC_AMP_GAIN)   /* 0.05 V/A */
#define IDC_ZERO_MV          1650.0f

/* 散热器 NTC：10kΩ B=3950，上拉 10kΩ 至 3.3V，查表换算温度 */
#define NTC_PULLUP_OHM       10000.0f

/* 输出电流额定：2.2kW / 220V / 0.85 / 0.97 ≈ 7.0A RMS，设计按 10.0A 器件裕量 */
#define I_OUT_RATED_A        10.0f
#define I_OC_LIMIT_A         22.0f     /* 逐波限流阈值（峰值） */
#define I_OC_TRIP_A          25.0f     /* 硬件/软件跳闸阈值 */
#define I_OVERLOAD_150_PCT_S 60.0f

/* ----------------------------- 温度保护 ----------------------------- */
#define TEMP_DERATE_C        85.0f     /* 降载波频率 */
#define TEMP_TRIP_C          95.0f
#define TEMP_RESET_C         75.0f

/* ----------------------------- 引脚映射 ----------------------------- */
/* PWM：TIMER0 三相互补
 *   CH0  = PA8   / CH0N = PA7
 *   CH1  = PA9   / CH1N = PB0
 *   CH2  = PA10  / CH2N = PB1
 */
#define PWM_CH0_PORT   GPIOA
#define PWM_CH0_PIN    8
#define PWM_CH0N_PORT  GPIOA
#define PWM_CH0N_PIN   7
#define PWM_CH1_PORT   GPIOA
#define PWM_CH1_PIN    9
#define PWM_CH1N_PORT  GPIOB
#define PWM_CH1N_PIN   0
#define PWM_CH2_PORT   GPIOA
#define PWM_CH2_PIN    10
#define PWM_CH2N_PORT  GPIOB
#define PWM_CH2N_PIN   1

/* IPM FO → TIMER0 BKIN（PB12），同时可作 GPIO 读状态 */
#define IPM_FO_PORT    GPIOB
#define IPM_FO_PIN     12

/* 继电器与输出
 *   K1 充电旁路继电器 : PC10
 *   故障继电器线圈     : PC9
 *   风扇              : PC8
 *   Y1 开路集电极      : PC11
 */
#define RELAY_K1_PORT   GPIOC
#define RELAY_K1_PIN    10
#define RELAY_FAULT_PORT GPIOC
#define RELAY_FAULT_PIN 9
#define FAN_PORT        GPIOC
#define FAN_PIN         8
#define Y1_PORT         GPIOC
#define Y1_PIN          11

/* RS485：USART0 重映射到 PB6(TX) / PB7(RX)，DE = PB9 */
#define RS485_DE_PORT   GPIOB
#define RS485_DE_PIN    9

/* TM1650 数码管：SCL = PB10，SDA = PB11（软件 I²C） */
#define TM_SCL_PORT     GPIOB
#define TM_SCL_PIN      10
#define TM_SDA_PORT     GPIOB
#define TM_SDA_PIN      11

/* 按键（低有效，内部上拉）
 *   ▲ = PB13  ▼ = PB14  PRG = PB15  RUN = PB3  STOP = PB4
 */
#define KEY_UP_PORT     GPIOB
#define KEY_UP_PIN      13
#define KEY_DN_PORT     GPIOB
#define KEY_DN_PIN      14
#define KEY_PR_PORT     GPIOB
#define KEY_PR_PIN      15
#define KEY_RUN_PORT    GPIOB
#define KEY_RUN_PIN     3
#define KEY_STOP_PORT   GPIOB
#define KEY_STOP_PIN    4

/* 数字输入 S1..S7：PC0..PC6（低有效，内部上拉） */
#define DIN_PORT        GPIOC
#define DIN_FIRST_PIN   0
#define DIN_COUNT       7

/* ADC 通道分配（ADC0 规则组顺序扫描）
 *   CH0 = PA0 母线电压
 *   CH1 = PA1 母线电流
 *   CH2 = PA2 散热器 NTC
 *   CH3 = PA3 面板电位器
 *   CH4 = PA4 AI1（0~10V 经 1/4 分压 → 0~2.5V）
 *   CH5 = PA5 AI2（0~10V 或 4~20mA 经 250Ω → 0~5V，再 1/2 分压）
 */
#define ADC_CH_VBUS     0
#define ADC_CH_IDC      1
#define ADC_CH_NTC      2
#define ADC_CH_POT      3
#define ADC_CH_AI1      4
#define ADC_CH_AI2      5
#define ADC_CH_COUNT    6

/* AO1：PA6 = TIMER2_CH0，PWM + RC 滤波输出 0~10V */
#define AO_PORT         GPIOA
#define AO_PIN          6

/* Flash 参数区：64KB Flash（1KB/页），使用最后两页做 A/B 双备份 */
#define FLASH_BASE_ADDR  0x08000000UL
#define FLASH_PAGE_SIZE  1024UL
#define PARAM_PAGE_A     62U
#define PARAM_PAGE_B     63U
#define PARAM_ADDR_A     (FLASH_BASE_ADDR + PARAM_PAGE_A * FLASH_PAGE_SIZE)
#define PARAM_ADDR_B     (FLASH_BASE_ADDR + PARAM_PAGE_B * FLASH_PAGE_SIZE)

#endif /* VFD_CONFIG_H */
