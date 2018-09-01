#include "system.h" 

//
//  Sorceror Stuff
//

bool spell_bone_dart( char_data* ch, char_data* victim, void*, int level, int duration )
{
  damage_physical( victim, ch, spell_damage( SPELL_BONE_DART, level ), "*The sliver of bone" );

  return TRUE;
}


bool spell_skull_watch( char_data* ch, char_data*, void*, int level, int duration )
{
  species_data*       skull;
  char_data*            mob;
  char_data*            pet;
  
  if( null_caster( ch, SPELL_SKULL_WATCH ) )
    return TRUE;

  if( ( skull = get_species( MOB_SKULL_WATCH ) ) == NULL ) {
    bug( "Skull Watch: Null mob, should be #%d", MOB_SKULL_WATCH );
    return TRUE;
  }

  for( int i = 0; i < ch->followers.size; i++ ) {
    if( is_pet( pet = ch->followers.list[i] ) ) {
      if( pet->species->vnum == MOB_SKULL_WATCH ) {
        send( ch, "You can only have one skull watch following you at a time.\r\n" );
        return TRUE;
      }
    }
  }

  if( is_set( ch->affected_by, AFF_SKULL_WATCH ) ) {
    send( ch, "You already have a skull watch assisting you.\r\n" );
    send( *ch->array, "%s fails to summon more assistance.\r\n", ch );
    return TRUE;
  }

  mob = create_mobile( skull );

  set_bit( &mob->status, STAT_PET );
  set_bit( &mob->status, STAT_TAMED );
  remove_bit( &mob->status, STAT_AGGR_ALL );
  remove_bit( &mob->status, STAT_AGGR_GOOD );
  remove_bit( &mob->status, STAT_AGGR_EVIL );

  mob->To( ch->array );
  add_follower( mob, ch, FALSE );
  
  send( ch, "%s rises and floats beside you.\r\n", mob );
  send( *ch->array, "%s rises and floats beside %s.\r\n", mob, ch );

  spell_affect( ch, ch, level, duration, SPELL_SKULL_WATCH, AFF_SKULL_WATCH );

  return TRUE;
}


bool spell_gift_night( char_data* ch, char_data* victim, void*, int level, int length )
{
  spell_affect( ch, victim, level, length, SPELL_GIFT_NIGHT, AFF_GIFT_NIGHT );
  damage_element( victim, ch, spell_damage( SPELL_GIFT_NIGHT, level ), "*the terrifying nightmare", ATT_MIND );

  return TRUE;
}
bool spell_blight( char_data* ch, char_data* victim, void* vo, int level, int duration )
{
  obj_data*  obj  = (obj_data*) vo;

  /* Quaff */

  if( ch == NULL && obj == NULL ) {
    fsend( victim, "You feel incredible pain as the blight starts consuming your body." );
    fsend_seen( victim, "%s grasps %s throat and starts spitting up blood.\r\n", victim );
    spell_affect( ch, victim, level, duration, SPELL_BLIGHT, AFF_BLIGHT ); 
    damage_element( victim, ch, spell_damage( SPELL_BLIGHT, level ), "*The blight", ATT_POISON );
    return TRUE;
  }  
  
  /* Throw-Cast */

  if( !is_set( victim->affected_by, AFF_BLIGHT ) )
    damage_element( victim, ch, spell_damage( SPELL_BLIGHT, level ), "*The blight", ATT_POISON );

  if( !makes_save( victim, ch, RES_POISON, SPELL_BLIGHT, level ) )
    spell_affect( ch, victim, level, duration, SPELL_BLIGHT, AFF_BLIGHT ); 

  return TRUE; 
}

bool spell_ghost_armor( char_data* ch, char_data* victim, void*, int level, int length )
{
  if( is_good( victim ) ) {
    send( ch, "The ghosts curse you for your attempts to force them to help %s.\r\n", victim );
    damage_element( ch, victim, spell_damage( SPELL_BLIGHT, level ), "*The blight", ATT_POISON );
    return FALSE;
  }

  spell_affect( ch, victim, level, length, SPELL_GHOST_ARMOR, AFF_GHOST_ARMOR );

  return TRUE;
}


bool spell_bone_wrench( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( victim->species != NULL && is_set( victim->species->act_flags, ACT_GHOST ) ) {
    fsend( victim, "You are unaffected by the spell." );
    fsend_seen( victim, "%s is unaffected by the spell.", victim );
    return FALSE;
  }

  damage_magic( victim, ch, spell_damage( SPELL_BONE_WRENCH, level ), "*The warping of bones" );

  return TRUE;
}


