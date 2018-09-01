#include "system.h"

bool is_undead( char_data* ch )
{
  if( ch->pcdata != NULL )
    return false;

  if( is_set( ch->affected_by, AFF_DEATH_SKIN ) )
    return TRUE;

  if( ch->shdata->race != RACE_UNDEAD && ch->shdata->race != RACE_DEMON &&
    ch->shdata->race != RACE_CELESTIAL && ch->species != NULL && 
    !is_set( ch->species->act_flags, ACT_GHOST ) )
    return false;

  return true;
}


bool can_assist( char_data* ch, char_data* victim )
{
  if( ch == NULL || victim == NULL || ch->species != NULL )
    return TRUE;

  if( ch->pcdata->clss != CLSS_CLERIC
    && ch->pcdata->clss != CLSS_PALADIN )
    return TRUE;

  if( abs( ch->shdata->alignment%3-victim->shdata->alignment%3 ) != 2 )
    return TRUE;

  send(ch, "In this time of great need, the spirits accede to your request.\r\n");
  return TRUE;

//  send( ch, "You feel the energy drain from you, but for some reason the spirits refuse\r\nyour request.\r\n" );
//  return FALSE;
}

bool affected_holy( char_data* ch, bool holy )
{
  if( holy ) {
    if( !is_set( ch->affected_by, AFF_BLESSED_AURA ) &&
      !is_set( ch->affected_by, AFF_HALLOWED_AURA ) &&
      !is_set( ch->affected_by, AFF_SAINTLY_AURA ) &&
      !is_set( ch->affected_by, AFF_HOLY_AURA ) )
      return FALSE;
    return TRUE;
  }
  else {
    if( !is_set( ch->affected_by, AFF_EVIL_AURA ) &&
      !is_set( ch->affected_by, AFF_WICKED_AURA ) &&
      !is_set( ch->affected_by, AFF_DREADFUL_AURA ) &&
      !is_set( ch->affected_by, AFF_UNHOLY_AURA ) &&
      !is_set( ch->affected_by, AFF_GHOST_SHIELD ) &&
      !is_set( ch->affected_by, AFF_DEMON_SHIELD ) )
      return FALSE;
    return TRUE;
  }
}


/*
 *   HEALING SPELLS
 */


void heal_victim( char_data* ch, char_data* victim, int hp )
{
  const char *heal_message[] = {
    "You were not wounded so were not healed.\r\n",
    "$n was not wounded so was not healed.",

    "You are completely healed.\r\n",
    "$n is completely healed.",

    "Most of your wounds are gone.\r\n",
    "Most of $n's wounds disappear.",

    "Your wounds feel quite a bit better.\r\n",
    "$n looks quite a bit less injured.",

    "You are slightly healed.\r\n",
    "$n looks a little bit better."
  };

  if( victim == NULL && ch == NULL )
    return;
  else if( victim == NULL && ch != NULL )
    victim = ch;

  int damage = victim->max_hit-victim->hit;
  int i;

  if( !can_assist( ch, victim ) )
    return;

  victim->hit = min( victim->max_hit, victim->hit+hp );

  i = ( damage <= hp ? ( damage <= 0 ? 0 : 1 ) : ( 2*damage < 3*hp ? 2 : ( damage < 3*hp ? 3 : 4 ) ) );

  char_data* fighting = opponent( victim );
  if( fighting )
    record_damage( fighting, ch, 10 );

  send( victim, heal_message[2*i] );
  act_notchar( heal_message[2*i+1], victim );

  update_pos( victim );
  update_max_move( victim );

  return;
}


bool spell_cure_light( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_CURE_LIGHT, level ) );

  return TRUE;
}


bool spell_cure_serious( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_CURE_SERIOUS, level ) );

  return TRUE;
}


bool spell_group_serious( char_data* ch, char_data*, void*, int level, int )
{
  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      heal_victim( ch, gch, spell_damage( SPELL_GROUP_SERIOUS, level ) );

  return TRUE;
}


bool spell_cure_critical( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_CURE_CRITICAL, level ) );

  return TRUE;
}



bool spell_group_critical( char_data* ch, char_data*, void*, int level, int )
{
  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      heal_victim( ch, gch, spell_damage( SPELL_GROUP_CRITICAL, level ) );

  return TRUE;

}

bool spell_heal( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_HEAL, level ) );

  return TRUE;
}

bool spell_restoration( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_RESTORATION, level ) );

  return TRUE;
}


/*
 *   REMOVE CURSE
 */


