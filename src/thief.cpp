#include "system.h"


bool can_hide( char_data*, bool );

void do_gouge( char_data* ch, char* argument )
{
  char_data* victim;
  int  delay;

  if( is_confused_pet( ch ) )
    return;
  
  if( is_mob( ch ) )
    return;

  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) {
    send( ch, "You can not gouge with parry on.\r\n" );
    return;
  }

  if( ch->get_skill( SKILL_EYE_GOUGE )  == UNLEARNT ) {
    send( ch, "You do not know how to gouge eyes.\r\n" );
    return;
  }

  if( (victim = get_victim( ch, argument, "Gouge whose eyes?\r\n" )) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "Gouging your own eyes is not very productive.\r\n" );
    return;
  }

  if( victim->species != NULL && !is_set( victim->species->act_flags, ACT_HAS_EYES ) ) {
    send( ch,  "This creature has no eyes and cannot be blinded.\r\n" );
    return;
  }

  /*
  if ( is_set( victim, AFF_BLIND ) ) {
    send( ch, "%s is already blinded!\r\n", victim );
    return;
  }
  */
 
  if( victim->fighting != ch && ch->fighting != victim ) {
    if( !can_kill( ch, victim ) )
      return;
    check_killer( ch, victim );
  }

  delay = gouge_attack( ch, victim );

  ch->fighting = victim ;
  react_attack( ch, victim );
  set_delay(ch, delay);

  remove_bit( &ch->status, STAT_LEAPING );
  remove_bit( &ch->status, STAT_WIMPY );

  ch->improve_skill( SKILL_EYE_GOUGE );
  return;
}


int gouge_attack( char_data* ch, char_data* victim, bool spam )
{
  int roll = number_range( -2*MAX_SKILL_LEVEL, 20 )
    + ch->get_skill( SKILL_EYE_GOUGE )/2
    + (ch->shdata->dexterity - victim->shdata->dexterity)/2
    + (ch->shdata->level - victim->shdata->level)/5;

  if( roll < 6 ) {
    if( !spam ) {
      send( ch, "You attempt to gouge %s but are unsuccessful.\r\n", victim );
      send( victim, "%s attempts to gouge you but is unsuccessful.\r\n", ch );
      send( *ch->array, "%s attempts to gouge %s but is unsuccessful.\r\n", ch, victim );
    }
    return 30-10*ch->get_skill( SKILL_EYE_GOUGE )/MAX_SKILL_LEVEL;
  }

  if( roll > 21 ) {
      send( ch, "You gouge %s in the eye!!\r\n", victim );
      send( victim, "%s gouges you in the eye!!\r\n", ch );
      send( *ch->array, "%s gouges %s in the eye!!\r\n", ch, victim );

    // The blindness duration is in PULSE_TICK's (approx. 50 secs)
    physical_affect( ch, victim, SPELL_BLIND, AFF_BLIND );

    disrupt_spell( victim ); 
    set_delay( victim, 20 );
    return 30-10*ch->get_skill( SKILL_EYE_GOUGE )/MAX_SKILL_LEVEL;
  }

  if( !spam ) {
    send( ch, "You attempt to gouge %s but fail.\r\n", victim );
    send( victim, "%s attempts to gouge you but fails.\r\n", ch );
    send( *ch->array, "%s attempts to gouge %s but fails.\r\n", ch, victim );
  }

  return 30-10*ch->get_skill( SKILL_EYE_GOUGE )/MAX_SKILL_LEVEL;
}


void do_disguise( char_data*, char* )
{
  return;
}


