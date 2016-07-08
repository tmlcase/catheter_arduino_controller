/* *************** */
/* pin definitions */
/* *************** */

int DAC_CS_pins[NCHANNELS] = { 2,3,4,5,6,7 };   // chip select (part of SPI) (DAC)
int ADC_CS_pins[NCHANNELS] = { 8,9,10,11,12,13 };  // chip select (part of SPI) (ADC)
int H_Enable_pins[NCHANNELS] = { 22,23,24,25,26,27 }; // h- bridge pins
int H_Neg_pins[NCHANNELS] = { 34,35,36,37,38,39 };    // H_ neg pins
int H_Pos_pins[NCHANNELS] = { 28,29,30,31,32,33 };    // H_pos pins
int DAC_LDAC_pins[NCHANNELS] = { 46,47,48,49,50,51 };   // what do these pins do?  (held low auto loads input buffer) (high to low loads input buffer).

//camera pins
int CAMERA_PINS[3] = {64, 63, 62};

// MRI pin (Input)
int mriPin = 65;