bool spell_remove_curse( char_data* ch, char_data* victim, void*, int level, int )
{
  obj_data*   obj;
  int       count  = 0;

  if( !consenting( victim, ch, "curse removal" ) ) 
    return TRUE;

  for( int i = victim->wearing.size-1; i >= 0; i-- ) {
    if( ( obj = object( victim->wearing[i] ) ) == NULL
      || ( !is_set( obj->extra_flags, OFLAG_NODROP )
      && ( !is_set( obj->extra_flags, OFLAG_NOREMOVE ) ) ) ) 
      continue;

    if( count++ > level/5 ) 
      return TRUE;

    send( victim, "%s which you are wearing turns to dust.\r\n", obj );
    send_seen( victim, "%s which %s is wearing turns to dust.\r\n", obj, victim );
    obj->Extract( );
    }

  for( int i = victim->contents.size-1; i >= 0; i-- ) {
    if( ( obj = object( victim->contents[i] ) ) == NULL
      || ( !is_set( obj->extra_flags, OFLAG_NODROP )
      && ( !is_set( obj->extra_flags, OFLAG_NOREMOVE ) ) ) ) 
      continue;

    if( count++ > level/5 ) 
      return TRUE;

    send( victim, "%s which you are carrying turns to dust.\r\n", obj );
    send_seen( victim, "%s which %s is carrying turns to dust.\r\n", obj, victim );
    obj->Extract( );
    }

  if( count == 0 ) 
    if( victim == ch ) 
      send( ch, "You weren't carrying anything cursed.\r\n" );
    else 
      send( ch, "%s wasn't carrying anything cursed.\r\n", victim );

  return TRUE;
}

bool spell_restore_item( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data*   obj  = (obj_data*) vo;
  int        roll  = number_range( 0, 100 );

  if( null_caster( ch, SPELL_RESTORE_ITEM ) ) 
    return TRUE;

  obj->selected = 1;

  if( !is_set( obj->extra_flags, OFLAG_NOREMOVE ) &&
    !is_set( obj->extra_flags, OFLAG_NODROP ) &&
    obj->value[0] >= 0 ) {
    send( *ch->array, "%s starts to glow, but the light turns black and then fades.\r\n", obj );
    return TRUE;
  }

  if( is_set( obj->extra_flags, OFLAG_NOREMOVE ) )
    roll -= 5;
  if( is_set( obj->extra_flags, OFLAG_NODROP ) )
    roll -= 5;

  if( roll < 30-level-10*obj->value[0] ) {
    fsend( *ch->array, "%s glows briefly, but then suddenly explodes in a burst of energy!\r\n", obj );
    obj->Extract( 1 );
    return TRUE;
  } 

  if( roll <= 60-2*level+20*obj->value[0] ) {
    fsend( *ch->array, "%s glows briefly but the power fails.\r\n", obj );
    return TRUE;
  }

  send( *ch->array, "%s glows for a while.\r\n", obj );

  if( obj->number > 1 ) {
    obj = object( obj->From( 1 ) );
    if( !obj || !obj->Is_Valid() ) {
      roach( "spell_minor_enchant: enchant succeeded but object not created for %s", ch );
      return TRUE;
    }
  }

  if( obj->value[0] < 0 )
    obj->value[0]++;

  remove_bit( obj->extra_flags, OFLAG_NOREMOVE );
  remove_bit( obj->extra_flags, OFLAG_NODROP );
  
  if( obj->array == NULL ) {
    obj->To( ch );
    consolidate( obj );
  }

  return TRUE;
}


/*
 *   POISON/DISEASE
 */


bool spell_cure_disease( char_data* ch, char_data* victim, void*, int, int )
{
  if( !is_set( victim->affected_by, AFF_PLAGUE )
   && !is_set( victim->affected_by, AFF_TOMB_ROT )
   && !is_set( victim->affected_by, AFF_RABIES )
   && !is_set( victim->affected_by, AFF_BLIGHT ) ) {
    if( ch != victim )
      send( ch, "%s wasn't diseased.\r\n", victim );
    return TRUE;
  }

  if( is_set( victim->affected_by, AFF_PLAGUE ) )
    strip_affect( victim, AFF_PLAGUE );
  if( is_set( victim->affected_by, AFF_TOMB_ROT ) )
    strip_affect( victim, AFF_TOMB_ROT );
  if( is_set( victim->affected_by, AFF_RABIES ) )
    strip_affect( victim, AFF_RABIES );
  if( is_set( victim->affected_by, AFF_BLIGHT ) )
    strip_affect( victim, AFF_BLIGHT );

  if( is_set( victim->affected_by, AFF_PLAGUE )
   || is_set( victim->affected_by, AFF_TOMB_ROT )
   || is_set( victim->affected_by, AFF_RABIES )
   || is_set( victim->affected_by, AFF_BLIGHT ) ) {
    send( ch, "%s is still diseased!?\r\n", victim );
    send( victim, "You are still diseased!?" );
  }

  return TRUE;
}


