#include "com/common_utils.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG

CatheterChannelCmdSet resetCmd()
{
	CatheterChannelCmdSet resetCmdSet;
	resetCmdSet.commandList.push_back(CatheterChannelCmd());
	return resetCmdSet;

}

CatheterChannelCmdSet pollCmd()
{
	CatheterChannelCmdSet pollCmdSet;
	pollCmdSet.commandList.push_back(CatheterChannelCmd());
	pollCmdSet.commandList[0].poll = true;
	pollCmdSet.commandList[0].channel = 0;
	return pollCmdSet;

}

void get_current_constants_by_channel(double* m, double* b, int chan, bool revG, bool set1) {
	if (revG) {
		*m = 12.8;
		*b = 0;
		switch (chan) {
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		}
	} else {
		switch (chan) {
		case 1: //may wish to define constants...
			if (set1) { //for currents below 10 mA
				*m = -29.07;
				*b = 4058;
			} else {
				*m = -16.30;
				*b = 3896;
			}
			break;
		case 2:
			if (set1) { //for currents below 10 mA
				*m = -28.99;
				*b = 4096;
			} else {
				*m = -16.48;
				*b = 3939;
			}
			break;
		case 3:
			if (set1) { //for currents below 10 mA
				*m = -36.76;
				*b = 4040;
			} else {
				*m = -15.78;
				*b = 3837;
			}
			break;
		}
	}
}

unsigned int convert_current_by_channel_ma2res(double ma, int chan) {    
    if (ma == 0) return DAC_RES_OFF;    
    if (ma < 0) ma = ma * -1;
    unsigned int res;
    double m, b;
    bool set1 = (ma <= 10);
	get_current_constants_by_channel(&m, &b, chan);
    res = (int)(m*ma + b);
    if (res < 0) res = 0;
    else if (res >= DAC_RES) res = DAC_RES - 1;
    return res;
}

double convert_current_by_channel_res2ma(int res, dir_t dir, int chan) {
    double ma;
    double m, b;
    bool set1 = (res >= 4000);
	get_current_constants_by_channel(&m, &b, chan);
    ma = (res - b)/m;
    if (dir == DIR_NEG) ma = -ma;
    return ma;
}

uint8_t compactCmdVal(const bool &poll, const bool &en, const bool &update, const dir_t &dir) {
    uint8_t cmd = 0;
    if (poll)   cmd |= (1 << POL_BIT);
    if (en)     cmd |= (1 << ENA_BIT);
    if (update) cmd |= (1 << UPD_BIT);
    if (dir==DIR_POS)    cmd |= (1 << DIR_BIT);
    return cmd;
}

CatheterChannelCmd emptyCommand() {
	CatheterChannelCmd cmd;
	cmd.channel = 0;
	cmd.currentMilliAmp = 0;
	cmd.currentMilliAmp_ADC = 0;
	cmd.poll = false;
	return cmd;
}

CatheterChannelCmd resetCommand() {
	CatheterChannelCmd cmd;
	cmd.channel = 0; //global
	cmd.currentMilliAmp = 0;
	cmd.currentMilliAmp_ADC = 0;
	cmd.poll = false;
	return cmd;
}

// should be called resetPacketBytes();
std::vector<uint8_t> resetCommandBytes() {
	std::vector<CatheterChannelCmd> cmdVect;
	cmdVect.push_back(resetCommand());
	return compactPacketBytes(cmdVect, 1);
}

/* return the state associated with a particular (4 bit) command value */
void expandCmdVal(const uint8_t &cmdVal, bool* poll, bool* en, bool* update, dir_t* dir) {
    *poll = (cmdVal >> POL_BIT) & 1;
    *en = (cmdVal >> ENA_BIT) & 1;
    *update = (cmdVal >> UPD_BIT) & 1;
    if ((cmdVal >> DIR_BIT) & 1)
        *dir = DIR_POS;
    else
        *dir = DIR_NEG;
}

