#pragma once
#ifndef COM_DEFS_H
#define COM_DEFS_H


// This file list communication based definitions.
// examples include packet size and channel count.


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

/* @TODO replace the macros with inline functions.*/

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


/* current direction definition. */
enum dir_t { DIR_POS = 1, DIR_NEG = 0 };


#endif // !COM_DEFS_H