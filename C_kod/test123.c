#include "test123.h"

bool read_my_pin(void) {
    bool state = false;

    // 2. Hide the actual hardware manipulation from Simulink
#ifndef MATLAB_MEX_FILE
    // --- REAL HARDWARE CODE (Compiles for STM32) ---
    // Reads the actual physical pin on the board
    state = (bool)HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
#else
    // --- PC SIMULATION CODE (Compiles for Simulink on your PC) ---
    // Provides a dummy value so the PC simulation runs without crashing
    state = false; 
#endif

    return state;
}