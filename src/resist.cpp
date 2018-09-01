#include "system.h"


/*
 *   CHARACTER SAVE ROUTINES
 */


bool char_data :: save_vs_poison( int )
{
  if( shdata->race == RACE_UNDEAD || is_set( affected_by, AFF_RESIST_POISON ) )
    return true;

  return( number_range( 0, 40 ) < Constitution( )+shdata->level/10 );    
}

