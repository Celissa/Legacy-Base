#include "system.h"


char *consider_string      ( double ratio );
int get_current_cost       ( char_data*, obj_data* );

int skill_index( const char* name )
{
  int i;

  if( *name != '\0' )
    for( i = 0; i < MAX_SKILL; i++ )
      if( !strcasecmp( name, skill_table[i].name ) )
       return i;

  return -1;
}

int total_synergy( char_data* ch, int skill )
{
  int                 total = 0;
  obj_data*            worn;
  obj_clss_data*   obj_clss;
  synergy*              syn;
  int             syn_skill;;

  if( is_empty( ch->wearing ) )
    return 0;

  for( int i = 0; i < ch->wearing; i++ ) {
    if( ( worn = object( ch->wearing[i] ) ) != NULL  ) {
      obj_clss = worn->pIndexData;

      if( skill_table[ skill ].synergy[ CLSS_DEFENSIVE ] )
        total += worn->pIndexData->clss_synergy[ CLSS_DEFENSIVE+1 ];

      if( skill_table[ skill ].synergy[ ch->pcdata->clss+1 ] )
        total += worn->pIndexData->clss_synergy[ ch->pcdata->clss+1 ];

      if( is_empty( obj_clss->synergy_array ) )
        continue;

      for( int i = 0; i < obj_clss->synergy_array; i++ ) {
        syn = obj_clss->synergy_array[i];
        if( ( syn_skill = skill_index( syn->skill ) ) != -1 && syn_skill == skill )
          total += syn->amount;
      }
    }
  }

  return total;
}


int char_data :: get_skill( int i )
{
  int skill_synergy = 0;

  if( this == NULL )
    return UNLEARNT;

  if( species != NULL ) {
    switch( i ) {
    case SKILL_CLIMB :
      return( is_set( species->act_flags, ACT_CAN_CLIMB ) ? MAX_SKILL_LEVEL : 0 );
    case SKILL_HIDE:
      return( is_set( species->affected_by, AFF_HIDE ) ? MAX_SKILL_LEVEL : 0 );
    case SKILL_PENETRATE:
      return 0;
    default :
      return MAX_SKILL_LEVEL;
    }
  }

  if( i < 0 || i > MAX_SKILL )
    return 0;

  if( pcdata->skill[i] == UNLEARNT )
    return 0;

  skill_synergy = total_synergy( this, i );

  skill_synergy += pcdata->skill[ i ];

  skill_synergy = min( 3/2*MAX_SKILL, max( 0, skill_synergy ) );

  // disabling this automatic 10 for now as it causes issues with using other classes skills
  // eg a warrior can spam '#50 prep heal' and eventually they will prepare a heal, its not
  // reliable enough for in-combat use, but you can use it to heal out of combat
  // if( number_range( 1, 200 ) == 1 )
  //   return (skill_synergy+10);

  return skill_synergy;
}

int char_data :: real_skill( int i/*, int table_type, char* skill_search */)
{
  if( species != NULL ) {
    switch( i ) {
    case SKILL_CLIMB :
      return( is_set( species->act_flags, ACT_CAN_CLIMB ) ? MAX_SKILL_LEVEL : 0 );
    case SKILL_HIDE:
      return( is_set( species->affected_by, AFF_HIDE ) ? MAX_SKILL_LEVEL : 0 );
    default :
      return MAX_SKILL_LEVEL;
    }
  }

  if( i < 0 || i > MAX_SKILL )
    return 0;
/*
  // need to search multiple different tables for new classes 
  // (core-class)

  if( clss_table[ pcdata->clss ].new_style ) {
    // search the core table for the skill (this is auto-defensive skills etc)
    if( table_type == skill_core ) {
      if( i >= 0 && i < MAX_ENTRY_CORE )
        return pcdata->core_skill[i];
      else
        return UNKNOWN;
    }
    else {
      // need to search the specific skill in the classes table to find
      // the specific skill - since all classes will be using the same 200
      // sized array for skill storage - so 1 is different for each class
      if( pcdata->clss == CLSS_CAVALIER )
        // find the skill in the cavalier list
      if( pcdata->clss == CLSS_BARBARIAN )
        // find the skill in the barbarian list
      if( pcdata->clss == CLSS_ROGUE )
        // find the skill in the rogue list
      if( pcdata->clss == CLSS_ASSASSIN )
        //find the skill in the assassin list
    }
      return pcdata->class_skill[i];
  }
  */
  return pcdata->skill[ i ];
}
/*
int skill_location( char* skill, int clss )
{
  const char* arg = skill;

  if( *skill == '\0' )
    return -1;

  if( clss == CLSS_CAVALIER ) {
    for( int i = 0; i < MAX_ENTRY_CAVALIER; i++ ) {
      const char* word = cavalier_skill_table[i].skill_name;
      if( exact_match( arg, skill ) )
        return i;
    }
  }

  else if( clss == CLSS_BARBARIAN ) {
    for( int i = 0; i < MAX_ENTRY_BARBARIAN; i++ ) {
*/
int char_data :: get_skill_usage( int i )
{
  if( species != NULL )
    return 0;

  if( i < 0 || i > MAX_SKILL )
    return 0;

  return pcdata->skill_usage[i];
}

