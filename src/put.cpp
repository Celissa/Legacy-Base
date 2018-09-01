#include "system.h"


void put_in         ( char_data*, thing_array*, obj_data* );
void put_obj_on     ( char_data*, char_data*, obj_data* );
void put_obj_on     ( char_data*, obj_data*,  obj_data* );
void wear_obj       ( char_data*, obj_data*, char_data* );

thing_data* cantwear( thing_data*, char_data*, thing_data* ); // in wear.cc

/*
 *   DO_PUT ROUTINE
 */


void do_put( char_data* ch, char* argument )
{
  char               arg  [ MAX_STRING_LENGTH ];
  thing_array*     array;
  obj_data*    container;
  char_data*      victim;
  obj_data*          obj = NULL ;

  if( contains_word( argument, "on", arg ) ) {
    if( ( victim = one_character( ch, argument, "put", ch->array ) ) != NULL ) {
      if( ( obj = one_object( ch, arg, "put", &ch->contents ) ) == NULL ) {
        send( ch, "You aren't holding anything like that.\r\n" );
      } else {
        put_obj_on( ch, victim, obj );
      }
    }
    return;
  }

  if( !two_argument( argument, "in", arg ) ) {
    send( ch, "Syntax: put <obj> [in] <container>.\r\n" );
    return;
  }

  if( ( container = one_object( ch, argument, "put in", &ch->wearing, &ch->contents, ch->array ) ) == NULL )
    return;

  if( ( array = several_things( OBJ_DATA, ch, arg, "put", &ch->contents ) ) == NULL )
    return;

  /* PIPES */

  if( container->pIndexData->item_type == ITEM_PIPE ) {
    obj = object((*array)[0]);
    if ( obj == NULL ) {
      send( ch, "%s is not something you wish to smoke.\r\n", array[0] );
    }
    if( obj->selected != 1 || array->size > 1 ) {
      send( ch, "You can only put one item in a pipe at a time.\r\n" );
    } else if( container->contents.size > 0 ) {
      send( ch, "%s already contains %s.\r\n", container, container->contents.list[0] );
    } else if( obj->pIndexData->item_type != ITEM_TOBACCO ) {
      send( ch, "%s is not something you wish to smoke.\r\n", obj );
    } else {
      put_in( ch, array, container );
    }
    return;
  }    

  if( container->pIndexData->item_type == ITEM_KEYRING ) { 
    obj = object((*array)[0]);
    if( obj == NULL ) {
      send( ch, "%s is not something you can put on a keyring.\r\n",
      array[0] );
    }
    if( obj->selected != 1 || array->size > 1 ) {
      send( ch, "You can only put one key on a keyring at a time.\r\n" );
    } else if( container->contents.size > container->value[0] ) {
      send( ch, "The keyring holds as many keys as it can.\r\n" );
    } else if( obj->pIndexData->item_type != ITEM_KEY ) {
      send( ch, "%s is not something you can put on a keyring.\r\n", obj );
    } else {
      put_in( ch, array, container );
    }
    return;
  }

  if( container->pIndexData->item_type == ITEM_CHARM_BRACELET ) {
    obj = object((*array)[0]);
    if( obj == NULL ) {
      send( ch, "%s is not something you can put on a charm bracelet.\r\n",
      array[0] );
    }
    if( obj->selected != 1 || array->size > 1 ) {
      send( ch, "You can only put one charm on a charm bracelet at a time.\r\n" );
    } else if( container->contents.size > container->value[0] ) {
      send( ch, "The charm bracelet holds as many charms as it can.\r\n" );
    } else if( obj->pIndexData->item_type != ITEM_CHARM ) {
      send( ch, "%s is not something you can put on a charm bracelet.\r\n", obj );
    } else {
      put_in( ch, array, container );
    }
    return;
  }
  /* BAGS & KEYRINGS */

  if( container->pIndexData->item_type != ITEM_CONTAINER
    && container->pIndexData->item_type != ITEM_KEYRING
    && container->pIndexData->item_type != ITEM_CHARM_BRACELET ) {
    fsend( ch, "You cannot put items into %s.", container );
    return;
  }

  if( is_set( &container->value[1], CONT_CLOSED) ) {
    send( ch, "%s is closed.\r\n", container );
    return;
  }

  // The container must be a singular (number == 1) object.  If it isn't,
  // split off a singular container and put stuff in that.
  if( container->number > 1 && container->array != NULL ) {
    obj_data* obj = object( container->From( 1 ) );
    if( !obj || !obj->Is_Valid() ) {
      roach( "do_put: put succeeded but container object not created for %s", ch );
      return;
    }

    // reversed put_in so the consolidate didn't junk the previous bagz0r
    // hopefully won't cause side effects (seems to work ok)
    put_in( ch, array, obj );
    
    if( obj->array == NULL ) {
      obj->To( container->array );
      consolidate( obj );
    }

  } else {
    put_in( ch, array, container );
  }
}


