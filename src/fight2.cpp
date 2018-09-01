#include "system.h"


void    disarm          ( char_data*, char_data* );


/*
 *   FIGHT SUB_ROUTINES
 */


void check_killer( char_data* ch, char_data* victim )
{
  if( ch->species != NULL || !is_enemy( victim, ch ) ) 
    return;
    
  add_enemy( victim, ch );
}


bool is_entangled( char_data* ch, const char* verb, bool message )
{
  if( !is_set( ch->affected_by, AFF_ENTANGLED ) && !is_set( ch->affected_by, AFF_ENSNARE_TRAPPED ) 
    && !is_set( ch->affected_by, AFF_GHOST_CHAINS ) && !is_set( ch->affected_by, AFF_ENSNARE )
    && !is_set( ch->affected_by, AFF_BRIARTANGLE ) && !is_set( ch->affected_by, AFF_WEB_ENTANGLE ) )
    return FALSE;

  if( message )
    send( ch, "You are too entangled to %s anything.\r\n", verb );

  return TRUE;
}


/*
 *   SUPPORT SUBROUTINES
 */


char_data* get_victim( char_data* ch, char* argument, char* msg )
{
  char_data*    victim;
  char_data*  opponent;
    
  opponent = ch->fighting;

  if( *argument == '\0' ) {
    if( opponent == NULL ) {
      if( msg != empty_string )
        send( msg, ch );
      return NULL;
    }
    return opponent;
  }

  if( ( victim = one_character(ch, argument, "fight", ch->array) ) == NULL )
     return NULL;

  if( opponent != NULL && opponent != victim ) {
    send( ch, "You are already fighting someone else.\r\n" );
    return NULL;
  }
    
  return victim;
}


/*
 *   DO_KILL FUNCTION
 */
    

