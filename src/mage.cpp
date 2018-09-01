#include "system.h"

/*
 *   DAMAGE
 */


bool spell_meteor_swarm( char_data* ch, char_data*, void*, int level, int )
{
  char_data* rch;

  if( null_caster( ch, SPELL_METEOR_SWARM ) )
    return TRUE;

  for( int i = *ch->array-1; i >= 0; i-- )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && can_kill( ch, rch, false ) && rch->Seen( ch ) )
      damage_physical( rch, ch, spell_damage( SPELL_METEOR_SWARM, level ), "*The swarm of meteors" );

  return TRUE;
}


bool spell_maelstrom( char_data* ch, char_data*, void*, int level, int )
{
  char_data*   rch;

  if( null_caster( ch, SPELL_MAELSTROM ) )
    return TRUE;

  for( int i = ch->array->size - 1; i >= 0; i-- ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && can_kill( ch, rch, false ) && rch->Seen( ch ) )
      damage_magic( rch, ch, spell_damage( SPELL_MAELSTROM, level ), "*The maelstrom" );     

  return TRUE;
}


/*
 *   MISSILE SPELLS
 */


bool spell_magic_missile( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_magic( victim, ch, spell_damage( SPELL_MAGIC_MISSILE, level ), "*The magic missile" );

  return TRUE;
}

