#include "system.h"


int obj_data :: vs_cold( )
{
  int save  = 100;
  int    i;

  for( i = 0; i < MAX_MATERIAL; i++ ) 
    if( is_set( &pIndexData->materials, i ) )
      save = min( save, material_table[i].save_cold );

  if( pIndexData->item_type != ITEM_ARMOR 
    || pIndexData->item_type != ITEM_WEAPON 
    || pIndexData->item_type != ITEM_SHIELD ) 
    return save;

  return save+value[0]*(100-save)/(value[0]+2);
}

bool affected_flames( char_data* ch )
{
  if( !is_set( ch->affected_by, AFF_FLAME_SHIELD ) &&
    !is_set( ch->affected_by, AFF_FIRE_SHIELD ) &&
    !is_set( ch->affected_by, AFF_FIERY_SHIELD ) &&
    !is_set( ch->affected_by, AFF_INFERNO_SHIELD ) )
    return FALSE;


  return TRUE;
}

/*
 *   COLD SPELLS
 */


bool spell_resist_cold( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_RESIST_COLD, AFF_RESIST_COLD );

  return TRUE;
}

bool spell_chilling_touch( char_data* ch, char_data* victim, void* vo, int level, int duration )
{
  obj_data* obj = (obj_data*) vo;
  int save;

  /* Quaff/Drinking */

  if( ch == NULL && obj == NULL ) {
    fsend( victim, "You feel incredible pain, as your insides unnaturally freeze from the liquid you consumed.  Luckily the pain does not last for long." );
    fsend_seen( victim, "%s turns blue in coloration, and spasms in pain - %s does not survive long.", victim, victim->He_She( ) );
    death_message( victim );
    death( victim, NULL, "freezing to death" );
    return TRUE;
  }

  /* Fill */

  if( ch == NULL ) {
    if( obj->metal( ) || is_set( &obj->materials, MAT_STONE ) )
      return FALSE;

    fsend( victim, "The freezing cold liquid solidifies %s, which you quickly drop and it shatters on impact." );
    fsend( *victim->array, "%s quickly drops %s, which shatters on impact.", victim, obj );

    obj->Extract( obj->selected );
    return TRUE;
  }

  /* Dip */

  if( duration == -3 ) {
    save = obj->vs_cold( );
    if( number_range( 0, 99 ) > save ) {
      if( number_range( 0, 99 ) > save ) {
        send( *ch->array, "%s shatters from the contact with the freezing cold.\r\n", obj );
        obj->Extract( 1 );
        return TRUE;
      }
      send( ch, "%s is partially destroyed by the cold.\r\n", obj );
    }
    return TRUE;
  }

  /* Throw-Cast */

  damage_element( victim, ch, spell_damage( SPELL_CHILLING_TOUCH, level ), "*the touch of ice", ATT_COLD );
  return TRUE;
}

bool spell_freeze( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_cold( victim, ch, spell_damage( SPELL_FREEZE, level ), "*the sphere of cold" );

  return TRUE; 
}


bool spell_ice_storm( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_cold( victim, ch, spell_damage( SPELL_ICE_STORM, level ), "*the icy blast" );

  return TRUE; 
}

bool spell_ice_lance( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_cold( victim, ch, spell_damage( SPELL_ICE_LANCE, level ), "*the frigid pierce" );

  return TRUE;
}


bool spell_hoar_frost( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "hoar frost" ) )
    return TRUE;

  if( affected_flames( victim ) ) {
    send( victim, "The hoar frost fails to form around you.\r\n" );
    send_seen( victim, "The flames protecting %s prevent the formation of hoar frost.\r\n", victim );
    return TRUE;
  }

  if( is_submerged( victim ) ) {
    send( ch, "The hoar frost encases you in a solid block of ice.\r\n" );
    send_seen( ch, "The water around %s turns to an encasing solid ice.\r\n", victim );
    damage_cold( ch, NULL, spell_damage( SPELL_HOAR_FROST, 0, 0 ), "The encasing ice" );
    return TRUE;
    }

  spell_affect( ch, victim, level, duration, SPELL_HOAR_FROST, AFF_HOAR_FROST );

  return TRUE;
}

