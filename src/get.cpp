#include "system.h"


void  get_obj         ( char_data*, thing_array*, obj_data* = NULL ); 
bool  valid_container ( char_data*, thing_data* );
bool  get_trigger     ( char_data*, obj_data* );


/*
 *   DO_GET ROUTINE
 */


void do_get( char_data* ch, char* argument )
{
  char                 arg  [ MAX_INPUT_LENGTH ];
  thing_array*       array;
  thing_data*        thing;

  if( ch->species != NULL && !is_set( ch->species->act_flags, ACT_CAN_CARRY ) ) {
    if( !is_confused_pet( ch ) )
      send( ch, "You are unable to carry items.\r\n" );
    return; 
  }

  if( *argument == '\0' ) {
    send( ch, "What do you wish to take?\r\n" );
    return;
  }

  if( !two_argument( argument, "from", arg ) ) {
    // Getting objects from the room ...
    if( (array = several_things( OBJ_DATA, ch, argument, "get", ch->array )) == NULL ) 
       return;

    get_obj( ch, array ); // deletes array
    return;
  }

  // Getting objects from a container ... figure out what the container is,
  // then get some objects from it.
  if( ( thing = one_object( ch, argument, "get from", ch->array, &ch->contents, &ch->wearing ) ) == NULL ) 
    return;

  if( !valid_container( ch, thing ) )
    return;

  if( ( array = several_things( OBJ_DATA, ch, arg, "get", &thing->contents ) ) == NULL )
    return;

  get_obj( ch, array, (obj_data*) thing ); // array deleted by get_obj
}


/*
 *   CONTAINERS
 */

thing_data* forbidden( thing_data* thing, char_data* ch, thing_data* )
{
  player_data*   player;
  obj_data*         obj;
  int             ident;

  if( ( obj = object( thing ) ) == NULL )
    return thing;
 
  ident = obj->value[3];

  if( obj->pIndexData->vnum != OBJ_CORPSE_PC || ident == 0 )
    return obj;

  if( ch->pcdata == NULL )
    return NULL;

  if( ch->shdata->level >= LEVEL_APPRENTICE || ch->pcdata->pfile->ident == ident || get_pfile( ident ) == NULL )
    return obj;

  for( int i = 0; i < player_list; i++ ) {
    player = player_list[i];
    if( player->In_Game( ) && player->pcdata->pfile->ident == ident )
      return( can_kill( ch, player, false ) || player->Befriended( ch ) || ( ch->leader == player && is_pet( ch ) ) ? obj : NULL ); 
  }

  return obj;
}


bool valid_container( char_data* ch, thing_data* thing )
{
  obj_data*  container;

  if( thing != forbidden( thing, ch ) ) {
    send( ch, "You can't steal from that corpse.\r\n" );
    return FALSE;
  }
  
  if( ( container = object( thing ) ) == NULL
    || ( container->pIndexData->item_type != ITEM_CONTAINER
    && container->pIndexData->item_type != ITEM_CORPSE
    && container->pIndexData->item_type != ITEM_TABLE 
    && container->pIndexData->item_type != ITEM_KEYRING
    && container->pIndexData->item_type != ITEM_CHARM_BRACELET ) ) {
    send( ch, "%s isn't a container.\r\n", container );
    return FALSE;
  }

  if( container->pIndexData->item_type == ITEM_CONTAINER
    && is_set( &container->value[1], CONT_CLOSED ) ) {
    include_closed = FALSE;
    send( ch, "%s is closed.\r\n", container );
    include_closed = TRUE;
    return FALSE;
  }

  if( is_empty( container->contents ) ) {
    send( ch, "%s contains nothing.\r\n", container );
    return FALSE;
  }

  return TRUE;
}


thing_data* gotten( thing_data* thing, char_data* ch, thing_data* )
{
  thing = thing->From( thing->selected );
  thing->To( ch );

  return thing;
}

thing_data* get_trigger( thing_data* thing, char_data* ch, thing_data* )
{
  obj_data* obj = object( thing );
  if( obj && obj->Is_Valid( ) && ch && ch->Is_Valid( ) ) {
    set_owner( obj, ch, NULL );
    
    // Check for "get" triggers
    if( get_trigger( ch, obj ) == FALSE || !obj || !obj->Is_Valid( ) )
      return NULL;
  }

  return thing;
}

void get_obj( char_data* ch, thing_array* array, obj_data* container )
{
  thing_array   subset  [ 7 ];
  thing_func*     func  [ 7 ]  = { cant_take, forbidden, sat_on, heavy, many, get_trigger, gotten };

  sort_objects( ch, *array, container, 7, subset, func );

  if( container != NULL ) {
    if( container->contents.size == 0 )
      container->contents.size = -1;  
    if( container->array != ch->array )   
      msg_type = MSG_INVENTORY;
  }

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't take" );
  page_priv( ch, &subset[1], "are forbidden from taking" );
  page_priv( ch, &subset[2], "can't unseat" );
  page_priv( ch, &subset[3], "can't lift" );
  page_priv( ch, &subset[4], "can't handle" );
  page_publ( ch, &subset[6], "get", container, "from", terrain_table[ ch->in_room->sector_type ].get_message );

  if( container != NULL && container->contents.size == -1 )
    container->contents.size = 0;  
 
  consolidate( subset[6] );

  delete array;
}


/*
 *   GET TRIGGERS 
 */


bool get_trigger( char_data* ch, obj_data* obj )
{
  if( !obj || !obj->Is_Valid( ) || !ch || !ch->Is_Valid( ) )
    return FALSE;

  // check room
  for( action_data* action = ch->in_room->action; action != NULL; action = action->next ) {
    if( action->trigger == TRIGGER_GET ) {
      var_ch   = ch;
      var_obj  = obj;
      var_room = ch->in_room;
      if( !execute( action ) || !obj || !obj->Is_Valid( ) || !ch || !ch->Is_Valid( ) )
        return FALSE;;
    }
  }

  // check oprogs
  for( oprog_data* oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) {
    if( oprog->trigger == OPROG_TRIGGER_GET ) {
      var_ch   = ch;
      var_obj  = obj; 
      var_room = ch->in_room;
      if( !execute( oprog ) || !obj || !obj->Is_Valid( ) || !ch || !ch->Is_Valid( ) )
        return FALSE;
    }
  }
    
  return TRUE;
}


