#pragma once
#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <vector>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

//This file defines a bunch of odds and ends and should be better named and moved.

/* number of channels being used */
#define NCHANNELS 6 

/* address which arduino interprets as GLOBAL (i.e., all channels) */
#define GLOBAL_ADDR 0

/* number of bytes in a channel command, preamble, postamble, and packet checksum */
#define CMD_LEN 3  
#define PRE_LEN 1
#define POST_LEN 1
#define PCK_CHK_LEN 1

/* error codes for arduino to send back to PC */
#define PRE_ERR 1
#define POST_ERR 2
#define PCK_CHK_ERR 4

/* macro to compute the size of a packet sent*/
#define PCK_LEN(N_CMDS) (PRE_LEN + (CMD_LEN*N_CMDS) + POST_LEN + PCK_CHK_LEN)
#define NCMDS(PCKLEN) ((PCKLEN-PRE_LEN-POST_LEN-PCK_CHK_LEN)/CMD_LEN)

/* macro to compute the size of a response packet */
//#define RESPONSE_LEN(ncmds, global, npolled) (global ? (3*NCHANNELS) : (3*ncmds))
#define RESPONSE_LEN(ncmds, global, npolled) (3 + 3 * (global ? NCHANNELS : ncmds) + 2 * ((global && npolled) ? NCHANNELS : npolled))

#define PCK_OK 1
#define DAC_RES 4096
#define DAC_RES_OFF 0 //4095

/* command value bits */
#define DIR_BIT 0
#define UPD_BIT 1
#define ENA_BIT 2
#define POL_BIT 3

enum dir_t {DIR_POS=1, DIR_NEG=0};

enum comStatus {
	invalid = -1, valid = 0, none = -2
};

// one packet component
struct CatheterChannelCmd {
	int channel;
    bool poll;
	bool enable;
	double currentMilliAmp;
	double currentMilliAmp_ADC;
	CatheterChannelCmd() : channel(0), poll(false), currentMilliAmp(0), currentMilliAmp_ADC(0) {}
};

// parsed into a full packet
struct CatheterChannelCmdSet {
	std::vector < CatheterChannelCmd > commandList;
	long delayTime;
	CatheterChannelCmdSet() : delayTime(0), commandList() {}
};

struct CatheterPacket {
    unsigned int pseqnum;
	unsigned int delay; // unused currently: fix this
    std::vector<CatheterChannelCmd> cmds;
	CatheterPacket() : pseqnum(0), delay(0) {}
};

CatheterChannelCmdSet resetCmd();
CatheterChannelCmdSet pollCmd();



///////////////////////
// "private" methods //
///////////////////////

/** \brief uint8_t compactCmdVal(int poll, int en, int update, dir_t dir): 
    return the command number (0-15) associated with the specified state */
uint8_t compactCmdVal(const bool &poll, const bool &en, const bool &update, const dir_t &dir);

/** \brief void expandCmdVal(uint8_t cmdVal, int * poll, int * en, int * update, dir_t * dir): 
    return the state associated with the specified commnd number (0-15) */
void expandCmdVal(const uint8_t &cmdVal, bool* poll, bool* en, bool* update, dir_t * dir);

/** \brief void compactCatheterCmd(CatheterChannelCmd& cmd, unsigned int* cmd4, unsigned int* data12): 
    extract the 12-bit DAC data and 4-bit command value from a CatheterChannelCmd */
void compactCatheterCmd(const CatheterChannelCmd& cmd, unsigned int* cmd4, unsigned int* data12);

/** \brief void expandCatheterCmd(CatheterChannelCmd& cmd, bool* enable, bool* update, dir_t* dir): 
    extract the semantic command bit values from a CatheterChannelCmd */
void expandCatheterCmd(const CatheterChannelCmd& cmd, bool* enable, bool* update, dir_t* dir);

/** \brief uint8_t fletcher8(int len, uint8_t bytes[]): 
    compute the fletcher checksum of an array of bytes of length 'len' using blocksize=8. 
    ('len' <= the actual length of the array, since we may not want to include all elements 
    of the array in the computation.) */
uint8_t fletcher8(int len, uint8_t bytes[]);

