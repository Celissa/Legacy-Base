#include "system.h"


/*
 *   DRINK ROUTINES
 */

void do_drink( char_data* ch, char* argument )
{
  obj_data*         obj;
  bool      room_source = false;

  if( !strncasecmp( argument, "from ", 5 ) )
    argument += 5;

  if( *argument == '\0' ) {
    if( ( obj = find_type( *ch->array, ITEM_FOUNTAIN ) ) == NULL ) {
      if( terrain_table[ ch->in_room->sector_type ].water_source[0] <= 0 ) {
        send( ch, "Drink what?\r\n" );
        return;
      }
      if( ( obj = create( get_obj_index( terrain_table[ ch->in_room->sector_type ].water_source[0] ) ) ) == NULL ) {
        send( ch, "Drink what?\r\n" );
        return;
      }
      if( obj != NULL )
        room_source = true;
    }
    if( obj != NULL )
      obj->selected = 1;
  } else {
    if( ( obj = one_object( ch, argument, "drink", &ch->contents, ch->array ) ) == NULL )
      return;
  }

  include_empty  = FALSE;
  include_liquid = FALSE;

  if( room_source )
    obj->To( ch->array );

  drink( ch, obj );

  if( room_source )
    obj->Extract( 1 );

  include_empty  = TRUE;
  include_liquid = TRUE;
}


bool drink( char_data* ch, obj_data* obj )
{
  content_array*   where;
  int             amount;
  int             liquid;
  int            alcohol;
  int              spell;

  if( obj->pIndexData->item_type != ITEM_FOUNTAIN && obj->pIndexData->item_type != ITEM_DRINK_CON ) {
    send( ch, "You can't drink from %s.\r\n", obj );
    return FALSE;
  }

  if( obj->value[1] == 0 ) {
    send( ch, "%s is already empty.\r\n", obj );
    return FALSE;
  }

  if( ch->species == NULL && !is_apprentice( ch ) && ch->pcdata->condition[ COND_THIRST ] > 40 ) {
    send( ch, "You do not feel thirsty.\r\n" );
    return FALSE;
  }
 
  if( ( liquid = obj->value[2] ) >= table_max[ TABLE_LIQUID ] || liquid < 0 ) {
    bug( "Do_drink: bad liquid number. (Obj# %d, Liq# %d)", obj->pIndexData->vnum, liquid );
    liquid = obj->value[2] = 0;
  }

  if( execute_use_trigger( ch, obj, OPROG_TRIGGER_USE ) )
    return FALSE;

  amount  = min( number_range( 20, 40 ), ( obj->value[1] == -1 ? 70 : obj->value[1] ) );
  alcohol = amount*liquid_table[liquid].alcohol/10;

  send( ch, "You drink %s%s from %s.\r\n", obj->value[1] == amount ? "the last drops of " : "", liquid_name( obj ), obj );
  send_seen( ch, "%s drinks %s from %s.\r\n", ch, liquid_name( obj ), obj );

  if( ch->pcdata != NULL ) {
    ch->pcdata->condition[ COND_ALCOHOL ] += (alcohol+1)/2;
    gain_drunk( ch, alcohol/2 );
  }

  gain_condition( ch, COND_FULL,   amount*liquid_table[liquid].hunger/10 );
  gain_condition( ch, COND_THIRST, amount*liquid_table[liquid].thirst/10 );

  if( obj->value[1] != -1 ) {
    where = obj->array;
    obj   = (obj_data*) obj->From( 1 );
    if( ( obj->value[1] -= amount ) == 0 )
      remove_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );
    obj->To( where );
  }

  if( execute_use_trigger( ch, obj, OPROG_TRIGGER_AFTER_USE ) )
    return FALSE;

  if( ( spell = liquid_table[liquid].spell ) == -1 ) 
    return TRUE;

  if( spell < SPELL_FIRST || spell >= WEAPON_FIRST ) {
    bug( "Do_drink: Liquid with non-spell skill." );
    return FALSE;
  }

  ( *spell_table[spell-SPELL_FIRST].function )( NULL, ch, NULL, 10, -1 ); 

  return TRUE;
}


/*
 *   FILL 
 */


thing_data* cant_fill( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj  = object( thing );

  return( obj == NULL || obj->pIndexData->item_type != ITEM_DRINK_CON ? NULL : obj );
}


thing_data* already_full( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj  = (obj_data*) thing;

  return( obj->value[1] >= obj->value[0] ? NULL : obj );
}


