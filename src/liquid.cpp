
#include "system.h"


const char* liquid_name( int i, bool known )
{
  if( i < 0 || i >= MAX_LIQUID ) {
    roach( "Liquid_Name: Impossible liquid." );   
    return "non-existent liquid";
  }

  return( known ? liquid_table[i].name : liquid_table[i].color );
}


const char* liquid_name( obj_data* obj )
{
  int i  =  obj->value[2];

  if( i < 0 || i >= MAX_LIQUID ) {
    roach( "Liquid_Name: Impossible liquid." );   
    return "non-existent liquid";
  }

  return( is_set( obj->extra_flags, OFLAG_KNOWN_LIQUID ) ? liquid_table[i].name : liquid_table[i].color );
}


bool react_filled( char_data* ch, obj_data* obj, int liquid )
{
  int spell;

  if( ( spell = liquid_table[liquid].spell ) == -1 )
    return FALSE;

  if( spell < SPELL_FIRST || spell >= WEAPON_FIRST ) {
    bug( "Do_fill: Liquid with non-spell skill." );
    return FALSE;
  }

  return( *spell_table[spell-SPELL_FIRST].function )( NULL, ch, obj, 10, -4 );
}


