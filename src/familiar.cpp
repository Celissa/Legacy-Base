#include "system.h"


/*
 *   Investigate Routines for Summon Reagents
 */

void do_investigate( char_data* ch, char* argument )
{
  obj_data*       obj;
  bool          found = false;
  species_data*  spec;
  char_data*      mob = NULL;

  if( ch->pcdata == NULL ) {
    send( ch, "Only players may use the investigate command.\r\n" );
    return;
  }

  if( argument == '\0' ) {
    send( ch, "What item do you wish to investigate?\r\n" );
    return;
  }

  if( ( obj = one_object( ch, argument, "investigate", &ch->contents ) ) == NULL )
    return;

  obj->selected = 1;
  obj->shown    = 1;

  switch( ch->pcdata->clss ) {
    case CLSS_PALADIN:
      for( int i = 0; i < MAX_ENTRY_SUM_DRAGON; i++ ) {
        if( summon_dragon_table[i].investigate != empty_string
          && summon_dragon_table[i].reagent[0] == obj->pIndexData->vnum 
          && is_set( &summon_dragon_table[i].alignment, ch->shdata->alignment ) 
          && ch->get_skill( SPELL_SUMMON_DRAGON ) != UNLEARNT ) {
          spec = get_species( summon_dragon_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, summon_dragon_table[i].investigate, ch, mob, obj );
        }
      }
      for( int i = 0; i < MAX_ENTRY_LESSER_MOUNT; i++ ) {
        if( lesser_mount_table[i].investigate != empty_string
          && lesser_mount_table[i].reagent[0] == obj->pIndexData->vnum 
          && is_set( &lesser_mount_table[i].alignment, ch->shdata->alignment ) 
          && ch->get_skill( SPELL_LESSER_MOUNT ) != UNLEARNT ) {
          spec = get_species( lesser_mount_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, lesser_mount_table[i].investigate, ch, mob, obj );
        }
      }
      for( int i = 0; i < MAX_ENTRY_FIND_MOUNT; i++ ) {
        if( find_mount_table[i].investigate != empty_string
          && find_mount_table[i].reagent[0] == obj->pIndexData->vnum 
          && is_set( &find_mount_table[i].alignment, ch->shdata->alignment ) 
          && ch->get_skill( SPELL_FIND_MOUNT ) != UNLEARNT ) {
          spec = get_species( find_mount_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, find_mount_table[i].investigate, ch, mob, obj );
        }
      }
      break;
                 
    case CLSS_DRUID:
      for( int i = 0; i < MAX_ENTRY_ANIMAL_COMP; i++ ) {
        if( animal_compan_table[i].investigate != empty_string
          && animal_compan_table[i].reagent[0] == obj->pIndexData->vnum 
          && animal_compan_table[i].level <= ch->shdata->level ) {
          spec = get_species( animal_compan_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, animal_compan_table[i].investigate, ch, mob, obj );
        }
      }
      break;

    case CLSS_SORCERER:
      for( int i = 0; i < MAX_ENTRY_WALK_DAMNED; i++ ) {
        if( walk_damned_table[i].investigate != empty_string
          && walk_damned_table[i].reagent[0] == obj->pIndexData->vnum 
          && ch->get_skill( SPELL_WALK_DAMNED ) != UNLEARNT ) {
          spec = get_species( walk_damned_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, walk_damned_table[i].investigate, ch, mob, obj );
        }
      }
      for( int i = 0; i < MAX_ENTRY_SUM_CADAVER; i++ ) {
        if( summon_cadaver_table[i].investigate != empty_string
          && summon_cadaver_table[i].reagent[0] == obj->pIndexData->vnum 
          && ch->get_skill( SPELL_SUMMON_CADAVER ) != UNLEARNT ) {
          spec = get_species( summon_cadaver_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, summon_cadaver_table[i].investigate, ch, mob, obj );
        }
      }
      for( int i = 0; i < MAX_ENTRY_RETURN_DEAD; i++ ) {
        if( return_dead_table[i].investigate != empty_string
          && return_dead_table[i].reagent[0] == obj->pIndexData->vnum 
          && ch->get_skill( SPELL_RETURN_THE_DEAD ) != UNLEARNT ) {
          spec = get_species( return_dead_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, return_dead_table[i].investigate, ch, mob, obj );
        }
      }
      for( int i = 0; i < MAX_ENTRY_BANE_FIEND; i++ ) {
        if( bane_table[i].investigate != empty_string
          && bane_table[i].reagent[0] == obj->pIndexData->vnum
          && ch->get_skill( SPELL_BANE_FIEND ) != UNLEARNT ) {
          spec = get_species( bane_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, bane_table[i].investigate, ch, mob, obj );
        }
      }
      for( int i = 0; i < MAX_ENTRY_SUMMON_UNDEAD; i++ ) {
        if( summon_undead_table[i].investigate != empty_string
          && summon_undead_table[i].reagent[0] == obj->pIndexData->vnum
          && ch->get_skill( SPELL_SUMMON_UNDEAD ) != UNLEARNT ) {
          spec = get_species( summon_undead_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, summon_undead_table[i].investigate, ch, mob, obj );
        }
      }
      for( int i = 0; i < MAX_ENTRY_BLOOD_FIEND; i++ ) {
        if( blood_fiend_table[i].investigate != empty_string
          && blood_fiend_table[i].reagent[0] == obj->pIndexData->vnum
          && ch->get_skill( SPELL_BLOOD_FIEND ) != UNLEARNT ) {
          spec = get_species( blood_fiend_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, blood_fiend_table[i].investigate, ch, mob, obj );
        }
      }
      break;

    case CLSS_MAGE:
      for( int i = 0; i < MAX_ENTRY_LSSR_FAMILIAR; i++ ) {
        if( lesser_familiar_table[i].investigate != empty_string
          && lesser_familiar_table[i].reagent[0] == obj->pIndexData->vnum
          && is_set( &lesser_familiar_table[i].alignment, ch->shdata->alignment )
          && ch->get_skill( SPELL_LESSER_SUMMONING ) != UNLEARNT ) {
          spec = get_species( lesser_familiar_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, lesser_familiar_table[i].investigate, ch, mob, obj );
        }
      }
      for( int i = 0; i < MAX_ENTRY_FIND_FAMILIAR; i++ ) {
        if( find_familiar_table[i].investigate != empty_string
          && find_familiar_table[i].reagent[0] == obj->pIndexData->vnum
          && is_set( &find_familiar_table[i].alignment, ch->shdata->alignment )
          && ch->get_skill( SPELL_FIND_FAMILIAR ) != UNLEARNT ) {
          spec = get_species( find_familiar_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, find_familiar_table[i].investigate, ch, mob, obj );
        }
      }
      for( int i = 0; i < MAX_ENTRY_REQUEST_ALLY; i++ ) {
        if( request_ally_table[i].investigate != empty_string
          && request_ally_table[i].reagent[0] == obj->pIndexData->vnum
          && is_set( &request_ally_table[i].alignment, ch->shdata->alignment )
          && ch->get_skill( SPELL_REQUEST_ALLY ) != UNLEARNT ) {
          spec = get_species( request_ally_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, request_ally_table[i].investigate, ch, obj, mob );
        }
      }
      break;

    case CLSS_CLERIC:
      for( int i = 0; i < MAX_ENTRY_CONSTRUCT_GOL; i++ ) {
        if( construct_golem_table[i].investigate != empty_string
          && construct_golem_table[i].reagent[0] == obj->pIndexData->vnum
          && is_set( &construct_golem_table[i].alignment, ch->shdata->alignment )
          && ch->get_skill( SPELL_CONSTRUCT_GOLEM ) != UNLEARNT ) {
          spec = get_species( construct_golem_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, construct_golem_table[i].investigate, ch, mob, obj );
        }
      }
      for( int i = 0; i < MAX_ENTRY_ANIMATE_CLAY; i++ ) {
        if( animate_clay_table[i].investigate != empty_string
          && animate_clay_table[i].reagent[0] == obj->pIndexData->vnum
          && is_set( &animate_clay_table[i].alignment, ch->shdata->alignment )
          && ch->get_skill( SPELL_ANIMATE_CLAY ) != UNLEARNT ) {
          spec = get_species( animate_clay_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, animate_clay_table[i].investigate, ch, mob, obj );
        }
      }
      break;

    case CLSS_RANGER:
      for( int i = 0; i < MAX_ENTRY_CONJ_ELEMENT; i++ ) {
        if( conjure_element_table[i].investigate != empty_string
          && conjure_element_table[i].reagent[0] == obj->pIndexData->vnum
          && is_set( &conjure_element_table[i].alignment, ch->shdata->alignment )
          && ch->get_skill( SPELL_CONJURE_ELEMENTAL ) != UNLEARNT ) {
          spec = get_species( conjure_element_table[i].mob );
          if( spec != NULL )
            mob = create_mobile( spec );
          act( ch, conjure_element_table[i].investigate, ch, mob, obj );
        }
      }
      break;
    
    default:
      act( ch, "@CWhy would you want to investigate $p?", ch, obj );
      found = true;
      return;
  }

  if( mob == NULL )
    send( ch, "You do not believe that can be used for a summons.\r\n" );
  else
    mob->Extract( );

  return;
}


