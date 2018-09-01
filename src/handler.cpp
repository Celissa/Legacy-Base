#include "system.h"


/*
 *   LIST GENERATING FUNCTIONS
 */


int get_trust( char_data *ch )
{
  if( ch == NULL || ch->pcdata == NULL )
    return LEVEL_HERO - 1;

  // used to return your level if you didn't have trust, eg a level 90 person would
  // be trusted at level 90... sounds iffy to me
  return ch->pcdata->trust;
}


/*
 *   WEIRD ROUTINES
 */


char_data* rand_tank( room_data* room, char_data* leader )
{
  player_data* a_player;
  player_array potential_targets;
  int j;

  //Build a set of all players in the room who are following
  //leader or are leader themselves.
  //If leader isn't the actual leader of the group adjust
  //leader to the right person.

  if( leader->leader != NULL )
    leader = leader->leader;

  for( j = 0; j < room->contents.size; j++ ) {
    a_player = player( room->contents.list[j] );
    if( a_player != NULL && a_player->shdata != NULL &&
      !is_set( a_player->pcdata->pfile->flags, PLR_WIZINVIS ) &&
      ( a_player->leader == leader || a_player == leader ) )
        potential_targets += a_player;
  }

  if( potential_targets.size == 0 )
    return NULL;

  //Pick a player at random from that set.
  j = number_range( 0, potential_targets.size - 1 );
  a_player = potential_targets.list[j];

  return a_player;
}

char_data* rand_player( room_data* room )
{
  player_data* a_player;
  player_array potential_targets;
  int j;

  // Build a set of _all_ players in the room.  Seen, unseen, you name it!
  // Do we want to filter out anyone?  Imms, hiddens, invis?  Imms for now.
  for( j = 0; j < room->contents.size; j++ ) {
    a_player = player( room->contents.list[j] );
    if ( a_player != NULL && a_player->shdata != NULL && !is_set( a_player->pcdata->pfile->flags, PLR_WIZINVIS ) ) {
      potential_targets += a_player;
    }
  }

  if( potential_targets.size == 0 )
    return NULL;

  // Pick a player at random from that set.
  j = number_range( 0, potential_targets.size - 1 );
  a_player = potential_targets.list[j];

  return a_player;
}


char_data* rand_victim( char_data* ch )
{
  char_data*  rch;
  char_array potential_targets;
  int j;

  if( ch->array == NULL )
    return NULL;

  // Count # people that ch sees in the room, or that is fighting ch.
  for( j = 0; j < ch->array->size; j++ ) {
    rch = character( ch->array->list[j] ); 
    if ( rch != NULL && rch != ch && (rch->Seen(ch) || rch->fighting == ch) ) {
      potential_targets += rch;
    }
  }

  if( potential_targets.size == 0 )
    return NULL;

  // Pick a character at random from that set.
  j = number_range( 0, potential_targets.size - 1 );
  rch = potential_targets.list[j];

  return rch;
} 
 
