/*
 * svpwm.c
 */
#include "svpwm.h"
#include <math.h>

#define SQRT3  1.73205081f
#define TWO_PI 6.28318531f

/*
 * 三相对称正弦参考（含 SVPWM 零序注入）
 *   sa = 0.5·m·cos θ
 *   sb = 0.5·m·cos(θ-120°)
 *   sc = 0.5·m·cos(θ+120°)
 *   offset = (max+min)/2，三项减去 offset 后加 0.5 → 占空比
 * 注入零序后，线电压不变，而线性调制范围扩大到 2/√3（比 SPWM 高 15.5%）。
 */
void svpwm_duty(float m, float theta, float duty[3])
{
    if (m < 0.0f) {
        m = 0.0f;
    }
    if (m > 2.0f) {
        m = 2.0f;                     /* 过调制硬限幅，避免数值发散 */
    }

    float sa = 0.5f * m * cosf(theta);
    float sb = 0.5f * m * cosf(theta - TWO_PI / 3.0f);
    float sc = 0.5f * m * cosf(theta + TWO_PI / 3.0f);

    float vmax = sa, vmin = sa;
    if (sb > vmax) vmax = sb;
    if (sc > vmax) vmax = sc;
    if (sb < vmin) vmin = sb;
    if (sc < vmin) vmin = sc;

    float offset = 0.5f * (vmax + vmin);
    duty[0] = 0.5f + sa - offset;
    duty[1] = 0.5f + sb - offset;
    duty[2] = 0.5f + sc - offset;
}

/* 最小脉宽钳位：占空比限制在 [dmin, 1-dmin]，防止窄脉冲丢失造成电流畸变 */
void svpwm_limit(float duty[3], float dmin)
{
    if (dmin < 0.0f) dmin = 0.0f;
    if (dmin > 0.1f) dmin = 0.1f;

    for (int i = 0; i < 3; i++) {
        if (duty[i] < dmin)      duty[i] = dmin;
        if (duty[i] > 1.0f - dmin) duty[i] = 1.0f - dmin;
    }
}
