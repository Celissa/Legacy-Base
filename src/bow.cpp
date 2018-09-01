#include "system.h"

obj_data*    search_arrow    ( char_data*, obj_data*, int );

void do_shoot( char_data *ch, char *argument )
{
/*
  char              arg  [ MAX_STRING_LENGTH ];
  char              buf  [ MAX_STRING_LENGTH ];
  char_data*     victim;
  exit_data*       exit;
  obj_data*       wield;
  obj_data*       arrow;
  room_data*       room  = ch->in_room;
  int                 i;
  int            length;
  int               dex;
  int              roll;
  int            chance;
  int            damage;
  const char*      word;

  if( ( ( wield = get_weapon( ch, WEAR_HELD_R)) == NULL 
    || wield->value[3] != WEAPON_BOW-WEAPON_FIRST ) {
    send( ch, "You aren't wielding a ranged weapon.\r\n" );
    return;
    } 

  for( int i = 0; i < ch->contents; i++ )  
    if( arrow->pIndexData->item_type == ITEM_ARROW )
      break;

  if( arrow == NULL ) {
    send( "You don't have any ammunition for your weapon.\r\n", ch );
    return;
    }

  argument = one_argument( argument, arg );
  length = strlen( arg );

  if( arg[0] == '\0' ) {
    send( "Which direction do you want to shoot?\r\n", ch );
    return;
    }

  for( i = 0; i < MAX_DOOR; i++ )
    if( !strncasecmp( arg, dir_table[i].name, length ) )
      break;
  
  if( i == MAX_DOOR ) {
    send( "That direction makes no sense!\r\n", ch );
    return;
    }

  if( ( exit = room->exit[i] ) == NULL
    || IS_SET( exit->exit_info, EX_CLOSED ) ) {
    send( "That would be shooting into a wall!?.\r\n", ch );
    return;
    }

  if( argument[0] == '\0' ) {
    send( "Who do you want to shoot at?\r\n", ch );
    return;
    }
  
  ch->in_room = room->exit[i]->to_room;
  victim = one_character( ch, argument, "shoot", ch->array );
  ch->in_room = room;
 
  if( victim == NULL ) {
    send( "You don't see them there ...\r\n", ch );
    return;
    }  

  roll   = number_range( -150, 150 ); 
  dex    = victim->Dexterity( ); 
  chance = -victim->mod_armor+ch->shdata->level
    +5*get_hitroll( ch, wield );

  if( chance < roll ) {
    sprintf( buf, "$p comes flying in from the %s, barely missing $n.",
      dir_table[i].name );
    act( buf, victim, arrow, NULL, TO_ROOM );
    sprintf( buf, "$p comes flying in from the %s, barely missing you.",
      dir_table[i].name );
    act_to( buf, victim, arrow, NULL, victim );
    sprintf( buf, "$n shoots $p %s barely missing $N.", dir_table[i].name );
    act( buf, ch, arrow, victim, TO_ROOM );
    sprintf( buf, "You shoot $p %s barely missing $N.", dir_table[i].name );
    act_to( buf, ch, arrow, victim, ch );
    return;
    }

  damage  = roll_dice( arrow->value[0], arrow->value[1] );
  word    = lookup( physical_index, damage );

  sprintf( buf, "You shoot $p %s.", dir_table[i].name );
  act_to( buf, ch, arrow, NULL, ch );

  sprintf( buf, "$n shoots $p %s.", dir_table[i].name );
  act( buf, ch, arrow, NULL, TO_ROOM );

  sprintf( buf, "$p comes flying in from the %s.", dir_table[i].name );
  act( buf, victim, arrow, NULL, TO_ALL );

  sprintf( buf, "$p %s $n.", word );
  act( buf, victim, arrow, NULL, TO_ROOM );

  room = victim->in_room;
  victim->in_room = ch->in_room;
  act( buf, victim, arrow, NULL, TO_ALL );
  victim->in_room = room;
 
  send( victim, "%s %s you.", arrow, word );

  extract( arrow, 1 );
  return;
*/
}

obj_data* find_ammo( char_data* ch, obj_data* weapon )
{
  //search through contents for an arrow.
  int ammo = 0;

  if( weapon == NULL ) {
    send( ch, "You have a null weapon?!?\r\n" );
    return NULL;
  }
  
  if( weapon->value[3] == WEAPON_BOW-WEAPON_FIRST )
    ammo = ITEM_ARROW;

  if( weapon->value[3] == WEAPON_SLING-WEAPON_FIRST )
    ammo = ITEM_STONE;

  if( weapon->value[3] == WEAPON_CROSSBOW-WEAPON_FIRST )
    ammo = ITEM_BOLT;

  for( int i = 0; i < ch->contents; i++ ) {
    obj_data* obj = object( ch->contents[i] );
    if( obj != NULL && obj->Is_Valid( ) && obj->pIndexData->item_type == ammo
      && can_use( ch, obj->pIndexData, obj ) )
    return obj;
  }

  //search through wearing for an arrow (including bags)

  for( int i = 0; i < ch->wearing; i++ ) {
    obj_data* obj = search_arrow( ch, (obj_data*) ch->wearing[i], ammo );
    if( obj != NULL && obj->Is_Valid( ) && can_use( ch, obj->pIndexData, obj ) ) 
      return obj;
  }
  
  //search through inventory again (including bags)
  
  for( int i = 0; i < ch->contents; i++ ) {
    obj_data* obj = search_arrow( ch, (obj_data*) ch->contents[i], ammo );
    if( obj != NULL && obj->Is_Valid( ) && can_use( ch, obj->pIndexData, obj ) )
      return obj;
  }

  return NULL;
}

obj_data* search_arrow( char_data* ch, obj_data* obj, int ammo )
{
  // sanity check, don't want invalid objects
  if( obj == NULL || !obj->Is_Valid( ) )
    return NULL;
  
  if( obj->pIndexData->item_type == ammo ) {
  // this is the arrow (unlikely)
    return obj;
  } 
  else if( obj->pIndexData->item_type == ITEM_CONTAINER ) {
  // search container for arrows
    for( int k = 0; k < obj->contents; k++ ) {
      obj_data* obj2 = object( obj->contents[k] );

      //sanity no invalid objects
      if( obj2 == NULL || !obj2->Is_Valid( ) )
        continue;

      if( obj2->pIndexData->item_type == ammo )
        return obj2;
    }
  }
  
  return NULL;
}

