#include "system.h"


/*
 *   ACID DAMAGE ROUTINES
 */




int obj_data :: vs_acid( )
{
  int save  = 100;
  int    i;

  for( i = 0; i < MAX_MATERIAL; i++ ) 
    if( is_set( &pIndexData->materials, i ) )
      save = min( save, material_table[i].save_acid );

  if( pIndexData->item_type != ITEM_ARMOR 
    || pIndexData->item_type != ITEM_WEAPON
    || pIndexData->item_type != ITEM_SHIELD ) 
    return save;

  return save+value[0]*(100-save)/(value[0]+2);
}

bool affected_shock( char_data* ch )
{
  if( !is_set( ch->affected_by, AFF_ION_SHIELD ) &&
    !is_set( ch->affected_by, AFF_SPARK_SHIELD ) &&
    !is_set( ch->affected_by, AFF_SHOCK_SHIELD ) &&
    !is_set( ch->affected_by, AFF_LIGHTNING_SHIELD ) )
    return FALSE;

  return TRUE;
}

/* 
 *   ACID BASED SPELLS
 */


bool spell_resist_acid( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_RESIST_ACID, AFF_RESIST_ACID );
  
  return TRUE;
}


bool spell_acid_blast( char_data* ch, char_data* victim, void* vo, int level, int duration )
{
  obj_data*  obj  = (obj_data*) vo;
  int       save;

  /* Quaff */

  if( ch == NULL && obj == NULL ) {
    fsend( victim, "You feel incredible pain as the acid eats away at your stomach and throat.  Luckily you don't feel it for long." );
    fsend_seen( victim, "%s grasps %s throat and spasms in pain - %s does not survive long.", victim, victim->His_Her( ), victim->He_She( ) );
    death_message( victim );
    death( victim, NULL, "drinking acid" );
    return TRUE;
  }  

  /* Fill */

  if( ch == NULL ) {
    if( obj->metal( ) || is_set( &obj->materials, MAT_STONE ) || is_set( &obj->materials, MAT_GLASS ) )
      return FALSE;

    fsend( victim, "The acid bubbles and boils, eating its way through %s, which you quickly drop and watch disappear into nothing.\r\n", obj );
    fsend( *victim->array, "%s quickly drops %s as %s dissolved by the liquid.", victim, obj, obj->selected > 1 ? "they are" : "it is" );

    obj->Extract( obj->selected );
    return TRUE;
  }

  /* Dip */

  if( duration == -3 ) {
    save = obj->vs_acid( );
    if( number_range( 0,99 ) > save ) {
      if( number_range( 0,99 ) > save ) {
        send( *ch->array, "%s is devoured.\r\n", obj );
        obj->Extract( 1 );
        return TRUE;
      }
      send( ch, "%s is partially destroyed.\r\n", obj );
    }
    if( obj->rust > 0 ) {
      send( ch, "%sthe %s on %s is removed.\r\n", 
        obj->rust-- > 1 ? "Some of " : "", "rust", obj );
    }
    return TRUE;
  }
  
  /* Throw-Cast */

  damage_acid( victim, ch, spell_damage( SPELL_ACID_BLAST, level ), "*the splatter of acid" );
  return TRUE; 
}


bool spell_acid_storm( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_acid( victim, ch, spell_damage( SPELL_ACID_STORM, level ), "*the blast of acid" );

  return TRUE;
}

bool spell_acid_arrow( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_acid( victim, ch, spell_damage( SPELL_ACID_ARROW, level ), "*the acidic arrow" );

  return TRUE;
}

bool spell_horrid_wilting( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_acid( victim, ch, spell_damage( SPELL_HORRID_WILTING, level ), "*the horrific acid" );

  return TRUE;
}

bool spell_withering_blast( char_data* ch, char_data* victim, void*, int level, int duration )
{
  damage_element( victim, ch, spell_damage( SPELL_WITHERING_BLAST, level ), "*the acidic blast", ATT_ACID );

  if( victim->hit >= 0 )
    spell_affect( ch, victim, level, duration, SPELL_WITHERING_BLAST, AFF_ACID_DEATH );
 
  return TRUE;
}


bool spell_erosion_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "shield of erosion" ) )
    return TRUE;

  if( affected_shock( victim ) ) {
    send( victim, "The roaring electricity prevents the acid from surrounding you.\r\n" );
    send_seen( victim, "The roaring electricity protecting %s stops the acid.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_EROSION_SHIELD, AFF_EROSION_SHIELD );

  return TRUE;
}

bool spell_corrosive_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "shield of corrosion" ) )
    return TRUE;

  if( affected_shock( victim ) ) {
    send( victim, "The roaring electricity prevents the acid from surrounding you.\r\n" );
    send_seen( victim, "The roaring electricity protecting %s stops the acid.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_CORROSIVE_SHIELD, AFF_CORROSIVE_SHIELD );

  return TRUE;
}

bool spell_caustic_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "caustic shield" ) )
    return TRUE;

  if( affected_shock( victim ) ) {
    send( victim, "The roaring electricity prevents the acid from surrounding you.\r\n" );
    send_seen( victim, "The roaring electricity protecting %s stops the acid.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_CAUSTIC_SHIELD, AFF_CAUSTIC_SHIELD );

  return TRUE;
}

/*
 *    Druid Acid Spells
 */

bool spell_autumn_touch( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_AUTUMN_TOUCH, level ), "*The erosion of autumn", ATT_ACID );

  return TRUE;
}

bool spell_frog_tongue( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_FROG_TONGUE, level ), "*The frog's acidic tongue", ATT_ACID );

  return TRUE;
}

bool spell_acidic_water( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_ACIDIC_WATER, level ), "*The bubbling acidic water", ATT_ACID );

  return TRUE;
}

bool spell_acid_rain( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_ACID_RAIN, level ), "*The stinging acid rain", ATT_ACID );

  return TRUE;
}
