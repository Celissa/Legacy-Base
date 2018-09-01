#include "system.h"


const char* stype_name [] = { "offensive", "peaceful", "self_only",
  "object", "room", "continent", "peaceful_other", "weapon", "drink_container",
  "mob_only", "annoying", "corpse", "recall", "weapon_armor", "replicate",
  "astral", "non_combat_healing", "cross_continent", "universal", 
  "annoying_evil", "annoying_good", "annoying_undead", "tree" };

const char* magic_entrap_word [] = { "ghost chains", "webs", "vines", "snare", "briars", "entangling vines", "prison of bone" };
int         magic_entrap [] = { AFF_GHOST_CHAINS, AFF_ENTANGLED, AFF_ENSNARE, AFF_ENSNARE_TRAPPED, AFF_BRIARTANGLE, AFF_WEB_ENTANGLE, AFF_BONE_PRISON };

bool trigger_attack( char_data*, char_data* );

/*
 *   LOCAL FUNCTIONS 
 */


int    check_mana        ( char_data*, int );
int    mana_cost         ( char_data*, int );
void   spell_action      ( int action, char_data*, thing_data*, obj_data* );
void   mod_element_damage( char_data*, int&, int );

/*
 *   CAST_DATA CREATOR/DESTRUCTOR
 */


Cast_Data :: Cast_Data( )
{
  record_new( sizeof( cast_data ), MEM_SPELL );
  target    = NULL;
  customer  = NULL;
  cast_type = 0;
  vzero( reagent, MAX_SPELL_WAIT );
  vzero( tool, MAX_SPELL_WAIT );
}


Cast_Data :: ~Cast_Data( )
{
  record_delete( sizeof( cast_data ), MEM_SPELL );
}


/*
 *   SANITY CHECK ROUTINES
 */

bool null_caster( char_data* ch, int spell )
{
  if( ch == NULL ) {
    bug( "%s: Null pointer for caster.", skill_table[spell].name );
    return TRUE;
  }

  return FALSE;
}


bool null_corpse( obj_data* corpse, int spell )
{
  if( corpse == NULL ) {
    bug( "%s: Null pointer for corpse.", skill_table[spell].name );
    return TRUE;
  }

  return FALSE;
}

/*
 *   Check for Fizzle of Spells
 */

affect_data* find_affect( char_data* ch, int affect )
{
  affect_data*    paf = NULL;

  if( affect < 0 )
    return paf;

  for( int i = ch->affected.size-1; i >= 0; i-- ) {
    if( ch->affected[i]->type == affect ) {
      paf = ch->affected[i];
      break;
    }
  }

  return paf;
}

bool Char_Data :: Fizzle( const char* word )
{
  int             affect = -1;
  affect_data*       paf = NULL;

  for( int i = 0; i < 7; i++ ) {
    affect = magic_entrap[i];
    paf = find_affect( this, affect );
    if( paf != NULL ) {
      if( number_range( 0, 100 ) < paf->mmodifier[1] ) {
        send( this, "%s interferes with your %s.\r\n", magic_entrap_word[i], word );
        return TRUE;
      }
    }
  }

  return FALSE;
}


/*
 *   CHECK_MANA ROUTINES
 */


int check_mana( char_data* ch, int spell )
{
  int i  = mana_cost( ch, spell );

  if( ch->mana < i ) {
    send( ch, "You need %d energy points to %s that.\r\n", i, ch->pcdata->clss == CLSS_DANCER ? "sing" : "cast" );
    return -1;
  }

  return i;
}


int mana_cost( char_data* ch, int spell )
{
  int    mana;
  bool  error  = FALSE;

  if( ch->species != NULL )
    return 0;

  mana = evaluate( spell_table[spell].cast_mana, error, 10*ch->get_skill( SPELL_FIRST+spell )/MAX_SKILL_LEVEL );

  if( error ) 
    bug( "Mana_Cost: Evaluate failed for %s.", spell_table[spell].name );

  if( ch->pcdata != NULL && ch->pcdata->clss == CLSS_MAGE )
    mana += mana*mana_absorption( ch )/10000;

  return mana;
}

int song_cost( char_data* ch, int spell )
{
  int mana;
  bool error = FALSE;

  if( ch->species != NULL )
    return 0;

  mana = evaluate( spell_table[spell].song_regen, error, 10*ch->get_skill( SPELL_FIRST+spell )/MAX_SKILL_LEVEL );

  if( error )
    bug( "Song Regen: Evaluate failed for %s.", spell_table[spell].name );

  return mana;
}

