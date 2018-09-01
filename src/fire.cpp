#include "system.h"


bool fireball_effect    ( char_data*, char_data*, int );


/*
 *   SMOKE COMMAND
 */


void do_smoke( char_data* ch, char* argument )
{
  obj_data*      pipe;
  obj_data*   tobacco;

  if( *argument == '\0' ) {
    send( ch, "What do you want to smoke?\r\n" );
    return;
  }

  if( ( pipe = one_object( ch, argument, "smoke", &ch->contents ) ) == NULL )
    return;

  if( pipe->pIndexData->item_type == ITEM_TOBACCO ) {
    send( ch, "You need to put that in a pipe to smoke it.\r\n" );
    return;
  }

  if( pipe->pIndexData->item_type != ITEM_PIPE ) {
    send( ch, "That is not an item you can smoke.\r\n" );
    return;
  }

  if( is_empty( pipe->contents ) ) {
    send( ch, "%s contains nothing to smoke.\r\n", pipe );
    return;
  }

  tobacco = object( pipe->contents[0] );

  fsend( ch, "You smoke %s, inhaling the aroma from %s.", pipe, tobacco );
  fsend( *ch->array, "%s smokes %s.", ch, pipe );

  var_container = pipe;
  if( execute_use_trigger( ch, tobacco, OPROG_TRIGGER_USE ) )
    return;

  tobacco->Extract( );
}


/*
 *   IGNITE
 */


void do_ignite( char_data* ch, char* argument )
{
  affect_data  affect;
  obj_data*       obj;
  obj_data*    source  = NULL;
  bool          found  = FALSE;
 
  if( ch->shdata->race == RACE_TROLL ) {
    send( ch, "Due to the natural attraction of flame and troll flesh and the associated\r\nchildhood nightmares burning things is not one of your allowed hobbies.\r\n" );
    return;
    }

  if( ch->shdata->race == RACE_ENT ) {
    send( ch, "Due to the natural attraction of flame and ent flesh and the associated\r\nchildhood nightmares burning things is not one of your allowed hobbies.\r\n" );
    return;
  }

  if( ( obj = one_object( ch, argument, "ignite",
    &ch->wearing, &ch->contents, ch->array ) ) == NULL )
    return;

  if( is_set( obj->extra_flags, OFLAG_BURNING ) ) {
    send( ch, "%s is already burning.\r\n", obj );
    return;
    }

  for( int i = 0; !found && i < *ch->array; i++ )
    if( ( source = object( ch->array->list[i] ) ) != NULL
      && is_set( source->extra_flags, OFLAG_BURNING ) ) 
      found = TRUE;

  for( int i = 0; !found && i < ch->contents; i++ )
    if( ( source = object( ch->contents[i] ) ) != NULL
      && is_set( source->extra_flags, OFLAG_BURNING ) ) 
      found = TRUE;

  if( !found ) {
    send( ch, "You have nothing with which to ignite %s.\r\n", obj );
    return;
    }

  if( obj->vs_fire( ) > 90 ) {
    send( ch, "Depressingly %s doesn't seem inclined to burn.\r\n", obj );
    return;
    }
  
  send( ch, "You ignite %s using %s.\r\n", obj, source );
  send( *ch->array, "%s ignites %s using %s.\r\n", ch, obj, source );
  
  affect.type      = AFF_BURNING;
  affect.duration  = 1;
  affect.level     = 1;
  affect.leech     = NULL;

  add_affect( obj, &affect );
}


/*
 *   FIRE DAMAGE ROUTINES
 */



int obj_data :: vs_fire( )
{
  int save  = 100;
  int    i;

  for( i = 0; i < MAX_MATERIAL; i++ ) 
    if( is_set( &pIndexData->materials, i ) )
      save = min( save, material_table[i].save_fire );

  if( pIndexData->item_type != ITEM_ARMOR 
    || pIndexData->item_type != ITEM_WEAPON
    || pIndexData->item_type != ITEM_SHIELD ) 
    return save;

  return save+value[0]*(100-save)/(value[0]+2);
}

bool affected_cold( char_data* ch )
{
  if( !is_set( ch->affected_by, AFF_FROST_SHIELD ) &&
    !is_set( ch->affected_by, AFF_HOAR_SHIELD ) &&
    !is_set( ch->affected_by, AFF_HOAR_FROST ) &&
    !is_set( ch->affected_by, AFF_ABSOLUTE_ZERO ) )
    return FALSE;

  return TRUE;
}

/* 
 *   FIRE BASED SPELLS
 */


bool spell_resist_fire( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_RESIST_FIRE, AFF_RESIST_FIRE );
  
  return TRUE;
}