void expandCatheterCmd(const CatheterChannelCmd& cmd, bool* enable, bool* update, dir_t* dir) {
    *enable = (cmd.currentMilliAmp != 0);
    *update = true;
    *dir = (cmd.currentMilliAmp < 0 ? DIR_NEG : DIR_POS);
}

void compactCatheterCmd(const CatheterChannelCmd& cmd, unsigned int* cmd4, unsigned int* data12) {
    *data12 = convert_current_by_channel_ma2res(cmd.currentMilliAmp, cmd.channel);
	if (cmd.currentMilliAmp > 0) 
		*cmd4 = compactCmdVal(cmd.poll, true, true, DIR_POS);
	else if (cmd.currentMilliAmp < 0)
		*cmd4 = compactCmdVal(cmd.poll, true, true, DIR_NEG); 
	else
		*cmd4 = compactCmdVal(cmd.poll, false, true, DIR_POS);
}

/* calculate 8-bit fletcher checksum using blocksize=4 */
uint8_t fletcher8(int len, uint8_t data[]) {
    uint8_t sum1 = 0, sum2 = 0;
    int i;
    for (i=0; i<len; i++) {     
        sum1 += (data[i] >> 4);   //first 4 bits
        sum2 += sum1;
     
        sum1 += (data[i] & 15);   //last 4 bits
        sum2 += sum1;
     
        sum1 %= 16;   //modulo 15
        sum2 %= 16;
    }
    return ((sum2) << 4) + (sum1);
}

std::vector<uint8_t> compactPreambleBytes(const int &pseqnum, const int &ncmds) {
    std::vector<uint8_t> bytes;
    int i;
    for (i=0; i<PRE_LEN; i++) {
        if (i==0) {
            bytes.push_back(PCK_OK << 7);          /* ok1 */
            bytes[i] |= (pseqnum & 7) << 4;    /* index3 */
            bytes[i] |= (ncmds & 15);           /* cmdCnt4 */
        }
    }
    return bytes;
}

std::vector<uint8_t> compactCommandBytes(const CatheterChannelCmd& cmd) {
    std::vector<uint8_t> bytes;
    
    unsigned int data12;
    unsigned int cmd4;
    compactCatheterCmd(cmd, &cmd4, &data12);    
    
    int i;
    for (i=0; i<CMD_LEN; i++) {
        if (i==0) {
            bytes.push_back(cmd.channel << 4);          // bits 1-4
            bytes[i] |= (cmd4 & 15);          // bits 5-8
        } else if (i==1) {            
            bytes.push_back((data12 >> 6) & 63);   // bits 9-16  (first 6 bits of DAC data (in the lower 6) (modified to match the arduin0
        } else if (i==2) {
            bytes.push_back((data12 & 63));    // bits 17-20 (last 6 bits of DAC data)
        }  // last 4 bits reserved
    }
    return bytes;
}

CatheterChannelCmd expandCommandBytes(const std::vector<uint8_t>& cmdBytes, int & index)
{
	CatheterChannelCmd result;
	// byte 1
	result.channel = cmdBytes[index] >> 4;
	
	bool poll(false),en(false),update(false);
	
	dir_t dir;

	expandCmdVal(cmdBytes[index], &poll, &en, &update, &dir);
	
	// bytes 2 and 3 (last 4 bits reserved)
	result.enable = en;
	// pull off the DAC value:
	uint16_t cmdData(((uint16_t)(cmdBytes[index+1]) << 6) + (cmdBytes[index+2] % 64));

	//convert the dac value to a double.
	result.currentMilliAmp = convert_current_by_channel_res2ma(static_cast<int> (cmdData), dir, result.channel);
	
	index += 3;
	// If the Poll bit is true, pull off the adc value
	if (poll)
	{
		result.poll = true;
		uint16_t adcData((static_cast<uint16_t> (cmdBytes[index]) << 8) + (cmdBytes[index+1]));
		//convert adc bits to a double.
		result.currentMilliAmp_ADC = convert_current_by_channel_res2ma(static_cast<int> (adcData), dir, result.channel);
		index += 2;
	}
	return result;
}

