#include "system.h"


/*
 *   BUG, IDEA, TYPO FILE ROUTINES
 */


void panic( const char* text )
{
  roach( text );
  roach( "** MUD KILLED **" );

  throw(1);
}


void log(const char *format, ...)
{
  va_list arglist;
  va_start(arglist, format);

  char temp[2048];
  *temp = '\0';
  vsprintf(temp, format, arglist);

  bug(BUG_SYSLOG, temp);
}

void bug( int level, const char* str, int info_level )
{
  FILE*    fp;

  if( str == NULL || str == empty_string )
    return;

  if( ( fp = fopen( BUG_FILE, "ab" ) ) != NULL ) {
    fprintf( fp, "[%s] %s%s", ltime( time(0) ) + 4, str, str[strlen(str) - 1] != '\n' ? "\r\n" : "");
    fclose( fp );
  } else {
    info( "", max(LEVEL_APPRENTICE, info_level), "Error opening bug file.", IFLAG_BUGS, 1 );
    fprintf(stderr, "[BUG] Error opening bug file.\n" );
  }

//  fprintf(stderr, str);
  info( "", max(LEVEL_APPRENTICE, info_level), str, IFLAG_BUGS, level );
}


/*
 *   DEFINE COMMAND
 */


const char* lookup( index_data* index, int number, bool plural )
{
  int i;

  for( i = 0; index[i].value < number && index[i].value != -1; i++ );

  return( plural ? index[i].plural : index[i].singular );
}


void do_define( char_data* ch, char* argument )
{
  char* name [] = { "Acid Damage", "Fire Damage", "Cold Damage",
    "Electrical Damage", "Physical Damage", "Poison Damage",
    "Fame", "Piety", "Reputation", "Holy Damage", "Unholy Damage", "" };

  index_data* index [] = { acid_index, fire_index, cold_index,
    electric_index, physical_index, poison_index, fame_index,
    piety_index, reputation_index, holy_index, unholy_index };

  int           prev;
  int           i, j;

  if( *argument == '\0' ) {
    send_title( ch, "Defined Terms" );
    for( i = 0; *name[i] != '\0'; i++ ) 
      send( ch, "%20s%s", name[i], i%3 == 2 || *name[i+1] == '\0'
        ? "\r\n" : "" );
    return;
    }

  for( i = 0; *name[i] != '\0'; i++ ) {
    if( fmatches( argument, name[i] ) ) {
      page_title( ch, name[i] );
      prev = ( index[i][0].value < 0 ? -1000 : 0 );
      for( j = 0; ; j++ ) {
        if( j == 0 && index[i][0].value == 0 )
          continue;
        if( index[i][j].value == -1 ) {
          page( ch, "%39s   %d+\r\n", index[i][j].singular, prev );
          return;
          }
        if( index[i][j].value == prev )
          page( ch, "%39s   %d\r\n", index[i][j].singular, prev );
        else
          page( ch, "%39s   %d to %d\r\n",
            index[i][j].singular, prev, index[i][j].value );
        prev = index[i][j].value+1;
        }
      }
    }

  send( ch, "Unknown field - see help define.\r\n" );
}


/*
 *   TYPO COMMAND
 */


void do_typo( char_data* ch, char* argument )
{
  char tmp [ MAX_STRING_LENGTH ];

  if( *argument == '\0' ) {
    send( ch, "Room #%d\r\n\r\n", ch->in_room->vnum );
    send( ch, "What typo do you wish to report?\r\n" );
    return;
  } 

  if( strlen( ch->in_room->comments ) > MAX_STRING_LENGTH-MAX_INPUT_LENGTH-30 ) {
    send( ch, "Comment field at max length - typo ignored.\r\n" );
    return;
  }

  ch->in_room->area->modified = TRUE;

  sprintf( tmp, "%s[%s] %s\r\n", ch->in_room->comments, ch->real_name( ), argument );

  free_string( ch->in_room->comments, MEM_ROOM );
  ch->in_room->comments = alloc_string( tmp, MEM_ROOM );

  send( "Typo noted - thanks.\r\n", ch );

  return;
}    


/*
 *   RANDOM SUPPORT ROUTINES
 */


bool player_in_room( room_data* room )
{
  char_data* ch;

  if( room != NULL )
    for( int i = 0; i < room->contents; i++ )
      if( ( ch = character( room->contents[i] ) ) != NULL 
        && ch->pcdata != NULL && !is_apprentice( ch ) )
        return TRUE;

  return FALSE;
};










