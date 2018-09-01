#include "system.h"

void        get_obj( char_data*, thing_array*, obj_data* ); // in get.cc

bool        can_die         ( char_data* );
void        death_cry       ( char_data* );
void        raw_kill        ( player_data* );
void        loot_corpse     ( obj_data*, char_data*, char_data* );
void        register_death  ( char_data*, char_data*, const char* );
obj_data*   make_corpse     ( char_data*, content_array* );

extern bool skin_corpse( char_data*, obj_data* );

/*
 *   HAS LIVE-SAVING?
 */ 


bool can_die( char_data* victim )
{
  obj_data*    obj;
  room_data*  room;
  room_data*  char_in_room = victim->in_room;
  char        tmp [ TWO_LINES ];

  if (!victim || !victim->array) {
    roach("can_die: NULL victim!");
    return FALSE;
  }

  if( victim->species != NULL )
    return TRUE;

  if( ( room = Room( victim->array->where ) ) != NULL && is_set( &room->room_flags, RFLAG_ARENA ) ) {
    send( *victim->array, "In a flash of light %s disappears.\r\n", victim );
    victim->From( );

    victim->hit = 1;
    remove_affect( victim );
    remove_leech( victim );
    rejuvenate( victim );

    victim->To( get_temple( victim, char_in_room ) );

    dismount( victim );
    clear_enemies( victim );
    victim->position = POS_RESTING;
    update_pos( victim );

    send_seen( victim, "%s slowly materializes.\r\nFrom %s appearance %s obviously came close to death.\r\n", victim, victim->His_Her( ), victim->He_She( ) );
    send( victim, "You wake up confused and dazed, but seem alive despite your memories.\r\n" );

    return FALSE;  
  }  

  if( victim->shdata->level >= LEVEL_APPRENTICE ) {
    fsend_seen( victim, "The body of %s slowly fades out of existence.", victim );
    fsend( *victim->array, "A swirling mist appears and %s slowly reforms.", victim );
    send( victim, "You find yourself alive again.\r\n" );
    victim->hit = victim->max_hit;
    update_max_move( victim );
    update_pos( victim );
    return FALSE;
  }   
    
  for( int i = 0 ; ; i++ ) {
    if( victim->shdata->level < 10 ) {
      fsend( *victim->array, "%s was protected from death by %s.\r\n", victim, victim->Deity( victim ) );
      fsend_seen( victim, "%s disappears from sight.\r\n", victim );
      sprintf( tmp, "%s was protected from death by the gods.", victim->real_name( ) );
      player_log( victim, tmp );
      break;
    }
    if( i >= victim->affected ) {
      for( int j = 0; ; j++ ) {
        if( j >= victim->wearing )
          return TRUE;
        obj = (obj_data*) victim->wearing[j];
        if( is_set( obj->pIndexData->affect_flags, AFF_LIFE_SAVING ) )
          break;
      }
      fsend( *victim->array, "%s that %s was wearing starts to glow a deep purple!", obj, victim );
      fsend_seen( victim, "The body of %s slowly disappears!", victim );
      sprintf( tmp, "%s was protected from death by %s", victim->real_name( ), obj->singular );
      player_log( victim, tmp );
      obj->Extract( 1 ); 
      break;
    } 
    if( victim->affected[i]->type == AFF_LIFE_SAVING && number_range( 0, 100 ) > 60 - 2 * victim->affected[i]->level ) {
      send_seen( victim, "%s disappears in an explosion of light and energy.\r\n", victim );
      sprintf( tmp, "%s was protected from death by the spell protect life.\r\n", victim->real_name( ) );
      player_log( victim, tmp );
      break;
    }
  }  

  victim->was_in_room = victim->in_room;
  victim->From( );

  remove_affect( victim );
  remove_leech( victim );

  rejuvenate( victim );

  victim->To( get_temple( victim, char_in_room ) );
  victim->was_in_room = NULL;
  dismount( victim );
  clear_enemies( victim );
  victim->position = POS_RESTING;

  send_seen( victim, "%s slowly materializes.\r\nFrom %s appearance %s obviously came close to death.\r\n", victim, victim->His_Her( ), victim->He_She( ) );
  send( victim, "You wake up confused and dazed, but seem alive despite your memories.\r\n" );

  return FALSE;
}