bool isFirstCommandGlobal(const std::vector<uint8_t>& cmdBytes) {
	if (cmdBytes.size() < PCK_LEN(1)) return false;
	return ((cmdBytes[0] >> 4) == GLOBAL_ADDR);
}

//Used after pre-amble.
std::vector<uint8_t> compactCommandVectBytes(const std::vector<CatheterChannelCmd>& cmdVect)
{
    std::vector<uint8_t> bytes;
	bytes.clear();
	int ncmds = cmdVect.size();
    for (int i(0); i < ncmds; i++)
	{
		std::vector<uint8_t> tempV = compactCommandBytes(cmdVect[i]);
        bytes.insert(bytes.end(), tempV.begin(), tempV.end()); 
    }
    return bytes;
}

std::vector<uint8_t> compactPostambleBytes(const int &pseqnum) {
    std::vector<uint8_t> bytes;
    int i;
    for (i=0; i<POST_LEN; i++) {
        if (i==0) {
            bytes.push_back(pseqnum << 5);  // index3
            bytes[i] |= PCK_OK & 1;     // packet OK bit appended to beginning and end of packet
        }
    }
    return bytes;
} 

// compact the catheter data sets into a string of bytes
//This 
std::vector<uint8_t> compactPacketBytes(const std::vector<CatheterChannelCmd>& cmdVect, const int &pseqnum)
{
    std::vector<uint8_t> bytes;
	bytes.clear();
    
    std::vector<uint8_t> preV = compactPreambleBytes(pseqnum, cmdVect.size());
    std::vector<uint8_t> cmdV = compactCommandVectBytes(cmdVect);
    std::vector<uint8_t> postV = compactPostambleBytes(pseqnum);

    bytes.insert(bytes.end(), preV.begin(), preV.end());
    bytes.insert(bytes.end(), cmdV.begin(), cmdV.end());
    bytes.insert(bytes.end(), postV.begin(), postV.end());
    
	uint8_t chksum = fletcher8(bytes.size(), bytes.data());
    bytes.insert(bytes.end(), 1, chksum);

    return bytes;
}

/* 
bool parseBytes2Cmds(const std::vector<unsigned char>& reply, std::vector<CatheterChannelCmd>& cmds) {

	cmds.clear();

	if (!reply.size()) return false;

	unsigned char nextByte;
	int pseqnum = -1;
	int data = 0;
	bool packet_ok = false;
	unsigned int byte_count = 0;

	for (int b = 0; b < reply.size(); b++) {
		nextByte = reply[b];

		if ((nextByte == '\r' || nextByte == '\n') && (reply.size() - b) <= 2) {
			break;
		}

		if (nextByte >= 128 || !(byte_count % RESPONSE_LEN(1,false,0))) {
			pseqnum = nextByte & 15;
			packet_ok = (nextByte >= 192);
			byte_count = 1;
			data = 0;
		} else {
			switch (byte_count % RESPONSE_LEN(1, false, 0)) {
			case 1:
				data += ((nextByte & 63) << 6);
				break;
			case 2:
				if (packet_ok) {
					data += (nextByte & 63);
					CatheterChannelCmd c = emptyCommand();
					c.currentMilliAmp = data; // this is dac resolution! convert this in the calling method!
					cmds.push_back(c);
				}
				break;
			}
			byte_count++;
		}
	}
	return packet_ok;
} */