/*
 *   BAGS 
 */

thing_data* wont_fit( thing_data* thing, char_data*, thing_data* container )
{
  int  n  = thing->Weight( thing->number );
  int  m  = container->Capacity( );

  // If thing is a TOBACCO object, and container is a PIPE object, 
  // just return thing.  Then put() will do the right thing.
  obj_data* obj_item = object(thing);
  obj_data* obj_container = object(container);
  if ( obj_item != NULL && obj_container != NULL )
     if ( ( obj_item->pIndexData->item_type == ITEM_TOBACCO &&
          obj_container->pIndexData->item_type == ITEM_PIPE ) ||
       ( obj_item->pIndexData->item_type == ITEM_KEY && 
         obj_container->pIndexData->item_type == ITEM_KEYRING ) ||
       ( obj_item->pIndexData->item_type == ITEM_CHARM && 
         obj_container->pIndexData->item_type == ITEM_CHARM_BRACELET ) )
        return thing;

  if( m >= n || n <= 0 )
    return thing;
  
  if( ( m = thing->selected*m/n ) <= 0 )
    return NULL;

  content_array* where = thing->array;

  thing = thing->From( m );
  thing->To( where );

  return thing;
}


thing_data* put( thing_data* thing, char_data* ch, thing_data* container )
{
  thing = thing->From( thing->selected );

  if ( container->array != NULL ) {
     char_data* container_holder = character( container->array->where );
     if( ch != container_holder ) {
        set_owner( (obj_data*) thing, NULL, ch );
     }
  }

  thing->To( container );

  return thing;
}

thing_array* check_put_progs( char_data* ch, thing_array& array, obj_data* container ) {
  
  oprog_data* oprog;
  thing_data* thing;
  obj_data* obj;
  thing_array* rarray = new thing_array;
  int sel;
  bool blocked = FALSE;
  
  //run put prog from container on each object put in
  for( int i = 0; i < array; i++ ) {
     thing = array[i];
     obj = object(thing); 
     for( int j = 0; j < obj->number && j < obj->selected; j++ ) {  
       blocked = FALSE;
       for( oprog = container->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) {
          if( oprog->trigger == OPROG_TRIGGER_PUT ) {
            var_room   = ch->in_room;
            var_ch     = ch;
            var_obj    = container;
      var_container = container;
      var_robj   = obj;
      //change selected to make message display singular
      sel = obj->selected;
      obj->selected = 1;
            if( !execute( oprog ) ) {
        obj->selected = sel;
        blocked = TRUE;
        // prog succeded prevent obj from being put
        if( obj != NULL ) {
    obj->selected--;
    j--;
        }
      }
      else obj->selected = sel;
      break;
    }
       }  
       if( !blocked ) {
    // check item going in for put prog
    for( oprog = obj->pIndexData->oprog; oprog != NULL;
      oprog = oprog->next ) {
      if( oprog->trigger == OPROG_TRIGGER_PUT ) {
    var_room = ch->in_room;
    var_ch = ch;
    var_obj = obj;
    var_robj = container;
    var_container = container;
    sel = obj->selected;
    obj->selected = 1;
    if( !execute( oprog ) ) {
      obj->selected = sel;
      if( obj != NULL ) {
        obj->selected--;
        j--;
      }
    }
    else
      obj->selected = sel;
          break;
       }
     }
         }
     }
  }
  // remove things that no longer want to be put
  for( int i =0; i < array; i++ ) {
    thing = array[i];
    if( thing != NULL && thing->selected > 0 ) {
    *rarray += thing;
    }
  }
  
  return rarray;
}
void put_in( char_data* ch, thing_array* array, obj_data* container )
{
  thing_array    subset  [ 5 ];
  thing_func*      func  [ 5 ]  = { same, cursed, wont_fit, cant_take, put };
  
  array = check_put_progs(ch, *array, container);

  sort_objects( ch, *array, container, 5, subset, func );

  msg_type = MSG_INVENTORY;

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't fold", NULL, "into itself" );
  page_priv( ch, &subset[1], "can't let go of" );
  page_priv( ch, &subset[2], "doesn't have room for", container );
  page_priv( ch, &subset[3], "can't put", container, "into", container->array == ch->array ? " on the ground" : empty_string );
  page_publ( ch, &subset[4], "put", container, "into", container->array == ch->array ? " on the ground" : empty_string );

  consolidate( subset[4] );

  delete array;
}


