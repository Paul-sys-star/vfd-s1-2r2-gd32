/*
 * vf.h  —  V/F 曲线发生器、加减速斜坡、AVR 自动稳压、死区补偿
 */
#ifndef VF_H
#define VF_H

#include <stdint.h>

typedef struct {
    /* 指令与斜坡 */
    float f_set;        /* 目标频率 Hz */
    float f_out;        /* 斜坡输出频率 Hz */
    float accel_s;      /* 加速时间：0 → 基频 所需秒数 */
    float decel_s;      /* 减速时间 */
    /* 电机曲线 */
    float f_base;       /* 基频 Hz */
    float v_rated;      /* 额定输出电压（线电压 RMS） */
    float boost_v;      /* 0Hz 转矩提升电压 V */
    float boost_pct;    /* 转矩提升百分比（占额定电压） */
    uint8_t curve;      /* 0 = 线性 V/F，1 = 平方转矩 V/F */
    uint8_t avr_en;     /* 1 = 开启自动稳压 */
    /* 输出状态 */
    float theta;        /* 电角度（弧度） */
    float v_out;        /* 当前输出电压（线电压 RMS） */
    float m_index;      /* 调制指数（对应 svpwm_duty 的 m） */
} vf_t;

void  vf_init(vf_t *v);
void  vf_reset_output(vf_t *v);

/* 由频率求电压（含转矩提升与平方曲线），未做母线修正 */
float vf_voltage_at(const vf_t *v, float f);

/* 每 dt 秒调用一次（建议 1ms）：做斜坡、计算电压与调制指数、累加电角度 */
void  vf_update(vf_t *v, float dt, float vbus_v);

/* 死区补偿：按相电流符号补偿占空比，dt_ratio = t_dead / T_pwm */
void  vf_deadtime_comp(float duty[3], const float i_phase[3], float dt_ratio);

#endif /* VF_H */
