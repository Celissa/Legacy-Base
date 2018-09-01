#include "system.h"


bool  trigger_attack    ( char_data*, char_data* );
void  end_berserk       ( char_data* );


/* 
 *   CAN_KILL ROUTINES
 */


bool in_sanctuary( char_data* ch, bool msg )
{
  if (!ch || !ch->array)
    return TRUE;

  room_data* room  = Room( ch->array->where ); 

  if( room != NULL && is_set( &room->room_flags, RFLAG_SAFE ) ) {
    if( msg ) 
      send( ch, "You are in a sanctuary.\r\n" );
    return TRUE;
  }

  return FALSE;
}


bool can_pkill( char_data* ch, char_data* victim, bool msg )
{
  if( !ch || !ch->array)
    return FALSE;

  room_data* room  = Room( ch->array->where ); 

  if( room != NULL ) {
    if( is_set( &room->room_flags, RFLAG_ARENA ) )
      return TRUE;

    if( msg )
      send( ch, "You can't kill players here.\r\n" );
    return FALSE; 
  }

  return FALSE;
/*
  if( victim == NULL )
    return TRUE;

  if( ch->shdata->race <= RACE_LIZARD && victim->shdata->race <= RACE_LIZARD ) {
    if( msg ) {
      const char* race = plyr_race_table[ victim->shdata->race ].name; 
      send( ch, "You would never kill a%s %s.\r\n", isvowel( *race ) ? "n" : "", race );
    }
    return FALSE;
  }

  return TRUE;
*/
}

bool can_kill( char_data* ch, char_data* victim, bool msg )
{
  program_data*   program;

  if( ch == NULL || victim == NULL )
    return TRUE;

  if( ch->fighting == victim || victim->fighting == ch )
    return TRUE;
  
  if( msg && !trigger_attack( ch, victim ) )
    return FALSE; 

  if( victim->species != NULL && ( program = victim->species->attack ) == NULL ) {
    compile( program );
    if( program->binary == NULL ) {
      if( msg ) {
        send( ch, "That mob has no attack program - please contact an immortal.\r\n" );
        bug( "No attack prog: %d.", victim->species->vnum );
      }
      return FALSE;
    }
  }

  if( ch->shdata->level >= LEVEL_APPRENTICE )
    return TRUE;

  if( in_sanctuary( ch, msg ) )
    return FALSE; 

  if( is_set( victim->affected_by, AFF_SANCTUARY ) ) {
    send( ch, "You cannot attack this particular mob.\r\n" );
    return FALSE;
  }

  if( ch->pcdata == NULL ) {
    // if it's a mob, and it's not a pet, then it's cool to fight
    if( ch->leader == NULL || !is_set( &ch->status, STAT_PET ) )
      return TRUE;

    // otherwise, check the perms of the mobs leader (player)
    ch = ch->leader;
  }

  // if its player vs player, then check pkill
  if( victim->pcdata != NULL && !can_pkill( ch, victim, msg ) )
    return FALSE;

  // if yer killing a mob, check pet flags
  if( victim->species != NULL ) {
    if( victim->leader != NULL && is_set( &victim->status, STAT_PET ) ) {
      if( ch == victim->leader ) {
        if( msg )
          fsend( ch, "You may not attack your own pet.", victim );
        return FALSE;
      }

      if( !can_pkill( ch, victim->leader, FALSE ) ) {
        if( msg ) 
          fsend( ch, "%s belongs to another player and attacking it is forbidden.\r\n", victim );
        return FALSE;
      }
    }

    if( victim->old_leader != NULL && ch->pcdata != NULL && victim->old_leader == ch->pcdata->pfile ) {
      if( msg )
        fsend( ch, "You may not attack your own pet, even if you have abandoned it.", victim );
      return FALSE;
    }
  }

#ifndef _DEBUG
  if( victim->shdata->level >= LEVEL_APPRENTICE ) {
    send( ch, "Attacking an immortal would be sure death.\r\n" );
    return FALSE;
  }
#endif

  return TRUE;
}