bool spell_cure_poison( char_data* ch, char_data* victim, void*, int, int )
{
  bool is_drunk;

  is_drunk = ( victim->species == NULL && ( victim->pcdata->condition[COND_ALCOHOL] > 0 || victim->pcdata->condition[COND_DRUNK] > 0 ) );

  if( !is_drunk && !is_set( victim->affected_by, AFF_POISON ) ) {
    if( ch != victim )
      send( ch, "%s wasn't poisoned.\r\n", victim );
    return TRUE;
  }

  if( victim->pcdata != NULL ) {
    victim->pcdata->condition[ COND_ALCOHOL ] = 0;
    victim->pcdata->condition[ COND_DRUNK ]   = 0;
  }

  strip_affect( victim, AFF_POISON );

  if( is_set( victim->affected_by, AFF_POISON ) ) {
    if( ch != victim ) 
      send( ch, "%s is still poisoned!?\r\n", victim );
    send( victim, "You are still poisoned!?\r\n" );
  }

  return TRUE;
}


/*
 *   SILENCE SPELL
 */


bool spell_gift_of_tongues( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_GIFT_OF_TONGUES,
    AFF_TONGUES );

  return TRUE;
}

bool spell_silence( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( makes_save( victim, ch, RES_MAGIC, SPELL_SILENCE, level ) )
    return TRUE;

  end_song( ch );
  spell_affect( ch, victim, level, duration, SPELL_SILENCE, AFF_SILENCE );
  record_damage( victim, ch, 10 );

  return TRUE;
}

/*
bool spell_augury( char_data* ch, char_data*, void* vo, int, int )
{
  obj_data*  obj  = (obj_data*) vo;

  if( ch == NULL || obj == NULL )
    return TRUE;

  if( !obj->Belongs( ch ) ) {
    fsend( ch, "%s glows black and you sense the true owner is %s.",
      obj, obj->owner->name );
    }
  else {
    send( ch, "Nothing happens.\r\n" );
    }

  return TRUE;
}
*/

bool spell_true_sight( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_TRUE_SIGHT, AFF_TRUE_SIGHT );

  return TRUE;
}


bool spell_protect_life( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_PROTECT_LIFE, AFF_LIFE_SAVING ); 

  return TRUE;
}


bool spell_holy_aura( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "holy aura" ) )
    return TRUE;

  if( affected_holy( victim, false ) ) {
    send( victim, "Your unholy aura prevents the aura from forming.\r\n" );
    send_seen( victim, "%s's unholy aura prevents the aura from forming.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_HOLY_AURA, AFF_HOLY_AURA ); 

  return TRUE;
}

bool spell_blessed_aura( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "blessed aura" ) )
    return TRUE;

  if( affected_holy( victim, false ) ) {
    send( victim, "Your unholy aura prevents the aura from forming.\r\n" );
    send_seen( victim, "%s's unholy aura prevents the aura from forming.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_BLESSED_AURA, AFF_BLESSED_AURA ); 

  return TRUE;
}

bool spell_hallowed_aura( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "hallowed aura" ) )
    return TRUE;

  if( affected_holy( victim, false ) ) {
    send( victim, "Your unholy aura prevents the aura from forming.\r\n" );
    send_seen( victim, "%s's unholy aura prevents the aura from forming.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_HALLOWED_AURA, AFF_HALLOWED_AURA ); 

  return TRUE;
}

bool spell_saintly_aura( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "saintly aura" ) )
    return TRUE;

  if( affected_holy( victim, false ) ) {
    send( victim, "Your unholy aura prevents the aura from forming.\r\n" );
    send_seen( victim, "%s's unholy aura prevents the aura from forming.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_SAINTLY_AURA, AFF_SAINTLY_AURA ); 

  return TRUE;
}

bool spell_evil_aura( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "evil aura" ) )
    return TRUE;

  if( affected_holy( victim, true ) ) {
    send( victim, "Your holy aura prevents the aura from forming.\r\n" );
    send_seen( victim, "%s's unholy aura prevents the aura from forming.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_EVIL_AURA, AFF_EVIL_AURA ); 

  return TRUE;
}

bool spell_wicked_aura( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "wicked aura" ) )
    return TRUE;

  if( affected_holy( victim, true ) ) {
    send( victim, "Your holy aura prevents the aura from forming.\r\n" );
    send_seen( victim, "%s's unholy aura prevents the aura from forming.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_WICKED_AURA, AFF_WICKED_AURA ); 

  return TRUE;
}

bool spell_dreadful_aura( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "dreadful aura" ) )
    return TRUE;

  if( affected_holy( victim, true ) ) {
    send( victim, "Your holy aura prevents the aura from forming.\r\n" );
    send_seen( victim, "%s's unholy aura prevents the aura from forming.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_DREADFUL_AURA, AFF_DREADFUL_AURA ); 

  return TRUE;
}

bool spell_unholy_aura( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "unholy aura" ) )
    return TRUE;

  if( affected_holy( victim, true ) ) {
    send( victim, "Your holy aura prevents the aura from forming.\r\n" );
    send_seen( victim, "%s's unholy aura prevents the aura from forming.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_UNHOLY_AURA, AFF_UNHOLY_AURA ); 

  return TRUE;
}

bool spell_sense_life( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_SENSE_LIFE, AFF_SENSE_LIFE );

  return TRUE;
}

