// Selt's Somewhat Unique ID System

#include "system.h"

// must be used compiled with a unique MUD id ...
// the MUD id values E0 through to FF are reserved for Selt's use only
// you must compile with a MUD id of DF or below

#define MUD_ID 0xFF

// 64 bit number   = 0x0000000000000000  (64 bits)
// sequence offset = 0xFFFF000000000000  (16 bits)  with the low bits first (eg 1505 is recorded as 0515)
// date offset     = 0x0000FFFFFFFF0000  (32 bits)  with the seconds first, minutes, hours, day, month, year
// reboot offset   = 0x000000000000FF00  ( 8 bits)  the reboot counter
// mud id offset   = 0x00000000000000FF  ( 8 bits)  this comes last in the ID since its the least likely thing to change

// second = max 59     63 = 6 bits
// minute = max 59     63 = 6 bits
// hour = max 23       31 = 5 bits
// day = max 31        31 = 5 bits
// month = max 12      15 = 4 bits
// year = max 63       63 = 6 bits
//                        --------
// date stamp              32 bits

// so all in all we'll have (in binary)
// 00000000 00000000 000000 000000 00000 00000 0000 000000 00000000 00000000
// seqlow   seqhigh  second minute hour  day   mon  year   reboot   mud id

unsigned char reboot_number = 0; // loaded/saved and incrimented in load_misc/save_misc


suid get_suid( )
{
  static unsigned short sequence = number_range( 0, 65535 );  // init sequence with a random number, instead o' 0 each reboot
  time_t tt = time( 0 );
  struct tm *t = gmtime( &tt );

  suid id = ( (suid) ( sequence ) )          << 56     // low byte of sequence
          | ( (suid) ( sequence >> 8 ) )     << 48     // high byte of sequence
          | ( (suid) ( t->tm_sec ) )         << 42     // seconds
          | ( (suid) ( t->tm_min ) )         << 36     // minutes
          | ( (suid) ( t->tm_hour ) )        << 31     // hours
          | ( (suid) ( t->tm_mday ) )        << 26     // days
          | ( (suid) ( t->tm_mon + 1 ) )     << 22     // months (0-11) so + 1
          | ( (suid) ( t->tm_year - 100 ) )  << 16     // years since 2000 (tm_year is years since 1900)
          | ( (suid) reboot_number )         <<  8     // reboot counter
          | ( (suid) MUD_ID )                <<  0     // MUD id
  ;

  sequence++;

  return id;
}

suid atosuid( const char* text )
{
  suid id = 0, value;
  if( *text == '#' )
    text++;

  // check length
  if( strlen( text ) != 16 )
    return 0;

  while( *text ) {
    if( isdigit( *text ) )
      value = *text - '0';
    else if( isalpha( *text ) )
      value = toupper( *text ) - 'A' + 10;
    else
      return 0;

    if( value >= 16 )
      return 0;

    id = id * 16 + value;
    text++;
  }

  return id;
}
