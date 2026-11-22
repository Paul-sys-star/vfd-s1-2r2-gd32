/*
 * svpwm.h  —  空间矢量调制（SVPWM）
 *
 * 实现方式是"正弦参考 + 零序（共模）注入"，与教科书 7 段式扇区法在数学上等价：
 *   相电压 Van = 0.5·m·Vdc·cos(θ)，线性区 m ∈ [0, 1.1547]（=2/√3），
 *   m > 1.1547 进入过调制，输出按六边形边界钳位（6-step 极限）。
 * 相比扇区查表法，本实现无扇区判断分支，占用更少且无跳变。
 */
#ifndef SVPWM_H
#define SVPWM_H

#include <stdint.h>

#define SVPWM_M_MAX_LINEAR   1.1547f   /* 2/√3 */

/* m：调制指数（1.0 = 相电压峰值 0.5·Vdc）；theta：电角度（弧度，0~2π） */
void svpwm_duty(float m, float theta, float duty[3]);

/* 最小脉宽与占空比限幅（dmin = t_min / Ts，取值 0~0.1） */
void svpwm_limit(float duty[3], float dmin);

#endif /* SVPWM_H */
