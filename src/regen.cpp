#include "system.h"


void condition_update( char_data* ch )
{
  affect_data* hunger = NULL;
  affect_data* thirst = NULL;
  int amount;

  if( !is_apprentice( ch ) ) {
    //*** needs to be soft-code modifiable at a later date, perhaps we need
    //*** a flag for affects which are either 'static' or 'pulsing' affects
    //*** static is normal affect, plusing is one that goes off every tick
    //*** (or x seconds) and re-applies the benefits/curses... means we could
    //*** also make poisons with variable speed/potency etc perhaps

   if( ch->shifted != NULL && ch->shifted->shdata->race == RACE_PLANT )
     return;

   if( is_set( ch->affected_by, AFF_HARVEST ) ) {
     for( int i = 0; ; i++ ) {
        if( i > ch->affected ) {
          bug( "Condition_Update: Harvest Affected Character with no affect." );
          remove_bit( ch->affected_by, AFF_HARVEST );
          return;
        }
        if( ch->affected[i]->type == AFF_HARVEST ) {
          hunger = ch->affected[i];
          break;
        }
      }
    }
    else if( is_set( ch->affected_by, AFF_WHEAT ) ) {
      for( int i = 0; ; i++ ) {
        if( i > ch->affected ) {
          bug( "Condition_Update: Wheat Affected Character with no affect." );
          remove_bit( ch->affected_by, AFF_HARVEST );
          return;
        }
        if( ch->affected[i]->type == AFF_WHEAT ) {
          hunger = ch->affected[i];
          break;
        }
      }
    }

   if( is_set( ch->affected_by, AFF_RIVER ) ) {
     for( int i = 0; ; i++ ) {
        if( i > ch->affected ) {
          bug( "Condition_Update: River Affected Character with no affect." );
          remove_bit( ch->affected_by, AFF_RIVER );
          return;
        }
        if( ch->affected[i]->type == AFF_RIVER ) {
          thirst = ch->affected[i];
          break;
        }
      }
    }
    else if( is_set( ch->affected_by, AFF_BROOK ) ) {
      for( int i = 0; ; i++ ) {
        if( i > ch->affected ) {
          bug( "Condition_Update: Brook Affected Character with no affect." );
          remove_bit( ch->affected_by, AFF_BROOK );
          return;
        }
        if( ch->affected[i]->type == AFF_BROOK ) {
          thirst = ch->affected[i];
          break;
        }
      }
    }

    if( is_set( ch->affected_by, AFF_WILDERNESS_LORE ) ) {
      gain_condition( ch, COND_FULL, 4 );
      gain_condition( ch, COND_THIRST, 4 );
    }

    if( is_set( ch->affected_by, AFF_WHEAT ) ) {
      gain_condition( ch, COND_FULL, (hunger->level+2)/3 );
    }
    else if( is_set( ch->affected_by, AFF_HARVEST ) ) {
      gain_condition( ch, COND_FULL, (hunger->level+6)/2 );
    }
    else {
      gain_condition( ch, COND_FULL, ch->shdata->race == RACE_TROLL ? -2 : -1 );
    }

    if( is_set( ch->affected_by, AFF_BROOK ) ) {
      gain_condition( ch, COND_THIRST, (thirst->level+2)/3 );
    } 
    else if( is_set( ch->affected_by, AFF_RIVER ) ) {
      gain_condition( ch, COND_THIRST, (thirst->level+6)/2 );
    } 
    else {
      amount = ( ch->shdata->race == RACE_LIZARD ? number_range( -1, -2 ) : -1 );
      if( ch->in_room != NULL && terrain_table[ ch->in_room->sector_type ].desert )
        amount *= 2;
      gain_condition( ch, COND_THIRST, amount );
    }
  }
  amount = ( 3+ch->pcdata->condition[COND_ALCOHOL] )/4;
  ch->pcdata->condition[COND_ALCOHOL] -= amount;
  gain_drunk( ch, amount-1 );
}


/*
 *   DRUNK ROUTINES
 */


const char* drunk_inc [] = {
  "",
  "You feel fairly intoxicated.",
  "You stumble and realize you are roaring drunk.",
  "You feel queasy and realize you drank more than you should have.",
  "You feel very ill and the world is spinning rapidly."
};

