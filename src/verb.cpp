#include "system.h"

bool can_climb( char_data* );
void struggle_web( char_data* );
void struggle_vines( char_data* );
void struggle_briartangle( char_data* );
void struggle_ghost_chains( char_data* );

void do_climb( char_data* ch, char* argument )
{
  action_data* action;
  bool         result = TRUE;
  room_data*     room = ch->in_room;

  if( *argument == '\0' ) {
    send( ch, "What do you want to climb?\r\n" );
    return;
    }
  
  if( ch->species != NULL && !can_climb( ch ) && !ch->can_fly( ) || is_entangled( ch, "climb", true ) )
    return;

  for( action = ch->in_room->action; action != NULL; action = action->next ) {
    if( action->trigger == TRIGGER_CLIMB && ( ( *action->target == '\0' &&
      *argument == '\0' ) || ( *argument != '\0' 
      && is_name( argument, action->target ) ) ) ) {
      var_ch = ch;
      var_room = ch->in_room;
      result = execute( action );
      if( !result || ch->in_room != room )
        return;
      if( result )
        ch->improve_skill( SKILL_CLIMB );
      return;
    }
  }

  send( ch, "Whatever that is you can't climb it.\r\n" );

  return;
}


void do_enter( char_data* ch, char* argument )
{
  /*
  exit_data *exit;

  if( *argument == '\0' ) {
    send( ch, "What do you want to enter?\r\n" );
    return;
    }
  
  if( ( exit = ch->in_room->exit[ DIR_EXTRA ] ) == NULL
    || !is_name( argument, exit->name ) ) {
    send( "Whatever that is you are unable to enter it.\r\n", ch );
    return;
    }

  move_char( ch, DIR_EXTRA, FALSE );
  */
  return;
}


void do_move( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "What do you want to move?\r\n" );
    return;
    }
 
  send( ch,
    "Whatever that is trying to move it does nothing interesting.\r\n" );

  return;
}

 
void do_pull( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "What do you want to pull?\r\n" );
    return;
    }
 
  send( ch,
    "Whatever that is pulling it does nothing interesting.\r\n" );

  return;
}


void do_push( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "What do you want to push?\r\n" );
    return;
    }
 
  send( ch,
    "Whatever that is pushing it does nothing interesting.\r\n" );

  return;
}


void do_read( char_data* ch, char* )
{
  send( "Whatever that is you can't read it.  Perhaps you should try looking at it.\r\n", ch );
  
  return;
}


void do_struggle( char_data* ch, char* argument )
{
  if( !is_entangled( ch, "" ) ) {
    send( ch, "You aren't currently entangled, so why would you want to do that?\r\n" );
    return;
  }

  if( ch->fighting != NULL ) {
    send( ch, "You are already trying as much as you can to escape.\r\n" );
    return;
  }

  if( is_set( ch->affected_by, AFF_ENTANGLED ) )
    struggle_web( ch );
  if( is_set( ch->affected_by, AFF_BRIARTANGLE ) )
    struggle_briartangle( ch );
  if( is_set( ch->affected_by, AFF_ENSNARE_TRAPPED ) )
    struggle_vines( ch );
  if( is_set( ch->affected_by, AFF_GHOST_CHAINS ) )
    struggle_ghost_chains( ch );

  if( is_set( ch->affected_by, AFF_ENTANGLED ) )
    send( ch, "You fail to break the webs.\r\n" );
  if( is_set( ch->affected_by, AFF_BRIARTANGLE ) )
    send( ch, "You fail to break out of the briars.\r\n" );
  if( is_set( ch->affected_by, AFF_ENSNARE_TRAPPED ) )
    send( ch, "You fail to break out of the vines.\r\n" );
  if( is_set( ch->affected_by, AFF_GHOST_CHAINS ) )
    send( ch, "You fail to dissolve the ghostly chains.\r\n" );

  set_delay( ch, 32 );
  return;
}