bool spell_fire_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( is_submerged( victim ) ) {
    fsend_all( victim->in_room, "The water around %s bubbles briefly.\r\n",
      victim );
    return TRUE;
    }
  
  if( !consenting( victim, ch, "fire shield" ) )
    return FALSE;

  if( affected_cold( victim ) ) {
    send( victim, "The freezing shields protecting you dissipate.\r\n" );
    send_seen( victim, "The freezing shields protecting %s dissipate.\r\n", victim );
    return FALSE;
  }

  spoil_hide( victim );

  spell_affect( ch, victim, level, duration, SPELL_FIRE_SHIELD, AFF_FIRE_SHIELD );

  return TRUE;
}

bool spell_flame_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( is_submerged( victim ) ) {
    fsend_all( victim->in_room, "The water around %s bubbles briefly.\r\n",
      victim );
    return TRUE;
    }
  
  if( !consenting( victim, ch, "shield of flame" ) )
    return FALSE;

  if( affected_cold( victim ) ) {
    send( victim, "The freezing shields protecting you dissipate.\r\n" );
    send_seen( victim, "The freezing shields protecting %s dissipate.\r\n", victim );
    return FALSE;
  }

  spoil_hide( victim );

  spell_affect( ch, victim, level, duration, SPELL_FLAME_SHIELD, AFF_FLAME_SHIELD );

  return TRUE;
}

bool spell_fiery_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( is_submerged( victim ) ) {
    fsend_all( victim->in_room, "The water around %s bubbles briefly.\r\n",
      victim );
    return TRUE;
    }
  
  if( !consenting( victim, ch, "fiery shield" ) )
    return FALSE;

  if( affected_cold( victim ) ) {
    send( victim, "The freezing shields protecting you dissipate.\r\n" );
    send_seen( victim, "The freezing shields protecting %s dissipate.\r\n", victim );
    return FALSE;
  }

  spoil_hide( victim );

  spell_affect( ch, victim, level, duration, SPELL_FIERY_SHIELD, AFF_FIERY_SHIELD );

  return TRUE;
}

bool spell_inferno_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( is_submerged( victim ) ) {
    fsend_all( victim->in_room, "The water around %s bubbles briefly.\r\n",
      victim );
    return TRUE;
    }
  
  if( !consenting( victim, ch, "shield of the inferno" ) )
    return FALSE;

  if( affected_cold( victim ) ) {
    send( victim, "The freezing shields protecting you dissipate.\r\n" );
    send_seen( victim, "The freezing shields protecting %s dissipate.\r\n", victim );
    return FALSE;
  }

  spoil_hide( victim );

  spell_affect( ch, victim, level, duration, SPELL_FIRE_SHIELD, AFF_FIRE_SHIELD );

  return TRUE;
}

bool spell_ignite_weapon( char_data* ch, char_data*, void* vo, int level, int )
{
  affect_data  affect;
  obj_data*       obj  = (obj_data*) vo;

  if( null_caster( ch, SPELL_IGNITE_WEAPON ) )
    return TRUE;

  if( is_set( &obj->pIndexData->materials, MAT_WOOD ) ) {
    fsend( ch, "%s you are carrying bursts into flames which quickly consume it.", obj );
    fsend( *ch->array, "%s %s is carrying bursts into flames which quickly consume it.", obj, ch );
    obj->Extract( 1 );
    return TRUE;
  }

  affect.type      = AFF_FLAMING;
  affect.duration  = level;
  affect.level     = level;
  affect.leech     = NULL;

  add_affect( obj, &affect );

  return TRUE;
}

 
/*
 *   DAMAGE SPELLS
 */

/*
bool spell_burning_hands( char_data* ch, char_data* victim, void*, int level, int )
{
  if( null_caster( ch, SPELL_BURNING_HANDS ) ) 
    return TRUE;

  damage_fire( victim, ch, spell_damage( SPELL_BURNING_HANDS, level ), "*the burst of flame" );

  return TRUE; 
}
*/

bool spell_burning_hands( char_data* ch, char_data* victim, void* vo, int level, int duration )
{
  obj_data*  obj  = (obj_data*) vo;
  int       save;

  /* Quaff */

  if( ch == NULL && obj == NULL ) {
    fsend( victim, "You feel incredible pain as you combust from the liquid you consumed.  Luckily you don't feel it for long." );
    fsend_seen( victim, "%s grasps %s throat and spasms in pain - %s does not survive long.", victim, victim->His_Her( ), victim->He_She( ) );
    death_message( victim );
    death( victim, NULL, "burning to death" );
    return TRUE;
  }  

  /* Fill */

  if( ch == NULL ) {
    if( obj->metal( ) || is_set( &obj->materials, MAT_STONE ) )
      return FALSE;

    fsend( victim, "The fire burns its way through %s, which you quickly drop and watch disappear into nothing.\r\n", obj );
    fsend( *victim->array, "%s quickly drops %s as %s destoryed by the liquid.", victim, obj, obj->selected > 1 ? "they are" : "it is" );

    obj->Extract( obj->selected );
    return TRUE;
  }

  /* Dip */

  if( duration == -3 ) {
    save = obj->vs_fire( );
    if( number_range( 0,99 ) > save ) {
      if( number_range( 0,99 ) > save ) {
        send( *ch->array, "%s is consumed in flames.\r\n", obj );
        obj->Extract( 1 );
        return TRUE;
      }
      send( ch, "%s is partially destroyed.\r\n", obj );
    }
    return TRUE;
  }
  
  /* Throw-Cast */

  damage_fire( victim, ch, spell_damage( SPELL_BURNING_HANDS, level ), "*the burst of flame" );
  return TRUE; 
}