const  char* drunk_dec [] = {
  "You feel quite sober now.",
  "You feel less intoxicated.",
  "The alcohol begins to wear off, though the walls are still moving.",
  "Your stomach settles, but you doubt you can walk.",
  "The world stops spinning right and shifts to the left.",
};


void gain_drunk( char_data *ch, int amount )
{
  int*  condition  = ch->pcdata->condition;
  int   tolerance  = 10;
  int    new_cond  = max( 0, condition[ COND_DRUNK ]+amount );
  int   new_level;
  int       level;
 
  if( new_cond == condition[ COND_DRUNK ] )
    return;
   
  new_level = min( 4, new_cond/tolerance );
  level     = min( 4, condition[ COND_DRUNK ]/tolerance );

  condition[ COND_DRUNK ] = new_cond;

  if( level == new_level )
    return;
    
  send( ch, level > new_level ? drunk_dec[new_level] : drunk_inc[new_level] );
  send( ch, "\r\n" );
}


/*
 *   HUNGER/THIRST ROUTINES
 */


void gain_condition( char_data* ch, int iCond, int value )
{
  char       buf  [ MAX_INPUT_LENGTH ];
  int   old_cond;
  int   new_cond;

  if( ch->species != NULL || value == 0 )
    return;
  
  if( iCond == COND_FULL && is_set( ch->affected_by, AFF_STARVING_MADNESS ) ) {
    ch->pcdata->condition[ COND_FULL ] = -100;
    return;
  }

  if( value < 0 )
    if( number_range( 0, 2*MAX_SKILL_LEVEL ) < ch->get_skill( SKILL_ENDURANCE ) ) {
      ch->improve_skill( SKILL_ENDURANCE );
      return;
    }

  old_cond = ch->pcdata->condition[ iCond ];
  new_cond = max( -50, min( old_cond+value, 50 ) );

  ch->pcdata->condition[ iCond ] = new_cond;
    
  if( value < 0 ) {
    if( iCond == COND_FULL && new_cond < 0 && is_set( &ch->pcdata->message, MSG_HUNGER ) ) {
      sprintf( buf, "You are %s\r\n", new_cond < -20 ? ( new_cond < -30 ? "starving to death!" : "ravenous." ) : ( new_cond < -10 ? "hungry." : "mildly hungry." ) );
      send( buf, ch );
    }
    
    if( iCond == COND_THIRST && new_cond < 0 && is_set( &ch->pcdata->message, MSG_THIRST ) ) {
      sprintf( buf, "You%s\r\n", new_cond < -20 ? ( new_cond < -30 ? " are dying of thirst!" : "r throat is parched." ) : ( new_cond < -10 ? " are thirsty." : "r throat feels dry." ) );
      send( buf, ch );
    }
    
    return;
  }

  if( iCond == COND_FULL && value != 999 ) {
    if( old_cond <= 40 && new_cond > 40 )
      send( ch, "You feel full.\r\n" );
    else if( old_cond < 0 && new_cond >= 0 )
      send( ch, "You no longer feel hungry.\r\n" );
  }

  if( iCond == COND_THIRST && value != 999 ) {
    if( old_cond <= 40 && new_cond > 40 )
      send( ch, "You can drink no more.\r\n" ); 
    else if( old_cond < 0 && new_cond >= 0 )
      send( ch, "You quench your thirst.\r\n" );
  }
    
  return;
}


/*
 *   HIT/MANA/MOVE REGENERATION
 */


void regen_update( void )
{
  char_data*          ch;
  struct timeval   start;
  
  gettimeofday( &start, NULL );

  for( int i = 0; i < mob_list; i++ ) {
    ch = mob_list[i];
    if( ch->Is_Valid( ) ) {
      if( ch->position >= POS_STUNNED )
        regenerate( ch );
      if( ch->position == POS_STUNNED )
        update_pos( ch );
    }
  }
  
  for( int i = 0; i < player_list; i++ ) {
    ch = player_list[i];
    if( ch->In_Game( ) ) {
      if( ch->position >= POS_STUNNED )
        regenerate( ch );
      if( ch->position == POS_STUNNED )
        update_pos( ch );
      if( ch->pcdata->prac_timer > 0 )
        ch->pcdata->prac_timer--;
    }
  }

  pulse_time[ TIME_REGEN ] = stop_clock( start );
}


