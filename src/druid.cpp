#include "system.h"


bool        non_water_type        ( room_data* );

/*
 *    Shape Changing Skill
 */

void dream_update( player_data* player )
{
  if( player->get_skill( SKILL_SHAPE_CHANGE ) == UNLEARNT )
    return;

  if( player->position != POS_SLEEPING )
    return;

  // initialize an array of possible shapes for player
  int shapes [ MAX_SHAPE_CHANGE ];
  int number = 0;
  int slevel = player->get_skill( SKILL_SHAPE_CHANGE );
  int level  = player->shdata->level;
  
  for( int i = 0; i < MAX_ENTRY_SHAPE_CHANGE; i++ ) {
    shapes[i] = -1;
    if( shape_table[i].open && shape_table[i].level <= level
      && shape_table[i].skill_level <= slevel && *shape_table[i].dream != '\0' ) {
      shapes[number] = i;
      number++;
    }
  }
  
  if( number > 0 )
    act( (char_data*) player, shape_table[ shapes[ number_range( 0, number-1 ) ] ].dream, (char_data*) player, NULL, NULL, NULL, VIS_ALL );

  return;
}

void do_shift( char_data* ch, char* argument )
{
  int                form = -1;
  species_data*   species;
  char_data*       victim;
  char*         arg = argument; 

  if( ch == NULL || ch->link == NULL )
    return;

  if( !ch->get_skill( SKILL_SHAPE_CHANGE ) ) {
    send( ch, "You do not know how to change into another shape.\r\n" );
    return;
  }

  if( ch->fighting != NULL && ch->get_skill( SKILL_COMBAT_CHANGE ) == UNLEARNT ) {
    send( ch, "You do not have the necessary skill to shift during combat.\r\n" );
    return;
  }

  if( ch->fighting != NULL )
    ch->improve_skill( SKILL_COMBAT_CHANGE );

  if( *argument == '\0' ) {
    send( ch, "What form do you wish to change into?\r\n" );
    return;
  }

  if( exact_match( argument, "return" ) ) {
    if( ch->shifted == NULL ) {
      send( ch, "You aren't shifted.\r\n" );
      return;
    }
    
    if( ch->fighting != NULL ) {
      send( ch, "You haven't the skill or ability to do that.\r\n" );
      return;
    }

    if( is_set( ch->affected_by, AFF_HIDE ) )
      strip_affect( ch, AFF_HIDE );
    if( is_set( ch->affected_by, AFF_CAMOUFLAGE ) )
      strip_affect( ch, AFF_CAMOUFLAGE );
    if( is_set( ch->affected_by, AFF_SNEAK ) )
      strip_affect( ch, AFF_SNEAK );

    remove_bit( &ch->status, STAT_HIDING );
    remove_bit( &ch->status, STAT_CAMOUFLAGED );

    remove_bit( ch->pcdata->pfile->flags, PLR_SNEAK );

    send( ch, "You return to your natural shape.\r\n" );
    send_seen( ch, "%s looks around in concentration, and reverts to %s natural form.\r\n", ch, ch->His_Her( ) );
    ch->shifted = NULL;

    int delay = 60-3*10*ch->get_skill( SKILL_COMBAT_CHANGE )/10;
    set_delay( ch, delay );
    return;
  }

  if( ch->shifted != NULL ) {
    send( ch, "You must return to your natural shape to assume another.\r\n" );
    return;
  }

  if( MAX_ENTRY_SHAPE_CHANGE == 0 ) {
    bug( "There are no entries in the shape.change table." );
    send( ch, "Shape change is buggy right now, post a bug note" );
    return;
  }

  for( int i = 0; i < MAX_ENTRY_SHAPE_CHANGE; i++ ) {
    const char* shape = shape_table[i].name;
    if( exact_match( arg, shape ) )
      form = i;
  }

  if( form != -1 && !is_apprentice( ch ) ) {
    if( !shape_table[form].open ) {
      send( ch, "That form is not currently available for selection.\r\n" );
      return;
    }
    if( shape_table[form].level > ch->shdata->level ) {
      send( ch, "You need to gain more experience to assume that form.\r\n" );
      return;
    }
    if( shape_table[form].skill_level > ch->get_skill( SKILL_SHAPE_CHANGE ) ) {
      send( ch, "You must learn more about shifting your form to assume that shape.\r\n" );
      return;
    }
  }

  if( form == -1 ) {
    send( ch, "%s is not one of the forms you can change into.\r\n", argument );
    return;
  }

  if( ( species = get_species( shape_table[form].mob ) ) == NULL ) {
    fsend( ch, "This form has not been finished yet, please notify an immortal that %s shape is not finished.", shape_table[form].name );
    return;
  }

  if( is_set( species->affected_by, AFF_MOVE_WATER_ONLY ) && non_water_type( ch->in_room ) ) {
    fsend( ch, "That form would be far more comfortable in the water.\r\n" );
    return;
  }

  ch->shifted = species;

  victim = create_mobile( species );

  reveal( victim );

  act( ch, "You begin your transformation into $N.\r\n", ch, victim, NULL, NULL, VIS_SIGHT );
  act_notchar( "$n gains a look of peacefulness and $s muscles ripple.\r\n", ch, victim, NULL, NULL, VIS_SIGHT );

  ch->improve_skill( SKILL_SHAPE_CHANGE );
  act( ch, "You have completed your transformation into $N.\r\n", ch, victim, NULL, NULL, VIS_SIGHT );
  act_notchar( "$n has assumed the form of $N.\r\n", ch, victim, NULL, NULL, VIS_SIGHT );

  victim->Extract( );

  int delay = 60-3*ch->get_skill( SKILL_COMBAT_CHANGE );

  set_delay( ch, delay );
  return;
}