bool spell_unceasing_vigilance( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "unceasing vigilance" ) )
    return TRUE;

  spell_affect( ch, victim, level, duration, SPELL_UNCEASING_VIGILANCE, AFF_UNCEASING_VIGILANCE ); 

  return TRUE;
}
/*
 *   CREATION SPELLS
 */


bool spell_create_water( char_data* ch, char_data* victim, void* vo, int level, int duration )
{
  obj_data*      obj  = (obj_data*) vo;
  obj_data*      obj2;
  obj_clss_data* obj_clss;
  content_array* where;
  int            liquid;
  int            metal;

  /* DIP */

  if( duration == -3 ) {
    if( ( metal = obj->metal( ) ) != 0 && number_range( 0,3 ) == 0 ) {
      if( obj->rust == 3 ) {
        send( *ch->array, "%s disintegrates into worthless pieces.\r\n", obj );
        obj->Extract( 1 );
      } else {
        send( *ch->array, "%s %s.\r\n", obj, material_table[ metal ].rust_name );
        obj->rust++;
      }  
    }
    return FALSE;
  }      

  /* THROWING, REACTING */ 

  if( ch == NULL || victim != NULL )
    return FALSE;

  /* CAST */

  liquid = LIQ_WATER;
  where  = obj->array;

  if( number_range( 0, 100 ) < 2 ) {
    for( ; ; ) {
      if( ( liquid = number_range( 0, MAX_ENTRY_LIQUID-1 ) ) == LIQ_WATER || liquid_table[ liquid ].create )
        break;
    }
  }

  obj = (obj_data*) obj->From( 1 );

  obj_clss = obj->pIndexData;
  obj2 = create( obj_clss );
  obj2->value[1] = obj->value[1];

  obj->value[1] += 100*level;
  obj->value[2]  = liquid;
  obj->selected  = 1;

  include_empty  = FALSE;
  include_liquid = FALSE;

  if( obj->value[1] >= obj->value[0] ) {
    send( *ch->array, "%s fills to overflowing with %s.\r\n", obj2, liquid_table[ liquid ].name );
    obj->value[1] = obj->value[0];
  } else {
    send( *ch->array, "%s fills partially up with %s.\r\n", obj2, liquid_table[ liquid ].name );
  }

  include_empty  = TRUE;
  include_liquid = TRUE;

  set_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );
  obj->To( where );
  obj2->Extract( );
  react_filled( ch, obj, liquid );

  return TRUE;    
}


/*
 *   OBJECT SPELLS
 */

bool spell_purify( char_data* ch, char_data* victim, void*, int, int )
{
  if( !is_set( victim->affected_by, AFF_SLEEP ) && 
    !is_set( victim->affected_by, AFF_SLOW ) && 
    !is_set( victim->affected_by, AFF_CURSE ) &&
    !is_set( victim->affected_by, AFF_GLOOM ) &&
    !is_set( victim->affected_by, AFF_DOOM ) &&
    !is_set( victim->affected_by, AFF_BANE ) ) {
    if( ch != victim )
      send( ch, "%s is not affected by anything which can be purified.\r\n", victim );
    else
      send( victim, "You are not affected by anything which can be purified.\r\n" );
    return TRUE;
  }

  if( is_set( victim->affected_by, AFF_SLEEP ) ) {
    if( ch != victim )
      send( ch, "%s is no longer affected by magical sleep.\r\n", victim );
    else
      send( victim, "You are no longer affected by magical sleep.\r\n" );
    strip_affect( victim, AFF_SLEEP );
  }

  if( is_set( victim->affected_by, AFF_CURSE ) ) {
    if( ch != victim )
      send( ch, "%s is no longer cursed.\r\n", victim );
    else
      send( victim, "You are no longer cursed.\r\n" );
    strip_affect( victim, AFF_CURSE );
  }

  if( is_set( victim->affected_by, AFF_SLOW ) ) {
    if( ch != victim )
      send( ch, "%s is no longer slowed.\r\n", victim );
    else
      send( victim, "You are no longer slowed.\r\n" );

    strip_affect( victim, AFF_SLOW );
  }
  if( is_set( victim->affected_by, AFF_GLOOM ) ) {
    if( ch != victim )
      send( ch, "%s is no longer affected by the gloom.\r\n", victim );
    else
      send( ch, "You are no longer affected by the gloom.\r\n" );
    strip_affect( victim, AFF_GLOOM );
  }
  if( is_set( victim->affected_by, AFF_DOOM ) ) {
    if( ch != victim )
      send( ch, "%s is no longer affected by %s doom.\r\n", victim, victim->His_Her( ) );
    else
      send( ch, "You are no longer affected by your doom.\r\n" );
    strip_affect( victim, AFF_DOOM );
  }
  if( is_set( victim->affected_by, AFF_BANE ) ) {
    if( ch != victim )
      send( ch, "%s is no longer affected by bane.\r\n" );
    else
      send( ch, "You are no longer affected by bane.\r\n" );
    strip_affect( victim, AFF_BANE );
  }

  return TRUE;
}


