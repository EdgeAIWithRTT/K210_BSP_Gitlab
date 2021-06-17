#include <rtthread.h>
#include <rtdevice.h>

#include "timer.h"
#include "sysctl.h"

struct khwtimer
{
    rt_hwtimer_t time_device;
    timer_device_number_t devno;
    int chn;
    const char *name;
};

extern uint32_t timer_get_count(timer_device_number_t timer_number, timer_channel_number_t channel);
extern void timer_set_reload(timer_device_number_t timer_number, timer_channel_number_t channel, uint32_t count);

static int timer_isr(void *ctx)
{
    struct khwtimer *ktm = (struct khwtimer*)ctx;

    rt_device_hwtimer_isr(&ktm->time_device);

    return 0;
}

static void khwtimer_init(struct rt_hwtimer_device *timer, rt_uint32_t state)
{
    struct khwtimer *ktm = (struct khwtimer*)(timer->parent.user_data);

    if (state)
    {
        timer_init(ktm->devno);
    }
    else
    {
        timer_set_enable(ktm->devno, ktm->chn, 0);
    }
}

static rt_err_t khwtimer_start(struct rt_hwtimer_device *timer, rt_uint32_t cnt, rt_hwtimer_mode_t mode)
{
    struct khwtimer *ktm = (struct khwtimer*)(timer->parent.user_data);

    timer_set_reload(ktm->devno, ktm->chn, cnt);
    timer_irq_register(ktm->devno, ktm->chn,
                       mode == HWTIMER_MODE_ONESHOT,
                       1,
                       timer_isr, ktm);

    timer_set_enable(ktm->devno, ktm->chn, 1);

    return 0;
}

static void khwtimer_stop(struct rt_hwtimer_device *timer)
{
    struct khwtimer *ktm = (struct khwtimer*)(timer->parent.user_data);

    timer_set_enable(ktm->devno, ktm->chn, 0);
}

static rt_uint32_t khwtimer_count_get(struct rt_hwtimer_device *timer)
{
    struct khwtimer *ktm = (struct khwtimer*)(timer->parent.user_data);

    return timer_get_count(ktm->devno, ktm->chn);
}

static rt_err_t khwtimer_control(struct rt_hwtimer_device *timer, rt_uint32_t cmd, void *args)
{
    struct khwtimer *ktm = (struct khwtimer*)(timer->parent.user_data);
    rt_uint32_t *f = (rt_uint32_t *)args;

    return 0;
}

static const struct rt_hwtimer_ops _ops =
{
    khwtimer_init,
    khwtimer_start,
    khwtimer_stop,
    khwtimer_count_get,
    khwtimer_control
};

static struct rt_hwtimer_info _info =
{
    .maxcnt  = 0xffffffff,
    .cntmode = HWTIMER_CNTMODE_DW
};

#ifdef BSP_USING_TIMER0
static struct khwtimer _timer0 =
{
    .devno = TIMER_DEVICE_0,
    .name = "timer0",
    .chn = 0,
};
#endif

#ifdef BSP_USING_TIMER1
static struct khwtimer _timer1 =
{
    .devno = TIMER_DEVICE_1,
    .name = "timer1",
    .chn = 0,
};
#endif

#ifdef BSP_USING_TIMER2
static struct khwtimer _timer2 =
{
    .devno = TIMER_DEVICE_2,
    .name = "timer2",
    .chn = 0,
};
#endif

int k210_hwtimer_init(void)
{
    struct khwtimer *ktm;

    _info.maxfreq = sysctl_clock_get_freq(SYSCTL_CLOCK_TIMER0);
    _info.minfreq = _info.maxfreq;

#ifdef BSP_USING_TIMER0
    ktm = &_timer0;
    ktm->time_device.ops = &_ops;
    ktm->time_device.info = &_info;

    rt_device_hwtimer_register(&ktm->time_device, ktm->name, ktm);
#endif

#ifdef BSP_USING_TIMER1
    ktm = &_timer1;
    ktm->time_device.ops = &_ops;
    ktm->time_device.info = &_info;

    rt_device_hwtimer_register(&ktm->time_device, ktm->name, ktm);
#endif

#ifdef BSP_USING_TIMER2
    ktm = &_timer2;
    ktm->time_device.ops = &_ops;
    ktm->time_device.info = &_info;

    rt_device_hwtimer_register(&ktm->time_device, ktm->name, ktm);
#endif

    return 0;
}
INIT_DEVICE_EXPORT(k210_hwtimer_init);