/*
 *   HORSES
 */


void put_obj_on( char_data* ch, char_data* victim, obj_data* obj )
{
  if( !is_demigod( ch ) && ( victim->leader != ch || !is_set( &victim->status, STAT_PET ) ) ) {
    send( ch, "%s isn't a pet of yours.\r\n", victim );
    return;
  }

  // CAPABILITY LOST:  putting multiple objects on a horse at once.
  wear_obj( victim, obj, ch );
}


void wear_obj( char_data* victim, obj_data* obj, char_data* ch )
{
  thing_data* thing_worn = cantwear( obj, victim, NULL );
  if ( thing_worn == NULL ) { // wear slot full or not valid wear slot
    send( ch, "%s can't wear %s.\r\n", victim, obj );
    return;
  }

  thing_worn = needremove( obj, victim, NULL );
  if ( thing_worn == NULL ) {
    // layering problem
    send( ch, "You need to remove something from %s to equip %s.\r\n", victim, obj );
    return;
  }

  // Object is worn.  Figure out where it went and say so.
  if ( can_wear( obj, ITEM_WEAR_HORSE_BACK ) ) {
    send( ch, "You place %s on the back of %s.\r\n", obj, victim );
    send_seen( ch, "%s places %s on the back of %s.\r\n", ch, obj, victim ); 
  } else if ( can_wear( obj, ITEM_WEAR_HORSE_BODY ) ) {
    send( ch, "You put %s on %s, protecting %s body.\r\n", obj, victim, victim->His_Her( ) );
    send_seen( ch, "%s puts %s on %s, protecting %s body.\r\n", ch, obj, victim, victim->His_Her( ) ); 
  } else if ( can_wear( obj, ITEM_WEAR_HORSE_FEET ) ) {
    send( ch, "You place %s on the feet of %s.\r\n", obj, victim );
    send_seen( ch, "%s puts %s on the feet of %s.\r\n", ch, obj, victim ); 
  } else {
    send( ch, "You place %s on %s.\r\n", obj, victim );
    send( victim, "%s places %s on you.\r\n", ch, obj );
    send_seen( ch, "%s puts %s on %s.\r\n", ch, obj, victim ); 
  }

  return;
}


/*
 *   TABLES
 */


void put_obj_on( char_data* ch, obj_data* container, list_data* list )
{
  /*
  obj_data*   obj;
  int           i;

  if( container->pIndexData->item_type != ITEM_TABLE ) {
    send( "You can only place items on tables.\r\n", ch );
    return;
    }

  for( i = 0; i < list->length; i++ ) {
    obj = (obj_data*) list->pntr[i];
    if( obj->array != ch )
      continue;
    if( container->where != ch->in_room && container->where != ch )
      break;
    if( !obj->droppable( ) ) {
      send( ch, "You can't let go of %s.\r\n", obj );
      continue;
      }
    act_to( "You place $p on $P.", ch, obj, container, ch );
    act_notchar( "$n places $p on $P.", ch, obj, container );
    obj = remove( obj, obj->selected );
    put_obj( obj, container );
    }
  
  */
  return;
}

