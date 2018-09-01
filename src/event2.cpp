#include "system.h"

bool is_set( char_data*, char* );
void remove_bit( char_data*, char* );

/*
 *   
 */


int modify_delay( char_data* ch, int delay )
{
  int snare = is_set( ch->affected_by, AFF_ENSNARE );
  int slow  = is_set( ch->affected_by, AFF_SLOW );
  int haste = is_set( ch->affected_by, AFF_HASTE );
  int blade_haste = is_set( ch->affected_by, AFF_BLADE_HASTE );
  int druid_slow = is_set( ch->affected_by, AFF_SLOW_ENTANGLE );

  int mod = blade_haste + haste - slow - snare - druid_slow;

  return (3-mod) * delay / 3;
}

/*
  // old formula
  if( slow-haste == -1 ) {
    return 2*delay/3;
  } else if( slow-haste == 1 ) {
    return 3*delay/2;
  } 

  return delay;
*/


/*
 *        Web type struggles
 */

void struggle_entangle( char_data* ch )
{
  affect_data* affect = NULL;
  int            diff = 0;

  for( int i = ch->affected.size-1; i >= 0 ; i-- ) {
    if( ch->affected[i]->type == AFF_WEB_ENTANGLE ) {
      affect = ch->affected[i];
      diff = affect->level/2;
      break;
    }
    if( affect == NULL && i == 0 ) {
      bug( "Struggle_Vines: Ensnared Character with no affect." );
      remove_bit( ch->affected_by, AFF_WEB_ENTANGLE );
      return;
    }
  }

  if( roll_dice( 3, 4+diff ) < ch->Strength( ) )
    strip_affect( ch, AFF_WEB_ENTANGLE );
}


void struggle_vines( char_data* ch )
{
  affect_data* affect = NULL;
  int            diff = 0;

  for( int i = ch->affected.size-1; i >= 0 ; i-- ) {
    if( ch->affected[i]->type == AFF_ENSNARE_TRAPPED ) {
      affect = ch->affected[i];
      diff = affect->level/2;
      break;
    }
    if( affect == NULL && i == 0 ) {
      bug( "Struggle_Vines: Ensnared Character with no affect." );
      remove_bit( ch->affected_by, AFF_ENSNARE_TRAPPED );
      return;
    }
  }

  if( roll_dice( 3, 12+diff ) < ch->Strength( ) )
    strip_affect( ch, AFF_ENSNARE_TRAPPED );
}

void struggle_briartangle( char_data* ch )
{
  affect_data* affect = NULL;
  char_data* mob = ch->fighting;
  int            diff = 0;

  for( int i = ch->affected.size-1; i >= 0 ; i-- ) {
    if( ch->affected[i]->type == AFF_BRIARTANGLE ) {
      affect = ch->affected[i];
      diff   = affect->level/2;
      break;
      }
    if( affect == NULL && i == 0 ) {
      bug( "Struggle_Briartangle: Ensnared Character with no affect." );
      remove_bit( ch->affected_by, AFF_BRIARTANGLE );
      return;
      }
    }

  if( roll_dice( 3, 10+diff ) < ch->Strength( ) )
    strip_affect( ch,  AFF_BRIARTANGLE );
  else
    damage_physical( ch, mob, spell_damage( SPELL_BRIARTANGLE, 0 ), "*The entangling briars" );
}

void struggle_web( char_data* ch )
{
  affect_data* affect = NULL;
  int            diff = 0;

  for( int i = ch->affected.size-1; i >= 0 ; i-- ) {
    if( ch->affected[i]->type == AFF_ENTANGLED ) {
      affect = ch->affected[i];
      diff   = affect->level/2;
      break;
    }
    if( affect == NULL && i == 0 ) {
      bug( "Execute_Web: Entangled Character with no affect." );
      remove_bit( ch->affected_by, AFF_ENTANGLED );
      return;
    }
  }

  if( roll_dice( 3, 7+diff ) < ch->Strength( ) )
    strip_affect( ch, AFF_ENTANGLED );
}

void struggle_ghost_chains( char_data* ch )
{
  affect_data* affect = NULL;
  int            diff = 0;

  for( int i = ch->affected.size-1; i >= 0 ; i-- ) {
    if( ch->affected[i]->type == AFF_GHOST_CHAINS ) {
      affect = ch->affected[i];
      diff   = 5*affect->level;
      break;
    }
    if( affect == NULL && i == 0 ) {
      bug( "Execute_Ghost_Chains: Snared Character with no affect." );
      remove_bit( ch->affected_by, AFF_GHOST_CHAINS );
      return;
    }

  }

  if( number_range( -105, 50+diff ) < ch->Save_Magic( ) )
    strip_affect( ch, AFF_GHOST_CHAINS );
}


