#include "system.h"


bool get_trigger    ( char_data*, obj_data* );
bool put_trigger    ( char_data*, obj_data*, obj_data* );
  

/*
 *
 */


obj_data* find_type( thing_array& array, int type )
{
  obj_data* obj;
  for( int i = 0; i < array; i++ )
    if( ( obj = object( array[i] ) ) != NULL
      && obj->pIndexData->item_type == type ) {
      obj->selected = 1;
      obj->shown    = 1;
      return obj;
    }
  return NULL;
}

obj_data* find_vnum_value( thing_array& array, int type, int value0, int value1 )
{
  obj_data* obj;

  for( int i = 0; i < array; i++ )
    if( ( obj = object( array[i] ) ) != NULL
      && obj->pIndexData->item_type == type
      && obj->value[ value0 ] == value1 )
      return obj;

  return NULL;
}
    
obj_data* find_vnum( thing_array& array, int vnum )
{
  obj_data* obj;

  for( int i = 0; i < array; i++ )
    if( ( obj = object( array[i] ) ) != NULL && obj->pIndexData->vnum == vnum )
      return obj;

  return NULL;
}


/*
 *   OBJECT MANLIPULATION ROUTINES
 */

void show_inventory_header( char_data* viewer, char_data* ch )
{
  if( !ch || !viewer || !ch->Is_Valid( ) || !viewer->Is_Valid( ) )
    return;

  char       title_buf[ MAX_STRING_LENGTH ];
  char             tmp[ MAX_STRING_LENGTH ];
  int           burden = ch->get_burden( );
  const char*     name = ch->Name( viewer );

  sprintf( title_buf, "@w+--[ @C%s@n@w ]-", name );
  if( burden > 0 )
    sprintf( tmp, "---[ @R%s@n@w ]--+@n", burden_name[ burden ] );
  else
    sprintf( tmp, "---+@n" );

  for( unsigned int i = cstrlen( title_buf ); i < 80 - cstrlen( tmp ); i++ )
    strcat( title_buf, "-" );
  strcat( title_buf, tmp );

  viewer->pagef( "%s%s\r\n", viewer == ch ? "" : "\r\n", title_buf );
}

bool show_inventory( char_data* viewer, char_data* ch, bool newstyle = false )
{
  if( !ch || !viewer || !ch->Is_Valid( ) || !viewer->Is_Valid( ) )
    return true;

  bool         nothing = TRUE;
  char        long_buf[ MAX_STRING_LENGTH ];
  thing_data*    thing;
  obj_data*        obj;
  int             wght = 0;
  in_character = FALSE;

  //-------------------------------------------------------------------------
  // do the coins first
  for( int i = 0; i < ch->contents; i++ ) {
    thing = ch->contents[ i ];
    if( ( obj = object( thing ) ) != NULL && obj->pIndexData->item_type == ITEM_MONEY ) { 
      obj->selected = 0;
      wght += obj->Weight( );
    } else 
      thing->selected = thing->number;
  }

  rehash_weight( ch, ch->contents );

  int coins = get_money( ch );
  if( viewer == ch || coins != 0 ) {
    if( newstyle ) {
      show_inventory_header( viewer, ch );
      sprintf( long_buf, "Coins: %d =%s", coins, coin_phrase( ch ) );
      viewer->pagef( "%s@e%*s@n\r\n\r\n", long_buf, 80 - strlen( long_buf ), float3( wght/100. ) );
    } else
      page( viewer, "Coins: %d = [%s ]    Weight: %.2f lbs\r\n\r\n", get_money( ch ), coin_phrase( ch ), float( wght/100. ) );
  }

  //-------------------------------------------------------------------------
  // do the items next (include wearing if is a pet)
  if( !newstyle ) {
    page( viewer, "%sItem                          Num   Wgt  Item                          Num   Wgt\r\n", bold_v( viewer ) );
    page( viewer, "----                          ---   ---  ----                          ---   ---%s\r\n", normal( viewer ) );
  }

  int col = 0;
  *long_buf = '\0';

  for( int i = 0; i < ch->contents; i++ ) {
    thing = ch->contents[ i ];
    obj = object( thing );
    if( thing == NULL || thing->shown == 0 )
      continue;

    const char* name = newstyle ? thing->Name( viewer, thing->shown, TRUE ) : thing->Name( );
    if( nothing ) {
      nothing = FALSE;
      if( newstyle && viewer != ch && coins == 0 )
        show_inventory_header( viewer, ch );
    }

    if( strlen( name ) < ( newstyle ? 33 : 30 ) ) {
      if( newstyle )
        viewer->pagef( "%-33s@e%6s@n%s", name, float3( thing->temp/100. ), ++col%2 == 0 ? "\r\n" : "  " );
      else
        page( viewer, "%-30s%3s%6s%s", name, int3( thing->shown ), float3( thing->temp/100. ), ++col%2 == 0 ? "\r\n" : "  " );
    } else {
      if( strlen( long_buf ) > MAX_STRING_LENGTH - 256 ) {
        if( col != 0 )
          page( viewer, "\r\n" );
        if( newstyle )
          viewer->pagef( long_buf );
        else
          page( viewer, long_buf );
        page( viewer, "\r\n" );
        *long_buf = '\0';
      } 
      if( newstyle )
        sprintf( long_buf+strlen( long_buf ), "%-75s@e%5s@n\r\n", name, float3( thing->temp/100. ) );
      else
        sprintf( long_buf+strlen( long_buf ), "%-71s%3s%6s\r\n", name, int3( thing->shown ), float3( thing->temp/100. ) );
    }
  }

  if( col%2 == 1 )
    page( viewer, "\r\n" );

  if( *long_buf != '\0' ) {
    if( col != 0 )
      page( viewer, "\r\n" );
    if( newstyle )
      viewer->pagef( long_buf );
    else
      page( viewer, long_buf );
  }

  if( viewer == ch && nothing ) 
    page( viewer, "< empty >\r\n" ); 

  //-------------------------------------------------------------------------
  // and the summary

  if( !newstyle ) {
    int burden = ch->get_burden( );

    page( viewer, "\r\n  Carried: %6.2f lbs   (%s%s%s)\r\n", float( ch->contents.weight/100. ), color_scale( ch, burden ), burden_name[ burden ], normal( ch ) );
    page( viewer, "     Worn: %6.2f lbs\r\n", float( ch->wearing.weight/100. ) );
    page( viewer, "   Number: %6d       (max %d)\r\n", ch->contents.number, ch->can_carry_n( ) );
  
  } else if( viewer == ch ) {
    page( viewer, "\r\n  Carried: %6.2f lbs\r\n", float( ch->contents.weight/100. ) );
    page( viewer, "     Worn: %6.2f lbs\r\n", float( ch->wearing.weight/100. ) );
    page( viewer, "   Number: %d/%d\r\n", ch->contents.number, ch->can_carry_n( ) );
  }
  in_character = TRUE;
  return nothing;
}


