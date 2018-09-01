#include "system.h"


bool is_switched( char_data* ch )
{
  if( ch->pcdata == NULL || ch->species == NULL )
    return FALSE;

  send( "You can't do that while switched.\r\n", ch );
  return TRUE;
}

bool can_switch( char_data* ch )
{
  if( ch->shifted != NULL ) {
    send( ch, "You may not do that while shifted into another form.\r\n" );
    return FALSE;
  }

  if( ch->species != NULL || ch->pcdata == NULL ) {
    send( ch, "Mobs may not do this.\r\n" );
    return FALSE;
  }
  
  if( is_apprentice( ch ) )
    return TRUE;

  if( ch->pcdata->clss != CLSS_DRUID && ch->pcdata->clss != CLSS_MAGE ) {
    send( ch, "You do not know how to do that.\r\n" );
    return FALSE;
  }

  return TRUE;
}