bool spell_gem_burst( char_data* ch, char_data* victim, void*, int level, int )
{
  if( damage_physical( victim, ch, spell_damage( SPELL_GEM_BURST, level ), "*The shattering gem" ) )
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

bool spell_prismic_missile( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_magic( victim, ch, spell_damage( SPELL_PRISMIC_MISSILE, level ), "*A multi-hued burst of light" );

  return TRUE;
}

bool spell_chromatic_orb( char_data* ch, char_data* victim, void*, int level, int )
{
  int        color;
  char   orb_color [ MAX_INPUT_LENGTH ];
  char         tmp [ MAX_INPUT_LENGTH ];
  char        tmp2 [ MAX_INPUT_LENGTH ];
  int        skill = 10*ch->get_skill( SPELL_CHROMATIC_ORB )/MAX_SKILL_LEVEL;

  if( null_caster( ch, SPELL_CHROMATIC_ORB ) )
    return TRUE;

  color = min( min( max( 1, skill ), number_range( 1, 9 ) ), 9 );

  if( color == 1 ) {
    sprintf( orb_color, "white" );
    spell_slow( ch, victim, NULL, level, skill*3 );
  }
  if( color == 2 ) {
    sprintf( orb_color, "red" );
    damage_fire( victim, ch, spell_damage( SPELL_CHROMATIC_ORB, level )/3, "*The fiery orb" );
  }
  if( color == 3 ) {
    sprintf( orb_color, "orange" );
    damage_acid( victim, ch, spell_damage( SPELL_CHROMATIC_ORB, level )/3, "*The orange orb" );
  }
  if( color == 4 ) {
    sprintf( orb_color, "yellow" );
    spell_blind( ch, victim, NULL, level, skill*3 );
  }
  if( color == 5 ) {
    sprintf( orb_color, "green" );
    damage_cold( victim, ch, spell_damage( SPELL_CHROMATIC_ORB, level )/3, "*The freezing green orb" );
  }
  if( color == 6 ) {
    sprintf( orb_color, "turquoise" );
    damage_shock( victim, ch, spell_damage( SPELL_CHROMATIC_ORB, level )/3, "*The shocking turqoise orb" );
  }
  if( color > 6 ) {
    char_data* rch;
    if( color == 7 )
      sprintf( orb_color, "blue" );
    else if( color == 8 )
      sprintf( orb_color, "violet" );
    else
      sprintf( orb_color, "black" );
    if( victim->Seen( ch ) ) 
      send( ch, "-* Your %s orb momentarily STUNS %s! *-\r\n", orb_color, victim->Seen_Name( ch ) ); 

    send( victim, "%s's %s orb momentarily STUNS you!\r\n", ch->Name( victim ), orb_color );

    for( int i = 0; i < *ch->array; i++ ) {
      if( ( rch = character( ch->array->list[i] ) ) == NULL || rch == ch || rch == victim || rch->link == NULL || !victim->Seen( rch ) )
        continue;
      sprintf( tmp, "%s's %s orb momentarily STUNS %s.\r\n", ch->Name( rch ), orb_color, victim->Seen_Name( rch ) );
      send( tmp, rch ); 
    }

    disrupt_spell( victim );
    set_delay( victim, 40 ); 
  }
  if( color == 8 )
    spell_slow( ch, victim, NULL, level, skill*3 );
  if( color == 9 ) {
    spell_slow( ch, victim, NULL, level, skill*3 );
    spell_blind( ch, victim, NULL, level, skill*3 );
    }
  
  sprintf( tmp2, "*The %s orb", orb_color );

  if( victim && victim->Is_Valid() )
    damage_magic( victim, ch, spell_damage( SPELL_CHROMATIC_ORB, level ), tmp2 );

  return TRUE;
}



/*
 *   SLEEP SPELLS
 */


void sleep_affect( char_data* ch, char_data* victim, int level, int duration )
{
  if( ch != NULL && !can_kill( ch, victim, false ) )
    return;
  
  if( is_set( victim->affected_by, AFF_SLEEP_RESIST ) )
    return; 

  if( ch != NULL && makes_save( victim, ch, RES_MAGIC, SPELL_SLEEP, level ) ) {
    if( victim->position > POS_SLEEPING ) {
      send( victim, "You feel drowsy but quickly shrug it off.\r\n" );
      send_seen( victim, "%s looks drowsy but quickly shrugs it off.\r\n", victim );
    }
    return;
  }

  spell_affect( ch, victim, level, duration, SPELL_SLEEP, AFF_SLEEP );
  record_damage( victim, ch, 10 );
  stop_fight( victim );
  clear_queue( victim );

  if( victim->position <= POS_SLEEPING ) 
    return;

  send( victim, "You feel sleepy and suddenly fall asleep!\r\n" );

  if( victim->mount != NULL ) {
    fsend( *victim->array, "%s tumbles off %s %s is riding and drops to the ground.", victim, victim->mount, victim->He_She( ) );
    victim->mount->rider = NULL;
    victim->mount = NULL;  
  } else {
    //int save_shown = victim->shown;
    //victim->shown = 1;
    send_seen( victim, "%s drops to the ground dead asleep.\r\n", victim );
    //victim->shown = save_shown;
  }

  victim->position = POS_SLEEPING;

  return;
}


bool spell_sleep( char_data* ch, char_data* victim, void*, int level, int duration )
{
  sleep_affect( ch, victim, level, duration );

  return TRUE;
}


bool spell_mists_sleep( char_data* ch, char_data*, void*, int level, int duration )
{
  char_data* rch;

  if( null_caster( ch, SPELL_MISTS_SLEEP ) )
    return TRUE;

  if( terrain_table[ ch->in_room->sector_type ].underwater  ) {
    send( ch, "You are unable to summon the mists underwater.\r\n" );
    return TRUE;
  }

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && can_kill( ch, rch, false ) && rch->Seen( ch ) )
      sleep_affect( ch, rch, level, duration );

  return TRUE;
}


/*
 *   STRENGTH SPELL
 */


bool spell_ogre_strength( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_OGRE_STRENGTH, AFF_OGRE_STRENGTH, min( 17, victim->shdata->strength+3 ) );

  return TRUE;
}


/*
 *   ENCHANTMENT SPELLS
 */


bool spell_detect_magic( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DETECT_MAGIC, AFF_DETECT_MAGIC );

  return TRUE;
}


