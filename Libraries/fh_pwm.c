#include "fh_driverlib.h"

void PWM_Enable(struct fh_pwm_obj *pwm_obj, int enable)
{
    SET_REG(pwm_obj->base + OFFSET_PWM_CTRL, enable);
}

unsigned int PWM_GetPwmCmd(struct fh_pwm_obj *pwm_obj, int device_id)
{
    return GET_REG(pwm_obj->base + OFFSET_PWM_CMD(device_id));
}

void PWM_SetPwmCmd(struct fh_pwm_obj *pwm_obj, int device_id, unsigned int reg)
{
    SET_REG(pwm_obj->base + OFFSET_PWM_CMD(device_id), reg);
}