bool die_forever( char_data* ch )
{
  if( ch->Type( ) == MOB_DATA ) 
    return TRUE;

  return FALSE;
}


/*
 *   DEATH HANDLER
 */


void on_death( char_data* victim )
{
  // go thru the players in the room and fire an on death event
  for( int i = 0; i < *victim->array; i++ ) {
    player_data* pc = player( victim->array->list[i] );
    if( pc != NULL ) {
      if( !has_enemy( pc ) ) {
        // if no enemy left, remove states like berserk
        remove_bit( &pc->status, STAT_BERSERK );
        remove_bit( &pc->status, STAT_STUDYING );
      }
    }
  }
}


void death( char_data* victim, char_data* ch, const char* dt )
{
  char               tmp  [ TWO_LINES ];
  obj_data*       corpse;
  content_array*   where  = victim->array;
  char_data*         rch;
  bool           survive;
 
  remove_bit( &victim->status, STAT_BERSERK );
  remove_bit( &victim->status, STAT_STUDYING );
  end_song( victim );
  end_dance( victim );

  if( ch == NULL ) {
    for( int i = 0; i < *where; i++ ) {
      if( ( rch = character( where->list[i] ) ) != NULL && includes( rch->aggressive, victim ) ) {
        ch = rch;
        break;
      }
    }
  }

  if( victim == NULL ) {
    roach( "death: NULL victim!" );
    return;
  }

  stop_fight( victim );
  clear_queue( victim );

  if( !can_die( victim ) )
    return;
 
  if( ch && ch->pcdata == NULL && victim && victim->pcdata != NULL )
    ch->shdata->kills++;

  disburse_exp( victim );
  register_death( victim, ch, dt );

//  clear_queue( victim );
  death_cry( victim );

  if( survive = !die_forever( victim ) )
    raw_kill( player( victim ) );

  victim->shifted = NULL;

  corpse = make_corpse( victim, where );
  
  char_data *looter = group_looter( ch );
  loot_corpse( corpse, looter && corpse && looter->array == corpse->array ? looter : ch, victim );

  on_death( victim );

  if( survive )
    return;

  if( mob( victim ) != NULL ) {
    victim->Extract( );
    return;
  }

  sprintf( tmp, "%s's soul is taken by death.", victim->Name( ) );
  info( tmp, LEVEL_APPRENTICE, tmp, IFLAG_DEATHS, 1, victim );

  clear_screen( victim );
  reset_screen( victim );

  send( victim, "Death is surprisingly peaceful.\r\n" );
  send( victim, "Good night.\r\n" );

  purge( player( victim ) );
}


void raw_kill( player_data* victim )
{
  if( !victim )
    return;

  affect_data    affect;
 
  victim->From( );

  remove_affect( victim );
  remove_leech( victim );

  victim->position  = POS_RESTING;
  victim->hit       = max( 1, victim->hit  );
  victim->mana      = max( 1, victim->mana );
  victim->move      = max( 1, victim->move );

  affect.type      = AFF_DEATH;
  affect.duration  = number_range( 30, 70 ) * 2;
  affect.level     = 10;
  affect.leech     = NULL;

  add_affect( victim, &affect );
  delete_list( victim->prepare );
  update_maxes( victim );

  victim->To( get_death( victim ) );

  dismount( victim );
  clear_enemies( victim );

  write( victim );
  backup( victim, PLAYER_DEATH_DIR );
}