bool spell_minor_enchantment( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data*   obj  = (obj_data*) vo;
  int        roll  = number_range( 0, 100 );

  if( null_caster( ch, SPELL_MINOR_ENCHANTMENT ) ) 
    return TRUE;

  obj->selected = 1;

  if( is_set( obj->extra_flags, OFLAG_NO_ENCHANT ) ) {
    send( ch, "This object cannot be enchanted.\r\n", obj );
    return TRUE;
  }

  if( obj->value[0] < 0 ) {
    send( *ch->array, "%s starts to glow, but the light turns black and then fades.\r\n", obj );
    return TRUE;
  }

  if( roll < 30-level+10*obj->value[0] ) {
    fsend( *ch->array, "%s glows briefly, but then suddenly explodes in a burst of energy!\r\n", obj );
    obj->Extract( 1 );
    return TRUE;
  } 

  if( roll <= 60-2*level+20*obj->value[0] ) {
    fsend( *ch->array, "%s glows briefly but the enchantment fails.\r\n", obj );
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

  obj->value[0]++;
  set_bit( obj->extra_flags, OFLAG_MAGIC );

  if( obj->array == NULL ) {
    obj->To( ch );
    consolidate( obj );
  }

  return TRUE;
}


bool spell_major_enchantment( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data*   obj  = (obj_data*) vo;
  int        roll  = number_range( 0, 100 );
 
  if( null_caster( ch, SPELL_MAJOR_ENCHANTMENT ) ) 
    return TRUE;
  
  obj->selected = 1;

  if( is_set( obj->extra_flags, OFLAG_NO_MAJOR ) ) {
    send( ch, "%s cannot be enchanted.\r\n", obj );
    return TRUE;
  }

  if( obj->value[0] < 0 || obj->value[0] >= 3 ) {
    fsend( *ch->array, "%s starts to glow, but the light turns black and then fades.", obj );
    return TRUE;
  }

  if( roll <= 30-level+20*obj->value[0] ) {
    fsend( *ch->array, "%s glows briefly but the enchantment fails.\r\n", obj );
    return TRUE;
  }

  send( *ch->array, "%s glows for a while.\r\n", obj );

  if( obj->number > 1 ) {
    obj = object( obj->From( 1 ) );
    if( !obj || !obj->Is_Valid() ) {
      roach( "spell_major_enchant: enchant succeeded but object not created for %s", ch );
      return TRUE;
    }
  }

  obj->value[0]++;
  set_bit( obj->extra_flags, OFLAG_MAGIC );
  set_bit( obj->extra_flags, OFLAG_IDENTIFIED );

  if( obj->array == NULL ) {
    obj->To( ch );
    consolidate( obj );
  }

  return TRUE;
}


/*
 *   REPLICATE
 */


bool spell_replicate( char_data* ch, char_data*, void* vo, int, int )
{
  obj_data*   obj  = (obj_data*) vo; 

  obj->selected = 1;

  if( is_set( obj->extra_flags, OFLAG_COPIED ) ) {
    fsend( ch, "You feel %s has already been copied and lacks the essence required for you to replicate it.", obj );
    return TRUE;
  }

  fsend( ch, "In a swirl of colors %s materializes in your hand.", obj );
  fsend_seen( ch, "In a swirl of colors %s materializes in %s's hand.", obj, ch );   

  /*
  obj->pIndexData->count++;

  if( obj->number > 1 ) {
    obj->number--;
    obj = duplicate( obj );
  }

  obj->number = 2;
  */

  obj = object( obj->From( 1 ) );
  if( !obj || !obj->Is_Valid() ) {
    roach( "spell_replicate: replicate succeeded but object not created for %s", ch );
    return TRUE;
  }

  obj->pIndexData->count++;
  obj->number++;
  set_bit( obj->extra_flags, OFLAG_COPIED );

  if( obj->array == NULL ) {
    obj->To( ch );
    consolidate( obj );
  }

  return TRUE;
}
 
 
/*
 *   IDENTIFY SPELL
 */


bool spell_identify( char_data* ch, char_data*, void* vo, int, int )
{
  char                  buf  [ MAX_INPUT_LENGTH ];
  affect_data*          paf;
  obj_data*             obj  = (obj_data*) vo;
  obj_clss_data*   obj_clss  = obj->pIndexData;
  bool                found  = FALSE;
  int                     i;

  if( null_caster( ch, SPELL_IDENTIFY ) )
    return TRUE;

  if( obj == NULL ) {
    bug( "Identify: NULL object." );
    return TRUE;
  }

  /*
  if( obj->number > 1 ) {
    obj->number--;
    obj = duplicate( obj );
  }
  */
 
  set_bit( obj->extra_flags, OFLAG_IDENTIFIED );
  set_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );

  send( ch, scroll_line[0] );

  in_character = FALSE;

  sprintf( buf, "%s%s", obj->Seen_Name( ch, 1 ), is_set( obj->extra_flags, OFLAG_NOREMOVE ) ? "  [CURSED]" : "" );
  send_title( ch, buf );

  send( ch, "     Base Cost: %-12d Level: %-11d Weight: %.2f lbs\r\n", obj->Cost( ), obj_clss->level, obj->Empty_Weight( 1 )/100. );
  sprintf( buf, "            " );

  switch( obj_clss->item_type ) {
  case ITEM_WEAPON:
    sprintf( buf+5,  "   Damage: %dd%d       ", obj_clss->value[1], obj_clss->value[2] );
    sprintf( buf+25, "    Class: %s          ", weapon_class( obj_clss ) );
    sprintf( buf+45, "   Attack: %s\r\n"      , weapon_attack( obj_clss ) );
    break;
  
  case ITEM_WAND:
    sprintf( buf+5,  "    Spell: %s          ", ( obj_clss->value[0] >= 0 && obj_clss->value[0] < SPELL_COUNT ) ? spell_table[ obj_clss->value[0] ].name : "none" );
    sprintf( buf+25, "  Charges: %d\r\n", obj->value[3] );
    break;
  
  case ITEM_WHISTLE:
    if( obj_clss->value[0] > 0 )
      sprintf( buf+5,  "   Range: %d\r\n", obj_clss->value[0] );
    break;
  
  case ITEM_SCROLL :
  case ITEM_POTION :
    sprintf( buf+5,  "    Spell: %s          ", ( obj_clss->value[0] >= 0 && obj_clss->value[0] < SPELL_COUNT ) ? spell_table[ obj_clss->value[0] ].name : "none" );
    sprintf( buf+25, "    Level: %d\r\n", obj_clss->value[1] );
    break;
  
  case ITEM_TRAP :
    sprintf( buf, "Damage: %dd%d\r\n", obj_clss->value[1], obj_clss->value[2] );
    break;
  
  case ITEM_SHIELD :
  case ITEM_ARMOR :
    sprintf( buf, "   Armor Class: %-12d  Global AC: %-3d", armor_class( obj ), obj_clss->value[2] );
    if( obj->value[2] != 0 && can_wear( obj, ITEM_WEAR_WRIST ) ) 
      sprintf( buf+strlen( buf ), "Protection Aura: %d", obj->value[2] );
    strcat( buf, "\r\n" );
    break;

  case ITEM_ARROW :
  case ITEM_BOLT :
  case ITEM_STONE :
    sprintf( buf+5, "Damage: %dd%d\r\n", obj_clss->value[1], obj_clss->value[2] );
    sprintf( buf+5, "Number of Shots: %-4d", obj->value[3] );
    break;

  default:
    buf[0] = '\0';
    break;
  } 

  if( *buf != '\0' ) {
    send( ch, buf );
  }

  sprintf( buf,    "%d%%", obj->vs_acid( ) );
  sprintf( buf+20, "%d%%", obj->vs_fire( ) );
  sprintf( buf+40, "%d%%", obj->vs_cold( ) );

  send( ch, "          Acid: %-13s Fire: %-13s Cold: %s\r\n\r\n", buf, buf+20, buf+40 );

  /* CONDITION */ 

  if( obj_clss->item_type == ITEM_ARMOR || obj_clss->item_type == ITEM_SHIELD || obj_clss->item_type == ITEM_WEAPON )  
    send( ch, "     Condition: %s\r\n", obj->condition_name( ch, TRUE ) );

  /* MATERIALS */

  strcpy( buf, "     Materials: " );
  material_flags.sprint( &buf[16], &obj->materials );
  send( ch, buf );
  send( ch, "\r\n" );

  /* WEAR LOC */

  buf[0] = '\0';
  for( i = 1; i < MAX_ITEM_WEAR; i++ )
    if( is_set( &obj_clss->wear_flags, i ) ) {
      if( buf[0] == '\0' ) 
        sprintf( buf, "     Wear Loc.: %s", wear_name[i] );
      else
        sprintf( buf+strlen( buf ), ", %s", wear_name[i] );
    }

  if( is_set( obj_clss->extra_flags, OFLAG_NO_SHIELD ) )
    strcat( buf, " (two.handed)" );

  if( *buf != '\0' ) {
    send( ch, buf );
    send( ch, "\r\n" );
  }

  /* ANTI-FLAGS */

  buf[0] = '\0';
  for( i = 0; i < MAX_ANTI; i++ )
    if( is_set( &obj_clss->anti_flags, i ) ) {
      if( buf[0] == '\0' ) 
        sprintf( buf, "\r\n    Anti-Flags: %s", anti_flags[i] );
      else
        sprintf( buf+strlen( buf ), ", %s", anti_flags[i] );
    }

  if( *buf != '\0' ) {
    strcat( buf, "\r\n" );
    send( ch, buf );
  }

  buf[0] = '\0';
  for( i = 0; i < MAX_SFLAG; i++ )
    if( is_set( &obj_clss->size_flags, i ) ) {
      if( buf[0] == '\0' )
        sprintf( buf, "\r\n    Size-Flags: %s", size_flags[i] );
      else
        sprintf( buf+strlen( buf ), ", %s", size_flags[i] );
    }

  if( *buf != '\0' ) {
    strcat( buf, "\r\n" );
    send( ch, buf );
  }

  /* AFFECTS */

  for( i = 0; i < obj_clss->affected; i++ ) {
    paf = obj_clss->affected[i];
    if( paf->type == AFF_NONE ) { 
      if( !found ) {
        send( ch, "\r\n          Affects:\r\n" );
        found = TRUE;    
      }
      for( int j = 0; j < MAX_AFF_MODS; j++ ) {
        if( paf->mlocation[ j ] != APPLY_NONE )
          send( ch, "%12s%s by %d.\r\n", "", affect_location[ paf->mlocation[ j ] ], paf->mmodifier[ j ] );
      }
    }
  }

  for( i = 0; i < MAX_ENTRY_AFF_CHAR; i++ ) {
    if( is_set( obj_clss->affect_flags, i ) ) {
      if( !found ) {
        send( ch, "\r\n          Affects:\r\n" );
        found = TRUE;    
      }
      if( aff_char_table[i].id_line ) {
        send( ch, "%12s%s\r\n", "", aff_char_table[i].id_line );
      } else {
        bug( "Affect with no id.line: %s", aff_char_table[i].name );
      }
    }
  }
  
  for( i = 0; i < MAX_OFLAG; i++ ) {
    if( is_set( obj->extra_flags, i ) && oflag_ident[i][0] != '\0' ) {
      if( !found ) {
        send( ch, "\r\n          Affects:\r\n" );
        found = TRUE;    
      }
      send( ch, "%12s%s\r\n", "", oflag_ident[i] );
    } 
  }

  /*  GENERAL SYNERGIES */

  found = false;
  bool player = false;

  if( ch->pcdata == NULL )
    player = false;
  else if( is_set( ch->pcdata->pfile->flags, PLR_NUMERIC ) )
    player = true;
  else
    player = false;

  for( int i = 0; i < MAX_NEW_CLSS; i++ ) {
    if( obj->pIndexData->clss_synergy[i] != 0 ) {
      if( !found ) {
        send( ch, "\r\n          General Synergies\r\n" );
        found = true;
      }
      if( i == 0 )
        send( ch, "            Modifies defensive skills by %s%s.\r\n", obj->pIndexData->clss_synergy[i] < 0 ? player ? "-" : "negative " : "", number_word( abs( obj->pIndexData->clss_synergy[i] ), ch ) );
      else if( i < MAX_CLSS+1 )
        send( ch, "            Modifies a %s' skills by %s%s.\r\n", clss_table[i-1].name, obj->pIndexData->clss_synergy[i] < 0 ? player ? "-" : "negative " : "", number_word( abs( obj->pIndexData->clss_synergy[i] ), ch ) );
      else
        send( ch, "            Modifies an unfinished class.\r\n" );
    }
  }

  /*  SKILL SPECIFIC SYNERGIES */

  if( !is_empty( obj_clss->synergy_array ) ) {
    send( ch, "\r\n          Skill Specific Synergies\r\n" );
    for( int i = 0; i < obj_clss->synergy_array; i++ ) {
      synergy* syn = obj_clss->synergy_array[i];
      if( syn != NULL ) {
        send( ch, "            Modifies %s by %s%s.\r\n", syn->skill, syn->amount < 0 ? player ? "-" : "negative " : "", number_word( abs( syn->amount ), ch ) );
      }
      else {
        send( ch, "[BUG]" );
      }
    }
  }

  send( ch, "\r\n" );
  send( ch, scroll_line[0] );

  if( obj->array == NULL )
    obj->To( ch );

  in_character = TRUE;

  return TRUE;
};