bool char_data :: check_skill( int i )
{
  if( pcdata == NULL )
    return TRUE;

  if( get_skill( i ) == UNLEARNT || number_range( 1, 11*MAX_SKILL_LEVEL/10 ) > get_skill( i ) )
    return FALSE;

  return TRUE;
}


/*
 *   BATTLE SKILLS
 */


void do_consider( char_data* ch, char* argument )
{
  char_data*  victim;

  if( *argument == '\0' ) {
    send( ch, "Who do you want to consider killing?\r\n" );
    return;
  }

  if( ( victim = one_character( ch, argument, "consider", ch->array ) ) == NULL )
    return;

  if( IS_AFFECTED( ch, AFF_HALLUCINATE ) ) {
    send( ch, "Hah! Death to all grid bugs!\r\n" );
    return;
  }

  if( victim->species != NULL && victim->shdata->deaths == 0 ) {
    send( ch, "You look at %s but are unable to determine how difficult %s is.\r\n", victim, victim->He_She( ) );
  } else { 
    send( consider_string( ( double ) xp_compute( victim ) / xp_compute( ch ) ), ch );
  }

  if( victim->species != NULL )
    send( ch, "[ Nation: %s ]\r\n", nation_table[ victim->species->nation ].name );
}


char* consider_string( double ratio )
{
  if( ratio > 16 )  return "Attacking and suicide are equivalent.\r\n";
  if( ratio > 8 )   return "You would require divine intervention.\r\n";
  if( ratio > 4 )   return "You don't have much of a hope.\r\n";
  if( ratio > 2 )   return "You'll need a lot of help.\r\n";
  if( ratio > 1.5 ) return "With luck on your side you might prevail.\r\n";
  if( ratio > 1 )   return "It will be a tough fight.\r\n";
  
  if( 1./ratio > 5 ) return "An unworthy opponent.\r\n";
  if( 1./ratio > 2 ) return "Should be an easy kill.\r\n";
  if( 1./ratio > 1.5 ) return "You shouldn't have much difficulty.\r\n";
  
  return "It looks like a fair fight.\r\n";
}
 

void do_inspect( char_data* ch, char* argument )
{
  if( is_confused_pet( ch ) )
    return;

  obj_data*   obj;
  if( ( obj = one_object( ch, argument, "inspect", ch->array ) ) == NULL ) {
    return;
  }

  if( obj->pIndexData->item_type != ITEM_CONTAINER ) {
    send( "That's not a container.\r\n", ch );
    return;
  }

  if( !is_set( &obj->value[1], CONT_CLOSED ) ) {
    send( ch, "It's open - just look inside it.\r\n" );
    return;
  }

  if( IS_NPC( ch ) || ch->get_skill( SKILL_INSPECT ) == UNLEARNT ) {
    send( ch, "You really don't know how to inspect containers.\r\n" );
    return;
  }

  obj_data*  trap = NULL;
  obj_data*  item = NULL;
  for( int i = 0; i < obj->contents.size; i++ ) {
    item = object( obj->contents[i] );
    if( item == NULL )
      continue;
    if( item->pIndexData->item_type == ITEM_TRAP && item->pIndexData->level <= ch->shdata->level && ch->check_skill( SKILL_INSPECT ) ) {
      trap = item;
      remove_bit( trap->extra_flags, OFLAG_DARK );
      break;
    }
  }

  if( trap == NULL ) {
    send( ch, "You see nothing unusual.\r\n" );
    return;
  }

  send( ch, "YIKES! You find %s.\r\n", trap );        
  ch->improve_skill( SKILL_INSPECT );

  return;
}


