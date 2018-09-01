/* ************************************************************************
*   File: selt_telnet.h             Selt's Mysteria Telnet Implimentation *
*  Usage: Partial Telnet Implimentation                                   *
*   (c) 1998+ Julian Forsythe (julian@rainchild.com)                      *
************************************************************************ */


#ifndef _SELT_TELNET_H
#define _SELT_TELNET_H

// telnet commands
#define TC_IAC        255     // interpret as command
#define TC_DONT       254     // don't use this option
#define TC_DO         253     // do use this option
#define TC_WONT       252     // won't use this option
#define TC_WILL       251     // will use this option
#define TC_SB         250     // start of subnegotiation
#define TC_SE         240     // end of subnegotiation

// telnet options
#define TO_ECHO         1     // echo
#define TO_CR          10     // negotiate CR
#define TO_FF          13     // negotiate FF
#define TO_TERM        24     // terminal type
#define TO_MCCP        85     // MUD Compression Protocol (MCCP)
#define TO_MCCP2       86     // MUD Compression Protocol version 2(MCCP)
#define TO_MSP         90     // MUD Sound Protocol (MSP) www.zuggsoft.com
#define TO_MXP         91     // MUD Extension Protocol (MXP) www.zuggsoft.com

// subnegotiation options
#define  TS_IS           0     // option is ...
#define  TS_SEND         1     // requestion option send

#endif