bool spell_obscure( char_data* ch, char_data*, void* vo, int, int )
{

  obj_data*             obj  = (obj_data*) vo;
 
  if( null_caster( ch, SPELL_OBSCURE  ) )
    return TRUE;

  remove_bit( obj->extra_flags, OFLAG_IDENTIFIED );
  remove_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );
  
  return TRUE;

}

/*
 *   FLASH OF LIGHT
 */


bool spell_blind( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( victim->species != NULL && !is_set( victim->species->act_flags, ACT_HAS_EYES ) ) {
    send( ch, "%s does not see with %s eyes so blind has no affect.\r\n", victim, victim->His_Her( ) );
    return TRUE;
  }

  if( !can_kill( ch, victim ) )
    return TRUE;

  if( makes_save( victim, ch, RES_MAGIC, SPELL_BLIND, level ) )
    return TRUE;

  spell_affect( ch, victim, level, duration, SPELL_BLIND, AFF_BLIND );
  record_damage( victim, ch, 10 );

  return TRUE;
}


bool spell_blinding_light( char_data* ch, char_data*, void*, int level, int duration )
{
  char_data* victim;
  for( int i = ch->array->size - 1; i >= 0; i-- ) {
    victim = character( ch->array->list[i] );
    if ( victim == NULL )
      continue;

    if( victim == ch || !victim->Can_See( ) || ( victim->species != NULL && !is_set( victim->species->act_flags, ACT_HAS_EYES ) ) )
      continue;

    send( victim, "### The room explodes in a flash of light. ###\r\n" );

    if( makes_save( victim, ch, RES_MAGIC, SPELL_BLINDING_LIGHT, level ) ) {
      send( victim, "Fortunately you were not looking at the blast and are unaffected.\r\n" );
    } else if( !can_kill( ch, victim, false ) ) {
      send( victim, "Oddly the flash has no affect on you.\r\n" );
    } else {
      spell_affect( ch, victim, level, duration, SPELL_BLINDING_LIGHT, AFF_BLIND );
      record_damage( victim, ch, 10 );
    }
  }

  return TRUE;
}