int move_cost( char_data* ch, int skill )
{
  int move;
  bool error = FALSE;

  if( ch->species != NULL && ch->pcdata == NULL )
    return 0;

  move = evaluate( skill_table[skill].move_cost, error, 10*ch->get_skill( skill )/MAX_SKILL_LEVEL );

  if( error )
    bug( "Move_Regen: Evaluate failed for %s.", skill_table[skill].name );

  if( ch->pcdata != NULL && ch->pcdata->clss == CLSS_DANCER )
    move += move*mana_absorption( ch )/10000;

  return move;
}


int mana_absorption( char_data* ch )
{
  obj_data*  obj;
  int       cost;
  int     number;
  int      total  = 0; 

  for( int i = 0; i < ch->wearing; i++ ) {
    obj = (obj_data*) ch->wearing[i];
    if( obj->mana_absorbing_metal( ) && obj->pIndexData->item_type != ITEM_WEAPON ) {
      cost   = 0;
      number = 0;
      for( int j = 0; j <= MAX_MATERIAL; j++ ) 
        if( is_set( &obj->materials, j ) ) {
          number++;
          if( j >= MAT_METAL )
            cost += obj->Empty_Weight( )*material_table[j].mana;
        }
      if( number > 0 )
        total += cost/number;
    }
  }

  return total;
}


/*
 *   REAGENT ROUTINE
 */


int used_reagent( cast_data* cast, obj_data* obj )
{
  int count  = 0;
  int     i;

  for( i = 0; i < MAX_SPELL_WAIT; i++ )
    if( cast->reagent[i] == obj && spell_table[cast->spell].reagent[i] > 0 || cast->target == obj )
      count++;

  return count;
}

obj_data* suitable_reagent( cast_data* cast, obj_clss_data* obj_clss, obj_data *obj )
{
  // sanity check, don't like invalid objects
  if( obj == NULL || !obj->Is_Valid( ) )
    return NULL;

  if( ( obj->pIndexData == obj_clss || obj_clss->item_type == ITEM_CROSS && obj->pIndexData->item_type == ITEM_CROSS ) && used_reagent( cast, obj ) < obj->number ) {
    // this is the reagent, yippee
    return obj;

  } else if( obj->pIndexData->item_type == ITEM_CONTAINER ) {
    // search container for reagents
    for( int k = 0; k < obj->contents; k++ ) {
      obj_data* obj2 = object( obj->contents[k] );

      // sanity check, don't like invalid objects
      if( obj2 == NULL || !obj2->Is_Valid( ) )
        continue;

      if( obj2->pIndexData == obj_clss && used_reagent( cast, obj2 ) < obj2->number ) {
        // this is the reagent, yippee
        return obj2;
      }
    }
  }

  return NULL;
}


bool has_reagents( char_data* ch, cast_data* cast )
{ 
  int                spell  = cast->spell;
  bool             prepare  = cast->prepare;
  int         wait_prepare  = spell_table[spell].prepare;
  obj_clss_data*  obj_clss;

  for( int i = ( prepare ? 0 : wait_prepare ); i < ( prepare ? wait_prepare : MAX_SPELL_WAIT ); i++ ) {
    // get object class for reagent
    if( ( obj_clss = get_obj_index( abs( spell_table[spell].reagent[i] ) ) ) == NULL )
      continue;

    // sanity check - don't cast spell on a reagent
    obj_data* target = object( cast->target );
    if( target != NULL && target->pIndexData->vnum == obj_clss->vnum ) {
      send( ch, "You can't cast a spell on one of its own reagents.\r\n" );
      return FALSE;
    }

    // let's go reagent hunting!
    cast->reagent[i] = NULL;

    // scan inventory for reagents
    for( int j = 0; j < ch->contents; j++ ) {
      obj_data* obj = object( ch->contents[j] );
      if( obj != NULL && obj->Is_Valid( )
        && ( obj->pIndexData == obj_clss || obj_clss->item_type == ITEM_CROSS && obj->pIndexData->item_type == ITEM_CROSS )
        && used_reagent( cast, obj ) < obj->number ) {
        cast->reagent[i] = obj;
        break;
      }
    }

    // scan worn stuff (including bags)
    if( cast->reagent[i] == NULL ) {
      for( int j = 0; j < ch->wearing; j++ ) {
        obj_data* obj = suitable_reagent( cast, obj_clss, object( ch->wearing[j] ) );

        if( obj != NULL ) {
          cast->reagent[i] = obj;
          break;
        }
      }
    }

    // scan inventory inventory again (this time including bags)
    if( cast->reagent[i] == NULL ) {
      for( int j = 0; j < ch->contents; j++ ) {
        obj_data* obj = suitable_reagent( cast, obj_clss, object( ch->contents[j] ) );

        if( obj != NULL ) {
          cast->reagent[i] = obj;
          break;
        }
      }
    }

    // if it's still NULL, it ain't looking good for the non-enlightened
    if( cast->reagent[i] == NULL ) {
      if( ch->pcdata == NULL || ch->shdata->level >= LEVEL_APPRENTICE ) {
        send( ch, "You create %s.\r\n", obj_clss->Name( ) );
        obj_data* obj = create( obj_clss );
        obj->To( ch );
        cast->reagent[i] = obj;
        continue;
      }

      send( ch, "You need %s to cast %s.\r\n", obj_clss->Name( ), spell_table[spell].name );
      return FALSE;
    }
  }

  return TRUE;
}