void register_death( char_data* victim, char_data* ch, const char* dt )
{
  char          tmp1  [ TWO_LINES ];
  char          tmp2  [ TWO_LINES ];
  mprog_data*  mprog;
  int            exp = 0;
  
  if( !victim || !victim->array ) {
    roach( "register_death: NULL victim!" );
    return;
  } else if ( !victim->array || !victim->array->where ) {
    roach( "register_death: victim is NOWHERE!" );
    return;
  }

  if( victim->species != NULL ) {
    char_data*   leader = victim->leader;
    if( is_pet( victim ) && leader != NULL && leader->pcdata != NULL ) {
      if( is_set( &victim->status, STAT_FAMILIAR ) && victim->leader != NULL ) {
        int familiar_death = victim->leader->Get_Integer( FAMILIAR_DEATH );
        if( familiar_death < 7 ) {
          familiar_death += 1;
          victim->leader->Set_Integer( FAMILIAR_DEATH, familiar_death );
        }
        if( familiar_death > 2 ) {
          exp = death_exp( victim->leader, ch );
          exp = (int) exp*1/( 6-( familiar_death-2 ) );
          add_exp( victim->leader, -exp, "You lose %d for your familiar dying.\r\n" );
        }
      }

      if( !is_set( &victim->status, STAT_FAMILIAR ) )
        sprintf( tmp1, "%s killed by %s at %s. (Pet)", victim->Name( ), ch == NULL ? dt : ch->Name( ), victim->array->where->Location( ) );
      else
        sprintf( tmp1, "%s killed by %s at %s. (Familiar: %d experience lost.)", victim->Name( ), ch == NULL ? dt : ch->Name( ), victim->array->where->Location( ), exp );

      if( leader != NULL && is_pet( victim ) ) {
        affect_data*    aff;
        if( is_set( leader->affected_by, AFF_COMPANIONS_STRENGTH ) ) {
          for( int i = leader->affected.size-1;i >= 0; i-- ) {
            aff = leader->affected[i];
            if( aff->type == AFF_COMPANIONS_STRENGTH ) {
              if( aff->mlocation[0] == affect_loc[ victim->species->compan_str ] &&
              aff->mmodifier[0] == victim->species->compan_amt ) {
                strip_affect( leader, AFF_COMPANIONS_STRENGTH );
              }
            }
          }
        }
      }
      player_log( victim->leader, tmp1 );
    } else if( victim->old_leader ) {
      sprintf( tmp1, "%s killed by %s at %s. (Ex-Pet)", victim->Name( ), ch == NULL ? dt : ch->Name( ), victim->array->where->Location( ) );
      player_log( victim->old_leader, tmp1 );
    }

    for( mprog = victim->species->mprog; mprog != NULL; mprog = mprog->next ) 
      if( mprog->trigger == MPROG_TRIGGER_DEATH ) {
        var_mob  = victim;
        var_ch   = ch;
        var_room = Room( victim->array->where );
        execute( mprog );
      }
    return;
  }

  sprintf( tmp1, "%s killed by %s.", victim->Name( ), ch == NULL ? dt : ch->Name( ) );
  sprintf( tmp2, "%s killed by %s at %s.", victim->Name( ), ch == NULL ? dt : ch->Name( ), victim->array->where->Location( ) );
  info( tmp1, LEVEL_APPRENTICE, tmp2, IFLAG_DEATHS, 1, victim );

  exp = death_exp( victim, ch );
  add_exp( victim, -exp, "You lose %d exp for dying.\r\n" );

  sprintf( tmp1, "Killed by %s at %s (%d xp lost).", ch == NULL ? dt : ch->real_name( ), victim->array->where->Location( ), exp );
  player_log( victim, tmp1 );
  
  if( ch != NULL && ch->pcdata != NULL ) {
    sprintf( tmp1, "Pkilled %s at %s.", victim->real_name( ), victim->array->where->Location( ) );
    player_log( ch, tmp1 );
  }
}


/* 
 *   DEATH CRY
 */


void death_message( char_data* victim )
{
  char          tmp1  [ ONE_LINE ];
  char          tmp2  [ ONE_LINE ];
  char_data*     rch;
  const char*   name;

  for( int i = 0; i < *victim->array; i++ ) {
    if( ( rch = character( victim->array->list[i] ) ) == NULL || rch->link == NULL || rch == victim || !victim->Seen( rch ) ) 
      continue;
    name = victim->Name( rch );
    send( rch, bold_red_v( rch ) );
    if( victim->species != NULL && is_set( victim->species->act_flags, ACT_SHATTERS ) ) {
      sprintf( tmp1, "  +-- %%%ds --+\r\n", strlen( name )+11 );
      sprintf( tmp2, tmp1, "" );
      sprintf( tmp1, "      %s SHATTERS!!\r\n", name );
    } else {
      sprintf( tmp1, "  +-- %%%ds --+\r\n", strlen( name )+10 );
      sprintf( tmp2, tmp1, "" );
      sprintf( tmp1, "      %s is DEAD!!\r\n", name );
    }
    tmp1[6] = toupper( tmp1[6] );
    send( rch, tmp2 );
    send( rch, tmp1 );
    send( rch, tmp2 );        
    send( rch, normal( rch ) );
  }

  send( victim, "You have been KILLED!!\r\n" );
}