bool trigger_attack( char_data* ch, char_data* victim )
{
  action_data*  action;
  mprog_data*    mprog;

  if( IS_NPC( victim ) ) {
    if( victim->fighting == NULL )
    for( mprog = victim->species->mprog; mprog != NULL; mprog = mprog->next )
      if( mprog->trigger == MPROG_TRIGGER_ATTACK ) {
        var_ch = ch;
        var_mob = victim; 
        var_room = ch->in_room;
        if( !execute( mprog ) || ch->in_room == NULL
          || ch->in_room != victim->in_room )   
          return FALSE;
        }
    }

  for( action = ch->in_room->action; action != NULL; action = action->next )
    if( action->trigger == TRIGGER_ATTACK ) {
      var_ch = ch;
      var_victim = victim; 
      var_room = ch->in_room;
      if( !execute( action ) || ch->in_room == NULL
        || ch->in_room != victim->in_room )
        return FALSE;
      }

  return TRUE;
}


/*
 *   AGGRESSIVE ROUTINES
 */


bool is_aggressive( char_data* ch, char_data* victim )
{
  char_data* opponent;

  if( ch == victim || ch->position <= POS_SLEEPING )
    return FALSE;

  if( ch->species != NULL && victim->species != NULL )
    return FALSE;

  if( ( opponent = victim->fighting ) != NULL )
    if( join_fight( opponent, victim, ch ) )
      return TRUE;

  if( ch->pcdata != NULL || ( victim->in_room != NULL && is_set( &victim->in_room->room_flags, RFLAG_SAFE ) || !victim->Seen( ch ) ) )
    return FALSE;

  if( !victim->species && victim->shdata->level >= LEVEL_APPRENTICE )
    return FALSE;
/*
  int check = 3*ch->get_skill( SKILL_SHAPE_CHANGE )+victim->shdata->level-ch->shdata->level+
    (victim->Wisdom( )-ch->Intelligence( ) )/2;
*/
  if( victim->shifted != NULL && 3*ch->get_skill( SKILL_SHAPE_CHANGE )+victim->shdata->level-
    ch->shdata->level+(victim->Wisdom( )-ch->Intelligence( )/2) > number_range( 0, 100 ) )
    return FALSE;

  if( ch->shdata->race == RACE_PLANT && is_set( victim->affected_by, AFF_PROT_PLANTS ) )
    return FALSE;

  if( ch->shdata->race == RACE_UNDEAD && is_set( victim->affected_by, AFF_DEATH_MASK ) )
    return FALSE;

  if( !is_undead( ch ) && is_set( &victim->dance, DANCE_MISTY_DAWN ) )
    return FALSE;

  if( is_evil( victim ) ) {
    if( is_set( &ch->status, STAT_AGGR_EVIL ) && is_set( ch->affected_by, AFF_DETECT_EVIL ) ) 
      return TRUE;
  } else if( is_good( victim ) ) {
    if( is_set( &ch->status, STAT_AGGR_GOOD ) && is_set( ch->affected_by, AFF_DETECT_GOOD ) ) 
      return TRUE;
  } else if( is_neutral( victim ) ) {
    if( is_set( &ch->status, STAT_AGGR_NEUT ) && is_set( ch->affected_by, AFF_DETECT_EVIL ) 
      && is_set( ch->affected_by, AFF_DETECT_GOOD ) ) 
      return TRUE;
  }

  if( is_set( &ch->status, STAT_PET ) || is_set( &victim->status, STAT_PET ) )
    return FALSE;

  if( victim->species == NULL && (is_set( &ch->status, STAT_AGGR_ALL ) || is_enemy( ch, victim )))
    return TRUE;

  return FALSE;
}


bool join_fight( char_data* victim, char_data* ch, char_data* rch )
{
  if( rch == ch )
    return FALSE;

  if( rch == victim )
    return TRUE;

  if( rch->pcdata != NULL ) {
    if( is_set( rch->pcdata->pfile->flags, PLR_AUTO_ASSIST ) 
      && is_same_group( rch, victim ) && !is_same_group( rch, ch )
      && can_kill( rch, ch ) )
      return TRUE;
    return FALSE;
  }
 
  if( is_set( &rch->status, STAT_PET ) ) {
    if( rch->leader == victim && ( victim->pcdata == NULL
      || is_set( victim->pcdata->pfile->flags, PLR_PET_ASSIST ) ) )
      return TRUE;
    return FALSE;
  }

  if( rch->species != NULL && victim->species != NULL 
    && is_set( rch->species->act_flags, ACT_ASSIST_GROUP )
    && !is_set( &victim->status, STAT_PET ) ) {
    if( victim->species->nation != NATION_NONE
      && rch->species->nation == victim->species->nation )
      return TRUE;
    if( victim->species->group != GROUP_NONE
      && victim->species->group == rch->species->group )
      return TRUE;
  }

  return FALSE;
}  
 