void remove_reagent( obj_data* reagent, char_data* ch )
{
  if( reagent->pIndexData->item_type != ITEM_REAGENT || reagent->value[0] <= 1 ) {
    // just the one item, or the last charge
    obj_data *temp = object( reagent->From( 1 ) );
    if( temp && temp->Is_Valid() )
      temp->Extract();
    else
      roach( "remove_reagent: unable to remove reagent from %s [single]", ch );

  } else if( reagent->number > 1 ) {
    if( reagent->array == ch->array ) {
      // special handling for reagents in the inventory
      obj_data *temp = object( reagent->From( reagent->number - 1 ) );
      if( !temp || !temp->Is_Valid() ) {
        roach( "remove_reagent: unable to remove reagent from %s [multiple]", ch );
        return;
      }

      // update reagent and give back to inventory
      reagent->value[0]--;
      temp->To( ch );
      consolidate( temp );

    } else {
      // special handling for reagents in bags
      obj_data *temp = object( reagent->From( 1 ) );
      if( !temp || !temp->Is_Valid() ) {
        roach( "remove_reagent: unable to remove reagent from %s [multiple]", ch );
        return;
      }

      // update reagent and give back to inventory
      temp->value[0]--;
      temp->To( ch );
      consolidate( temp );
    }
  } else {
    // subtract 1 from the charge
    reagent->value[0]--;
  }
}


/*
 *   CASTING ROUTINES
 */


int find_spell( char_data* ch, char* argument, int length )
{
  int spell;

  for( spell = 0; spell < SPELL_COUNT; spell++ ) 
    if( ( ch->species != NULL || ch->get_skill( SPELL_FIRST + spell ) != UNLEARNT )
      && !strncasecmp( spell_table[ spell ].name, argument, abs( length ) ) ) 
      return spell;

  if( length < 0 )
    return -1;

  for( spell = 0; spell < SPELL_COUNT; spell++ ) 
    if( !strncasecmp( spell_table[ spell ].name, argument, length ) )
      break;

  if( ch->pcdata->clss == CLSS_DANCER ) {
    if( spell == SPELL_COUNT )
      send( ch, "Unknown Song.\r\n" );
    else
      send( ch, "You don't know the %s.\r\n", spell_table[ spell ].name );

    return -1;
  }

  if( spell == SPELL_COUNT )
    send( ch, "Unknown Spell.\r\n" );
  else
    send( ch, "You don't know the spell %s.\r\n", spell_table[ spell ].name );

  return -1;
}


void do_focus( char_data *ch, char* )
{
  send( "Function disabled.\r\n", ch );
}