/*
 *   REGENERATE ROUTINE
 */


inline int tenth( int value )
{
  value += sign( value )*number_range( 0,9 );

  return value/10;
}


void regenerate( char_data* ch )
{
  if( ch->hit < ch->max_hit && !is_set( ch->affected_by, AFF_POISON ) || ch->Hit_Regen( ) < 0 ) {
    ch->hit += tenth( ch->Hit_Regen( ) ); //SECT_REGEN ? 5 : 0 ) );

    if( ch->species == NULL && is_set( &ch->dance, DANCE_SPRING ) && number_range( 1, 4 ) == 1 ) {
      ch->hit += 10;
      act( ch, "@RAs you dance, your body is surrounded by warm red glow.\r\n", ch );
      act_notchar( "@RAs $n dances, you see $s body surrounded by a warm red glow.\r\n", ch );
      ch->improve_skill( SKILL_SPRING );
    }
    
    if( ch->position == POS_MEDITATING && ch->mana != 0 && ch->pcdata != NULL ) {
      ch->improve_skill( SKILL_MEDITATE );
      ch->improve_skill( SKILL_TRANCE );
    }

    if( ch->hit >= ch->max_hit ) {
      ch->hit = ch->max_hit;    
      if( ch->pcdata != NULL && is_set( &ch->pcdata->message, MSG_MAX_HIT ) )
        send( ch, "You are now at max hitpoints.\r\n" );
    }
  
    update_max_move( ch );
  }

  if( ch->mana < ch->max_mana || ( ch->position == POS_MEDITATING && ch->hit < ch->max_hit ) || ch->Mana_Regen( ) < 0 ) {
    ch->mana += tenth( ch->Mana_Regen( ) );
    if( ch->species == NULL && is_set( &ch->dance, DANCE_DRAKE ) && number_range( 1, 4 ) == 1 ) {
      ch->mana += 10;
      act( ch, "@BAs you dance, your body is surrounded by pale blue light.\r\n", ch );
      act_notchar( "As $n dances, you see their body surrounded by a pale blue light.\r\n", ch );
      ch->improve_skill( SKILL_DRAKE );
    }

    if( ch->mana >= ch->max_mana ) {
      ch->mana = ch->max_mana;
      if( ch->pcdata != NULL && is_set( &ch->pcdata->message, MSG_MAX_ENERGY ) )
        send( ch, "You are now at max energy.\r\n" );
    }
  }

  if( ch->move < ch->max_move || ch->Move_Regen( ) < 0 ) {
    ch->move += tenth( ch->Move_Regen( ) );
    if( ch->move >= ch->max_move ) {
      ch->move = ch->max_move;
      if( ch->hit == ch->max_hit && ch->pcdata != NULL && ( ch->position != POS_STANDING || ch->mount != NULL ) && is_set( &ch->pcdata->message, MSG_MAX_MOVE ) )
        send( ch, "You are now at max movement.\r\n" );
    }
  }

  if( ch->move < 0 ) {
    ch->move = 0;
    if( is_dancing( ch ) ) {
      end_dance( ch );
      send( ch, "You are too exhausted to continue dancing.\r\n" );
    }
  }

  if( ch->mana < 0 ) {
    ch->mana = 0;
    if( is_singing( ch ) ) {
      send( ch, "You feel too drained to keep singing.\r\n" );
      end_song( ch );
    }
  }

  if( ch->mana < 0 ) {
    ch->mana = 0; 
    if( !is_empty( ch->leech_list ) ) {
      if( ch != ch->leech_list[0]->victim ) {
        fsend( ch, "You are unable to continue supporting the %s affect on %s.", 
          aff_char_table[ch->leech_list[0]->type].display_name == empty_string ?
          aff_char_table[ch->leech_list[0]->type].name : aff_char_table[ch->leech_list[0]->type].display_name,
          ch->leech_list[0]->victim );
        fsend( ch->leech_list[0]->victim, "The energy supply to the %s affect on you is cut.", aff_char_table[ch->leech_list[0]->type].name );
      } else {
        fsend( ch, "You are unable to support the %s affect on yourself.", aff_char_table[ch->leech_list[0]->type].display_name == empty_string ? aff_char_table[ch->leech_list[0]->type].name :aff_char_table[ch->leech_list[0]->type].display_name );
      }
      remove_leech( ch->leech_list[0] );
    }
  }

  return;
}