/*
 *   SWITCH/RETURN
 */

void find_table_buddy( char_data*, obj_data*, int, int );

void do_switch( char_data* ch, char* argument )
{
  char_data*     victim;
  link_data*       link;
  player_data*       pc;

  pc = player( ch );

  if( is_mob( ch ) )
    return;

  if( ch->shdata->level < LEVEL_APPRENTICE ) {
    if( !can_switch( ch ) )
      return;
    else if( ( victim = pc->familiar ) == NULL ) {
      send( ch, "You don't have a familiar to switch to!\r\n" );
      return;
    }
  } else {
    if( *argument == '\0' ) {
      send( ch, "Syntax: switch <mob>\r\n" );
      return;
    }

    if( ( victim = one_character( ch, argument, "switch", ch->array, (thing_array*)&player_list, (thing_array*)&mob_list ) ) == NULL ) {
      send( ch, "They aren't here.\r\n" );
      return;
    }

    if ( player(victim) != NULL ) {
      send( ch, "You cannot switch to players.\r\n" );
      return;
    }

    if( victim->link != NULL ) {
      send( ch, "Character in use.\r\n" );
      return;
    }
  }

  link = ch->link;

  link->character  = victim;
  pc->switched     = victim;
  pc->link         = NULL;
  victim->link     = link;
  victim->pcdata   = pc->pcdata;
  victim->timer    = time(0);

  send( victim, "Ok.\r\n" );
  return;
}