void do_untrap( char_data *ch, char *argument )
{
  if( is_confused_pet( ch ) )
    return;

  char arg [ MAX_INPUT_LENGTH ];
  argument = one_argument( argument, arg );

  if( *arg == '\0' ) {
    send( "What do you want to try to remove a trap from?\r\n", ch );
    return;
  }
  
  if( IS_NPC( ch ) ) { // || ch->get_skill( SKILL_UNTRAP ) == UNLEARNT ) {
    send( ch, "You really don't know how to remove traps.\r\n" );
    return;
  }

  if( ch->in_room == NULL ) {
    //roach("Character trying to untrap while not in a room.", ch);
    return;
  }

  thing_data *thing;
  if( ( thing = one_thing( OBJ_DATA | EXIT_DATA, ch, arg, "remove a trap from", ch->array, (thing_array*) &ch->in_room->exits ) ) == NULL )
    return;

  obj_data* obj;
  exit_data *door;
  set_delay( ch, 32 );
  if( ( door = exit( thing ) ) != NULL ) {
    room_data *room = ch->in_room;

    for( action_data *action = room->action; action != NULL; action = action->next )
      if( action->trigger == TRIGGER_UNTRAP_DOOR && is_set( &action->flags, door->direction ) ) {
        var_ch   = ch;
        var_room = room;
        if( execute( action ) ) // && ch->in_room == room ) // returns 'true' on a 'continue' success
          ch->improve_skill( SKILL_UNTRAP );
        return;
      }
    send( ch, "You find no traps to disarm.\r\n" );

  } else if( ( obj = object( thing ) ) != NULL ) {
    oprog_data *oprog;
    for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) {
      if( oprog->trigger == OPROG_TRIGGER_UNTRAP ) {
        var_obj  = obj;
        var_ch   = ch;
        var_room = ch->in_room;
        if( execute( oprog ) ) // returns 'true' on a 'continue' success
          ch->improve_skill( SKILL_UNTRAP );
        return;
      }
    }

    if( obj->pIndexData->item_type != ITEM_CONTAINER ) {
      send( "That's not a container.\r\n", ch );
      return;
    }

    if( !is_set( &obj->value[1], CONT_CLOSED ) ) {
      send( "It's open - just look inside it.\r\n", ch );
      return;
    }

    // Loop over all traps inside the container.
    int i;
    obj_data* trap = NULL;
    bool found_trap = false;
    for( i = obj->contents.size - 1; i >= 0; i-- ) {
      trap = object( obj->contents[i] );
      if( !trap || !trap->Is_Valid( ) )
        continue;
      if( trap->pIndexData->item_type == ITEM_TRAP && trap->Seen( ch ) ) {
        found_trap = true;
        // Execute the trap's DISARM trigger.  Hopefully this does a test on
        // player's untrap skill, and either extracts the trap (thus affecting
        // obj->contents array) or executes the trap's USE (detonate) trigger.
        for( oprog = trap->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) {
          if( oprog->trigger == OPROG_TRIGGER_UNTRAP ) {
            var_obj       = trap;
            var_container = obj;
            var_ch        = ch;    
            var_room      = ch->in_room;
            if( execute( oprog ) ) // returns 'true' on a 'continue' success
              ch->improve_skill( SKILL_UNTRAP );
            break;
          }
        }
      }
    }

    if ( !found_trap ) {
      send( ch, "You find no traps to disarm.\r\n" );
    }
  }

  return;
}