void do_garrote( char_data* ch, char* argument )
{
  int level = ch->get_skill( SKILL_GARROTE );
  obj_data* garrote = NULL;

  for( int i = 0; i < ch->contents.size; i++ ) {
    obj_data* an_object = object( ch->contents[i] );
    if ( an_object != NULL && an_object->pIndexData->item_type == ITEM_GARROTE ) {
      garrote = an_object;
      break;
    }
  }

  if( garrote == NULL ) {
    send( ch, "You don't have a garrote.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "You have no target for your garrote.\n\b");
    return;
  }
  
  if( level == UNLEARNT ) {
    send( "Garrote is not part of your repertoire\r\n", ch );
    return;
  }

  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) {
    send( ch, "You can not garrote with parry on.\r\n" );
    return;
  }

  char_data* victim;
  if( (victim = get_victim( ch, argument, "Garrote whom?\r\n" )) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "Garroting yourself is not very productive.\r\n" );
    return;
  }

  if( !can_kill( ch, victim ) )
    return;

  if( ( ch->leader == NULL || ch->leader == ch ) && ( ch->Seen( victim ) || victim->Alert( ) ) ) {
    send( ch, "%s is too wary of you for garrote to succeed.\r\n", victim );
    return;
  }

  if( ch->leader != NULL && ch->leader != ch && ch->in_room != ch->leader->in_room && ( ch->Seen( victim ) || victim->Alert( ) ) ) {
    send( ch, "%s is too wary of you for garrote to succeed.\r\n", victim );
    return;
  }

  if( ch->leader != NULL && ch->leader->fighting != victim && ch->in_room == ch->leader->in_room && ( victim->Alert( ) || ch->Seen( victim ) ) ) {
    send( ch, "Your leader must be engaged in the fight with %s to distract %s.\r\n", victim, victim->Him_Her( ) );
    return;
  }

  if( victim->shdata->race != RACE_UNDEAD && victim->shdata->race != RACE_GOLEM &&
    2*level > number_range( 0, 10*MAX_SKILL_LEVEL-3/2*(ch->shdata->level-victim->shdata->level ) ) ) {
      send( ch, "You snap %s windpipe, killing %s instantly.\r\n", victim, victim->Him_Her( ) );
      record_damage( victim, ch, victim->hit );
      death( victim, ch, "garrote" );
      return;
    }

  if( victim->shdata->race != RACE_UNDEAD && victim->shdata->race != RACE_GOLEM ) {
    physical_affect( ch, victim, SKILL_GARROTE, AFF_CHOKING );
    send( victim, "A garrote begins to tighten around your neck.\r\n" );
    send( *victim->array, "The garrote tightens around %s neck.\r\n", victim ); 
  }

  ch->fighting = victim ;
  react_attack( ch, victim );
  set_delay( ch, 20 );

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );

  set_delay( victim, 32 );
  ch->improve_skill( SKILL_GARROTE );
 
  return;
}


/* 
 *   BACKSTAB FUNCTIONS
 */


void do_backstab( char_data* ch, char* argument )
{
  char_data*  victim;
  obj_data*      obj;
  obj_data*   second;
  int          skill  = ch->get_skill( SKILL_BACKSTAB );

  if( skill == UNLEARNT ) {
    send( "Backstabbing is not part of your repertoire.\r\n", ch );
    return;
  }

  if( ch->mount != NULL ) {
    send( ch, "Backstabbing while mounted is beyond your skill.\r\n" );
    return;
  }

  if( opponent( ch ) != NULL ) {
    send( "You are already fighting someone.\r\n", ch );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "Backstab whom?\r\n" );
    return;
  }

  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) {
    send( ch, "You can not backstab with parry on.\r\n" );
    return;
  }

  if( ( victim = one_character( ch, argument, "backstab", ch->array ) ) == NULL ) 
    return;

  if( victim == ch ) {
    send( "You can't backstab yourself!\r\n", ch );
    return;
  }

  obj = get_weapon( ch, WEAR_HELD_R );
  second = get_weapon( ch, WEAR_HELD_L );

  if( obj == NULL && second == NULL ) {
    send( "You need to be wielding a weapon to backstab.\r\n", ch );
    return;
  }

  if( obj && !is_set( obj->pIndexData->extra_flags, OFLAG_BACKSTAB ) ) {
    if( second && !is_set( second->pIndexData->extra_flags, OFLAG_BACKSTAB ) ) {
      send( ch, "It isn't possible to use either of your weapons to backstab.\r\n" );
      return;
    }
    else if( second )
      obj = second;
    else {
      send( ch, "It isn't possible to use %s to backstab.\r\n", obj );
      return;
    }
  }
  /*
    primary = FALSE;;
  
  if( second && !is_set( second->pIndexData->extra_flags, OFLAG_BACKSTAB ) )
    secondary = FALSE;
  
  if( !primary && !secondary ) {;
    send( ch, "It isn't possible to use %s to backstab.\r\n", obj );
    return;
  }

  if( !primary )
    obj = second;
*/
  if( ( ch->leader == NULL || ch->leader == ch ) && ( ch->Seen( victim ) || victim->Alert( ) ) ) { //  && includes( victim->aggressive, ch ) ) {
    send( ch, "%s is too wary of you for backstab to succeed.\r\n", victim );
    return;
  }

  if( ch->leader != NULL && ch->leader != ch && ch->in_room != ch->leader->in_room && ( ch->Seen( victim ) || victim->Alert( ) ) ) {
    send( ch, "%s is too wary of you for your backstab to succeed.\r\n", victim );
    return;
  }

  if( victim->fighting != NULL && !is_same_group( victim->fighting, ch ) ) {
    send( ch, "Someone from your group must be engaged in the fight to distract the mob for your attempted backstab.\r\n" );
    return;
  }

  if( !can_kill( ch, victim ) )
    return;

  check_killer( ch, victim );
  ch->fighting = victim ;
  react_attack( ch, victim );
  set_delay(ch, 20);

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );

  if( !attack( ch, victim, "+backstab", obj, -1, 0 ) ) {
    send( ch, "Your attempted backstab misses the mark.\r\n" );
  }

  return;
}