void do_kill( char_data* ch, char* argument )
{
  char_data*    victim;

  if( ( victim = one_character( ch, argument, "kill", ch->array ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "Typing quit is easier.\r\n" );
    return;
  }

  if( ch->mount != NULL && ch->Wearing( WEAR_HELD_R ) == NULL ) {
    send( ch, "You can't attack without a weapon while mounted.\r\n" );
    return;
  }

  if( ch->fighting == victim ) {
    send( ch, "You are already attacking %s!\r\n", victim );
    return;
  }

  if( !can_kill( ch, victim ) )
    return;

  if( ch->fighting != victim && ch->fighting != NULL )
    send( ch, "You switch your attacks to %s.\r\n", victim );
    
// moved to attack
//  leave_camouflage(ch);
//  leave_shadows(ch);
//  strip_affect( ch, AFF_INVISIBLE );

  send_seen( ch, "%s leaps to attack %s!\r\n", ch, victim );
  check_killer( ch, victim );
  ch->fighting = victim;
  react_attack( ch, victim );

  remove_bit( &ch->status, STAT_WIMPY );

  leave_camouflage(ch);
  leave_shadows(ch);
  strip_affect( ch, AFF_INVISIBLE );
  remove_bit( ch->affected_by, AFF_INVISIBLE );

  fight_round( ch );
}


/*
 *   DISARM
 */


void do_disarm( char_data* ch, char* )
{
  char_data*    victim;
  obj_data*        obj;

  if( is_confused_pet( ch ) )
    return;

  if( ch->pcdata != NULL && ch->get_skill( SKILL_DISARM ) == UNLEARNT ) {
    send( ch, "You don't know how to disarm opponents.\r\n" );
    return;
  }

  if( ch->Wearing( WEAR_HELD_R ) == NULL ) {
    send( ch, "You must wield a weapon to disarm.\r\n" );
    return;
  }

  if( ( victim = ch->fighting ) == NULL ) {
    send( "You aren't fighting anyone.\r\n", ch );
    return;
  }

  if( ( obj = victim->Wearing( WEAR_HELD_R ) ) == NULL ) {
    send( "Your opponent is not wielding a weapon.\r\n", ch );
    return;
  }
  
  if( victim->species && is_set( victim->species->act_flags, ACT_NO_DISARM ) ) {
    send( ch, "You failed to disarm %s.\r\n", victim );
    return;
  }

  disarm( ch, victim );
  set_delay( ch, 10 );

  return;
}


void disarm( char_data* ch, char_data* victim )
{
  obj_data* obj;

  if( ( obj = victim->Wearing( WEAR_HELD_R ) ) == NULL )
    return;

  if( is_set( obj->pIndexData->extra_flags, OFLAG_NO_DISARM ) ) { 
    send(ch, "The weapon %s is wielding is impossible to disarm.\r\n", victim->Name(ch));
    return;
  }

  if( is_set( obj->extra_flags, OFLAG_NOREMOVE ) ) {
    send( ch, "%s seems to be a part of %s.\r\n", obj, victim );
    return;
  }

  int percent = number_range(0, 50) + (victim->shdata->level - ch->shdata->level) / 3;
  if( !IS_NPC(ch) && percent > 10*ch->get_skill( SKILL_DISARM )/MAX_SKILL_LEVEL ) {
    spam_char( ch, "You failed to disarm %s.\r\n", victim );
    return;
  }
 
  if( get_weapon( ch, WEAR_HELD_R ) == NULL && number_range( 0, 1 ) == 0 )
    return;

  ch->improve_skill( SKILL_DISARM );

  send(victim, "+++ %s disarms you! +++\r\n", ch);
  send(ch, "You disarm %s!\r\n", victim);
  send(*victim->array, "%s disarms %s!\r\n", ch, victim);

  thing_data* thing = obj->From(1);
  if ((obj = object(thing)) != NULL)
    obj->To(victim);

  set_delay( ch, 10 );
//    if (victim->species != NULL) {
//      obj->To(victim);
//    } else {
//      obj->To(victim->in_room);
//    }

  return;
}


/*
 *   KICK, PUNCH, BITE ROUTINES
 */


void do_punch( char_data* ch, char* argument )
{
  char_data*     victim;

  if(  is_confused_pet( ch )
    || !is_humanoid( ch )
    || is_mounted( ch )
    || is_entangled( ch, "punch", true ) ) 
    return;

  if( ( victim = get_victim( ch, argument, "Punch who?\r\n" ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "Punching yourself is pointless.\r\n" );
    return;
    }

  if( ch->pcdata != NULL && ch->get_skill( SKILL_PUNCH ) == UNLEARNT ) {
    send( ch, "You are untrained in the art of punching.\r\n" );
    return;
    }

  if( !can_kill( ch, victim ) )
    return;

  check_killer( ch, victim );
  ch->fighting = victim;
  ch->improve_skill( SKILL_PUNCH );

  attack( ch, victim, "punch", NULL, roll_dice( 1,4 ), 0 );

  react_attack( ch, victim );
  set_delay(ch, 20);

  return;
}


void do_bite( char_data* ch, char* argument )
{
  char_data*     victim;

  if(  is_confused_pet( ch )
    || is_mounted( ch )
    || is_entangled( ch, "bite", true ) ) 
    return;

  if( ( victim = get_victim( ch, argument, "Bite who?\r\n" ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "Biting yourself is pointless.\r\n" );
    return;
    }

  if(  ch->shdata->race != RACE_LIZARD
    && ch->shdata->race != RACE_TROLL
    && ch->shdata->race != RACE_GOBLIN ) {
    send( ch, "Your teeth are not long or sharp enough for biting to be an effective\r\nattack.\r\n" );
    return;
  }

  if( !can_kill( ch, victim ) )
    return;

  check_killer( ch, victim );
  react_attack( ch, victim );
  ch->fighting = victim;
  set_delay(ch, 20);

  return;
}


/*
 *   SPIN KICK ROUTINE
 */



void do_spin_kick( char_data* ch, char* )
{
  char_data*       rch;
  bool           found  = FALSE;

  if( is_confused_pet( ch ) || is_mounted( ch ) || !is_humanoid( ch ) )
    return;

  if( ch->pcdata == NULL || ch->get_skill( SKILL_SPIN_KICK ) == UNLEARNT ) {
    send( "You are untrained in that skill.\r\n", ch );
    return;
  }

  for( int i = *ch->array-1; i >=0; i-- ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL && can_kill( ch, rch, false ) && ( ( rch->fighting != NULL && is_same_group( rch->fighting, ch ) ) || rch->fighting == ch || ch->fighting == rch ) ) {
      if( !found ) {
        found = TRUE;
        send( ch, "You leap in the air, spinning rapidly.\r\n" );
        send_seen( ch, "%s leaps into the air, becoming a deadly blur.\r\n", ch );
      }
      attack( ch, rch, "spin kick", NULL, roll_dice( 5, 10+10*ch->get_skill( SKILL_SPIN_KICK )/MAX_SKILL_LEVEL ), 0 );
    }
  }

  if( !found )
    send( "You are not fighting anyone!\r\n", ch );

  if( found )
    ch->improve_skill( SKILL_SPIN_KICK );

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );

  set_delay( ch, 45-30*ch->get_skill( SKILL_SPIN_KICK )/MAX_SKILL_LEVEL );

  return;
}


void do_melee( char_data* ch, char* )
{
  char_data*       rch;
  bool           found  = FALSE;

  if( is_confused_pet( ch ) || is_mounted( ch ) || !is_humanoid( ch ) )
    return;

  if( ch->pcdata == NULL || ch->get_skill( SKILL_MELEE ) == UNLEARNT ) {
    send( ch, "You are untrained in that skill.\r\n" );
    return;
  }
  
  for( int i = *ch->array-1; i >=0; i-- ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL && can_kill( ch, rch, false ) && ( ( rch->fighting != NULL && is_same_group( rch->fighting, ch ) ) || rch->fighting == ch || ch->fighting == rch ) ) {
      if( !found ) {
        found = TRUE;
        send( ch, "You spin about, striking at all your foes!!\r\n" );
        send_seen( ch, "%s spins, striking out at all %s foes!!\r\n", ch, ch->His_Her( ) ); 
      }
      attack( ch, rch, "MELEE", NULL, roll_dice( 5, 8+10*ch->get_skill( SKILL_MELEE )/MAX_SKILL_LEVEL ), 0 );
    }
  }

  if( !found )
    send( "You are not fighting anyone!\r\n", ch );

  if( found )
    ch->improve_skill( SKILL_MELEE );

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );

  set_delay( ch, 40-20*ch->get_skill( SKILL_MELEE )/MAX_SKILL_LEVEL );

  return;
}