bool spell_starving_madness( char_data* ch, char_data* victim, void*, int level, int length )
{
  if( null_caster( ch, SPELL_STARVING_MADNESS ) )
    return TRUE;
 
  if( !consenting( victim, ch, "starving madness" ) )
    return TRUE;

  if( is_set( victim->affected_by, AFF_STARVING_MADNESS ) ) {
    fsend( victim, "The beast is unable to affect you anymore.\r\n" );
    fsend_seen( victim, "The beast is unable to affect %s anymore.\r\n", victim );
    return TRUE;
  }

  if( victim->species == NULL )
    victim->pcdata->condition[ COND_FULL ] = -100;

  spell_affect( ch, victim, level, length, SPELL_STARVING_MADNESS, AFF_STARVING_MADNESS );

  return TRUE;
}


bool spell_doom( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DOOM, AFF_DOOM );
  record_damage( victim, ch, 10 );

  return TRUE;
}


bool spell_ghost_chains( char_data* ch, char_data* victim, void*, int level, int duration )
{
  affect_data* paf;
  
  if( null_caster( ch, SPELL_GHOST_CHAINS ) )
    return TRUE;

  if( is_set( victim->affected_by, AFF_GHOST_CHAINS ) ) {
    for( int i = 0; i < victim->affected; i++ ) {
      paf = victim->affected[i];
      if( paf->type == AFF_GHOST_CHAINS ) {
        paf->level += 1+number_range( 0, level/3 );
        paf->duration = max( level/2, paf->duration+1 );
        break;
      }
    }

    send( "The ghost chains around you wrap tighter.\r\n", victim );
    send( *victim->array, "The chains around %s wrap tighter.\r\n", victim );
    record_damage( victim, ch, 10 );
    return TRUE;

  } else {
    send( victim, "The ghost chains attempt to ensnare you.\r\n" );
    send( *victim->array, "Some ghostly chains attempt to ensnare %s.\r\n", victim );

    if( !is_entangled( victim, "avoid the ghostly chains", true )
     && ( makes_save( victim, ch, RES_DEXTERITY, SPELL_GHOST_CHAINS, level )
     || makes_save( victim, ch, RES_MAGIC, SPELL_GHOST_CHAINS, level ) )
    ) {
      send( victim, "You manage to just avoid the ghostly chains.\r\n" );
      send( *victim->array, "%s just manages to avoid the ghostly chains.\r\n", victim );
      return TRUE;
    }
  }

  spell_affect( ch, victim, level, duration, SPELL_GHOST_CHAINS, AFF_GHOST_CHAINS );

  disrupt_spell( victim );
  record_damage( victim, ch, 10 );

  return TRUE;
}


bool spell_withering_flesh( char_data* ch, char_data* victim, void*, int level, int length )
{
  affect_data paf;
  
  if( victim->species != NULL && is_set( victim->species->act_flags, ACT_GHOST ) ) {
    send( ch, "Your attempt to call unholy fire on %s fails.\r\n", victim );
    send( victim, "%s attempts to call an unholy fire on you.\r\n", ch );
    send( *victim->array, "The unholy fire fails to heed %s's call.\r\n", ch );
    return TRUE;
  }

  paf.type     = AFF_POISON;
  paf.duration = number_range( 4, 12 )*level;
  paf.level    = level;
  paf.mlocation[ 0 ] = APPLY_NONE;
  paf.mmodifier[ 0 ] = 0;

  add_affect( victim, & paf );

  damage_unholy( victim, ch, spell_damage( SPELL_WITHERING_FLESH, level ), "*the unholy fire" );

  return TRUE;
}


bool spell_shield_bones( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_SHIELD_BONES, AFF_SHIELD_OF_BONES );

  return TRUE;
}


bool spell_mind_crush( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( victim->shdata->race == RACE_UNDEAD || ( victim->species != NULL && is_set( victim->species->act_flags, ACT_GHOST ) ) ) {
    fsend( victim, "You are unaffected by the spell." );
    fsend_seen( victim, "%s is unaffected by the spell.", victim );
    return FALSE;
  }
  
  damage_mind( victim, ch, spell_damage( SPELL_MIND_CRUSH, level ), "*the debilitating nightmare" );

  return TRUE;
}


bool spell_weapon_blight( char_data* ch, char_data*, void* vo, int level, int duration )
{
  obj_data* weapon = (obj_data*) vo;
  int         roll = number_range( 0, 150 ); 

  if( null_caster( ch, SPELL_WEAPON_BLIGHT ) )
    return TRUE;

  weapon->selected = 1;

  if( is_set( weapon->extra_flags, OFLAG_WEAPON_BLIGHT ) ) {
    fsend( ch, "%s is already touched by death.\r\n", weapon );
    return FALSE;
  }

  if( roll < 150-10*level+10*weapon->value[0] ) {
    fsend( *ch->array, "%s takes on a slight cast of death-black frost and cracks into pieces.\r\n", weapon );
    weapon->Extract( 1 );
    return TRUE;
  }
  if( roll <= 150-10*level+20*weapon->value[0] ) {
    fsend( *ch->array, "%s lets a stream of steam off as %s returns to room temperature.\r\n", weapon, weapon );
    return TRUE;
  }

  send( *ch->array, "%s takes on a death-black coat of frost, which slowly sinks into it.\r\n", weapon );

  if( weapon->number > 1 ) {
    weapon = object( weapon->From( 1 ) );
    if( !weapon || !weapon->Is_Valid( ) ) {
      roach( "spell_weapon_blight: blight worked but object not created for %s", ch );
      return TRUE;
    }
  }

  set_bit( weapon->extra_flags, OFLAG_WEAPON_BLIGHT );
  weapon->age += 100-5*level;

  return TRUE;
}