void do_compare( char_data* ch, char* argument )
{
  char           arg  [ MAX_INPUT_LENGTH ];
  obj_data*     obj1;
  obj_data*     obj2;
  int          flags;

  if( !get_flags( ch, argument, &flags, "sS", "compare" ) )
    return;

  if( !two_argument( argument, "with", arg ) ) {
    send( ch, "Syntax: compare <obj1> [with] <obj2>.\r\n" );
    return;
  }  

  /*
  if( flags != 0 ) {
    if( ( keeper = find_keeper( ch ) ) == NULL )
      return;
    if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) ) {
      send( ch, "You can't compare pets yet.\r\n" );
      return;
      }
    }
  */
    
  if( ( obj1 = one_object( ch, arg, "compare", &ch->contents ) ) == NULL )  
    return;

  if( ( obj2 = one_object( ch, argument, "compare", &ch->contents ) ) == NULL )  
    return;

  if( obj1 == obj2 ) {
    fsend( ch, "You compare %s with itself coming to no useful conclusion.", obj1 );
    return;
  }

  if( obj1->pIndexData->item_type != obj2->pIndexData->item_type ) {
    send( ch, "Objects must be of same type.\r\n" );
    return;
  }

  if( obj1->pIndexData->item_type == ITEM_ARMOR ||
    obj1->pIndexData->item_type == ITEM_SHIELD ) {
    int armor1 = armor_class( obj1 );
    int armor2 = armor_class( obj2 );
  
    if( ( obj1->pIndexData->wear_flags & obj2->pIndexData->wear_flags & ~( 1 << ITEM_TAKE ) ) == 0 ) {
      send( "Those items do not get worn on the same location, so comparing them is not a\r\nuseful exercise.\r\n", ch );
      return;
    }

    if( armor1 == armor2 ) {
      fsend( ch, "You think %s and %s would offer similar protection.\r\n", obj1, obj2 );
      return;
    }

    if( armor1 < armor2 )
      m2swap( obj1, obj2 );

    fsend( ch, "You think %s would offer better protection than %s.", obj1, obj2 );
    return;
  }

  if( obj1->pIndexData->item_type == ITEM_WEAPON ||
    obj1->pIndexData->item_type == ITEM_ARROW ) {
    int dam1 = obj1->value[1]*(obj1->value[2]+1)+2*obj1->value[0];
    int dam2 = obj2->value[1]*(obj2->value[2]+1)+2*obj2->value[0];

    if( dam1 == dam2 ) {
      fsend( ch, "You think %s and %s would do similar damage.", obj1, obj2 );
      return;
    }

    if( dam1 < dam2 ) 
      m2swap( obj1, obj2 );

    fsend( ch, "You think %s would do more damage than %s.", obj1, obj2 );
    return;
  }

  send( ch, "You can only compare weapons and armor.\r\n" );

  return;
}


void do_appraise( char_data *ch, char *argument )
{
  obj_data* obj;

  if( *argument == '\0' ) {
    send( ch, "Appraise what?\r\n" );
    return;
  }

  if( ( obj = one_object( ch, argument, "appraise", &ch->contents ) ) == NULL ) {
    send( ch, "You do not have that item.\r\n" );
    return;
  }

  player_data* a_player = player( ch );
  if( a_player == NULL || ch->get_skill( SKILL_APPRAISE ) == UNLEARNT ) {
    send( ch, "You know no more about an object's value than what you have learnt from\r\nlooking in the shops.\r\n" );
    return;
  }

  if( !is_set( obj->extra_flags, OFLAG_APPRAISED ) ) {
    ch->check_skill( SKILL_APPRAISE );
    set_bit( obj->extra_flags, OFLAG_APPRAISED );
  }

  int skill = 10*ch->get_skill( SKILL_APPRAISE )/MAX_SKILL_LEVEL;
  if( obj->pIndexData->cost > skill * skill * skill * ch->shdata->level / 2) {
    send( ch, "You think %s is valuable but are unsure how valuable.\r\n", obj );
    return;
  }

  if( obj->pIndexData->cost <= 0 ) {
    send( ch, "%s looks worthless to you.\r\n", obj );
    return;
  }

  int appraised_cost = get_current_cost( ch, obj );

  send( ch, "You think %s would be worth about %d cp.\r\n", obj, obj->pIndexData->cost );

  if( appraised_cost > 0 )
    send( ch, "In its current condition though, you think %s would be worth about %i cp.\r\n", obj, appraised_cost );
  else if( is_set( obj->extra_flags, OFLAG_NO_SELL ) )
    send( ch, "You do not believe any shop keeper will buy %s.\r\n", obj );
  else
    send( ch, "In its current condition, %s is worthless\r\n", obj );

  ch->improve_skill( SKILL_APPRAISE );
}


