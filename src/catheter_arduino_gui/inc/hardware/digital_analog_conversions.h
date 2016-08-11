#pragma once

#ifndef DIGITAL_ANALOG_H
#define DIGITAL_ANALOG_H

#include <cstdint>
#include <cstdlib>
#include "com/common_utils.h"

// This file declares the functions capable of converting byte data to analog current
// I.E. ADC data to mA and mA to DAC data.


/**
 * @brief This function converts up to a 16 bit adc input to a milliAmp current.
*/
double adc2MilliAmp(uint16_t dataIn);

/**
* @brief This function converts up to a milliAmp current into a dac bit value.
*/
uint16_t milliAmp2Dac(double mA);

/**
* @brief This function converts the dac bit value to a milliAmp current.
*/
double dac2MilliAmp(uint16_t dacVal, dir_t dir);


#endif