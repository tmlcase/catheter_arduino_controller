
#include "com/catheter_commands.h"
#include "hardware/digital_analog_conversions.h"


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

/* calculate 8-bit fletcher checksum using blocksize=4 */
// This is for error correction.
uint8_t fletcher8(int len, uint8_t data[]) {
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



CatheterChannelCmdSet resetCmd()
{
	CatheterChannelCmdSet resetCmdSet;
	resetCmdSet.commandList.push_back(CatheterChannelCmd());
	return resetCmdSet;
}

CatheterChannelCmd emptyCommand() {
	CatheterChannelCmd cmd;
	cmd.channel = 0;
	cmd.currentMilliAmp = 0;
	cmd.currentMilliAmp_ADC = 0;
	cmd.poll = false;
	return cmd;
}

//overall encoding


std::vector<uint8_t> encodeCommandSet(const CatheterChannelCmdSet& cmds, int pseqnum)
{
	std::vector<uint8_t> encodedSet;
	std::vector<uint8_t> tempEncoding;

	encodedSet.clear();
	
	int n(cmds.commandList.size());

	tempEncoding = encodePreamble(pseqnum, n);


	encodedSet.insert(encodedSet.end(), tempEncoding.begin(), tempEncoding.end());

	tempEncoding.clear();

	for (int ind(0); ind < n; ind++)
	{
		tempEncoding = encodeSingleCommand(cmds.commandList[ind]);
		encodedSet.insert(encodedSet.end(), tempEncoding.begin(), tempEncoding.end());
		tempEncoding.clear();
	}
	
	tempEncoding = encodePostamble(pseqnum);
	encodedSet.insert(encodedSet.end(), tempEncoding.begin(), tempEncoding.end());
	uint8_t chksum = fletcher8(encodedSet.size(), encodedSet.data());
	encodedSet.insert(encodedSet.end(), 1, chksum);
	// encode the preamble.
	return encodedSet;
}

std::vector<uint8_t> encodePreamble(int pseqnum, int ncmds)
{
	std::vector<uint8_t> bytes;
	int i;
	for (i = 0; i<PRE_LEN; i++) {
		if (i == 0) {
			bytes.push_back(PCK_OK << 7);          /* ok1 */
			bytes[i] |= (pseqnum & 7) << 4;    /* index3 */
			bytes[i] |= (ncmds & 15);           /* cmdCnt4 */
		}
	}
	return bytes;
}

std::vector<uint8_t> encodeSingleCommand(const CatheterChannelCmd& cmd)
{
	std::vector<uint8_t> bytes;


	uint8_t encodedByte = 0;
	if (cmd.poll)   encodedByte |= (1 << POL_BIT);
	if (cmd.enable)     encodedByte |= (1 << ENA_BIT);
	encodedByte |= (1 << UPD_BIT); //always update.
	//if (cmd.update)   encodedByte |= (1 << UPD_BIT);
	encodedByte |= (cmd.currentMilliAmp > 0.0) ? (DIR_POS << DIR_BIT) : (DIR_NEG << DIR_BIT);

	// bit 1-4 is the channel number
	// bit 5-8 is the encoded channel command information.
	uint8_t firstByte(cmd.channel << 4 | (encodedByte & 15));

	uint16_t dacSetting(milliAmp2Dac(cmd.currentMilliAmp));

	for (int i(0); i < CMD_LEN; i++) {
		if (i == 0)
		{
			bytes.push_back(firstByte);          // bits 1-4
		}
		else if (i == 1)
		{
			// bits 8-15  (first 6 bits of DAC data (in the lower 6) (modified to match the arduin0
			bytes.push_back((dacSetting >> 6) & 63);   
		}
		else if (i == 2)
		{
			// bits 16-23 (last 6 bits of DAC data)
			bytes.push_back((dacSetting & 63));   
		}
	}
	return bytes;
}

std::vector<uint8_t> encodePostamble(int pseqnum) {
	std::vector<uint8_t> bytes;
	int i;
	for (i = 0; i < POST_LEN; i++) {
		if (i == 0) {
			bytes.push_back(pseqnum << 5);  // index3
			bytes[i] |= PCK_OK & 1;     // packet OK bit appended to beginning and end of packet
		}
	}
	return bytes;
}


CatheterChannelCmd parseSingleCommand(const std::vector<uint8_t>& cmdBytes, int & index)
{
	CatheterChannelCmd result;
	// byte 1
	result.channel = cmdBytes[index] >> 4;

	bool poll(false), en(false), update(false);

	//expandCmdVal(, &poll, &en, &update, &dir);
	result.poll = (cmdBytes[index] >> POL_BIT) & 1;
	result.enable = (cmdBytes[index] >> ENA_BIT) & 1;
	result.update = (cmdBytes[index] >> UPD_BIT) & 1;
	 
	result.dir = ((cmdBytes[index] >> DIR_BIT) & 1) ? DIR_POS : DIR_NEG;


	// bytes 2 and 3 (last 4 bits reserved)
	result.enable = en;
	// pull off the DAC value:
	uint16_t cmdData(((uint16_t)(cmdBytes[index + 1]) << 6) + (cmdBytes[index + 2] % 64));

	//convert the dac value to a double.
	result.currentMilliAmp = dac2MilliAmp(cmdData, result.dir);

	index += 3;
	// If the Poll bit is true, pull off the adc value
	if (poll)
	{
		result.poll = true;
		uint16_t adcData((static_cast<uint16_t> (cmdBytes[index]) << 8) + (cmdBytes[index + 1]));
		//convert adc bits to a double.
		result.currentMilliAmp_ADC = adc2MilliAmp(adcData);
		index += 2;
	}
	return result;
}

// generate, populate, and return a reset command 
CatheterChannelCmdSet resetCommand()
{
	CatheterChannelCmd cmd;
	cmd.channel = 0; //global
	cmd.currentMilliAmp = 0;
	cmd.currentMilliAmp_ADC = 0;
	cmd.poll = false;
	CatheterChannelCmdSet resetCmdSet;
	resetCmdSet.commandList.resize(1, cmd);
	resetCmdSet.delayTime = 0;
	return resetCmdSet;
}


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
		if ((nextByte == '\r' || nextByte == '\n') && (reply.size() - b) <= 2)
		{
			break;
		}
		if (nextByte >= 128 || !(byte_count % RESPONSE_LEN(1, false, 0)))
		{
			pseqnum = nextByte & 15;
			packet_ok = (nextByte >= 192);
			byte_count = 1;
			data = 0;
		}
		else 
		{
			switch (byte_count % RESPONSE_LEN(1, false, 0))
			{
			case 1:
				data += ((nextByte & 63) << 6);
				break;
			case 2:
				if (packet_ok)
				{
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
}


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
	uint8_t chksum(fletcher8(sizeEst - 1, bytesRead.data()));
	if (chksum != bytesRead[sizeEst - 1])
	{
		// clear
		bytesRead.clear();
		return invalid;
	}

	// intrepret each byte
	int byteIndex(2);

	while (byteIndex + 3 < sizeEst)
	{
		cmds.push_back(parseSingleCommand(bytesRead, byteIndex));
	}

	bytesRead.erase(bytesRead.begin(), bytesRead.begin() + sizeEst);
	return valid;

}

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
	//}


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


	CatheterChannelCmdSet pollCmd()
	{
		CatheterChannelCmdSet pollCmdSet;
		pollCmdSet.commandList.push_back(CatheterChannelCmd());
		pollCmdSet.commandList[0].poll = true;
		pollCmdSet.commandList[0].channel = 0;
		return pollCmdSet;

	}