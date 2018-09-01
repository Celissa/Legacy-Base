// Selt's Somewhat Unique ID system

#ifndef __SUID_H
#define __SUID_H

typedef UINT64 suid;

extern suid get_suid( );
extern suid atosuid( const char* text );

#endif