bool spell_sanctify( char_data* ch, char_data*, void *vo, int level, int )
{
  obj_data *obj = (obj_data*) vo;
  int dam;

  dam = obj->value[0]*(obj->value[1]+1);

  if( is_set( obj->extra_flags, OFLAG_SANCT ) ) {
    send( ch, "Nothing happens.\r\n" );
    return TRUE;
  }

  if( dam > 3*level ) {
    fsend( *ch->array, "%s glows with a pale blue light which quickly fades.", obj );
    return TRUE;
  }

  fsend( *ch->array, "%s glows with a pale blue light.\r\nThis slowly fades as if being absorbed into the item.", obj );

  set_bit( obj->extra_flags, OFLAG_SANCT );

  return TRUE;
};


/*
 *   PROTECTION SPELLS
 */


bool spell_armor( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_ARMOR, AFF_ARMOR );

  return TRUE;
}


bool spell_bless( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_BLESS, AFF_BLESS );

  return TRUE;
}


bool spell_protect_good( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_PROTECT_GOOD, AFF_PROTECT_GOOD );

  return TRUE;
}


bool spell_protect_evil( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_PROTECT_EVIL, AFF_PROTECT_EVIL );

  return TRUE;
}


/*
 *   WRATH
 */


bool spell_holy_wrath( char_data *ch, char_data *victim, void*, int level, int duration )
{
  if( ch != NULL
    && ch->shdata->alignment%3 != victim->shdata->alignment%3 ) {
    send( ch, "You are unable to incite %s into a holy rage.\r\n", victim );
    send( victim, "%s is unable to incite you into a holy rage.\r\n", ch );
    return TRUE;
    }

  spell_affect( ch, victim, level, duration, SPELL_HOLY_WRATH, AFF_WRATH );

  return TRUE;
}


/*
 *   GOLEM SPELLS

bool spell_animate_clay( char_data* ch, char_data*, void*, int, int )
{
  char_data*       golem;
  species_data*  species;

  if( null_caster( ch, SPELL_ANIMATE_CLAY ) )
    return TRUE;

  for( int i = 0; i < ch->followers; i++ ) { 
    golem = ch->followers[i];
    if( is_set( &golem->status, STAT_PET ) && golem->shdata->race == RACE_GOLEM ) {
      send( ch, "The incarnation fails as you can only control one golem at a time.\r\n" );
      return TRUE;
    }
  }

  if( ( species = get_species( MOB_CLAY_GOLEM ) ) == NULL ) {
    bug( "Animate_Clay: NULL species." );
    return TRUE;
  }

  golem = create_mobile( species );

  golem->To( ch->array );
  add_follower( golem, ch );
  set_bit( &golem->status, STAT_PET );

  return TRUE;
}

 */

/*
 *   DEATH
 */

/*
bool spell_resurrect( char_data*, char_data*, void*, int, int )
{
  return TRUE;
}
*/

