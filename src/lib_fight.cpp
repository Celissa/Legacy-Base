#include "system.h"


/*
 *   FIGHT ROUTINES
 */

void* code_song( void** argument )
{
  char_data*   ch = (char_data*) argument[0];
  int        song = (int) argument[1];

  if( song < SPELL_FIRST || song >= SPELL_MAX ) {
    code_bug( "Code_Song: Invalid Song" );
    return NULL;
  }

  if( ch == NULL || ch->pcdata != NULL ) {
    code_bug( "Code_Song: Invalid singer" );
    return NULL;
  }

  if( !spell_table[ song-SPELL_FIRST ].song ) {
    code_bug( "Code_Song: It is not a song." );
    return NULL;
  }

  cast_data* cast = new cast_data;
  cast->spell     = song-SPELL_FIRST;
  cast->mana      = mana_cost( ch, song );
  cast->cast_type = UPDATE_SONG;

  ch->cast = cast;
  return NULL;
}

void* code_song_end( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  end_song( ch );
  return NULL;
}

void* code_dance( void** argument )
{
  char_data*     ch = (char_data*) argument[0];
  int         dance = (int) argument[1];

  if( dance < 0 || dance > MAX_DANCE ) {
    code_bug( "Code_Dance: Invalid Dance" );
    return NULL;
  }

  if( ch == NULL )
    return NULL;

  if( ch->pcdata != NULL )
    return NULL;

  set_bit( &ch->dance, dance );

  return NULL;
}

void* code_dance_end( void** argument )
{
  char_data*     ch = (char_data*) argument[0];
  int         dance = (int) argument[1];

  if( ch == NULL )
    return NULL;

  if( dance < 0 || dance > MAX_DANCE ) {
    code_bug( "Code_Dance_End: Invalid Dance" );
    return NULL;
  }

  remove_bit( &ch->dance, dance );
  return NULL;
}

void* code_attack_weapon( void** argument )
{
  char_data*      ch  = (char_data*) argument[0];
  char_data*  victim  = (char_data*) argument[1];
  int            dam  = (int)        argument[2];
  char*       string  = (char*)      argument[3];
  obj_data*    wield;
  int              i;
//  int         ranged;

  if( string == NULL ) {
    code_bug( "Code_Attack_Weapon: NULL attack string." );
    return NULL;
  }

  // Sanity check, and require attacker to be standing up.
  if( ch == NULL || !ch->Is_Valid() || victim == NULL || !victim->Is_Valid() || ch->in_room != victim->in_room || ch->position < POS_FIGHTING ) 
    return NULL;

  wield = get_weapon( ch, WEAR_HELD_R );

  if( wield == NULL )
    wield = get_weapon( ch, WEAR_HELD_L );

  push( );
 /* 
  if( wield != NULL && wield->pIndexData->item_type != WEAPON_BOW )
    ranged = ATT_BRAWL;
  else
    ranged = ATT_RANGED;
*/
  if( ch->species != NULL )
    i = attack( ch, victim, wield == NULL ? string : attack_word( wield ), wield, wield == NULL ? dam : dam*5/4, 0 );
  else
    i = attack( ch, victim, wield == NULL ? string : attack_word( wield ), wield, wield == NULL ? dam : -1, 0 );

  pop( );

  // Queue up a leap, if one isn't already queued.
  if( ch && ch->Is_Valid( ) && ch->fighting == NULL )
    init_attack( ch, victim );  // init_attack does bounds checking on victim's validity

  return (void*) i;
}


void* code_attack_room( void** argument )
{
  char_data*        ch  = (char_data*) argument[0];
  int              dam  = (int)        argument[1];
  char*         string  = (char*)      argument[2];
//  int           ranged  = (int)        argument[4];
  char_data*       rch;

  if( string == NULL ) {
    code_bug( "Code_Attack_Room: NULL attack string." );
    return NULL;
  }

  // Sanity check, and require attacker to be standing up.
  if( ch == NULL || !ch->Is_Valid() || ch->position < POS_FIGHTING ) // was pos_sleeping
    return NULL;
  if( !is_set( &ch->status, STAT_PET ) ) {
    for( int i = *ch->array - 1; i >= 0; i-- ) {
      if( ch && ch->Is_Valid( ) && ( rch = character( ch->array->list[i] ) ) != NULL
        && rch != ch && rch->Is_Valid( ) && ( rch->pcdata != NULL || is_set( &rch->status, STAT_PET ) ) 
        && rch->Seen( ch ) ) {
        damage_physical( rch, ch, dam, string );
      }
    }
  }
  else {
    for( int i = *ch->array - 1; i >= 0; i-- ) {
      if( ch && ch->Is_Valid( ) && ( rch = character( ch->array->list[i] ) ) != NULL
        && rch != ch && rch->Is_Valid( ) && rch->species != NULL && !is_set( &rch->status, STAT_PET ) 
        && rch->Seen( ch ) ) {
        damage_physical( rch, ch, dam, string );
      }
    }
  }

  return NULL;
}


