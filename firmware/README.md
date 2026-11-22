# 固件说明（VFD-S1-2R2）

寄存器级实现，不依赖 GD32 官方固件库；`inc/gd32f103_regs.h` 只定义本工程用到的外设。
目标芯片 **GD32F103C8T6**（Cortex-M3 @72MHz，64KB Flash / 20KB SRAM），编译器 `arm-none-eabi-gcc ≥ 10`。

## 文件职责

| 文件 | 说明 |
|---|---|
| `inc/gd32f103_regs.h` | RCU/GPIO/AFIO/TIMER/ADC/USART/FLASH/SysTick 寄存器定义 |
| `inc/vfd_config.h` | 载波、死区、保护定值、采样标定、全部引脚映射 |
| `inc/board.h` `src/board.c` | 72MHz 时钟树、GPIO 初始化、SysTick 1ms |
| `src/svpwm.c` | 零序注入 SVPWM（等价七段式），含过调制与最小脉宽 |
| `src/vf.c` | V/F 曲线、加减速斜坡、AVR、死区补偿 |
| `src/protect.c` | 逐波限流、I²t、过压/欠压/过温、IPM FO |
| `src/timer_pwm.c` | TIMER0 三相互补 PWM、死区 2µs、BKIN 封锁 |
| `src/adc_sense.c` | ADC0 六通道扫描 + 工程量换算 |
| `src/param.c` `src/flash_param.c` | 参数表、A/B 双页 Flash 备份 + CRC16 |
| `src/keypad.c` | TM1650 数码管、5 键消抖 |
| `src/modbus.c` | Modbus-RTU 从站（03/06/16） |
| `src/control.c` `src/main.c` | 状态机与前后台调度 |
| `startup/startup_gd32f103c8.c` | C 语言向量表 + Reset_Handler（免汇编） |
| `linker/gd32f103c8.ld` | Flash 62KB（末两页留给参数区）/ RAM 20KB |

## 引脚表

| 功能 | 引脚 | 说明 |
|---|---|---|
| PWM U 高/低 | PA8 / PA7 | TIMER0_CH0 / CH0N |
| PWM V 高/低 | PA9 / PB0 | TIMER0_CH1 / CH1N |
| PWM W 高/低 | PA10 / PB1 | TIMER0_CH2 / CH2N |
| IPM FO | PB12 | 兼作 TIMER0_BKIN，硬件封锁 |
| ADC 母线电压 | PA0 | ADC0_IN0，750k/5.1k 分压 |
| ADC 母线电流 | PA1 | ADC0_IN1，2mΩ + ×25 |
| ADC NTC | PA2 | ADC0_IN2 |
| ADC 电位器 | PA3 | ADC0_IN3 |
| ADC AI1 / AI2 | PA4 / PA5 | ADC0_IN4 / IN5 |
| K1 继电器 | PC10 | 高有效 |
| 故障继电器 | PC9 | 高有效（TA-TC 闭合） |
| 风扇 | PC8 | 高有效 |
| Y1 输出 | PC11 | 开路集电极 |
| 数字输入 S1~S7 | PC0~PC6 | 低有效，内部上拉 |
| 485 TX / RX | PB6 / PB7 | USART0 重映射 |
| 485 DE | PB9 | 高 = 发送 |
| 数码管 SCL / SDA | PB10 / PB11 | 软件 I²C，外部 4.7k 上拉 |
| 按键 | PB13 ▲ / PB14 ▼ / PB15 PRG / PB3 RUN / PB4 STOP | 低有效 |

> PB3/PB4 上电需先关闭 JTAG（保留 SWD），已在 `board_init()` 中处理；烧写请用 SWD 四线。

## 调度

- **125µs**（TIMER0 更新中断）：读采样快照 → 逐波限流 → SVPWM → 死区补偿 → 写 CC 寄存器
- **1ms**（主循环，SysTick 时基）：保护慢判 → 状态机 → 预充电时序 → 载波降额 → 按键 → V/F 更新 → 显示
- **5ms**：按键扫描
- **异步**：ADC EOC（采样）、BKIN（硬件封锁）、USART0（485 接收）

## 首次上电前必须标定

1. **电流零点**：短路 R_sh 输入，读 ADC 原始值应在 2048±30，否则调整 `IDC_ZERO_MV`。
2. **母线分压比**：用稳压源加 300V，核对 `g_sense.vbus_v` 与万用表一致，偏差 >2% 改 `VBUS_DIV_RATIO`。
3. **NTC 曲线**：25℃ 与 80℃ 两点核对 `protect_ntc_celsius()`。
4. **FO 极性**：拔掉 IPM 一侧的 FO 上拉，确认故障时 MCU 读到低电平。
5. **自举充电**：先给 15V 驱动电源，示波器看三相上桥 GE 波形，确认自举电容能在低占空比下维持驱动电压（>13V）。
6. **I²t 系数**：按实际电机额定电流整定 `P0.11`，然后做 150%/60s 温升试验，必要时修正 `OVERLOAD_TRIP`。

## 已知限制

- 浮点三角函数在 C8T6（无 FPU）上耗时较长，`control_pwm_isr()` 实测约 30~45µs；载波提到 16kHz（周期 62.5µs）时余量不足，建议改用查表 + 线性插值，或换 GD32F303（带 FPU）。
- ADC 采用连续扫描而非定时器触发，开关噪声较大；抗干扰要求高的场合请改由 TIMER0 CC1 触发采样。
- 未实现：制动斩波控制、S 曲线加减速、参数上传下载、故障录波。