void death_cry( char_data* ch )
{
  room_data*  room;
  const char*  msg;
 
  if( !ch || !ch->array || ( room = Room( ch->array->where ) ) == NULL )
    return;

  msg = ch->species ? "You hear something's death cry.\r\n"
    : "You hear someone's death cry.\r\n";

  for( int i = 0; i < room->exits; i++ ) 
    send( room->exits[i]->to_room->contents, msg );

  if( is_set( &ch->status, STAT_FAMILIAR ) && ch->leader != NULL )
    send( ch->leader, "You sense your familiars death!\r\n" );

}


/*
 *   CORPSE ROUTINES
 */


const char* fragments_msg =
  "The fragments from %s quickly melt, leaving %s %s.";   


obj_data* make_corpse( char_data* ch, content_array* where )
{
  obj_data*          corpse;
  obj_data*             obj;
  obj_clss_data*   obj_clss;
  thing_data*         thing;
  int                   exp = 0;

  /* GHOSTS */

  if( ch->species != NULL && is_set( ch->species->act_flags, ACT_GHOST ) ) {
    for( int i = ch->wearing-1; i >= 0; i-- ) {
      thing = ch->wearing[i]->From( ch->wearing[i]->number );
      thing->To( &ch->contents ); 
    }
    send_publ( ch, &ch->contents, "fades out of existence", "dropping" );
    for( int i = ch->contents.size-1; i >= 0; i-- ) {
      thing = ch->contents[i]->From( ch->contents[i]->number );
      thing->To( where );
    }
    return NULL;
  }

  /* ELEMENTALS */

  if( ch->species != NULL && is_set( ch->species->act_flags, ACT_SHATTERS ) ) {
    for( int i = ch->wearing-1; i >= 0; i-- ) {
      thing = ch->wearing[i]->From( ch->wearing[i]->number );
      thing->To( &ch->contents ); 
    }

    if( ( obj_clss = get_obj_index( ch->species->corpse ) ) != NULL ) {
      corpse = create( obj_clss );
      corpse->To( &ch->contents );
    }

    send_publ( ch, &ch->contents, "shatters", "leaving behind" );
    for( int i = ch->contents.size-1; i >= 0; i-- ) {
      thing = ch->contents[i]->From( ch->contents[i]->number );
      thing->To( where );
    }
    return NULL;
  }

  /* CREATE CORPSE */

  if( ch->species != NULL ) {
    if( is_set( &ch->status, STAT_PET ) ) {
      corpse           = create( get_obj_index( OBJ_CORPSE_PC ) );
      corpse->value[1] = ch->leader ? ch->leader->pcdata->pfile->ident : 0;
      corpse_list += corpse;
      if( is_set( &ch->status, STAT_FAMILIAR ) && ch->leader != NULL ) {
        int familiar_death = ch->leader->Get_Integer( FAMILIAR_DEATH );
        if( familiar_death > 2 ) {
          exp = death_exp( ch->leader, ch );
          exp = (int) exp*1/( 6-( familiar_death-2 ) );
          
        }
      }
    } else {
      if( ( obj_clss = get_obj_index( ch->species->corpse ) ) == NULL ) 
        return NULL;
      corpse = create( obj_clss );
      if( obj_clss->item_type == ITEM_CORPSE )
        corpse->value[1] = ch->species->vnum;
    }
  } else {
    corpse           = create( get_obj_index( OBJ_CORPSE_PC ) );
    corpse->value[3] = ch->pcdata->pfile->ident;
    corpse->value[2] = ch->shdata->level;
    exp = death_exp( ch, NULL );
    corpse_list += corpse;
    
    char_data* pet;
    int i, j;

    for( i = j = 0; i < ch->followers.size; i++ ) {
      if( is_pet( pet = ch->followers.list[i] ) ) {
        if( pet->species->vnum == MOB_SKULL_WATCH )
          pet->Extract( );
      }
    }
  }
  
  /* WEIGHT */

  if( corpse->pIndexData->item_type == ITEM_CORPSE && corpse->pIndexData->weight == 0 )
    corpse->weight = ch->Empty_Weight( );

  /* NAME CORPSE */ 

  if( !strncmp( corpse->pIndexData->singular, "corpse of", 9 ) ) {
    char* tmp = static_string( );

    if( ch->descr->name != empty_string ) {
      sprintf( tmp, "corpse of %s", ch->descr->name );
      corpse->singular = alloc_string( tmp, MEM_OBJECT );
      sprintf( tmp, "%s corpses", ch->descr->name );
      corpse->plural = alloc_string( tmp, MEM_OBJECT );
    } else {
      sprintf( tmp, "corpse of %s", ch->Name( NULL ) );
      corpse->singular = alloc_string( tmp, MEM_OBJECT );
      sprintf( tmp, "%s corpses", seperate( ch->descr->singular, TRUE ) );
      corpse->plural = alloc_string( tmp, MEM_OBJECT );
    }
  }

  /* SOUL STONE CREATION */

  if( exp != 0 && ( ch->pcdata != NULL || ( is_set( &ch->status, STAT_FAMILIAR ) && ch->leader != NULL ) ) ) {
    obj_clss_data* obj_clss = get_obj_index( OBJ_SOUL_STONE );
  
    if( obj_clss != NULL ) {
      obj_data* obj = create( obj_clss );
      if( obj != NULL ) {
        obj->value[3] = ch->pcdata ? ch->pcdata->pfile->ident : ch->leader->pcdata->pfile->ident;
        obj->value[2] = ch->pcdata ? ch->shdata->level : ch->leader->shdata->level;
        obj->value[1] = exp;
        obj->To( corpse );
      }
    } else {
      bug( "Soul Stone object does not exist.  Should be obj #%i", OBJ_SOUL_STONE );
    }
  }
    
  /* TRANSFER ITEMS TO CORPSE */

  for( int i = ch->wearing-1; i >= 0; i-- ) {
    if( ch->species != NULL || number_range( 0,10 ) == 0 ) {
      obj = (obj_data*) ch->wearing[i];
      obj->From( obj->number );
      obj->To( corpse );
    }
  }

  for( int i = ch->contents-1; i >= 0; i-- ) {
    if( ( obj = object( ch->contents[i] ) ) != NULL && ( ch->species != NULL || number_range( 0,10 ) == 0 ) ) {
      obj = (obj_data*) obj->From( obj->number );
      obj->To( corpse );
    }
  }

  corpse->To( where );

  return corpse;
}


