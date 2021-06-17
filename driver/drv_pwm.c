#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "pwm.h"

typedef struct
{
    struct rt_device_pwm pwm;
    pwm_device_number_t devno;
    uint32_t chnmask;
    char pin[PWM_CHANNEL_MAX];
    fpioa_function_t iofunc;
}kpwm_t;

static rt_err_t _pwm_control(struct rt_device_pwm *device, int cmd, void *arg)
{
    struct rt_pwm_configuration *cfg = (struct rt_pwm_configuration*)arg;
    kpwm_t *kpwm = (kpwm_t*)device;
    int chn = cfg->channel;

    if (!(kpwm->chnmask & (1<<chn)))
        return -1;

    switch (cmd)
    {
    case PWM_CMD_ENABLE:
        pwm_set_enable(kpwm->devno, chn, 1);
        break;
    case PWM_CMD_DISABLE:
        pwm_set_enable(kpwm->devno, cfg->channel, 0);
        break;
    case PWM_CMD_SET:
    {
        double freq, duty = 0;

        freq = 1000000000/(double)cfg->period;
        if (cfg->pulse)
        {
            duty = (double)100/((double)cfg->period/cfg->pulse);
            duty = duty/100;
        }

        pwm_set_frequency(kpwm->devno, cfg->channel, freq, duty);
    }break;
    case PWM_CMD_GET:
        break;
    default:
        return -RT_EINVAL;
    }

    return 0;
}

static const struct rt_pwm_ops _ops =
{
    .control = _pwm_control,
};

#ifdef BSP_USING_PWM0
static kpwm_t _pwm0 = 
{
    .devno = PWM_DEVICE_0,
    .chnmask = 0,
    .iofunc = FUNC_TIMER0_TOGGLE1,
};
#endif

#ifdef BSP_USING_PWM1
static kpwm_t _pwm1 =
{
    .devno = PWM_DEVICE_1,
    .chnmask = 0,
    .iofunc = FUNC_TIMER1_TOGGLE1,
};
#endif

#ifdef BSP_USING_PWM2
static kpwm_t _pwm2 =
{
    .devno = PWM_DEVICE_2,
    .chnmask = 0,
    .iofunc = FUNC_TIMER2_TOGGLE1,
};
#endif

int rt_hw_pwm_init(void)
{
    kpwm_t *kpwm;

#ifdef BSP_USING_PWM0
    kpwm = &_pwm0;

#ifdef BSP_PWM0_CHN0_ENABLE
    kpwm->chnmask |= (1<<0);
    kpwm->pin[0] = BSP_PWM0_CHN0_PIN;
#endif

    pwm_init(kpwm->devno);
    rt_device_pwm_register(&kpwm->pwm, "pwm0", &_ops, kpwm);
#endif

#ifdef BSP_USING_PWM1
    kpwm = &_pwm1;

#ifdef BSP_PWM1_CHN0_ENABLE
    kpwm->chnmask |= (1<<0);
    kpwm->pin[0] = BSP_PWM1_CHN0_PIN;
#endif

    pwm_init(kpwm->devno);
    rt_device_pwm_register(&kpwm->pwm, "pwm1", &_ops, kpwm);
#endif

#ifdef BSP_USING_PWM2
    kpwm = &_pwm2;

#ifdef BSP_PWM2_CHN0_ENABLE
    kpwm->chnmask |= (1<<0);
    kpwm->pin[0] = BSP_PWM2_CHN0_PIN;
#endif

    pwm_init(kpwm->devno);
    rt_device_pwm_register(&kpwm->pwm, "pwm2", &_ops, kpwm);
#endif

    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_pwm_init);
