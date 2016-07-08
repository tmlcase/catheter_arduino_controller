#include <SPI.h>

#include "ard_due_defs.h"
#include "pin_defs.h"
#include "spi_com.h"
#include "serial_com_ard.h"

/* structs to represent channel status, channel commands, and serial packets */

/* book-keeping for each channel */
struct channelStatus {
	int enable;
	int dir;
	uint16_t DAC_val;
	uint16_t ADC_val;
};

bool updatePending;

/* **************** */
/* global variables */
/* **************** */

/* tracks status of channels */
channelStatus channelList[NCHANNELS];
uint8_t packetStatus; // {X,X,X,X,PCK_CHK_ERR, POST_ERR, CMD_CHK_ERR, PRE_ERR} 

uint8_t inputBytes[512];
uint8_t outputBytes[512];

unsigned int camera_counter;

/* ******************* */
/* new camera function */
/* ******************* */

int camera_write(int counter)
{
	// make sure counter is integer 0 - 7
	counter = counter % 4;
	if (counter % 2 == 1) digitalWrite(CAMERA_PINS[0], HIGH);
  else digitalWrite(CAMERA_PINS[0], LOW);
  if ((counter >> 1) % 2 == 1) digitalWrite(CAMERA_PINS[1], HIGH);
  else digitalWrite(CAMERA_PINS[1], LOW);
  int mriStatus = digitalRead(mriPin);
  if(mriStatus) digitalWrite(CAMERA_PINS[2],HIGH);
  else digitalWrite(CAMERA_PINS[2],LOW);
  // if (counter >= 4) digitalWrite(CAMERA_PINS[2], HIGH);
  // else digitalWrite(CAMERA_PINS[2], LOW);
  return mriStatus;
}

#include "cmd_support.h"
#include "cmd_parse.h"

/* set pins to output mode and put them in defined state */
void pin_init() {
	for (int i = 0; i<NCHANNELS; i++) {
		pinMode(ADC_CS_pins[i], OUTPUT);
		pinMode(DAC_CS_pins[i], OUTPUT);
		pinMode(H_Enable_pins[i], OUTPUT);
		pinMode(H_Neg_pins[i], OUTPUT);
		pinMode(H_Pos_pins[i], OUTPUT);
		pinMode(DAC_LDAC_pins[i], OUTPUT);
		if (i < 3) {
			pinMode(CAMERA_PINS[i], OUTPUT);
			digitalWrite(CAMERA_PINS[i], LOW);
		}
    
    pinMode(mriPin,INPUT);
    
		digitalWrite(ADC_CS_pins[i], !CS_EN);
		digitalWrite(DAC_CS_pins[i], !CS_EN);
		digitalWrite(DAC_LDAC_pins[i], LOW);
		digitalWrite(H_Enable_pins[i], !H_EN);
		digitalWrite(H_Neg_pins[i], !DIR_ON);
		digitalWrite(H_Pos_pins[i], DIR_ON);
	}
}

/* enable or disable the H-bridge on a given channel (Active LOW) */
void toggle_enable(int channel, int en) {
	if (en == 0) {  // disable 
		digitalWrite(H_Enable_pins[channel], !H_EN);
	}
	else {  // enable 
		digitalWrite(H_Enable_pins[channel], H_EN);
	}
}

//* set the direction of the H-bridge for a given channel */
void set_direction(int channel, int direction) {
	if (direction == 0) {
		digitalWrite(H_Neg_pins[channel], DIR_ON);
		digitalWrite(H_Pos_pins[channel], !DIR_ON);
	}
	else {
		digitalWrite(H_Pos_pins[channel], DIR_ON);
		digitalWrite(H_Neg_pins[channel], !DIR_ON);
	}
}

bool addr_is_global(uint8_t addr) {
   return addr==0;
}

bool mriStatOld;

/* ************ */
/* main program */
/* ************ */

void setup() {
	pin_init();
	SPI_init();

	serial_init();

	packetStatus = 0; // {X,X,X,X,PCK_CHK_ERR, POST_ERR, CMD_CHK_ERR, PRE_ERR} 
  
  for ( int i = 0; i < 512; i++)
  {
    inputBytes[i] = 0;
    outputBytes[i] = 0;
  }     
	camera_counter = 0;
	delay(START_DELAY);
  mriStatOld = false;
  updatePending = false;
}

void loop() {
        if(serial_available())
        {
          uint8_t counter = (uint8_t) serial_available();
          uint8_t packetSize = read_bytes( inputBytes, counter);
          uint8_t packetIndex(0);
          uint8_t cmdCount(0);
          
          // when data is available parse it.
          //If the parsing passes the checksum, then it is acted on.
          if(cmd_check(inputBytes, packetSize, &packetIndex, &cmdCount))
          {
            camera_counter = camera_counter + 1;
	          
            uint8_t outputLength(0);
            // This function no longer actually changes 
            outputLength = cmd_parse(inputBytes, packetSize, cmdCount, outputBytes, packetIndex);
            write_bytes(outputBytes, outputLength);
          }
          else writeError(packetIndex);
        }
        int mriStat(camera_write(camera_counter));
        if (mriStat && !mriStatOld)
        {
          mriStatOld = true;
          // set the axial coil (channel 3) to be at 75 ma.
          // do NOT change directions.
          DAC_write(0, 0);
          DAC_write(1, 0);
          if ( channelList[2].enable == 0)
          {
            // renable channel 3 (toindexed as 2).
            toggle_enable(2, 1);
          }
          uint16_t maSettings(960); // This is the 75 ma (estimated value for the MRI).
          DAC_write(2, maSettings);
        }
        if (!mriStat && mriStatOld)
        {
          mriStatOld = false;
          // resets the coil currents to their previous values.
          // do NOT change directions.
          DAC_write(0, channelList[0].DAC_val);
          DAC_write(1, channelList[1].DAC_val);
          // reset channel 3 (toindexed as 2) to desired value.
          toggle_enable(2, channelList[2].enable);
          set_direction(2, channelList[2].dir);
          //and direction.
          
          DAC_write(2, channelList[2].DAC_val);
        }
        
}  //end loop