void do_inventory( char_data* ch, char* argument )
{
  if( !ch || is_confused_pet( ch ) )
    return;

  if( ch->species != NULL && !is_set( ch->species->act_flags, ACT_CAN_CARRY ) && get_trust( ch ) < LEVEL_APPRENTICE ) {
    send( ch, "You are unable to carry items.\r\n" );
    return;
  }

  if( ch->shifted != NULL && !is_apprentice( ch ) ) {
    send( ch, "You are currently unable to carry items.\r\n" );
    return;
  }

  int     flags = 0;
  bool newstyle = ch->pcdata ? is_set( ch->pcdata->pfile->flags, PLR_PET_INVENTORY ) : false;
  bool showpets = ch->pcdata ? is_set( ch->pcdata->pfile->flags, PLR_PET_INVENTORY ) : false;

  if( !get_flags( ch, argument, &flags, "pon", "Inventory" ) )
    return;

  if( is_set( &flags, 0 ) ) {
    newstyle = true;
    showpets = true;
  } else if( is_set( &flags, 1 ) ) {
    newstyle = false;
    showpets = false;
  } else if( is_set( &flags, 2 ) ) {
    newstyle = true;
    showpets = false;
  }
  show_inventory( ch, ch, newstyle );

  if( !showpets )
    return;

  if( is_demigod( ch ) ) {
    // demigods get everyone's inventory mwhahaha
    for( int i = 0; i < ch->in_room->contents.size; i++ ) {
      char_data* pet = character( ch->in_room->contents.list[ i ] );
      if( pet && pet != ch )
        show_inventory( ch, pet, newstyle );
    }

  } else {
    // new pet inventory
    for( int i = 0; i < ch->followers.size; i++ ) {
      char_data* pet = ch->followers.list[ i ];
      if( pet->in_room == ch->in_room && is_pet( pet ) ) {
        pet->selected = 1;
        pet->shown    = 1;
        show_inventory( ch, pet, newstyle );
      }
    }
  }
}


/*
 *   JUNK ROUTINE
 */

thing_data* junk( thing_data* thing, char_data*, thing_data* )
{
  return thing;
}


void execute_junk( event_data* event )
{
  player_data* pc = (player_data*) event->owner;

  extract( pc->junked );
  event->owner->events -= event;

  delete event;
}
  

