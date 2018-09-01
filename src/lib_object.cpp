#include "system.h"

void* code_is_wearing( void** argument )
{
  int        vnum = (int)        argument[0];
  char_data*   ch = (char_data*) argument[1];

  if( ch == NULL ) {
    code_bug( "Is_Wearing: NULL character." );
    return NULL;
  }

  obj_data* obj = find_vnum( ch->wearing, vnum );

  return obj;
}

void* code_wearing( void** argument )
{
  char_data* ch = (char_data*) argument[ 0 ];
  int      slot = (int) argument[ 1 ];
  int     layer = (int) argument[ 2 ];

  if( ch == NULL ) {
    code_bug( "Wearing: NULL character." );
    return NULL;
  }

  obj_data *obj = ch->Wearing( slot, layer );

  return obj;
}

void* code_obj_to_char( void** argument )
{
  obj_data*   obj  = (obj_data*)  argument[0];
  char_data*   ch  = (char_data*) argument[1];
  int           i  = (int)        argument[2];

  if( obj == NULL || ch == NULL )
    return NULL;

  if( obj->array ) {
    obj = (obj_data*) obj->From( i == 0 ? obj->number : i );
    if( obj == NULL )
      return NULL;
  }

  obj->To( ch );

  consolidate( obj );

  return obj;
}
   
void* code_obj_to_cont( void** argument )
{
  obj_data*   obj  = (obj_data*)  argument[0];
  obj_data*  cont  = (obj_data*)  argument[1];
  int           i  = (int)        argument[2];

  if( obj == NULL || cont == NULL )
    return NULL;

  if( !is_container( cont ) ) {
    code_bug( "Code_obj_to_cont: Attempting to put an object into something that's not a container!" );
    return NULL;
  }

  if( i > obj->number ) {
    code_bug( "Code_obj_to_cont: i > number" );
    return NULL;
  }

  if( obj->array ) {
    obj = (obj_data*) obj->From( i == 0 ? obj->number : i );
    if( obj == NULL )
      return NULL;
  }

  obj->To( cont );

  consolidate( obj );

  return obj;
}

void* code_obj_to_room( void** argument )
{
  obj_data*    obj  = (obj_data*)  argument[0];
  room_data*  room  = (room_data*) argument[1];
  int            i  = (int)        argument[2];

  if( obj == NULL || room == NULL )
    return NULL;

  if( obj->array ) {
    obj = (obj_data*) obj->From( i == 0 ? obj->number : i );
    if( obj == NULL )
      return NULL;
  }

  obj->To( room );
  
  consolidate( obj );

  return NULL;
}

void* code_coin_value( void** argument )
{
  obj_data*  obj  = (obj_data*) argument[0];

  if( obj == NULL || obj->pIndexData->item_type != ITEM_MONEY )  
    return NULL;

  return (void*) ( obj->number * obj->pIndexData->cost );
}

void* code_weapon_type( void** argument )
{
  char_data*      ch = (char_data*) argument[0];
  int           type = (int) argument[1];
  int         value0 = (int) argument[2];
  int         value1 = (int) argument[3];

  if( ch == NULL ) {
    code_bug( "Weapon_Type:  NULL character." );
    return NULL;
  }

  if( type < 0 || type > MAX_ITEM ) {
    code_bug( "Weapon_Type:  Undefined Item Type." );
    return NULL;
  }

  if( value0 < 0 || value0 > 3 ) {
    code_bug( "Weapon_Type: No such value number exists." );
    return NULL;
  }

  obj_data* obj = find_vnum_value( ch->contents, type, value0, value1 );
  if( obj == NULL )
    obj = find_vnum_value( ch->wearing, type, value0, value1 );

  return obj;
}

void* code_has_obj( void** argument )
{
  int        vnum  = (int)        argument[0];
  char_data*   ch  = (char_data*) argument[1];

  if( ch == NULL ) {
    code_bug( "Has_Obj: NULL character." );
    return NULL;
  }
 
  obj_data* obj = find_vnum( ch->contents, vnum );
  if( obj == NULL )
    obj = find_vnum( ch->wearing, vnum );

  return obj;
}

void* code_has_type( void** argument )
{
  int        type = (int)        argument[0];
  char_data*   ch = (char_data*) argument[1];

  if( ch == NULL ) {
    code_bug( "Has_Type: NULL character." );
    return NULL;
  }

  if( type < 0 || type > MAX_ITEM ) {
    code_bug( "Has_Type:  Item type undefined." );
    return NULL;
  }

  obj_data* obj = find_type( ch->contents, type );
  if( obj == NULL ) 
    obj = find_type( ch->wearing, type );

  return obj;
}

void* code_obj_index( void** argument )
{
  obj_data*   obj  = (obj_data*) argument[0];

  code_bug( "Switch this to obj_stat( )" );

  if( obj == NULL ) {
    code_bug( "Code_Obj_Index: NULL object." );
    return (void*) 0;
  }

  return (void*) obj->pIndexData->vnum;
}

void* code_obj_in_thing( void** argument )
{
  int          vnum = (int)         argument[0];
  thing_data* thing = (thing_data*) argument[1];

  if( thing == NULL ) {
    code_bug( "Obj_in_thing: NULL thing." );
    return NULL;
  }

  return find_vnum( thing->contents, vnum );
}

void* code_object_type( void** argument )
{
  obj_data* obj = (obj_data*) argument[0];

  if( obj == NULL ) {
    code_bug( "NULL obj in object_type" );
    return (void*) 0;
  }

  return (void*) obj->pIndexData->item_type;
}