void passive_action( char_data* ch )
{
  command_data*   cmd;

  if( ch->cast != NULL && ch->cast->cast_type == UPDATE_CAST ) {
    spell_update( ch );
    return;
  }

  if( ch->concoct != NULL ) {
    if( ch->concoct->cast_type == UPDATE_CONCOCT )
      concoct_update( ch );
    else if( ch->concoct->cast_type == UPDATE_POISON )
      poison_concoct_update( ch );
    else if( ch->concoct->cast_type == UPDATE_SERVICE )
      service_update( ch );
    else if( ch->concoct->cast_type == UPDATE_SCRIBE )
      scribe_update( ch );
    else if( ch->concoct->cast_type == UPDATE_WAND )
      empower_update( ch );
    else if( ch->concoct->cast_type == UPDATE_FLETCH )
      fletch_update( ch );
    else {
      bug( "%s has an unknown type of concoction.\r\n", ch );
      send( ch, "You had an unknown type of concoction set, it has been deleted.\r\n" );
      ch->concoct = NULL;
    }
    return;
  }    
    
  for( ; ; ) {
    if( ( cmd = pop( ch->cmd_queue ) ) == NULL )
      break;
    assign_bit( &ch->status, STAT_ORDERED, cmd->ordered );
    interpret( ch, cmd->string );
    remove_bit( &ch->status, STAT_ORDERED );
    delete cmd;
    if( ch->link != NULL )
      ch->link->idle = 0;
    if( ch->active.time != -1 )
      return;
  }

  check_song( ch );
/*
  if( is_set( ch->affected_by, AFF_WEB_ENTANGLE ) ) {
    struggle_entangle( ch );
    return;
  }

  if( is_set( ch->affected_by, AFF_ENTANGLED ) ) {
    struggle_web( ch );
    return;
  }
  
  if( is_set( ch->affected_by, AFF_GHOST_CHAINS ) ) {
    struggle_ghost_chains( ch );
    return;
  }

  if( is_set( ch->affected_by, AFF_BRIARTANGLE ) ) {
    struggle_briartangle( ch );
    return;
  }
*/
  if( ch->position < POS_STANDING && ( ch->fighting != NULL || !is_empty( ch->aggressive ) ) ) {
    jump_feet( ch );
    set_delay(ch, 32);
  }    
}


void execute_leap( event_data* event )
{
  char_data* ch      = character(event->owner);
  char_data* victim  = character((thing_data*) event->pointer);

  extract( event );

  if( !ch || !ch->Is_Valid( ) ) {
    bug( "Execute_Leap: Attacker of Leap is invalid." );
    return;
  }

  if( !victim || !victim->Is_Valid( ) ) {
    bug( "Execute_Leap: Victim of Leap is invalid." );
    return;
  }
  
  if( ch->position <= POS_SLEEPING ) 
    return;

  // DEBUG
  if( ch->in_room != victim->in_room ) {
    bug( "Execute_Leap: %s and %s are in different rooms.", ch, victim );
    return;
  }

//  ch->aggressive += victim;

  if( ch->fighting != NULL || ch->active.time != -1 )
    return;

  ch->shown = 1;

  bool stabbed = false;
  if( ch->pcdata && is_set( ch->pcdata->pfile->flags, PLR_AUTO_BACKSTAB ) && !is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) {
    obj_data*      obj;
    int          skill  = 10*ch->get_skill( SKILL_BACKSTAB )/MAX_SKILL_LEVEL;

    if( ch->mount == NULL && !opponent( ch ) && skill != UNLEARNT && ( obj = ch->Wearing( WEAR_HELD_R ) ) != NULL
      && is_set( obj->pIndexData->extra_flags, OFLAG_BACKSTAB ) && can_kill( ch, victim ) )
//      ( ( ch->leader == NULL || ch->leader == ch ) && victim->Alert( ) == false ) && !ch->Seen( victim ) )
    {
      if( ( ch->leader == NULL || ch->leader == ch ) && ( victim->Alert( ) || ch->Seen( victim ) ) ) {
        send( ch, "%s is to alert for you to do that while solo, or tanking.\r\n", victim );
        return;
      }
      if( victim->fighting != NULL && !is_same_group( victim->fighting, ch ) ) {
//      if( ch->leader != NULL && ch->leader->fighting != victim && ch->in_room == ch->leader->in_room && ( victim->Alert( ) || ch->Seen( victim ) ) ) {
        send( ch, "Someone in your group must be engaged in the fight with %s to distract %s.\r\n", victim, victim->Him_Her( ) );
        return;
      }
      check_killer( ch, victim );
      ch->fighting = victim ;
      react_attack( ch, victim );
      set_delay(ch, 20);

      remove_bit( &ch->status, STAT_WIMPY );
      remove_bit( &ch->status, STAT_LEAPING );

      if( !attack( ch, victim, "+backstab", obj, -1, 0 ) ) {
        send( ch, "Your attempted backstab misses the mark.\r\n" );
      }

      stabbed = true;
    }
  }

  if( !stabbed ) {
    // normal attack
    reveal( ch );
    passive_action( ch );

    if( ch->active.time != -1 )
      return; 

    leap_message( ch, victim );

    ch->fighting = victim;
    react_attack( ch, victim );

    fight_round( ch );
  }
}