/*
 *   ENTROPY SPELLS
 */

/*
bool spell_wither( char_data* ch, char_data* victim, void*, int level, int )
{
  affect_data affect;

  if( !can_kill( ch, victim ) )
    return TRUE;

  if( makes_save( victim, ch, RES_MAGIC, SPELL_WITHER, level ) ) {
    send( ch, "The spell fails.\r\n" );
    return TRUE;
  }

  send( victim, "You feel your skin shrivel.\r\n" );
  send_seen( victim, "%s seems to shrivel before you.\r\n", victim );
 
  affect.type      = AFF_NONE;
  affect.location  = APPLY_CON;
  affect.modifier  = -1; 
  affect.duration  = level*3;
  affect.level     = level;
  affect.leech     = NULL;

  add_affect( victim, &affect );

  damage_magic( victim, ch, spell_damage( SPELL_WITHER, level ), "*A withering stare" );

  return TRUE;
}
*/

bool spell_drain_life( char_data* ch, char_data* victim, void*, int level, int )
{
  affect_data affect;

  if( victim->shdata->race == RACE_UNDEAD ) {
    send( ch, "You cannot drain life from the undead.\r\n" );
    return TRUE;
  }

  if( !can_kill( ch, victim ) )
    return TRUE;

  if( makes_save( victim, ch, RES_MAGIC, SPELL_DRAIN_LIFE, level ) ) {
    send( ch, "The spell fails.\r\n" );
    return TRUE;
  }

  send( victim, "The chill of death momentarily touchs your soul.\r\n" );
  send_seen( victim, "%s shivers for a brief instant.\r\n", victim );
 
  affect.type           = AFF_NONE;
  affect.mlocation[ 0 ] = APPLY_CON;
  affect.mmodifier[ 0 ] = -1; 
  affect.duration       = level*3;
  affect.level          = level;
  affect.leech          = NULL;

  add_affect( victim, &affect );
  record_damage( victim, ch, 10 );

  heal_victim( victim, ch, spell_damage( SPELL_CURE_CRITICAL, level ) );

  return TRUE;
}