bool spell_souls_balm( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data*         corpse  = (obj_data*) vo;
  char_data*         victim = NULL;
  player_data*           pc;
  player_data*       caster = player( ch );
  thing_data*         thing;
  int                 ident, clevel, exp_int;
  double                exp = 0;
  bool               worked = false;
  obj_clss_data*   obj_clss = get_obj_index( OBJ_SOUL_STONE );

  if( null_caster( ch, SPELL_SOULS_BALM ) )
    return TRUE;

  if( obj_clss == NULL ) {
    send( ch, "[BUG] The 'soul stone' object does not exist. Please notify an immortal.\r\n" );
    bug( "Soul Stone Object does not exist.  Should be object vnum #%i", OBJ_SOUL_STONE );
    return TRUE;
  }
      
  if( corpse->pIndexData->vnum != OBJ_SOUL_STONE ) {
    send( ch, "The spell %s fails to work on %s.  It will only work on %s\r\n", spell_table[ SPELL_SOULS_BALM-SPELL_FIRST ].name,
      corpse, obj_clss->Name( ) );
    return TRUE;
  }

  exp    = corpse->value[1];
  clevel = corpse->value[2];
  ident  = corpse->value[3];

  int percentage = 90;
  percentage += clevel >= 80 ? -90 : clevel >= 70+level ? -60 : clevel >= 50+level ? -30 : clevel >= 30+level ? 0 : 2;

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( pc->In_Game( ) && pc->pcdata->pfile->ident == ident ) {
      if( exp == 0 )
        exp = death_exp( clevel );
      exp = percentage*exp/100;
      exp_int = (int) exp;
      thing = (thing_data*) pc;
      victim = character( thing );
      if( !consenting( victim, ch, "souls balm" ) )
        return FALSE;
      if( exp == 0 ) {
        send( ch, "For some reason the spell fails to work on %s.\r\n", corpse );
        return FALSE;
      }
      add_exp( victim, exp_int );
      player_log( victim, "Souls balm restored %i experience.  Casted by %s", exp_int, ch );
      worked = true;
      pc->prayer -= 50;
      caster->prayer -= 20;
      send( victim, "You have recovered %i experience point%s.\r\n", exp_int, exp > 1 ? "s" : "" );
    }
  }
  
  if( worked ) {
    send( victim, "Your soul has been balmed by %s, you have recovered some spirituality.\r\n", ch );
    fsend( *ch->array, "The spell consumes %s.", corpse );
    corpse->Extract( 1 );
    }
  else {
    send( ch, "The spell mysteriously fails to work.\r\n" );
    fsend( *ch->array, "The spell fails to work on %s.\r\n", corpse );
    }

  return TRUE;
}

bool spell_souls_respite( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data*          corpse = (obj_data*) vo;
  char_data*         victim = NULL;
  player_data*           pc;
  player_data*       caster = player( ch );
  thing_data*         thing;
  int                 ident, clevel, exp_int;
  double                exp = 0;
  bool               worked = false;
  obj_clss_data*   obj_clss = get_obj_index( OBJ_SOUL_STONE );

  if( null_caster( ch, SPELL_SOULS_RESPITE ) )
    return TRUE;

  if( obj_clss == NULL ) {
    send( ch, "[BUG] The 'soul stone' object does not exist. Please notify an immortal.\r\n" );
    bug( "Soul Stone Object does not exist.  Should be object vnum #%i", OBJ_SOUL_STONE );
    return TRUE;
  }
      
  if( corpse->pIndexData->vnum != OBJ_SOUL_STONE ) {
    send( ch, "The spell %s fails to work on %s.  It will only work on %s\r\n", spell_table[ SPELL_SOULS_RESPITE-SPELL_FIRST ].name,
      corpse, obj_clss->Name( ) );
    return TRUE;
  }

  ident  = corpse->value[3];
  clevel = corpse->value[2];
  exp    = corpse->value[1];

  int percentage = 90;
  percentage += clevel >= 80 ? -60 : clevel >= 70+level ? -30 : clevel >= 50+level ? 0 : clevel >= 30+level ? 2 : 4;

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( pc->In_Game( ) && pc->pcdata->pfile->ident == ident ) {
      if( exp == 0 )
        exp = death_exp( clevel );
      exp = percentage*exp/100;
      exp_int = (int) exp;
      thing = (thing_data*) pc;
      victim = character( thing );
      if( !consenting( victim, ch, "souls respite" ) )
        return FALSE;
      if( exp == 0 ) {
        send( ch, "For some reason the spell fails to work on %s.\r\n", corpse );
        return FALSE;
      }
      add_exp( victim, exp_int );
      player_log( victim, "Souls respite restored %i experience. Casted by %s", exp_int, ch );
      worked = true;
      pc->prayer -= 75;
      caster->prayer -= 30;
      send( victim, "You have recovered %i experience point%s.\r\n", exp_int, exp > 1 ? "s" : "" );
    }
  }
  
  if( worked ) {
    send( victim, "Your soul has been respited by %s, you have recovered some spirituality.\r\n", ch );
    fsend( *ch->array, "The spell consumes %s.", corpse );
    corpse->Extract( 1 );
    }
  else {
    send( ch, "The spell mysteriously fails to work.\r\n" );
    fsend( *ch->array, "The spell fails to work on %s.\r\n", corpse );
    }

  return TRUE;
}