void fight_round( char_data* ch )
{
  int time;

  if( ch->fighting == NULL || !ch->fighting->Is_Valid() )
    return;

  ch->fighting->shown = 1;

  time = ( ch->species != NULL ? mob_round( ch, ch->fighting ) : ch->shifted != NULL ? mob_round( ch, ch->fighting ) : player_round( ch, ch->fighting ) );

  if( !ch->Is_Valid( ) )
    return;

//  time = modify_delay( ch, time );

  if( ch->species != NULL )
    add_round( ch, time );

  if( ch->active.time == -1 ) 
    set_delay( ch, time );
}


void next_action( event_data* event )
{
  if( event == NULL ) {
    roach( "Next_Action: NULL Event!" );
    return;
  }

  char_data* ch = (char_data*) event->owner;

  if( !ch || !ch->Is_Valid() ) {
    roach( "Next_Action: Invalid owner!!");
    roach( "-- Owner = %s", (event->owner_name ? event->owner_name : "[BUG]") );
    return;
  }

  if( ch->position <= POS_SLEEPING ) 
    return;

  ch->shown = 1;

  passive_action( ch );

  if( !ch || !ch->Is_Valid( ) || ch->active.time != -1 || ( ch->fighting == NULL && is_empty( ch->aggressive ) ) )
    return;

  if( ch->fighting != NULL ) {
    fight_round( ch );
  } else {
//    init_attack( ch );
    init_attack( ch, ch->aggressive[0] );
  }
}


/*
 *   WANDER EVENT
 */


void execute_wander( event_data* event )
{
  char_data*     ch  = (char_data*) event->owner;
  exit_data*   exit = NULL;

  if( is_set( &ch->status, STAT_ALERT ) && number_range( 0, 30 ) == 0 )
    remove_bit( &ch->status, STAT_ALERT );

  if(  ch->position == POS_STANDING
    && ch->leader == NULL
    && ( exit = random_movable_exit( ch ) ) != NULL
    && !is_set( &exit->to_room->room_flags, RFLAG_NO_MOB )
    && ( !is_set( ch->species->act_flags, ACT_STAY_AREA )
    || exit->to_room->area == ch->in_room->area ) )
    move_char( ch, exit->direction, FALSE );

  if( ch->Is_Valid( ) ) 
    delay_wander( event );
}


void delay_wander( event_data* event )
{
  char_data*      ch  = (char_data*) event->owner;
  int         wander  = ch->species->wander;

  if( wander >= 200 ) {
    add_queue( event, number_range( wander/10, wander ) );
  } else {
    add_queue( event, number_range( 400, 4000 ) );
  }
}


/*
 *   DROWN EVENT
 */


void execute_drown( event_data* event )
{
  char_data* ch = (char_data*) event->owner;
  int damage;

  if( !is_set( ch->affected_by, AFF_WATER_BREATHING ) && !is_apprentice( ch )
    && ch->shdata->race != RACE_UNDEAD) {
    send( ch, "You try to breathe but just swallow water.\r\n" );
    send_seen( ch, "%s is drowning!\r\n", ch );
    damage = ch->max_hit / 12;
    if( inflict( ch, NULL, damage, "drowning" ) )
      return;
  }

  add_queue( event, number_range( 50, 75 ) );
}

