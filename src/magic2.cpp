#include "system.h"


/*
 *   CHARACTERS
 */ 


bool target_offensive( char_data* ch, cast_data* cast, char* argument )
{
  char           tmp  [ ONE_LINE ];  
  char_data*  victim;

  if( *argument == '\0' && ( cast->target = opponent( ch ) ) != NULL )
    return TRUE;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( victim = one_character( ch, argument, tmp, ch->array ) ) == NULL )
    return FALSE;

  if( victim == ch ) {
    fsend( ch, "Your anima speaks up nervously and argues persuasively that casting %s at yourself would be fatuous.", spell_table[ cast->spell ].name );
    return FALSE;
  }

  if( !can_kill( ch, victim ) )
    return FALSE;

  cast->target = victim;
  return TRUE;
}


bool target_self_only( char_data* ch, cast_data* cast, char* argument )
{
  if( *argument != '\0' ) {
    fsend( ch, "Casting %s requires no argument as the only possible victim is yourself.", spell_table[ cast->spell ].name );
    return FALSE;
  }

  cast->target = ch;
  return TRUE;
}


bool target_peaceful( char_data* ch, cast_data* cast, char* argument )
{
  char tmp  [ ONE_LINE ];  

  if( *argument == '\0' ) {
    cast->target = ch;
    return TRUE;
  }

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  return( ( cast->target = one_character( ch, argument, tmp, ch->array ) ) != NULL );
}


bool target_peaceful_other( char_data* ch, cast_data* cast, char* argument )
{
  char           tmp  [ ONE_LINE ];  
  char_data*  victim;

  if( *argument == '\0'
    && ( cast->target = opponent( ch ) ) != NULL )
    return TRUE;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( victim = one_character( ch, argument, tmp,
    ch->array ) ) == NULL )
    return FALSE;

  if( ch == victim ) {
    send( ch, "You can't cast %s on yourself.\r\n", spell_table[ cast->spell ].name ); 
    return FALSE;
    }

  cast->target = victim;

  return TRUE;
}


bool target_mob_only( char_data* ch, cast_data* cast, char* argument )
{
  if( *argument == '\0' && ( cast->target = opponent( ch ) ) != NULL ) 
    return TRUE;

  char           tmp  [ ONE_LINE ];  
  char_data*  victim;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( victim = one_character( ch, argument, tmp, ch->array ) ) == NULL )
    return FALSE;

  if( victim->pcdata != NULL ) {
    send( ch, "You can't cast %s on players.\r\n", spell_table[ cast->spell ].name );
    return FALSE;
  }

  cast->target = victim;
  return TRUE;
}


bool target_continent( char_data* ch, cast_data* cast, char* argument )
{
  char           tmp  [ ONE_LINE ];  
  char_data*  victim;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( victim = one_character( ch, argument, tmp, (thing_array*) &player_list, (thing_array*) &ch->followers, (thing_array*) &mob_list ) ) == NULL )
    return FALSE;

  if( victim == ch ) {
    send( ch, "You can't cast that on yourself!\r\n" );
    return FALSE;
  }

  if( !is_apprentice( ch ) ) {
    if( !victim->in_room || !victim->in_room->area || !ch->in_room || !ch->in_room->area || victim->in_room->area->continent != ch->in_room->area->continent ) {
      send( ch, "Your spell cannot cross sea or ocean.\r\n");
      return FALSE;
    }
  }

  cast->target = victim;
  return TRUE;
}


/*
 *   OBJECTS
 */


bool target_replicate( char_data* ch, cast_data* cast, char* argument )
{
  obj_data* obj;

  if( ( obj = one_object( ch, argument, "replicate", &ch->contents ) ) == NULL )
    return FALSE;

  if( !is_empty( obj->contents ) || ( obj->pIndexData->item_type == ITEM_DRINK_CON && obj->value[1] != 0 ) ) {
    include_empty = FALSE;
    send( ch, "The contents of %s would disrupt the spell.\r\n", obj );
    include_empty = TRUE;
    return FALSE;
  }

  if( is_set( obj->extra_flags, OFLAG_MAGIC ) ) {
    send( ch, "You are unable to copy magical items.\r\n" ); 
    return FALSE;
  }

  if( !is_set( obj->pIndexData->extra_flags, OFLAG_REPLICATE ) ) {
    send( ch, "%s is too complex for you to replicate.\r\n", obj );
    return FALSE;
  }

  cast->target = obj;

  return TRUE;
}


bool target_weapon_armor( char_data* ch, cast_data* cast, char* argument ) 
{
  char         tmp  [ ONE_LINE ];  
  obj_data*    obj;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( obj = one_object( ch, argument, tmp, &ch->contents, ch->array ) ) == NULL )
    return FALSE;

  if( obj->pIndexData->item_type != ITEM_WEAPON && obj->pIndexData->item_type != ITEM_ARMOR && obj->pIndexData->item_type != ITEM_SHIELD) {
    send( ch, "You can only cast %s on weapons and armor.\r\n", spell_table[ cast->spell ].name );
    return FALSE;
  }

  cast->target = obj;

  return TRUE;
}