void do_cast( char_data* ch, char* argument )
{
  cast_data*           cast;
  cast_data*        prepare  = NULL;;
  int                 spell  = -1;
  int                  mana;

  if( is_confused_pet( ch ) || is_familiar( ch ) || is_shifted( ch ) )
    return;

  if( is_set( ch->affected_by, AFF_SILENCE ) && !is_apprentice( ch ) ) {
    send( ch, "You are silenced and unable to cast spells!\r\n" );
    return;
  }

  if( ch->cast != NULL && ch->cast->cast_type == UPDATE_SONG ) {
    send( ch, "You can't sing and cast spells at the same time.\r\n" );
    return;
  }

  if( ch->Fizzle( "attempted casting of a spell" ) )
    return;
/* same as blade-dancer, better check put in after spell is found
  if( ch->pcdata->clss == CLSS_DANCER ) {
    send( ch, "You know nothing of casting spells.\r\n" );
    return;
  }
*/
  if( *argument == '\0' ) {
    send( ch, "What spell do you want to cast?\r\n" );
    return;
  }

  // new spellfinding code
  int target = strlen( argument );

  // loop until we find a spell or get down to zero arguments
  for( ;; ) {
    // see if we have a valid spell name
    spell = find_spell( ch, argument, -target );
    if( spell != -1 )
      break;

    // if not, go back to the previous word
    while( isspace( argument[ target ] ) )
      target--;

    // to the start of that word
    for( ; argument[ target ] != ' ' && target > 0; target-- );

    // and the spaces before that word
    while( isspace( argument[ target ] ) )
      target--;

    // if we're down to zero it was the first argument of the spell, so
    // let it give the unknown spell message and exit
    if( target == 0 ) {
      spell = find_spell( ch, argument, strlen( argument ) );
      if( spell == -1 )
        return;
    }

    // increase target separator by 1 so rather than sitting on the
    // 'l rabbit' of 'fireball rabbit' it sits on the ' ' between the words
    target++;
  }

  mana = 0;

  if( spell_table[spell].prepare != 0 ) {
    if( ( prepare = has_prepared( ch, spell ) ) == NULL ) {
      send( ch, "You don't have that spell prepared.\r\n" );
      return;
    }
  } else {
    if( ch->species == NULL && ( mana = check_mana( ch, spell ) ) < 0 )
      return;
  }

  if( !allowed_location( ch, &spell_table[spell].location, "cast", spell_table[spell].name ) )
    return;

  if( ch->species == NULL && ch->fighting != NULL ) 
    if( spell_table[spell].type == STYPE_NON_COMBAT_HEALING ) {
      send( ch, "You can't cast that spell while fighting.\r\n" );
      return;
    }

  if( spell_table[spell].song ) {
    send( ch, "That is a song, not a spell.\r\n" );
    return;
  }

  cast            = new cast_data;
  cast->spell     = spell;
  cast->prepare   = FALSE;
  cast->wait      = spell_table[spell].prepare-1;
  cast->mana      = mana;
  cast->cast_type = UPDATE_CAST;

  if( !get_target( ch, cast, &argument[ target] ) || !has_reagents( ch, cast ) ) {
    delete cast;
    return;
  }

  send( ch, "You begin casting %s.\r\n", spell_table[spell].name );

  if( ch->species == NULL && spell_table[spell].prepare != 0 ) {
    if( --prepare->times == 0 ) { 
      remove( ch->prepare, prepare );
      delete prepare;
    } else if( is_set( &ch->pcdata->message, MSG_SPELL_COUNTER ) ) {
      send( ch, "[ You have %s %s spell%s remaining. ]\r\n", number_word( prepare->times, ch ), spell_table[spell].name, prepare->times == 1 ? "" : "s" );
    }
  }

  ch->cast    = cast;
  ch->mana   -= mana;
  
  int delay = ch->species != NULL ? 6 : 15-10*ch->get_skill( SPELL_FIRST+spell )/MAX_SKILL_LEVEL;
  set_delay(ch, delay);

  return;
}