/*
 *   STEAL ROUTINES
 */


void do_steal( char_data* ch, char* argument )
{
  char           buf  [ MAX_INPUT_LENGTH ];
  char           arg  [ MAX_INPUT_LENGTH ];
  char_data*  victim;
  obj_data*      obj;

  if( is_confused_pet( ch ) )
    return;
 
  if( is_mob( ch ) )
    return;
 
  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) {
    send( ch, "You can not steal with parry on.\r\n" );
    return;
  }

  argument = one_argument( argument, arg );

  for( ; ; ) {
    argument = one_argument( argument, buf );
    if( buf[ 0 ] == '\0' || !strcasecmp( buf, "from" ) )
      break;
    sprintf( arg+strlen( arg ), " %s", buf );
  }

  if( arg[0] == '\0' || argument[0] == '\0' ) {
    send( "Syntax: steal <object> from <character>\r\n", ch );
    return;
  }

  if( ( victim = one_character( ch, argument, "steal", ch->array ) ) == NULL ) 
    return;

  if( victim == ch ) {
    send( "That's pointless.\r\n", ch );
    return;
  }

  if( !can_kill( ch, victim ) ) {
    send( "You can't steal from them.\r\n", ch );
    return; 
  } 

  remove_bit( ch->pcdata->pfile->flags, PLR_PARRY );

  if( !ch->check_skill( SKILL_STEAL ) || number_range( 3, 70 ) < victim->Intelligence( ) ) {
    send( ch, "%s catches you stealing from %s!!\r\n", victim, victim->Him_Her( ) );
    send( victim, "You catch %s trying to steal from you.\r\n", ch );
    send( *ch->array, "%s catches %s stealing from %s.\r\n", victim, ch, victim->Him_Her( ) );

    leave_camouflage(ch);
    leave_shadows(ch);
    ch->fighting = victim ;
    react_attack( ch, victim );
    add_queue( &ch->active, number_range(5,20) );
    set_delay( ch, 32 );

    remove_bit( &ch->status, STAT_LEAPING );
    remove_bit( &ch->status, STAT_WIMPY );
    modify_reputation( ch, victim, REP_STOLE_FROM );
    if( victim->pShop != NULL ) {
      sprintf( buf, "Guards! %s is a thief!!", ch->Name( victim ) );
      buf[8] = toupper(buf[8]); // capitalize thief's appearance or name
      do_yell( victim, buf );
      summon_help( victim, ch );
    }
    return;
  }

  if( ( obj = one_object( victim, arg, "steal", &victim->contents ) ) == NULL ) {
    send( "You can't find it.\r\n", ch );
    return;
  }
    
  if( !obj->droppable() ) {
    send( ch, "You can't pry it away.\r\n" );
    return;
  }

  if( ch->contents.number >= ch->can_carry_n() ) {
    send( "You have your hands full.\r\n", ch );
    return;
  }

  if( obj->Weight() > ch->Capacity() ) {
    send( "You can't carry that much weight.\r\n", ch );
    return;
  }

  thing_data* stolen_thing = obj->From( 1 );
  obj_data* stolen_obj = object(stolen_thing);
  if( stolen_obj != NULL ) {
    if( obj->pIndexData->item_type == ITEM_MONEY )
      send( ch, "You succeeded in stealing a coin.\r\n" );
    else
      send( ch, "You succeeded in stealing %s.\r\n", obj ); 
    stolen_obj->To( &ch->contents );
//     send( ch, "You succeeded in stealing %s.\r\n", obj );
    ch->improve_skill( SKILL_STEAL );
  }

  return;
}