/*
 *    Harvest Skill
 */

void do_harvest( char_data* ch, char* )
{
  int       harvest_attempt = ch->in_room->Get_Integer( HARVEST_ATTEMPT );
  int            poss_entry = 0;
  int                  item, item2 = 0;
  int             possibles [ MAX_HARVEST ] [ MAX_HARVEST ];
  int                 level = 10*ch->get_skill( TRADE_HARVEST )/10;
  int                 moves = move_cost( ch, TRADE_HARVEST );
  obj_data*             obj;
  obj_clss_data*   obj_clss;
  int                     i;

  if( ch->pcdata == NULL ) {
    send( ch, "As a creature you are unable to do that.\r\n" );
    return;
  }

  if( terrain_table[ ch->in_room->sector_type ].civilized ) {
    send( ch, "How do you expect to harvest something %s %s.\r\n", ch->in_room->sector_type == SECT_CITY 
      ? "on a" : ch->in_room->sector_type == SECT_ROAD ? "on a" : "in a", terrain_table[ ch->in_room->sector_type ].terrain_name );
    return;
  }

  if( ch->move < moves ) {
    send( ch, "You are too exhausted to try and harvest anything.\r\n" );
    return;
  }

  ch->move -= moves;

  if( harvest_attempt > 5 ) {
    send( ch, "You search the landscape, but it appears you have harvested all you can.\r\n" );
    return;
  }

  harvest_attempt++;
  ch->in_room->Set_Integer( HARVEST_ATTEMPT, harvest_attempt );

  for( i = 0; i < MAX_ENTRY_HARVEST; i++ ) {
    if( ( harvest_table[i].terrain == 0 || is_set( &harvest_table[i].terrain, ch->in_room->sector_type ) )
      && harvest_table[i].open && level >= harvest_table[i].skill
      && harvest_table[i].level <= ch->shdata->level &&
      is_set( &harvest_table[i].clss, ch->pcdata->clss )
      && ( harvest_table[i].difficulty == 0 || 
      (number_range( 0, 99 )-level/2) < harvest_table[i].difficulty ) ) {
      if( harvest_table[i].object[0] != 0 ) {
        possibles [ poss_entry ] [ 0 ] = harvest_table[i].object[0];
        possibles [ poss_entry ] [ 1 ] = i;
        poss_entry++;
      }
      else if( harvest_table[i].rtable >= 0 && harvest_table[i].rtable < rtable_list ) {
        int num = reset_table_load( find_rtable( harvest_table[i].rtable ) );
        if( num != 0 ) {
          possibles[ poss_entry ] [ 0 ] = num;
          possibles [ poss_entry ] [ 1 ] = i;
          poss_entry++;
        }
      }
    }
  }

  if( MAX_ENTRY_HARVEST == 0 ) {
    bug( "Harvest Table has no entries." );
    send( ch, "Harvest has not been completed, or the entries have all been deleted.  Please post a bug note about this.\r\n" );
    return;
  }

  if( poss_entry == 0 ) {
    send( ch, "You are unable to find anything to harvest.\r\n" );
    return;
  }

  item = number_range( 0, poss_entry-1 );
  i    = possibles[ item ][ 1 ];
  item = possibles[ item ][0];

  if( ( obj_clss = get_obj_index( item ) ) == NULL ) {
    bug( "Item set to NULL item. %i harvest table entry", i );
    send( ch, "Harvest item was NULL.\r\n" );
    return;
  }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Harvest: NULL object %i harvest table entry", i );
    return;
  }

  // successful harvest messages
  if( *harvest_table[item2].harvest_msg != '\0' )
    act( ch, harvest_table[i].harvest_msg, ch, obj );
  else
    send( ch, "You manage to harvest %s from the %s.\r\n", obj, terrain_table[ch->in_room->sector_type].terrain_name );
  if( *harvest_table[i].rharvest_msg != '\0' )
    act_notchar( harvest_table[i]. rharvest_msg,ch, NULL, obj );
  else
    send_seen( ch, "%s manages to harvest %s from the %s.\r\n", ch, obj, tolower( *terrain_table[ch->in_room->sector_type ].terrain_name ) );
  
  obj->To( ch );

  ch->improve_skill( TRADE_HARVEST );

  int delay = ch->species != NULL ? 6 : 30-5*ch->get_skill( TRADE_HARVEST )/MAX_SKILL_LEVEL;
  set_delay(ch, delay);

  return;
}