void* code_attack( void** argument )
{
  char_data*      ch  = (char_data*) argument[0];
  char_data*  victim  = (char_data*) argument[1];
  int            dam  = (int)        argument[2];
  char*       string  = (char*)      argument[3];
  int          range  = (int)        argument[4];
  bool        plural  = (int)        argument[5] != 0;
  int              i;

  if( string == NULL ) {
    code_bug( "Code_Attack: NULL attack string." );
    return NULL;
  }

  // Sanity check, and require attacker to be standing up.
  if( ch == NULL || !ch->Is_Valid() || victim == NULL || !victim->Is_Valid() || ch->in_room != victim->in_room || ch->position < POS_FIGHTING ) 
    return NULL;

  push( );

  i = attack( ch, victim, string, NULL, dam, ATT_PHYSICAL, ATT_PHYSICAL, range, plural );

  pop( );

  // Queue up a leap, if one isn't already queued.
  if( ch && ch->Is_Valid( ) && ch->fighting == NULL )
    init_attack( ch, victim );  // init_attack does bounds checking on victim's validity

  return (void*) i;
}


/*
 *   ELEMENTAL ATTACKS
 */

void* code_attack_element( void** argument )
{
  char_data*      ch  = (char_data*) argument[0];
  char_data*  victim  = (char_data*) argument[1];
  int            dam  = (int)        argument[2];
  int           type  = (int)        argument[3];
  char*       string  = (char*)      argument[4];
  int          range  = (int)        argument[5];
  int           plur  = (int)         argument[6];
  bool        plural;
  int              i;

  if( string == NULL ) {
    code_bug( "Attack_Element: Null string." );
    return NULL;
  }

  // Sanity check, and require attacker to be standing up.
  if( !victim || !victim->Is_Valid( ) ) {
  //  code_bug( "Damage: NULL victim  " );
    return NULL;
  }

  if( !ch || !ch->Is_Valid( ) ) {
    code_bug( "Attack Element: Ch does not exist or is sleeping" );
    return NULL;
  }

  if( ch->position < POS_FIGHTING )
    return NULL;

  if( plur == 0 )
    plural = false;
  else
    plural = true;
    
  push( );

  i = attack( ch, victim, string, NULL, dam, ATT_PHYSICAL, type, range, plural );

  pop( );

  // Queue up a leap, if one isn't already queued.
  if( ch && ch->Is_Valid( ) && ch->fighting == NULL )
    init_attack( ch, victim );  // init_attack does bounds checking on victim's validity

  return (void*) i;
}


void* element_attack( void** argument, int type )
{
  char_data*      ch  = (char_data*) argument[0];
  char_data*  victim  = (char_data*) argument[1];
  int            dam  = (int)        argument[2];
  char*       string  = (char*)      argument[3];
  bool        plural  = (int)        argument[4] != 0;
  int          range  = 0;
  int              i;

  if( string == NULL ) {
    code_bug( "Attack: Null string." );
    return NULL;
  }

  // Sanity check, and require attacker to be standing up.

  if( !victim || !victim->Is_Valid( ) ) {
    code_bug( "Element Attack: NULL victim." );
    return NULL;
  }

  if( !ch || !ch->Is_Valid( ) ) {
    code_bug( "Element Attack: NULL ch or ch isn't in a position to fight" );
    return NULL;
  }

  if( ch->position < POS_FIGHTING ) 
    return NULL;

  push( );

  i = attack( ch, victim, string, NULL, dam, ATT_PHYSICAL, type, range, plural );

  pop( );

  // Queue up a leap, if one isn't already queued.
  if( ch && ch->Is_Valid( ) && ch->fighting == NULL )
    init_attack( ch, victim );  // init_attack does bounds checking on victim's validity

  return (void*) i;
}

void* code_attack_acid  ( void** arg )
{
  code_bug( "Attack_Acid: Change code to use Attack_Element( Acid )" );
  return element_attack( arg, ATT_ACID );
}

void* code_attack_cold  ( void** arg )
{
  code_bug( "Attack_Cold: Change code to use Attack_Element( Cold )" );
  return element_attack( arg, ATT_COLD );
}

void* code_attack_shock ( void** arg )
{
  code_bug( "Attack_Shock: Change code to use Attack_Element( Shock )" );
  return element_attack( arg, ATT_SHOCK );
}

void* code_attack_fire  ( void** arg )
{
  code_bug( "Attack_Fire: Change code to use Attack_Element( Fire )" );
  return element_attack( arg, ATT_FIRE );
}

/*
 *   DAMAGE ROUTINES
 */ 