void do_heist( char_data* ch, char* argument )
{
/*
  char           buf  [ MAX_INPUT_LENGTH ];
  char           arg  [ MAX_INPUT_LENGTH ];
  char_data*  victim;
  obj_data*      obj;

  if( is_confused_pet( ch ) )
    return;
 
  if( is_mob( ch ) )
    return;
 
  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) {
    send( ch, "You can not steal with parry on.\r\n" );
    return;
    }

  argument = one_argument( argument, arg );

  for( ; ; ) {
    argument = one_argument( argument, buf );
    if( buf[ 0 ] == '\0' || !strcasecmp( buf, "from" ) )
      break;
    sprintf( arg+strlen( arg ), " %s", buf );
    }

  if( arg[0] == '\0' || argument[0] == '\0' ) {
    send( "Syntax: steal <object> from <character>\r\n", ch );
    return;
    }

  if( ( victim = one_character( ch, argument, "steal", ch->array ) ) == NULL ) 
    return;

  if( victim == ch ) {
    send( "That's pointless.\r\n", ch );
    return;
    }

  if( !can_kill( ch, victim ) ) {
    send( "You can't steal from them.\r\n", ch );
    return; 
    } 

  remove_bit( ch->pcdata->pfile->flags, PLR_PARRY );

  if( !ch->check_skill( SKILL_STEAL )
    || number_range( 3, 35 ) < victim->Intelligence( ) ) {
    leave_camouflage(ch);
    leave_shadows(ch);
    ch->fighting = victim ;
    react_attack( ch, victim );
    add_queue( &ch->active, number_range(5,20) );
    set_delay( ch, 32 );
 
    remove_bit( &ch->status, STAT_LEAPING );
    remove_bit( &ch->status, STAT_WIMPY );
    send( victim, "%s tried to steal from you.\r\n", ch );
    send( *ch->array, "%s tried to steal from %s.\r\n", ch, victim );
    modify_reputation( ch, victim, REP_STOLE_FROM );
    if( victim->pShop != NULL ) {
      sprintf( buf, "Guards! %s is a thief.", ch->Name( victim ) );
      buf[8] = toupper(buf[8]);
      do_yell( victim, buf );
      summon_help( victim, ch );
      }
    return;
    }

  if( ( obj = one_object( victim, arg, "steal", &victim->contents ) ) 
       == NULL ) {
    send( "You can't find it.\r\n", ch );
    return;
    }
    
  if( !obj->droppable( ) ) {
    send( ch, "You can't pry it away.\r\n" );
    return;
    }

  if( ch->num_ins >= ch->can_carry_n( ) ) {
    send( "You have your hands full.\r\n", ch );
    return;
    }

  if( ch->wght_ins + obj.weight > ch->Capacity( ) ) {
    send( "You can't carry that much weight.\r\n", ch );
    return;
    }

  obj = remove( obj, 1 );
  put_obj( obj, ch );
  send( ch, "You succeeded in stealing %s.\r\n", obj );
  ch->improve_skill( SKILL_STEAL );
  return;
*/
}

 
/*
 *   SNEAK FUNCTIONS
 */


void do_sneak( char_data* ch, char* argument )
{
  if( not_player( ch ) ) {
    // Mobs with the sneak ability should be able to sneak.
    if ( ch->species != NULL && is_set( ch->species->affected_by, AFF_SNEAK ) ) {
       switch_bit( ch->affected_by, AFF_SNEAK );
    }
    return;
  }

  if( ch->get_skill( SKILL_SNEAK ) == UNLEARNT ) {
    send( ch, "Sneaking is not something you are adept at.\r\n" );
    return;
  }

  /*
  if( !can_hide( ch, TRUE ) ) {
    return;
  }
  */

  if( toggle( ch, argument, "Sneak", ch->pcdata->pfile->flags, PLR_SNEAK ) ) {
    return;
  }

  send( ch, "Usage: sneak <on|off>\r\n" );
  return;
}


/*
 *   HIDE ROUTINES
 */


