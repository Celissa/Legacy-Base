#include "system.h"

bool is_set( char_data*, char* );

/*
 *   LOCAL FUNCTIONS
 */


const char*   get_attack     ( char_data*, obj_data* );

//void    poison            ( char_data* );
bool    absorb_armor      ( char_data*, char_data*, const char*, int&,
                            const char*& );
bool    absorb_bracers    ( char_data*, char_data*, int&, int, const char* );
bool    absorb_shield     ( char_data*, char_data*, const char*, int& ); 
bool    can_defend        ( char_data*, char_data* );
bool    block_shield      ( char_data*, char_data*, const char* );
void    damage_modifiers  ( char_data*, char_data*, obj_data*, int& );
bool    damage_weapon     ( char_data*, obj_data*& );
bool    dodge             ( char_data*, char_data*, int& );
bool    mirror_image      ( char_data*, char_data* );
bool    ward_blow         ( char_data*, char_data*, int& );
bool    side_step         ( char_data*, char_data*, int& );
void    evasion           ( char_data*, char_data*, int& );
bool    tumble            ( char_data*, char_data*, int& );
bool    fire_shield       ( char_data*, char_data* );
bool    physical_shield   ( char_data*, char_data* );
bool    whirling_shield   ( char_data*, char_data* );
bool    thorn_shield      ( char_data*, char_data* );
bool    ion_shield        ( char_data*, char_data* );
bool    hoar_shield       ( char_data*, char_data* );
bool    elemental_aura    ( char_data*, char_data* );
bool    dance_winter      ( char_data*, char_data* );
bool    holy_aura         ( char_data*, char_data* );
bool    unholy_aura       ( char_data*, char_data* );
bool    ghost_shield      ( char_data*, char_data* );
bool    demon_shield      ( char_data*, char_data* );
bool    flaming_weapon    ( char_data*, char_data*, obj_data* );
bool    blighted_weapon   ( char_data*, char_data*, obj_data* );
bool    diseased_weapon   ( char_data*, char_data*, obj_data* ); 
bool    misses_blindly    ( char_data*, char_data*, int&, const char* );
bool    parry             ( char_data*, char_data*, int& );
bool    off_hand_parry    ( char_data*, char_data*, int& );
bool    guard             ( char_data*, char_data*, int& );
bool    shadow_dance      ( char_data*, char_data*, const char* );
bool    switch_victim     ( char_data*, char_data* );
bool    trip              ( char_data*, char_data*, obj_data* );
bool    simulcrum         ( char_data*, char_data* );
bool    physical_entrap   ( char_data*, char_data*, int&, int, const char* );
void    damage_entrap     ( char_data*, int, int, const char* );
bool    firefly           ( char_data*, char_data* );
int     damage_armor      ( char_data*, int );
int     charge_damage     ( char_data* );
void    act_spam_notvict  ( char*, char_data*, obj_data*, char_data* );
bool    confused_char     ( char_data* );
void    critical_hit      ( char_data*, char_data*, obj_data*, int& );
void    deadly_strike     ( char_data*, char_data*, obj_data*, int& );
void    backstab_damage   ( char_data*, char_data*, obj_data*, int& );
void    sneak_attack      ( char_data*, char_data*, obj_data*, int& );
bool    is_blocked        ( char_data*, char_data*, obj_data*, char* );
void    leap_message      ( char_data*, char_data* );
void    modify_roll       ( char_data*, char_data*, obj_data*, int& );
void    power_strike      ( char_data*, char_data*, obj_data*, int& );
void    stun              ( char_data*, char_data*, const char* );
void    trigger_hit       ( char_data*, char_data*, obj_data* );
void    trigger_poison    ( char_data*, char_data*, obj_data* );
void    player_multi      ( char_data*, char_data* );
void    barkskin          ( char_data*, int& );
void    invulnerable      ( char_data*, int& );
void    protgood          ( char_data*, char_data*, int& );
void    wraith_form       ( char_data*, char_data*, int& );
void    protevil          ( char_data*, char_data*, int& );
void    treeform          ( char_data*, char_data*, int& );
void    auto_disarm       ( char_data*, char_data* );
void    disarm            ( char_data*, char_data* );
void    shield_bash       ( char_data*, char_data*, obj_data* );
void    auto_gouge        ( char_data*, char_data*, obj_data* );
void    shin_kick         ( char_data*, char_data*, char );
obj_data* find_ammo       ( char_data*, obj_data* );
void    dance_flavour     ( char_data* );
void    mind_body         ( char_data*, int& );
bool    storm_damage      ( char_data*, char_data*, int&, const char* );
bool    dance_summer      ( char_data*, char_data* );
bool    dance_shards      ( char_data*, char_data* );
void    spring_attack     ( char_data*, char_data*, int& );
void    mod_element_damage( char_data*, int&, int );

/*
 *   LOCAL CONSTANTS
 */


const char* dam_loc_name [] = { "", "head", "neck", "torso", "waist", "arm", "right wrist", "left wrist", "hand", "finger", "finger", "", "leg", "feet", "", "", "body" };
const char* weapon_attack_name [ WEAPON_COUNT ] = { "punch", "pierce", "slash", "bash", "swing", "slash", "pound", "whip", "chop", "arrow", "pierce", "bolt", "stone" };
const char* entrap_word [] = { "ghost chains", "webs", "vines", "snare", "briars", "entangling vines", "prison of bones" };
int entrap [] = { AFF_GHOST_CHAINS, AFF_ENTANGLED, AFF_ENSNARE, AFF_ENSNARE_TRAPPED, AFF_BRIARTANGLE, AFF_WEB_ENTANGLE, AFF_BONE_PRISON };

/*
 *   EXTERNAL SUPPORT ROUTINES
 */


// monks will scream if you call this for crit hit at the moment, but all new critical-based functions (eg sneak attack, etc) should call it
inline bool can_critical( char_data* victim )
{
  if( !victim )
    return false;

  if( victim->shdata->race == RACE_UNDEAD )
    return false;

  if( victim->species ) {
    // checks for mobs
    if( is_set( victim->species->act_flags, ACT_NO_CRIT ) )
      return false;
    if( is_set( victim->species->act_flags, ACT_OOZES ) )
      return false;
    if( is_set( victim->species->act_flags, ACT_GHOST ) )
      return false;
  }

  return true;
}


inline bool can_hamstring( char_data* victim )
{
  if( !victim )
    return false;

  if( is_set( victim->affected_by, AFF_HAMSTRING ) )
    return false;
  
  if( victim->species ) {
    // checks for mobs
    if( is_set( victim->species->act_flags, ACT_SLITHERS ) )
      return false;
  }

  return true;
}


int initiative( char_data* )
{
  return number_range( 5,30 );
}


int response( char_data* )
{
  return number_range( 5,30 );
}


char_data* opponent( char_data* ch )
{
  char_data* rch;

  if( ch->fighting != NULL )
    return ch->fighting;

  if( ch->array != NULL ) {
    for( int i = 0; i < *ch->array; i++ )
      if( ( rch = character( ch->array->list[i] ) ) != NULL && rch->fighting == ch )
        return rch;
  }

  return NULL;
} 


char_data* has_enemy( char_data* ch )
{
  char_data* rch;

  if( ch->fighting != NULL )
    return ch->fighting;

  if( ch->array != NULL ) {
    for( int i = 0; i < *ch->array; i++ ) 
      if( ( rch = character( ch->array->list[i] ) ) != NULL && rch->position > POS_SLEEPING && includes( rch->aggressive, ch ) )
        return rch;
  }

  if( ch->cast != NULL && spell_table[ ch->cast->spell ].type == STYPE_OFFENSIVE )
    return (char_data*) ch->cast->target;

  return NULL;
}
 

/*
 *   TOP LEVEL ROUND HANDLER
 */


void leap_message( char_data* ch, char_data* victim )
{
  char_data* rch;

  if( victim->fighting == ch ) {
    send( ch, "You counterattack %s!!\r\n", victim );

    if( ch->Seen( victim ) )
      send( victim, "%s%s counterattacks you!!%s\r\n",
        bold_v( victim ), ch, normal( victim ) );

    for( int i = 0; i < *victim->array; i++ ) 
      if( ( rch = character( victim->array->list[i] ) ) != NULL 
        && rch != victim && rch != ch && ch->Seen( rch )
        && rch->link != NULL ) 
        send( rch, "%s%s counterattacks %s!!%s\r\n",
          damage_color( rch, ch, victim ), ch, victim, normal( rch ) );
    }
  else {
    send( ch, "You leap to attack %s!!\r\n", victim );
    if( ch->Seen( victim ) )
      send( victim, "%s%s leaps to attack you!!%s\r\n",
        bold_v( victim ), ch, normal( victim ) );

    for( int i = 0; i < *victim->array; i++ ) 
      if( ( rch = character( victim->array->list[i] ) ) != NULL 
        && rch != victim && rch != ch && ch->Seen( rch )
        && rch->link != NULL ) 
        send( rch, "%s%s leaps to attack %s!!%s\r\n",
          damage_color( rch, ch, victim ), ch, victim, normal( rch ) );
  }
}


bool jump_feet( char_data* ch )
{
  if( ch->position != POS_RESTING && ch->position != POS_MEDITATING ) 
    return FALSE;

  send( ch, "You quickly jump to your feet.\r\n" );
  send_seen( ch, "%s jumps to %s feet.\r\n", ch, ch->His_Her( ) );

  ch->position = POS_STANDING;
  ch->pos_obj  = NULL;

  check_wimpy( ch, ch->fighting );
  return TRUE;
} 


/*
 *   PLAYER FIGHT ROUTINES
 */

int player_round( char_data* ch, char_data* victim )
{
  int attack_skills [] = { SKILL_SECOND, SKILL_THIRD, SKILL_FOURTH, SKILL_FIFTH, SKILL_OFFHAND_ATTACK, SKILL_DUAL_WIELD };

  obj_data*           wield  = NULL;
  obj_data*       secondary  = NULL;
  obj_data*          shield  = NULL;
  obj_data*            left;
  obj_data*           right;
//  obj_clss_data*   obj_clss  = NULL;
  int                 skill;
  int                  rate;
  int                weight;
  int                     i;
  int                 range = ATT_BRAWL;

  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) )
    return 25;

  if( ( left = get_shield( ch, WEAR_HELD_L ) ) != NULL )
    shield = left;
  else if( ( left = get_weapon( ch, WEAR_HELD_L ) ) != NULL )
    secondary = left;

  if( ( right = get_shield( ch, WEAR_HELD_R ) ) != NULL )
    shield = right;
  else if( ( right = get_weapon( ch, WEAR_HELD_R ) ) != NULL )
    wield = right;

  if( wield == NULL || ( number_range( 0, 2 ) == 0 && secondary != NULL ) )
    wield = secondary;

  if( wield != NULL ) {
    i = ( shield == NULL && secondary == NULL ? 3 : 2 );
    weight = max( 100, wield->Weight( 1 )-5*i*ch->Strength( ) );
    if( wield->Weight( 1 ) >= 400 ) {
      float new_weight = weight;
      ch->improve_skill( SKILL_QUICK_STRIKE );
      new_weight *= ( 1-ch->get_skill( SKILL_QUICK_STRIKE )*0.03 );
      weight = (int) new_weight;
    }
    skill  = WEAPON_FIRST+wield->value[3];
  } else {
    weight = 100;
    skill  = WEAPON_UNARMED;
  }

  if( range == ATT_BRAWL && wield != NULL )
    if( wield->pIndexData != NULL )
      if( wield->value[3] == WEAPON_BOW-WEAPON_FIRST
        || wield->value[3] == WEAPON_SLING-WEAPON_FIRST
        || wield->value[3] == WEAPON_CROSSBOW-WEAPON_FIRST )
        range = ATT_RANGED;
    
  if( ( i = number_range( -1, ( secondary == NULL ? 3 : 5 ) ) ) == -1 )
    ch->improve_skill( skill );
  else {
    if( attack_skills[i] == SKILL_OFFHAND_ATTACK || attack_skills[i] == SKILL_DUAL_WIELD ) {
      if( secondary != NULL ) {
        if( secondary->value[3] == WEAPON_AXE-WEAPON_FIRST && number_range( 0, 3 ) == 0 ) {
          ch->improve_skill( WEAPON_OHAXE );
        }
        else if( secondary->value[3] == WEAPON_SWORD-WEAPON_FIRST && number_range( 0, 3 ) == 0 ) {
          ch->improve_skill( WEAPON_OHSWORD );
        }
        else if( secondary->value[3] == WEAPON_SPEAR-WEAPON_FIRST && number_range( 0, 3 ) == 0 ) {
          ch->improve_skill( WEAPON_OHSPEAR );
        }
        else if( secondary->value[3] == WEAPON_DAGGER-WEAPON_FIRST && number_range( 0, 3 ) == 0 ) {
          ch->improve_skill( attack_skills[i] );
        }
      }
    }
    else
      ch->improve_skill( attack_skills[i] );
  }

  attack( ch, victim, get_attack( ch, wield ), wield, -1, ATT_PHYSICAL, ATT_PHYSICAL, range );
  skill = ch->get_skill( skill );
  rate  = 0;

  // offhand = 4, dual-wield = 5... theoretically should make no diff unless you have BOTH skills in which case you'll get 2x bonus hits
  // heh, bug since GC's time "i < ( sec ..." meant fifth never did anything and neither did offhand
  for( i = 0; i <= ( secondary == NULL ? 3 : 5 ); i++ )
    rate += max( 0, ch->get_skill( attack_skills[i] ) - ( i >= 4 ? 2 : i * 2 ) );

  rate = 10+rate/2;
  rate = (320+weight/5-4*skill-2*ch->Dexterity( )-2*ch->Strength( ))/rate;
  rate = number_range( rate/2, 3*rate/2 );

  return rate;
}


