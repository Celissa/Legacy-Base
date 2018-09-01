#include "system.h"


/*
 *   TRACK_DATA CLASS
 */


typedef class Track_Data  track_data;


class Track_Data
{
 public:
  track_data*   next;
  int           race;
  int           to_dir;
  long          decay_time;

  Track_Data( );
  ~Track_Data( );
};


Track_Data :: Track_Data( )
{
  record_new( sizeof( track_data ), MEM_TRACK );

  return;
}


Track_Data :: ~Track_Data( )
{
  record_delete( sizeof( track_data ), MEM_TRACK );

  return;
}


/*
 *  DO_TRACK ROUTINE
 */


void do_track( char_data* ch, char* argument )
{
  if( is_mob( ch ) )
    return;

  if ( ch->get_skill( SKILL_TRACK ) == UNLEARNT ) {
    send( ch, "Tracking is not something you are adept at.\r\n" );
    return;
  }

  if( toggle( ch, argument, "Track", ch->pcdata->pfile->flags, PLR_TRACK ) ) {
    return;
  }

  send( ch, "Usage: track <on|off>\r\n" );
  return;
}


/*
 *   MAKE TRACK ROUTINE
 */


void make_tracks( char_data* ch, room_data* room, int door )
{
  track_data*  track;
  track_data*   next;
  track_data*   prev;

  if( !ch || !ch->Is_Valid( ) || !room || !room->Is_Valid( ) )
    return;

  if( ch->mount != NULL )
    ch = ch->mount;

  if( ch->can_fly( ) || water_logged( room ) ) 
    return;

  track              = new track_data;
  track->decay_time  = time(0) + number_range( 300,1000 );
  track->to_dir      = door;
  track->race        = ch->shdata->race;   
  track->next        = room->track;
  room->track        = track;

  for( prev = NULL; track != NULL; track = next ) {
    next = track->next;
    if( ( track->decay_time -= 150 ) < time( 0 ) ) {
      if( prev == NULL ) {
        room->track = next;
      } else {
        prev->next = next;
      }
      delete track;
    } else {
      prev = track;
    }
  }

  return;
}


/*
 *   SHOW TRACK ROUTINE
 */


void show_tracks( char_data* ch )
{
  room_data*     room  = ch->in_room;
  track_data*   track;
  int        time_val;
  bool          found  = FALSE;

  if( ch->species != NULL || !is_set( ch->pcdata->pfile->flags, PLR_TRACK ) || !ch->in_room->Seen( ch ) ) 
    return;

  time_val = time(0) + 75*MAX_SKILL_LEVEL - 75 * ch->get_skill( SKILL_TRACK );

  for( track = room->track; track != NULL; track = track->next ) {
    if( track->decay_time > time_val ) {
      ch->improve_skill( SKILL_TRACK );
      send( ch, "%sYou see %s tracks heading %s.\r\n", found ? "" : "\r\n", race_table[ track->race ].name, dir_table[ track->to_dir ].name );
      found = TRUE;
    }  
  }
 
  return;
}