bool can_hide( char_data* ch, bool msg )
{
  room_data*  room  = ch->in_room;

  if( ch->mount != NULL ) {
    if( msg ) 
      send( ch, "Hiding while mounted is impossible.\r\n" );
    return FALSE;
  }

  if( ch->fighting != NULL ) {
    if( msg )
      send( ch, "Hiding while fighting someone is difficult.\r\n" );
    return FALSE;
  }

  if ( !ch->Can_See() ) {
    if( msg )
      send( ch, "Hiding while blind is extremely difficult.\r\n" );
    return FALSE;
  }

  if( affected_flames( ch ) || is_set( ch->affected_by, AFF_FAERIE_FIRE ) ) {
    if( msg ) 
      send( ch, "Your fiery glow rather spoils that.\r\n" );
    return FALSE;
  }

  if( affected_shock( ch ) ) {
    if( msg )
      send( ch, "The blue-white sparks flashing around you prevent that.\r\n" );
    return FALSE;
  }

  if( terrain_table[ room->sector_type ].spoil_hide ) {
    if( msg )
      send( ch, "You can not hide in water.\r\n" );
    return FALSE;
  }
    
  return TRUE;
}


void do_camouflage( char_data* ch, char* )
{
  char_data* rch;

  if( not_player( ch ) )
    return;

  if( ch->get_skill( SKILL_CAMOUFLAGE ) == UNLEARNT ) {
    send( ch, "Camouflage is not something you are adept at.\r\n" );
    return;
  }

  if( leave_camouflage( ch ) )
    return;

  if( !can_hide( ch, TRUE ) )
    return;

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) )
      ch->seen_by += rch;

  send( ch, "You camouflage yourself and disappear from plain view.\r\n" );
  fsend_seen( ch, "%s tries to blend in with %s surroundings.\r\n", ch, ch->His_Her( )  );
  
  set_bit( &ch->status, STAT_CAMOUFLAGED );
  set_bit( ch->affected_by, AFF_CAMOUFLAGE );

  ch->improve_skill( SKILL_CAMOUFLAGE );

  return;
}


void do_hide( char_data* ch, char* )
{
  char_data* rch;

  if( is_entangled( ch, "hide", true ) )
    return;
/*
  if( not_player( ch ) ) {
    //Mobs with the hide ability should be able to hide.
    if( ch->species != NULL && is_set( ch->species->affected_by, AFF_HIDE ) )
      switch_bit( ch->affected_by, AFF_HIDE );
    return;
  }
*/
  if( ch->get_skill( SKILL_HIDE ) == UNLEARNT ) {
    send( ch, "Hiding is not something you are adept at.\r\n" );
    return;
  }

  if( leave_shadows( ch ) )
    return;

  if( !can_hide( ch, TRUE ) )
    return;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) )
      ch->seen_by += rch;

  send( ch, "You step into the shadows.\r\n" );
  fsend_seen( ch, "%s steps into the shadows attempting to hide.\r\n", ch );
  
  set_bit( &ch->status, STAT_HIDING );
  set_bit( ch->affected_by, AFF_HIDE );

  ch->improve_skill( SKILL_HIDE );
}



/* 
 *   DIP ROUTINE
 */


void do_dip( char_data* ch, char* argument )
{
  char              arg  [ MAX_INPUT_LENGTH ];
  obj_data*   container;
  obj_data*         obj;
  int             value;
  affect_data    affect;
  int             spell;

  if( !two_argument( argument, "into", arg ) ) {
    send( ch, "Syntax: Dip <object> [into] <object>\r\n" );
    return;
  }

  if( ( obj = one_object( ch, arg, "dip", &ch->contents ) ) == NULL ) 
    return;

  if( ( container = one_object( ch, argument, "dip into", &ch->contents, ch->array ) ) == NULL ) 
    return;

  if( container->pIndexData->item_type != ITEM_DRINK_CON && container->pIndexData->item_type != ITEM_FOUNTAIN ) {
    send( ch, "%s isn't something you can dip things into.\r\n", container );
    return;
  }

  if( container == obj ) {
    send( ch, "You can't dip %s into itself.\r\n", obj );
    return;
  }

  value               = container->value[1];
  container->value[1] = -2;

  if( value == 0 ) {
    send( ch, "%s is empty.\r\n", container );
    container->value[1] = value;
    return;
  }

  if( strip_affect( obj, AFF_BURNING ) ) {
    fsend( ch, "You extinguish %s by quickly dipping it into %s.", obj, container );
    fsend( *ch->array, "%s extinguishes %s by quickly dipping it into %s.", ch, obj, container );
    return;
  }

  if( execute_use_trigger( ch, container, OPROG_TRIGGER_USE ) )
    return;

  send( ch, "You dip %s into %s.\r\n", obj, container );
  send_seen( ch, "%s dips %s into %s.\r\n", ch, obj, container );
  container->value[1] = ( value == -1 ? -1 : max( 0, value-5 ) );

  if( ( spell = liquid_table[container->value[2]].spell ) == -1 ) 
    return;

  if( spell < SPELL_FIRST || spell >= WEAPON_FIRST ) {
    bug( "Do_dip: Liquid with non-spell skill." );
    return;
  }

  ( *spell_table[spell-SPELL_FIRST].function )( ch, NULL, obj, 10, -3 ); 

  return;
}  