/*
 *   SPECIES FIGHT ROUTINES
 */


int mob_round( char_data* ch, char_data* victim )
{
  ch->shown     = 1;
  victim->shown = 1;

  if( ch->shifted != NULL && ch->pcdata != NULL && is_set( ch->pcdata->pfile->flags, PLR_PARRY ) )
    return 32;

  if( switch_victim( ch, victim ) )
    return 16;

  var_mob    = ch;
  var_room   = ch->in_room;
  var_ch     = victim;
  var_victim = victim;

  if( ch->shifted == NULL )
    execute( ch->species->attack );
  else
    execute( ch->shifted->attack );
  
  // speed increase to offset damage penalty
  // a pet that is 100% leveled will have speed doubled (this is max)
  if( is_pet( ch ) && ch->Is_Valid( ) && ch->hit > 0) {
    return int(32/(1+(float(ch->Get_Integer( "/Squeed/Code/Levels" ))/ch->shdata->level)));
  }
  return 32;
}


bool switch_victim( char_data* ch, char_data* victim )
{
  char_data* rch;

  if( victim->pcdata == NULL || number_range( 0, 4 ) != 0  ||
    !is_set( victim->pcdata->pfile->flags, PLR_PARRY )  )
    return FALSE;

  for( int i = 0; ; i++ ) { 
    if( i >= *ch->array )
      return FALSE;
    if( ( rch = character( ch->array->list[i] ) ) != NULL 
      && rch != victim && rch->fighting == ch ) 
      break;
    }

  ch->fighting = rch;

  fsend( victim, "%s stops attacking you and leaps to attack %s.", ch, rch );
  fsend( rch, "%s stops attacking %s and leaps to attack you.", ch, victim );
  fsend_seen( ch, "%s stops attacking %s and leaps to attack %s.", ch, victim, rch );

  return TRUE;
}


void add_round( char_data* ch, int delay )
{
  if( number_range( 0,32 ) > delay )
    return;

  ch->rounds++;

  if( ch->species->exp > 1e8 || ch->species->rounds++ > 1e8 || ch->species->damage > 1e8 ||
    ch->species->damage_taken > 1e8 ) {
    ch->species->rounds          /= 2;
    ch->species->damage          /= 2;
    ch->species->damage_taken    /= 2;
    ch->species->shdata->deaths  /= 2;
    ch->species->shdata->kills   /= 2;
    ch->species->exp             /= 2;
    ch->species->special         /= 2;
  }
}


/*
 *   DAMAGE AND ATTACK NAME ROUTINES
 */