/*
 *    Here is the beginning of the druid spells.
 */

/*
 *    Utility Spells
 */

bool spell_shillelagh( char_data* ch, char_data*, void* vo, int level, int duration )
{
  obj_data* staff = (obj_data*) vo;

  if( staff == NULL ) {
    bug( "NULL object in shillelagh?!?" );
    return TRUE;
  }

  if( staff->value[3] != WEAPON_STAFF-WEAPON_FIRST ) {
    send( ch, "Shillelagh can only be used upon staves.\r\n" );
    return TRUE;
  }

  if( is_set( staff->extra_flags, OFLAG_MAGIC ) || staff->value[0] != 0 ) {
    send( ch, "%s already has a magical affect.\r\n", staff );
    return TRUE;
  }

  if( 5*ch->get_skill( SPELL_SHILLELAGH )+ch->shdata->level/2 < staff->pIndexData->level ) {
    send( ch, "Your ability is not sufficient to alter %s.\r\n", staff );
    return TRUE;
  }

  if( number_range( 0, 100 ) > 7*ch->get_skill( SPELL_SHILLELAGH ) ) {
    send( ch, "You fail to call the ancient druids power to %s.\r\n", staff );
    return TRUE;
  }

  set_bit( staff->extra_flags, OFLAG_MAGIC );
  staff->value[0]++;

  send( ch, "%s is overshadowed by the ancient power of druids.\r\n", staff );
  return TRUE;
}

bool spell_water_walk( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_WATER_WALK, AFF_WATER_WALK );

  return TRUE;
}

bool spell_wind_walk( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_WIND_WALK, AFF_WIND_WALK );

  return TRUE;
}


/*
 *    Defense Spells
 */

bool spell_green_finger( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_GREEN_FINGER, AFF_GREEN_FINGER );

  return TRUE;
}

