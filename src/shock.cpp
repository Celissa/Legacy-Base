#include "system.h"


// Saving Throw for Objects vs Shock

int obj_data :: vs_shock( )
{
  int i;
  int save = 100;

  for( i = 0; i < MAX_MATERIAL; i++ ) {
    if( is_set( &pIndexData->materials, i ) )
      save = min( save, material_table[i].save_shock );
  }

  if( pIndexData->item_type != ITEM_ARMOR
    || pIndexData->item_type != ITEM_WEAPON
    || pIndexData->item_type != ITEM_SHIELD )
    return save;
  
  return save+value[0]*(100-save)/(value[0]+2);
}


void water_shock( char_data* ch, int spell, int level )
{
  char_data* rch;

  if( !ch || !ch->Is_Valid( ) || !ch->array )
    return;

  for( int i = *ch->array - 1; i >= 0; i-- )
    if( ch && ch->Is_Valid( ) && ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && rch->Is_Valid( ) && can_kill( ch, rch, false ) )
      damage_shock( rch, ch && ch->Is_Valid( ) ? ch : NULL, 2*spell_damage( spell, level ), "*The water shock" );

  if( ch && ch->Is_Valid( ) )
    damage_shock( ch, ch, 2*spell_damage( spell, level ), "*The water shock" );
    
/*
  for( int i = 0; i < *ch->array; i++ )
    if( ( victim = character( ch->array->list[i] ) ) != NULL && can_kill( ch, victim, false ) )
      damage_shock( victim, ch, 2*spell_damage( spell, level ), "*The water shock" );
*/
    
  return;
}


bool affected_acid( char_data* ch )
{
  if( !is_set( ch->affected_by, AFF_EROSION_SHIELD ) &&
    !is_set( ch->affected_by, AFF_CORROSIVE_SHIELD ) &&
    !is_set( ch->affected_by, AFF_CAUSTIC_SHIELD ) &&
    !is_set( ch->affected_by, AFF_THORN_SHIELD ) )
    return FALSE;

  return TRUE;
}


/* 
 *   ELECTRICITY BASED SPELLS
 */


bool spell_resist_shock( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_RESIST_SHOCK, AFF_RESIST_SHOCK );
  
  return TRUE;
}