void do_prepare( char_data* ch, char* argument )
{
  char           tmp  [ MAX_INPUT_LENGTH ];
  cast_data*    cast;
  int          spell;
  int           mana;
  int         length  = strlen( argument );
  
  if( ch->species != NULL ) {
    send( ch, "Only players can prepare spells.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    if( ch->prepare == NULL ) {
      send( "You have no spells prepared.\r\n", ch );
      return;
    }
    page_underlined( ch, "Num  Spell                   Mana\r\n" );
    for( cast = ch->prepare; cast != NULL; cast = cast->next ) {
      sprintf( tmp, "%3d  %-25s%3d\r\n", cast->times, spell_table[cast->spell].name, cast->mana*cast->times );
      page( ch, tmp );
    }

    return;
  }
  
  if( !strcasecmp( "clear", argument ) ) {
    delete_list( ch->prepare );
    send( ch, "All prepared spells forgotten.\r\n" );
    update_max_mana( ch );
    return;
  }
  
  if( is_set( ch->affected_by, AFF_SILENCE ) ) {
    send( "You are silenced and unable to prepare a spell!\r\n", ch );
    return;
  }
/*  
  if( is_set( ch->affected_by, AFF_ENTANGLED ) || is_set( ch->affected_by, AFF_ENSNARE_TRAPPED ) ) {
    send( ch, "You are too entangled to prepare a spell.\r\n" );
    return;
  }
*/
  if( is_entangled( ch, "prepare", true ) )
    return;

  if( ch->position < POS_RESTING ) {
    send( ch, "You cannot prepare spells while sleeping or meditating.\r\n" );
    return;
  }
  
  if( ( spell = find_spell( ch, argument, length ) ) == -1 )
    return;
  
  if( spell_table[ spell ].prepare == 0 ) {
    send( ch, "That is not a spell which you prepare.\r\n" );
    return;
  } 
  
  if( ( mana = check_mana( ch, spell ) ) < 0 )
    return;

  cast           = new cast_data;
  cast->spell    = spell;    
  cast->wait     = -1;
  cast->prepare  = TRUE;
  cast->mana     = mana;
  
  if( !has_reagents( ch, cast ) ) {
    delete cast;
    return;
  }

  send( ch, "You begin preparing %s.\r\n", spell_table[spell].name );

  ch->cast    = cast;
  ch->mana   -= mana;

  int delay = ch->species != NULL ? 12 : 16 - 5*ch->get_skill( SPELL_FIRST+spell )/MAX_SKILL_LEVEL;
  set_delay(ch, delay);

  return;
}


cast_data* has_prepared( char_data* ch, int spell )
{
  cast_data* prepare;

  if( ch->species == NULL && spell_table[spell].prepare != 0 ) 
    for( prepare = ch->prepare; prepare != NULL; prepare = prepare->next )
      if( prepare->spell == spell )
        return prepare;

  return NULL;
}


/*
 *   UPDATE ROUTINE
 */