void* code_object_in_obj( void** argument )
{
  obj_data*    obj = (obj_data*) argument[0];
  int            i = (int) argument[1];

  if( obj == NULL ) {
    code_bug( "NULL container in obj_in_obj" );
    return NULL;
  }

  if( !is_container( obj ) ) {
    code_bug( "Object in obj_in_obj is not a container." );
    return NULL;
  }

  if( is_empty( obj->contents ) )
    return NULL;

  if( i < 0 || i >= obj->contents ) {
    code_bug( "i is a negative number, or too big for objects contents." );
    return NULL;
  }

  return (void*) obj->contents[i];
}


void* code_oload( void** argument )
{
  int       vnum  = (int) argument[0];
  int          i  = (int) argument[1];
  obj_data*  obj;

  if( i == 0 )
    i = 1;

  if( ( obj = create( get_obj_index( vnum ), i ) ) != NULL ) 
    set_alloy( obj, 10 );

  return obj;
}

void* code_obj_stat( void** argument )
{
  obj_data*    obj = (obj_data*) argument[0];
  int            i = (int) argument[1];

  if( obj == NULL ) {
    code_bug( "Obj_Stat: Null Object." );
    return NULL;
  }

  if( i < 0 || i > 17 ) {
    code_bug( "Obj_Stat: Value out of range." );
    return NULL;
  }

  int value [] = { obj->pIndexData->level, obj->condition, obj->pIndexData->durability,
    obj->Cost( ), obj->value[0], obj->value[1], obj->value[2], obj->value[3],
    obj->rust, obj->pIndexData->repair, obj->Light( ), obj->age, obj->pIndexData->vnum,
    obj->number*obj->pIndexData->cost, obj->contents.size, obj->number, obj->source != NULL ? 1 : 0,
    coin_value_in_condition( obj ) };

  return (void*) value[i];
}

void* code_modify_obj( void** argument )
{
  obj_data*      obj = (obj_data*) argument[0];
  int              i = (int) argument[1];
  int              j = (int) argument[2];

  if( obj == NULL ) {
    code_bug( "Modify_Obj: Null Object." );
    return NULL;
  }

  if( i < 0 || i > 17 ) {
    code_bug( "Modify_Obj: Value out of range." );
    return NULL;
  }

  if( j == 0 ) {
    code_bug( "Modify_Obj: Modify by 0?" );
    return NULL;
  }

  if( i == 0 || i == 2 || i == 3 || i == 9 || i == 10 || i == 12 || i == 13 ||
    i == 14 || i == 15 || i == 16 || i == 17 )
    return NULL;

  if( i == 1 )
    obj->condition += j;

  if( i == 4 )
    obj->value[0] += j;

  if( i == 5 )
    obj->value[1] += j;

  if( i == 6 )
    obj->value[2] += j;

  if( i == 7 )
    obj->value[3] += j;

  if( i == 8 )
    obj->rust += j;

  if( i == 11 )
    obj->age += j;

  return NULL;
}

void* code_obj_value( void** argument )
{
  obj_data*  obj  = (obj_data*) argument[0];
  int          i  = (int)       argument[1];  

  code_bug( "Switch this to obj_stat( )" );

  if( obj == NULL ) {
    code_bug( "Obj_Value: Null object." );
    return NULL;
  }

  if( i < 0 || i > 3 ) {
    code_bug( "Obj_Value: Value out of range." );
    return NULL;
  }

  return (void*) obj->value[ i ];
}


void* code_junk_obj( void** argument )
{
  obj_data*     obj  = (obj_data*) argument[0];
  int             i  = (int)       argument[1];  

  if( obj == NULL ) {
    code_bug( "Junk_Obj: Null object." );
    return NULL;
  }

  if( i == 0 )
    obj->Extract( );
  else 
  obj->Extract( i );

  return NULL;
}


void* code_open_object( void** argument )
{
  obj_data* obj = (obj_data*) argument[0];

  if( obj == NULL ) {
    code_bug( "Open_Object: Null object." );
    return NULL;
    }

  if( obj->pIndexData->item_type != ITEM_CONTAINER ) 
    return NULL;
 
  if( !is_set( &obj->value[1], CONT_CLOSEABLE ) ) 
    return NULL;
  
  if( is_set( &obj->value[1], CONT_LOCKED ) ) 
    return NULL;

  remove_bit( &obj->value[1], CONT_CLOSED );
  
  return NULL;
}

void* code_close_object( void** argument )
{
  obj_data* obj = (obj_data*) argument[0];

  if( obj == NULL ) {
    code_bug( "Close_Object: Null object." );
    return NULL;
    }

  if( obj->pIndexData->item_type != ITEM_CONTAINER ) 
    return NULL;
 
  if( is_set( &obj->value[1], CONT_CLOSEABLE ) ) 
    return NULL;
  
  set_bit( &obj->value[1], CONT_CLOSED );
  
  return NULL;
}

void* code_lock_object( void** argument )
{
  obj_data* obj = (obj_data*) argument[0];

  if( obj == NULL ) {
    code_bug( "Lock_Object: Null Object." );
    return NULL;
    }

  if( !is_set( &obj->value[1], CONT_CLOSEABLE ) ) 
    return NULL;

  if( !is_set( &obj->value[1], CONT_CLOSED ) )
    set_bit( &obj->value[1], CONT_CLOSED );

  set_bit( &obj->value[1], CONT_LOCKED );

  return NULL;
}

void* code_unlock_object( void** argument )
{
  obj_data* obj = (obj_data*) argument[0];

  if( obj == NULL ) {
    code_bug( "Lock_Object: Null Object." );
    return NULL;
    }

  if( !is_set( &obj->value[1], CONT_CLOSEABLE ) ) 
    return NULL;

  if( !is_set( &obj->value[1], CONT_CLOSED ) )
    return NULL;

  remove_bit( &obj->value[1], CONT_LOCKED );

  return NULL;
}