void do_blackjack( char_data* ch, char* argument )
{
  obj_data* garrote = NULL;
  int level;

  if( ch->species != NULL )
    return;

  if( ch->fighting != NULL ) {
    send( ch, "You can not knock someone out right now.\r\n" );
    return;
  }

  if( ch->mount != NULL ) {
    send( ch, "You can not blacjack while mounted.\r\n" );
    return;
  }

  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) {
    send( ch, "You can not knock out someone with parry on.\r\n" );
    return;
  }

  for( int i = 0; i < ch->contents.size; i++ ) {
    obj_data* an_object = object( ch->contents[i] );
    if ( an_object != NULL && is_set( an_object->extra_flags, OFLAG_BLACKJACK ) ) {
      garrote = an_object;
      break;
    }
  }

  if( garrote == NULL ) {
    send( ch, "You don't have a blackjack.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "You have no target for you to blackjack.\r\n");
    return;
  }
  
  if( ( level = ch->get_skill( SKILL_BLACKJACK ) ) == UNLEARNT ) {
    send( "Blackjacking is not part of your repertoire\r\n", ch );
    return;
  }

  char_data* victim;
  if ( (victim = get_victim( ch, argument, "Blacjack whom?\r\n" )) == NULL )
    return;

  if ( victim == ch ) {
    send( ch, "Hitting yourself is not very productive.\r\n" );
    return;
  }

  if( victim->position <= POS_INCAP ) {
    send( ch, "%s is already on unconcious on the ground.\r\n" );
    return;
  }

  if( (ch->leader == NULL || ch->leader == ch ) && ( ch->Seen( victim ) || victim->Alert( ) ) ) {
    send( ch, "%s is too wary of you for blackjack to succeed.\r\n", victim );
    return;
  }

  if( ch->leader != NULL && ch->leader != ch && ch->in_room != ch->leader->in_room && ( ch->Seen( victim ) || victim->Alert( ) ) ) {
    send( ch, "%s is too wary of you for blackjack to succeed.\r\n", victim );
    return;
  }

  if( ch->leader != NULL && ch->leader->fighting != victim && ch->in_room == ch->leader->in_room && ( victim->Alert( ) || ch->Seen( victim ) ) ) {
    send( ch, "Your leader must be engaged in the fight with %s to distract %s.\r\n", victim, victim->Him_Her( ) );
    return;
  }

  if( !can_kill( ch, victim ) )
    return;

  ch->fighting = victim ;
  react_attack( ch, victim );
  set_delay( ch, 50-20*ch->get_skill( SKILL_BLACKJACK )/MAX_SKILL_LEVEL );

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );

  ch->improve_skill( SKILL_BLACKJACK );

  if( is_set( victim->affected_by, AFF_SLEEP_RESIST ) || victim->shdata->race == RACE_UNDEAD ) {
    send( ch, "Your attempt to blackjack %s was extremely unsuccessful.\r\n", victim );
    attack( ch, victim, "blackjack", garrote, -1, 0, ATT_PHYSICAL, ATT_BRAWL );
    return; 
  }

  if( level < number_range( 0, 3/2*MAX_SKILL_LEVEL ) ) {
    send( ch, "%s turns to attack as you attempt to knock %s out.\r\n", victim, victim->Him_Her( ) );
    attack( ch, victim, "blackjack", garrote, -1, 0, ATT_PHYSICAL, ATT_BRAWL );
    return;
  }

  if( attack( ch, victim, "blackjack", garrote, -1, 0, ATT_PHYSICAL, ATT_BRAWL ) ) {
    if( !victim || !victim->Is_Valid() )
      return;

    physical_affect( ch, victim, SKILL_BLACKJACK, AFF_SLEEP );
    //record_damage( victim, ch, 10 );
    stop_fight( victim );

    if( victim->position <= POS_SLEEPING ) 
      return;

    send( victim, "Something collides with the back of your head and you pass out!\r\n" );

    if( victim->mount != NULL ) {
      fsend( *victim->array, "%s tumbles off %s %s is riding and drops to the ground.", victim, victim->mount, victim->He_She( ) );
      victim->mount->rider = NULL;
      victim->mount = NULL;  
    } else {
      //int save_shown = victim->shown;
      //victim->shown = 1;
      send_seen( victim, "%s drops to the ground out cold.\r\n", victim );
      //victim->shown = save_shown;
    }
    
    victim->position = POS_SLEEPING;
  }

  set_delay( victim, 32 );
  return;
}