bool spell_destructive_touch( char_data* ch, char_data* victim, void*, int level, int duration )
{
  obj_data* obj;
  int       count = 0;
  
  if( !consenting( victim, ch, "destructive touch" ) )
    return TRUE;

  for( int i = victim->wearing.size-1; i >= 0; i-- ) {
    if( ( obj = object( victim->wearing[i] ) ) == NULL 
      || ( !is_set( obj->extra_flags, OFLAG_NODROP )
      && ( !is_set( obj->extra_flags, OFLAG_NOREMOVE ) ) ) )
      continue;

    if( count++ > level/5 )
      return TRUE;

    send( victim, "%s is covered by black shadows then turns into black dust.\r\n", obj );
    send( *victim->array, "%s which %s is wearing is covered by black shadows then turns into black dust.\r\n", obj, victim );
    obj->Extract( );
    if( number_range( 0, 50 ) < level ) {
      send( ch, "The black shadow soaks into your body.\r\n" );
      send_seen( ch, "The black shadow soaks into $n's body.\r\n", ch );
      ch->mana += number_range( 1, 25 );
      ch->mana = min( ch->mana, ch->max_mana );
    }
  }

  for( int i = victim->contents.size-1; i >= 0; i-- ) {
    if( ( obj = object( victim->contents[i] ) ) == NULL
      || ( !is_set( obj->extra_flags, OFLAG_NODROP )
      && ( !is_set( obj->extra_flags, OFLAG_NOREMOVE ) ) ) )
      continue;

    if( count++ > level/5 )
      return TRUE;

    send( victim, "%s is covered by black shadows and vanishes.\r\n", obj );
    send_seen( victim, "%s covered by black shadows and vanishes.\r\n", victim, ch );
    obj->Extract( );
    if( number_range( 0, 50 ) < level ) {
      send( ch, "The black shadow soaks into your body.\r\n" );
      send_seen( ch, "The black shadow soaks into $n's body.\r\n", ch );
      ch->mana += number_range( 1, 25 );
      ch->mana = min( ch->mana, ch->max_mana );
    }
  }
  
  if( count == 0 ) {
    if( victim == ch )
      send( ch, "You weren't carrying anything cursed.\r\n" );
    else
      send( ch, "%s wasn't carrying anything cursed.\r\n", victim );
  }

  return TRUE;
}


bool spell_bane( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_BANE, AFF_BANE );
  record_damage( victim, ch, 10 );

  return TRUE;
}


bool spell_mind_destruction( char_data* ch, char_data* victim, void*, int level, int duration )
{
  damage_mind( victim, ch, spell_damage( SPELL_MIND_DESTRUCTION, level ), "*the physical nightmare" );

  return TRUE;
}


bool spell_shuddering_touch( char_data* ch, char_data* victim, void*, int level, int duration )
{
  damage_element( victim, ch, spell_damage( SPELL_SHUDDERING_TOUCH, level ), "*the dark freezing touch", ATT_COLD );

  return TRUE;
}


bool spell_dead_man_eyes( char_data* ch, char_data* victim, void*, int level, int duration )
{
  obj_data *light = create( get_obj_index( OBJ_DEAD_MAN_EYES ) );
  obj_data*   obj;
  
  if( null_caster( ch, SPELL_DEAD_MAN_EYES ) )
    return TRUE;
  
  if( light == NULL ) {
    bug( "Spell - Dead Man Eyes, light object does not exist needs to be object #3999" );
    return TRUE;
  }
  
  for( int i = 0; i <= MAX_LAYER; i++ ) {
    if( ( obj = ch->Wearing( WEAR_FLOATING, i ) ) != NULL && i != MAX_LAYER ) {
      send( ch, "%s absorbs the light from %s, and it vanishes.\r\n", light, obj );
      send_seen( ch, "%s absorbs the light from %s, and it vanishes.\r\n", light, obj );
      obj->Extract( 1 );
    }
  }

  if( ch->get_skill( SPELL_CORPSE_LIGHT ) != UNLEARNT && light->value[ 3 ] > 0 )
    light->value[ 2 ] = light->value[ 3 ] * ch->get_skill( SPELL_CORPSE_LIGHT )/MAX_SKILL_LEVEL-1;
  else
    light->value[ 2 ] = level;

  if( number_range( 1, 20 ) == 1 )
    light->value[ 2 ] += 5;

  //A haunted eye rises to float around your head.
  send( ch, "%s rises to float around your head.\r\n", light );
  send( *ch->array, "%s rises to float around %s's head.\r\n", light, ch );

  light->owner    = ch->pcdata->pfile;
  light->position = WEAR_FLOATING;
  light->layer    = LAYER_BASE;
  light->To( &ch->wearing );

  spell_affect( ch, victim, level, duration, SPELL_DEAD_MAN_EYES, AFF_DEAD_MAN_EYES );
      
  return TRUE;
}


