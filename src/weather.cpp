#include "system.h"


weather_data  weather;

const char*  game_month_name    [ MONTHS_IN_GAME_YEAR ]  = { "Cuimnhe", "Pakhane", "Grai Te", "Na'Suan", "Arlmaugh", "Navain", "Searg", "Dorchva" };
const int    days_in_game_month [ MONTHS_IN_GAME_YEAR ]  = {    40,         40,        40,       40,         40,        40,       39,      38     };
const char*  game_day_name      [ DAYS_IN_GAME_WEEK ]    = { "Ahldya", "Sealldya", "Thejdya", "Maeldya", "Kyardya", "Amhairc", "Saras" };

int days_in_year = 0;


void init_time( )
{
  days_in_year = 0;

  for( int i = 0; i < MONTHS_IN_GAME_YEAR; i++ )
    days_in_year += days_in_game_month[ i ];
}


// encode/decode time experimental ... untested
// purpose - convert the current mud date into an integer that can be stored in an acode

int encode_time( int year, int month, int day, int hour, int minute )
{
  // sanity check
  if( month > MONTHS_IN_GAME_YEAR || day > days_in_game_month[ month ] || hour > 23 || minute > 59 )
    return 0;

  int time = year * days_in_year;
  for( int i = 0; i < month; i++ )
    time += days_in_game_month[ i ];

  time += day;

  time *= 1440;
  time += hour * 60;
  time += minute;

  return time;
}


void decode_time( int time, int &year, int &month, int &day, int &hour, int &minute )
{
  int dmy = time/1440;  // extract day, month, year
  int hm = time%1440;   // extract hours and mintues

  minute = hm%60; // grab minute
  hour = hm/60; // grab hour

  year = dmy / days_in_year; // grab year
  dmy -= year * days_in_year; // subtract year value from dmy, then loop for month/day

  month = 0;
  day = 1;
  for( int i = 0; i < MONTHS_IN_GAME_YEAR; i++ ) {
    if( dmy > days_in_game_month[ i ] ) {
      month++;
      dmy -= days_in_game_month[ i ];
    } else {
      day = dmy;
      break;
    }
  }
}


int now( )
{
  return encode_time( weather.year, weather.month, weather.day, weather.hour, weather.minute );
}


int week_day( void )
{
  int day = weather.year * days_in_year;
  for( int i = 0; i < weather.month; i++ )
    day += days_in_game_month[ i ];
  day += weather.day;
 
  return day % DAYS_IN_GAME_WEEK;
}


void time_update( void )
{
  char         tmp  [ ONE_LINE ];
  
  tmp[0] = '\0';

  // TFE did 6 seconds RT = 1 minute GT
  // TFH did 6 seconds RT = 6 minutes GT
  // added a new 'pulse' in update handler to smoothly incriment time

  // increase minute
  weather.minute += 1;

  // increase hour if necessary
  if( weather.minute > 59 ) {
    weather.hour++;
    weather.minute = 0;

    switch ( weather.hour ) {
    case  5:
      strcat( tmp, "The day has begun.\r\n" );
      break;

    case  6:
      strcat( tmp, "The sun rises in the east.\r\n" );
      break;

    case 19:
      strcat( tmp, "The sun slowly disappears in the west.\r\n" );
      break;

    case 20:
      strcat( tmp, "The night has begun.\r\n" );
      break;

    case 24:
      weather.hour = 0;
      weather.day++;
      break;
    }
  }

  // increase month if necessary
  if( weather.day > days_in_game_month[ weather.month ] ) {
    weather.day = 1;
    weather.month++;
  }

  // increase year if necessary
  if( weather.month >= MONTHS_IN_GAME_YEAR ) {
    weather.month = 0;
    weather.year++;
  }

  weather.sunlight = sunlight( 60*weather.hour+weather.minute );

  if( *tmp != '\0' ) {
    link_nav links( &link_list );
    for( link_data* link = links.first(); link != NULL; link = links.next() ) {
      if( link->connected == CON_PLAYING && link->character && link->character->in_room
        && link->character->in_room->is_outside( ) && link->character->is_awake( )
        && link->character->Can_See() && is_set( &link->character->pcdata->message, MSG_WEATHER ) )
        send( tmp, link->character );
    }
  }
}


const char* sky_state( )
{
  if( weather.hour < 5 || weather.hour > 20 ) 
    return "Night";

  if( weather.hour < 6  )  return "Early Morning";
  if( weather.hour < 8  )  return "Mid-Morning";
  if( weather.hour < 14 )  return "Near-Noon";
  if( weather.hour < 17 )  return "Afternoon";
   
  return "Evening";
}
    

