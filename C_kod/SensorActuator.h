#ifndef SENSORACTUATOR_H
#define SENSORACTUATOR_H
//Includes

#include <stdint.h>
#include <stdbool.h>

// Global variables

extern uint32_t last_distance;
extern uint32_t batteryVoltage;
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
    float pRed;
    float pBlue;
    float pGreen;
    ColorChannel_t Color;
    bool stepFlag;
} colorSensorTypedef;

typedef struct{
    uint16_t R;
    uint16_t G;
    uint16_t B;
} RGB_t;

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
    #define SCALE 10000

    #define R_P_MAX 1100
    #define R_P_MIN 70

    #define G_P_MAX 1550
    #define G_P_MIN 70

    #define B_P_MAX 1800
    #define B_P_MIN 65

    #define R_MIN (SCALE / R_P_MAX)
    #define R_MAX (SCALE / R_P_MIN)
    #define G_MIN (SCALE / G_P_MAX)
    #define G_MAX (SCALE / G_P_MIN)
    #define B_MIN (SCALE / B_P_MAX)
    #define B_MAX (SCALE / B_P_MIN)

// Functions


uint32_t infrared();
void Timer_Init_Custom(void);
uint16_t readBatteryVoltage(bool buttonValue);
void Update_Battery_Logic(bool buttonValue);
double getSpeedA();
double getSpeedB();
RGB_t getRGB();

//Global variables extended
extern volatile encoderTypedef Encoder;
#endif /* SENSORACTUATOR_H */