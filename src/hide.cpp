#include "system.h"


void spoil_hide( char_data* ch )
{
  bool stat_hiding = is_set( &ch->status, STAT_HIDING );
  bool aff_hiding = is_set( ch->affected_by, AFF_HIDE );
  bool stat_camo = is_set( &ch->status, STAT_CAMOUFLAGED );
  bool aff_camo = is_set( ch->affected_by, AFF_CAMOUFLAGE );
  bool aff_shadow = is_set( ch->affected_by, AFF_SHADOW );

  if( ( !stat_hiding && !aff_hiding ) && ( !stat_camo && !aff_camo ) 
    && !aff_shadow )
    return;

  remove_bit( &ch->status, STAT_CAMOUFLAGED );
  remove_bit( ch->affected_by, AFF_CAMOUFLAGE );
  remove_bit( &ch->status, STAT_HIDING );
  remove_bit( ch->affected_by, AFF_HIDE );
  remove_bit( ch->affected_by, AFF_SHADOW );

  if( ch->species == NULL && ch->pcdata != NULL && ch->pcdata->clss == CLSS_DANCER )
    ch->cast= NULL;

  // What do room denizens do?
  if( ch->array != 0 ) {
    for( int i = ch->array->size - 1; i >= 0; i-- ) {
      char_data *rch = character( ch->array->list[i] );
      if( rch != NULL && ch != rch && ch->Seen( rch ) ) {

        // They notice that ch is there.
        if( !includes( ch->seen_by, rch ) ) {
          send( rch, "++ You notice %s hiding in the shadows! ++\r\n", ch );
          ch->seen_by += rch;
        }
        
        // If aggressive, they leap to attack.
        if( is_aggressive( rch, ch ) ) {
          init_attack( rch, ch );
        }
      }
    }
  }

  if( stat_hiding )
    set_bit( &ch->status, STAT_HIDING );
  if( aff_hiding )
    set_bit( ch->affected_by, AFF_HIDE );
  if( stat_camo )
    set_bit( &ch->status, STAT_CAMOUFLAGED );
  if( aff_camo )
    set_bit( ch->affected_by, AFF_CAMOUFLAGE );

  return;
}


bool leave_camouflage( char_data* ch )
{
  if (!is_set(&ch->status, STAT_CAMOUFLAGED) && !is_set(ch->affected_by, AFF_CAMOUFLAGE))
    return FALSE;

  remove_bit( &ch->status, STAT_CAMOUFLAGED );
  remove_bit( ch->affected_by, AFF_CAMOUFLAGE );

  clear( ch->seen_by );

  send( ch, "You stop camouflaging yourself.\r\n" );
  send_seen( ch, "%s suddenly appears from nowhere.\r\n", ch );

  // What do room denizens do?
  char_data* rch = NULL;
  if( ch->array != NULL ) {
    for( int i = 0; i < ch->array->size; i++ ) {
      rch = character( ch->array->list[i] );
    //If aggressive, they leap to attack.
      if( rch != NULL && rch != ch && ch->Seen( rch ) &&
        is_aggressive( rch, ch ) ) {
        init_attack( rch, ch ); 
      }
    }
  }
  return TRUE;
}


bool leave_shadows( char_data* ch )
{
  if ( !is_set( &ch->status, STAT_HIDING ) && !is_set( ch->affected_by, AFF_HIDE )
    && !is_set( ch->affected_by, AFF_SHADOW ) )
    return FALSE;

  remove_bit( &ch->status, STAT_HIDING );
  remove_bit( ch->affected_by, AFF_HIDE );
  remove_bit( ch->affected_by, AFF_SHADOW );
  if( ch->species == NULL && ch->pcdata->clss == CLSS_DANCER )
    ch->cast = NULL;

  clear( ch->seen_by );

  send( ch, "You stop hiding.\r\n" );
  send_seen( ch, "%s suddenly appears from the shadows.\r\n", ch );

  // What do room denizens do? 
  char_data* rch = NULL;
  if ( ch->array != NULL ) {
    for ( int i = 0; i < ch->array->size; i++ ) {
      rch = character( ch->array->list[i] );
      // If aggressive, they leap to attack.
      if ( rch != NULL && rch != ch && ch->Seen( rch ) && is_aggressive( rch, ch ) ) {
        init_attack( rch, ch );
      }
    }
  }

  return TRUE;
}


bool reveal( char_data* ch )
{
  leave_camouflage( ch );
  leave_shadows( ch );
  strip_affect( ch, AFF_INVISIBLE );
  remove_bit( ch->affected_by, AFF_INVISIBLE );

  return true;
}

