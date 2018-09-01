#include "system.h"


/*
 *   ASSIST ROUTINE
 */


void do_assist( char_data* ch, char* argument )
{
  char_data*    victim;
  char_data*    target;

  if( ( victim = one_character( ch, argument, "assist", ch->array ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "You can't assist yourself.\r\n" );
    return;
  }

  if ((target = opponent(victim)) == NULL) {
    send( ch, "%s isn't fighting anyone.\r\n", victim );
    return;
  }

  if( victim->species != NULL ) {
    send( ch, "You can only assist players.\r\n" );
    return;
  }

  if( target == ch ) {
    send( ch, "Perhaps you should let the fight finish on its own.\r\n" );
    return;
  }

  if( ch->fighting != NULL ) {
    send( ch, "You are already in battle.\r\n" );
    return;
  }

  if( !can_kill( ch, target ) )
    return;

  check_killer( ch, target );
  ch->fighting = target;
  react_attack( ch, target );

  leave_camouflage(ch);
  leave_shadows(ch);
  strip_affect( ch, AFF_INVISIBLE );
  remove_bit( ch->affected_by, AFF_INVISIBLE );

  remove_bit( &ch->status, STAT_WIMPY );

  fight_round( ch );
}


/*
 *   RESCUE ROUTINE
 */


void do_rescue( char_data* ch, char* argument )
{
  char_data*  victim;
  char_data*     rch;
  char_array    list;

  if( is_mob( ch ) )
    return;

  if( ( victim = one_character( ch, argument, "rescue", ch->array ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "What about fleeing instead?\r\n" );
    return;
  }

  if( victim->species != NULL && !is_pet( victim ) ) { 
    send( ch, "You are unable to rescue monsters.\r\n" );
    return;
  }

  if( ch->fighting == victim ) {
    send( ch, "You can't rescue someone you are attacking!\r\n" );
    return;
  }

  for( int i = 0; i < victim->in_room->contents; i++ ) 
    if( ( rch = character( victim->in_room->contents[i] ) ) != NULL && rch->fighting == victim ) {
      if( !can_kill( ch, rch ) )
        return;
      list += rch;
    }

  if( is_empty( list ) ) {
    send( ch, "No one is attacking %s.\r\n", victim );
    return;
  }

  ch->improve_skill( SKILL_RESCUE );
  
  if( !ch->check_skill( SKILL_RESCUE ) ) {
    send( ch, "You fail to rescue %s.\r\n", victim );
    if( ch->fighting == NULL )
      init_attack( ch, victim->fighting );
    set_delay( ch, 10 );
    return;
  }

  send( ch, "You rescue %s!\r\n", victim );
  send( victim, "++ %s rescues you! ++\r\n", ch );
  send( *ch->array, "%s rescues %s!\r\n", ch, victim );

/*
  if( victim->fighting != NULL ) {
    ch->fighting     = victim->fighting;
    victim->fighting = NULL;
  } else {
    ch->fighting = list[0];
  }

  if( victim->fighting != NULL )
    victim->fighting = NULL;
*/
  if( ch->fighting == NULL )
    ch->fighting = list[0];

  for( int i = 0; i < list; i++ ) {
    list[i]->fighting = ch;
    record_damage( list[i], ch, 10 );
  }

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );
  set_delay( ch, 10 );
}


/*
 *   BERSERK ROUTINE
 */


void do_berserk( char_data* ch, char* ) 
{
  if( is_mob( ch ) )
    return;

  if( ch->fighting == NULL ) {
    send( ch, "You aren't in battle.\r\n" );
    return;
  }

  if( ch->pcdata == NULL || ch->species != NULL || !ch->get_skill( SKILL_BERSERK ) ) {
    send( ch, "You are unable to incite yourself into a further rage.\r\n" );
    return;
  }

  if( is_set( &ch->status, STAT_BERSERK ) ) {
    send( ch, "You are already in a frenzy beyond control.\r\n" );
    return;
  }
  remove_bit( &ch->status, STAT_STUDYING );
  set_bit( &ch->status, STAT_BERSERK );
  ch->improve_skill( SKILL_BERSERK );
  
  send( ch, "You scream a cry of war, and attack with renewed fury!!\r\n" );
  fsend( *ch->array, "%s screams a cry of war, and attacks with renewed fury!!", ch );
}
    
/*
 *   Warrior Skills
 */

void do_hardiness( char_data* ch, char * )
{
  int level = ch->get_skill( SKILL_HARDINESS );

  if( null_caster( ch, SKILL_HARDINESS ) )
    return;

  if( level == UNLEARNT ) {
    send( ch, "You know nothing of that.\r\n" );
    return;
  }
  
  if( is_set( ch->affected_by, AFF_HARDINESS ) ) {
    send( ch, "You have already enhanced your hardiness.\r\n" );
    return;
  }

  ch->improve_skill( SKILL_HARDINESS );

  physical_affect( ch, ch, SKILL_HARDINESS, AFF_HARDINESS );

  int delay = ch->species != NULL ? 6 : 30-10*ch->get_skill( SKILL_HARDINESS )/MAX_SKILL_LEVEL;
  set_delay(ch, delay);

  return;
}  

void do_study( char_data* ch, char * )
{
  if( is_mob( ch ) )
    return;

  if( ch->fighting == NULL ) {
    send( ch, "You aren't in battle.\r\n" );
    return;
  }

  if( ch->pcdata == NULL || ch->species != NULL || !ch->get_skill( SKILL_STUDY_OPPONENT ) ) {
    send( ch, "You have no skill at studying creatures.\r\n" );
    return;
  }

  if( is_set( &ch->status, STAT_BERSERK ) ) {
    send( ch, "Why do you care what they are thinking?\r\n" );
    return;
  }

  if( is_set( &ch->status, STAT_STUDYING ) ) {
    send( ch, "You are watching as closely as you can.\r\n" );
    return;
  }

  set_bit( &ch->status, STAT_STUDYING );
  ch->improve_skill( SKILL_STUDY_OPPONENT );
  
  send( ch, "You study your opponents, and gain insight into their movements.\r\n" );
  fsend( *ch->array, "%s pauses for a moment looking carefuly at %s opponents!!", ch, ch->His_Her( ) );
}
