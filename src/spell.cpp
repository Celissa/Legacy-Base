#include "system.h"


bool spell_bug( char_data* ch, char_data* victim, void*, int, int )
{
  beetle( "spell_bug called" );
  return TRUE;
}

bool spell_amnesia( char_data* ch, char_data* victim, void*, int, int )
{
  cast_data*  cast;
  int         i = 0;

  if( ch == NULL ) {
    ch = victim;
    if( ch == NULL )
      return TRUE;
  }

  send( ch, "You feel very confused!\r\n" );
  send( ch, "Your memory seems to have gone blank!\r\n" );

  if( ch->species != NULL )
    return TRUE;

  ch->pcdata->practice = (int)(7*total_pracs( ch )/10);

  for( i = 0; i < MAX_SKILL; i++ )
    ch->pcdata->skill[i] = 0;

  ch->pcdata->speaking = LANG_HUMANIC+ch->shdata->race;
  ch->pcdata->skill[ LANG_HUMANIC+ch->shdata->race ] = 10;
  ch->pcdata->skill[ LANG_COMMON ] = 10; 

  for( ; ; ) {
    if( ( cast = ch->prepare ) == NULL )
      break;
    ch->prepare = cast->next;
    delete cast;
  }

  return TRUE;
}


bool spell_magic_mapping( char_data* ch, char_data*, void*, int, int )
{
  show_map( ch, 60, 20 );

  return TRUE;
}


bool spell_youth( char_data* ch, char_data* victim, void* vo, int, int duration )
{
  player_data*  pc;
  obj_data*        obj  = (obj_data*) vo;

  if( duration == -4 )
    return TRUE;

  if( duration == -3 ) {
    if( obj->age > 0 ) {
      send( *ch->array, "%s appears less deteriorated by age.\r\n", obj );
      obj->age = max( 0, obj->age-10 );
    }
    return TRUE;
  }

  if( ( pc = player( victim ) ) == NULL )
    return TRUE;

  if( pc->Age( ) > plyr_race_table[ victim->shdata->race ].start_age ) {
    pc->base_age--;
    send( victim, "You feel younger!\r\n" );
  }

  return TRUE;
} 


/*
 *   CLERIC/PALADIN SPELLS
 */


bool spell_cause_light( char_data *ch, char_data *victim, void*, int level, int )
{
  bool good = ch == NULL ? false : ch->pcdata == NULL ? is_good( ch ) : ch->pcdata->religion == 0 ? is_good( ch ) 
    : is_int_good( religion_table[ch->pcdata->religion].galign );

  if( good )
    damage_holy( victim, ch, spell_damage( SPELL_CAUSE_LIGHT, level ), "*The spell" );
  else
    damage_unholy( victim, ch, spell_damage( SPELL_CAUSE_LIGHT, level ), "*the spell" );

  return TRUE;
}


bool spell_cause_serious( char_data *ch, char_data *victim, void*, int level, int )
{
  bool good = ch == NULL ? false : ch->pcdata == NULL ? is_good( ch ) : ch->pcdata->religion == 0 ? is_good( ch ) 
  : is_int_good( religion_table[ch->pcdata->religion].galign );

  if( good )
    damage_holy( victim, ch, spell_damage( SPELL_CAUSE_SERIOUS, level ), "*The spell" );
  else
    damage_unholy( victim, ch, spell_damage( SPELL_CAUSE_SERIOUS, level ), "*the spell" );
  
  return TRUE;
}


bool spell_cause_critical( char_data *ch, char_data *victim, void*, int level, int )
{
  bool good = ch == NULL ? false : ch->pcdata == NULL ? is_good( ch ) : ch->pcdata->religion == 0 ? is_good( ch ) 
    : is_int_good( religion_table[ch->pcdata->religion].galign );

  if( good )
    damage_holy( victim, ch, spell_damage( SPELL_CAUSE_CRITICAL, level ), "*The spell" );
  else
    damage_unholy( victim, ch, spell_damage( SPELL_CAUSE_CRITICAL, level ), "*the spell" );

  return TRUE;
}


bool spell_create_food( char_data *ch, char_data*, void*, int level, int )
{
  obj_data*            obj;  
  obj_clss_data*  obj_clss;
  int                 item;
  int           poss_entry = 0;
  int            possibles [ MAX_CREATE_FOOD ];

  if( null_caster( ch, SPELL_CREATE_FOOD ) )
    return TRUE;
/*
  level = range( 1, level, 10 );

  for( ; ; ) {
    item = number_range( 0, 3*level-1 ); 
    item = max( number_range( 0, 3*level-1 ), item ); 
    item = list_value[ LIST_CREATE_FOOD ][ item ];

    if( ( obj_clss = get_obj_index( item ) ) != NULL )
      break;
  }
*/
  for( int i = 0; i < MAX_ENTRY_CREATE_FOOD; i++ ) {
    if( create_food_table[i].open && level >= create_food_table[i].level ) {
      possibles[ poss_entry ] = create_food_table[i].food[0];
      poss_entry++;
    }
  }

  if( MAX_ENTRY_CREATE_FOOD == 0 ) {
    bug( "Create Food Table has no entries." );
    send( ch, "Create Food has not been completed, or the entries have all been deleted.  Please post a bug note about this.\r\n" );
    return FALSE;
  }

  if( poss_entry == 0 ) {
    send( ch, "You are unable to create any food?!?.\r\n" );
    return FALSE;
  }

  item = number_range( 0, poss_entry-1 );
  item = possibles[ item ];

  if( ( obj_clss = get_obj_index( item ) ) == NULL ) {
    bug( "Item set to NULL item." );
    send( ch, "Create Food item was NULL.\r\n" );
    return FALSE;
  }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Create_food: NULL object" );
    return TRUE;
  }  

  obj->To( ch );

  send( ch, "%s appears in your hand.\r\n", obj );
  send_seen( ch, "%s appears in %s's hand.\r\n", obj, ch );

  consolidate( obj );
  
  return TRUE;    
}