bool spell_lightning_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "lightning shield" ) )
    return TRUE;

  if( affected_acid( victim ) ) {
    send( victim, "The shield of lightning fails to surround you.\r\n" );
    send_seen( victim, "The shield of lightning fails to surround %s.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_LIGHTNING_SHIELD, AFF_LIGHTNING_SHIELD );

  return TRUE;
}

bool spell_shock_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "lightning shield" ) )
    return TRUE;

  if( affected_acid( victim ) ) {
    send( victim, "The shock shield fails to surround you.\r\n" );
    send_seen( victim, "The shock shield fails to surround %s.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_SHOCK_SHIELD, AFF_SHOCK_SHIELD );

  return TRUE;
}

bool spell_spark_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "lightning shield" ) )
    return TRUE;

  if( affected_acid( victim ) ) {
    send( victim, "The spark shield fails to surround you.\r\n" );
    send_seen( victim, "The spark shield fails to surround %s.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_SPARK_SHIELD, AFF_SPARK_SHIELD );

  return TRUE;
}

bool spell_ion_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "lightning shield" ) )
    return TRUE;

  if( affected_acid( victim ) ) {
    send( victim, "The ion shield fails to surround you.\r\n" );
    send_seen( victim, "The ion shield fails to surround %s.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_ION_SHIELD, AFF_ION_SHIELD );

  return TRUE;
}

bool spell_call_lightning( char_data* ch, char_data* victim, void*, int level, int )
{
  if( terrain_table[ ch->in_room->sector_type ].underwater ) {
    water_shock( ch, SPELL_CALL_LIGHTNING, level );
    return TRUE;
    }

  damage_shock( victim, ch, spell_damage( SPELL_CALL_LIGHTNING, level ), "*A bolt of lightning" );
  return TRUE;
}


bool spell_lightning_bolt( char_data* ch, char_data* victim, void*, int level, int )
{
  if( terrain_table[ ch->in_room->sector_type ].underwater ) {
    water_shock( ch, SPELL_LIGHTNING_BOLT, level );
    return TRUE;
    }

  damage_shock( victim, ch, spell_damage( SPELL_LIGHTNING_BOLT, level ), "*The brilliant bolt of lightning" );

  return TRUE;
}


bool spell_chain_lightning( char_data* ch, char_data* victim, void*, int level, int )
{
  room_data*  room;

  if( null_caster( ch, SPELL_CHAIN_LIGHTNING ) ) 
    return TRUE;

  room = ch->in_room;

  if( terrain_table[ room->sector_type ].underwater ) {
    water_shock( ch, SPELL_CHAIN_LIGHTNING, level );
    return TRUE;
  }
 
  for( ; victim != NULL; ) {
    damage_shock( victim, ch, spell_damage( SPELL_CHAIN_LIGHTNING, level ), "*The bifurcating lightning bolt" );
    if( victim->fighting == NULL )
      react_attack( ch, victim );

    if( number_range( 0, 3 ) == 0 || ( victim = random_pers( room ) ) == ch || !can_kill( ch, victim, false ) || !victim->Seen( ch ) )
      break;
  }

  return TRUE;
}


bool spell_alter_magnetism( char_data* ch, char_data* victim, void*, int level, int )
{
  if( terrain_table[ ch->in_room->sector_type ].underwater ) {
    water_shock( ch, SPELL_ALTER_MAGNETISM, level );
    return TRUE;
  }

  damage_shock( victim, ch, spell_damage( SPELL_ALTER_MAGNETISM, level ), "*The turbulent forces", TRUE );

  return TRUE;
}

bool spell_shock ( char_data* ch, char_data* victim, void* vo, int level, int duration )
{
  obj_data* obj = (obj_data*) vo;
  int save;

  /* Quaff/Drinking */

  if( ch == NULL && obj == NULL ) {
    fsend( victim, "You feel incredible pain, as lightning shoots through your body, causing the explosion of major internal organs.  Luckily the pain does not last long." );
    fsend_seen( victim, "%s spasms in pain, sparks fly off %s body - %s doesn't survive long.", victim, victim->His_Her( ), victim->He_She( ) );
    death_message( victim );
    death( victim, NULL, "shocked to death" );
    return TRUE;
  }

  /* Fill */

  if( ch == NULL ) {
    if( !obj->metal( ) )
      return FALSE;

    fsend( victim, "The electrified liquid melts %s into nothing, the remaining charge disperses.", obj );
    fsend( *victim->array, "%s quickly drops %s as it melts into nothing.", victim, obj );

    obj->Extract( obj->selected );
    return TRUE;
  }

  /* Dip */

  if( duration == -3 ) {
    save = obj->vs_shock( );
    if( number_range( 0, 99 ) > save ) {
      if( number_range( 0, 99 ) > save ) {
        send( *ch->array, "%s melts from the contact with the electrified liquid.\r\n", obj );
        obj->Extract( 1 );
        return TRUE;
      }
      send( ch, "%s is partically destroyed by the electrified liquid.\r\n", obj );
    }
    return TRUE;
  }

  /* Throw-Cast */

  if( terrain_table[ ch->in_room->sector_type ].underwater ) {
    water_shock( ch, SPELL_SHOCK, level );
    return TRUE;
  }

  damage_shock( victim, ch, spell_damage( SPELL_SHOCK, level ), "*The blue arcs of energy", TRUE );
  return TRUE;
}

bool spell_static_charge( char_data* ch, char_data* victim, void*, int level, int )
{
  if( terrain_table[ ch->in_room->sector_type ].underwater ) {
    water_shock( ch, SPELL_STATIC_CHARGE, level );
    return TRUE;
  }

  damage_element( victim, ch, spell_damage( SPELL_STATIC_CHARGE, level ), "*The static discharge", ATT_SHOCK );

  return TRUE;
}


/*
 *    Druid Shock Spells
 */

bool spell_harness_winds( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_HARNESS_WINDS, level ), "*The striking lightning of autumn", ATT_SHOCK );

  return TRUE;
}

bool spell_electric_eel( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_TOUCH_ELECTRIC_EEL, level ), "*The touch of the electric eel", ATT_SHOCK );

  return TRUE;
}

bool spell_electrical_storm( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_ELECTRIC_STORM, level ), "*The raining electrical storm", ATT_SHOCK );

  return TRUE;
}

bool spell_deep_energy( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_DEEP_ENERGY, level ), "*The drawn energy from Gaia", ATT_SHOCK );

  return TRUE;
}