/*
 *   START/STOP FIGHT ROUTINES
 */


void renter_combat( char_data* ch )
{ 
  char_data*  rch;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL || rch == ch )
      continue;
 
    if( is_aggressive( ch, rch ) ) 
      ch->aggressive += rch;
  }
 
  init_attack( ch );
}


void init_attack( char_data* ch, char_data* victim )
{
  event_data* event;

  if( !ch || !ch->Is_Valid( ) ) {
    bug( "Init_Attack: ch is invalid." );
    return;
  }

  if( victim && victim->Is_Valid( ) && victim->in_room == ch->in_room && victim->position > POS_DEAD )
    ch->aggressive += victim;

  if( ch->active.time != -1 )
    return;

  // check if char already has an execute leap
  for( int i = 0; i < ch->events.size; i++ ) {
    if( ch->events[ i ]->func == execute_leap )
      return;
  }

  // scan room for the unceasingly vigilent (might need a can_kill in here too, but is_aggressive looks to be fixed it)
  bool found = false;
  for( int i = 0; i < ch->in_room->contents.size; i++ ) {
    char_data* gch = character( ch->in_room->contents[ i ] );
    if( gch && is_set( gch->affected_by, AFF_UNCEASING_VIGILANCE ) && is_aggressive( ch, gch ) ) {
      if( !found ) {
        clear( ch->aggressive );
        if( ( gch->position == POS_RESTING || gch->position == POS_SLEEPING )
          && gch->mount == NULL ) {
          gch->pos_obj  = NULL;
          gch->position = POS_STANDING;
        }
        ch->aggressive += gch;
        found = true;
      }
    }
  }

  if( found && !is_empty( ch->aggressive ) ) {
    victim = ch->aggressive[ number_range( 0, ch->aggressive.size - 1 ) ];
    if( !victim || !victim->Is_Valid( ) ) {
      bug("Init_Attack: victim is invalid.");
      return;
    }
  }

  // if victim is invalid, pick a different one
  if( !victim || !victim->Is_Valid( ) || victim->in_room != ch->in_room ) {
    if( is_empty( ch->aggressive ) )
      return;

    victim = ch->aggressive[ number_range( 0, ch->aggressive.size - 1 ) ];
    if( !victim || !victim->Is_Valid( ) ) {
      bug("Init_Attack: victim is invalid.");
      return;
    }
  }

  event          = new event_data( execute_leap, ch );
  event->pointer = (void*) victim; 

  add_queue( event, modify_delay( ch, 20 ) );
}


void react_attack( char_data* ch, char_data* victim )
{
  char_data*     rch;

  if( victim->position <= POS_DEAD )
    return;

  if( victim->leader == ch )
    stop_follower( victim );

  if( ch->leader == victim )
    stop_follower( ch );

  init_attack( victim, ch );

  ch->Set_Integer( LEAP_ATTACK, 0 );
  victim->Set_Integer( LEAP_ATTACK, 0 );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL ) {
      if( join_fight( victim, ch, rch ) )
        init_attack( rch, ch );
      else if( join_fight( ch, victim, rch ) )
        init_attack( rch, victim );
    }
  }
}


void stop_fight( char_data* ch )
{
  char_data*     rch;

  if( !ch || !ch->Is_Valid( ) ) {
    roach("Stop_Fight: NULL character!");
    return;
  }

  remove_bit( &ch->status, STAT_BERSERK );
  remove_bit( &ch->status, STAT_STUDYING );

  ch->fighting = NULL;
  disrupt_spell( ch );
  clear( ch->aggressive );

  stop_combat_events( ch );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL )
      continue;

    if( rch->cast != NULL && rch->cast->target == ch && spell_table[rch->cast->spell].type != STYPE_CONTINENT ) 
      disrupt_spell( rch );

    rch->aggressive -= ch;

    if( rch->fighting == ch ) 
      rch->fighting = NULL;

    for( int j = rch->events-1; j >= 0; j-- )
      if( rch->events[j]->pointer == (void*) ch ) {
        extract( rch->events[j] );
        init_attack( rch );  // attack next in agro list
      }
  }
}



 






