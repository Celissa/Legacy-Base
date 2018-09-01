#include "system.h"


void do_kick( char_data* ch, char* argument )
{
  char_data* victim;

  if( is_confused_pet( ch ) || !is_humanoid( ch ) || is_mounted( ch ) || is_entangled( ch, "kick", true ) )
    return;

  if( ( victim = get_victim( ch, argument, "Kick who?\r\n" ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "Kicking yourself is pointless.\r\n" );
    return;
  }

  if( ch->pcdata != NULL && ch->get_skill( SKILL_KICK ) == UNLEARNT ) {
    send( ch, "You are untrained in the art of kicking.\r\n" );
    return;
  }

  if( victim->mount != NULL ) {
    send( ch, "You can't kick a mounted person.\r\n" );
    return;
  }

  if( !can_kill( ch, victim ) )
    return;

  check_killer( ch, victim );
  ch->fighting = victim;
  ch->improve_skill( SKILL_KICK );

  attack( ch, victim, "kick", NULL, roll_dice( 2,4 ), 0 );

  react_attack( ch, victim );
  set_delay(ch, 20);
}

bool opposed_roll( char_data* ch, char_data* victim, int skill, stat_type pstat, stat_type sstat )
{
  if( !ch || !victim || victim->hit < 1 )
    return false;

  int skill_level = ch->get_skill( skill );
  if( !skill_level )
    return false;

  int primary = 0;
  int secondary = 0;

  switch( pstat ) {
  case STRENGTH:
    primary = ch->Strength( ) - victim->Strength( );
    break;
  case INTELLIGENCE:
    primary = ch->Intelligence( ) - victim->Intelligence( );
    break;
  case WISDOM:
    primary = ch->Wisdom( ) - victim->Wisdom( );
    break;
  case DEXTERITY:
    primary = ch->Dexterity( ) - victim->Dexterity( );
    break;
  case CONSTITUTION:
    primary = ch->Constitution( ) - victim->Constitution( );
    break;
  }

  switch( sstat ) {
  case STRENGTH:
    secondary = ch->Strength( ) - victim->Strength( );
    break;
  case INTELLIGENCE:
    secondary = ch->Intelligence( ) - victim->Intelligence( );
    break;
  case WISDOM:
    secondary = ch->Wisdom( ) - victim->Wisdom( );
    break;
  case DEXTERITY:
    secondary = ch->Dexterity( ) - victim->Dexterity( );
    break;
  case CONSTITUTION:
    secondary = ch->Constitution( ) - victim->Constitution( );
    break;
  }

  secondary /= 2;

  int roll = number_range( ch->shdata->level / 2 + skill_level + primary + secondary, ch->shdata->level + skill_level + primary + secondary );

  // you fail if your roll is less than your victim's level
  return ( roll >= victim->shdata->level );
}


void do_charge( char_data* ch, char* argument )
{
  char_data*  victim;
  obj_data*      obj;

  if( ch->mount != NULL && 
    ch->get_skill( SKILL_MOUNTED_FIGHTING ) < MAX_SKILL_LEVEL ) {
    send( ch, "Charging while mounted is beyond your skill.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "Charge whom?\r\n" );
    return;
  }

  if( ( victim = one_character( ch, argument, "charge", ch->array ) ) == NULL )
      return;

  if( ( obj = ch->Wearing( WEAR_HELD_R ) ) == NULL ) {
    send( "You must wield a weapon to charge at someone.\r\n", ch );
    return;
  }

  if( opponent( ch ) != NULL ) {
    send( "You are already fighting someone.\r\n", ch );
    return;
  }

  if( victim == ch ) {
    send( "How can you charge against yourself?\r\n", ch );
    return;
  }

  if( !ch->get_skill( SKILL_CHARGE ) ) {
    send( "Charging is not part of your repertoire.\r\n", ch );
    return;
  }

  if( !can_kill( ch, victim ) )
    return;

  if( victim->Alert( ) ) {
    send( ch, "%s dodges your attempt to charge %s.\r\n", victim, victim->Him_Her( ) );
    send( victim, "You nimbly step out of the way of %s's charge.\r\n", ch );
    send( *ch->array, "%s nimbly steps out of the way of %s's charge.\r\n", victim, ch );
    return;
  }

  check_killer( ch, victim );
  ch->fighting = victim;
  react_attack( ch, victim );
  set_delay( ch, 20 );

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );

  if( ch->mount ) {
    send( ch, "Riding %s you charge into the fray, bearing down on %s.\r\n", ch->mount, victim );
    send( victim, "Riding %s, %s comes charging toward you!\r\n", ch->mount, ch );
    send( *ch->array, "Riding %s, %s charges toward %s.\r\n", ch->mount, ch, victim );
  } else {
    send( ch, "You charge into the fray, bearing down on %s.\r\n", victim );
    send( victim, "%s comes charging toward you!\r\n", ch );
    send( *ch->array, "%s charges toward %s.\r\n", ch, victim );
  }
  
  if( opposed_roll( ch, victim, SKILL_CHARGE, STRENGTH, INTELLIGENCE ) )
    attack( ch, victim, "+charge", obj, -1, 0 );
  else
    attack( ch, victim, "charge", obj, -1, 0 );

  return;
}