bool spell_souls_recovery( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data*          corpse = (obj_data*) vo;
  char_data*         victim = NULL;
  player_data*           pc;
  player_data*       caster = player( ch );
  thing_data*         thing;
  int                 ident, clevel, exp_int;
  double                exp = 0;
  bool               worked = false;
  obj_clss_data*   obj_clss = get_obj_index( OBJ_SOUL_STONE );

  if( null_caster( ch, SPELL_SOULS_RECOVERY ) )
    return TRUE;

  if( obj_clss == NULL ) {
    send( ch, "[BUG] The 'soul stone' object does not exist. Please notify an immortal.\r\n" );
    bug( "Soul Stone Object does not exist.  Should be object vnum #%i", OBJ_SOUL_STONE );
    return TRUE;
  }
      
  if( corpse->pIndexData->vnum != OBJ_SOUL_STONE ) {
    send( ch, "The spell %s fails to work on %s.  It will only work on %s\r\n", spell_table[ SPELL_SOULS_RECOVERY-SPELL_FIRST ].name,
      corpse, obj_clss->Name( ) );
    return TRUE;
  }

  ident  = corpse->value[3];
  clevel = corpse->value[2];
  exp    = corpse->value[1];

  int percentage = 90;
  percentage += clevel >= 80 ? -30 : clevel >= 70+level ? 0 : clevel >= 50+level ? 2 : clevel >= 30+level ? 4 : 6;

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( pc->In_Game( ) && pc->pcdata->pfile->ident == ident ) {
      if( exp == 0 )
        exp = death_exp( clevel );
      exp = percentage*exp/100;
      exp_int = (int) exp;
      thing = (thing_data*) pc;
      victim = character( thing );
      if( !consenting( victim, ch, "souls recovery" ) )
        return FALSE;
      if( exp == 0 ) {
        send( ch, "For some reason the spell fails to work on %s.\r\n", corpse );
        return FALSE;
      }
      add_exp( victim, exp_int );
      player_log( victim, "Souls recovery restored %i experience.  Casted by %s", exp_int, ch );
      worked = true;
      pc->prayer -= 100;
      caster->prayer -= 40;
      send( victim, "You have recovered %i experience point%s.\r\n", exp_int, exp > 1 ? "s" : "" );
    }
  }
  
  if( worked ) {
    send( victim, "Your soul has been recovered by %s, you have recovered some spirituality.\r\n", ch );
    fsend( *ch->array, "The spell consumes %s.", corpse );
    corpse->Extract( 1 );
    }
  else {
    send( ch, "The spell mysteriously fails to work.\r\n" );
    fsend( *ch->array, "The spell fails to work on %s.\r\n", corpse );
    }

  return TRUE;
}

bool spell_souls_return( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data*          corpse = (obj_data*) vo;
  char_data*         victim = NULL;
  player_data*           pc;
  player_data*       caster = player( ch );
  thing_data*         thing;
  int                 ident, clevel, exp_int;
  double                exp = 0;
  bool               worked = false;
  obj_clss_data*   obj_clss = get_obj_index( OBJ_SOUL_STONE );

  if( null_caster( ch, SPELL_SOULS_RETURN ) )
    return TRUE;

  if( obj_clss == NULL ) {
    send( ch, "[BUG] The 'soul stone' object does not exist. Please notify an immortal.\r\n" );
    bug( "Soul Stone Object does not exist.  Should be object vnum #%i", OBJ_SOUL_STONE );
    return TRUE;
  }
      
  if( corpse->pIndexData->vnum != OBJ_SOUL_STONE ) {
    send( ch, "The spell %s fails to work on %s.  It will only work on %s\r\n", spell_table[ SPELL_SOULS_BALM-SPELL_FIRST ].name,
      corpse, obj_clss->Name( ) );
    return TRUE;
  }

  ident  = corpse->value[3];
  clevel = corpse->value[2];
  exp    = corpse->value[1];

  int percentage = 90;
  percentage += clevel >= 80 ? 0 : clevel >= 70+level ? 2 : clevel >= 50+level ? 4 : clevel >= 30+level ? 6 : 8;

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( pc->In_Game( ) && pc->pcdata->pfile->ident == ident ) {
      if( exp == 0 )
        exp = death_exp( clevel );
      exp = percentage*exp/100;
      exp_int = (int) exp;
      thing = (thing_data*) pc;
      victim = character( thing );
      if( !consenting( victim, ch, "souls return" ) )
        return FALSE;
      if( exp == 0 ) {
        send( ch, "For some reason the spell fails to work on %s.\r\n", corpse );
        return FALSE;
      }
      add_exp( victim, exp_int );
      player_log( victim, "Souls return restored %i experience.  Casted by %s", exp_int, ch );
      worked = true;
      pc->prayer -= 200;
      caster->prayer -= 50;
      send( victim, "You have recovered %i experience point%s.\r\n", exp_int, exp > 1 ? "s" : "" );
    }
  }
  
  if( worked ) {
    send( victim, "Your soul has been returned by %s, you have recovered some spirituality.\r\n", ch );
    fsend( *ch->array, "The spell consumes %s.", corpse );
    corpse->Extract( 1 );
    }
  else {
    send( ch, "The spell mysteriously fails to work.\r\n" );
    fsend( *ch->array, "The spell fails to work on %s.\r\n", corpse );
    }

  return TRUE;
}