/*
 *   REGEN FUNCTIONS
 */


int Char_Data :: Hit_Regen( )
{
  int     full;
  int   thirst;
  int    regen     = 5+max_hit/8;
  player_data*     rch;
  thing_data*     room = (thing_data*) in_room;
  
  if( shdata->race < MAX_PLYR_RACE && shdata->race >= 0 )
    regen += plyr_race_table[ shdata->race ].hp_bonus;

  if( species == NULL && pcdata != NULL && pcdata->clss >= 0 ) {
    regen += clss_table[ pcdata->clss ].hit_bonus;

    if( ( full = pcdata->condition[ COND_FULL ] ) < 0 )
      regen = 10 * regen / ( 10 - full );

    if( ( thirst = pcdata->condition[ COND_THIRST ] ) < 0 )
      regen = 10 * regen / ( 10 - thirst );

    if( IS_DRUNK( this ) ) 
      regen += 3;

    if( position == POS_MEDITATING && mana != 0 )
      regen += regen * ( 10*get_skill( SKILL_MEDITATE )/MAX_SKILL_LEVEL
      + 10*get_skill( SKILL_TRANCE )/MAX_SKILL_LEVEL ) / 15;

  } else {
    if( is_set( species->act_flags, ACT_ZERO_REGEN ) )
      return 0;

    if( leader == NULL )
      regen *= 2;

    regen = min( regen, max_hit / 10 );
  }

  if( position == POS_SLEEPING || position == POS_MEDITATING )
    regen *= 2;
  else if( position == POS_RESTING )
    regen = 3 * regen / 2;

  if( is_set( affected_by, AFF_REGENERATION ) )
    regen = 5 * regen / 4;

  if( is_set( affected_by, AFF_POISON ) )
    regen /= 4;

  regen = regen / 2 + hit_regen;

  if( in_room )
    regen += terrain_table[ in_room->sector_type ].recovery_bonus;

  if( room != NULL ) {
    if( array != NULL ) {
      obj_data*       camp = find_vnum( in_room->contents, OBJ_CAMPFIRE );
      for( int i = 0; i < *array; i++ ) {
        if( ( rch = player( array->list[i] ) ) != NULL ) {
          if( rch->pcdata != NULL && camp != NULL && -rch->pcdata->pfile->ident == camp->Get_Integer( CAMP_SITE ) 
            && is_same_group( rch, this ) && rch->array == camp->array )
            regen += camp->Get_Integer( CAMP_SITE_REGEN );
          if( is_set( rch->affected_by, AFF_CIRCLE_HEALING ) && is_same_group( rch, this ) ) {
            for( int j = 0; j < rch->affected; j++ ) {
              if( j > rch->affected ) {
                bug( "Hit_Regen: Circle_Healing Character with no affect." );
                remove_bit( rch->affected_by, AFF_CIRCLE_HEALING );
              }
              if( rch->affected[j]->type == AFF_CIRCLE_HEALING ) {
                regen += rch->affected[j]->mmodifier[0];
                break;
              }
            }
          }
          if( is_set( rch->affected_by, AFF_DRUIDIC_RING ) && is_same_group( rch, this ) ) {
            for( int j = 0; j < rch->affected; j++ ) {
              if( j > rch->affected ) {
                bug( "Druidic Ring on Character with no affect." );
                remove_bit( rch->affected_by, AFF_DRUIDIC_RING );
              }
              if( rch->affected[j]->type == AFF_DRUIDIC_RING ) {
                regen += rch->affected[j]->mmodifier[0];
                break;
              }
            }
          }
        }
      }
    }
  }

  if( is_set( affected_by, AFF_SPRING ) )
    regen += 30;

  return regen;
}