bool spell_soul_tear( char_data* ch, char_data* victim, void*, int level, int length )
{
  spell_affect( ch, victim, level, length, SPELL_SOUL_TEAR, AFF_SOUL_TEAR );
  damage_mind( victim, ch, spell_damage( SPELL_SOUL_TEAR, level ), "*The soul ripping" );

  return TRUE;
}


bool spell_death_mask( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DEATH_MASK, AFF_DEATH_MASK );

  return TRUE;
}


bool spell_volley_bones( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !makes_save( victim, ch, RES_HOLY, SPELL_VOLLEY_BONES, level ) ) {
    spell_affect( ch, victim, level, duration, SPELL_VOLLEY_BONES, AFF_POISON );
    spell_affect( ch, victim, level, duration, SPELL_VOLLEY_BONES, AFF_TOMB_ROT );
  }

  damage_element( victim, ch, spell_damage( SPELL_VOLLEY_BONES, level ), "*the hail of bone spikes", ATT_PHYSICAL );

  return TRUE;
}


bool spell_blood_bond( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "blood bond" ) )
    return TRUE;

  damage_element( victim, ch, spell_damage( SPELL_BLOOD_BOND, level ), "*the bond of blood", ATT_UNHOLY );

  send( victim, "The blood bond restores some of your energy.\r\n" );
  send( *victim->array, "The blood bond restores some of %s's energy.\r\n", victim );

  ch->mana += spell_damage( SPELL_BLOOD_BOND, level );
  ch->mana = min( ch->mana, ch->max_mana );
   
  spell_affect( ch, victim, level, duration, SPELL_BLOOD_BOND, AFF_BLOOD_BOND );

  return TRUE;
}


bool spell_death_skin( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DEATH_SKIN, AFF_DEATH_SKIN );

  return TRUE;
}


bool spell_desolation( char_data* ch, char_data* victim, void*, int level, int duration )
{
  char_data* rch;
  
  if( null_caster( ch, SPELL_DESOLATION ) )
    return TRUE;

  for( int i = *ch->array-1; i >= 0; i-- )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && can_kill( ch, rch, false ) && rch != ch && rch->Seen( ch ))
      damage_element( rch, ch, spell_damage( SPELL_DESOLATION, level ), "*the demonic fire", ATT_UNHOLY );

  return TRUE;
}


bool spell_forsaken( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !makes_save( victim, ch, RES_HOLY, SPELL_FORSAKEN, level ) )
    spell_affect( ch, victim, level, duration, SPELL_FORSAKEN, AFF_FORSAKEN );

  damage_element( victim, ch, spell_damage( SPELL_FORSAKEN, level ), "*the unholy acidic fire", ATT_ACID );

  return TRUE;
}


bool spell_feast_life( char_data* ch, char_data* victim, void* vo, int level, int duration )
{
  obj_data* corpse = (obj_data*) vo;
  thing_data* thing = (thing_data*) vo;
      
  if( null_caster( ch, SPELL_FEAST_LIFE ) || null_corpse( corpse, SPELL_FEAST_LIFE ) )
    return TRUE;

  vo = forbidden( thing, ch );

  if( vo == NULL ) {
    send( ch, "You are forbidden from using %s as a corpse.\r\n", corpse );
    return TRUE;
  }
    
  send( ch, "%s crumbles to dust as you drain its energy.\r\n", corpse );
  send( *ch->array, "%s crumbles to dust as %s drains its energy.\r\n", corpse, ch );

  ch->mana += spell_damage( SPELL_FEAST_LIFE, level );
  ch->mana  = min( ch->mana, ch->max_mana );
 
  ch->hit  += spell_damage( SPELL_FEAST_LIFE, level )/2;
  ch->hit   = min( ch->hit, ch->max_hit ); 

  corpse->Extract( 1 );

  return TRUE;
}


bool spell_mind_blank( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_MIND_BLANK, AFF_MIND_BLANK );
 
  return TRUE;
}


bool spell_bless_taraskatt( char_data* ch, char_data* victim, void*, int level, int duration )
{
  char_data* rch;

  if( null_caster( ch, SPELL_BLESS_TARASKATT ) )
    return TRUE;

  for( int i = *ch->array-1; i >= 0; i-- )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && can_kill( ch, rch, false ) && ch != rch && rch->Seen( ch ) )  
      damage_poison( rch, ch, spell_damage( SPELL_BLESS_TARASKATT, level ), "*the rotting flesh" );

  return TRUE;
}


