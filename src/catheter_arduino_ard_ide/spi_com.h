#include <SPI.h>
/* ************************ */
/* SPI functions */
/* ************************ */

/*  read a 12-bit value from the MCP3201 ADC on a specified channel (0 through NCHANNELS-1) */
uint16_t ADC_read(int channel) {
    digitalWrite(ADC_CS_pins[channel], CS_EN);
    uint16_t ret1(SPI.transfer16(0x0000)); 
    //uint16_t ret2(SPI.transfer16(0x0000)); 
    digitalWrite(ADC_CS_pins[channel], !CS_EN);
    //if( ((ret >> 12) & 0x01)) return uint16_t(0); 
    uint16_t retO(ret1 >> 1);
    return retO;
}

/*  write a 12-bit value to the MCP4921 DAC on a specified channel (0 through NCHANNELS-1) */
void DAC_write(uint8_t channel, uint16_t to_dac) {       
    byte dataMSB = highByte(to_dac);
    byte dataLSB = lowByte(to_dac);
    
    dataMSB &= 0b00001111;
    dataMSB = dataMSB | DAC_SELECT | INPUT_BUF | GAIN_SELECT | PWR_DOWN;
    
    // trying new SPI call per Arduino Due documentation
    //SPI.beginTransaction(settings);
    digitalWrite(DAC_CS_pins[channel], CS_EN);
    SPI.transfer(dataMSB);//if (serialbyte > 0) write_byte(serialbyte);//20 bytes
    SPI.transfer(dataLSB);
    digitalWrite(DAC_CS_pins[channel], !CS_EN); 
    //SPI.endTransaction();
    // SPI.transfer(
}

/* initialize the SPI bus */
void SPI_init() {
  // trying new SPI.begin() call per Arduino Due documentation
  SPI.begin();  // Auto into mode1
#ifdef DUE
  SPI.setClockDivider(SPI_CLOCK_DIV4);    /* 20 MHz (due is 88, mega2560 is 16) */
#endif
  SPI.setBitOrder(MSBFIRST);
  for (int i = 0; i <= NCHANNELS; i++) {
    DAC_write(i, (uint16_t)0);
  }
}