/*
 *   WEB SPELLS
 */


bool spell_web( char_data* ch, char_data* victim, void*, int level, int duration )
{
  affect_data* paf = NULL;

  if( null_caster( ch, SPELL_WEB ) || !victim )
    return TRUE;

  if( !can_kill( ch, victim ) )
    return TRUE;

  if( is_set( victim->affected_by, AFF_ENTANGLED ) ) {
    paf = find_affect( victim, AFF_ENTANGLED );
    if( paf != NULL ) {
      bool failed;
      paf->mmodifier[0] += evaluate( aff_char_table[AFF_ENTANGLED].mmodifier[0], failed );;   
      paf->duration = max( level/2, paf->duration+1 ); 
    }
    send( "The web around you thickens.\r\n", victim );
    send( *victim->array, "The web trapping %s thickens.\r\n", victim );
    record_damage( victim, ch, 10 );
    return TRUE;

  } else {
    send( victim, "A web begins to form around you.\r\n" );
    send( *victim->array, "A web begins to form around %s.\r\n", victim );

    if( !is_entangled( victim, "avoid the webbing", true ) 
      && makes_save( victim, ch, RES_DEXTERITY, SPELL_WEB, level ) && victim->position != POS_SLEEPING ) {
      send( victim, "Luckily you avoid becoming entangled.\r\n" );
      send( *victim->array, "%s skillfully avoids the web.\r\n", victim );
      return TRUE;
    }

    if( is_set( victim->affected_by, AFF_FIRE_SHIELD ) ) {
      send( victim, "As the webbing makes contact with your fire shield, it bursts into flames.\r\n" );
      send( *victim->array, "%s winces in pain as %s fire shield burns the web into nothingness.\r\n", victim, victim->His_Her() );
      damage_fire( victim, ch, 5 * number_range(level/2, level), "*The incinerating web" );
      return TRUE;
    }
  }

  spell_affect( ch, victim, level, duration, SPELL_WEB, AFF_ENTANGLED );
  record_damage( victim, ch, 10 );

  reveal( victim );
  disrupt_spell( victim );

  return TRUE;
}