void do_bandage( char_data *ch, char *argument )
{
  char_data *victim;

  if( argument[0] == '\0' ) {
    send( "Bandage who?\r\n", ch );
    return;
    }

  if( ( victim = one_character( ch, argument, "bandage", ch->array ) ) == NULL ) {
    send( "They aren't here.\r\n", ch );
    return;
    }
/*
  if( ch == victim ) {
    send(
      "If you are able to bandage you don't need to be bandaged.\r\n", ch );
    return;
    }

  if( victim->hit > 0 ) {
    send( "They don't need bandaging.\r\n", ch );
    return;
    }
*/
  if( ch->fighting != NULL ) {
    send( "You can't bandage someone while fighting!\r\n", ch );
    return;
    }

  if( IS_NPC( ch ) || ch->get_skill( SKILL_BANDAGE ) == UNLEARNT ) {
    send( "You really wouldn't know how to bandage wounds.\r\n", ch );
    return;
  }

  obj_data* obj = NULL;
  for ( int i = 0 ; i < ch->contents.size ; i++ ) {
    obj = object(ch->contents[i]);
    if( obj != NULL ) {
      if( obj->pIndexData->item_type == ITEM_BANDAGE && 
        obj->position == WEAR_NONE )
          break;
    }
    obj = NULL;
  }

  if( obj == NULL ) {
    send( "You don't have anything to bandage them with!\r\n", ch );
    return;
    }
  
  obj->selected = 1;

  if( ch->check_skill( SKILL_BANDAGE ) ) {
    if( ch != victim ) {
    send( ch,
      "You use %s to bandage %s's wounds stabilizing %s injuries.\r\n",
      obj, victim, victim->His_Her( ) );
    send( victim, "%s bandages your wounds, stabilizing your injuries.\r\n", ch );
    act_neither( "$n uses $p to bandage $N's wounds stabilizing $S injuries.", ch, victim, obj );
    }
    else {
      send( ch, "You use %s to bandage your wounds with, to stabilize your injuries.\r\n", obj );
      send( *ch->array, "%s uses %s to bandage %s wounds with, stabilizing %s injuries.\r\n", ch, obj, ch->His_Her( ), ch->His_Her( ) );
    }
    if( victim->hit < 1 )
      victim->hit = 1;
    physical_affect( ch, victim, SKILL_BANDAGE, AFF_BANDAGE );
    update_pos( victim );
    ch->improve_skill( SKILL_BANDAGE );
    }
  else {
    if( ch != victim ) {
    send( ch,
      "You try to bandage %s's wounds but just make the situation worse.\r\n",
      victim );
    send( victim, "%s tries to bandage your wounds, but just makes the situation worse.\r\n", ch );
    act_neither( "$n tries to bandage $N's wounds but does more harm than good.", ch, victim, obj );
    }
    else {
      send( ch, "You try to bandage your wounds, but just make the situation worse.\r\n" );
      send( *ch->array, "%s tries to bandage %s wounds, but just makes the situation worse.\r\n", ch, ch->His_Her( ) );
    }
  }

  obj->Extract( 1 );
  return;
}

void physical_affect( char_data* ch, char_data* victim, int skill, int type, int var )
{
  affect_data affect;
  int          level = ch->get_skill( skill );
  
  affect.type     = type;
  affect.duration = (10+10*level/MAX_SKILL_LEVEL)*5;
  affect.level    = level;
  
  add_affect( victim, &affect );
}  

