bool spell_unholy_fire( char_data* ch, char_data* victim, void*, int level, int duration )
{
  damage_unholy( victim, ch, spell_damage( SPELL_UNHOLY_FIRE, level ), "*the incinerating unholy fire" ); 

  return TRUE;
}


bool spell_lichdom( char_data* ch, char_data* victim, void*, int level, int duration )
{
  return TRUE;
}


bool spell_decaying_touch( char_data* ch, char_data*, void* vo, int level, int duration )
{
  obj_data* weapon = (obj_data*) vo;
  int         roll = number_range( 0, 99 );

  if( null_caster( ch, SPELL_DECAYING_TOUCH ) )
    return TRUE;

  weapon->selected = 1;

  if( is_set( weapon->extra_flags, OFLAG_GOOD ) ) {
    fsend( ch, "%s strikes back at your attempt to disease it.\r\n", weapon );
    damage_element( ch, NULL, spell_damage( SPELL_SKIN_WRENCH, level ), "*The good powers", ATT_HOLY, "the good powers" );
    return TRUE;
  }

  if( is_set( weapon->extra_flags, OFLAG_DISEASED ) ) {
    fsend( ch, "%s has already been diseased.\r\n", weapon );
    return TRUE;
  }

  if( roll < 100-10*level+10*weapon->value[0] ) {
    fsend( *ch->array, "%s continues bubbling then melts into a puddle.\r\n", weapon );
    weapon->Extract( 1 );
    return TRUE;
  }
  
  if( roll <= 100-10*level+20*weapon->value[0] ) {
    fsend( *ch->array, "%s bubbles a bit, then returns to its original form.\r\n", weapon );
    return TRUE;
  }

  send( *ch->array, "%s glows a faint green.\r\n", weapon );

  if( weapon->number > 1 ) {
    weapon = object( weapon->From( 1 ) );
    if( !weapon || !weapon->Is_Valid( ) ) {
      roach( "spell_decaying_touch: decay worked but object not created for %s, ch" );
      return TRUE;
    }
  }

  set_bit( weapon->extra_flags, OFLAG_DISEASED );
  weapon->age += 100-level*5;

  if( weapon->array == NULL ) {
    weapon->To( ch );
    consolidate( weapon );
  }

  return TRUE;
}


bool spell_feast_of_dead( char_data* ch, char_data*, void* vo, int level, int duration )
{
  obj_data* corpse = (obj_data*) vo;
  thing_data* thing = (thing_data*) vo;
  
  if( null_caster( ch, SPELL_FEAST_OF_DEAD ) || null_corpse( corpse, SPELL_FEAST_OF_DEAD ) )
    return TRUE;

  vo = forbidden( thing, ch );
  if( vo == NULL ) {
    send( ch, "You are forbidden from using %s as a corpse.\r\n", corpse );
    return TRUE;
  }

  send( ch, "%s crumbles to dust as you drain its energy.\r\n", corpse );
  send( *ch->array, "%s crumbles to dust as %s drains its energy.\r\n", corpse, ch );

  ch->mana += spell_damage( SPELL_FEAST_OF_DEAD, level );
  ch->mana = min( ch->mana, ch->max_mana );

  corpse->Extract( 1 );

  return TRUE;
}


bool spell_mind_snap( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( victim->shdata->race == RACE_UNDEAD ) {
    fsend( victim, "%s attempts to rend your mind with nightmare images.", ch );
    fsend_seen( victim, "%s attemts to rend %s's mind with nightmarish images.", ch, victim );
    return TRUE;
  }

  damage_mind( victim, ch, spell_damage( SPELL_MIND_SNAP, level ), "*the nightmarish thought" );

  return TRUE;
}