void* code_damage       ( void** argument )
{
  char_data*      ch  = (char_data*) argument[0];
  char_data*  victim  = (char_data*) argument[1];
  int            dam  = (int)        argument[2];
  int           type  = (int)        argument[3];
  char*       string  = (char*)      argument[4];
  char*        death  = (char*)      argument[5];
  int           plur  = (int)        argument[6];
  bool        plural;

  if( string == NULL ) {
    code_bug( "damage: Null string." );
    return NULL;
  }

  // Sanity check, require attacker to be NULL (acode) or valid, require victim to be valid

  if( !victim || !victim->Is_Valid( ) ) {
    //code_bug( "Damage: NULL victim  " );
    return NULL;
  }

  if( ch && !ch->Is_Valid( ) ) {
    code_bug( "Damage: Ch exists but is not valid" );
    return NULL;
  }

  if( ch && victim && ch->in_room != victim->in_room ) {
    code_bug( "Damage: Ch and Victim valid, but in different rooms?" );
    return NULL;
  }

  if( plur == 0 )
    plural = false;
  else
    plural = true;

  return (void*) damage_element( victim, ch, dam, string, type, death, true, true, plural );

  return NULL;
}

/*
 *   INFLICT ROUTNIES
 */

void* element_inflict( void** argument, int type )
{
  char_data*  victim  = (char_data*) argument[0];
  char_data*      ch  = (char_data*) argument[1];
  int            dam  = (int)        argument[2];
  char*       string  = (char*)      argument[3];

  if( string == NULL ) {
    code_bug( "Inflict: Null string." );
    return NULL;
  }

  // Sanity check, require attacker to be NULL (acode) or valid, require victim to be valid

  if( !victim || !victim->Is_Valid( ) ) {
    //code_bug( "Damage: NULL victim  " );
    return NULL;
  }

  if( ch && !ch->Is_Valid( ) ) {
    code_bug( "Ch exists but is not valid" );
    return NULL;
  }

  return (void*) inflict( victim, ch, dam, string );

  return NULL;
}


void* code_inflict       ( void** arg )
{
  return element_inflict(arg, ATT_PHYSICAL);
}

void* code_inflict_acid  ( void** arg )
{
  code_bug( "Inflict_Acid: Change code to use Inflict, or Damage( Acid )" );
  return element_inflict(arg, ATT_ACID);
}

void* code_inflict_cold  ( void** arg )
{
  code_bug( "Inflict_Cold: Change code to use Inflict, or Damage( Cold )" );
  return element_inflict(arg, ATT_COLD);
}

void* code_inflict_shock ( void** arg )
{
  code_bug( "Inflict_Shock: Change code to use Inflict, or Damage( Shock )" );
  return element_inflict(arg, ATT_SHOCK);
}

void* code_inflict_fire  ( void** arg )
{
  code_bug( "Inflict_Fire: Change code to use Inflict, or Damage( Fire )" );
  return element_inflict(arg, ATT_FIRE);
}


/*
 *   DAM_MESSAGE ROUTNIES
 */

void* dam_message_element( void** argument, int type )
{
  char_data*  ch  = (char_data*) argument[0];
  int        dam  = (int)        argument[1];
  char*   string  = (char*)      argument[2];
  bool    plural  = (int)        argument[3] != 0;

  if( ch == NULL || !ch->Is_Valid( ) )
    return NULL;

  switch (type) {
    case ATT_ACID:
      dam_message( ch, NULL, dam, string, lookup( acid_index, dam, plural ) );
      break;

    case ATT_COLD:
      dam_message( ch, NULL, dam, string, lookup( cold_index, dam, plural ) );
      break;

    case ATT_SHOCK:
      dam_message( ch, NULL, dam, string, lookup( electric_index, dam, plural ) );
      break;

    case ATT_FIRE:
      dam_message( ch, NULL, dam, string, lookup( fire_index, dam, plural ) );
      break;

    case ATT_MAGIC:
      dam_message( ch, NULL, dam, string, lookup( physical_index, dam, plural ) );
      break;

    case ATT_MIND:
      dam_message( ch, NULL, dam, string, lookup( physical_index, dam, plural ) );
      break;

    default:
      dam_message( ch, NULL, dam, string, lookup( physical_index, dam, plural ) );
      break;
  }

  return NULL;
}

void* code_dam_message       ( void** arg ) { return dam_message_element(arg, ATT_PHYSICAL); }
void* code_dam_message_acid  ( void** arg ) { return dam_message_element(arg, ATT_ACID); }
void* code_dam_message_cold  ( void** arg ) { return dam_message_element(arg, ATT_COLD); }
void* code_dam_message_shock ( void** arg ) { return dam_message_element(arg, ATT_SHOCK); }
void* code_dam_message_fire  ( void** arg ) { return dam_message_element(arg, ATT_FIRE); }

