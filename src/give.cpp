#include "system.h"


bool   give_npc       ( char_data*, char_data*, thing_array* );
bool   give_pet       ( char_data*, char_data*, obj_data* );

/*
 *   GIVE FUNCTIONS
 */


thing_data* given( thing_data* obj, char_data* receiver, thing_data* giver )
{
  obj = obj->From( obj->selected );
  set_owner( (obj_data*) obj, receiver, (char_data*) giver );
  obj->To( receiver );

  return obj;
}


void do_give( char_data* ch, char* argument )
{
  char            arg  [ MAX_STRING_LENGTH ];
  char_data*   victim;
  thing_array*  array;

  if( is_confused_pet( ch ) || newbie_abuse( ch ) ) 
     return;

  if( !two_argument( argument, "to", arg ) ) {
    send( ch, "Give what to whom?\r\n" );
    return;
  }

  if( ( victim = one_character( ch, argument, "give to", ch->array ) ) == NULL )
    return;

  if( ( array = several_things( OBJ_DATA, ch, arg, "give", &ch->contents ) ) == NULL )
    return;

  if( victim->position <= POS_SLEEPING ) {
    send( ch, "%s isn't in a position to be handed items.\r\n", victim );
    return;
  }

  if( give_npc( ch, victim, array ) )
    return;

  thing_array   subset  [ 4 ];
  thing_func*     func  [ 4 ]  = { cursed, heavy, many, given };

  sort_objects( victim, *array, ch, 4, subset, func );

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't let go of" );
  page_priv( ch, &subset[1], "can't lift", victim );
  page_priv( ch, &subset[2], "can't handle", victim );
  page_publ( ch, &subset[3], "give", victim, "to" );

  consolidate( subset[3] );

  delete array;
}


bool give_npc( char_data* ch, char_data* victim, thing_array* array )
{
  mprog_data*       mprog  = NULL;
  obj_data*           obj;
  obj_clss_data* obj_clss;
  int                   i;

  if( victim->pcdata != NULL ) 
    return FALSE;

//  if( array->size != 1 || array->list[0]->selected != 1 ) {
  if( array->size != 1 ) {
    fsend( ch, "%s would be much happier if items were offered one at a time.", victim );
    return TRUE;
  }
  
  if( ( obj = object( array->list[0] ) ) != NULL ) {
    if( is_set( &victim->status, STAT_PET ) ) 
      return give_pet( ch, victim, obj );

    for( mprog = victim->species->mprog; mprog != NULL; mprog = mprog->next ) {
      if( mprog->trigger == MPROG_TRIGGER_GIVE && ( mprog->value == obj->pIndexData->vnum || mprog->value == 0 ) ) {
        i = atoi( mprog->string );
        if( i <= 1 )
          i = 1;
        if( i == 1 && obj->selected != 1 ) {
          send( ch, "%s would be much happer if items were offered one at a time.", victim );
          return TRUE;
        }
        if( i != array->list[0]->selected ) {
          send( ch, "%s requires you hand him %s.\r\n", victim, obj->Seen_Name( ch, i ) );
          return TRUE;
        }
        break;
      }
      if( mprog->trigger == MPROG_TRIGGER_GIVE_TYPE ) {
        obj_clss = get_obj_index( mprog->value );
        if( obj_clss == NULL ) {
          code_bug( "Mob is set to receive a non-existent item." );
          return TRUE;
        }

        if( obj_clss->item_type == obj->pIndexData->item_type )
          break;

//        memory leak
//        object_type = create( obj_clss );
//        if( object_type->pIndexData->item_type == obj->pIndexData->item_type )
 
      }
    } 
  }

  if( mprog == NULL ) {
    fsend( ch, "%s doesn't seem interested in %s.", victim, array->list[0] );
    return TRUE;
  } 

  send( ch, "You give %s to %s.\r\n", obj, victim );
  send( *ch->array, "%s gives %s to %s.\r\n", ch, obj, victim );

  obj = (obj_data*) obj->From( obj->selected );
  set_owner( obj, ch, NULL );
  obj->To( victim );

  var_ch   = ch;
  var_mob  = victim;  
  var_obj  = obj; 
  var_room = ch->in_room;
  execute( mprog );

  return TRUE;
}


bool give_pet( char_data* ch, char_data* victim, obj_data* obj )
{
  fsend( ch, "You offer %s to %s.", obj, victim );
  fsend( *ch->array, "%s offers %s to %s.", ch, obj, victim );

  if( obj->pIndexData->item_type == ITEM_FOOD && eat( victim, obj ) )
    return TRUE;

  if( obj->pIndexData->item_type == ITEM_DRINK_CON && drink( victim, obj ) ) 
    return TRUE;
  
  fsend( ch, "%s inspects it but doesn't seem interested.", victim->He_She( ) );

  return TRUE;
}