bool spell_skin_of_cadaver( char_data* ch, char_data* victim, void*, int level, int durat )
{
  if( is_set( victim->affected_by, AFF_SKIN_CADAVER ) ) {
    fsend( victim, "You are already affected by the skin of the dead.\r\n" );
    fsend_seen( victim, "%s is already affected by the skin of the dead.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, durat, SPELL_SKIN_CADAVER, AFF_SKIN_CADAVER );

  return TRUE;
}


bool spell_skin_wrench( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( victim->species != NULL && is_set( victim->species->act_flags, ACT_GHOST ) ) {
    fsend( victim, "%s attempts to rip your flesh of.", victim );
    fsend_seen( victim, "%s attempts to rip %s's skin.", ch, victim );
    return FALSE;
  }

  if( victim != NULL && victim->shdata->race == RACE_UNDEAD )
    damage_physical( victim, ch, spell_damage( SPELL_SKIN_WRENCH, level )*2/3, "*the tearing skin" );
  else
    damage_physical( victim, ch, spell_damage( SPELL_SKIN_WRENCH, level ), "*the tearing skin" );

  return TRUE;
}


bool spell_dark_chill( char_data* ch, char_data* victim, void*, int level, int duration )
{
  damage_cold( victim, ch, spell_damage( SPELL_DARK_CHILL, level ), "*A freezing cold tendril" );

  return TRUE;
}


bool spell_gloom( char_data* ch, char_data* victim, void*, int level, int length )
{
  if( is_set( victim->affected_by, AFF_GLOOM ) ) {
    fsend( victim, "The gloom is unable to affect you.\r\n" );
    fsend_seen( victim, "The gloom is unable to affect %s.\r\n", victim );
    return TRUE;
  }

  if( makes_save( victim, ch, RES_MIND, SPELL_GLOOM, level/2 ) ) {
    fsend( victim, "The darkness tries to engulf you but you shake off the affects." );
    fsend_seen( victim, "%s is enguled in the darkness, but manages to shake it off.", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, length, SPELL_GLOOM, AFF_GLOOM ); 
  record_damage( victim, ch, 10 );

  return TRUE;
}


bool spell_corpse_light( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data* corpse = (obj_data*) vo;
  thing_data* thing = (thing_data*) vo;
  
  if( null_caster( ch, SPELL_CORPSE_LIGHT ) || null_corpse( corpse, SPELL_CORPSE_LIGHT ) )
    return TRUE;

  vo = forbidden( thing, ch );

  if( vo == NULL ) {
    send( ch, "You are forbidden from using %s as a corpse.\r\n", corpse );
    return TRUE;
  }

  int slot;

  for( slot = 0; slot <= MAX_LAYER; slot++ ) { 
    if( ch->Wearing( WEAR_FLOATING, slot ) == NULL && slot != MAX_LAYER )
      break;

    if( slot == MAX_LAYER ) { 
      send( ch, "You are already controlling as many corpse lights as you can.\r\n" );
      return TRUE;
    }
  }

  obj_data *light = create( get_obj_index( OBJ_CORPSE_LIGHT ) );

  if( light == NULL ) {
    bug( "Corpse Light Object is NULL, should be vnum %i", OBJ_CORPSE_LIGHT );
    return TRUE;
  }

  if( ch->get_skill( SPELL_CORPSE_LIGHT ) != UNLEARNT && light->value[ 3 ] > 0 )
    light->value[ 2 ] = light->value[ 3 ] * ch->get_skill( SPELL_CORPSE_LIGHT ) / MAX_SKILL_LEVEL - 1;
  else 
    light->value[ 2 ] = level;

  if( number_range( 1, 20 ) == 5 )
    light->value[ 2 ] += 5;

  // A ghoulish light rises from a corpse of a rabbit as it crumbles to dust.
  send( ch, "%s rises from the crumbling %s.\r\n", light, corpse->singular );
  send( *ch->array, "%s rises from the crumbling %s, at %s's command.\r\n", light, corpse->singular, ch );

  corpse->Extract( 1 );
  
  light->owner    = ch->pcdata->pfile;
  light->position = WEAR_FLOATING;
  light->layer    = slot;
  light->To( &ch->wearing );

  return TRUE;
}


bool spell_corpse_harvest( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data* corpse = (obj_data*) vo;
  thing_data* thing = (thing_data*) vo;
  
  if( null_caster( ch, SPELL_CORPSE_HARVEST ) || null_corpse( corpse, SPELL_CORPSE_HARVEST ) )
    return TRUE;

  vo = forbidden( thing, ch );

  if( vo == NULL ) {
    send( ch, "You are forbidden from using %s as a corpse.\r\n", corpse );
    return TRUE;
  }

  if( ch->pcdata != NULL ) {
    if( !is_set( ch->affected_by, AFF_STARVING_MADNESS ) ) {
      ch->pcdata->condition[ COND_FULL ] += spell_damage( SPELL_CORPSE_HARVEST, level, 0 );;
      ch->pcdata->condition[ COND_FULL ] = min( 100, ch->pcdata->condition[ COND_FULL ] );
    }
    
    ch->pcdata->condition[ COND_THIRST ] += 70*ch->get_skill( SPELL_CORPSE_HARVEST )/MAX_SKILL_LEVEL;
    ch->pcdata->condition[ COND_THIRST ] = min( 100, ch->pcdata->condition[ COND_THIRST ] );
  }

  send( ch, "You finish your extraction of vital fluids and meat from %s.\r\n", corpse );
  send( *ch->array, "%s extracts the vital fluids and meat from %s and consumes them.\r\n", ch, corpse );
  corpse->Extract( 1 );

  return TRUE;
}

bool spell_ghost_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "ghost shield" ) )
    return TRUE;

  if( affected_holy( victim, true ) ) {
    send( victim, "Your holy aura prevents the aura from forming.\r\n" );
    send_seen( victim, "%s's holy aura prevents the aura from forming.\r\n", victim );
    return TRUE;
  }

  if( is_good( victim ) ) {
    send( ch, "The ghosts attack you for your attempts to force them to help %s.\r\n", victim );
    spell_doom( victim, ch, NULL, level, duration );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_GHOST_SHIELD, AFF_GHOST_SHIELD );

  return TRUE;
}

