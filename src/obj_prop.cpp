#include "system.h"



int obj_data :: metal( )
{
  int i;

  for( i = MAT_METAL; i < MAT_KRYNITE+1; i++ )
    if( is_set( &materials, i ) )
      return i;

  return 0;
}


int obj_data :: mana_absorbing_metal( )
{
  int i;

  for( i = MAT_METAL; i < MAT_LIRIDIUM; i++ )
    if( is_set( &materials, i ) )
      return i;

  return 0;
}


int obj_clss_data :: affect_levels( )
{
  int level = 0;

  for( int i = 0; i < MAX_ENTRY_AFF_CHAR; i++ ) {
    if( is_set( affect_flags, i ) )
      level += aff_char_table[i].level_cost;
  }

  for( int i = 0; i < MAX_ENTRY_OFLAG; i++ ) {
    if( is_set( extra_flags, i ) )
      level += oflag_table[i].level/100;
  }

  if( !is_empty( affected ) ) {
    for( int i = 0; i < affected; i++ ) {
      affect_data* paf = affected[i];
      if( paf->mmodifier[0] > 0 )
        level += paf->mmodifier[0]*affect_table[paf->mlocation[0]].level/100;
      else
        level += paf->mmodifier[0]*affect_table[paf->mlocation[0]].level/200;
    }
  }

  return level;
}


int obj_clss_data :: metal( )
{
  int i;

  if( is_set( extra_flags, OFLAG_RANDOM_METAL ) )
    return -1;

  for( i = MAT_METAL; i < MAT_KRYNITE+1; i++ )
    if( is_set( &materials, i ) )
      return i;

  return 0;
}