void loot_corpse( obj_data* corpse, char_data* ch, char_data* victim )
{
  if( ch == NULL || ch->pcdata == NULL || victim->species == NULL || corpse == NULL ) 
    return;

  if( ch->position == POS_SLEEPING ) {
    send( ch, "You dream of looting %s.\r\n", corpse );
    return;
  }

  if( !corpse->Seen(ch) ) {
    send( ch, "You can't loot a corpse that you can't see.\r\n");
    return;
  }

  if( corpse != forbidden( corpse, ch ) ) {
    send( ch, "You can't steal from that corpse.\r\n" );
    return;
  }
 
  int level = level_setting( &ch->pcdata->pfile->settings, SET_AUTOLOOT );
  bool skin = is_set( ch->pcdata->pfile->flags, PLR_AUTO_SKIN ) && ( ( corpse->pIndexData->item_type == ITEM_CORPSE )
    || corpse->pIndexData->item_type == ITEM_SEARCHABLE_CORPSE );
  // bool junk = 
  int amount = 0;
  obj_data* loot_obj = NULL;
  thing_array* loot_array = NULL;
  if( is_container( corpse ) ) {  
    switch ( level ) {
    case 0: // don't care about corpse contents
      break;

    case 1: // info about corpse contents only
      send_priv( ch, &corpse->contents, "contains", corpse ); 
      break;

    case 2: // loot coins
      loot_array = new thing_array;
      for( int jj = 0 ; jj < corpse->contents.size ; jj++ ) {
        if( ( loot_obj = object( corpse->contents[ jj] ) ) != NULL && loot_obj->pIndexData->item_type == ITEM_MONEY ) {
          if( !loot_obj->Seen( ch ) )
            continue;

          *loot_array += (thing_data*) loot_obj ;
          if( loot_obj->pIndexData->item_type == ITEM_MONEY )
            amount += loot_obj->number * loot_obj->pIndexData->cost;
        }
      }
      get_obj( ch, loot_array, corpse ); // loot_array deleted by get_obj
      send_priv( ch, &corpse->contents, "contains", corpse ); 
      break;

    case 3: // loot all
      loot_array = new thing_array;
      for( int j = 0 ; j < corpse->contents.size ; j++ ) {
        if( ( loot_obj = object( corpse->contents[ j ] ) ) != NULL) {
          if( !loot_obj->Seen( ch ) )
            continue;

          *loot_array += (thing_data*) loot_obj;
          // if autosplitting, need to compute amount of money in corpse 
          if( loot_obj->pIndexData->item_type == ITEM_MONEY )
            amount += loot_obj->number * loot_obj->pIndexData->cost;
        }
      }
      get_obj( ch, loot_array, corpse ); // loot_array deleted by get_obj
      break;

    default: // impossible
      break;
    }
  }

  if( amount > 0 && is_set( ch->pcdata->pfile->flags, PLR_AUTO_SPLIT ) ) 
    split_money( ch, amount, FALSE );

  if( skin )
    if( skin_corpse( ch, corpse ) )
      return; // skin extracted the corpse, so junk doesn't have to

  return;
}