bool spell_create_feast( char_data *ch, char_data*, void*, int level, int )
{
  int pieces = (level/2) + number_range( 2, 6 );
  for( int i = 0; i < pieces; i++ )
    spell_create_food( ch, NULL, NULL, level, 0 );

  return TRUE;
}

bool spell_cure_blindness( char_data* ch, char_data* victim, void*, int, int )
{
  if( !is_set( victim->affected_by, AFF_BLIND ) ) {
    if( ch != victim )
      send( ch, "%s wasn't blind.\r\n", victim );
    else
      send( ch, "You aren't blind!\r\n" );
    return TRUE;
  }

  strip_affect( victim, AFF_BLIND );

  if( is_set( victim->affected_by, AFF_BLIND ) ) {
    if( ch != victim )
      send( ch, "%s is still blind!!\r\n", victim );
    send( victim, "You are still blind!!\r\n" );
  }

  return TRUE;
}


bool spell_curse( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_CURSE, AFF_CURSE );
  record_damage( victim, ch, 10 );

  return TRUE;
}


bool spell_detect_evil( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DETECT_EVIL, AFF_DETECT_EVIL );

  return TRUE;
}


bool spell_detect_good( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DETECT_GOOD, AFF_DETECT_GOOD );

  return TRUE;
}


bool spell_faerie_fire( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_FAERIE_FIRE, AFF_FAERIE_FIRE );
  record_damage( victim, ch, 10 );

  reveal( victim );
  return TRUE;
}


bool spell_harm( char_data *ch, char_data *victim, void*, int level, int )
{
  bool good = ch == NULL ? false : ch->pcdata == NULL ? is_good( ch ) : ch->pcdata->religion == 0 ? is_good( ch ) 
    : is_int_good( religion_table[ch->pcdata->religion].galign );

  if( good )
    damage_holy( victim, ch, spell_damage( SPELL_HARM, level ), "*The heaven sent bolt of energy" );
  else
    damage_unholy( victim, ch, spell_damage( SPELL_HARM, level ), "*the hell sent bolt of energy" );

  return TRUE;
}


bool spell_neutralize( char_data *ch, char_data *victim, void*, int, int )
{
  if( !is_set( victim->affected_by, AFF_HALLUCINATE ) && !is_set( victim->affected_by, AFF_SILENCE ) 
    && !is_set( victim->affected_by, AFF_CONFUSED ) ) {
    if( ch != victim )
      send( ch, "%s was not hallucinating, silenced or confused.\r\n", victim );
    else
      send( victim, "You weren't hallucinating, silenced or confused so the spell had no affect.\r\n" );
    return TRUE;
  }

  strip_affect( victim, AFF_HALLUCINATE );
  strip_affect( victim, AFF_SILENCE );
  strip_affect( victim, AFF_CONFUSED );

  if( is_set( victim->affected_by, AFF_HALLUCINATE ) ) {
    send( ch, "%s is still hallucinating!!\r\n", victim );
    send( "You are still hallucinating!!\r\n", victim );
  }

  if( is_set( victim->affected_by, AFF_SILENCE ) ) {
    send( ch, "%s is still silenced!!\r\n", victim );
    send( "You are still silenced!!\r\n", victim );
  }

  if( is_set( victim->affected_by, AFF_CONFUSED ) ) {
    send( ch, "%s is still confused!!\r\n", victim );
    send( "You are still confused!!\r\n", victim );
  }
  return TRUE;
}


bool spell_revitalize( char_data* ch, char_data* victim, void* obj, int level, int )
{
  int move;

  if( obj != NULL && ( victim == NULL || ch == NULL ) )
    return TRUE;

  move = victim->move+15*level;

  strip_affect( victim, AFF_DEATH );
  update_maxes( victim );
  
  if( move >= victim->max_move ) {
    send( victim, "You are completely revitalized.\r\n" );
    send( *victim->array, "%s is completely revitalized.\r\n", victim );
    victim->move = victim->max_move;
    return TRUE;
  }
  
  send( "You are partially revitalized.\r\n", victim );
  send( *victim->array, "%s is partially revitalized.\r\n", victim );
        
  victim->move = move;

  return TRUE;
}