bool spell_natures_boon( char_data* ch, char_data*, void* vo, int level, int duration )
{
  obj_data*             obj   = (obj_data*) vo;
  obj_clss_data*   obj_clss;

  if( obj == NULL ) {
    send( ch, "NULL object for natures boon?!?\r\n" );
    bug( "NULL oobject for natures boon." );
    return FALSE;
  }

  if( obj->pIndexData->vnum != OBJ_ACORN ) {
    send( ch, "This spell may only be casted upon an acorn.\r\n" );
    return FALSE;
  }

  act( ch, "You pass your hands over $p and it shimmers a second.\r\n", ch, obj );
  act_notchar( "$n passes $s hands over $p and it shimmers a second.\r\n", ch, NULL, obj );

  act_room( ch->in_room, "$p vanishes, and in its place is a small necklace.\r\n", ch, obj );

  obj->Extract( 1 );

  if( ( obj_clss = get_obj_index( OBJ_ACORN_NECKLACE ) ) == NULL ) {
    bug( "OBJ_ACORN_NECKLACE (should be vnum: %i was NULL", OBJ_ACORN_NECKLACE );
    send( ch, "Acorn necklace item was NULL.\r\n" );
    return TRUE;
  }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Natures Boon  NULL object" );
    return TRUE;
  }

  obj->To( ch );

  return TRUE;
}

bool spell_natures_shielding( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_NATURES_SHIELDING, AFF_NATURES_SHIELDING );

  return TRUE;
}

bool spell_obscuring_mist( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_OBSCURING_MIST, AFF_OBSCURING_MIST );

  return TRUE;
}

/*
 *    Protective Spells (damage reduction defensive spells)
 */

bool spell_tree_form( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_TREE_FORM, AFF_TREE_FORM );

  return TRUE;
}

/*
 *    Cure Spells
 */

bool spell_circle_healing( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_CIRCLE_HEALING, AFF_CIRCLE_HEALING );

  return TRUE;
}

bool spell_druidic_ring( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DRUIDIC_RING, AFF_DRUIDIC_RING );

  return TRUE;
}

bool spell_good_berry( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data* berry = (obj_data*) vo;
  obj_clss_data*         good_berry;
  obj_data*                     obj;

  if( vo == NULL )
    return FALSE;

  if( berry->pIndexData->vnum != OBJ_BRAMBERRY ) {
    send( ch, "This spell will only work upon a simple bramberry.\r\n" );
    return FALSE;
  }

  if( ( good_berry = get_obj_index( OBJ_GOOD_BERRY ) ) == NULL ) {
    send( ch, "The object good berry does not exist, please post a note about this.\r\n" );
    return FALSE;
  }

  send( ch, "%s becomes %s.\r\n", berry, good_berry );

  obj = create( good_berry );

  obj->Set_Integer( GOOD_BERRY, 3*ch->get_skill( SPELL_GOOD_BERRY ) );

  berry->Extract( 1 );
  obj->To( ch );
  consolidate( obj );

  return TRUE;
}

bool spell_blessed_fruit( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data* berry = (obj_data*) vo;
  obj_clss_data*         good_berry;
  obj_data*                     obj;

  if( vo == NULL )
    return FALSE;

  if( berry->pIndexData->vnum != OBJ_RED_GREEN_FRUIT ) {
    send( ch, "This spell will only work upon a simple piece of red and green fruit.\r\n" );
    return FALSE;
  }

  if( ( good_berry = get_obj_index( OBJ_BLESSED_FRUIT ) ) == NULL ) {
    send( ch, "The object blessed fruit does not exist, please post a note about this.\r\n" );
    return FALSE;
  }

  send( ch, "%s becomes %s.\r\n", berry, good_berry );

  obj = create( good_berry );
  obj->Set_Integer( GOOD_BERRY, 3*ch->get_skill( SPELL_BLESSED_FRUIT ) );

  berry->Extract( 1 );
  obj->To( ch );
  consolidate( obj );

  return TRUE;
}