comStatus parseBytes2Cmds(std::vector<unsigned char>& bytesRead, std::vector<CatheterChannelCmd>& cmds)
{
	// reset the cmd...
	// cmd.reset();
	cmds.clear();

	// populate the top level command information
	int sizeEst(parsePreamble(bytesRead));

	// calculate the size of the return
	if (sizeEst > bytesRead.size())
	{
		bytesRead.clear();
		return invalid;
	}

	//validate the bytes and fletcher code.
	uint8_t chksum(fletcher8(sizeEst -1, bytesRead.data()));
	if (chksum != bytesRead[sizeEst-1])
	{
		// clear
		bytesRead.clear();
		return invalid;
	}

	// intrepret each byte
	int byteIndex(2);

	while (byteIndex + 3 < sizeEst)
	{
		cmds.push_back(expandCommandBytes(bytesRead, byteIndex));
	}

	bytesRead.erase(bytesRead.begin(), bytesRead.begin() + sizeEst);
	return valid;
	
	
	/*if (!(bytesRead[0] & 128)) return false; // packet-status bit
	
	unsigned int pindex = (bytesRead[0] & 7);
	unsigned int ncmds = (bytesRead[1] >> 4);
	unsigned int npolled = (bytesRead[1] & 15);	
	if (npolled > ncmds) return false;

	bool poll, ena, upd;
	dir_t dir;
	int chan;
	unsigned int dac_data, adc_data;

	chan = (bytesRead[2] >> 4);
	if (chan == GLOBAL_ADDR) {
		if (!(bytesRead.size() == RESPONSE_LEN(ncmds, true, npolled))) return false;
	} else if (!(bytesRead.size() == RESPONSE_LEN(ncmds, false, npolled))) return false;

	CatheterChannelCmd cmd;
	size_t rindex = 2;
	for (size_t i = 0; i < ncmds; i++) {
		chan = (bytesRead[rindex] >> 4);
		expandCmdVal(bytesRead[rindex] & 15, &poll, &ena, &upd, &dir);		
		dac_data = (bytesRead[rindex + 1] << 4) + (bytesRead[rindex + 2] >> 4);
		rindex += 3;
		
		cmd = emptyCommand();
		cmd.channel = chan;
		cmd.currentMilliAmp = convert_current_by_channel_res2ma(dac_data, dir, chan);
		cmd.poll = poll;
		
		if (poll) {
			adc_data = (bytesRead[rindex] << 4) + (bytesRead[rindex + 1] >> 4);
			rindex += 2;
			cmd.currentMilliAmp_ADC = convert_current_by_channel_res2ma(adc_data, dir, chan);
		}
		
		cmds.push_back(cmd);
	}

	std::vector<uint8_t> bytes = bytesSent;//get rid of the const...
	return true;  */
}

int parsePreamble(const std::vector < uint8_t > &inputBytes )
{
	// byte 1 
	uint8_t ok((inputBytes[0] >> 6) % 2);

	//cmdCount
	uint8_t cmdCount(inputBytes[1] >> 4);
	uint8_t pollCount(inputBytes[1] & 15);

	int totalSize(0);
	// if the byte is ok, the expected size is 3 * # cmd + 2 * # poll
	if (ok > 0) 
	{
		totalSize = static_cast<int> (cmdCount)*3 + static_cast<int> (pollCount)*2 + 3;
	}
	else
	{
		// verify the size of a bad packet.
		totalSize = -1;
	}
	return totalSize;
}

// This function seems unneeded.
/*
std::vector<CatheterChannelCmd> padChannelCmds(const std::vector<CatheterChannelCmd>& cmds) {
	// count the number of packets
	int npackets = 0;
	for (int i = 0; i < cmds.size(); i++) {
		if (cmds[i].delayTime > 0 || i == (cmds.size() - 1)) {
			npackets++;
		}
	}
	// define new command vector with a command for each channel in each packet
	std::vector<CatheterChannelCmd> newCmds(npackets * NCHANNELS, resetCommand());
	// iterate through all new commands and conditionally assign them based on old commands
	int cmdptr = 0;
	for (int i = 0; i < npackets; i++) {
		for (int j = 0; j < NCHANNELS; j++) {
			if (cmds[cmdptr].channel == (j + 1)) {
				newCmds[(i * NCHANNELS) + j] = cmds[cmdptr];
				cmdptr++;
			} else {
				newCmds[(i * NCHANNELS) + j].channel = j + 1;
				if (i > 0) {
					newCmds[(i * NCHANNELS) + j] = newCmds[((i - 1) * NCHANNELS) + j];
				}
			}
		}
	}
	return newCmds;
} */