void do_return( char_data* ch, char* )
{
  link_data*       link;
  player_data*   pc;

  if( not_player( ch ) )
    return;

  if( ( ch->link == NULL || ch->link->player == ch ) ) {
    send( ch, "You aren't switched.\r\n" );
    return;
  }

  link             = ch->link;
  pc               = link->player;

  if( pc->switched == NULL ) {
    send( ch, "You are shape-changed, not switched, please use shift return.\r\n" );
    return;
  }

  send( ch, "You return to your original body.\r\n" );

  link->character  = pc;
  pc->link         = ch->link; 
  ch->link         = NULL;
  ch->pcdata       = NULL;
  pc->switched     = NULL;

  return;
}


/*
 *   SUMMONING SPELLS
 */
/*

void find_familiar( char_data* ch, obj_data* obj, int level, int species_list, int obj_list )
{
  char_data*      familiar;
  player_data*      pc;
  species_data*    species;
  int                    i;

  if( obj == NULL ) {
    bug( "Find_Familiar: Null Object as reagent!?" );
    return;
  }

  if( ( pc = player( ch ) ) == NULL )
    return;

  if( ch->shdata->level < LEVEL_APPRENTICE
    && is_set( ch->pcdata->pfile->flags, PLR_FAMILIAR ) ) {
    send( ch, "Nothing happens.\r\n" );
    send( ch, "You can only summon one familiar per level.\r\n" );
    return;
  }

  if( !is_apprentice( ch ) && pc->familiar != NULL ) {
    send( ch, "Nothing happens.\r\n" );
    send( ch, "You can only have one familiar at a time.\r\n" );
    return;
  }

  for( i = 0; i < 10; i++ )
    if( obj->pIndexData->vnum == list_value[ obj_list ][ 10*(ch->shdata->alignment%3)+i ] )
      break;

  if( i == 10 ) {
    send( ch, "Nothing happens.\r\n" );
    return;
  }

  send( *ch->array, "%s disintegrates in a burst of blue flame.\r\n", obj );
  obj->Extract( 1 );

  if( number_range( 0,100 ) < 70-7*level ) {
    send( ch, "You feel the summoning fail.\r\n" );
    return;
  }

  i = list_value[ species_list ][ 10*(ch->shdata->alignment%3)+i ];

  if( ( species = get_species( i ) ) == NULL ) {
    bug( "Find_familiar: unknown species." );
    return;
  }

  familiar         = create_mobile( species );
  familiar->reset  = NULL;
  pc->familiar = familiar;

  familiar->base_hit = familiar->original_hit;
  update_maxes( familiar );

  set_bit( &familiar->status, STAT_PET );
  set_bit( &familiar->status, STAT_FAMILIAR );
  set_bit( ch->pcdata->pfile->flags, PLR_FAMILIAR );

  remove_bit( &familiar->status, STAT_AGGR_ALL );
  remove_bit( &familiar->status, STAT_AGGR_GOOD );
  remove_bit( &familiar->status, STAT_AGGR_EVIL );

  mreset_mob( (mob_data*) familiar );
  familiar->To( ch->array );

  send( ch, "%s comes to your summons, stepping from the shadows.\r\n", familiar );

  add_follower( familiar, ch );

  return;
}
*/

