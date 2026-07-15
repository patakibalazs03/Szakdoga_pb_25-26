#ifndef SENSORACTUATOR_H
#define SENSORACTUATOR_H
//Includes

#include <stdint.h>
#include <stdbool.h>

// Global variables

extern uint32_t last_distance;
extern volatile uint32_t batteryVoltage;
extern uint32_t last_speed;
// Different Color Channels for the Color Sensor
typedef enum {
    COLOR_RED   = 0,
    COLOR_BLUE  = 1,
    COLOR_GREEN = 2,
    COLOR_CLEAR = 3
} ColorChannel_t;

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

typedef struct{
    uint32_t capture_valueA;
    uint32_t prev_capture_valueA;
    uint32_t capture_valueB;
    uint32_t prev_capture_valueB;
    double speed_mpsA;
    double speed_mpsB;
    uint32_t clk_Hz;
    uint16_t PSC;
    uint16_t CPR;
    uint8_t PSCR1;
    uint8_t PSCR3;
    bool skip_next_calcA;
    bool skip_next_calcB;
    uint8_t dirA;
    uint8_t dirB;
    uint8_t watchdog_timerA;
    uint8_t watchdog_timerB;
} encoderTypedef;

typedef struct{
    uint32_t sum;
    uint8_t counter;
    uint16_t capture_value;
    uint16_t prev_capture_value;
    uint16_t PSC;
    uint32_t clk_Hz;
    uint16_t tickRed;
    uint16_t tickBlue;
    uint16_t tickGreen;
    uint8_t itCnt;
    float fRed;
    float fBlue;
    float fGreen;
    ColorChannel_t Color;
} colorSensorTypedef;

typedef struct{
    uint16_t R;
    uint16_t G;
    uint16_t B;
} RGB_t;

typedef struct{
    uint8_t Hue;
    float Saturation;
    float Value;
} HSV_t;

// Defines

    // For the Distance Measuring, not needed, possible implementation of averaging
    #define InrfaSamples 8
    //Set OOEDISTANCE to set the out of range distance of the infrared sensor; [mm]
    #define OORDISTANCE 2000
    //For the ADC
    #define ADC_TO_VBAT_MULTIPLIER      (3300 * 5 / 4096)
    #define ADC_TO_VBAT_OFFSET        220
    //For the encoders
    #define PI 3.14159265f
    #define WHEEL_DIAMETER 0.0325f     //[m]
    #define HIGH_SPEED_THRESHOLD 0.2f //[m/s]
    #define LOW_SPEED_THRESHOLD 0.1f //[m/s]
    #define MAXIMUM_IDLE_TIME 5  //[interrupt cycles]

    // ColorSensor
    #define S2 LL_GPIO_PIN_5
    #define S3 LL_GPIO_PIN_4
 

    #define R_F_MAX 23188
    #define R_F_MIN 5128

    #define G_F_MAX 16666
    #define G_F_MIN 3193

    #define B_F_MAX 18181
    #define B_F_MIN 3193


    #define NumOfAverages 16
    #define NumOfAveragesf 16.0f

// Functions


uint32_t infrared();
void Timer_Init_Custom(void);
uint16_t readBatteryVoltage(bool buttonValue);
void Update_Battery_Logic(bool buttonValue);
double getSpeedA();
double getSpeedB();
RGB_t getRGB();
RGB_t calibrateRGB();
HSV_t getHSVfromRGB(RGB_t measurement);


//Global variables extended
extern volatile encoderTypedef Encoder;
#endif /* SENSORACTUATOR_H */