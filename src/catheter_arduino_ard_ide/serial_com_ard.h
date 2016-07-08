/* start serial connection for Atmega2560 */

/* **************** */
/* Serial functions */
/* **************** */


void serial_init() {
#ifdef DUE
	//set millisecond timeout so commands can be entered
	SerialUSB.setTimeout(2);
	SerialUSB.begin(BAUD);
	//while (! SerialUSB); // Wait untilSerial is ready - Leonardo
#else
	Serial.setTimeout(2);
	Serial.begin(BAUD);
#endif
}

int serial_available(void) {
#ifdef DUE
	return SerialUSB.available();
#else
	return Serial.available();
#endif
}

void flush_bytes(int count) {
	int i;
	for (i = 0; i<count; i++) {
#ifdef DUE
		SerialUSB.read();
#else
		Serial.read();
#endif
	}
}

void write_byte(uint8_t b) {
#ifdef DUE
	SerialUSB.write(b);
#else
	Serial.write(b);
#endif
}

void write_bytes(uint8_t bytes[], uint8_t nb) {
	uint8_t i;
	for (i = 0; i<nb; i++) {
		write_byte(bytes[i]);
	}
}

uint8_t read_byte() {
#ifdef DUE
	return (uint8_t)(SerialUSB.read());
#else
	return (uint8_t)(Serial.read());
#endif  
}


void serial_flush() {
  #ifdef DUE
    SerialUSB.flush();
    #else
    Serial.flush();
    #endif
}


// byte 1: bit 1 set indicates that this is the beginning of a message
// byte 1: bit 2 unset indicates that there was an error with the packet
// byte 1: bits 4-7: packet index
// bytes 2-3: don't-cares
void writeError(uint8_t packetIndex = 0)
{
   write_byte((8 << 4) + (packetIndex & 15));
}

void writeGood(uint8_t packetIndex)
{
   write_byte((12 << 4) + (packetIndex & 15));
}


uint8_t read_bytes(uint8_t charBuffer[], int count)
{
#ifdef DUE
		uint8_t i(SerialUSB.readBytes(charBuffer,count));
#else
		uint8_t i(Serial.readBytes(charBuffer,count));
#endif
  return i;
}