void do_junk( char_data* ch, char* argument )
{
  thing_array*  array;
  thing_array   subset  [ 3 ];
  thing_func*     func  [ 3 ]  = { cursed, no_junk, junk };
  player_data*      pc         = player( ch );
  event_data*    event;
  obj_data*        obj;

  page_priv( ch, NULL, empty_string );

  if( !strcasecmp( argument, "undo" ) ) {
    if( pc == NULL ) {
      send( ch, "Only player may junk undo.\r\n" );
      return;
    }

    if( is_empty( pc->junked ) ) {
      fsend( ch, "There are no objects you can unjunk." );
      return;
    }

    /*
    fpage( ch, junk_undo_msg );
    page( ch, "\r\n" );
    */

    stop_events( ch, execute_junk );

    for( int i = pc->junked-1; i >= 0; i-- ) {
      obj_data *obj = object(pc->junked[i]);
      if (obj && obj->unjunked++ < 2)
        subset[0] += obj;
      else
        subset[1] += obj;
    }

    page_priv( ch, &subset[1], "can't retrieve" );
    page_publ( ch, &subset[0], "unjunk" );

    for( int i = 0; i < subset[0]; i++ ) {
      obj = (obj_data*) subset[0][i];
      obj = (obj_data*) obj->From( obj->number );
      obj->To( &ch->contents );
    }

    extract( subset[1] );
    return;
  }

  if( ( array = several_things( OBJ_DATA, ch, argument, "junk", &ch->contents ) ) == NULL )
    return;

  sort_objects( ch, *array, NULL, 3, subset, func );

  page_priv( ch, &subset[0], "can't let go of" );
  page_priv( ch, &subset[1], "can't junk" );
  page_publ( ch, &subset[2], "junk" );

  if( !is_empty( subset[2] ) ) {
    if( pc != NULL ) {
      stop_events( ch, execute_junk );
      extract( pc->junked );
      for( int i = 0; i < subset[2]; i++ ) {
        obj = (obj_data*) subset[2][i];
        obj = (obj_data*) obj->From( obj->selected );
        obj->To( &pc->junked );
      }

      event = new event_data( execute_junk, ch );
      add_queue( event, 2 * PULSE_TICK );
    } else 
      extract( subset[2] );
  }

  delete array;
}


/*
 *   DROP ROUTINES
 */


thing_data* drop( thing_data* thing, char_data* ch, thing_data* )
{
  obj_data* obj;

  thing = thing->From( thing->selected );

  if( ( obj = object( thing ) ) != NULL )
    set_owner( obj, NULL, ch );

  thing->To( ch->array );

  return thing;
}


void do_drop( char_data* ch, char* argument )
{
  thing_array*  array;
  thing_array   subset  [ 2 ];
  thing_func*     func  [ 2 ]  = { cursed, drop };

  if( newbie_abuse( ch ) )
    return; 

  if( ( array = several_things( OBJ_DATA, ch, argument, "drop", &ch->contents ) ) == NULL ) 
    return;
 
  sort_objects( ch, *array, NULL, 2, subset, func );

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't let go of" );
  page_publ( ch, &subset[1], "drop", NULL, terrain_table[ ch->in_room->sector_type ].drop_message );

  consolidate( subset[1] );

  delete array;
}


/*
 *   Inventory Utils
 */ 

bool is_container( obj_data* obj )
{
  if( !obj || !obj->Is_Valid( ) )
    return false;

  switch( obj->pIndexData->item_type ) {
  case ITEM_DRINK_CON:
  case ITEM_SPELLBOOK:
  case ITEM_CHARM_BRACELET:
  case ITEM_KEYRING:
  case ITEM_CONTAINER:
  case ITEM_CORPSE:
  case ITEM_TABLE:
    return true;
  }

  return false;
}

obj_data *get_bag( char_data *ch, int slot )
{
  if ( !ch->Wearing( slot ) )
    return NULL;

  for( int j = 0; j < MAX_LAYER; j++ ) {
    obj_data *temp = ch->Wearing( slot, j );
    if( temp && temp->pIndexData->item_type == ITEM_CONTAINER )
      return temp;
  }

  return NULL;
}

obj_data *get_cross( char_data *ch, int slot )
{
  if( !ch->Wearing( slot ) )
    return NULL;

  for( int j = 0; j < MAX_LAYER; j++ ) {
    obj_data *temp = ch->Wearing( slot, j );
    if( temp && temp->pIndexData->item_type == ITEM_CROSS )
      return temp;
  }

  return NULL;
}

obj_data *get_weapon( char_data *ch, int slot )
{
  if( !ch->Wearing( slot ) )
    return NULL;

  for( int j = 0; j < MAX_LAYER; j++ ) {
    obj_data *temp = ch->Wearing( slot, j );
    if( temp && temp->pIndexData->item_type == ITEM_WEAPON )
      return temp;
  }

  return NULL;
}

obj_data *get_shield( char_data *ch, int slot )
{
  if (!ch->Wearing(slot))
    return NULL;

  for( int j = 0; j < MAX_LAYER; j++ ) {
    obj_data *temp = ch->Wearing( slot, j );
    if( temp && ( temp->pIndexData->item_type == ITEM_SHIELD ) ) // || temp->pIndexData->item_type == ITEM_ARMOR))
      return temp;
  }

  return NULL;
}






