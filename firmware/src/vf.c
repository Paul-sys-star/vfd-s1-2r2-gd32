/*
 * vf.c  —  V/F 控制核心
 *
 * 调制指数换算：
 *   SVPWM 线性区内  Van_peak = 0.5·m·Vdc
 *   目标            Van_peak = V_line_rms · √2 / √3 = 0.8165·V
 *   →  m = 2·0.8165·V / Vdc = 1.633·V / Vdc
 *   220V 电机 + 311V 母线时 m = 1.155 ≈ 2/√3，正好落在 SVPWM 线性区上限，
 *   这也是单相 220V 输入直接驱动 220V 三相电机的极限工况（母线跌落时需 AVR 抬压）。
 */
#include "vf.h"
#include "vfd_config.h"
#include <math.h>

#define TWO_PI  6.28318531f
#define V_RATIO 1.6330f          /* m = 1.633·V / Vdc */

void vf_init(vf_t *v)
{
    v->f_set    = 0.0f;
    v->f_out    = 0.0f;
    v->accel_s  = 5.0f;
    v->decel_s  = 5.0f;
    v->f_base   = 50.0f;
    v->v_rated  = MOTOR_V_RATED;
    v->boost_v  = BOOST_VOLT_DEFAULT;
    v->boost_pct = BOOST_RATIO_DEFAULT;
    v->curve    = 0;
    v->avr_en   = 1;
    v->theta    = 0.0f;
    v->v_out    = 0.0f;
    v->m_index  = 0.0f;
}

void vf_reset_output(vf_t *v)
{
    v->f_set   = 0.0f;
    v->f_out   = 0.0f;
    v->theta   = 0.0f;
    v->v_out   = 0.0f;
    v->m_index = 0.0f;
}

float vf_voltage_at(const vf_t *v, float f)
{
    float v;
    float fbase = (v->f_base < 1.0f) ? 1.0f : v->f_base;

    if (f >= fbase) {
        return v->v_rated;                    /* 恒压（弱磁）区 */
    }
    if (v->curve) {
        /* 平方转矩：u ∝ f²，适合风机水泵，低频电流小、噪音低 */
        float r = f / fbase;
        v = v->v_rated * r * r;
    } else {
        v = v->v_rated * f / fbase;
    }
    /* 低频转矩提升：按 (1 - f/fbase) 线性衰减叠加，到基频归零 */
    float boost = v->boost_v;
    float pct_boost = v->v_rated * v->boost_pct * 0.01f;
    if (pct_boost > boost) {
        boost = pct_boost;
    }
    v += boost * (1.0f - f / fbase);
    return v;
}

void vf_update(vf_t *v, float dt, float vbus_v)
{
    /* ---- 1. 加减速斜坡 ---- */
    float fbase = (v->f_base < 1.0f) ? 1.0f : v->f_base;
    float err = v->f_set - v->f_out;

    if (err > 0.0f) {
        float rate = fbase / (v->accel_s > 0.01f ? v->accel_s : 0.01f);
        v->f_out += rate * dt;
        if (v->f_out > v->f_set) v->f_out = v->f_set;
    } else if (err < 0.0f) {
        float rate = fbase / (v->decel_s > 0.01f ? v->decel_s : 0.01f);
        v->f_out -= rate * dt;
        if (v->f_out < v->f_set) v->f_out = v->f_set;
    }
    if (v->f_out < 0.0f) v->f_out = 0.0f;

    /* ---- 2. 电压与调制指数 ---- */
    v->v_out = vf_voltage_at(v, v->f_out);

    float vbus = (vbus_v > 150.0f) ? vbus_v : 150.0f;
    float m;

    if (v->avr_en) {
        /* AVR 开：用实测母线电压反算占空比，输出电压不随输入波动 */
        m = V_RATIO * v->v_out / vbus;
        if (m > 1.15f) m = 1.15f;            /* 线性区上限附近钳位，留 0.4% 余量 */
    } else {
        /* AVR 关：占空比按额定母线固定，输入偏低时输出电压同比下跌 */
        m = V_RATIO * v->v_out / VBUS_NOMINAL_V;
        if (m > 1.15f) m = 1.15f;
    }
    v->m_index = m;

    /* ---- 3. 电角度累加 ---- */
    v->theta += TWO_PI * v->f_out * dt;
    while (v->theta >= TWO_PI) {
        v->theta -= TWO_PI;
    }
}

void vf_deadtime_comp(float duty[3], const float i_phase[3], float dt_ratio)
{
    /* 死区导致实际占空比偏移：i>0 时占空比损失 t_dead/Ts，i<0 时多导通 */
    for (int k = 0; k < 3; k++) {
        float sgn = 0.0f;
        if (i_phase[k] > 0.05f)      sgn =  1.0f;
        else if (i_phase[k] < -0.05f) sgn = -1.0f;
        duty[k] += sgn * dt_ratio;
        if (duty[k] < 0.0f) duty[k] = 0.0f;
        if (duty[k] > 1.0f) duty[k] = 1.0f;
    }
}
