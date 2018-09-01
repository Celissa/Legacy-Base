#include "system.h"

const char*  cook_word [] = { "raw", "cooked", "burnt" };


/*
 *   EAT ROUTINES
 */


bool eats_corpses( char_data* ch )
{
  if( ch->species != NULL )
    return is_set( ch->species->act_flags, ACT_CARNIVORE );

  return( ch->shdata->race == RACE_TROLL );
}


bool vegetarian( char_data* ch )
{
  if( ch->species != NULL )
    return !is_set( ch->species->act_flags, ACT_CARNIVORE );

  return( ch->shdata->race == RACE_ENT );
}
 

void do_eat( char_data* ch, char* argument )
{
  obj_data* obj;
 
  if( ( obj = one_object( ch, argument, "eat", &ch->contents, ch->array ) ) == NULL ) 
    return;

  eat( ch, obj );
}


bool eat( char_data* ch, obj_data* obj )
{
  if( !is_apprentice( ch ) ) {
    if( !is_set( &obj->pIndexData->wear_flags, ITEM_TAKE ) ) {
      send( ch, "You can't pick up %s.\r\n", obj );
      return FALSE;
    }

    if( obj->pIndexData->item_type == ITEM_CORPSE ) {
      if( !eats_corpses( ch ) ) {
        send( ch, "What a disgusting suggestion!\r\n" );
        return FALSE;
      }
      else {
        for( int i = 0; i < obj->contents; i++ ) {
          if( obj->metal( ) || obj->stone( ) ) {
            send( ch, "There is something too crunchy even for your taste within %s.\r\n", obj );
            return FALSE;
          }
        }
      }
    } else if( obj->pIndexData->item_type != ITEM_FOOD ) {
      if( ch->shdata->race == RACE_TROLL ) {
        if( obj->metal( ) || obj->stone( ) ) {
          send( ch, "That is a bit too crunchy even for your taste.\r\n" );
          return FALSE;
        }
      } else {
        send( ch, "That's not edible.\r\n" );
        return FALSE;
      }
    }

    if( vegetarian( ch ) && is_set( &obj->materials, MAT_FLESH ) ) {
      send( ch, "You do not find meat edible.\r\n" );
      return FALSE;
    }

    if( ch->species == NULL && ch->pcdata->condition[COND_FULL] > 40 ) {
      send( ch, "You are too full to eat more.\r\n" );
      return FALSE;
    }
  }

  if( !is_empty( obj->contents ) ) {
    send( ch, "That object is not empty, remove its contents first.\r\n" );
    return FALSE;
  }

  if( execute_use_trigger( ch, obj, OPROG_TRIGGER_USE ) )
    return FALSE;

  switch ( obj->pIndexData->item_type ) {
  case ITEM_SCROLL:
    send( ch, "You rip %s into shreds and happily munch it.\r\nAnd who said real trolls don't eat salad?\r\n", obj );
    fsend_seen( ch, "%s rips %s into shreds and happily munches it.\r\n", ch, obj );
    break;

  case ITEM_CORPSE:
    if( obj != forbidden( obj, ch ) ) {
      fsend( ch, "You feel the gods would be unduly displeased if you ate %s.", obj );
      return FALSE;
    }

    send( ch, "You gleefully pull apart %s and eat it piece by piece.\r\n", obj );
    fsend( *ch->array, "%s pulls apart %s and eats it piece by piece making a disgusting sight.\r\n", ch, obj );
    break;

  case ITEM_FOOD:
//    if( obj->value[3] != 0 && !(ch->save_vs_poison( obj->value[3] ) ) {
    if( obj->value[3] != 0 && number_range( 0, 99 ) > ch->Save_Poison( ) ) { 
      affect_data af;
      send( ch, "You choke and gag.\r\n" );
      send( *ch->array, "%s chokes and gags.\r\n", ch );
      af.type           = AFF_POISON;
      af.duration       = number_range(3,7) * (2 * obj->value[0]);
      af.mlocation[ 0 ] = APPLY_NONE;
      af.mmodifier[ 0 ] = 0;
      af.leech          = NULL; 
      add_affect( ch, &af );
    }

  default:
    send( ch, "You eat %s.\r\n", obj );
    send_seen( ch, "%s eats %s.\r\n",  ch, obj );
    break;
  }

  gain_condition( ch, COND_FULL, obj->pIndexData->item_type != ITEM_FOOD ? obj->weight/100 : obj->value[0] );

  if( execute_use_trigger( ch, obj, OPROG_TRIGGER_AFTER_USE ) )
    return TRUE;

  obj->Extract( 1 );
  return TRUE;
}


/*
 *   COOK ROUTINES
 */


void do_cook( char_data* ch, char* argument )
{
  /*
  obj_data*   obj;
  obj_data*  fire;

  if( *argument == '\0' ) {
    send( "Cook what?\r\n", ch );
    return;
    }

  if( ( obj = get_obj_inv( ch, argument ) ) == NULL ) {
    send( "You do not have that item.\r\n", ch );
    return;
    }

  if( obj->selected != 1 && obj->next_list != NULL ) {
    send( "You can only cook one object at a time.\r\n", ch );
    return;
    }

  for( fire = *ch->array; fire != NULL; fire = fire->next_content )
    if( fire->pIndexData->item_type == ITEM_FIRE )
      break;

  if( fire == NULL ) {
    send( ch,
      "You see nothing in the room with which to cook %s.\r\n", obj );
    return;
    }

  send( ch, "You cook %s over %s.\r\n", obj, fire );

  if( obj->pIndexData->item_type == ITEM_FOOD
    && obj->value[1] >= 0 && obj->value[1] < 2 ) {
    if( obj->number > 1 ) {
      // should use obj->From etc
      obj->number--;
      obj = duplicate( obj );
      obj->value[1]++;
      put_obj( obj, ch );
      }
    else {
      obj->value[1]++;
      }
    }
  */
  return;
}