thing_data* fill( thing_data* t1, char_data* ch, thing_data* t2 )
{
  obj_data*          obj  = (obj_data*) t1;
  obj_data*     fountain  = (obj_data*) t2;
  content_array*   where  = obj->array;
  obj = (obj_data*) obj->From( obj->selected );

  if( obj->value[1] == 0 || obj->value[2] == fountain->value[2] ) 
    obj->value[2] = fountain->value[2];
  else
    obj->value[2] = 9;

  if( fountain->pIndexData->item_type == ITEM_FOUNTAIN || fountain->value[1] == -1 ) {
    obj->value[1] = obj->value[0];
  } else if( fountain->value[1] < obj->selected*( obj->value[0]-obj->value[1] ) ) {
    obj->value[1]     += fountain->value[1]/obj->selected;
    fountain->value[1] = 0;
  } else {
    fountain->value[1] -= obj->selected*(obj->value[0]-obj->value[1]);
    obj->value[1]       = obj->value[0];
  }
  
  obj->To( where );

  return obj;
}


void do_fill( char_data* ch, char* argument )
{
  char                    arg  [ MAX_INPUT_LENGTH ];
  thing_array*          array;
  obj_data*          fountain;
  content_array*        where;
  bool            room_source = false;

  if( count_args( argument ) == 1 ) {
    if( ( fountain = find_type( *ch->array, ITEM_FOUNTAIN ) ) == NULL ) {
      if( terrain_table[ ch->in_room->sector_type ].water_source[0] <= 0 ) {
        send( ch, "You see nothing obvious to fill from.\r\n" );
        return;
      }
      if( ( fountain = create( get_obj_index( terrain_table[ ch->in_room->sector_type ].water_source[0] ) ) ) == NULL ) {
        send( ch, "You see nothing obvious to fill from.\r\n" );
        return;
      }
      if( fountain != NULL )
        room_source = true;
    }
  } else {
    if( !two_argument( argument, "from", arg ) ) {
      send( ch, "Syntax: Fill <object> [from] <container>.\r\n" );
      return;
    }
    
    if( ( fountain = one_object( ch, argument, "fill", &ch->contents, &ch->wearing, ch->array ) ) == NULL ) 
      return;
    
    if( fountain->pIndexData->item_type != ITEM_DRINK_CON && fountain->pIndexData->item_type != ITEM_FOUNTAIN ) {
      send( ch, "%s isn't something you can fill from.\r\n", fountain );
      return;
    }

    if( fountain->pIndexData->item_type == ITEM_DRINK_CON && fountain->value[1] == 0 ) {
      include_empty = FALSE; 
      send( ch, "%s is empty.\r\n", fountain );
      include_empty = TRUE; 
      return;
    }

    argument = arg;
  }

  if( room_source )
    fountain->To( ch->array );

  where    = fountain->array;
  fountain = (obj_data*) fountain->From( 1 );
  fountain->To( where ); 

  if( ( array = several_things( OBJ_DATA, ch, argument, "fill", &ch->contents ) ) == NULL ) 
    return;

  if( fountain->pIndexData->item_type == ITEM_DRINK_CON && fountain->value[0] != -1 
    && ( array->size != 1 || array->list[0]->selected != 1 ) ) {
    send( ch, "Due to the rabid complexity of limited liquid and multiple containers you may\r\nonly fill one container at a time from non-infinite sources.\r\n" );
    return;
  }

  if( execute_use_trigger( ch, fountain, OPROG_TRIGGER_USE ) )
    return;

  thing_array   subset  [ 4 ];
  thing_func*     func  [ 4 ]  = { cant_fill, same, already_full, fill };

  sort_objects( ch, *array, fountain, 4, subset, func );

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't fill" );
  page_priv( ch, &subset[1], "Filling", NULL, "from itself is impossible" );
  page_priv( ch, &subset[2], NULL, NULL, "is already full", "are already full" );
  page_publ( ch, &subset[3], "fill", fountain, "from" );

  if( room_source )
    fountain->Extract( 1 );

  delete array;
}


/*
 *   EMPTY ROUTINES
 */


thing_data* not_container( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = object( thing );

  return( obj->pIndexData->item_type == ITEM_DRINK_CON
    ? obj : NULL );
}


thing_data* already_empty( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = object( thing );

  return( obj->value[1] != 0 ? thing : NULL );
}


thing_data* fountain( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = object( thing );

  return( obj->value[0] != -1 ? thing : NULL );
}


thing_data* empty_con( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj         = object( thing );
  content_array* where  = obj->array;

  obj = (obj_data*) obj->From( obj->selected );
  obj->value[1] = 0;
  obj->To( where );

  return obj;
}


void do_empty( char_data* ch, char* argument )
{
  thing_array*  array;
  thing_array   subset  [ 4 ];
  thing_func*     func  [ 4 ]  = { not_container, already_empty,
                                 fountain, empty_con };

  if( ( array = several_things( OBJ_DATA, ch, argument, "empty", &ch->contents ) ) == NULL ) 
    return;
 
  sort_objects( ch, *array, NULL, 4, subset, func );

  include_empty = FALSE;

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "emptying", NULL, "doesn't make sense" );
  page_priv( ch, &subset[1], NULL, NULL, "is already empty", "are already empty" );
  page_priv( ch, &subset[2], "can't empty" );
  page_publ( ch, &subset[3], "empty" );

  include_empty = TRUE;
  
  consolidate( subset[3] );

  delete array;
}