bool spell_gift_of_the_land( char_data* ch, char_data*, void* vo, int level, int )
{
  obj_data*              berry = (obj_data*) vo;
  obj_clss_data*    good_berry;
  obj_data*                obj;

  if( vo == NULL )
    return FALSE;

  if( berry->pIndexData->item_type != ITEM_FOOD || !is_set( &berry->materials, MAT_FLESH ) ) {
    send( ch, "This spell will only work upon a simple piece of meat.\r\n" );
    return FALSE;
  }

  if( ( good_berry = get_obj_index( OBJ_BLESSED_MEAL ) ) == NULL ) {
    send( ch, "The object blessed meal does not exist, please post a note about this.\r\n" );
    return FALSE;
  }

  send( ch, "%s becomes %s.\r\n", berry, good_berry );

  obj = create( good_berry );
  obj->Set_Integer( GOOD_BERRY, 3*ch->get_skill( SPELL_GIFT_OF_THE_LAND ) );

  berry->Extract( 1 );
  obj->To( ch );
  consolidate( obj );

  return TRUE;
}

bool spell_caress_of_spring( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_CARESS_OF_SPRING, level ) );

  return TRUE;
}

bool spell_breath_of_life( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_BREATH_LIFE, level ) );

  return TRUE;
}

bool spell_embrace_of_the_world( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_EMBRACE_WORLD, level ) );

  return TRUE;
}

bool spell_natures_health( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_NATURES_HEALTH, level ) );
  
  return TRUE;
}

bool spell_companion_health( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_COMPANION_HEALTH, level ) );

  return TRUE;
}

bool spell_natures_restoration( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_NATURES_RESTORATION, level ) );

  return TRUE;
}

bool spell_touch_healing( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_TOUCH_HEALING, level ) );

  return TRUE;
}

/*
 *    Statistical Spells (+str/con/etc spells)
 */

bool spell_trail_blaze( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_TRAIL_BLAZE, AFF_TRAIL_BLAZE );

  return TRUE;
}

bool spell_path_find( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_PATH_FIND, AFF_PATH_FIND );

  return TRUE;
}

bool spell_owls_gift( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_GIFT_OWL, AFF_GIFT_OWL );

  return TRUE;
}

bool spell_caress_nature( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_CARESS_NATURE, AFF_CARESS_NATURE );

  return TRUE;
}

bool spell_bears_gift( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_GIFT_BEAR, AFF_GIFT_BEAR );

  return TRUE;
}

bool spell_snakes_gift( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_GIFT_SNAKE, AFF_GIFT_SNAKE );

  return TRUE;
}

bool spell_lions_gift( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_GIFT_LION, AFF_GIFT_LION );

  return TRUE;
}

bool spell_caress_gaia( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_CARESS_GAIA, AFF_CARESS_GAIA );

  return TRUE;
}

bool spell_hawks_gift( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_GIFT_HAWK, AFF_GIFT_HAWK );

  return TRUE;
}

bool spell_wyrms_gift( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_GIFT_WYRM, AFF_GIFT_WYRM );

  return TRUE;
}

bool spell_wilderness_lore( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_WILDERNESS_LORE, AFF_WILDERNESS_LORE );

  return TRUE;
}

/*
 *    Pet Spells (taming, buffing, altering)
 */


/*
 *    Offensive Spells (put elemental spells in their proper element file)
 */

bool spell_poison_touch( char_data* ch, char_data* victim, void*, int level, int )
{
  affect_data affect;

  if( !makes_save( victim, ch, RES_POISON, SPELL_POISON_TOUCH, level ) && victim->hit > 0 ) {
    affect.type           = AFF_POISON;
    affect.duration       = number_range(3,7) * 10;
    affect.level          = 8;
    affect.leech          = NULL;
    affect.mlocation[ 0 ] = APPLY_NONE;
    affect.mmodifier[ 0 ] = 0;

    add_affect( victim, &affect );
  }

  damage_element( victim, ch, spell_damage( SPELL_POISON_TOUCH, level ), "*The poisonous touch", ATT_POISON );
  
  return TRUE;
}

    
bool spell_insect_plague( char_data* ch, char_data*, void*, int level, int duration )
{
  char_data* rch;

  if( null_caster( ch, SPELL_INSECT_PLAGUE ) )
    return TRUE;

  for( int i = *ch->array-1; i >= 0; i-- )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && can_kill( ch, rch, false ) && rch->Seen( ch ) ) {
      damage_element( rch, ch, spell_damage( SPELL_INSECT_PLAGUE, level ), "*The insect plague", ATT_PHYSICAL );
      if( rch->hit > 0 && !makes_save( rch, ch, RES_MAGIC, SPELL_INSECT_PLAGUE, level ) )
        spell_affect( ch, rch, level, duration, SPELL_INSECT_PLAGUE, AFF_BLIND );
    }

  return TRUE;
}