/*
 *   MIND SPELLS
 */


bool spell_confuse( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( makes_save( victim, ch, RES_MIND, SPELL_CONFUSE, level/2 ) ) 
    return TRUE;

  spell_affect( ch, victim, level, duration, SPELL_CONFUSE, AFF_CONFUSED );
  return TRUE;
}


bool spell_hallucinate( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( makes_save( victim, ch, RES_MIND, SPELL_HALLUCINATE, level ) )
    return TRUE;

  spell_affect( ch, victim, level, duration, SPELL_HALLUCINATE, AFF_HALLUCINATE );
  return TRUE;
}


/*
 *   VISION SPELLS
 */


bool spell_sense_danger( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_SENSE_DANGER, AFF_SENSE_DANGER );

  return TRUE;
}


bool spell_eagle_eye( char_data* ch, char_data* victim, void*, int level, int )
{
  room_data* room;

  if( null_caster( ch, SPELL_EAGLE_EYE ) )
    return FALSE;

  room = ch->in_room;

  if( !victim || victim->in_room == NULL ) {
    send( ch, "They are somewhere unusual.\r\n" );
    return TRUE;
  }

  if( victim->in_room->area->status != AREA_OPEN || is_set( &victim->in_room->room_flags, RFLAG_NO_MAGIC )
    || ( is_apprentice( victim ) && get_trust( ch ) < get_trust( victim ) ) ) {
    send( ch, "The spell is mysteriously blocked.\r\n" );
    return TRUE;
  }

  if( level < 4 && is_set( &victim->in_room->room_flags, RFLAG_INDOORS ) ) {
    send( ch, "Your victim is not visible from the sky.\r\n" );
    return TRUE;
  }

  if( level < 8 && is_set( &victim->in_room->room_flags, RFLAG_UNDERGROUND ) ) {
    send( ch, "Your victim is nowhere above ground.\r\n" );
    return TRUE;
  }
  
  ch->From( );
  ch->To( victim->in_room );
//  ch->in_room = victim->in_room;
  send( ch, "\r\n" );
  do_look( ch, "" );
  ch->From( );
  ch->To( room );
//  ch->in_room = room;

  return TRUE;
}