const char* attack_word( obj_data* obj )
{
  int weapon = obj == NULL ? 0 : obj->value[3];

  if( weapon == 0 && MAX_ENTRY_UNARMED_WORDS > 0 )
    return unarmed_attack_table[ number_range( 0, MAX_ENTRY_UNARMED_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_DAGGER-WEAPON_FIRST && MAX_ENTRY_DAGGER_WORDS > 0 )
    return dagger_attack_table[ number_range( 0, MAX_ENTRY_DAGGER_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_SWORD-WEAPON_FIRST && MAX_ENTRY_SWORD_WORDS > 0 )
    return sword_attack_table[ number_range( 0, MAX_ENTRY_SWORD_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_CLUB-WEAPON_FIRST && MAX_ENTRY_CLUB_WORDS > 0 )
    return club_attack_table[ number_range( 0, MAX_ENTRY_CLUB_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_STAFF-WEAPON_FIRST && MAX_ENTRY_STAFF_WORDS > 0 )
    return staff_attack_table[ number_range( 0, MAX_ENTRY_STAFF_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_POLEARM-WEAPON_FIRST && MAX_ENTRY_POLEARM_WORDS > 0 )
    return polearm_attack_table[ number_range( 0, MAX_ENTRY_POLEARM_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_MACE-WEAPON_FIRST && MAX_ENTRY_MACE_WORDS > 0 )
    return mace_attack_table[ number_range( 0, MAX_ENTRY_MACE_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_WHIP-WEAPON_FIRST && MAX_ENTRY_WHIP_WORDS > 0 )
    return whip_attack_table[ number_range( 0, MAX_ENTRY_WHIP_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_AXE-WEAPON_FIRST && MAX_ENTRY_AXE_WORDS > 0 )
    return axe_attack_table[ number_range( 0, MAX_ENTRY_AXE_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_BOW-WEAPON_FIRST && MAX_ENTRY_BOW_WORDS > 0 )
    return bow_attack_table[ number_range( 0, MAX_ENTRY_BOW_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_SPEAR-WEAPON_FIRST && MAX_ENTRY_SPEAR_WORDS > 0 )
    return spear_attack_table[ number_range( 0, MAX_ENTRY_SPEAR_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_CROSSBOW-WEAPON_FIRST && MAX_ENTRY_XBOW_WORDS > 0 )
    return xbow_attack_table[ number_range( 0, MAX_ENTRY_XBOW_WORDS-1 ) ].noun;
  else if( weapon == WEAPON_SLING-WEAPON_FIRST && MAX_ENTRY_SLING_WORDS > 0 )
    return sling_attack_table[ number_range( 0, MAX_ENTRY_SLING_WORDS-1 ) ].noun;

  return weapon_attack_name[weapon];
}

const char* get_attack( char_data* ch, obj_data* wield )
{
  int skill [] = { SKILL_DEMON_SLASH, SKILL_DRAGON_STRIKE, SKILL_PAIN_STRIKE,
    SKILL_EAGLE_CLAW, SKILL_BEAR_GRASP, SKILL_LOCUST_KICK, SKILL_TIGER_PAW };

  int  weapon;
  int       i;

  weapon = ( wield == NULL ? 0 : wield->value[3] );

  if( ch->species != NULL || weapon != 0 ) 
    return attack_word( wield );

  if( ch->mount == NULL && ch->get_skill( SKILL_TIGER_PAW ) != UNLEARNT ) 
    for( i = 0; i < 7; i++ ) {  
      if( number_range( 0, 7*MAX_SKILL_LEVEL-9*i ) < ch->get_skill( skill[i] ) ) {
        ch->improve_skill( skill[i] );
        return skill_table[ skill[i] ].name;
      }
    }

  if( wield == NULL && ch->shdata->race == RACE_LIZARD && 
    ch->Wearing( WEAR_HANDS ) == NULL )
    return "claw";

  return attack_word( wield );
}


int get_damage( char_data* ch, obj_data* wield, char letter )
{
  int damage;

  if( wield != NULL ) {
    damage  = roll_dice( wield->value[1], wield->value[2] );
  } else {
    damage = roll_dice( 1, letter == 'c' ? 6 : 4 );
  }

  if( ch->species == NULL && ( wield == NULL || wield->value[3] == 0 ) ) {
    damage += ch->get_skill( SKILL_PUNCH)/3;
    switch( letter ) {
    case 't' : 
      damage = roll_dice( 2, 6 )+ch->get_skill( SKILL_TIGER_PAW )/3;
      break;
    case 'l' : 
      damage = roll_dice( 2, 8  )+ch->get_skill( SKILL_LOCUST_KICK )/3;
      break;
    case 'b' : 
      damage = roll_dice( 2, 10 )+ch->get_skill( SKILL_BEAR_GRASP )/2;
      break;
    case 'e' : 
      damage = roll_dice( 2, 14 )+ch->get_skill( SKILL_EAGLE_CLAW )/2;
      break;
    case 'd' : 
      damage = roll_dice( 2, 16 )+ch->get_skill( SKILL_DRAGON_STRIKE );
      break;
    case '*' : 
      damage = roll_dice( 2, 18 )+ch->get_skill( SKILL_DEMON_SLASH );
      break;
    case 'n' : 
      damage = roll_dice( 2, 15 )+ch->get_skill( SKILL_PAIN_STRIKE )/3;
      affect_data affect;
      affect.type = AFF_PAIN;
      affect.duration = 3*ch->get_skill( SKILL_PAIN_STRIKE );
      affect.leech = NULL;
      affect.level = ch->get_skill( SKILL_PAIN_STRIKE );
      add_affect( ch->fighting, &affect );
      break;
    }   
  }

  return damage;
}


void damage_modifiers( char_data* ch, char_data* victim, obj_data* wield, int& damage )
{
  int mlevel, level;
  float mod = 0.0;
  
  damage += (int)ch->Damroll( wield );
  
  if( ch->move > 0 && is_set( &ch->status, STAT_BERSERK ) ) {
    ch->move--;
    damage += 2+ch->get_skill( SKILL_BERSERK )/3;
    if( ch->get_skill( SKILL_FRENZY ) > 0 ) {
      ch->improve_skill( SKILL_FRENZY );
      damage += 4+ch->get_skill( SKILL_FRENZY );
    }
  }

  if( victim->fighting == NULL && ch->Get_Integer( LEAP_ATTACK ) == 0 )
    spring_attack( ch, victim, damage );

  if( ch->move > 0 && is_set( &ch->dance, DANCE_BATTLEFIEND ) ) {
    if( number_range( 1, 20 ) == 3 ) {

      char to_char [ TWO_LINES ] = "@mYou unleash the fiend of battle.@n\r\n";
      char to_room [ TWO_LINES ];
      char tmp     [ TWO_LINES ];
      char_data* rch;

      convert_to_ansi( ch, to_char, tmp, sizeof( tmp ) );
      send( ch, tmp );
      if( ch->array != NULL ) {
        for( int i = 0; i < ch->array->size; i++ ) {
          rch = character( ch->array->list[i] );
          if( rch != NULL && rch != ch && ch->Seen( rch ) ) {
            sprintf( to_room, "@m%s unleashes the fiend of battle!@n\r\n", ch->Seen_Name( rch ) );
            convert_to_ansi( rch, to_room, tmp, sizeof( tmp ) );
            send( rch, tmp );
          }
        }
      }
    }
    ch->move--;
    damage += ch->get_skill( SKILL_BATTLEFIEND )*damage/50;
    ch->improve_skill( SKILL_BATTLEFIEND );
  }

  if( ch->move > 0 && is_set( &ch->dance, DANCE_SUMMER ) )
    damage += ch->get_skill( SKILL_SUMMER )/3;

  if( ch->move > 0 && is_set( &ch->dance, DANCE_WINTER ) )
    damage += ch->get_skill( SKILL_WINTER )/3;

  // bonus damage for pets:
  // assume 3 attacks per round
  // assume 75% average damage of max allowed by standards
  // 80% of damage due to being a pet
  // 40% of damage ( another 40% will be in speed )
  // Notice damage advancement is flattened out from standards to prevent sudden leaps.
  if( is_pet( ch ) ) {
     mlevel = ch->Get_Integer( "/Squeed/Code/Levels" );
     level = ch->shdata->level;
     int i = 0;
     
     for( int x = 80; x >= 0 && mlevel > 0; x -=40) {
       if( ( i = level-x) < 1 )
         continue;
       mod += float(min( i, mlevel )*(4+x/40)*2)/25;
       mlevel -= i;
     }
     
     mod = 2*(float(mlevel*(2*level-mlevel+160))/40)/25;
     damage += int(mod);
     //bug( "damage plus %d \r\n", int(mod) );     
  }

  //bug( "damage from %s = %d \r\n", ch, damage );  
  if( victim->position == POS_RESTING )
    damage = 3*damage/2;
  else if( victim->position <= POS_SLEEPING )
    damage *= 4;

  return;
}


float char_data :: Hitroll( obj_data* obj )
{
  float x;

  x = hitroll+float( Dexterity( ) )/3-3; 

  if( species == NULL && get_skill( SKILL_BLIND_FIGHTING ) > 0 ) {
    x = x + 1+ float( 10*get_skill( SKILL_BLIND_FIGHTING )/(3*MAX_SKILL_LEVEL ) ) ;
    improve_skill( SKILL_BLIND_FIGHTING );
  }

  if( species == NULL && is_set( &dance, DANCE_DRAGONFLY ) ) {
    x += float( 10*get_skill( SKILL_DRAGONFLY )/(3*MAX_SKILL_LEVEL) );
    improve_skill( SKILL_DRAGONFLY );
  }

  if( leader != NULL && in_room == leader->in_room ) {
    x+= 5*leader->get_skill( SKILL_LEADERSHIP )/MAX_SKILL_LEVEL;
    leader->improve_skill( SKILL_LEADERSHIP );
  }

  if( obj == NULL || obj->pIndexData->item_type != ITEM_WEAPON )
    return x;

  if( obj->value[3] == WEAPON_AXE-WEAPON_FIRST ) {
    if( get_skill( SKILL_AXE_MASTERY ) != UNLEARNT ) {
      x += 1+get_skill( SKILL_AXE_MASTERY )/3;
      improve_skill( SKILL_AXE_MASTERY );
    }
    if( is_set( affected_by, AFF_AXE_PROF ) )
      x++;
  }

  if( obj->value[3] == WEAPON_SWORD-WEAPON_FIRST ) {
    if( get_skill( SKILL_BLADE_MASTERY ) != UNLEARNT ) {
      x += 1+get_skill( SKILL_BLADE_MASTERY )/3;
      improve_skill( SKILL_BLADE_MASTERY );
    }
    if( is_set( affected_by, AFF_SWORD_PROF ) )
      x++;
  }

  if( obj->value[3] == WEAPON_DAGGER-WEAPON_FIRST ) {
    if( get_skill( SKILL_DAGGER_MASTERY ) != UNLEARNT ) {
      x += 1+get_skill( SKILL_DAGGER_MASTERY )/3;
      improve_skill( SKILL_DAGGER_MASTERY );
    }
  }

  if( obj->value[3] == WEAPON_SPEAR-WEAPON_FIRST ) {
    if( get_skill( SKILL_SPEAR_MASTERY ) != UNLEARNT ) {
      x += 1+get_skill( SKILL_SPEAR_MASTERY )/3;
      improve_skill( SKILL_SPEAR_MASTERY );
    }
  }

  x += obj->value[0];

  return x;
}


float char_data :: Hitroll_No_Improves( obj_data* obj )
{
  float x;

  x = hitroll+float( Dexterity( ) )/3-3; 

/* Adding in bits to show blind fighting, and special item affects
 * to score
 */

  if( species == NULL && get_skill( SKILL_BLIND_FIGHTING ) >0 ) 
    x = x+1+float( 10*get_skill( SKILL_BLIND_FIGHTING )/( 3*MAX_SKILL_LEVEL ) );

  if( species == NULL && is_set( &dance, DANCE_DRAGONFLY ) )
    x += float( 10*get_skill( SKILL_DRAGONFLY )/( 3*MAX_SKILL_LEVEL ) );

  if( leader != NULL && in_room == leader->in_room ) 
    x+= 5*leader->get_skill( SKILL_LEADERSHIP )/MAX_SKILL_LEVEL;

  if( obj == NULL || obj->pIndexData->item_type != ITEM_WEAPON )
    return x;

  if( obj->value[3] == WEAPON_AXE-WEAPON_FIRST ) {
    if( get_skill( SKILL_AXE_MASTERY ) != UNLEARNT )
      x += 1+get_skill( SKILL_AXE_MASTERY )/3;
    if( is_set( affected_by, AFF_AXE_PROF ) )
      x++;
  }

  if( obj->value[3] == WEAPON_SWORD-WEAPON_FIRST ) {
    if( get_skill( SKILL_BLADE_MASTERY ) != UNLEARNT )
      x += 1+get_skill( SKILL_BLADE_MASTERY )/3;
    if( is_set( affected_by, AFF_SWORD_PROF ) )
      x++;
  }

  if( obj->value[3] == WEAPON_DAGGER-WEAPON_FIRST 
    && get_skill( SKILL_DAGGER_MASTERY ) != UNLEARNT )
    x += 1+get_skill( SKILL_DAGGER_MASTERY )/3;

  if( obj->value[3] == WEAPON_SPEAR-WEAPON_FIRST 
    && get_skill( SKILL_SPEAR_MASTERY ) != UNLEARNT )
    x += 1+get_skill( SKILL_SPEAR_MASTERY )/3;

  x += obj->value[0];

  return x;
}


float char_data :: Damroll( obj_data* obj )
{
  float x;

  x = damroll+float( Strength( ) )/3-4;

  if( mount != NULL ) {
    improve_skill( SKILL_MOUNTED_FIGHTING );
    x += 5*get_skill( SKILL_MOUNTED_FIGHTING )/MAX_SKILL_LEVEL;
  }

  if( is_set( &dance, DANCE_FURY ) ) {
    x += float( 10*get_skill( SKILL_FURY )/( 3*MAX_SKILL_LEVEL ) );
    improve_skill( SKILL_FURY );
  }

  if( leader != NULL && leader->in_room == in_room ) 
    x += 5*leader->get_skill( SKILL_LEADERSHIP )/MAX_SKILL_LEVEL;

  if( obj == NULL || obj->pIndexData->item_type != ITEM_WEAPON )
    return x;

  if( obj->value[3] == WEAPON_AXE-WEAPON_FIRST ) {
    if( get_skill( SKILL_AXE_MASTERY ) != UNLEARNT )
      x += 1+get_skill( SKILL_AXE_MASTERY )/3;
    if( is_set( affected_by, AFF_AXE_PROF ) )
      x++;
  }

  if( obj->value[3] == WEAPON_SWORD-WEAPON_FIRST ) {
    if( get_skill( SKILL_BLADE_MASTERY ) != UNLEARNT )
      x += 1+get_skill( SKILL_BLADE_MASTERY )/3;
    if( is_set( affected_by, AFF_SWORD_PROF ) )
      x++;
  }

  if( obj->value[3] == WEAPON_DAGGER-WEAPON_FIRST ) {
    if( get_skill( SKILL_DAGGER_MASTERY ) != UNLEARNT )
      x += 1+get_skill( SKILL_DAGGER_MASTERY )/3;
  }

  if( obj->value[3] == WEAPON_SPEAR-WEAPON_FIRST ) {
    if( get_skill( SKILL_SPEAR_MASTERY ) != UNLEARNT )
      x += 1+get_skill( SKILL_SPEAR_MASTERY )/3;
  }

  x += obj->value[0];

  return x;
}


float char_data :: Damroll_No_Improves( obj_data* obj )
{
  float x;

  x = damroll+float( Strength( ) )/3-4;

  if( mount != NULL )
    x += 5*get_skill( SKILL_MOUNTED_FIGHTING )/MAX_SKILL_LEVEL;

  if( is_set( &dance, DANCE_FURY ) )
    x += float( 10*get_skill( SKILL_FURY )/( MAX_SKILL_LEVEL*3 ) );
  
  if( leader != NULL && in_room == leader->in_room ) 
    x+= 5*leader->get_skill( SKILL_LEADERSHIP )/MAX_SKILL_LEVEL;

  if( obj == NULL || obj->pIndexData->item_type != ITEM_WEAPON )
    return x;

  if( obj->value[3] == WEAPON_AXE-WEAPON_FIRST ) {
    if( get_skill( SKILL_AXE_MASTERY ) != UNLEARNT )
      x += 1+get_skill( SKILL_AXE_MASTERY )/3;
    if( is_set( affected_by, AFF_AXE_PROF ) )
      x++;
  }

  if( obj->value[3] == WEAPON_SWORD-WEAPON_FIRST ) {
    if( get_skill( SKILL_BLADE_MASTERY ) != UNLEARNT )
      x += 1+get_skill( SKILL_BLADE_MASTERY )/3;
    if( is_set( affected_by, AFF_SWORD_PROF ) )
      x++;
  }

  if( obj->value[3] == WEAPON_DAGGER-WEAPON_FIRST ) {
    if( get_skill( SKILL_DAGGER_MASTERY ) != UNLEARNT )
      x += 1+get_skill( SKILL_DAGGER_MASTERY )/3;
  }

  if( obj->value[3] == WEAPON_SPEAR-WEAPON_FIRST ) {
    if( get_skill( SKILL_SPEAR_MASTERY ) != UNLEARNT )
    x += 1+get_skill( SKILL_SPEAR_MASTERY )/3;
  }

  x += obj->value[0];

  return x;
}


/*
 *   MAIN BATTLE ROUTINES
 */


bool attack( char_data* ch, char_data* victim, const char* dt, obj_data* wield, int damage, int modifier, int type, int range, bool plural )
{
  int                 roll;
  bool              active;
  bool            backstab;
  bool              charge;
  const char*     loc_name;
  obj_data*           ammo = NULL;
  player_data*          pc;
  char_data*           pet;

  pc = player( victim );

  if( !ch || !ch->Is_Valid() || !victim || !victim->Is_Valid() )
    return FALSE;

  if( victim->in_room != ch->in_room ) {
    bug( "Attack: char (%s) in different room to victim (%s)", ch->Name( ), victim->Name( ) );
    return FALSE;
  }

  if( ch->position < POS_FIGHTING )
    return FALSE;

  if( victim->position == POS_DEAD ) {
    bug( "Attack: char (%s) hitting already dead victim (%s)", ch->Name( ), victim->Name( ) );
    return FALSE;
  }

  if( is_set( ch->affected_by, AFF_CONFUSED ) )
    if( confused_char( ch ) )
      return false;

  if( is_set( victim->affected_by, AFF_BANDAGE ) )
    strip_affect( victim, AFF_BANDAGE );

  if( is_set( ch->affected_by, AFF_BANDAGE ) )
    strip_affect( ch, AFF_BANDAGE );

  if( is_set( ch->affected_by, AFF_DEATH_MASK ) &&
    victim->shdata->race == RACE_UNDEAD ) {
    strip_affect( ch, AFF_DEATH_MASK );
   // What do room denizens do?
    if( ch->array != 0 ) {
      for( int i = ch->array->size - 1; i >= 0; i-- ) {
        char_data *rch = character( ch->array->list[i] );
        if( rch != NULL && ch != rch && ch->Seen( rch ) ) {

          // They notice that ch is there.
          if( !includes( ch->seen_by, rch ) ) {
            send( rch, "++ You notice %s hiding in the shadows! ++\r\n", ch );
            ch->seen_by += rch;
          }
        
          // If aggressive, they leap to attack.
          if( is_aggressive( rch, ch ) ) {
            init_attack( rch, ch );
          }
        }
      }
    }
  }

    
  roll  = number_range( -250, 250 );
  roll += modifier;

  modify_roll( ch, victim, wield, roll );

  active   = can_defend( victim, ch );
  backstab = !strcmp( dt, "+backstab" );
  charge = !strcmp( dt, "+charge" );

  if( backstab ) {
    obj_data* left_wield = get_weapon( ch, WEAR_HELD_L );
    obj_data* right_wield = get_weapon( ch, WEAR_HELD_R );

    if( ch->get_skill( SKILL_DOUBLE_STRIKE ) != UNLEARNT && left_wield != NULL && is_set( left_wield->pIndexData->extra_flags, OFLAG_BACKSTAB ) &&
      right_wield != NULL && is_set( right_wield->pIndexData->extra_flags, OFLAG_BACKSTAB ) )
      dt = "double strike backstab";
    else if( ch->get_skill( SKILL_ASSASSINATE ) != UNLEARNT )
      dt = "assassinate";
  }

  if( *dt != '\0' ) {
    if( *dt == '+' )
      dt++;
  }

//  if ( backstab && ch->Seen(victim) ) // must be unseen to backstab
//     return FALSE;
/*  moving this to after the real attack-don't reveal on a missed attack now :p
  leave_camouflage(ch);
  leave_shadows(ch);
  strip_affect( ch, AFF_INVISIBLE );
  remove_bit( ch->affected_by, AFF_INVISIBLE );
*/
  if( ( !backstab && !charge ) && type == ATT_PHYSICAL && misses_blindly( ch, victim, roll, dt ) ) {
    reveal( ch );
    return FALSE;
  }

  if( active && mirror_image( ch, victim ) ) {
    reveal( ch );
    return FALSE;
  }

  if( active && dodge( ch, victim, roll ) ) {
    reveal( ch );
    return FALSE;
  }

  if( active && tumble( ch, victim, roll ) ) {
    reveal( ch );
    return FALSE;
  }

  if( active && firefly( ch, victim ) ) {
    reveal( ch );
    return FALSE;
  }

  if( active && simulcrum( ch, victim ) ) {
    pet = pc->familiar;
    victim = pet;
  }

  if( active && side_step( ch, victim, roll ) ) {
    reveal( ch );
    return FALSE;
  }

  if( active && ward_blow( ch, victim, roll ) ) {
    reveal( ch );
    return FALSE;
  }

  if( damage_weapon( ch, wield ) && range == ATT_RANGED ) {
    reveal( ch );
    return FALSE;
  }

  if( range == ATT_BRAWL ) {
    if( fire_shield( ch, victim ) || ion_shield( ch, victim ) ||
      thorn_shield( ch, victim ) || hoar_shield( ch, victim ) ||
      holy_aura( ch, victim ) || unholy_aura( ch, victim ) ||
      ghost_shield( ch, victim ) || demon_shield( ch, victim ) || 
      dance_winter( ch, victim ) || dance_summer( ch, victim ) ||
      dance_shards( ch, victim ) || elemental_aura( ch, victim ) ||
      whirling_shield( ch, victim ) || physical_shield( ch, victim ) ) {
      reveal( ch );
      return FALSE;
    }

    if( damage_weapon( ch, wield ) || ( active && !backstab && !charge && ( parry( ch, victim, roll ) 
      || ( active && !backstab && !charge && ( guard( ch, victim, roll )
      || block_shield( ch, victim, dt ) || off_hand_parry( ch, victim, roll )
      || trip( ch, victim, wield ) ) ) ) ) ) {
      reveal( ch );
      return FALSE;
    }
  }
  
  if( !ch || !ch->Is_Valid( ) || !victim || !victim->Is_Valid( ) )
    return FALSE;

  if( active && ( !backstab && !charge ) && shadow_dance( ch, victim, dt ) ) {
    reveal( ch );
    return FALSE;
  }

  if( damage == -1 )
    damage = get_damage( ch, wield, dt[0] );

  // all critical multipliers should be inserted here, before the +damroll

  // elemental damage modifiers
  mod_element_damage( ch, damage, type );

  if( wield != NULL && ( wield->pIndexData->value[3] == WEAPON_BOW-WEAPON_FIRST
    || wield->pIndexData->value[3] == WEAPON_SLING-WEAPON_FIRST
    || wield->pIndexData->value[3] == WEAPON_CROSSBOW-WEAPON_FIRST ) ) {
    ammo = find_ammo( ch, wield );
    if( ammo != NULL )
      damage += roll_dice( ammo->value[1], ammo->value[2] );
  }

  damage_modifiers( ch, victim, wield, damage );
  damage = max( 1, damage );

  /*
   *      'Web' Spell Routines for damage absorption
   */

  for( int i = 0; i < 7; i++ ) {
    if( physical_entrap( ch, victim, damage, entrap[i], entrap_word[i] ) )
      return FALSE;
  }

  /* 
   *    being victim of an attack damages 'Web'
   */

  for( int i = 0; i < 6; i++ )
    damage_entrap( victim, damage, entrap[i], entrap_word[i] );

  /*
   *    End of the 'Web' Stuff
   */

  bool mod = false;

  if( victim->species != NULL && victim->hit > 0 ) {
    if( !is_set( &victim->status, STAT_PET ) ) {
      victim->damage_taken += min( victim->hit, damage );
      victim->species->damage_taken += min( victim->hit, damage );
    }
  } else if( ch->species != NULL ) {
    ch->damage          += damage/2;
    ch->species->damage += damage/2;
    if( victim->pcdata != NULL && is_set( victim->pcdata->pfile->flags, PLR_PARRY ) )
      ch->damage        -= damage/2;
//    mod = true;
  }

  switch( type ) {
  case ATT_PHYSICAL:
    if( storm_damage( ch, victim, damage, dt ) )
      return FALSE;

    auto_disarm( ch, victim );
    auto_gouge( ch, victim, wield );
    shin_kick( ch, victim, dt[0] );
    if( !victim || !victim->Is_Valid( ) )
      return FALSE;
    
    if( backstab ) {
      backstab_damage( ch, victim, wield, damage );
    } else {
      deadly_strike( ch, victim, wield, damage );
      critical_hit( ch, victim, wield, damage );
      sneak_attack( ch, victim, wield, damage );
      power_strike( ch, victim, wield, damage );
    }

    if( charge )
      damage *= charge_damage( ch );

    barkskin( victim, damage );
    invulnerable( victim, damage );
    protgood( victim, ch, damage );
    protevil( victim, ch, damage );
    treeform( victim, ch, damage );
    evasion( ch, victim, damage );
    wraith_form( ch, victim, damage );
    mind_body( victim, damage );
    if( absorb_shield( victim, ch, dt, damage ) || 
      absorb_armor( victim, ch, dt, damage, loc_name ) ) {
      if( ( victim->position == POS_SLEEPING || victim->position == POS_MEDITATING ) && !is_set( ch->affected_by, AFF_SLEEP ) ) {
        send( victim, "You are suddenly %s by a soft touch.\r\n",
        victim->position == POS_MEDITATING ? "disturbed" : "awakened" );
        send_seen( victim, "%s %s.\r\n", victim, 
        victim->position == POS_MEDITATING ? "meditation is disturbed" : "wakes up" );
        victim->position = POS_RESTING;
        renter_combat( victim );
      }
      if( mod ) {
        ch->damage          -= damage/2;
        ch->species->damage -= damage/2;
      }
      return FALSE;
    }
    
    if( damage > 1000 ) {
      if( ch != NULL )
        bug( "Attack: %d points of damage! (%s)", damage, ch->Name( )  );
      else
        bug( "Attack: %d points of damage! (acode)", damage );
      damage = 1000;
    }
    
    dam_local( victim, ch, damage, dt, loc_name, plural );
    
    if( !inflict( victim, ch, damage, "") && !flaming_weapon( ch, victim, wield ) &&
     !blighted_weapon( ch, victim, wield ) && !diseased_weapon( ch, victim, wield ) ) { 
      if( range != ATT_RANGED )
        stun( ch, victim, dt ); 
      if( ammo != NULL )
        trigger_hit( ch, victim, ammo );
      trigger_hit( ch, victim, wield );
      trigger_poison( ch, victim, wield );
    }
    break;

  default:
    damage_element( victim, ch, damage, dt, type, NULL, true, true, plural );
    break;
  }

  dance_flavour( ch );

  if( mod && ch->species ) {
    ch->damage          -= damage/2;
    ch->species->damage -= damage/2;
  }
 
  if( ammo != NULL ) {
    ammo->value[3]--;
      if( ammo->value[3] <= 0 )
        ammo->Extract( 1 );
  }

  leave_camouflage(ch);
  leave_shadows(ch);
  strip_affect( ch, AFF_INVISIBLE );
  remove_bit( ch->affected_by, AFF_INVISIBLE );

  return( damage > 0 );
}


void modify_roll( char_data* ch, char_data* victim, obj_data* wield, int& roll )
{
  int weapon = ( wield == NULL ?  0 : wield->value[3] );

//  roll -= victim->mod_armor;
  roll += ch->shdata->level + (5 * (int)ch->Hitroll(wield));
  roll += ( ch->species == NULL ? 8*ch->get_skill( WEAPON_FIRST+weapon ) : 130 );

  if( is_set( &ch->dance, DANCE_DRAGONFLY ) )
    roll += 5*ch->get_skill( SKILL_DRAGONFLY )/3;

  if( is_set( &ch->status, STAT_STUDYING ) )
    roll += 8*ch->get_skill( SKILL_STUDY_OPPONENT );

  if( victim->fighting == NULL ) 
    roll += 50;
 
  if( victim->position <= POS_SLEEPING )
    roll += 100;
 
  if( victim->position > POS_SLEEPING ) {
    roll -= victim->get_skill( SKILL_ACROBATICS );
    victim->improve_skill( SKILL_ACROBATICS );
  }

  return;
}


bool can_defend( char_data* victim, char_data* ch )
{
  if(  victim->position < POS_FIGHTING || !ch->Seen( victim ) )
    return FALSE;

  return TRUE;
}


/*
 *   WEAPON ROUTINES
 */


bool damage_weapon( char_data* ch, obj_data*& wield )
{
  if( ch->get_skill( SKILL_PRESERVE_EQUIPMENT ) > number_range( 0, 20 ) ) {
    ch->improve_skill( SKILL_PRESERVE_EQUIPMENT );
    return FALSE;
  }

  if( wield == NULL || number_range(0, 15 - 4 * wield->rust) > (int) !is_set(wield->extra_flags, OFLAG_SANCT)
    || number_range(1, 4 + wield->value[0]) > 4 || --wield->condition > 0)
    return FALSE;

  send( ch, "%s you are wielding shatters into pieces.\r\n", wield );
  send_seen( ch, "%s which %s is wielding shatters into pieces.\r\n", wield, ch );

  wield->Extract( );
  wield = NULL;

  return TRUE;
}

void spring_attack( char_data* ch, char_data* victim, int& damage )
{ 
  float damage_mod = 0;
  int   skill      = ch->get_skill( SKILL_LEAP_ATTACK )+ch->get_skill( SKILL_SPRING_ATTACK );

  if( ch->species != NULL || ( ch->get_skill( SKILL_LEAP_ATTACK ) == UNLEARNT
    && ch->get_skill( SKILL_SPRING_ATTACK ) == UNLEARNT ) )
    return;
  
  if( !can_critical( victim ) )
    return;

  ch->Set_Integer( LEAP_ATTACK, 1 );

  if( number_range( 0, 1 ) == 0 )
    ch->improve_skill( SKILL_LEAP_ATTACK );
  else
    ch->improve_skill( SKILL_SPRING_ATTACK );

  send( ch, "Your attack penetrates deeply!\r\n" );
  send_seen( ch, "%s's attack penetrates deeply!\r\n", ch );

  damage_mod = damage*skill/20;
  
  damage += (int) damage_mod;
}
    
void trigger_hit( char_data* ch, char_data* victim, obj_data* wield )
{
  oprog_data* oprog;

  if( !ch || !ch->Is_Valid() || !victim || !victim->Is_Valid() || !wield || 
    ch->fighting != victim )
    return;

  for( oprog = wield->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) {
    if( oprog->trigger == OPROG_TRIGGER_HIT ) { 
      var_ch     = ch;
      var_victim = victim;
      var_obj    = wield;    
      var_room   = ch->in_room;
      execute( oprog );
    }
  }

  return;
}

void trigger_poison( char_data* ch, char_data* victim, obj_data* wield )
{
  thing_data*  weapon;
  affect_data      af;
  int           level;
  oprog_data*   oprog;
  obj_clss_data* obj_clss;

  if( !ch || !ch->Is_Valid() || !victim || !victim->Is_Valid() || !wield 
    || ch->fighting != victim || 
    !is_set( wield->extra_flags, OFLAG_POISON_COATED ) )
    return;

  weapon = (thing_data*) wield;
  
  if( is_set( victim->affected_by, AFF_POISON ) )
    return;
  
  if( weapon->Get_Integer( VAR_POISONED ) == 0 || 
    weapon->Get_Integer( POISON_HITS ) == 0 
    || weapon->Get_Integer( VAR_POISONED ) > MAX_ENTRY_POISON ) {
    remove_bit( wield->extra_flags, OFLAG_POISON_COATED );
    weapon->Set_Integer( VAR_POISONED, 0 );
    weapon->Set_Integer( POISON_HITS, 0 );
  }
  
  if( !is_set( wield->extra_flags, OFLAG_POISON_COATED ) )
    return;
  
  level = weapon->Get_Integer( POISON_HITS );

  if( number_range( 0, 99 ) < victim->Save_Poison( ) ) {
    weapon->Set_Integer( POISON_HITS, weapon->Get_Integer( POISON_HITS )-1 );
    return;
  }
  
  if( (obj_clss = get_obj_index( list_value[ LIST_POISON ][ weapon->Get_Integer(
          VAR_POISONED )-1 ] ) ) == NULL )
    return;
  for( oprog = obj_clss->oprog; oprog != NULL; oprog = oprog->next ) {
    if( oprog->trigger == OPROG_TRIGGER_HIT ) {
      var_ch = ch;
      var_victim = victim;
      var_mob = NULL;
      var_obj = wield;
      var_room = ch->in_room;
      execute( oprog );
    }
  }    
  /*
  switch( weapon->Get_Integer( VAR_POISONED ) ) {
    case 1 :
      af.type = AFF_POISON;
      af.duration = number_range( 5, 5*level );
      af.location = APPLY_STR;
      af.modifier = number_range( -1, -level );
      af.leech = NULL;
      af.level = level;
      add_affect( victim, &af );

    default :
      af.type = AFF_POISON;
      af.duration = number_range( 5, 5*level );
      af.leech = NULL;
      af.level = level;
      add_affect( victim, &af );
  }
  */
  weapon->Set_Integer( POISON_HITS, weapon->Get_Integer( POISON_HITS )-1 );
  
  if( weapon->Get_Integer( POISON_HITS ) == 0 )
    act( ch, "@CThe poison coating $p has faded from it.\r\n", ch, weapon );


  return;
}

bool flaming_weapon( char_data* ch, char_data* victim, obj_data* wield )
{
  if( !ch || !ch->Is_Valid( ) || !victim || !victim->Is_Valid( ) || !wield || 
    !is_set( wield->extra_flags, OFLAG_FLAMING ) )
    return FALSE;

  damage_fire( victim, ch, roll_dice( 2,4 ), "flaming weapon" );
  if( victim->hit <= 0 )
    return TRUE;

  return FALSE;
}

bool blighted_weapon( char_data* ch, char_data* victim, obj_data* wield )
{
  if( !ch || !ch->Is_Valid( ) || !victim || !victim->Is_Valid( ) || !wield ||
    !is_set( wield->extra_flags, OFLAG_WEAPON_BLIGHT ) )
    return FALSE;

  damage_element( victim, ch, roll_dice( 2, 4 ), "blighted weapon", ATT_COLD );

  if( victim->hit <= 0 )
    return TRUE;

  return FALSE;
}

bool diseased_weapon( char_data* ch, char_data* victim, obj_data* wield )
{
  if( !ch || !ch->Is_Valid( ) || !victim || !victim->Is_Valid( ) || !wield ||
    !is_set( wield->extra_flags, OFLAG_DISEASED ) )
    return FALSE;

  damage_element( victim, ch, roll_dice( 1, 4 ), "diseased weapon", ATT_POISON );

  if( victim->hit <= 0 )
    return TRUE;

  return FALSE;
}

/* 
 *   ARMOR ROUTINES
 */


bool absorb_armor( char_data* victim, char_data* ch, const char* dt, int& damage, const char*& loc_name )
{
  char      tmp  [ TWO_LINES ];
  int      roll;
  int  absorbed;
  int         i;
  int       loc;

  roll = number_range( 0, 2000 );

  if( victim->species == NULL && victim->shifted == NULL ) {
    if( roll < 160 )  loc = WEAR_HEAD;
    else if( roll < 240 )  loc = WEAR_NECK;
    else if( roll < 760 )  loc = WEAR_BODY;
    else if( roll < 920 )  loc = WEAR_WAIST;
    else if( roll < 1240 ) loc = WEAR_ARMS;
    else if( roll < 1560 ) loc = WEAR_LEGS;
    else if( roll < 1620 ) loc = WEAR_WRIST_R;
    else if( roll < 1680 ) loc = WEAR_WRIST_L;
    else if( roll < 1760 ) loc = WEAR_HANDS;
    else if( roll < 1800 ) loc = WEAR_FINGER_L;
    else if( roll < 1840 ) loc = WEAR_FINGER_R;
    else if( roll < 1950 ) loc = WEAR_ABOUT;
    else                   loc = WEAR_FEET; 

    absorbed = damage_armor( victim, loc );
    absorbed -= max( 0, absorbed-ch->get_skill( SKILL_PENETRATE ) );
    ch->improve_skill( SKILL_PENETRATE );

    damage -= absorbed+victim->mod_armor;

    if( damage <= 0 ) {
      sprintf( tmp, "Your %s armor absorbs %s's %s.\r\n", dam_loc_name[loc], ch->Name( victim ), dt );
      spam_char( victim, tmp );
      sprintf( tmp, "%s's %s armor absorbs your %s.\r\n", victim->Name( ch ), dam_loc_name[loc], dt );
      spam_char( ch, tmp );
      sprintf( tmp, "%%s's %s armor absorbs %%s's %s.\r\n", dam_loc_name[loc], dt );
      spam_room( tmp, victim, ch );

      return TRUE;
    }

    if( absorb_bracers( victim, ch, damage, absorbed, dt ) )
      return TRUE;

    loc_name = dam_loc_name[loc];

    return FALSE;
  } else {
    roll /= 2;
    int chance[ MAX_ARMOR ] = { 1000, 1000, 1000, 1000, 1000 };
    int narmor[ MAX_ARMOR ] = { 1000, 1000, 1000, 1000, 1000 };
    for( i = 0; i < MAX_ARMOR; i++ ) {
      chance[i] = victim->shifted == NULL ? victim->species->chance[i] : victim->shifted->chance[i];
      narmor[i] = victim->shifted == NULL ? victim->species->armor[i] : victim->shifted->armor[i];
      if( roll <= chance[i] )
         break;
    }

    int reduc = 1000;

    if( i < MAX_ARMOR )
      reduc = narmor[i];
     

    int arm = max( 0, reduc-ch->get_skill( SKILL_PENETRATE )/2 );

    // just a thought about this way of toughening mobs is people might notice that they're
    // doing less damage than 'normal'... so if this doesn't work, let's try sneaking in a
    // hp boost into the create_mobile function

    float minimum = 2/3*arm;
    int  min = (int) minimum;
    
    damage -= number_range( min, arm );

    ch->improve_skill( SKILL_PENETRATE );

    if( i < MAX_ARMOR && i >= 0 )
      loc_name = victim->shifted == NULL ? victim->species->part_name[i] : victim->shifted->part_name[i];
    else {
      bug( "%s's [%i] marmor is not setup properly", victim->shifted == NULL ? victim->Name( ) : victim->shifted->descr->name, victim->shifted == NULL ? victim->species->vnum : victim->shifted->vnum );
      loc_name = "[BUG]";
    }

    if( damage > 0 )       
      return FALSE;

    if( *loc_name != '\0' ) {
      sprintf( tmp, "Your %s absorbs %s's attack.\r\n", loc_name, ch->Name( victim ) );
      spam_char( victim, tmp );
      sprintf( tmp, "You %s %s's %s inflicting no damage.\r\n", dt, victim->Name( ch ), loc_name );
      spam_char( ch, tmp );
      sprintf( tmp, "%%s's %s absorbs %%s's %s.\r\n", loc_name, dt );
      spam_room( tmp, victim, ch );
    } else {
      spam_char( victim, "Your armor absorbs %s's attack.\r\n", ch );
      sprintf( tmp, "You %s %s inflicting no damage.\r\n", dt, victim->Name( ch ) );
      spam_char( ch, tmp );
      spam_room( "%s's armor absorbs %s's attack.\r\n", victim, ch );
    }

    return TRUE;
  }
}

void extract_contents( char_data* ch, obj_data* container )
{
  room_data*     room;

  if( container == NULL || container->array == NULL 
    || ch == NULL || ( room = ch->in_room ) == NULL )
    return;
  if( container->contents.size > 0 )
    send( ch, "The contents of %s are hurled through the room.\r\n", container );

  for( int i = container->contents.size - 1; i >= 0; i-- ) {
    obj_data *content = object(container->contents[i]);
    if (!content || !content->Is_Valid())
      continue;
    content->From(content->number);
    content->To(room);
  }

  return;
}
  
int damage_armor( char_data* ch, int loc )
{
  obj_data*     armor;
  int        absorbed  = 0;

  for( int i = ch->wearing-1; i >= 0; i-- ) {
    if( ( armor = object( ch->wearing[i] ) ) != NULL && armor->position == loc ) {
      if( armor->pIndexData->item_type == ITEM_ARMOR ||
        armor->pIndexData->item_type == ITEM_SHIELD ) 
        absorbed += number_range( 0, armor_class( armor ) );
      if( armor->pIndexData->durability < number_range( 0, 20000 ) ) {
        ch->improve_skill( SKILL_PRESERVE_EQUIPMENT );
        if( ch->get_skill( SKILL_PRESERVE_EQUIPMENT ) < number_range( 0, 20 ) ) {
          if( number_range( 0, 15-4*armor->rust ) <= (int) !is_set( armor->extra_flags, OFLAG_SANCT )
            && number_range( 1, 4+armor->value[0] ) <= 4 && --armor->condition < 0 ) {
              send( ch, "%s you are wearing shatters into pieces.\r\n", armor );
              send_seen( ch, "%s which %s is wearing shatters into pieces.\r\n", armor, ch );
              extract_contents( ch, armor );
              armor->Extract( );
          }
        }
      }
    }
  }

  return absorbed;
}


/*
 *   ATTACK ROUTINES
 */


void critical_hit( char_data* ch, char_data* victim, obj_data* wield, int& damage )
{
  int  weapon;

  if( ch->species != NULL || ch->get_skill( SKILL_CRITICAL_HIT ) == UNLEARNT )
    return;

  if( victim->species && is_set( victim->species->act_flags, ACT_NO_CRIT ) )
    return;

  weapon = ( wield == NULL ? 0 : wield->value[3] );

  if( number_range( 1, 50 ) > ch->get_skill( WEAPON_FIRST + weapon ) )
    return; 

  if( ch->get_skill( SKILL_DEATH_STRIKE ) != UNLEARNT && 
    number_range( 0, 40 ) < ch->get_skill( SKILL_DEATH_STRIKE ) ) {
    send( ch, "You call upon the SHADOWS to DESTROY %s!\r\n", victim );
    send( victim, "%s calls upon the SHADOWS to DESTROY you!\r\n", ch );
    send( *ch->array, "%s calls upon the SHADOWS to DESTROY %s!\r\n", ch, victim );
    ch->improve_skill( SKILL_DEATH_STRIKE );
    damage *= 6;
  } else if( number_range( 0, 30 ) < ch->get_skill( SKILL_CRITICAL_HIT ) ) {
    send( ch, "You CRITICALLY hit %s!\r\n", victim );
    send( victim, "%s CRITICALLY hits you!\r\n", ch );
    send( *ch->array, "%s CRITICALLY hits %s!\r\n", ch, victim );
    ch->improve_skill( SKILL_CRITICAL_HIT );
    damage *= 4;
  }
}

void deadly_strike( char_data* ch, char_data* victim, obj_data* wield, int& damage )
{
  int  weapon;
  int  skill_loc [] = { -1, SKILL_DAGGER_MASTERY, SKILL_BLADE_MASTERY,
    -1, -1, -1, -1, -1, SKILL_AXE_MASTERY, -1, SKILL_SPEAR_MASTERY, -1, 
    -1, -1, -1, -1 };

  if( !can_critical( victim ) )
    return;

  if( ch->species != NULL || ch->get_skill( SKILL_DEADLY_STRIKE ) == UNLEARNT )
    return;

  if( victim->species && is_set( victim->species->act_flags, ACT_NO_CRIT ) )
    return;

  weapon = ( wield == NULL ? 0 : wield->value[3] );
  int skill_master = skill_loc[ weapon ] == -1 ? 0 : ch->get_skill( skill_loc[ weapon ] );

  if( number_range( 1, 100 ) > ch->get_skill( WEAPON_FIRST + weapon ) 
    + skill_master )
    return; 

  if( number_range( 0, 30 ) < ch->get_skill( SKILL_DEADLY_STRIKE ) ) {
    send( ch, "You make a deadly strike on %s!\r\n", victim );
    send( victim, "%s makes a deadly strike on you!\r\n", ch );
    send( *ch->array, "%s makes a deadly strike on %s!\r\n", ch, victim );
    ch->improve_skill( SKILL_DEADLY_STRIKE );
    damage *= 3;
  }
}

void power_strike( char_data* ch, char_data* victim, obj_data* wield, int& damage )
{
  int weapon;

  if( ch->species != NULL || ch->get_skill( SKILL_POWER_STRIKE ) == UNLEARNT )
    return;

  if( victim->species && is_set( victim->species->act_flags, ACT_NO_CRIT ) )
    return;

  weapon = ( wield == NULL ? 0 : wield->value[3] );
  
  if( number_range( 1, 75 ) > ch->get_skill( WEAPON_FIRST + weapon ) )
    return;

  if( number_range( 0, 25 ) < ch->get_skill( SKILL_POWER_STRIKE ) ) {
    send( ch, "Your POWER STRIKE hits %s!\r\n", victim );
    send( victim, "%s hits you with a POWER STRIKE!\r\n", ch );
    send( *ch->array, "%s hits %s with a POWER STRIKE!\r\n", ch, victim );
    ch->improve_skill( SKILL_POWER_STRIKE );
    damage *= 3;
  }
}


bool storm_damage( char_data* ch, char_data* victim, int& damage, const char* dt )
{
  if( !ch )
    return FALSE;

  int roll = number_range( 1, 20 );
  int skill = 0;
  int skill2;
  int type;

  if( is_set( &ch->dance, DANCE_ELEMENTS ) && roll >= 17 ) {
    skill2 = 0;
    ch->improve_skill( SKILL_ELEMENTS );
    damage += ch->get_skill( SKILL_ELEMENTS )/3;
  } else if( is_set( &ch->dance, DANCE_STORMS ) && roll >= 17 ) {
    skill = ch->get_skill( SKILL_STORMS );
    ch->improve_skill( SKILL_STORMS );
    skill2 = 1;
  } else if( is_set( &ch->dance, DANCE_STORMLORD ) && roll >= 16 ) {
    skill = ch->get_skill( SKILL_STORMLORD );
    ch->improve_skill( SKILL_STORMLORD );
    skill2 = 2;
  } else if( is_set( &ch->dance, DANCE_STORMMASTER ) && roll >= 15 ) {
    skill = ch->get_skill( SKILL_STORMMASTER );
    ch->improve_skill( SKILL_STORMMASTER );
    skill2 = 3;
  } else
    return FALSE;

  switch( number_range( 1, 4 ) ) {
  case 1:
    type = ATT_COLD;
    break;
  case 2:
    type = ATT_FIRE;
    break;
  case 3:
    type = ATT_ACID;
    break;
  case 4:
    type = ATT_SHOCK;
    break;
  default:
    bug( "Storm Damage - Undefined Element (%s)", ch );
    return FALSE;
  }

  for( int j = 0 ; j < skill2 ; j++ ) {
    damage += ch->Wisdom( )/2;
    damage += skill;  ;
  }

  if( roll == 20 && !is_set( &ch->dance, DANCE_ELEMENTS ) ) {
    act( ch, "@CRaw magical energy channels through your body.", ch );
    act_notchar( "@C$n glows with raw magical energy.", ch );
    type = ATT_PHYSICAL;
    damage *= 3/2;
  }

  damage_element( victim, ch, damage, dt, type, NULL, true, true );
  return TRUE;
}

void sneak_attack( char_data* ch, char_data* victim, obj_data* wield, int& damage )
{
  if( ch->species != NULL || ch->get_skill( SKILL_SNEAK_ATTACK ) == UNLEARNT || ch->leader == NULL || ch->leader->in_room != ch->in_room )
    return;
  
  if( victim == NULL || !can_critical( victim ) )
    return;

  if( !wield || !is_set( wield->pIndexData->extra_flags, OFLAG_BACKSTAB ) )
    return;

  int weapon = ( wield == NULL ? 0 : wield->value[3] );
  
  if( number_range( 1, 20 ) > ch->get_skill( WEAPON_FIRST + weapon ) )
    return;

  if( number_range( 0, 100 ) < ch->get_skill( SKILL_SNEAK_ATTACK ) ) {
    int level = ch->get_skill( SKILL_HAMSTRING );
    if( level != UNLEARNT && can_hamstring( victim ) && number_range( ch->shdata->level / 2 + level, ch->shdata->level + level ) >= victim->shdata->level ) {
      // new trial of opposed roll formula, fails if you don't roll higher than the mobs lvl
      send( ch, "You move into your opponents blind spot and slice %s tendens!\r\n", victim->His_Her( ) );
      send( victim, "%s hits you with a sneak attack, slicing your tendons!\r\n", ch );
      send( *ch->array, "%s hits %s with a sneak attack, slicing %s tendons!\r\n", ch, victim, victim->His_Her( ) );
      
      physical_affect( ch, victim, SKILL_HAMSTRING, AFF_HAMSTRING );
      ch->improve_skill( SKILL_HAMSTRING );

    } else {
      send( ch, "You move into your opponents blind spot!\r\n" );
      send( victim, "%s hits you with a sneak attack!\r\n", ch );
      send( *ch->array, "%s hits %s with a sneak attack!\r\n", ch, victim );
    }

    ch->improve_skill( SKILL_SNEAK_ATTACK );
    damage += 2*ch->get_skill( SKILL_SNEAK_ATTACK ) + ch->shdata->level / 2;
    
    set_delay( ch, 40 - ch->get_skill( SKILL_SNEAK_ATTACK ) + ch->get_skill( SKILL_HAMSTRING ) );
  }

  return;
}



/*
 *   OFFENSIVE ROUTINES
 */

void auto_gouge( char_data* ch, char_data* victim, obj_data* wield )
{
  int  delay;

  if( wield && wield->value[3] != WEAPON_DAGGER - WEAPON_FIRST )
    return;

  if( ch->species != NULL )
    return;

  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) )
    return;
    
  if( ch->get_skill( SKILL_EYE_GOUGE ) == UNLEARNT ) 
    return;

  if( victim == NULL )
    return;

  if( victim->species != NULL && 
    !is_set( victim->species->act_flags, ACT_HAS_EYES ) ) 
    return;

  if ( is_set( victim->affected_by, AFF_BLIND ) )
    return;

  if( victim->fighting != ch && ch->fighting != victim ) {
    if( !can_kill( ch, victim ) )
      return;
    check_killer( ch, victim );
  }

  delay = gouge_attack( ch, victim, TRUE );

  ch->fighting = victim;
  react_attack( ch, victim );
  set_delay( ch, delay );

  remove_bit( &ch->status, STAT_LEAPING );
  remove_bit( &ch->status, STAT_WIMPY );

  ch->improve_skill( SKILL_EYE_GOUGE );
  return;
}


void auto_disarm( char_data* ch, char_data* victim )
{
  obj_data* obj = NULL;

  if( !ch || !ch->Is_Valid( ) || !victim || !victim->Is_Valid( ) )
    return;

  if( number_range( 0, 2 ) != 1 )
    return;

  if( victim->species && is_set( victim->species->act_flags, ACT_NO_DISARM ) ) 
    return;
  
  if( ch->species != NULL )
    return;

  if( ch->get_skill( SKILL_DISARM ) == UNLEARNT ) 
    return;

  if( get_weapon( ch, WEAR_HELD_R ) == NULL ) 
    return;

  if( ( obj = get_weapon( victim, WEAR_HELD_R ) ) == NULL ) 
    return; 

  if( is_set( obj->pIndexData->extra_flags, OFLAG_NO_DISARM ) ) 
    return;

  int i = ch->get_skill( SKILL_DISARM );
  int j = number_range( 0, 20)+2*( victim->shdata->level-ch->shdata->level );
  if( i < j ) 
    return;
  
  disarm( ch, victim );
}


void stun( char_data* ch, char_data* victim, const char* dt )
{
  char        tmp  [ MAX_INPUT_LENGTH ];
  char_data*  rch;

  if( !ch || !ch->Is_Valid() || !victim || !victim->Is_Valid() )
    return;

  if( victim->species && is_set( victim->species->act_flags, ACT_NO_STUN ) )
    return;

  if( victim->position <= POS_STUNNED || victim->shdata->race == RACE_PLANT )
    return;

  if( ch->species != NULL || ch->get_skill( SKILL_STUN ) == UNLEARNT || 
    ch->get_skill( SKILL_STUN ) <= number_range( 0, 100+5*( 
      7/4*victim->shdata->level-( ch->shdata->level
      -skill_table[SKILL_STUN].level[ch->pcdata->clss] ) ) ) ) 
    return;

  ch->improve_skill( SKILL_STUN );

  if( victim->Seen( ch ) ) 
    send( ch, "-* Your %s momentarily STUNS %s! *-\r\n", dt, victim->Seen_Name( ch ) ); 

  send( victim, "%s's %s momentarily STUNS you!\r\n", ch->Name( victim ), dt );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL || rch == ch || 
      rch == victim || rch->link == NULL || !victim->Seen( rch ) )
      continue;
    sprintf( tmp, "%s's %s momentarily STUNS %s.\r\n", ch->Name( rch ), dt, victim->Seen_Name( rch ) );
    send( tmp, rch ); 
  }

  disrupt_spell( victim );
  set_delay( victim, 40 ); 
}


bool trip( char_data* ch, char_data* victim, obj_data* wield )
{
  if( victim->position < POS_FIGHTING || victim->mount || ch->mount )
    return FALSE;
 
  if( ch->species == NULL && ( wield != NULL && wield->pIndexData->value[ 3 ] != 0 )
    && ch->get_skill( SKILL_TRIP ) == UNLEARNT 
    && ch->get_skill( SKILL_KICK ) < MAX_SKILL_LEVEL ) 
    return FALSE;

  // if the victim is a ghost, or non-trippable, then nothing
  if( victim->species != NULL && ( is_set( victim->species->act_flags, ACT_GHOST ) || 
    is_set( victim->species->act_flags, ACT_NO_TRIP ) || ( ch->get_skill( SKILL_TRIP ) == UNLEARNT && 
    !is_set( victim->species->act_flags, ACT_CAN_TRIP ) ) ) )
    return FALSE;

  // if it's a mob and can't trip or is a ghost, then nothing
  if( ch->species != NULL && ( !is_set( ch->species->act_flags, ACT_CAN_TRIP ) || 
    is_set( ch->species->act_flags, ACT_GHOST ) ) )
    return FALSE; 

  if( ch->species == NULL ) {
    if( number_range( 0, 50*MAX_SKILL_LEVEL ) > 2 * ch->get_skill( SKILL_TRIP ) + ch->get_skill( SKILL_KICK ) )
      return FALSE;
  } else {
    if( number_range( 0, 200 ) > 5 )
      return FALSE;
  }
  
  if( ch->species != NULL && !is_pet( ch ) && ch->fighting != NULL && ch->fighting->fighting != NULL && ch->fighting->fighting != ch )
    return FALSE;

  if( victim->mount != NULL ) {
    send( ch, "You knock %s from %s mount!!\r\n", victim, victim->Him_Her( ) );
    send( victim, "%s knocks you from your mount!!\r\n", ch );
    send( *ch->array, "%s knocks %s from %s mount!!\r\n", ch, victim, victim->Him_Her( ) );
    victim->mount->rider = NULL;
    victim->mount = NULL;
  } else {
    send( ch, "With a well timed kick you knock the feet out from under %s!\r\n", victim );
    fsend( victim, "%s delivers a well timed kick to your feet, knocking you\r\nto the ground!", ch );
    send( *ch->array, "%s kicks %s's leg, knocking %s to the ground.\r\n", ch, victim, victim->Him_Her( ) );
  }

  if( ch->species == NULL && ch->get_skill( SKILL_TRIP ) )
    ch->improve_skill( SKILL_TRIP );

  disrupt_spell( victim );
  set_delay( victim, 32 );
  victim->position = POS_RESTING;

  record_damage( victim, ch, 10 );

  return TRUE;
} 


int charge_damage( char_data* ch )
{
  int i;

  if( ch->species != NULL )
    return 5;

  ch->improve_skill( SKILL_CHARGE );

  i = 100*ch->get_skill( SKILL_CHARGE )/MAX_SKILL_LEVEL + ch->shdata->level/2 + ch->Strength( );
  i = 1+i/25;
  return i;
}


void backstab_damage( char_data* ch, char_data* victim, obj_data* wield, int& damage )
{
  // this is fukt... *4 multi for bs, *8 for assass, *10 for dual-bs
  // really need to nerf all multiplying skills but lets leave that a month or two
  // should be something like grant skill lvl + player lvl bonus damage for each skill
  // eg at lvl 10 .. 10+10 = 20 bonus damage
  // at lvl 50 ... 10+50+10+50 bonus damage
  // or something blahish like that

  int i = 0;
  int j = 0;
  obj_data* left_wield = NULL;
  obj_data* right_wield = NULL;

  left_wield  = get_weapon( ch, WEAR_HELD_L );
  right_wield = get_weapon( ch, WEAR_HELD_R );

  if( right_wield != NULL && is_set( right_wield->pIndexData->extra_flags, OFLAG_BACKSTAB ) ) {
    if( left_wield != NULL && is_set( left_wield->pIndexData->extra_flags, OFLAG_BACKSTAB ) && ch->get_skill( SKILL_DOUBLE_STRIKE ) ) {
      j = 50*ch->get_skill( SKILL_DOUBLE_STRIKE )/MAX_SKILL_LEVEL;
      ch->improve_skill( SKILL_DOUBLE_STRIKE );
      send( ch, "You flow from the shadows, twin weapons poised!\r\n" );
    }
  }

  if( ch->species != NULL ) {
    damage *= 5;
    return;
  }

  if( number_range( 0, 1 ) == 0 )
    ch->improve_skill( SKILL_BACKSTAB );
  else
    ch->improve_skill( SKILL_ASSASSINATE );

  i = 100*ch->get_skill( SKILL_BACKSTAB )/MAX_SKILL_LEVEL + 100*ch->get_skill( SKILL_ASSASSINATE )/MAX_SKILL_LEVEL + j;
  i = 1+i/25;

  damage *= i;

  if( ch->Seen( victim ) || victim->Alert( ) )
    damage /= 2;
}


/*
 *   DEFENSIVE ROUTINES
 */


bool absorb_bracers( char_data* victim, char_data* ch, int& damage, int armor, const char* dt )
{
  char           tmp  [ MAX_INPUT_LENGTH ];
  obj_data*  bracers;

  if( ( bracers = victim->Wearing( WEAR_WRIST_L ) ) != NULL )
    armor -= bracers->value[2];

  if( ( bracers = victim->Wearing( WEAR_WRIST_R ) ) != NULL )
    armor -= bracers->value[2];

  if( armor >= 0 || ( damage -= number_range( 0, -armor ) ) > 0 )
    return FALSE;

  spam_char( victim, "Your bracers glow briefly and %s's %s is deflected.\r\n", ch, dt );
  spam_char( ch, "%s's bracers glow briefly and your %s is magically deflected.\r\n", victim, dt );

  sprintf( tmp, "%%s's bracers glow briefly and %%s's %s is deflected.", dt );
  spam_room( tmp, victim, ch );

  return TRUE;
}


bool absorb_shield( char_data* victim, char_data* ch, const char* dt, int& damage )
{
  char tmp [ TWO_LINES ];

  if( !is_set( victim->affected_by, AFF_PROTECT ) )
    return FALSE;

  damage -= number_range( 0, 3 );

  if( damage > 0 ) 
    return FALSE;

  spam_char( victim, "The air crackles as %s's %s is mysteriously blocked.\r\n", ch, dt );
  spam_char( ch, "The air around %s crackles as your %s is mysteriously blocked.\r\n", victim, dt );

  sprintf( tmp, "The air crackles as %%s's %s is mysteriously blocked.\r\n", dt );
  spam_room( tmp, ch, victim );

  return TRUE;
}

// electrical shields should go in here

bool ion_shield( char_data* ch, char_data* victim )
{
  int damage = 0;

  if( IS_AFFECTED( victim, AFF_ION_SHIELD ) )
    damage += spell_damage( SPELL_ION_SHIELD, 0 );
  else if( IS_AFFECTED( victim, AFF_LIGHTNING_SHIELD ) ) 
    damage += spell_damage( SPELL_LIGHTNING_SHIELD, 0 );
  else if( IS_AFFECTED( victim, AFF_SPARK_SHIELD ) ) 
    damage += spell_damage( SPELL_SPARK_SHIELD, 0 );
  else if( IS_AFFECTED( victim, AFF_SHOCK_SHIELD ) ) 
    damage += spell_damage( SPELL_SHOCK_SHIELD, 0 );

  if( damage > 0 ) {
    damage_element( ch, victim, damage, "electric spark", ATT_SHOCK );
    if( ch->hit <= 0 )
      return TRUE;
  }
  if( ch->in_room != victim->in_room )
    return TRUE;

  return FALSE;
}

// Acid shield spells should go in here

bool thorn_shield( char_data* ch, char_data* victim )
{
  int damage = 0;

  if( IS_AFFECTED( victim, AFF_THORN_SHIELD ) || is_set( &victim->dance, DANCE_THORNS ) ) {
    if( is_set( &victim->dance, DANCE_THORNS ) )
      victim->improve_skill( SKILL_THORNS ); 
    damage += spell_damage( SPELL_THORN_SHIELD, 0 );
  }
  else if( IS_AFFECTED( victim, AFF_CAUSTIC_SHIELD ) )
    damage += spell_damage( SPELL_CAUSTIC_SHIELD, 0 );
  else if( IS_AFFECTED( victim, AFF_EROSION_SHIELD ) )
    damage += spell_damage( SPELL_EROSION_SHIELD, 0 );
  else if( IS_AFFECTED( victim, AFF_CORROSIVE_SHIELD ) )
    damage += spell_damage( SPELL_CORROSIVE_SHIELD, 0 );

  if( damage > 0 ) {
    damage_element( ch, victim, damage, "acidic thorns", ATT_ACID );
    if( ch->hit <= 0 )
      return TRUE;
  }
  if( ch->in_room != victim->in_room )
    return TRUE;

  return FALSE;
}

// Cold damage shields in here

bool hoar_shield( char_data* ch, char_data* victim )
{
  int damage = 0;

  if( IS_AFFECTED( victim, AFF_ABSOLUTE_ZERO ) )
    damage += spell_damage( SPELL_ABSOLUTE_ZERO, 0 );
  else if( IS_AFFECTED( victim, AFF_HOAR_FROST ) )
    damage += spell_damage( SPELL_HOAR_FROST, 0 );
  else if( IS_AFFECTED( victim, AFF_FROST_SHIELD ) )
    damage += spell_damage( SPELL_FROST_SHIELD, 0 );
  else if( IS_AFFECTED( victim, AFF_HOAR_SHIELD ) )
    damage += spell_damage( SPELL_HOAR_SHIELD, 0 );

  if( damage > 0 ) {
    damage_element( ch, victim, damage, "hoar frost", ATT_COLD );
    if( ch->hit <= 0 )
      return TRUE;
  }
  
  if( ch->in_room != victim->in_room )
    return TRUE;
  
  return FALSE;
}

bool dance_winter( char_data* ch, char_data* victim )
{
  int dam = victim->Wisdom()/5 + 2*victim->get_skill( SKILL_WINTER )/MAX_SKILL_LEVEL;

  if( is_set( &victim->dance, DANCE_WINTER ) ) {
    damage_cold( ch, victim, roll_dice( 1, dam ), "chilling cold" );
    victim->improve_skill( SKILL_WINTER );

    if( ch->hit <= 0 )
      return TRUE;
  }
  
  if( ch->in_room != victim->in_room )
    return TRUE;

  return FALSE;
}  

bool dance_summer( char_data* ch, char_data* victim )
{
  int dam = victim->Wisdom()/5 + 2*victim->get_skill( SKILL_SUMMER )/MAX_SKILL_LEVEL;

  if( is_set( &victim->dance, DANCE_SUMMER ) ) {
    damage_fire( ch, victim, roll_dice( 1, dam ), "burning heat" );
    victim->improve_skill( SKILL_SUMMER );

    if( ch->hit <= 0 )
      return TRUE;
  }
  
  if( ch->in_room != victim->in_room )
    return TRUE;

  return FALSE;
}

bool dance_shards( char_data* ch, char_data* victim )
{
  int dam = victim->Wisdom()/2;
  int dam2 = 5*victim->get_skill( SKILL_SHARDS )/MAX_SKILL_LEVEL;

  if( is_set( &victim->dance, DANCE_SHARDS ) ) {
    damage_physical( ch, victim, roll_dice( dam2, dam ), "cutting shards" );
    victim->improve_skill( SKILL_SHARDS );

    if( ch->hit <= 0 )
      return TRUE;
  }
  
  if( ch->in_room != victim->in_room )
    return TRUE;

  return FALSE;
}  

// put holy shield spells in here

bool holy_aura( char_data* ch, char_data* victim )
{
  int damage = 0;

  if( is_good( ch ) )
    return FALSE;

  if( IS_AFFECTED( victim, AFF_HOLY_AURA ) )
    damage += spell_damage( SPELL_HOLY_AURA, 0 );
  else if( IS_AFFECTED( victim, AFF_SAINTLY_AURA ) )
    damage += spell_damage( SPELL_SAINTLY_AURA, 0 );
  else if( IS_AFFECTED( victim, AFF_HALLOWED_AURA ) )
    damage += spell_damage( SPELL_HALLOWED_AURA, 0 );
  else if( IS_AFFECTED( victim, AFF_BLESSED_AURA ) )
    damage += spell_damage( SPELL_BLESSED_AURA, 0 );

  if( damage > 0 ) {
    damage_element( ch, victim, damage, "holy aura", ATT_HOLY );
    if( ch->hit <= 0 )
      return TRUE;
  }

  if( ch->in_room != victim->in_room )
    return TRUE;
  
  return FALSE;
}

// put unholy shield spells in here (not sorcerer types)

bool unholy_aura( char_data* ch, char_data* victim )
{
  int damage = 0;

  if( is_evil( ch ) )
    return FALSE;

  if( IS_AFFECTED( victim, AFF_UNHOLY_AURA ) ) 
    damage += spell_damage( SPELL_UNHOLY_AURA, 0 );
  else if( IS_AFFECTED( victim, AFF_WICKED_AURA ) )
    damage += spell_damage( SPELL_WICKED_AURA, 0 );
  else if( IS_AFFECTED( victim, AFF_DREADFUL_AURA ) )
    damage += spell_damage( SPELL_DREADFUL_AURA, 0 );
  else if( IS_AFFECTED( victim, AFF_EVIL_AURA ) )
    damage += spell_damage( SPELL_EVIL_AURA, 0 );

  if( damage > 0 ) {
    damage_element( ch, victim, damage, "unholy aura", ATT_UNHOLY );
    if( ch->hit <= 0 )
      return TRUE;
  }

  if( ch->in_room != victim->in_room )
    return TRUE;

  return FALSE;
}

bool ghost_shield( char_data* ch, char_data* victim )
{
  if( IS_AFFECTED( victim, AFF_GHOST_SHIELD ) ) {
    damage_element( ch, victim, spell_damage( SPELL_GHOST_SHIELD, 0 ), "surrounding ghosts", ATT_UNHOLY );
    if( ch->hit <= 0 )
      return TRUE;
  }
  if( ch->in_room != victim->in_room )
    return TRUE;

  return FALSE;
}

bool demon_shield( char_data* ch, char_data* victim )
{
  if( IS_AFFECTED( victim, AFF_DEMON_SHIELD ) ) {
    damage_element( ch, victim, spell_damage( SPELL_DEMON_SHIELD, 0 ), "shrieking demons", ATT_UNHOLY );
    if( ch->hit <= 0 )
      return TRUE;
  }
  if( ch->in_room != victim->in_room )
    return TRUE;
  
  return FALSE;
}

// Put all fire shield types in here.

bool fire_shield( char_data* ch, char_data* victim ) 
{
  int damage = 0;

  if( IS_AFFECTED( victim, AFF_INFERNO_SHIELD ) ) 
    damage += spell_damage( SPELL_INFERNO_SHIELD, 0 );
  else if( IS_AFFECTED( victim, AFF_FIERY_SHIELD ) ) 
    damage += spell_damage( SPELL_FIERY_SHIELD, 0 );
  else if( IS_AFFECTED( victim, AFF_FIRE_SHIELD ) ) 
    damage += spell_damage( SPELL_FIRE_SHIELD, 0 );
  else if( IS_AFFECTED( victim, AFF_FLAME_SHIELD ) ) 
    damage += spell_damage( SPELL_FLAME_SHIELD, 0 );

  if( damage > 0 ) {
    damage_element( ch, victim, damage, "fire shield", ATT_FIRE );
    if( ch->hit <= 0 )
      return TRUE;
  }
  if( ch->in_room != victim->in_room )
    return TRUE;
  
  return FALSE;
}

bool whirling_shield( char_data* ch, char_data* victim )
{
  if( IS_AFFECTED( victim, AFF_WHIRLING_DEATH ) ) {
    damage_element( ch, victim, spell_damage( SPELL_WHIRLING_DEATH, 0 ), "dicing blades", ATT_PHYSICAL );
    if( ch->hit <= 0 )
      return TRUE;
  }
  if( ch->in_room != victim->in_room )
    return TRUE;
  
  return FALSE;
}

bool physical_shield( char_data* ch, char_data* victim )
{
  int damage = 0;

  if( IS_AFFECTED( victim, AFF_THISTLE_SKIN ) )
    damage += spell_damage( SPELL_THISTLE_SKIN, 0 );
  else if( IS_AFFECTED( victim, AFF_BRAMBLE_SKIN ) )
    damage += spell_damage( SPELL_BRAMBLE_SKIN, 0 );

  if( damage > 0 ) {
    damage_element( ch, victim, damage, "brambles", ATT_MAGIC );
    if( ch->hit <= 0 )
      return TRUE;
  }
  if( ch->in_room != victim->in_room )
    return TRUE;

  return FALSE;
}

bool elemental_aura( char_data* ch, char_data* victim )
{
  species_data* spc = victim->shifted == NULL ? victim->species : victim->shifted;

  if( spc == NULL )
    return FALSE;

  if( is_set( spc->act_flags, ACT_FIRE_AURA ) ) {
    damage_fire( ch, victim, roll_dice( 1, 4 ), "fire aura" );
    if( ch->hit <= 0 )
      return TRUE;
    if( ch->in_room != victim->in_room )
      return TRUE;
  }

  if( is_set( spc->act_flags, ACT_COLD_AURA ) ) {
    damage_cold( ch, victim, roll_dice( 1, 4 ), "cold aura" );
    if( ch->hit <= 0 )
      return TRUE;
    if( ch->in_room != victim->in_room )
      return TRUE;
  }

  if( is_set( spc->act_flags, ACT_SHOCK_AURA ) ) {
    damage_shock( ch, victim, roll_dice( 1, 4 ), "shock aura" );
    if( ch->hit <= 0 )
      return TRUE;
    if( ch->in_room != victim->in_room )
      return TRUE;
  }

  if( is_set( spc->act_flags, ACT_ACID_AURA ) ) {
    damage_acid( ch, victim, roll_dice( 1, 4 ), "acid aura" );
    if( ch->hit <= 0 )
      return TRUE;
    if( ch->in_room != victim->in_room )
      return TRUE;
  }

  return FALSE;
}


bool block_shield( char_data* ch, char_data* victim, const char* dt )
{
  char          tmp  [ TWO_LINES ];
  obj_data*  shield;
  int          roll;

  if( victim->shifted != NULL )
    return FALSE;

  if ((shield = get_shield(victim, WEAR_HELD_L)) == NULL)
    return FALSE; 

  if ((roll = number_range(0, 100)) > 25)
    return FALSE;

  if (roll > shield->value[1]/2+( victim->pcdata == NULL ? 7 : 10*victim->get_skill( SKILL_SHIELD )/MAX_SKILL_LEVEL ) ) 
    return FALSE;

  damage_armor( ch, WEAR_HELD_L );
  victim->improve_skill( SKILL_SHIELD ); 

  spam_char( victim, "%s's %s is blocked by your shield.\r\n", ch, dt );
  spam_char( ch, "%s blocks your %s with %s shield.\r\n", victim, dt, victim->His_Her( ) );
  sprintf( tmp, "%%s blocks %%s's %s with %s shield.\r\n", dt, victim->His_Her( ) );
  spam_room( tmp, victim, ch );

  shield_bash( victim, ch, shield );
     
  return TRUE;
}    

void shield_bash( char_data* ch, char_data* victim, obj_data* shield )
{
  int damage;
  
  if( ch->species == NULL && is_set( ch->pcdata->pfile->flags, PLR_PARRY ) )
    return;

  if( number_range( 0, 10*MAX_SKILL_LEVEL ) >= ch->get_skill( SKILL_SHIELD_BASH ) )
    return;

  damage = number_range( 1, shield->Weight( 1 )/50 );
  damage_modifiers( ch, victim, shield, damage );
  damage += armor_class( shield );

  damage_physical( victim, ch, damage, "shield strike" );

  ch->improve_skill( SKILL_SHIELD_BASH );

  return;
}

bool shadow_dance( char_data* ch, char_data* victim, const char* dt )
{
  char tmp [ TWO_LINES ];

  if( victim->pcdata == NULL || number_range( 0, 10*MAX_SKILL_LEVEL ) >= victim->get_skill( SKILL_SHADOW_DANCE ) ||
    victim->shifted != NULL || is_entangled( victim, "" ) )
    return FALSE;
  
  victim->improve_skill( SKILL_SHADOW_DANCE );

  spam_char( victim, "You meld with the shadows avoiding %s's %s.\r\n", ch, dt );
  spam_char( ch, "%s melds with the shadows avoiding your %s.\r\n", victim, dt );
  sprintf( tmp, "%%s melds with the shadows avoiding %%s's %s.\r\n", dt );
  spam_room( tmp, victim, ch );
 
  return TRUE;
}


bool firefly( char_data* ch, char_data* victim )
{
  int roll = number_range( 1, 100 );
  int wis = victim->Wisdom(); 
  int wis2 = ch->Wisdom();
  int skill = 10*victim->get_skill( SKILL_FIREFLY )/MAX_SKILL_LEVEL;

  if( victim->species != NULL || !is_set( &victim->dance, DANCE_FIREFLY ) || victim->shifted != NULL )
    return FALSE;

  // victim has to roll > 95 to distract attacker
  roll = roll + wis + skill - wis2;

  if( ch->Can_See( ) && ( ch->species && is_set( ch->species->act_flags, ACT_HAS_EYES ) ) && roll > 95 ) {
    spam_char( victim, "The fireflies spin, distracting %s.\r\n", ch );
    spam_char( ch, "Lights dance around %s, causing your blow to miss its mark.\r\n", victim );
    spam_room( "Lights dance around %s, seeming to distract %s.", victim, ch );
    victim->improve_skill( SKILL_FIREFLY );
    return TRUE;
  }

  return FALSE;
}

bool off_hand_parry( char_data* ch, char_data* victim, int& roll )
{
  obj_data*            obj;
  int               defend = 0;

  if( victim->species != NULL ) {
    if( ( obj = get_weapon( victim, WEAR_HELD_L ) )  != NULL && obj->pIndexData->item_type == ITEM_WEAPON ) {
      defend = 25;
      defend = is_entangled( victim, "" ) ? 2 : 1;
      roll -= defend;
    }
  } else if( victim->shifted != NULL ) {
      return FALSE;
  } else {
      if( ( obj = get_weapon( victim, WEAR_HELD_L ) ) != NULL && obj->pIndexData->item_type == ITEM_WEAPON ) {
        defend = 30*victim->get_skill( SKILL_OFFHAND_PARRY )/MAX_SKILL_LEVEL;
        defend /= is_entangled( victim, "" ) ? 2 : 1;
        roll -= defend;
      }
  }

  if( roll >= 0 )
    return FALSE;

  spam_char( victim, "You parry with your off-hand %s's attack.\r\n", ch );
  spam_char( ch, "%s parries your attack with %s off-hand weapon.\r\n", victim, victim->His_Her( ) );
  spam_room( "%s parries %s's attack.\r\n", victim, ch );

  victim->improve_skill( SKILL_OFFHAND_PARRY );

  return TRUE;
}
      
bool parry( char_data* ch, char_data* victim, int& roll )
{
  int defend = 0;

  if( victim->species != NULL ) {
    if( get_weapon( victim, WEAR_HELD_R ) != NULL ) {
      defend = 25;
      defend /= is_entangled( victim, "" ) ? 2 : 1;
      roll -= defend;
    }
  } else if( victim->shifted != NULL ) {
    return FALSE;
  } else {
    defend = 40*victim->get_skill( SKILL_PARRY )/MAX_SKILL_LEVEL;
    defend /= is_entangled( victim, "" ) ? 2 : 1;
    roll -= defend;
  }

  if( roll >= 0 ) 
    return FALSE;

  spam_char( victim, "You parry %s's attack.\r\n", ch );
  spam_char( ch, "%s parries your attack.\r\n", victim );
  spam_room( "%s parries %s's attack.\r\n", victim, ch );

  victim->improve_skill( SKILL_PARRY );
  
  if( victim->get_skill( SKILL_COUNTER_ATTACK ) != UNLEARNT /* && victim->pcdata && victim->pcdata->clss == CLSS_DANCER */ && victim->Wearing( WEAR_HELD_R ) != NULL ) {
    obj_data* wield = get_weapon( victim, WEAR_HELD_R );
    if( number_range( 0, 49 ) < 10*victim->get_skill( SKILL_COUNTER_ATTACK )/MAX_SKILL_LEVEL && wield != NULL ) {
      act( victim, "@GYou duck in for a quick counter strike!@n\r\n", victim );
      act_notchar( "@G$n ducks in for a quick counter strike!@n\r\n", victim );
      attack( victim, ch, attack_word( wield ), wield, wield->value[2] + victim->damroll, 0 );
      // may be dead here (fireshield, etc)
      if( victim->Is_Valid( ) )
        victim->improve_skill( SKILL_COUNTER_ATTACK );
    }
  }

  return TRUE;
}



bool guard( char_data* ch, char_data* victim, int& roll )
{
  int defend = 0;

  if( victim->species != NULL ) {
    if( victim->Wearing( WEAR_HELD_R ) != NULL ) {
      defend = 30;
      defend /= is_entangled( victim, "" ) ? 2 : 1;
      roll -= defend;
    }
  } else if( victim->shifted != NULL ) {
      return FALSE;
  } else {
    defend = 50*victim->get_skill( SKILL_GUARD )/MAX_SKILL_LEVEL; 
    defend /= is_entangled( victim, "" ) ? 2 : 1;
    roll -= defend;
  }
  
  if( roll >= 0 )
    return FALSE;

  spam_char( victim, "You guard yourself from %s's attack.\r\n", ch );
  spam_char( ch, "%s guards from your attack.\r\n", victim );
  spam_room( "%s guards from %s's attack.\r\n", victim, ch );

  victim->improve_skill( SKILL_GUARD );

  return TRUE;
}


bool tumble( char_data* ch, char_data* victim, int& roll )
{
  int dex;

  if( victim->species != NULL  || victim->shifted != NULL )
    return FALSE;

  if( !victim->get_skill( SKILL_TUMBLE ) || is_entangled( victim, "" ) )
    return FALSE;

  dex = victim->Dexterity( );

  roll -= 40*victim->get_skill( SKILL_TUMBLE )/MAX_SKILL_LEVEL+2*dex-20;

  if( roll > 0 )
    return FALSE;

  spam_char( ch, "%s tumbles away, avoiding your attack.\r\n", victim );
  spam_char( victim, "You tumble away, avoiding %s's attack.\r\n", ch );
  spam_room( "%s tumbles away, avoiding %s's attack.\r\n", victim, ch );

  victim->improve_skill( SKILL_TUMBLE );

  return TRUE;
}

bool ward_blow( char_data* ch, char_data* victim, int& roll )
{
  if( victim->species != NULL || victim->shifted != NULL || is_entangled( victim, "" ) )
    return FALSE;

  roll -= 20*victim->get_skill( SKILL_WARD_BLOW )/MAX_SKILL_LEVEL;

  if( roll > 0 )
    return FALSE;

  spam_char( ch, "%s wards your attack from %s body.\r\n", victim, victim->His_Her( ) );
  spam_char( victim, "You ward %s's attack from your body.\r\n", ch );
  spam_room( "%s wards %s's attack.\r\n", victim, ch );
  
  victim->improve_skill( SKILL_WARD_BLOW );

  return TRUE;
}

bool side_step( char_data* ch, char_data* victim, int& roll )
{
  if( victim->species != NULL || victim->shifted != NULL || is_entangled( victim, "" ) )
    return FALSE;

  roll -= 20*victim->get_skill( SKILL_SIDE_STEP )/MAX_SKILL_LEVEL;

  if( roll > 0 )
    return FALSE;

  spam_char( ch, "%s side steps your attack.\r\n", victim );
  spam_char( victim, "You side step %s's attack.\r\n", ch );
  spam_room( "%s side steps %s's attack.\r\n", victim, ch );
  
  victim->improve_skill( SKILL_SIDE_STEP );

  return TRUE;
}

bool mirror_image( char_data* ch, char_data* victim )
{
  if( !is_set( &victim->dance, DANCE_MIRROR_IMAGE ) )
    return FALSE;

  if( number_range( 0, 40 ) > ch->get_skill( SKILL_MIRROR_IMAGE ) ) 
    return FALSE;

  spam_char( ch, "You strike %s, but your attack passes right through %s.\r\n", victim, victim->Him_Her( ) );
  spam_char( victim, "%s strikes one of your images.\r\n", ch );
  spam_room( "%s strikes %s, but the attack passes right through.\r\n", ch, victim );

  victim->improve_skill( SKILL_MIRROR_IMAGE );

  return TRUE;
}

bool dodge( char_data* ch, char_data* victim, int& roll )
{
  int dex;
  int defend = 0;

  dex = victim->Dexterity( ); 

  if( victim->species == NULL ) {
    defend = 30*victim->get_skill( SKILL_DODGE )/MAX_SKILL_LEVEL+2*dex-20;
    defend /= is_entangled( victim, "" ) ? 4 : 1;
    roll -= defend;
  }
  else {
    defend = 4*dex;
    defend /= is_entangled( victim, "" ) ? 2 : 1;
    roll -= defend; 
  }

  if( roll > 0 )
    return FALSE;

  spam_char( ch, "%s dodges your attack.\r\n", victim );
  spam_char( victim, "You dodge %s's attack.\r\n", ch );
  spam_room( "%s dodges %s's attack.\r\n", victim, ch );

  victim->improve_skill( SKILL_DODGE );

  return TRUE;
}


bool simulcrum( char_data* ch, char_data* victim )
{
  char_data* pet;
  player_data* pc;

  if( victim->species != NULL  )
    return FALSE;

  pc = player( victim );

  int wis = victim->Wisdom( );
  int roll = number_range( 1, 100 ) + 10*victim->get_skill( SPELL_SIMULCRUM )/MAX_SKILL_LEVEL + wis/2;

  if( ( pet = pc->familiar ) == NULL )
    return FALSE;

  if( !IS_AFFECTED( victim, AFF_SIMULCRUM ) || pet->in_room->vnum != victim->in_room->vnum || roll < 100 )
    return FALSE;

  send( ch, "As you strike %s their form is wreathed in shadow.\r\n", victim );
  send( victim, "You feel the shadows bend their will to your dark design.\r\n" );
  send( *ch->array, "At the point of being struck, %s's form is wreathed in shadow.\r\n", victim );
  
  return TRUE;
}

void damage_entrap( char_data* ch, int damage, int affect, const char* word )
{
  affect_data*         paf = NULL;

  if( !IS_AFFECTED( ch, affect ) )
    return;

  for( int i = ch->affected.size-1; i >= 0; i-- ) {
    if( ch->affected[i]->type == affect ) {
      paf = ch->affected[i];
      break;
    }
    if( paf == NULL && i == 0 ) {
      bug( "Is_Affected: %s Character with no affect [%s].", word, ch );
      remove_bit( ch->affected_by, affect );
      return;
    }
  }

  if( paf == NULL )
    return;

  paf->mmodifier[0] -= damage;

  if( paf->mmodifier[0] <= 0 )
    strip_affect( ch, affect );
  
  return;
}

bool physical_entrap( char_data* ch, char_data* victim, int& damage, int affect, const char* word )
{
  affect_data*         paf = NULL;
  int            reduction = 0;

  if( !IS_AFFECTED( ch, affect ) )
    return FALSE;

  paf = find_affect( ch, affect );

  if( paf == NULL )
    return FALSE;

  if( affect == AFF_BRIARTANGLE )
    damage_physical( paf->victim, ch, spell_damage( SPELL_BRIARTANGLE, 0 ), "*The entangling briars" );

  if( number_range( 0, 100 ) > paf->mmodifier[1] )
    return FALSE;
  
  reduction = paf->mmodifier[0];

  if( damage <= reduction ) {
    paf->mmodifier[0] -= damage;
    damage = 0;
  }
  else {
    paf->mmodifier[0] -= damage;
    damage -= reduction;
  }

  if( damage <= 0 ) {
    send( ch, "The %s interferes with your attacks.\r\n", word );
    send( victim,  "The %s surrounding %s blocks %s attack.\r\n", word, ch, ch->His_Her( ) );
    spam_room( "%s's attack gets interfered with.\r\n",  ch, victim );
    if( paf->mmodifier[0] <= 0 )
      strip_affect( ch, affect ); 
    return TRUE;
  }

  if( paf->mmodifier[0] <= 0 )
    strip_affect( ch, affect );
  
  return FALSE;
}

bool misses_blindly( char_data* ch, char_data* victim, int& chance, const char* dt )
{
  if( chance < 0 ) {
    spam_char( victim, "%s's attack misses you.\r\n", ch );
    spam_char( ch, "You miss %s.\r\n", victim );
    spam_room( "%s's attack misses %s.\r\n", ch, victim );
    return TRUE;
  }

  if( !ch->in_room->Seen( ch ) && ( chance -= 30 ) < 0 ) {
    spam_char( ch, "You swing wildly in the dark missing everything.\r\n" );
    spam_room( "%s swings wildly missing everything.\r\n", ch, NULL );
    return TRUE;
  }

  if( !victim->Seen( ch ) ) {
    if( ( chance -= 30 ) < 0 ) {
      spam_char( ch, "You swing at your unseen victim, but hit nothing.\r\n" );
      spam_char( victim, "%s tries to hit you, but swings in the wrong direction.\r\n", ch );
      spam_room( "%s tries to hit %s, but swings in the wrong direction.\r\n", ch, victim );
      return TRUE;
    }
  } else {
    if( is_set( victim->affected_by, AFF_DISPLACE ) && ( chance -= 10 ) < 0 ) {
      spam_char( ch, "Your %s seems to hit %s, but passes through %s.\r\n", dt, victim, victim->Him_Her( ) );
      spam_char( victim, "%s's %s strikes your displaced image, doing you no harm.\r\n", ch, dt );
      spam_room( "%s strikes the displaced image of %s.\r\n", ch, victim );
      return TRUE;
    }
    if( is_set( victim->affected_by, AFF_OBSCURING_MIST ) && ( chance -= 30 ) < 0  ) {
      spam_char( ch, "Your %s seems to hit %s but the mist confuses you.\r\n", dt, victim );
      spam_char( victim, "%s's %s passes harmlessly through the mist.\r\n", ch, dt );
      spam_room( "%s strikes at the obscured image of %s.\r\n", ch, victim );
      return TRUE;
    }
  }
 
  return FALSE;
}


void invulnerable( char_data* ch, int& damage )
{
  if( IS_AFFECTED( ch, AFF_INVULNERABILITY ) )
      damage = 8*damage/10;
 
  return;
}


void barkskin( char_data* ch, int& damage )
{
  if( IS_AFFECTED( ch, AFF_BARKSKIN ) ) 
    damage = 9*damage/10;

  if( IS_AFFECTED( ch, AFF_IRONSKIN ) )
    damage = 8*damage/10;

  return;
}

void protgood( char_data* ch, char_data* victim, int& damage )
{
  if( IS_AFFECTED( ch, AFF_PROTECT_GOOD ) && is_good( victim )
    && is_evil( ch ) )
    damage = 7*damage/10;

  return;
}

void wraith_form( char_data* ch, char_data* victim, int& damage )
{
  if( IS_AFFECTED( ch, AFF_WRAITH_FORM ) )
    damage = 9*damage/10;

  return;
}

void protevil( char_data* ch, char_data* victim, int& damage )
{
  if(IS_AFFECTED( ch, AFF_PROTECT_EVIL ) && is_evil( victim )
    && is_good( ch ) )
    damage = 7*damage/10;

  return;
}

void treeform( char_data* ch, char_data* victim, int& damage )
{
  if( IS_AFFECTED( ch, AFF_TREE_FORM ) )
    damage = 9*damage/10;

  return;
}

/*
 *   CONFUSED CHARACTER ROUTINE
 */

   
bool confused_char( char_data* ch )
{
  char *word[] = { "hop", "pout", "smile", "growl", "sulk", "drool", "clap", "fume" };

  //char_data*  rch;
  int           i  = number_range( 0, 43-2*affect_level( ch, AFF_CONFUSED ) );

  if( i < 8 ) {
    interpret( ch, word[i] );
    return true;
  }
  /*
  if( i < 16 ) 
    if( ( rch = rand_victim( ch ) ) != NULL && rch != ch ) {
      //ch->fighting = rch ;
      //multi_hit( ch, rch );
    }
  */    
  return false;
}

   

/*
 *    SUPPORT ROUTINES
 */

const char* weapon_class( obj_clss_data* obj )
{
  if( obj && obj->value[3] >= 0 && obj->value[3] < WEAPON_COUNT )
    return skill_table[ WEAPON_FIRST+obj->value[3] ].name;

  return "none";
}

const char* weapon_attack( obj_clss_data* obj )
{
  if( obj && obj->value[3] >= 0 && obj->value[3] < WEAPON_COUNT )
    return weapon_attack_name[ obj->value[3] ];

  return "none";
}

void dance_flavour( char_data* ch )
{
  if( ch->species != NULL )
    return;

  if( is_dancing( ch ) && number_range( 1, 20 ) == 1 ) {
    act( ch, "@GYou spin and twirl, turning the fray into a deadly dance!@n\r\n", ch );
    act_notchar( "@G$n spins and twirls, turning the fray into a deadly dance!@n\r\n", ch );
  }
  return;
}