bool spell_lesser_summoning( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_LESSER_SUMMONING ) ) 
    return FALSE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_LESSER_FAMILIAR );

  return TRUE;
}


bool spell_find_familiar( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_FIND_FAMILIAR ) ) 
    return FALSE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_FIND_FAMILIAR );

  return TRUE;
}


bool spell_request_ally( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_REQUEST_ALLY ) ) 
    return FALSE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_REQUEST_ALLY );

  return TRUE;
}

void find_table_buddy( char_data* ch, obj_data* obj, int level, int table )
{
  char_data*        buddy;
  player_data*         pc;
  species_data*   species;
  int                   i = -1;
  int                   j = 0;
  char*               msg = empty_string;
  char*              rmsg = empty_string;

  if( obj == NULL ) {
    bug( "Find_Table_Buddy: Null Object as reagent?" );
    return;
  }

  if( ( pc = player( ch ) ) == NULL )
    return;

  if( pc->familiar != NULL ) {
    send( ch, "Nothing happens.\r\n" );
    if( ch->pcdata->clss == CLSS_MAGE )
      send( ch, "You can only have one familiar at a time.\r\n" );
    else if( ch->pcdata->clss == CLSS_CLERIC )
      send( ch, "You can only have one golem at a time.\r\n" );
    else if( ch->pcdata->clss == CLSS_RANGER )
      send( ch, "You can only have one elemental at a time.\r\n" );
    else if( ch->pcdata->clss == CLSS_SORCERER )
      send( ch, "You may only have one major undead or demon at a time.\r\n" );
    else if( ch->pcdata->clss == CLSS_PALADIN )
      send( ch, "You may only have one magical mount at a time.\r\n" );
    else if( ch->pcdata->clss == CLSS_DRUID )
      send( ch, "You may have only one animal friend at a time.\r\n" );
    else
      send( ch, "You may only have one ally at a time.(post this as a bug)\r\n" );
    return;
  }

  if( table == TABLE_ANIMAL_COMPANION ) {
    for( j = 0; j < MAX_ENTRY_ANIMAL_COMP; j++ ) {
      if( obj->pIndexData->vnum == animal_compan_table[j].reagent[0] &&
        is_set( &animal_compan_table[j].alignment, ch->shdata->alignment ) &&
        animal_compan_table[j].open && animal_compan_table[j].level <= ch->shdata->level ) {
        msg = animal_compan_table[j].arrival;;
        rmsg = animal_compan_table[j].rarrival;
        i = animal_compan_table[j].mob;
      }
    }
  }
  else if( table == TABLE_WALK_DAMNED ) {
    for( j = 0; j < MAX_ENTRY_WALK_DAMNED; j++ ) {
      if( obj->pIndexData->vnum == walk_damned_table[j].reagent[0] &&
        is_set( &walk_damned_table[j].alignment, ch->shdata->alignment ) &&
        walk_damned_table[j].open ) {
        msg = walk_damned_table[j].arrival;
        rmsg = walk_damned_table[j].rarrival;
        i = walk_damned_table[j].mob;
      }
    }
  }
  else if( table == TABLE_SUMMON_CADAVER ) {
    for( j = 0; j < MAX_ENTRY_SUM_CADAVER; j++ ) {
      if( obj->pIndexData->vnum == summon_cadaver_table[j].reagent[0] &&
        is_set( &summon_cadaver_table[j].alignment, ch->shdata->alignment ) &&
        summon_cadaver_table[j].open ) {
        msg = summon_cadaver_table[j].arrival;
        rmsg = summon_cadaver_table[j].rarrival;
        i = summon_cadaver_table[j].mob;
      }
    }
  }
  else if( table == TABLE_SUMMON_DRAGON ) {
    for( j = 0; j < MAX_ENTRY_SUM_DRAGON; j++ ) {
      if( obj->pIndexData->vnum == summon_dragon_table[j].reagent[0] &&
        is_set( &summon_dragon_table[j].alignment, ch->shdata->alignment ) &&
        summon_dragon_table[j].open ) {
        msg = summon_dragon_table[j].arrival;
        rmsg = summon_dragon_table[j].rarrival;
        i = summon_dragon_table[j].mob;
      }
    }
  }
  else if( table == TABLE_RETURN_DEAD ) {
    for( j = 0; j < MAX_ENTRY_RETURN_DEAD; j++ ) {
      if( obj->pIndexData->vnum == return_dead_table[j].reagent[0] &&
        is_set( &return_dead_table[j].alignment, ch->shdata->alignment ) &&
        return_dead_table[j].open ) {
        msg = return_dead_table[j].arrival;
        rmsg = return_dead_table[j].rarrival;
        i = return_dead_table[j].mob;
      }
    }
  }
  else if( table == TABLE_BANE_FIEND ) {
    for( j = 0; j < MAX_ENTRY_BANE_FIEND; j++ ) {
      if( obj->pIndexData->vnum == bane_table[j].reagent[0] &&
        is_set( &bane_table[j].alignment, ch->shdata->alignment ) &&
        bane_table[j].open ) {
        msg = bane_table[j].arrival;
        rmsg = bane_table[j].rarrival;
        i = bane_table[j].mob;
      }
    }
  }
  else if( table == TABLE_SUMMON_UNDEAD ) {
    for( j = 0; j < MAX_ENTRY_SUMMON_UNDEAD; j++ ) {
      if( obj->pIndexData->vnum == summon_undead_table[j].reagent[0] &&
        is_set( &summon_undead_table[j].alignment, ch->shdata->alignment ) &&
        summon_undead_table[j].open ) {
        msg = summon_undead_table[j].arrival;
        rmsg = summon_undead_table[j].rarrival;
        i = summon_undead_table[j].mob;
      }
    }
  }
  else if( table == TABLE_LESSER_FAMILIAR ) {
    for( j = 0; j < MAX_ENTRY_LSSR_FAMILIAR; j++ ) {
      if( obj->pIndexData->vnum == lesser_familiar_table[j].reagent[0] &&
        is_set( &lesser_familiar_table[j].alignment, ch->shdata->alignment ) &&
        lesser_familiar_table[j].open ) {
        msg = lesser_familiar_table[j].arrival;
        rmsg = lesser_familiar_table[j].rarrival;
        i = lesser_familiar_table[j].mob;
      }
    }
  }
  else if( table == TABLE_ANIMATE_CLAY ) {
    for( j = 0; j < MAX_ENTRY_ANIMATE_CLAY; j++ ) {
      if( obj->pIndexData->vnum == animate_clay_table[j].reagent[0] &&
        is_set( &animate_clay_table[j].alignment, ch->shdata->alignment ) &&
        animate_clay_table[j].open ) {
        msg = lesser_familiar_table[j].arrival;
        rmsg = lesser_familiar_table[j].rarrival;
        i = animate_clay_table[j].mob;
      }
    }
  }
  else if( table == TABLE_FIND_FAMILIAR ) {
    for( j = 0; j < MAX_ENTRY_FIND_FAMILIAR; j++ ) {
      if( obj->pIndexData->vnum == find_familiar_table[j].reagent[0] &&
        is_set( &find_familiar_table[j].alignment, ch->shdata->alignment ) &&
        find_familiar_table[j].open ) {
        msg = find_familiar_table[j].arrival;
        rmsg = find_familiar_table[j].rarrival;
        i = find_familiar_table[j].mob;
      }
    }
  }
  else if( table == TABLE_FIND_MOUNT ) {
    for( j = 0; j < MAX_ENTRY_FIND_MOUNT; j++ ) {
      if( obj->pIndexData->vnum == find_mount_table[j].reagent[0] &&
        is_set( &find_mount_table[j].alignment, ch->shdata->alignment ) &&
        find_mount_table[j].open ) {
        msg = find_mount_table[j].arrival;
        rmsg = find_mount_table[j].rarrival;
        i = find_mount_table[j].mob;
      }
    }
  }
  else if( table == TABLE_LESSER_MOUNT ) {
    for( j = 0; j < MAX_ENTRY_LESSER_MOUNT; j++ ) {
      if( obj->pIndexData->vnum == lesser_mount_table[j].reagent[0] &&
        is_set( &lesser_mount_table[j].alignment, ch->shdata->alignment ) &&
        lesser_mount_table[j].open ) {
        msg = lesser_mount_table[j].arrival;
        rmsg = lesser_mount_table[j].rarrival;
        i = lesser_mount_table[j].mob;
      }
    }
  }
  else if( table == TABLE_CONSTRUCT_GOLEM ) {
    for( j = 0; j < MAX_ENTRY_CONSTRUCT_GOL; j++ ) {
      if( obj->pIndexData->vnum == construct_golem_table[j].reagent[0] &&
        is_set( &construct_golem_table[j].alignment, ch->shdata->alignment ) &&
        construct_golem_table[j].open ) {
        msg = construct_golem_table[j].arrival;
        rmsg = construct_golem_table[j].rarrival;
        i = construct_golem_table[j].mob;
      }
    }
  }
  else if( table == TABLE_REQUEST_ALLY ) {
    for( j = 0; j < MAX_ENTRY_REQUEST_ALLY; j++ ) {
      if( obj->pIndexData->vnum == request_ally_table[j].reagent[0] &&
        is_set( &request_ally_table[j].alignment, ch->shdata->alignment ) &&
        request_ally_table[j].open ) {
        msg = request_ally_table[j].arrival;
        rmsg = request_ally_table[j].rarrival;
        i = request_ally_table[j].mob;
      }
    }
  }
  else if( table == TABLE_CONJURE_ELEMENTAL ) {
    for( j = 0; j < MAX_ENTRY_CONJ_ELEMENT; j++ ) {
      if( obj->pIndexData->vnum == conjure_element_table[j].reagent[0] &&
        is_set( &conjure_element_table[j].alignment, ch->shdata->alignment ) &&
        conjure_element_table[j].open ) {
        msg = conjure_element_table[j].arrival;
        rmsg = conjure_element_table[j].rarrival;
        i = conjure_element_table[j].mob;
      }
    }
  }
  else if( table == TABLE_BLOOD_FIEND ) {
    for( j = 0; j < MAX_ENTRY_BLOOD_FIEND; j++ ) {
      if( obj->pIndexData->vnum == blood_fiend_table[j].reagent[0] &&
        is_set( &blood_fiend_table[j].alignment, ch->shdata->alignment ) &&
        blood_fiend_table[j].open ) {
        msg = blood_fiend_table[j].arrival;
        rmsg = blood_fiend_table[j].rarrival;
        i = blood_fiend_table[j].mob;
      }
    }
  }
  else {
    bug( "Illegal summon table for familiar spell." );
    bug( "Table not finished: %s.", tedit_table[table-MAX_PLYR_RACE].name );
    send( ch, "Cayln didn't finish this, please notify him. (this means post a bug note)\r\n" );
    return;
  }

  if( i == 0 ) {
    bug( "Find_Table_Buddy: Has a reagent set, but no mob associated?!?" );
    bug( "Table: %s   Entry: #%i.", tedit_table[table].name, j );
    send( ch, "This is set as a reagent, but the associated creature is not set.\r\n" );
    return;
  }

  send( *ch->array, "%s disintegrates in a burst of blue flame.\r\n", obj );
  obj->Extract( 1 );

  if( i == -1 ) {
    send( "Nothing happens.\r\n", ch );
    return;
  }

  if( number_range( 0,100 ) < 70-7*level ) {
    send( ch, "You feel the summoning fail.\r\n" );
    return;
  }
  
  if( ( species = get_species( i ) ) == NULL ) {
    bug( "Find_buddy: unknown species." );
    return;
  }

  buddy = create_mobile( species );
  
  // Paladin size check for being able to mount summoned creature
  if( table == TABLE_SUMMON_DRAGON || table == TABLE_FIND_MOUNT || table == TABLE_LESSER_MOUNT ) {
    if( buddy->Size( ) < ch->Size( ) + 1 ) {
      send( ch, "%s refuses to answer your summons.\r\n", buddy );
      buddy->Extract( );
      return;
    }
  }

  pc->familiar = buddy;
  
  buddy->base_hit = buddy->original_hit;
  update_maxes( buddy );

  set_bit( &buddy->status, STAT_PET );
  set_bit( &buddy->status, STAT_FAMILIAR );
  set_bit( ch->pcdata->pfile->flags, PLR_FAMILIAR );

  remove_bit( &buddy->status, STAT_AGGR_ALL );
  remove_bit( &buddy->status, STAT_AGGR_GOOD );
  remove_bit( &buddy->status, STAT_AGGR_EVIL );

  mreset_mob( (mob_data*) buddy );

  buddy->To( ch->array );
  if( msg == empty_string )
    send( ch, "%s comes to your summons, stepping from the shadows.\r\n", buddy );
  else
    act( ch, msg, ch, buddy, NULL, NULL, VIS_ALL );
  if( rmsg != empty_string )
    act_notchar( rmsg, ch, buddy, NULL );

  add_follower( buddy, ch );

  return;
}
/*
void find_buddy( char_data* ch, obj_data* obj, int level, int species_list, int obj_list )
{
  char_data*         buddy;
  player_data*      pc;
  species_data*    species;
  int                    i;

  if( obj == NULL ) {
    bug( "Find_Buddy: Null Object as reagent!?" );
    return;
  }

  if( ( pc = player( ch ) ) == NULL )
    return;

  if( pc->familiar != NULL ) {
    send( ch, "Nothing happens.\r\n" );
    send( ch, "You can only have one ally at a time.\r\n" );
    return;
  }

  for( i = 0; i < 10; i++ )
    if( obj->pIndexData->vnum == list_value[ obj_list ][ 10*(ch->shdata->alignment%3)+i ] )
      break;

  if( i == 10 ) {
    send( ch, "Nothing happens.\r\n" );
    return;
  }

  send( *ch->array, "%s disintegrates in a burst of blue flame.\r\n", obj );
  obj->Extract( 1 );

  if( number_range( 0,100 ) < 70-7*level ) {
    send( ch, "You feel the summoning fail.\r\n" );
    return;
  }

  i = list_value[ species_list ][ 10*(ch->shdata->alignment%3)+i ];

  if( ( species = get_species( i ) ) == NULL ) {
    bug( "Find_buddy: unknown species." );
    return;
  }

  buddy = create_mobile( species );
  if( species_list == LIST_LM_SPECIES || species_list == LIST_FM_SPECIES
    || species_list == LIST_SD_SPECIES ) {
    if( buddy->Size( ) < ch->Size( ) + 1 ) {
      send( ch, "%s refuses to answer your summons.\r\n", buddy );
      buddy->Extract( );
      return;
    }
  }
  pc->familiar = buddy;
  
  buddy->base_hit = buddy->original_hit;
  update_maxes( buddy );

  set_bit( &buddy->status, STAT_PET );
  set_bit( &buddy->status, STAT_FAMILIAR );
  set_bit( ch->pcdata->pfile->flags, PLR_FAMILIAR );

  remove_bit( &buddy->status, STAT_AGGR_ALL );
  remove_bit( &buddy->status, STAT_AGGR_GOOD );
  remove_bit( &buddy->status, STAT_AGGR_EVIL );

  mreset_mob( (mob_data*) buddy );

  buddy->To( ch->array );

  send( ch, "%s comes to your summons, stepping from the shadows.\r\n", buddy );
  add_follower( buddy, ch );

  return;
}
*/