bool spell_flame_strike( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_fire( victim, ch, spell_damage( SPELL_FLAME_STRIKE, level ), "*An incandescent spear of flame" );

  return TRUE;
}



bool spell_conflagration( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_fire( victim, ch, spell_damage( SPELL_CONFLAGRATION, level ), "*A raging inferno" );

  return TRUE;
}


/*
 *   FIREBALL
 */


bool spell_fireball( char_data* ch, char_data* victim, void*, int level, int )
{
  if( null_caster( ch, SPELL_FIREBALL ) )
    return TRUE; 

  if( fireball_effect( ch, victim, level ) )
    return TRUE;

  if( victim->in_room != ch->in_room )
    return TRUE;

  if( victim->mount != NULL ) 
    fireball_effect( ch, victim->mount, level );

  if( victim->rider != NULL )
    fireball_effect( ch, victim->rider, level );

  return TRUE;
}


bool fireball_effect( char_data *ch, char_data *victim, int level )
{
  if( damage_fire( victim, ch, spell_damage( SPELL_FIREBALL,level ), "*The raging fireball" ) )
    return TRUE;

  if( victim->mount != NULL )
    if( number_range( 0, 12*MAX_SKILL_LEVEL/10 ) > victim->get_skill( SKILL_RIDING ) ) {
      send( "The blast throws you from your mount!\r\n", victim );
      fsend_seen( victim, "%s is thrown from %s mount by the blast.", victim, victim->Him_Her() );
      victim->mount->rider = NULL;
      victim->mount = NULL;
      victim->position = POS_RESTING;
      set_delay(victim, 32);
      return FALSE;
    }   

  if( number_range( 0, SIZE_HORSE ) > victim->Size( ) && number_range( 0, 100 ) < 7 ) {
    exit_data *exit = random_movable_exit(victim);
    if (!exit)
      return FALSE;
    
    send( victim, "The blast throws you %s from the room!\r\n", dir_table[exit->direction].name );
    fsend_seen( victim, "The blast throws %s %s from the room!", victim, dir_table[exit->direction].name );

//    room_data *in_room = ch->in_room;
    room_data *to_room = exit->to_room;

    victim->From( );
    victim->To( to_room );

    fsend_seen( victim, "A large explosion flings %s in from %s!", victim, dir_table[ exit->direction ].arrival_msg );

    send( "\r\n", victim );
    do_look( victim, "");
  }

  return FALSE;
}


bool spell_inferno( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_INFERNO, level ), "*the raging inferno", ATT_FIRE );

  return TRUE;
}

/*
 *    Druid Fire Spells
 */

bool spell_summer_touch( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_SUMMER_TOUCH, level ), "*The scorching summer heat", ATT_FIRE );

  return TRUE;
}

bool spell_dancing_tallow( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_DANCING_TALLOW, level ), "*The whip like tendril of fire", ATT_FIRE );

  return TRUE;
}

bool spell_fire_ant( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_FIRE_ANT, level ), "*The fire ants bite", ATT_FIRE );

  return TRUE;
}

bool spell_river_magma( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_RIVER_MAGMA, level ), "*The river of magma", ATT_FIRE );

  return TRUE;
}

bool spell_sunburst( char_data* ch, char_data*, void*, int level, int duration )
{
  char_data* rch;

  if( null_caster( ch, SPELL_SUNBURST ) )
    return TRUE;

  for( int i = *ch->array-1; i >= 0; i-- )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && can_kill( ch, rch, false ) && rch->Seen( ch ) ) {
      damage_element( rch, ch, spell_damage( SPELL_SUNBURST, level ), "*The raging sunburst", ATT_FIRE );
      if( !makes_save( rch, ch, RES_MAGIC, SPELL_SUNBURST, level ) && rch->Can_See( ) )
        spell_affect( ch, rch, level, duration, SPELL_SUNBURST, AFF_BLIND );
    }

  return TRUE;
}