bool spell_slay( char_data *ch, char_data *victim, void*, int level, int  )
{
  bool good = ch == NULL ? false : ch->pcdata == NULL ? is_good( ch ) : ch->pcdata->religion == 0 ? is_good( ch ) 
    : is_int_good( religion_table[ch->pcdata->religion].galign );

  if( good )
    damage_holy( victim, ch, spell_damage( SPELL_SLAY, level ), "*The divine fury of the channeled power" );
  else
    damage_unholy( victim, ch, spell_damage( SPELL_SLAY, level ), "*the divine fury of the channeled power" );

  return TRUE;
}


/*
 *   RANGER SPELLS
 */


bool spell_protection_plants( char_data *ch, char_data *victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_PROT_PLANTS, AFF_PROT_PLANTS );

  return TRUE;
}


/*
 *   MAGE SPELLS
 */


bool spell_detect_hidden( char_data *ch, char_data *victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DETECT_HIDDEN, AFF_DETECT_HIDDEN );

  return TRUE;
}


bool spell_displace( char_data *ch, char_data *victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DISPLACE, AFF_DISPLACE );

  return TRUE;
}


bool spell_invisibility( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_INVISIBILITY, AFF_INVISIBLE );

  return TRUE;
}

/*
bool spell_locust_swarm( char_data *ch, char_data*, void*, int level, int )
{
  obj_data *obj;

  if( ch == NULL ) {
    bug( "Locust_Swarm: NULL caster.", 0 );
    return TRUE;
    }

  if( ( obj = find_vnum( *ch->array, 271 ) ) != NULL ) {
    obj->value[0] = UMAX( level, obj->value[0] );
    return TRUE;
    }

  if( ch->in_room->sector_type == SECT_UNDERWATER ) {
    send( ch, "The insects don't seem to be responding.\r\n" );
    fsend_seen( ch, "%s is looking around expectantly and frowning.\r\n", ch );
    return TRUE;
    }

  obj = create( get_obj_index( 271 ) );
  obj->timer = 1+level/2;
  obj->To( ch->array );

  return TRUE;
}
*/

bool spell_poison_cloud( char_data *ch, char_data*, void*, int level, int )
{
  obj_data *obj;
 
  if( null_caster( ch, SPELL_POISON_CLOUD ) )
    return TRUE;

  if( ( obj = find_vnum( *ch->array, 279 ) ) != NULL ) {
    obj->value[0] = max( level, obj->value[0] );
    return TRUE;
  }

  if( terrain_table[ ch->in_room->sector_type ].underwater ) {
    send( ch, "You fail to raise the cloud underwater.\r\n" );
    return TRUE;
  }

  obj = create( get_obj_index( 279 ) );
  obj->timer = 1+level/2;
  obj->To( ch->array );

  return TRUE;
}

/*
bool spell_polymorph( char_data*, char_data*, void*, int, int )
{
  return TRUE;
}
*/

bool spell_mystic_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_MYSTIC_SHIELD, AFF_PROTECT );

  return TRUE;
}


bool spell_infravision( char_data *ch, char_data *victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_INFRAVISION, AFF_INFRARED );

  return TRUE; 
}


bool spell_detect_invisible( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DETECT_INVISIBLE, AFF_SEE_INVIS );

  return TRUE;
}    


bool spell_vitality( char_data *ch, char_data *victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_VITALITY, AFF_VITALITY );

  return TRUE;
}

bool spell_vigor( char_data *ch, char_data *victim, void*, int level, int duration )
{
  if( is_set( ch->affected_by, AFF_VITALITY ) ) {
    send( ch, "You can not be affected by vigor right now.\r\n" );
    return FALSE;
    }

  spell_affect( ch, victim, level, duration, SPELL_VIGOR, AFF_VIGOR );

  return TRUE;
}

bool spell_calm( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_CALM ) ) 
    return TRUE;

  if( victim->position < POS_RESTING ) {
    send( ch, "%s is unconscious and so the spell has no affect.\r\n", victim );
    return TRUE;
  }

  if( ch->fighting == NULL ) {
    send( ch, "You are not fighting anyone." );
    return TRUE;
  }

/*
  if( victim->fighting != ch ) {
    send( ch, "%s isn't fighting you.\r\n", victim );
    return TRUE;
  }
*/
  if( makes_save( victim, ch, RES_MIND, SPELL_CALM, level ) ) {
    send( victim, "You are unaffected by the calm spell.\r\n" );
    send_seen( victim, "%s seems to pause a moment but then continues to fight!\r\n", victim );
    return TRUE;
  }

  stop_fight( victim );
  record_damage( victim, ch, 10 );

  spell_affect( ch, victim, level, duration, SPELL_CALM, AFF_CALM );

  send( ch, "%s stops attacking you.\r\n", victim );
  send( victim, "You don't feel like fighting %s any more.\r\n", ch );
  send_seen( victim, "%s stops attacking %s.\r\n", ch, victim );

  return TRUE;
} 