bool spell_whirlwind( char_data* ch, char_data* victim, void*, int level, int )
{
//  damage_element( victim, ch, spell_damage( SPELL_WHIRLWIND, level. 0, ch ), "*The raging wind", ATT_PHYSICAL );
  damage_element( victim, ch, spell_damage( SPELL_WHIRLWIND, level ), "*the raging wind", ATT_PHYSICAL );
  return TRUE;
}

bool spell_summon_swarm( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_SUMMON_SWARM, level ), "*The insect swarm", ATT_PHYSICAL );

  return TRUE;
}

bool spell_spike_growth( char_data* ch, char_data* victim, void*, int level, int duration )
{
  char_data* rch;

  if( null_caster( ch, SPELL_SPIKE_GROWTH ) )
    return TRUE;

  for( int i = *ch->array-1; i >= 0; i-- )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && can_kill( ch, rch, false ) && rch->Seen( ch ) &&
      !rch->can_fly( ) ) {
      if( !makes_save( rch, ch, RES_MAGIC, SPELL_SPIKE_GROWTH, level ) )
        spell_affect( ch, victim, level, duration, SPELL_SPIKE_GROWTH, AFF_SLOW );
      damage_element( rch, ch, spell_damage( SPELL_SPIKE_GROWTH, level ), "*The spiked growth", ATT_PHYSICAL );
    }

  return TRUE;
}

bool spell_natures_fury( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_NATURES_FURY, level ), "*The fiery elements", ATT_FIRE );
  if( ch->hit <= 0 )
    return TRUE;
  damage_element( victim, ch, spell_damage( SPELL_NATURES_FURY, level ), "*The freezing elements", ATT_COLD );
  if( ch->hit <= 0 )
    return TRUE;
  damage_element( victim, ch, spell_damage( SPELL_NATURES_FURY, level ), "*The howling wind", ATT_PHYSICAL );
  if( ch->hit <= 0 )
    return TRUE;
  damage_element( victim, ch, spell_damage( SPELL_NATURES_FURY, level ), "*The raging lightning", ATT_SHOCK );

  return TRUE;
}

bool spell_defoliation( char_data* ch, char_data* victim, void*, int level, int )
{
  damage_element( victim, ch, spell_damage( SPELL_DEFOLIATION, level ), "*The choking summer", ATT_PHYSICAL );

  return TRUE;
}

bool spell_parching_grasp( char_data* ch, char_data* victim, void*, int level, int )
{
  if( victim->pcdata != NULL )
    victim->pcdata->condition[COND_THIRST] = -50;

  damage_element( victim, ch, spell_damage( SPELL_PARCHING_GRASP, level ), "*The horrid sandstorm", ATT_PHYSICAL );

  return TRUE;
}

bool spell_entangle( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !makes_save( victim, ch, RES_MAGIC, SPELL_ENTANGLE, level ) )
    spell_affect( ch, victim, level, duration, SPELL_ENTANGLE, AFF_SLOW_ENTANGLE );

  if( !makes_save( victim, ch, RES_MAGIC, SPELL_ENTANGLE, level ) ) 
    spell_affect( ch, victim, level, duration, SPELL_ENTANGLE, AFF_WEB_ENTANGLE );

  return TRUE;
}

/*
 *    Druid Damaging Shield Spells
 */

bool spell_bramble_skin( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_BRAMBLE_SKIN, AFF_BRAMBLE_SKIN );

  return TRUE;
}

bool spell_thistle_skin( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_THISTLE_SKIN, AFF_THISTLE_SKIN );

  return TRUE;
}