bool target_object( char_data* ch, cast_data* cast, char* argument ) 
{
  char         tmp  [ ONE_LINE ];  
  obj_data*    obj;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( obj = one_object( ch, argument, tmp, &ch->contents, ch->array ) ) == NULL )
    return FALSE;

  cast->target = obj;

  return TRUE;
}


bool target_weapon( char_data* ch, cast_data* cast, char* argument ) 
{
  char         tmp  [ ONE_LINE ];  
  obj_data*    obj;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( obj = one_object( ch, argument, tmp, &ch->contents, ch->array ) ) == NULL )
    return FALSE;

  if( obj->pIndexData->item_type != ITEM_WEAPON ) {
    send( ch, "You can only cast %s on weapons.\r\n", spell_table[ cast->spell ].name );
    return FALSE;
  }

  cast->target = obj;

  return TRUE;
}


bool target_corpse( char_data* ch, cast_data* cast, char* argument )
{
  char         tmp  [ ONE_LINE ];  
  obj_data*    obj;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( obj = one_object( ch, argument, tmp, ch->array ) ) == NULL )
    return FALSE;

  if( obj->pIndexData->item_type != ITEM_CORPSE ) {
    send( ch, "%s can only be cast on corpses.\r\n", spell_table[ cast->spell ].name );
    return FALSE;
  }

  if( obj->pIndexData->item_type != ITEM_CORPSE ) {
    send( ch, "%s may not be cast on the corpses of players or their pets.\r\n", spell_table[ cast->spell ].name );
    return FALSE;
  }

  cast->target = obj;

  return TRUE;
}


bool target_drink_con( char_data* ch, cast_data* cast, char* argument )
{
  char         tmp  [ ONE_LINE ];  
  obj_data*    obj;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( obj = one_object( ch, argument, tmp, &ch->contents, ch->array ) ) == NULL )
    return FALSE;

  if( obj->pIndexData->item_type != ITEM_DRINK_CON ) {
    send( ch, "You can only cast that on a drink container.\r\n" );
    return FALSE;
  }

  cast->target = obj;

  return TRUE;
}


bool target_room( char_data* ch, cast_data* cast, char* argument )
{
  /*
    if( *argument == '\0' ) {
      send( ch, "What location do you want to cast it on?\r\n" );
      }
    else if( atoi( argument ) != 0 ) {
      send( "What type of location is that??\r\n", ch );
      }
    else if( ( room = find_location( ch, argument ) ) == NULL ) {
      send( "Unknown Location.\r\n", ch );
      }
    else {
      cast->room = room;
      return TRUE;
      }
   */
   return FALSE;
}

bool target_annoying( char_data* ch, cast_data* cast, char* argument )
{
   return !in_sanctuary( ch );
}

bool target_recall( char_data* ch, cast_data* cast, char* argument )
{
  if( *argument == '\0' ) {
//    cast->target = ch;
    return TRUE;
  }

  char arg[ MAX_INPUT_LENGTH ];
  argument = one_argument( argument, arg );

  room_data* room = get_town( arg, false );
  if( !room ) {
    send( ch, "You can't recall to that location.\r\n" );
    return FALSE;
  }

  cast->target = room;

//  if( *argument == '\0' ) {
//     cast->target = ch;
//   } 
//   else if( ( cast->target = one_character( ch, argument, "cast on", 
//                   ch->array ) ) == NULL ) {
//     return FALSE;
//   }

  return TRUE;
}

bool target_astral( char_data* ch, cast_data* cast, char* argument )
{
  char arg[ MAX_INPUT_LENGTH ];
  if( ch->fighting != NULL ) {
    send( ch, "You cannot concentrate enough.\r\n" );
    return FALSE;
  }

  if( *argument == '\0' ) {
    send( ch, "At where do you wish to focus the astral gate?\r\n" );
    return FALSE;
  }

  argument = one_argument( argument, arg );
  for( int i = 0; i < MAX_ENTRY_ASTRAL; i++ ) {     
    if( fmatches( arg, astral_table[i].name ) ) {
      room_data *room = get_room_index( astral_table[i].recall );
      if( room && room->area && ch->in_room && ch->in_room->area && ( room->area->continent == ch->in_room->area->continent || is_apprentice( ch ) ) ) {
        cast->target = (thing_data*) room;
        return TRUE;
      } else {
        send( ch, "Your spell cannot cross sea or ocean.\r\n");
        return FALSE;
      }
    }
  }

  send( ch, "Unknown astral focus point '%s'.\r\n", arg );
  return FALSE;
}

