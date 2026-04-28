#ifndef SENSORACTUATOR_H
#define SENSORACTUATOR_H
//Includes

#include <stdint.h>
#include <stdbool.h>
/*
#ifndef MATLAB_MEX_FILE
    #include "main.h"
#else
    typedef struct TIM_HandleTypeDef TIM_HandleTypeDef; 
    typedef struct ADC_HandleTypeDef ADC_HandleTypeDef;
#endif
//*/
//Constants

extern uint32_t last_distance;
extern uint32_t batteryVoltage;

// Structs

typedef struct {
    uint16_t rise_capture_value;
    uint16_t fall_capture_value;
    uint16_t distance_mm;
    uint32_t clk_MHz;
    uint16_t PSC;
    bool oor;
    bool valid;
} infraredTypedef;

// Defines

#define InrfaSamples 8
#define OORDISTANCE 2000
#define ADC_TO_VBAT_MULTIPLIER      (3300 * 5 / 4096)
#define ADC_TO_VBAT_OFFSET          220

// Functions

uint32_t infrared();
/*
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
*/
void Timer_Init_Custom(void);
uint16_t readBatteryVoltage();

#endif /* SENSORACTUATOR_H */