/*
 *   FEAR
 */


bool spell_fear( char_data* ch, char_data* victim, void*, int level, int )
{
  /*
  if( ch == NULL ) {
    send( victim, "Tell merior she forgot about fear potions.\r\n" );
    return TRUE; 
    }
  */

  if( makes_save( victim, ch, RES_MAGIC, SPELL_FEAR, level ) ) {
    send( ch, "You are unable to make %s afraid!\r\n", victim );
    send( victim, "%s is unable to make you afraid!\r\n", ch );
    send( *ch->array, "%s is unable to make %s afraid.\r\n", ch, victim );
    return TRUE;
    }

  send( victim, "%s causes you to cringe in fear!\r\n", ch );
  send( *ch->array, "%s cringes in fear!\r\n", victim );
 
  disrupt_spell( victim );
  record_damage( victim, ch, 10 );
  set_delay( victim, 40+level );

  return TRUE;
}

bool spell_earthquake( char_data* ch, char_data*, void*, int level, int )
{
  char_data* rch;

  if( null_caster( ch, SPELL_EARTHQUAKE ) )
    return TRUE;

  for( int i = *ch->array-1; i >= 0; i-- )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch 
      && can_kill( ch, rch, FALSE ) && rch->Seen( ch ) && !rch->can_fly( ) 
      && ( ch->pcdata != NULL || ( ch->pcdata == NULL && is_same_group( rch, ch->fighting ) ) ) )
      damage_physical( rch, ch, spell_damage( SPELL_EARTHQUAKE, level ), "*The rumbling earth" );

  return TRUE;
}

bool spell_holy_word( char_data *ch, char_data*, void*, int level, int )
{
  char_data* rch;

  if( null_caster( ch, SPELL_HOLY_WORD ) )
    return TRUE;

  for( int i = *ch->array-1; i >= 0; i-- )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && can_kill( ch, rch, false ) && rch->Seen( ch ) 
      && is_evil( rch ) && ( ch->pcdata != NULL || ( ch->pcdata == NULL && is_same_group( rch, ch->fighting ) ) ) )
        damage_holy( rch, ch, spell_damage( SPELL_HOLY_WORD, level ), "*The holy magic" );

  return TRUE;
}

bool spell_unholy_word( char_data *ch, char_data*, void*, int level, int )
{
  char_data* rch;

  if( null_caster( ch, SPELL_UNHOLY_WORD ) )
    return TRUE;

  for( int i = *ch->array-1; i >= 0; i-- )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ( ch->pcdata == NULL && ( rch->fighting == ch || rch->fighting == ch->leader ) ) && can_kill( ch, rch, false ) && rch->Seen( ch ) 
      && is_good( rch ) && ( ch->pcdata != NULL || ( ch->pcdata == NULL && is_same_group( rch, ch->fighting ) ) ) )
        damage_holy( rch, ch, spell_damage( SPELL_UNHOLY_WORD, level ), "*The unholy magic" );

  return TRUE;
}

bool spell_unholy_flail( char_data *ch, char_data*, void*, int level, int )
{
  obj_data* obj;

  if( null_caster( ch, SPELL_UNHOLY_FLAIL ) )
    return TRUE;

  if( ( obj = create( get_obj_index( OBJ_UNHOLY_FLAIL ) ) ) == NULL ) {
    code_bug( "Unholy Flail doesn't exist, needs to be vnum 3111" );
    return FALSE;
  }

  set_alloy( obj, 10 );
  obj->To( ch );

  return TRUE;
}

bool spell_holy_flail( char_data *ch, char_data*, void*, int level, int )
{
  obj_data* obj;

  if( null_caster( ch, SPELL_HOLY_FLAIL ) )
    return TRUE;

  if( ( obj = create( get_obj_index( OBJ_HOLY_FLAIL ) ) ) == NULL ) {
    code_bug( "Holy Flail doesn't exist, needs to be vnum 3106" );
    return FALSE;
    }

  set_alloy( obj, 10 );

  obj->To( ch );

  return TRUE;
}