bool target_cross_continent( char_data* ch, cast_data* cast, char* argument )
{
  char           tmp  [ ONE_LINE ];  
  char_data*  victim;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( victim = one_character( ch, argument, tmp, (thing_array*) &player_list, (thing_array*) &ch->followers, (thing_array*) &mob_list ) ) == NULL )
    return FALSE;

  if( victim == ch ) {
    send( ch, "You can't cast that on yourself!\r\n" );
    return FALSE;
  }

  if( !victim->in_room || !victim->in_room->area || !ch->in_room || !ch->in_room->area ) {
    send( ch, "Your spell cannot cross sea or ocean.\r\n");
    return FALSE;
  }

  cast->target = victim;
  return TRUE;
}

bool target_universal( char_data* ch, cast_data* cast, char* argument )
{

  char arg[ MAX_INPUT_LENGTH ];
  if( ch->fighting != NULL ) {
    send( ch, "You cannot concentrate enough.\r\n" );
    return FALSE;
  }

  if( *argument == '\0' ) {
    send( ch, "At where do you wish to focus the universal gate?\r\n" );
    return FALSE;
  }

  argument = one_argument( argument, arg );
  for( int i = 0; i < MAX_ENTRY_ASTRAL; i++ ) {     
    if( fmatches( arg, astral_table[i].name ) ) {
      room_data *room = get_room_index( astral_table[i].recall );
      if (room && room->area && ch->in_room && ch->in_room->area ) {
        cast->target = (thing_data*) room;
        return TRUE;
      } else {
        send( ch, "Your spell cannot cross sea or ocean.\r\n");
        return FALSE;
      }
    }
  }

  send( ch, "Unknown universal focus point '%s'.\r\n", arg );

  return FALSE;
}

bool target_tree_gate( char_data* ch, cast_data* cast, char* argument )
{

  char arg[ MAX_INPUT_LENGTH ];
  if( ch->fighting != NULL ) {
    send( ch, "You cannot concentrate enough.\r\n" );
    return FALSE;
  }

  if( *argument == '\0' ) {
    send( ch, "At where point do you wish to awaken the ancient oak?\r\n" );
    return FALSE;
  }

  argument = one_argument( argument, arg );
  for( int i = 0; i < MAX_ENTRY_ASTRAL; i++ ) {     
    if( fmatches( arg, tree_table[i].name ) ) {
      room_data *room = get_room_index( tree_table[i].recall );
      if (room && room->area && ch->in_room && ch->in_room->area ) {
        cast->target = (thing_data*) room;
        return TRUE;
      } else {
        send( ch, "Your spell cannot cross sea or ocean.\r\n");
        return FALSE;
      }
    }
  }

  send( ch, "Unknown ancient oak location: '%s'.\r\n", arg );

  return FALSE;
}

/*
 *   MAIN ROUTINE
 */


bool get_target( char_data* ch, cast_data* cast, char* argument )
{
  int           spell  = cast->spell;
  int            type  = spell_table[spell].type;

  #define parm ch, cast, argument

  switch( type ) {
    case STYPE_REPLICATE      : return target_replicate      ( parm );
    case STYPE_SELF_ONLY      : return target_self_only      ( parm ); 
    case STYPE_PEACEFUL       : return target_peaceful       ( parm ); 
    case STYPE_OFFENSIVE      : return target_offensive      ( parm ); 
    case STYPE_CORPSE         : return target_corpse         ( parm ); 
    case STYPE_DRINK_CON      : return target_drink_con      ( parm ); 
    case STYPE_WEAPON         : return target_weapon         ( parm ); 
    case STYPE_WEAPON_ARMOR   : return target_weapon_armor   ( parm );
    case STYPE_CONTINENT      : return target_continent      ( parm );
    case STYPE_MOB_ONLY       : return target_mob_only       ( parm );
    case STYPE_PEACEFUL_OTHER : return target_peaceful_other ( parm );
    case STYPE_OBJECT         : return target_object         ( parm );
    case STYPE_ROOM           : return target_room           ( parm );
    case STYPE_ANNOYING       : return target_annoying       ( parm );
    case STYPE_RECALL         : return target_recall         ( parm );
    case STYPE_ASTRAL         : return target_astral         ( parm );
    case STYPE_NON_COMBAT_HEALING: return target_peaceful    ( parm );
    case STYPE_CROSS_CONTINENT : return target_cross_continent ( parm );
    case STYPE_UNIVERSAL      : return target_universal      ( parm );
    case STYPE_ANNOYING_GOOD  : return target_annoying       ( parm );
    case STYPE_ANNOYING_EVIL  : return target_annoying       ( parm );
    case STYPE_ANNOYING_UNDEAD : return target_annoying       ( parm );
    case STYPE_TREE:            return target_tree_gate      ( parm );
    default                   : bug( "Get_Target: Unknown spell type." );
  }

  #undef parm
 
  return FALSE;
}