bool spell_demon_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "demon shield" ) )
    return TRUE;

  if( affected_holy( victim, true ) ) {
    send( victim, "Your holy aura prevents the shield from forming.\r\n" );
    send_seen( victim, "%s's holy aura prevents the shield from forming.\r\n", victim );
    return TRUE;
  }

  if( is_good( victim ) ) {
    send( ch, "The demons attack you for your attempts to force them to help %s.\r\n", victim );
    damage_element( ch, victim, spell_damage( SPELL_SOUL_TEAR, level ), "*The raging demons", ATT_UNHOLY );
    return TRUE;
  }  

  spell_affect( ch, victim, level, duration, SPELL_DEMON_SHIELD, AFF_DEMON_SHIELD );

  return TRUE;
} 

bool spell_body_rot( char_data* ch, char_data* victim, void*, int level, int duration )
{
  obj_data* obj = victim->Wearing( WEAR_HELD_R );

  if( victim->species != NULL && 
    !makes_save( victim, ch, RES_MAGIC, SPELL_BODY_ROT, level ) &&
    !is_set( victim->species->act_flags, ACT_NO_DISARM ) ) {
      if( obj != NULL && !is_set( obj->extra_flags, OFLAG_NO_DISARM ) ) {
        send( victim, "Your rotting flesh causes you to drop %s.\r\n", obj );
        send( *victim->array, "%s's rotting flesh causes %s to drop %s.\r\n", victim, victim->Him_Her( ), obj );

        thing_data* thing = obj->From( 1 );
        if( ( obj = object( thing ) ) != NULL )
          obj->To( victim );
    }
  }


  if( victim->species == NULL && 
    !makes_save( victim, ch, RES_MAGIC, SPELL_BODY_ROT, level ) ) {
    if( obj != NULL && !is_set( obj->extra_flags, OFLAG_NO_DISARM ) ) {
      send( victim, "Your rotting flesh causes you to drop %s.\r\n", obj );
      send( *victim->array, "%s's rotting flesh causes %s to drop %s.\r\n", victim, victim->His_Her( ), obj );

      thing_data* thing = obj->From( 1 );
      if( ( obj = object( thing ) ) != NULL )
        obj->To( victim );
    }
  }
      
  damage_element( victim, ch, spell_damage( SPELL_BODY_ROT, level ), "*The rotting flesh", ATT_PHYSICAL );

  return TRUE;
} 

bool spell_shatter_resolve( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_SHATTER_RESOLVE, AFF_SHATTER_RESOLVE );
  damage_element( victim, ch, spell_damage( SPELL_SHATTER_RESOLVE, level ), "*Disolving resolutions", ATT_MIND );
  
  return TRUE;
}

bool spell_liquify_bones( char_data* ch, char_data* victim, void*, int level, int duration )
{
  damage_element( victim, ch, spell_damage( SPELL_LIQUIFY_BONES, level ), "*Dissolving bones", ATT_PHYSICAL );

  return TRUE;
}

bool spell_mind_blow( char_data* ch, char_data* victim, void*, int level, int duration )
{
  damage_element( victim, ch, spell_damage( SPELL_MIND_BLOW, level ), "*The mental attack", ATT_MIND );

  return TRUE;
}

