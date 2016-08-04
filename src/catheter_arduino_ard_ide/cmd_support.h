/* start serial connection for Atmega2560 */

/* **************** */
/* support functions for command IO */
/* **************** */


/* calculate 8-bit fletcher checksum using blocksize=4 */
uint8_t fletcher8(uint8_t len, const uint8_t* data) {
	uint8_t sum1 = 0, sum2 = 0;
	int i;
	for (i = 0; i<len; i++) {
		sum1 += (data[i] >> 4);   //first 4 bits
		sum2 += sum1;

		sum1 += (data[i] & 15);   //last 4 bits
		sum2 += sum1;

		sum1 %= 16;   //modulo 15
		sum2 %= 16;
	}
	return ((sum2) << 4) + (sum1);
}

void errorEncode(uint8_t packetIndex, uint8_t *reply, uint8_t *replyLen) {
  reply[0] = (packetIndex & 16);
  *replyLen = 1;
}

uint8_t compactCmdResponsePoll(const channelStatus& local_) {
  uint8_t cmd(0);
  cmd |= (1 << POL_B);
  if (local_.enable) cmd |= (1 << ENA_B);
  if (local_.dir) cmd |= (1 << DIR_B);
  return cmd;
}

uint8_t compactCmdResponse(const channelStatus& local_, bool update) {
  uint8_t cmd(0);
  if(update) cmd |= (1 << UPD_B);
  if (local_.enable) cmd |= (1 << ENA_B);
  if (local_.dir) cmd |= (1 << DIR_B);
  return cmd;
}


//This function processes a single channel for the arduino.
uint8_t processSingleChannel(int i, uint8_t cmdVal, uint16_t cmdData, uint8_t *responseBytes, uint8_t *responseIndex)
{
  bool poll = (cmdVal >> 3) & 1;
  bool en = (cmdVal >> 2) & 1;
  bool upd = (cmdVal >> 1) & 1;
  bool dir = (cmdVal >> 0) & 1;
  if (poll)  // poll disables all other action.
  {
    // get the channel index and the 
    uint8_t channel( (i+1) << 4 );
    uint8_t cmdVal(compactCmdResponsePoll(channelList[i]));
    uint8_t response = channel | cmdVal;
        
    // get the DAC value;
    uint8_t DACU = (0b00111111 & (channelList[i].DAC_val >> 6));
    uint8_t DACL = (0b00111111 & channelList[i].DAC_val);        
    // get the ADC settings:
    uint16_t  ADCm(ADC_read(i));
    channelList[i].ADC_val = ADCm; //this is saved for now but does very little.

    responseBytes[*responseIndex] = response;  // channel number and channel commands
    responseBytes[*responseIndex+1] = DACU;    // DAC upper 6 bits
    responseBytes[*responseIndex+2] = DACL;    // DAC lower 6 bits
    responseBytes[*responseIndex+3] = (uint8_t) (ADCm >> 8); // ADC upper 6 bits
    responseBytes[*responseIndex+4] = (uint8_t) (ADCm & 0b11111111);         // ADC lower 6 bits
    *responseIndex += 5;
	return 1;
  } // if(poll)
  else {  
    toggle_enable(i, en);  // enable or disable channel 
    channelList[i].enable = en;  //saved but useless.
    if (upd)  // set DAC value if necessary
    {
        DAC_write(i, cmdData);
        channelList[i].DAC_val = cmdData;
    }
    set_direction(i, dir);   // set H-brige direction 
    channelList[i].dir = dir;
    
    uint8_t DACU = (0b00111111 & (channelList[i].DAC_val >> 6));
    uint8_t DACL = (0b00111111 & channelList[i].DAC_val);
	
    
    uint8_t channel( (i+1) << 4 );
    uint8_t cmdVal(compactCmdResponse(channelList[i], upd));
    uint8_t response = channel | cmdVal;
	
	responseBytes[*responseIndex] = response;  // channel number and channel commands
    responseBytes[*responseIndex+1] = DACU;    // DAC upper 6 bits
    responseBytes[*responseIndex+2] = DACL;    // DAC lower 6 bits
	*responseIndex += 3;
	return 0;
  }  // if (poll) else
}
  
