#include "system.h"


bool water_logged( room_data* room )
{
  if( terrain_table[ room->sector_type ].water_surface
    || terrain_table[ room->sector_type ].underwater
    || terrain_table[ room->sector_type ].shallow )  
    return TRUE;

  return FALSE;
}


bool deep_water( room_data* room )
{
  if( terrain_table[ room->sector_type ].water_surface
    || terrain_table[ room->sector_type ].underwater ) 
    return TRUE;

  return FALSE;
}


bool is_submerged( char_data* ch )
{
  room_data* room;

  if( !ch || !ch->array || ( room = Room( ch->array->where ) ) == NULL )
    return FALSE;

  if( terrain_table[ room->sector_type ].underwater )
    return TRUE;

  if( terrain_table[ room->sector_type ].water_surface && !ch->can_fly( ) ) // && !ch->can_float( ) )
    return TRUE;

  return FALSE;
}


void enter_water( char_data* ch )
{
  char         tmp  [ TWO_LINES ];
  char        list  [ 5 ][ ONE_LINE ];
  int        count  = 0;
  obj_data*    obj;
  room_data*  room;

  if( !ch )
    return;

  // shield spells dispelled by entering water (should be all, shock doing damage)

  if( is_set( ch->affected_by, AFF_FIRE_SHIELD ) ) 
    strip_affect( ch, AFF_FIRE_SHIELD );
  
  if( is_set( ch->affected_by, AFF_HOAR_FROST ) ) 
    strip_affect( ch, AFF_HOAR_FROST );

  if( is_set( ch->affected_by, AFF_FLAME_SHIELD ) )
    strip_affect( ch, AFF_FLAME_SHIELD );

  if( is_set( ch->affected_by, AFF_FIERY_SHIELD ) )
    strip_affect( ch, AFF_FIERY_SHIELD );

  if( is_set( ch->affected_by, AFF_INFERNO_SHIELD ) )
    strip_affect( ch, AFF_INFERNO_SHIELD );

  if( is_set( ch->affected_by, AFF_FROST_SHIELD ) )
    strip_affect( ch, AFF_FROST_SHIELD );

  if( is_set( ch->affected_by, AFF_HOAR_SHIELD ) )
    strip_affect( ch, AFF_HOAR_SHIELD );

  if( is_set( ch->affected_by, AFF_ABSOLUTE_ZERO ) )
    strip_affect( ch, AFF_ABSOLUTE_ZERO );

  // drowning event

  if( ch->array && ( room = Room( ch->array->where ) ) != NULL && terrain_table[ room->sector_type ].underwater
    && ( ch->species == NULL || !is_set( ch->affected_by, AFF_WATER_BREATHING ) ) )
    add_queue( new event_data( execute_drown, ch ), number_range( 50, 75 ) );

  // dispellation of floating objects that aren't rust proof

  if( ( obj = ch->Wearing( WEAR_FLOATING ) ) != NULL ) {
    if( !is_set( obj->extra_flags, OFLAG_WATER_PROOF ) ) {
      sprintf( list[count++], obj->Name( ch ) );
      unequip( ch, obj );
    }
  }

  /*
  for ( obj = ch->Wearing(WEAR_FLOATING); obj != NULL;
        obj = obj->next_list ) {
    if( !is_set( obj->extra_flags, OFLAG_WATER_PROOF ) ) {
      sprintf( list[count++], obj->Name( ch ) );
        unequip_char( ch, obj );
      }
    }  
  */

  if( count == 0 )
    return;

  *tmp = '\0';
  for( int i = 0; i < count; i++ ) 
    sprintf( tmp+strlen( tmp ), "%s%s",
      i == 0 ? "" : ( i == count-1 ? "and " : ", " ), list[i] );

  send( ch, "As you enter the water %s you are carrying %s quickly extinguished.\r\n", tmp, count == 1 ? "is" : "are" );
}


/*
 *   WATER BASED SPELLS
 */


bool spell_water_breathing( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_WATER_BREATHING, AFF_WATER_BREATHING );

  return TRUE;
}