bool spell_gryphons_watch( char_data* ch, char_data* victim, void*, int level, int )
{
  if( null_caster( ch, SPELL_GRYPHONS_WATCH ) )
    return FALSE;

  spell_eagle_eye( ch, victim, NULL, level, 0 );

  return TRUE;
}
/*
bool spell_scry( char_data* ch, char_data* victim, void*, int level, int )
{
  room_data* room;

  if( null_caster( ch, SPELL_SCRY ) )
    return FALSE;

  room = ch->in_room;

  if( victim->in_room == NULL ) {
    send( ch, "They are somewhere unusual.\r\n" );
    return TRUE;
  }

  if( victim->in_room->area->status != AREA_OPEN || is_set( &victim->in_room->room_flags, RFLAG_NO_MAGIC )
    || ( is_apprentice( victim ) && get_trust( ch ) < get_trust( victim ) ) ) {
    send( ch, "The spell is mysteriously blocked.\r\n" );
    return TRUE;
  }

  if( level < 4 && is_set( &victim->in_room->room_flags, RFLAG_INDOORS ) ) {
    send( ch, "Your victim is not visible from the sky.\r\n" );
    return TRUE;
  }

  if( level < 8 && is_set( &victim->in_room->room_flags, RFLAG_UNDERGROUND ) ) {
    send( ch, "Your victim is nowhere above ground.\r\n" );
    return TRUE;
  }

  ch->in_room = victim->in_room;
  send( ch, "\r\n" );
  do_look( ch, "" );
  ch->in_room = room;

  return TRUE;
}



bool spell_hawks_view( char_data* ch, char_data* victim, void*, int level, int )
{
  room_data* room;

  if( null_caster( ch, SPELL_HAWKS_VIEW ) )
    return FALSE;

  room = ch->in_room;

  if( victim->in_room == NULL ) {
    send( ch, "They are somewhere unusual.\r\n" );
    return TRUE;
    }

  if( victim->in_room->area->status != AREA_OPEN || is_set( &victim->in_room->room_flags, RFLAG_NO_MAGIC )
    || ( is_apprentice( victim ) && get_trust( ch ) < get_trust( victim ) ) ) {
    send( ch, "The spell is mysteriously blocked.\r\n" );
    return TRUE;
  }

  if( level < 4 && is_set( &victim->in_room->room_flags, RFLAG_INDOORS ) ) {
    send( ch, "Your victim is not visible from the sky.\r\n" );
    return TRUE;
  }

  if( level < 8 && is_set( &victim->in_room->room_flags, RFLAG_UNDERGROUND ) ) {
    send( ch, "Your victim is nowhere above ground.\r\n" );
    return TRUE;
  }

  ch->in_room = victim->in_room;
  send( ch, "\r\n" );
  do_look( ch, "" );
  ch->in_room = room;

  return TRUE;
}
*/


/*
 *   INVULNERABILITY
 */


bool spell_invulnerability( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_INVULNERABILITY, AFF_INVULNERABILITY );

  return TRUE;
}


/*
 *   LEVITATION/FLY
 */


bool spell_float( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_FLOAT, AFF_FLOAT );

  return TRUE;
}


bool spell_fly( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_FLY, AFF_FLY );

  return TRUE;
}

bool spell_ethereal_bridge( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_ETHEREAL_BRIDGE, AFF_SOLIDIFY );
  
  return TRUE;
}


/*
 *  SLOW/HASTE
 */


bool spell_haste( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_HASTE, AFF_HASTE );

  return TRUE;
}


bool spell_slow( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( makes_save( victim, ch, RES_MAGIC, SPELL_SLOW, level ) ) 
    return TRUE;

  spell_affect( ch, victim, level, duration, SPELL_SLOW, AFF_SLOW );
  record_damage( victim, ch, 10 );

  return TRUE;
}










