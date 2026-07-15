#include "SensorActuator.h"
////////////////////////////////////////////////
//// Hardware includes and defines go here ////
//////////////////////////////////////////////
#if !defined(SLCC_SIMULATION) && !defined(MATLAB_MEX_FILE)
    #include "main.h"
    
    extern ADC_HandleTypeDef hadc1;
    extern TIM_HandleTypeDef htim4;
    extern TIM_HandleTypeDef htim5;
    
    #define VBAT_ADC (&hadc1)
#endif

  ///////////////////////////
 //// Global Variables /////
///////////////////////////

// The PSC is set to 1000, because it gives 100 kHz, which gives the ideal resolution s timer overflow
infraredTypedef InfraSensor = { .clk_MHz = 100, .PSC = 999 };    //PSC and clk_MHz also written from the initialization!
uint32_t last_distance = 400; // The default should be 40 centimeters, because I usually set the target to 40 cm-s
volatile uint32_t batteryVoltage = 0;  //
uint32_t last_speed = 0;
bool skipFlagColorSensor = false;

volatile encoderTypedef Encoder = { .clk_Hz = 100000000, .PSC = 0, .PSCR1 = 1, .PSCR3 = 1, .CPR = 1024};
volatile colorSensorTypedef ColorSensor = {.Color = COLOR_RED};

