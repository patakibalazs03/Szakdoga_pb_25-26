#ifndef SENSORACTUATOR_H
#define SENSORACTUATOR_H

#include <stdint.h>
#include <stdbool.h>

// 1. Hide the STM32 hardware includes from Simulink's PC simulation
#ifndef MATLAB_MEX_FILE
    #include "main.h" 
#endif

// Functions
uint32_t infrared(TIM_HandleTypeDef *htim, uint32_t Channel,uint32_t PSC,uint32_t clk_MHz);
uint32_t HAL_TIM_ReadCapturedValue(const TIM_HandleTypeDef *htim, uint32_t Channel);

#endif /* SENSORACTUATOR_H */