void do_hamstring( char_data* ch, char* argument )
{
  send( ch, "Hamstring is now an auto-skill.\r\n" );
}


void shin_kick( char_data* ch, char_data* victim, char letter )
{
  if( ch->species != NULL )
    return;

  if( victim->species != NULL && !is_set( victim->species->act_flags, ACT_CAN_TRIP ) )
     return;

  if( ch->get_skill( SKILL_SHIN_KICK ) == UNLEARNT )
    return;

  if( ch->get_skill( SKILL_SHIN_KICK ) < number_range( 0, 10*MAX_SKILL_LEVEL ) )
    return;

  if( ch->mount != NULL || victim->mount != NULL )
    return;

  if( ch->Get_Integer( SHIN_KICK ) != 0 ) {
    ch->Set_Integer( SHIN_KICK, 0 );
    return;
  }

  damage_element( victim, ch, roll_dice( 2, 4 ), "shin kick", ATT_PHYSICAL );

  ch->improve_skill( SKILL_SHIN_KICK );
  set_delay( ch, 20-10*ch->get_skill( SKILL_SHIN_KICK )/MAX_SKILL_LEVEL );
}

void do_create_poison( char_data* ch, char* argument )
{
  cast_data*           cast;
  int               potion = -1;

  if( !ch->get_skill( TRADE_POISON ) ) {
    send( ch, "You don't know how to craft poisons.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    bool found = false;
    page( ch, "You can craft these:\r\n" );
    int count = 0;
    for( int i = 0; i < MAX_ENTRY_POISON; i++ ) {
      if( poison_data_table[i].level <= ch->shdata->level &&
        poison_data_table[i].skill <= ch->get_skill( TRADE_POISON ) &&
        poison_data_table[i].open ) {
        if( !found )
          found = true;
        page( ch, "%20s%s", poison_data_table[i].name, ++count%3 == 0 ? "\r\n" : "" );
      }
    }
    if( count%3 != 0 )
      page( ch, "\r\n" );
    if( !found )
      page( ch, "%20s\r\n", "No poisons found." );
    return;
  }

  if( ch->shifted != NULL ) {
    send( ch, "You can't concoct things while in another form.\r\n" );
    return;
  }

  const char* arg = argument;

  if( MAX_ENTRY_POISON == 0 ) {
    bug( "Poison Table is empty?!?" );
    return;
  }

  for( int i = 0; i < MAX_ENTRY_POISON; i++ ) {
    const char* concoction = poison_data_table[i].name;
    if( exact_match( arg, concoction )
      && poison_data_table[i].skill <= ch->get_skill( TRADE_POISON )
      && poison_data_table[i].level <= ch->shdata->level 
      && poison_data_table[i].open )
      potion = i;
  }

  if( potion == -1 ) {
    send( ch, "%s can not be concocted.\r\n", argument );
    return;
  }

  cast            = new cast_data;
  cast->spell     = potion;
  cast->prepare   = FALSE;
  cast->wait      = 0;
  cast->mana      = 0;                    // this is a physical skill
  cast->cast_type = UPDATE_POISON;

  if( !has_ingredients( ch, cast ) ) {
    delete cast;
    return;
  }

  if( !has_tools( ch, cast ) ) {
    delete cast;
    return;
  }

  ch->concoct   = cast;

  send( ch, "You begin crafting %s.\r\n", poison_data_table[cast->spell].name );

  int delay = 70-10*ch->get_skill( TRADE_POISON )/MAX_SKILL_LEVEL;
  set_delay( ch, delay );

  return;
}

/*
void do_create_poison( char_data* ch, char* argument )
{
  obj_data*        obj = NULL;
  int         level, i = 0;
  obj_data*     poison = NULL;

  if( ch->species != NULL )
    return;

  if( argument == '\0' ) {
    send( ch, "What do you wish to create a poison with?\r\n" );
    return;
  }
  
  if( ( level = ch->get_skill( TRADE_POISON ) ) == UNLEARNT ) {
    send( ch, "Creating poisons would wind up with you dead.\r\n" );
    return;
  }
  
  if( ( obj = one_object( ch, argument, "create a poison with", &ch->contents ) ) == NULL )
    return;

  for( i = 0; i < 30; i++ ) 
    if( obj->pIndexData->vnum == list_value[ LIST_POISON_INGRED ][ i ] )
      break;
  
  if( i == 30 ) {
    send( ch, "You don't think you can make a poison out of %s.\r\n", obj );
    return;
  }

  send( *ch->array, "You put %s in a small bowl, mixing it with water attempting\r\n", obj );
  send( *ch->array, "to craft a poison from it.\r\n" );

  obj->Extract( 1 );
   //changed 7*level, to 15*level
  if( number_range( 0, 300 ) < 150-15*level+10*i ) {
    send( ch, "The poison you were creating bursts into flames, destroying what\r\n" );
    send( ch, "you were working on.\r\n" );
    return;
  }
  
  i = list_value[ LIST_POISON ][ i ];

  if( ( poison = create( get_obj_index( i ) ) ) == NULL ) {
    bug( "Create Poison: Unknown Poison" );
    return;
  }
  
  if( !poison || !poison->Is_Valid() ) {
    roach( "skill create poison: create poison succeeded but object not created for %s", ch );
    return;
  }
  
  poison->pIndexData->count++;
  poison->number++;

  if( poison->array == NULL ) {
    poison->To( ch );
    consolidate( poison );
  }
  
  ch->improve_skill( TRADE_POISON );

  send( *ch->array, "You carefully put the poison into a small vial for later application.\r\n" );
  return;
}

*/
void do_apply( char_data* ch, char* argument )
{
  obj_data*    poison;
  obj_data*    weapon;
  thing_data*     obj;
  char            arg  [ MAX_STRING_LENGTH ];
  int               i;

  if( ch->species != NULL )
    return;

  if( !two_argument( argument, "on", arg ) ) {
    send( ch, "Syntax: apply <poison> [on] <weapon>.\r\n" );
    return;
  }

  if( ( weapon = one_object( ch, argument, "apply on", &ch->wearing, &ch->contents ) ) == NULL )
    return;

  if( weapon->pIndexData->item_type != ITEM_WEAPON ) {
    send( ch, "You can only poison weapons.\r\n" );
    return;
  }

  if( ( poison = one_object( ch, arg, "apply on", &ch->contents ) ) == NULL )
    return;

  for( i = 0; i < MAX_ENTRY_POISON; i++ ) 
    if( poison->pIndexData->vnum == poison_data_table[i].poison[0] )
      break;
  
  if( i == MAX_ENTRY_POISON ) {
    send( ch, "%s is not a poison, how do you expect to apply it to %s.\r\n", poison, weapon );
    return;
  }

  if( is_set( weapon->extra_flags, OFLAG_POISON_COATED ) ) {
    send( ch, "%s is already poisoned, you wouldn't want to waste any would you?!?\r\n" );
    return;
  }

  send( ch, "You apply %s to %s, and it slowly spreads over it.\r\n", poison, weapon );
  send_seen( ch, "%s applies %s to %s %s.\r\n", ch, poison, ch->His_Her( ), weapon );

  set_bit( weapon->extra_flags, OFLAG_POISON_COATED );

  poison->Extract( 1 );
  
  obj = (thing_data*) weapon;
  obj->Set_Integer( VAR_POISONED, i+1 );
  obj->Set_Integer( POISON_HITS, number_range( 0, ch->get_skill( TRADE_POISON )+1 ) );

  return;
}
