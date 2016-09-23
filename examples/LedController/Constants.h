// ----------------------------------------------------------------------------
// Constants.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "Arduino.h"


namespace constants
{
//MAX values must be >= 1, >= created/copied count, < RAM limit
enum{METHOD_COUNT_MAX=4};

extern const size_t baudrate;

extern const size_t led_pin;

}
#endif