/////////////////////////////////////////////
//// All the hardware interrupts go here ///
///////////////////////////////////////////
#if !defined(SLCC_SIMULATION) && !defined(MATLAB_MEX_FILE)
//----------------------------//
// HAL_TIM_IC_CaptureCallback //
//----------------------------//
/*
Overdefines the standard HAL interrupt callback; handles all the interupts of timer input capture modes
Takes the handle
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
    // ---------------------------------------------------------
    // Timer 4
    // ---------------------------------------------------------
    if (htim->Instance == TIM4) 
    {
        // ---------------------------------------------------------
        // CHANNEL 2 (Distance measurement, rising edge)
        // ---------------------------------------------------------
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) 
        {
            // Read the captured value of the rising edge
            uint16_t captured_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
            InfraSensor.rise_capture_value = captured_value;
        }
        // ---------------------------------------------------------
        // CHANNEL 1 (Distance measurement, rising edge)
        // ---------------------------------------------------------
        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            // Read the capture value of the falling edge
            uint16_t captured_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            InfraSensor.fall_capture_value = captured_value;
            // Calculate the difference, the timer is 16 bit, thus the difference is 16 bit and the captured values are 16 bit
            uint16_t diff = InfraSensor.fall_capture_value - InfraSensor.rise_capture_value;
            // Just in case the register doesn't update
            if(diff == 0) return;
             // Calculate the distance [mm]
            InfraSensor.distance_mm = ((uint32_t)diff * (InfraSensor.PSC+1) / (InfraSensor.clk_MHz*10)); 
            // From left to right(*almost): we get the difference, which we divide with the clk_MHz/(PSC+1), thus the PSC+1 goes into the numerator, then we get the time elapsed in us, so the scaling constant is 100
            // I have reordered it, so the multiplication comes before the division, thus it is more efficient; also casting the variable to prevent overflow
        }
        // ---------------------------------------------------------
        // CHANNEL 3 (Color Sensor,)
        // ---------------------------------------------------------
        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
        {
            // The colorsensor works with 50% PWM, we need to know the Frequency of the signal, which gives the intensity of the 3 color channels
            // In this part, we use the timer input capture the get how many counts are elapsed between 2 rising edges
            ColorSensor.capture_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
            uint16_t diff = ColorSensor.capture_value - ColorSensor.prev_capture_value;
            ColorSensor.prev_capture_value = ColorSensor.capture_value;

            // In case of a counting error, we return
            if(diff == 0) return;
            // We can set averaging, the default value is 16
            ColorSensor.sum += diff;
            // 3 cases: -we need more counts for the avrage
            //          -we are evaluating the average
            //          -we need to skip the first sample of each measurement because the previous count will overlap with the previous color
            // Note: we use the skipFlagColorSensor for this operation
            if(ColorSensor.itCnt >= NumOfAverages && !skipFlagColorSensor)
            {

                uint16_t total_ticks = ColorSensor.sum;

                if(ColorSensor.Color == COLOR_RED)
                {
                    ColorSensor.tickRed = total_ticks;
                    LL_GPIO_ResetOutputPin(GPIOB, S2);
                    LL_GPIO_SetOutputPin(GPIOB, S3);
                    ColorSensor.Color = COLOR_BLUE;
                    skipFlagColorSensor = true;
                }
                else if(ColorSensor.Color == COLOR_BLUE)
                {
                    ColorSensor.tickBlue = total_ticks;
                    LL_GPIO_SetOutputPin(GPIOB, S3);
                    LL_GPIO_SetOutputPin(GPIOB, S2);
                    ColorSensor.Color = COLOR_GREEN;
                    skipFlagColorSensor = true;
                }
                else if(ColorSensor.Color == COLOR_GREEN)
                {
                    ColorSensor.tickGreen = total_ticks;
                    LL_GPIO_ResetOutputPin(GPIOB, S2);
                    LL_GPIO_ResetOutputPin(GPIOB, S3);
                    ColorSensor.Color = COLOR_RED;
                    skipFlagColorSensor = true;
                }
                ColorSensor.sum = 0;
                ColorSensor.itCnt = 0;
            }
            else if(skipFlagColorSensor)
            {
                skipFlagColorSensor = false;
            }
            else
            {
                ColorSensor.itCnt++;
            }
        }
    }
    // ---------------------------------------------------------
    // Timer 5
    // ---------------------------------------------------------
    else if(htim->Instance == TIM5)
    {
        // ---------------------------------------------------------
        // CHANNEL 1 (ENCODER A)
        // ---------------------------------------------------------
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            // Reset the Encoder.watchdog_timerA, which takes the speed to 0 if there are no interrupts
            Encoder.watchdog_timerA = 0;
            // Calculate the difference of the timer counter between two rising edges
            Encoder.capture_valueA = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
            uint32_t diff = Encoder.capture_valueA-Encoder.prev_capture_valueA;
            Encoder.prev_capture_valueA = Encoder.capture_valueA;
            // If there has been an error, return
            if(diff == 0) return;
            // Decide if the motor is running on low or high speed, switching takes 1 clock cycle, which will be a wrong value, so we don't calculate with it
            if(Encoder.PSCR1 == 1 && !Encoder.skip_next_calcA)
            {
                // speed in [m/s] = 2*pi*f_clk/((1+PSC)*diff*CPR)
                Encoder.speed_mpsA = Encoder.clk_Hz * 2.0f * PI * WHEEL_DIAMETER / (1+Encoder.PSC) / diff / Encoder.CPR;
            }
            else if(Encoder.PSCR1 == 8 && !Encoder.skip_next_calcA)
            {
                // on high speed we only calculate the time it takes to make 8 edges, so the processor isn't overwhelmed
                Encoder.speed_mpsA = Encoder.clk_Hz * 2.0f * PI * WHEEL_DIAMETER * 8 / (1+Encoder.PSC) / diff / Encoder.CPR;
            }
            else
            {
                // reset the false data flag
                Encoder.skip_next_calcA = false;
            }

            // For the direction, we simply watch, whether the other channel is HIGH/LOW in the time of this one
            Encoder.dirA = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1) ? 1 : 0;

            // Logic setting the timer channel division prescaler
            if(Encoder.speed_mpsA > HIGH_SPEED_THRESHOLD && Encoder.PSCR1 == 1)
            {
                //High speed
                    __HAL_TIM_SET_ICPRESCALER(htim, TIM_CHANNEL_1, TIM_ICPSC_DIV8);
                    Encoder.PSCR1 = 8;
                    Encoder.skip_next_calcA = true;
            }
            else if(Encoder.speed_mpsA < LOW_SPEED_THRESHOLD && Encoder.PSCR1 == 8)
            {
                //Low speed
                    __HAL_TIM_SET_ICPRESCALER(htim, TIM_CHANNEL_1, TIM_ICPSC_DIV1);
                    Encoder.PSCR1 = 1;
                    Encoder.skip_next_calcA = true;
            }
        }
        // ---------------------------------------------------------
        // CHANNEL 3 (ENCODER B)
        // ---------------------------------------------------------
        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
        {
            // Same as last
            Encoder.watchdog_timerB = 0;
            Encoder.capture_valueB = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
            uint32_t diff = Encoder.capture_valueB - Encoder.prev_capture_valueB;
            Encoder.prev_capture_valueB = Encoder.capture_valueB;
            
            if(diff == 0) return;
            
            if(Encoder.PSCR3 == 1 && Encoder.skip_next_calcB == 0)
            {
                Encoder.speed_mpsB = Encoder.clk_Hz * 2 * PI * WHEEL_DIAMETER / (1 + Encoder.PSC) / diff / Encoder.CPR;
            }
            else if(Encoder.PSCR3 == 8 && Encoder.skip_next_calcB == 0)
            {
                Encoder.speed_mpsB = Encoder.clk_Hz * 2 * PI * WHEEL_DIAMETER * 8 / (1 + Encoder.PSC) / diff / Encoder.CPR;
            }
            else
            {
                Encoder.skip_next_calcB = false;
            }
            
            Encoder.dirB = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_3) ? 0 : 1;
            
            if(Encoder.speed_mpsB > HIGH_SPEED_THRESHOLD && Encoder.PSCR3 == 1)
            {
                __HAL_TIM_SET_ICPRESCALER(htim, TIM_CHANNEL_3, TIM_ICPSC_DIV8);
                Encoder.PSCR3 = 8;
                Encoder.skip_next_calcB = true;
            }
            else if(Encoder.speed_mpsB < LOW_SPEED_THRESHOLD && Encoder.PSCR3 == 8)
            {
                __HAL_TIM_SET_ICPRESCALER(htim, TIM_CHANNEL_3, TIM_ICPSC_DIV1);
                Encoder.PSCR3 = 1;
                Encoder.skip_next_calcB = true;
            }
        }
    }
}

// HAL_ADC_Conv...: 
//Conversion complete callback in non blocking mode; overdefined.
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    // Check the adc instance
    if (hadc == VBAT_ADC)
    {
        //Note: this code is part of the firmware for the Lego Tanfolyam Robot
        uint16_t adcVal = HAL_ADC_GetValue(VBAT_ADC);
        batteryVoltage =  adcVal * ADC_TO_VBAT_MULTIPLIER + ADC_TO_VBAT_OFFSET;
    }
}

#endif

// Update_Battery_Logic
// Used to call an ADC interrupt upon pressing the button.
void Update_Battery_Logic(bool buttonValue) {
    #if !defined(SLCC_SIMULATION) && !defined(MATLAB_MEX_FILE)
    //read the hardware
    if (buttonValue == 0) {
        //HAL_ADC_Start_IT(VBAT_ADC);
    }
    #else
        // For the simulation, do nothing
    #endif
}



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
    // ELSE: The measured distance is in range
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


// Display the Battery voltage
uint16_t readBatteryVoltage(bool buttonValue)
{
    Update_Battery_Logic(buttonValue);
    #if !defined(SLCC_SIMULATION) && !defined(MATLAB_MEX_FILE)
    HAL_ADC_Stop_IT(VBAT_ADC);
    #endif
    return batteryVoltage;
}

double getSpeedA()
{
    // Decide if the wheels are stationary; For how many cycles, can be set by the MAXIMUM_IDLE_TIME
    if(Encoder.watchdog_timerA < MAXIMUM_IDLE_TIME)
    {
        Encoder.watchdog_timerA++;
        return Encoder.dirA == 1 ? Encoder.speed_mpsA : -Encoder.speed_mpsA;
    }
    else
    {
        Encoder.speed_mpsA = 0.0f;
        return 0;
    }
}

double getSpeedB()
{
    // Same as last
    if(Encoder.watchdog_timerB < MAXIMUM_IDLE_TIME)
    {
        Encoder.watchdog_timerB++;
        return Encoder.dirB == 1 ? Encoder.speed_mpsB : -Encoder.speed_mpsB;
    }
    else
    {
        Encoder.speed_mpsB = 0.0f;
        return 0;
    }
}

RGB_t getRGB()
{
    ColorSensor.fRed = (ColorSensor.clk_Hz) * NumOfAveragesf / (ColorSensor.tickRed * (1.0f + (float)ColorSensor.PSC)); //Hz frequency!
    ColorSensor.fGreen = (ColorSensor.clk_Hz) * NumOfAveragesf / (ColorSensor.tickGreen *  (1.0f + (float)ColorSensor.PSC));
    ColorSensor.fBlue = (ColorSensor.clk_Hz) * NumOfAveragesf / (ColorSensor.tickBlue *  (1.0f + (float)ColorSensor.PSC)) ;
    RGB_t result = {0};

    if (ColorSensor.fRed < R_F_MIN) ColorSensor.fRed = R_F_MIN;
    if (ColorSensor.fRed > R_F_MAX) ColorSensor.fRed = R_F_MAX;
    if (ColorSensor.fBlue < B_F_MIN) ColorSensor.fBlue = B_F_MIN;
    if (ColorSensor.fBlue > B_F_MAX) ColorSensor.fBlue = B_F_MAX;
    if (ColorSensor.fGreen < G_F_MIN) ColorSensor.fGreen = G_F_MIN;
    if (ColorSensor.fGreen > G_F_MAX) ColorSensor.fGreen = G_F_MAX;

    result.R = (ColorSensor.fRed - R_F_MIN) * 255 / (R_F_MAX - R_F_MIN);
    result.G = (ColorSensor.fGreen - G_F_MIN) * 255 / (G_F_MAX - G_F_MIN);
    result.B = (ColorSensor.fBlue - B_F_MIN) * 255 / (B_F_MAX - B_F_MIN);

    return result;
}

HSV_t getHSVfromRGB(RGB_t measurement)
{
    float Red = (float)measurement.R / 255.0f;
    float Green = (float)measurement.G / 255.0f;
    float Blue = (float)measurement.B / 255.0f;

    float max = (Red > Green) ? Red : Green;
    max = (max > Blue) ? max : Blue;
    float min = (Red < Green) ? Red : Green;
    min = (min < Blue) ? min : Blue;

    HSV_t result = {0};

    result.Value = (float)max * 100.0f;

    float delta = max - min;

    result.Saturation = ((max == 0) ? 0 : delta / max) * 100.0f;

    if (max == 0.0f) 
    {
        result.Saturation = 0.0f;
        result.Hue = 0.0f; // Hue is undefined if color is black
        return result;
    }
    if (delta == 0.0f)
    {
        result.Hue = 0.0f;
    }
    else 
    {
        if (max == Red)
        {
            result.Hue = 60.0f * (Green - Blue) / delta;
        }
        else if (max == Green)
        {
            result.Hue = 60.0f * ((Blue - Red) / delta + 2.0f);
        }
        else // max == Blue
        {
            result.Hue = 60.0f * ((Red - Green) / delta + 4.0f);
        }

        // Keep the angle positive between 0 and 360
        if (result.Hue < 0.0f)
        {
            result.Hue += 360.0f;
        }
    }

    return result;
}

RGB_t calibrateRGB()
{
    ColorSensor.fRed = (ColorSensor.clk_Hz) * NumOfAveragesf / (ColorSensor.tickRed * (1.0f + (float)ColorSensor.PSC)); //Hz frequency!
    ColorSensor.fGreen = (ColorSensor.clk_Hz) * NumOfAveragesf / (ColorSensor.tickGreen *  (1.0f + (float)ColorSensor.PSC));
    ColorSensor.fBlue = (ColorSensor.clk_Hz) * NumOfAveragesf / (ColorSensor.tickBlue *  (1.0f + (float)ColorSensor.PSC)) ;
    RGB_t result = {0};

    result.R = ColorSensor.fRed;
    result.G = ColorSensor.fGreen;
    result.B = ColorSensor.fBlue;

    return result;
}

// Functions that doesn't need to be seen by simulink
#if !defined(SLCC_SIMULATION) && !defined(MATLAB_MEX_FILE)
// Helper function for dynamically setting the prescaler division ratio for each timer channel
uint8_t Get_Actual_Ratio(uint32_t hal_macro) {
    switch (hal_macro) {
        case TIM_ICPSC_DIV8:
            return 8;
        case TIM_ICPSC_DIV4:
            return 4;
        case TIM_ICPSC_DIV2:
            return 2;
        case TIM_ICPSC_DIV1:
        default:
            return 1;
    }
}
#endif



// Timer_Init_Custom
// Starts the interrupt handling of the timers; needs to be seen by simulink!!
// Also: reads the clock speed and certain register values from the generated code 
void Timer_Init_Custom(void){
#if !defined(SLCC_SIMULATION) && !defined(MATLAB_MEX_FILE)
    // Infrared sensor Part //
        // Start Input capture
            // Rising edge
        HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_1);  
            // Falling edge
        HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_2);
        //Update the PSC from the generated STM32CubeMX code :))))
        InfraSensor.PSC = htim4.Init.Prescaler; // This should be 999, so we get 100 kHz, so we get the best resolution
        //Update the clk_MHz from the generated ... :)
        uint32_t apb1_freq_hz = HAL_RCC_GetPCLK1Freq();
        InfraSensor.clk_MHz = (apb1_freq_hz * 2) / 1000000; // The multiplication is required because of the clock configuration (->/2->*2-> Problem!; APB1)

    // Encoder Part //
        // EncoderA (1,2)
        HAL_TIM_IC_Start_IT(&htim5,TIM_CHANNEL_1);
        // EncoderB (1,2)
        HAL_TIM_IC_Start_IT(&htim5,TIM_CHANNEL_3);

        Encoder.PSC = htim5.Init.Prescaler; 
        Encoder.clk_Hz = InfraSensor.clk_MHz*1000000;   // This sould be in MHz

        // Get the raw HAL bitmasks
        uint32_t rawPSC1 = __HAL_TIM_GET_ICPRESCALER(&htim5, TIM_CHANNEL_1);
        uint32_t rawPSC3 = __HAL_TIM_GET_ICPRESCALER(&htim5, TIM_CHANNEL_3);

        // Convert them to integers (1, 2, 4, or 8) and store them
        Encoder.PSCR1 = Get_Actual_Ratio(rawPSC1);
        Encoder.PSCR3 = Get_Actual_Ratio(rawPSC3);

    // Color sensor Part //
        HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_3);
        ColorSensor.PSC = htim4.Init.Prescaler;
        ColorSensor.clk_Hz = (apb1_freq_hz * 2);
        LL_GPIO_ResetOutputPin(GPIOB, S2);
        LL_GPIO_ResetOutputPin(GPIOB, S3);
        ColorSensor.Color = COLOR_RED;
        ColorSensor.capture_value = 0;
        ColorSensor.prev_capture_value = 0;
#endif
}