#include "bsp_motor.h"
#include "nrf_drv_gpiote.h"

void Foreward_Motor(void)
{
    nrf_gpio_pin_set(INA);	
    nrf_gpio_pin_clear(INB);	
}

void Inversion_Motor(void)
{
    nrf_gpio_pin_clear(INA);	
	  nrf_gpio_pin_set(INB);	
}

void Stop_Motor(void)
{
    nrf_gpio_pin_clear(INA);	
	  nrf_gpio_pin_clear(INB);	
}

