#pragma once

#ifndef CATHETER_COM_H
#define CATHETER_COM_H

#include "com/communication_definitions.h"

#include <vector>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>


/**
 \brief This enumerated type is meant to list the parsing results of an incoming command.
 */
enum comStatus {
	invalid = -1, valid = 0, none = -2
};

/**
 \brief This is the catheter command structure.

 It contains a channel, poll information, an enable flag,
 And update flag, and both a DAC and ADC setting.
 */
struct CatheterChannelCmd {
	int channel;
	bool poll;
	bool enable;
	bool update;
	dir_t dir;
	double currentMilliAmp;
	double currentMilliAmp_ADC;

	// default constructor:
	CatheterChannelCmd() : channel(0), poll(false), currentMilliAmp(0), currentMilliAmp_ADC(0) {}
};


// a set of catheter commands
// meant to be a single packet
/**
 \brief This struct contains a list of commands
 the time delay is meant for after the command is sent.


 */
struct CatheterChannelCmdSet {
	std::vector < CatheterChannelCmd > commandList;
	long delayTime;

	// default constructor. 
	CatheterChannelCmdSet() : delayTime(0), commandList() {}
};


// automatically create a global reset command
CatheterChannelCmdSet resetCmd();

// automatically create a global poll command
CatheterChannelCmdSet pollCmd();

/**
 * \brief std::vector<uint8_t> encodeCommandSet(const CatheterChannelCmdSet&, int pseqnum);
 */
std::vector<uint8_t> encodeCommandSet(const CatheterChannelCmdSet&, int pseqnum);

/**
 * \brief encode the preamble bytes.
 * The preamble encodes the number of commands as well as the sequence number into it.
 */
std::vector<uint8_t> encodePreamble(int pseqnum, int ncmds);

/**
 * \brief encode the postamble. 
 * currently, all that is included is the sequence number.
 */
std::vector<uint8_t> encodePostamble(int pseqnum);


/** 
 * \brief std::vector<uint8_t> encodeSingleCommand(const catheterChannelCmd):
 * compacts a single arduino command into a 3 byte packet.
 * 
 * This function is used in the encode command set.
 */
std::vector<uint8_t> encodeSingleCommand(const CatheterChannelCmd& cmd);


/** \brief uint8_t fletcher8(int len, uint8_t bytes[]):
compute the fletcher checksum of an array of bytes of length 'len' using blocksize=8.
('len' <= the actual length of the array, since we may not want to include all elements
of the array in the computation.) */
uint8_t fletcher8(int len, uint8_t bytes[]);

/**
 \brief bool parseBytes2Cmds(const std::vector<unsigned char>& reply, std::vector<CatheterChannelCmd>& cmds):
 validates returned bytes for a packet containing commands for all channels and
 returns the channel data parsesd from the return values (or -1 on error)
 NB: the currentMilliAmp field is filled with DAC RES data!

*/
comStatus parseBytes2Cmds(std::vector<uint8_t>& reply, std::vector<CatheterChannelCmd>& cmds);


/**
 * \brief parsePreamble(const std::vector < uint8_t > &)
 * 
 * parses the incoming data from the arduino in order
 * to validate that the command packet was valid.
 */
int parsePreamble(const std::vector < uint8_t > &);

#endif