void spell_update( char_data* ch )
{
  cast_data*     cast  = ch->cast;
  thing_data*  target  = cast->target;
  char_data*   victim  = character( target );
  int           spell  = cast->spell;
  bool        prepare  = cast->prepare;
  int            wait  = ++cast->wait;

  char_data*      rch;
  int           skill;
  obj_data*   reagent;
  int          action;
  cast_data*     prev;

  if( wait < ( cast->prepare ? spell_table[spell].prepare : spell_table[spell].wait ) ) {
    reagent  = cast->reagent[wait];
    action   = spell_table[spell].action[wait];
    msg_type = cast->prepare ? MSG_PREPARE : MSG_STANDARD;
    
    spell_action( action, ch, target, reagent );
    if( reagent != NULL && spell_table[spell].reagent[wait] >= 0 )
      remove_reagent( reagent, ch );
    
    int delay = ch->species != NULL ? 31 : 35-10*ch->get_skill( SPELL_FIRST+spell )/MAX_SKILL_LEVEL;
    set_delay(ch, delay);
    return;
  }

  ch->cast = NULL;

  set_delay( ch, 2 );
  update_max_mana( ch );

  if( !prepare ) { 
    delete cast;

    if( is_set( &ch->in_room->room_flags, RFLAG_NO_MAGIC ) && !is_apprentice( ch ) ) {
      fsend( ch, "As you cast %s, you feel the energy drain from you and nothing happens.\r\n", spell_table[spell].name );
      send( *ch->array, "%s casts %s, but nothing happens.\r\n", ch, spell_table[spell].name );
      return;
    }
  
    send( ch, "+++ You cast %s. +++\r\n", spell_table[spell].name );
    send_seen( ch, "%s casts %s.\r\n", ch, spell_table[spell].name );

    skill = ( ch->species != NULL ? 8 : 10*ch->get_skill( SPELL_FIRST+spell )/MAX_SKILL_LEVEL );
    
    if( spell_table[spell].type == STYPE_ANNOYING ) {
      for( int i = 0; i < *ch->array; i++ ) {
        if( ( rch = mob( ch->array->list[i] ) ) != NULL && can_kill( ch, rch ) && rch->Seen( ch ) ) {
          check_killer( ch, rch );
          if( ch->fighting == NULL ) {
            ch->fighting = rch;
            react_attack( ch, rch );
            set_delay( ch, 15 );
            init_attack( ch, rch );
          } else 
            init_attack( rch, ch );
        }
      }
    }

    if( spell_table[spell].type == STYPE_ANNOYING_GOOD ) {
      for( int i = 0; i < *ch->array; i++ ) {
        if( ( rch = mob( ch->array->list[i] ) ) != NULL && can_kill( ch, rch ) && rch->Seen( ch ) && is_good( rch ) ) {
          check_killer( ch, rch );
          trigger_attack( ch, rch );
          if( ch->fighting == NULL ) {
            ch->fighting = rch;
            react_attack( ch, rch );
            set_delay( ch, 15 );
            init_attack( ch, rch );
          } else 
            init_attack( rch, ch );
        }
      }
    }

    if( spell_table[spell].type == STYPE_ANNOYING_EVIL ) {
      for( int i = 0; i < *ch->array; i++ ) {
        if( ( rch = mob( ch->array->list[i] ) ) != NULL && can_kill( ch, rch ) && rch->Seen( ch ) && is_evil( rch ) ) {
          check_killer( ch, rch );
          trigger_attack( ch, rch );
          if( ch->fighting == NULL ) {
            ch->fighting = rch;
            react_attack( ch, rch );
            set_delay( ch, 15 );
            init_attack( ch, rch );
          } else
            init_attack( rch, ch );
        }
      }
    }

    if( spell_table[spell].type == STYPE_ANNOYING_UNDEAD ) {
      for( int i = 0; i < *ch->array; i++ ) {
        if( ( rch = mob( ch->array->list[i] ) ) != NULL && can_kill( ch, rch ) && rch->Seen( ch ) && rch->shdata->race == RACE_UNDEAD ) {
          check_killer( ch, rch );
          trigger_attack( ch, rch );
          if( ch->fighting == NULL ) {
            ch->fighting = rch;
            react_attack( ch, rch );
            set_delay( ch, 15 );
            init_attack( ch, rch );
          } else
            init_attack( rch, ch );
        }
      }
    }
    
    if( spell_table[spell].type == STYPE_OFFENSIVE ) {
      check_killer( ch, victim );
      ch->fighting = victim;
      react_attack( ch, victim );
      set_delay( ch, 15 );
      init_attack( ch, victim );
    }

    remove_bit( &ch->status, STAT_LEAPING );
    remove_bit( &ch->status, STAT_WIMPY );

    leave_camouflage(ch);
    leave_shadows(ch);
    strip_affect( ch, AFF_INVISIBLE );
    remove_bit( ch->affected_by, AFF_INVISIBLE );

    ch->improve_skill( SPELL_FIRST+spell );

    ( *spell_table[spell].function )( ch, character( target ), target, skill, 0 );

    return;
  }

  for( prev = ch->prepare; prev != NULL; prev = prev->next )
    if( prev->spell == spell ) {
      prev->times++;
      delete cast;
      break;
    }

  if( prev == NULL ) {
    cast->times = 1; 
    cast->next  = ch->prepare;
    ch->prepare = cast;
    prev        = cast;
  }

  if( prev->times > 1 ) {
    send( ch, "You now have %s incantations of %s prepared.\r\n", number_word( prev->times, ch ), spell_table[spell].name );
  } else {
    send( ch, "You have prepared %s.\r\n", spell_table[spell].name );
  }

  update_max_mana( ch );
}


void spell_action( int action, char_data* ch, thing_data* target, obj_data* reagent )
{
  char_data* victim = NULL;

  if( action < 0 || action > table_max[ TABLE_SPELL_ACT ] ) {
    roach( "Spell_Action: Impossible action." );
    roach( "-- Caster = %s", ch->Name( ) );
    return;
  }

  if( reagent != NULL )
    reagent->selected = 1;

  if( target == ch ) {
    act( ch, spell_act_table[action].self_self,       ch, NULL, reagent );
    act_notchar( spell_act_table[action].others_self, ch, NULL, reagent );
    return;
  }

  victim = character( target );
  if( victim != NULL )
    act( ch, spell_act_table[action].self_other, ch, victim, reagent );
  else // target isn't a character
    act( ch, spell_act_table[action].self_other, ch, NULL, reagent, target );

  if( target != NULL && victim != NULL && spell_act_table[action].victim_other != empty_string ) {
    if( victim->position > POS_SLEEPING )
      act( victim, spell_act_table[ action ].victim_other, ch, victim, reagent );
    act_neither( spell_act_table[ action ].others_other, ch, victim, reagent );
  } else { // target isn't a character
    act_notchar( spell_act_table[action].others_other, ch, NULL, reagent, target );
  }
}