/** \brief std::vector<uint8_t> compactPacketBytes(std::vector<catheterChannelCmd>&, const int &pseqnum): 
    compacts a packet into a string to be sent over serial */
std::vector<uint8_t> compactPacketBytes(const std::vector<CatheterChannelCmd>&, const int& pseqnum);

/** \brief std::vector<uint8_t> compactPreambleBytes(int pseqnum,int cmdCount): 
    uses the command index and number of commands to generate the preamble bit(s). */
std::vector<uint8_t> compactPreambleBytes(const int &pseqnum, const int& cmdCount);

/** \brief std::vector<uint8_t> compactPostamble(int pseqnum): 
    uses the command index to generate the postamble bit(s). */
std::vector<uint8_t> compactPostambleBytes(const int &pseqnum);

/** \brief std::vector<uint8_t> compactCommandVectBytes(const std::vector<catheterChannelCmd>&): 
    generalize version of generating a command for multiple channels simultaneously. */
std::vector<uint8_t> compactCommandVectBytes(const std::vector<CatheterChannelCmd>& cmds);

/** \brief std::vector<uint8_t> compactCommandBytes(const catheterChannelCmd): 
    compacts a single arduino command into a 3 byte packet. */
std::vector<uint8_t> compactCommandBytes(const CatheterChannelCmd& cmd);

bool expandCommandBytes(const std::vector<uint8_t>& cmdBytes, int* channel, int* cmdVal, int* cmdData);
bool isFirstCommandGlobal(const std::vector<uint8_t>& cmdBytes);

//////////////////////
// "public" methods //
//////////////////////

/** \brief std::vector<CatheterChannelCmd> padChannelCmds(std::vector<CatheterChannelCmd> cmds):
    ensures there is a command for each channel that will be part of a packet */
std::vector<CatheterChannelCmd> padChannelCmds(const std::vector<CatheterChannelCmd>& cmds);

/** \brief void getPacketBytes(std::vector<CatheterChannelCmd>& commandVect, std::vector<std::vector<uint8_t>>& pbytes, 
                               std::vector<int>& pdelays):
    separate a list of catheter channel commands into packets-- packet bytes and packet delays */
void getPacketBytes(const int& pseqnum, const std::vector<CatheterChannelCmd>& cmds, std::vector<std::vector<uint8_t>>& cmdBytes, 
                    std::vector<int>& delays, std::vector<int>& ncmdsPerPacket);

/** \brief bool parseBytes2Cmds(const std::vector<unsigned char>& reply, std::vector<CatheterChannelCmd>& cmds): 
    validates returned bytes for a packet containing commands for all channels and
    returns the channel data parsesd from the return values (or -1 on error) 
    NB: the currentMilliAmp field is filled with DAC RES data! */
comStatus parseBytes2Cmds(std::vector<unsigned char>& reply, std::vector<CatheterChannelCmd>& cmds);

// Parse the preamble.
int parsePreamble(const std::vector < uint8_t > &inputBytes );

/** \brief void emptyCommand(CatheterChannelCmd& cmd): populate a generic, empty channel command */
CatheterChannelCmd emptyCommand();
/** \brief CatheterChannelCmd resetCommand(): generate a global reset catheter channel command */
CatheterChannelCmd resetCommand();
/** \brief std::vector<uint8_t> resetCommandBytes(): generate the bytes for a global reset command */
std::vector<uint8_t> resetCommandBytes();

/** \brief void get_current_constants_by_channel(double* m, double* y, int chan, bool revG = true, bool set1 = false):
	return the slope and y-intercept for the current conversion function for a given channel. Different operation depending
	on whether we are using version >= revision G. bool set1 only applies if using version < revision G, and is for current values
	less than 10 milliamps. */
void get_current_constants_by_channel(double* m, double* y, int chan, bool revG = true, bool set1 = false);
/** \brief unsigned int convert_current_by_channel(double ma, int chan): return the DAC resolution associated with a milliamp current */
unsigned int convert_current_by_channel_ma2res(double ma, int chan);
/** \brief double convert_current_by_channel(int res, dir_t dir, int chan): return the milliamp current associated witha DAC resolution */
double convert_current_by_channel_res2ma(int res, dir_t dir, int chan);

#endif