bool spell_frost_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "frost shield" ) )
    return TRUE;

  if( affected_flames( victim ) ) {
    send( victim, "The frost fails to form around you.\r\n" );
    send_seen( victim, "The flames protecting %s prevent the formation of frost.\r\n", victim );
    return TRUE;
  }

  if( is_submerged( victim ) ) {
    send( ch, "The frost shield encases you in a solid block of ice.\r\n" );
    send_seen( ch, "The water around %s turns to an encasing solid ice.\r\n", victim );
    damage_cold( ch, NULL, spell_damage( SPELL_FROST_SHIELD, 0, 0 ), "The encasing ice" );
    return TRUE;
    }

  spell_affect( ch, victim, level, duration, SPELL_FROST_SHIELD, AFF_FROST_SHIELD );

  return TRUE;
}

bool spell_hoar_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "hoar shield" ) )
    return TRUE;

  if( affected_flames( victim ) ) {
    send( victim, "The hoar is prevented from forming by the flames protecting you.\r\n" );
    send_seen( victim, "The flames protecting %s prevents the formation of the hoar.\r\n", victim );
    return TRUE;
  }

  if( is_submerged( victim ) ) {
    send( ch, "The hoar shield encases you in a solid block of ice.\r\n" );
    send_seen( ch, "The water around %s turns to an encasing solid ice.\r\n", victim );
    damage_cold( ch, NULL, spell_damage( SPELL_FROST_SHIELD, 0, 0 ), "The encasing ice" );
    return TRUE;
    }

  spell_affect( ch, victim, level, duration, SPELL_HOAR_SHIELD, AFF_HOAR_SHIELD );

  return TRUE;
}

bool spell_absolute_zero( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "absolute zero" ) )
    return TRUE;

  if( affected_flames( victim ) ) {
    send( victim, "The absolute freezing is prevented from forming by the flames protecting you.\r\n" );
    send_seen( victim, "The flames protecting %s prevent the forming of the absolute freezing.\r\n", victim );
    return TRUE;
  }

  if( is_submerged( victim ) ) {
    send( ch, "The shield of absolute zero encases you in a solid block of ice.\r\n" );
    send_seen( ch, "The water around %s turns to an encasing solid ice.\r\n", victim );
    damage_cold( ch, NULL, spell_damage( SPELL_ABSOLUTE_ZERO, 0, 0 ), "The encasing ice" );
    return TRUE;
    }

  spell_affect( ch, victim, level, duration, SPELL_ABSOLUTE_ZERO, AFF_ABSOLUTE_ZERO );

  return TRUE;
}

bool spell_solid( char_data* ch, char_data* victim, void*, int level, int duration )
{
  damage_element( victim, ch, spell_damage( SPELL_SOLID, level ), "*the encasing ice", ATT_COLD );
  
  if( victim->hit >= 0 )
    spell_affect( ch, victim, level, duration, SPELL_SOLID, AFF_SLOW );

  return TRUE;
}

/*
 *    Druid Cold Spells
 */

bool spell_winters_gale( char_data* ch, char_data* victim, void*, int level, int )
{
  ch->move -= spell_damage( SPELL_WINTERS_GALE, level );
  ch->move = max( 0, ch->move );

  damage_element( victim, ch, spell_damage( SPELL_WINTERS_GALE, level ), "*The howling ice and wind", ATT_COLD );

  return TRUE;
}

bool spell_winters_touch( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_WINTERS_TOUCH, level ), "*The winters first frost", ATT_COLD );

  return TRUE;
}

bool spell_hail_stones( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_HAIL_STONES, level ), "*The frosty hail stones", ATT_COLD );

  return TRUE;
}

bool spell_breath_serpent( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_BREATH_SERPENT, level ), "*The ice serpents breath", ATT_COLD );

  return TRUE;
}