/*
 *   SLAY ROUTINE
 */


void do_slay( char_data* ch, char* argument )
{
  char_data*  victim;
    
  if( ch->shdata->level < LEVEL_BUILDER ) {
    send( ch, "To prevent abuse you are unable to use slay in mortal form.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "Slay whom?\r\n" );
    return;
  }

  if( ( victim = one_character( ch, argument, "slay", ch->array ) ) == NULL )
    return;

  if( ch == victim ) {
    send( ch, "Suicide is a mortal sin.\r\n" );
    return;
  }
    
  if( victim->species == NULL && ( get_trust( victim ) >= get_trust( ch ) || !has_permission( ch, PERM_PLAYERS ) ) ) {
    send( ch, "You failed.\r\n" );
    return;
  }

  send( ch, "You slay %s in cold blood!\r\n", victim );
  send( victim, "%s slays you in cold blood!\r\n", ch );
  send( *ch->array, "%s slays %s in cold blood!\r\n", ch, victim );

  if( player( victim ) != NULL )
    raw_kill( player( victim ) );

  make_corpse( victim, ch->array );

  if( mob( victim ) != NULL )
    victim->Extract( );
}


/*
 *   EXTRACTION ROUTINES
 */


void dereference( char_data* wch, char_data* ch )
{
  player_data*   pc;
  wizard_data*  imm;

  if( !wch->Is_Valid( ) )
    return;

  if( wch->cast != NULL && wch->cast->target == ch ) 
    disrupt_spell( wch );

  wch->seen_by  -= ch;
  wch->known_by -= ch;

  if( wch->fighting == ch ) {
    wch->fighting = NULL;
  }

  if( ( imm = wizard( wch ) ) != NULL && imm->player_edit == ch ) {
    send( imm, "The player you were editing just quit.\r\n" );
    imm->player_edit = NULL;
  }

  if( ( pc = player( wch ) ) != NULL && pc->familiar == ch )
    pc->familiar = NULL;

  if( ( pc = player( wch ) ) != NULL && pc->reply == ch ) {
    if( is_set( &pc->status, STAT_REPLY_LOCK ) ) {
      send( pc, "Reply pointer removed from %s.\r\n", ch );
      remove_bit( &pc->status, STAT_REPLY_LOCK );
    }
    pc->reply = NULL;
  }
} 


void Char_Data :: Extract( )
{
  affect_data*    affect;
  char_data*         wch;
  obj_array*       array;
  mob_data*          npc;
  player_data*        pc;
  wizard_data*       imm;

  /* REMOVE WORLD REFERENCES */

  if( !Is_Valid( ) ) {
    roach( "Extract Char: Extracting invalid character." ); 
    return;
  }

  if( species != NULL ) {
    if( link != NULL  ) 
      do_return( this, "" );
    unregister_reset( this );
  }

  if( link != NULL ) {
    link->character = NULL;
    link->player    = NULL;
    link            = NULL;
  }

  if( this->array != NULL )
    From( );

  clear_queue( this );

  if( ( pc = player( this ) ) != NULL ) {
    remove( request_app, pc );
    remove( request_imm, pc );
  }

  /* CLEAR EVENTS */

  stop_events( this );
  unlink( &active );
  active.time = -1;

  /* EXTRACT OBJECTS */

  if( ( pc = player( this ) ) != NULL ) {
    array = &pc->save_list;
    for( int i = 0; i < array->size; i++ )
      if( array->list[i] != NULL )
        array->list[i]->save = NULL;
    clear( *array );
    extract( pc->locker );
    extract( pc->junked );
  }
 
  extract( contents );
  extract( wearing );

  /* REMOVE FOLLOWERS */

  if( leader != NULL ) {
    if( ( pc = player( leader ) ) != NULL && pc->familiar == this )
      pc->familiar = NULL;
    leader->followers -= this;
    leader = NULL;
  }

  for( int i = 0; i < followers; i++ ) {
    wch         = followers[i];
    wch->leader = NULL;
    wch->shown  = 1;
    wch->selected = 1;
    if( wch->species != NULL && is_set( &wch->status, STAT_PET ) ) {
      send_seen( wch, "%s goes into limbo and awaits %s master's return.\r\n", wch, wch->His_Her( ) );
      wch->Extract( );
    }
  }

  clear( followers );
  
  /* REMOVE REFERENCES ON OTHER PLAYERS */

  for( int i = 0; i < player_list; i++ ) 
    dereference( player_list[i], this );

  for( int i = 0; i < mob_list; i++ ) 
    dereference( mob_list[i], this );

  if( mount != NULL ) {
    mount->rider = NULL;
    mount = NULL;
  }

  if( rider != NULL ) {
    rider->mount = NULL;
    rider = NULL;
  }

  /*
   *   FREE MEMORY
   */

  remove_leech( this );

  for( int i = 0; i < affected; i++ ) {
    affect = affected[i]; 
    if( affect->leech != NULL )
      fsend( affect->leech, "Leech for %s on %s dissipated.", aff_char_table[affect->type].name, descr->name );
    remove_leech( affect );
  }
  clear( affected );

  extract( enemy );
  delete_list( prepare );

  clear( cmd_queue );
  clear( known_by );
  clear( seen_by );
  clear( seen_exits );

  if( species == NULL || descr != species->descr ) 
    delete descr;

  if( ( pc = player( this ) ) != NULL ) {
    delete_list( pc->alias );
    delete_list( pc->tell );
    delete_list( pc->gtell );
    delete_list( pc->whisper );
    delete_list( pc->chat );
    delete_list( pc->gossip );
    delete_list( pc->say );
    delete_list( pc->yell );
    delete_list( pc->shout );
    delete_list( pc->atalk );
    delete_list( pc->to );
    delete_list( pc->chant );
    player_list -= pc; // moved here from Player_Data::~Player_Data()
  }

  if( ( imm = wizard( this ) ) != NULL ) {
    delete_list( imm->build_chan );
    delete_list( imm->imm_talk   );
    delete_list( imm->god_talk   );
    delete_list( imm->avatar     );

    free_string( imm->bamfin,       MEM_WIZARD );
    free_string( imm->bamfout,      MEM_WIZARD );
    free_string( imm->level_title,  MEM_WIZARD );
  }

  if( ( npc = mob( this ) ) != NULL ) { 
    if( npc->pTrainer != NULL ) 
      npc->pTrainer->mob = NULL;
    free_string( npc->pet_name, MEM_MOBS );
  }

  if( pcdata != NULL ) {
    free_string( pcdata->title,        MEM_PLAYER );
    free_string( pcdata->buffer,       MEM_PLAYER ); 
    free_string( pcdata->tmp_short,    MEM_PLAYER );
    free_string( pcdata->tmp_keywords, MEM_PLAYER );
    free_string( pcdata->prompt,       MEM_PLAYER );

    delete pcdata->recognize; 
    delete pcdata;
    delete shdata;
  }

  pcdata   = NULL;
  shdata   = NULL;
  descr    = NULL;
  species  = NULL;
  position = POS_EXTRACTED;
  pShop    = NULL;
  reset    = NULL;
  valid    = -1;

  extracted += this;
}