int Char_Data :: Mana_Regen( )
{
  int     mana;
  int    regen;
  int     full;
  int   thirst;
  thing_data*   room = in_room;
  player_data*   rch;
    
  if( position == POS_MEDITATING && hit < max_hit )
    return -leech_regen( this )-60+mana_regen;

  mana  = base_mana+mod_mana+shdata->level*Intelligence( )/4;
  regen = 5+mana/12;

  if( shdata->race < MAX_PLYR_RACE )
    regen += plyr_race_table[shdata->race].mana_bonus;
  
  if( species == NULL ) {
    regen += clss_table[pcdata->clss].mana_bonus;
 
    if( ( full = pcdata->condition[ COND_FULL ] ) < 0 )
      regen = 10*regen/(10-full);

    if( ( thirst = pcdata->condition[ COND_THIRST ] ) < 0 )
      regen = 10*regen/(10-thirst);
    }

  if( position == POS_SLEEPING ) 
    regen *= 2;
  else if( position == POS_RESTING )
    regen = 3*regen/2; 

  regen += mana_regen-leech_regen( this );

  if( room != NULL ) {
    if( array != NULL ) {
      obj_data*     camp = find_vnum( in_room->contents, OBJ_CAMPFIRE );
      for( int i = 0; i < *array; i++ ) {
        if( ( rch = player( array->list[i] ) ) != NULL ) {
          if( camp != NULL && -rch->pcdata->pfile->ident == camp->Get_Integer( CAMP_SITE ) 
            && rch->array == camp->array && is_same_group( this, rch ) )
            regen += camp->Get_Integer( CAMP_SITE_REGEN );
          if( is_set( rch->affected_by, AFF_DRUIDIC_RING ) && is_same_group( rch, this )
            && rch->in_room == this->in_room ) {
            for( int j = 0; j < rch->affected; j++ ) {
              if( j > rch->affected ) {
                bug( "Druidic Ring on Character with no affect." );
                remove_bit( rch->affected_by, AFF_DRUIDIC_RING );
              }
              if( rch->affected[j]->type == AFF_DRUIDIC_RING ) {
                regen += rch->affected[j]->mmodifier[1];
                break;
              }
            }
          }
        }
      }
    }
  }
  
  if( pcdata && pcdata->clss == CLSS_DANCER && cast != NULL )
    regen -= song_leech( this );

  return regen;
}


int Char_Data :: Move_Regen( )
{
  int  move_max = max( 1, base_move + mod_move + shdata->level * ( Dexterity( ) - 12 ) / 10 );

  int    regen  = Dexterity( ) + move_max/10;  // 10+Dexterity( );
  int     full;
  int   thirst;
  player_data* rch;
  thing_data* room = in_room;
  
  if( shdata->race < MAX_PLYR_RACE ) 
    regen += plyr_race_table[shdata->race].move_bonus;

  if( species == NULL ) {
    full   = pcdata->condition[ COND_FULL ];
    thirst = pcdata->condition[ COND_THIRST ];
    regen += clss_table[pcdata->clss].move_bonus;

    if( full < 0 ) 
      regen = 10*regen/(10-full);

    if( thirst < 0 )
      regen = 10*regen/(10-thirst);

    if( IS_DRUNK( this ) )
      regen += 3;
    }

  if( room != NULL ) {
    if( array != NULL ) {
      obj_data*  camp = find_vnum( in_room->contents, OBJ_CAMPFIRE );
      for( int i = 0; i < *array; i++ ) {
        if( ( rch = player( array->list[i] ) ) != NULL ) {
          if( camp != NULL && -rch->pcdata->pfile->ident == camp->Get_Integer( CAMP_SITE )
            && is_same_group( rch, this ) && camp->array == rch->array )
            regen += camp->Get_Integer( CAMP_SITE_REGEN );
          if( is_set( rch->affected_by, AFF_DRUIDIC_RING ) && is_same_group( rch, this )
            && rch->in_room == this->in_room ) {
            for( int j = 0; j < rch->affected; j++ ) {
              if( j > rch->affected ) {
                bug( "Druidic Ring on Character with no affect." );
                remove_bit( rch->affected_by, AFF_DRUIDIC_RING );
              }
              if( rch->affected[j]->type == AFF_DRUIDIC_RING ) {
                regen += rch->affected[j]->mmodifier[2];
                break;
              }
            }
          }
        }
      }
    }
  }

  switch( position ) {
  case POS_SLEEPING:
    regen *= 2;
    break;
  
  case POS_RESTING:
    regen = 3*regen/2;
    break;
  }

  if( is_set( affected_by, AFF_POISON ) ) 
    regen /= 2;

  regen += move_regen;

  if( is_dancing( this ) )
    regen -= dance_move( this );

  return regen;
}