/*
 *   RESISTANCE ROUTINES  
 */


bool makes_save( char_data* victim, char_data* ch, int type, int spell, int level )
{
  // cast a spell on yourself, always fail
  if( ch == victim )
    return FALSE;

  // if victim is null, assume passed
  if (victim == NULL) {
    roach("makes_save: NULL victim!");
    return TRUE;
  }
  
  // if victim is being extracted, assume true
  if( victim->hit <= 0 )
    return TRUE;

  // small chance of failing or succeeding right out
  if (number_range(0, 33) == 33)
    return number_range(0, 1) ? FALSE : TRUE;

  int chance;

  switch( type ) {
  case RES_MAGIC:
    chance = victim->Save_Magic() + victim->shdata->level / 2;
    break;

  case RES_MIND:
    chance = victim->Save_Mind() + victim->shdata->level / 2; 
    break;
  
  case RES_STRENGTH:
    if (victim->position <= POS_RESTING)
      return FALSE;
    chance = 2 * victim->Strength() + victim->shdata->level / 2;
    break;

  case RES_DEXTERITY:
    if (victim->position <= POS_RESTING)
      return FALSE;
    chance = 2*victim->Dexterity( ) + victim->shdata->level / 5;
    break;

  case RES_HOLY:
    chance = victim->Save_Holy( ) + victim->shdata->level/2;
    break;
    
  case RES_POISON:
    chance = victim->Save_Poison( ) + victim->shdata->level/2;
    break;

  default:
    bug( "Makes_Save: Unknown Resistance." );
    return TRUE;
  }

  if (ch != NULL) {
    chance += (victim->shdata->level - ch->shdata->level) / 5;
    chance -= 10*ch->get_skill( spell )/MAX_SKILL_LEVEL;
  }
  
  return (number_range( 0, 99 ) < chance);
}


/*
 *   TABLE EVALUATE ROUTINES
 */


int spell_damage( int spell, int level, int var )
{
  int   damage;
  bool   error  = FALSE;

  if( level < 0 ) 
    level = (-level)%100;

  spell  -= SPELL_FIRST;
  damage  = evaluate( spell_table[spell].damage, error, level, var );

  if( error ) 
    bug( "Spell_Damage: Evaluate failed for %s.", spell_table[spell].name );

  return damage;
}


int duration( int spell, int level, int var )
{
  int    duration;
  bool      error  = FALSE;

  spell    -= SPELL_FIRST;
  duration  = evaluate( spell_table[spell].duration, error, level, var );

  if( error ) 
    bug( "Duration: Evaluate failed for %s.", spell_table[spell].name );

  return duration;
}


int leech_regen( int spell, int level, int var )
{
  int    regen;
  bool   error  = FALSE;

  spell -= SPELL_FIRST;
  regen  = evaluate( spell_table[spell].regen, error, level, var );

  if( error ) 
    bug( "Leech_Regen: Evaluate failed for %s.", spell_table[spell].name );

  return regen;
}


int leech_mana( int spell, int level, int var )
{
  int    mana;
  bool  error  = FALSE;

  spell -= SPELL_FIRST;
  mana   = evaluate( spell_table[spell].leech_mana, error, level, var );

  if( error ) 
    bug( "Leech_Mana: Evaluate failed for %s.", spell_table[spell].name );

  return mana;
}


void spell_affect( char_data* ch, char_data* victim, int level, int time, int spell, int type, int var )
{
  if( !victim )
    return;

  affect_data affect;
  affect.type = type;

  if( ch == NULL || time > 0 ) {
    affect.duration = time;
    affect.level    = level;
  } else {
    affect.level    = level;
    affect.duration = duration( spell, level, var );

    if( spell_table[ spell-SPELL_FIRST ].regen != empty_string ) {
      affect.leech_regen = leech_regen( spell, level, var );
      affect.leech_max   = leech_mana( spell, level, var );
      affect.leech       = ch;
    }

    if( spell == SPELL_COMPANIONS_STRENGTH ) {
      affect.mlocation[ 0 ] = affect_loc[ victim->species->compan_str ];
      affect.mmodifier[ 0 ] = victim->species->compan_amt;
    }
  }
   
  if( spell == SPELL_COMPANIONS_STRENGTH )
    add_affect( ch, &affect );
  else
    add_affect( victim, &affect );

  return;
}



