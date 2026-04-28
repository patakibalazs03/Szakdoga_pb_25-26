#include "SensorActuator.h"
// Everything that needs to be hidden from simulink like hardware stuff goes here
#if !defined(SLCC_SIMULATION) && !defined(MATLAB_MEX_FILE)
    #include "main.h"
    
    extern ADC_HandleTypeDef hadc1;
    extern TIM_HandleTypeDef htim4;
    
    #define VBAT_ADC (&hadc1)
#endif

//Global Variables

infraredTypedef InfraSensor = { .clk_MHz = 100, .PSC = 999 };    //PSC and clk_MHz written from the initialization!!! Changing it here doesnt do anything! Reason: robust code, less "mouse clicks" :)
uint32_t last_distance = 400; // The default should be 40 centimeters, because I usually set the target to 40 cm-s
uint32_t batteryVoltage = 0;  //



#if !defined(SLCC_SIMULATION) && !defined(MATLAB_MEX_FILE)
// HAL_TIM_IC_CaptureCallback
// Overdefines the standard HAL interrupt callback; handles all the interupts of timer input capture modes
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
//#ifndef MATLAB_MEX_FILE
    // Check the timer instance
    if (htim->Instance == TIM4) 
    {
        // Check the timer channel
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) 
        {
            // Read the captured value of the rising edge
            uint16_t captured_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
            InfraSensor.rise_capture_value = captured_value;
        }
        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            // Read the capture value of the falling edge
            uint16_t captured_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            InfraSensor.fall_capture_value = captured_value;
            // Calculate the difference, the timer is 16 bit, thus the difference is 16 bit and the captured values are 16 bit
            uint16_t diff = InfraSensor.fall_capture_value - InfraSensor.rise_capture_value;
             // Calculate the distance [mm]
            InfraSensor.distance_mm = ((uint32_t)diff * (InfraSensor.PSC+1) / (InfraSensor.clk_MHz*10)); 
            // From left to right(*almost): we get the difference, which we divide with the clk_MHz/(PSC+1), thus the PSC+1 goes into the numerator, then we get the time elapsed in us, so the scaling constant is 100
            // I have reordered it, so the multiplication comes before the division, thus it is more efficient; also casting the variable to prevent overflow
        }
    }
//#endif
}

// HAL_ADC_Conv...: 
//Conversion complete callback in non blocking mode; overdefined.
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
//#ifndef MATLAB_MEX_FILE
    // Check the adc instance
    if (hadc == VBAT_ADC)
    {
        //Note: this code is part of the firmware for the Lego Tanfolyam Robot
        uint16_t adcVal = HAL_ADC_GetValue(VBAT_ADC);
        batteryVoltage =  adcVal * ADC_TO_VBAT_MULTIPLIER + ADC_TO_VBAT_OFFSET;
    }
//#endif
}

#endif

////    Functions  ////

// ifrared:
// takes: the raw measured range for a global typedef
// returns: the conditioned range
uint32_t infrared(){
    // If-else if-else statements for determining the validity of the measure value
    // IF: the target is out of range
    if (InfraSensor.distance_mm > OORDISTANCE)
    {
        InfraSensor.oor = true;
        InfraSensor.valid = true;
    }
    // ELSIF: The mesured distance is invalid
    else if(InfraSensor.distance_mm < 0)
        InfraSensor.valid = false;
    // ELSE: The measured distance is out of range
    else
    {
        InfraSensor.oor = false;
        InfraSensor.valid = true;
    }
    // Determining the validity of the data, we set the measured distance
    // If everything is okay, we save the current data and exit out of the function
    if (InfraSensor.valid && !InfraSensor.oor)
    {
        last_distance = InfraSensor.distance_mm;
        return InfraSensor.distance_mm;
    }
    // If the data isn't valid we keep holding the previous value
    else if(!InfraSensor.valid)
    {
        return last_distance;
    }
    // If the data is oor, we say it is the maximum distance it can measure, this value can be adjusted in the .h file
    else
    {
        return OORDISTANCE;
    }
}





uint16_t readBatteryVoltage(){
    return batteryVoltage;
}

// Timer_Init_Custom
// Starts the interrupt handling of the timers
// Also: reads the clock speed and certain register values from the generated code 
void Timer_Init_Custom(void){
#if !defined(SLCC_SIMULATION) && !defined(MATLAB_MEX_FILE)
    // Infrared sensor Part //
        // Start Input capture
        HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_1);
        HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_2);
        //Update the PSC from the generated STM32CubeMX code :))))
        InfraSensor.PSC = htim4.Init.Prescaler;
        //Update the clk_MHz from the generated ... :)
        uint32_t apb1_freq_hz = HAL_RCC_GetPCLK1Freq();
        InfraSensor.clk_MHz = (apb1_freq_hz * 2) / 1000000; // The multiplication is required because of the clock configuration (->/2->*2-> Problem!; APB1)
    //Battery Indicator
        HAL_ADC_Start_IT(VBAT_ADC);
#endif
}