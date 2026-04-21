#ifndef TEST123_H
#define TEST123_H

#include <stdint.h>
#include <stdbool.h>

// 1. Hide the STM32 hardware includes from Simulink's PC simulation
#ifndef MATLAB_MEX_FILE
    #include "main.h" 
#endif

bool read_my_pin(void);

#endif /* TEST123_H */