bool spell_animate_clay( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_ANIMATE_CLAY ) )
    return FALSE;
  
  find_table_buddy( ch, (obj_data*) vo, level, TABLE_ANIMATE_CLAY );

  return TRUE;
}


bool spell_construct_golem( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_CONSTRUCT_GOLEM ) ) 
    return FALSE;
  
  find_table_buddy( ch, (obj_data*) vo, level, TABLE_CONSTRUCT_GOLEM );

  return TRUE;
}


bool spell_conjure_elemental( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_CONJURE_ELEMENTAL ) ) 
    return FALSE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_CONJURE_ELEMENTAL );

  return TRUE;
}


bool spell_find_mount( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_FIND_MOUNT ) ) 
    return FALSE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_FIND_MOUNT );

  return TRUE;
}


bool spell_lesser_mount( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_LESSER_MOUNT ) ) 
    return FALSE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_LESSER_MOUNT );

  return TRUE;
}

bool spell_summon_dragon( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_SUMMON_DRAGON ) ) 
    return FALSE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_SUMMON_DRAGON );

  return TRUE;
}

bool spell_bane_fiend( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_BANE_FIEND ) )
    return TRUE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_BANE_FIEND );

  return TRUE;
}

bool spell_summon_undead( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_SUMMON_UNDEAD ) )
    return TRUE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_SUMMON_UNDEAD );

  return TRUE;
}

bool spell_summon_cadaver( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_SUMMON_CADAVER ) )
    return TRUE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_SUMMON_CADAVER );

  return TRUE;
}

bool spell_blood_fiend( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_SUMMON_CADAVER ) )
    return TRUE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_BLOOD_FIEND );

  return TRUE;
}

bool spell_return_the_dead( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_RETURN_THE_DEAD ) )
    return TRUE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_RETURN_DEAD );

  return TRUE;
}

bool spell_walk_damned( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_WALK_DAMNED ) )
    return TRUE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_WALK_DAMNED );

  return TRUE;
}

bool spell_animal_companion( char_data* ch, char_data*, void* vo, int level, int )
{
  if( null_caster( ch, SPELL_ANIMAL_COMPANION ) ) 
    return FALSE;

  find_table_buddy( ch, (obj_data*) vo, level, TABLE_ANIMAL_COMPANION );

  return TRUE;
}