bool spell_enfeeblement( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( makes_save( victim, ch, RES_MAGIC, SPELL_ENFEEBLEMENT, level ) ) {
    send( victim, "You shake of the affects of the deadly spell.\r\n" );
    send( *victim->array, "%s shakes of the affects of the deadly spell.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_ENFEEBLEMENT, AFF_SLOW );
  spell_affect( ch, victim, level, duration, SPELL_ENFEEBLEMENT, AFF_BLIND );

  record_damage( victim, ch, 10 );

  return TRUE;
}

bool spell_simulcrum( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_SIMULCRUM, AFF_SIMULCRUM );

  return TRUE;
}

bool spell_deaths_recall( char_data* ch, char_data* victim, void*, int level, int )
{
  bool                    found = false;
  room_data*               room = NULL;
  int         caster_first_room = ch->in_room->area->room_first->vnum;
  int         corpse_first_room = 0;

  if( null_caster( ch, SPELL_DEATHS_RECALL ) )
    return FALSE;
  
  if( victim == NULL )
    return FALSE;

  if( !consenting( victim, ch, "deaths recall" ) )
    return FALSE;

  for( int i = 0; i < corpse_list; i++ ) {
    obj_data* corpse = corpse_list[i];
    if( corpse->array->where != NULL ) {
      room = Room( corpse->array->where );
      if( room == NULL ) {
        send( ch, "The corpse is somewhere weird.\r\n" );
        return TRUE;
      }
      corpse_first_room = room->area->room_first->vnum;
    }
    if( corpse->value[3] == victim->pcdata->pfile->ident && !found 
      && caster_first_room == corpse_first_room ) {
        corpse->From( );
        corpse->To( ch->array );
        found = true;
    }
  }
  if( found )
    send( *ch->array, "Clouds of shadow wreathe the area, and a form appears on the ground.\r\n" );
  else
    send( *ch->array, "The spell mysteriously fails.\r\n" );

  return TRUE;
}

bool spell_body_snatch( char_data* ch, char_data* victim, void*, int level, int )
{
  bool                    found = false;
  room_data*               room = NULL;
  int         caster_first_room = ch->in_room->area->continent;
  int         corpse_first_room = 0;

  if( null_caster( ch, SPELL_BODY_SNATCH ) )
    return FALSE;
  
  if( victim == NULL )
    return FALSE;

  if( !consenting( victim, ch, "body snatch" ) )
    return FALSE;

  for( int i = 0; i < corpse_list; i++ ) {
    obj_data* corpse = corpse_list[i];
    if( corpse->array->where != NULL ) {
      room = Room( corpse->array->where );
      if( room == NULL ) {
        send( ch, "The corpse is somewhere weird.\r\n" );
        return TRUE;
      }
      corpse_first_room = room->area->continent;
    }
    if( corpse->value[3] == victim->pcdata->pfile->ident && !found 
      && caster_first_room == corpse_first_room && 
      is_set( &ch->in_room->room_flags, RFLAG_ALTAR ) ) {
        corpse->From( );
        corpse->To( ch->array );
        found = true;
    }
  }
  if( found )
    send( *ch->array, "Clouds of shadow wreathe the area, and a form appears on the ground.\r\n" );
  else
    send( *ch->array, "The spell mysteriously fails.\r\n" );

  return TRUE;
}

bool spell_summon_corpse( char_data* ch, char_data* victim, void*, int level, int )
{
  bool                    found = false;
  room_data*               room = NULL;
  int         caster_first_room = ch->in_room->area->continent;
  int         corpse_first_room = 0;

  if( null_caster( ch, SPELL_SUMMON_CORPSE ) )
    return FALSE;
  
  if( victim == NULL )
    return FALSE;

  if( !consenting( victim, ch, "summon corpse" ) )
    return FALSE;

  for( int i = 0; i < corpse_list; i++ ) {
    obj_data* corpse = corpse_list[i];
    if( corpse->array->where != NULL ) {
      room = Room( corpse->array->where );
      if( room == NULL ) {
        send( ch, "The corpse is somewhere weird.\r\n" );
        return TRUE;
      }
      corpse_first_room = room->area->continent;
    }
    if( corpse->value[3] == victim->pcdata->pfile->ident && !found 
      && caster_first_room == corpse_first_room ) {
        corpse->From( );
        corpse->To( ch->array );
        found = true;
    }
  }
  if( found )
    send( *ch->array, "Clouds of shadow wreathe the area, and a form appears on the ground.\r\n" );
  else
    send( *ch->array, "The spell mysteriously fails.\r\n" );

  return TRUE;
}

bool spell_theft_of_death( char_data* ch, char_data* victim, void*, int level, int )
{
  bool                    found = false;

  if( null_caster( ch, SPELL_THEFT_OF_DEATH ) )
    return FALSE;
  
  if( victim == NULL )
    return FALSE;

  if( !consenting( victim, ch, "theft of death" ) )
    return FALSE;

  for( int i = 0; i < corpse_list; i++ ) {
    obj_data* corpse = corpse_list[i];
    if( corpse->value[3] == victim->pcdata->pfile->ident && !found ) {
        corpse->From( );
        corpse->To( ch->array );
        found = true;
    }
  }
  if( found )
    send( *ch->array, "Clouds of shadow wreathe the area, and a form appears on the ground.\r\n" );
  else
    send( *ch->array, "The spell mysteriously fails.\r\n" );

  return TRUE;
}

bool spell_bone_prison( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( makes_save( victim, ch, RES_HOLY, SPELL_BONE_PRISON, level ) ) {
    send( victim, "The bones attempting to surround you dissolve.\r\n" );
    send_seen( victim, "The bones attempting to surround %s dissolve.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_BONE_PRISON, AFF_BONE_PRISON );
  record_damage( victim, ch, 10 );

  return TRUE;
}

bool spell_life_tap( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( makes_save( victim, ch, RES_HOLY, SPELL_LIFE_TAP, level ) ) {
    send( victim, "An unholy force attempts to rip some life from you.\r\n" );
    send_seen( victim, "An unholy force attempts to rip some life from %s.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, ch, level, duration, SPELL_LIFE_TAP, AFF_LIFE_TAP );

  return TRUE;
}

bool spell_wraith_form( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_WRAITH_FORM, AFF_WRAITH_FORM );

  return TRUE;
}

