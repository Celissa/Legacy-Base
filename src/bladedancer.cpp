#include "system.h"


bool wischeck       (char_data*, char_data* /*, int, int */);
bool check_social   ( char_data*, char*, char* );
bool check_progs    ( char_data*, char*, char* );
int dance [] = { SKILL_DRAGONFLY, SKILL_SPRING, SKILL_MISTY_DAWN, SKILL_DRAKE,
  SKILL_SUMMER, SKILL_FIREFLY, SKILL_WINTER, SKILL_STORMS, SKILL_STORMLORD,
  SKILL_STORMMASTER, SKILL_FURY, SKILL_BATTLEFIEND, SKILL_DESTRUCTION,
  SKILL_ARMAGEDDON, SKILL_H_STRIKES, SKILL_T_STRIKES, SKILL_THORNS, SKILL_SHARDS,
  SKILL_ELEMENTS, SKILL_MIRROR_IMAGE, SKILL_WHIRLING_SWORDS };
int what_dance      ( char_data* );

bool is_dancing( char_data* ch )
{
  if( !ch )
    return FALSE;

  if( ch->dance != 0 )
    return TRUE;
  else
    return FALSE;
}

bool is_singing( char_data* ch )
{
  if( !ch )
    return false;

  if( ch->cast == NULL )
    return false;

  return ( ch->cast->cast_type == UPDATE_SONG );
}

void end_dance( char_data* ch, bool message )
{
  if( !is_dancing( ch ) ) 
    return;

  ch->dance = 0;

  if( message ) {
    act( ch, "@GYou stop dancing.@n", ch );
    act_notchar( "@G$n stops $s dance, seeming a little out of breath.@n", ch );
  }
}


void dance_message( char_data* ch )
{
  int   skill = 0;
  char    buf [ MAX_STRING_LENGTH ];

  if( is_dancing( ch ) && number_range( 1, 20 ) == 1 ) {
    skill = what_dance( ch );
    if( skill >= 0 && skill < MAX_ENTRY_DANCE ) {
      sprintf( buf, "@GYou spin and flow, losing yourself in the steps of the %s.@n", skill_table[dance[skill]].name );
      act( ch, buf, ch );
      act_notchar( "@G$n flows from step to step, losing $mself in the dance.@n", ch );
    }
  }
}


int dance_move( char_data* ch )
{
  int regen = 0;
  int    i = 0;

  if( !ch || !ch->cast || !is_dancing( ch ) )
    return 0;

  i = what_dance( ch );

  if( i >= 0 && i < MAX_ENTRY_DANCE )
    regen = move_cost( ch, dance[i] );

  return regen;
}

int what_dance( char_data* ch )
{
  int i = 0;

  if( !is_dancing( ch ) )
    return -1;

  for( ; i < MAX_DANCE; i++ ) {
    if( is_set( &ch->dance, i ) )
      break;
  }

  return i;
}

void do_dance( char_data* ch, char* argument ) 
{
  if( is_mob( ch ) )
    return;

  if( ch->pcdata == NULL || ch->species != NULL )
    return;

  if( exact_match( argument, "end" ) || exact_match( argument, "stop" ) ) {
    end_dance( ch );
    return;
  }

  if( ch->pcdata->clss != CLSS_DANCER ) {
    send( ch, "You are not skilled enough in dancing to do a worthwhile performance.\r\n" );
    return;
  }

  if( is_dancing( ch ) ) {
    if( what_dance( ch ) == -1 )
      send( ch, "[BUG] You are dancing yet not?!?\r\n" );
    else
      send( ch, "You are already performing the %s.\r\n", skill_table[ dance_table[ what_dance( ch ) ].skill ].name );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "Which dance do you wish to perform?\r\n" );
    return;
  }

  if( MAX_ENTRY_DANCE == 0 ) {
    bug( "There are no dance entries." );
    send( ch, "The dance table has broken.  Please contact an immortal.\r\n" );
    return;
  }

  int dance = 0;
  char* arg = argument;

  for( ; dance < MAX_ENTRY_DANCE; dance++ ) {
    const char* cdance = dance_table[dance].dance;
      if( dance_table[dance].open && exact_match( arg, cdance ) )
        break;
  }

  if( ch->get_skill( dance_table[dance].skill ) == UNLEARNT ) {
    send( ch, "You are unskilled in the %s.\r\n", skill_table[dance_table[dance].skill].name );
    return;
  }

  if( dance == MAX_ENTRY_DANCE ) {
    send( ch, "You know of no such dance.\r\n", argument );
    return;
  }

  set_bit( &ch->dance, dance );
  if( *dance_table[dance].message_to != '\0' )
    act( ch, dance_table[dance].message_to, ch, NULL );
  if( *dance_table[dance].message_rto != '\0' )
    act_notchar( dance_table[dance].message_rto, ch, NULL );
/*
  if( fmatches( argument, "elements" ) ) {
    if( !ch->get_skill( SKILL_ELEMENTS ) ) {
      send( ch, "You are unskilled in the dance of the elements.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_ELEMENTS );
    act( ch, "@GYou dance, tapping into elemental power.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "dragonfly" ) ) {
    if( !ch->get_skill( SKILL_DRAGONFLY ) ) {
      send( ch, "You are unskilled in the dance of the dragonfly.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_DRAGONFLY );
    act( ch, "@GYou begin the rather primitive dance of the dragonfly.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "spring" ) ) {
    if( !ch->get_skill( SKILL_SPRING ) ) {
      send( ch, "You are unskilled in the dance of spring.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_SPRING );
    act( ch, "@GYou begin the joyful dance of spring.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "misty.dawn" ) ) {
    if( !ch->get_skill( SKILL_MISTY_DAWN ) ) {
      send( ch, "You are unskilled in the dance of the misty dawn.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_MISTY_DAWN );
    ch->improve_skill( SKILL_MISTY_DAWN );
    act( ch, "@GYour form is cloaked in mist as you begin your dance.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "drake" ) ) {
    if( !ch->get_skill( SKILL_DRAKE ) ) {
      send( ch, "You are unskilled in the dance of the drake.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_DRAKE );
    act( ch, "@GYou twist and weave, dancing the forms of the drake.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "fury" ) ) {
    if( !ch->get_skill( SKILL_FURY ) ) {
      send( ch, "You are unskilled in the dance of fury.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_FURY );
    act( ch, "@GYou dance, embuing your attacks with the power of fury.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "firefly" ) ) {
    if( !ch->get_skill( SKILL_FIREFLY ) ) {
      send( ch, "You are unskilled in the dance of the firefly.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_FIREFLY );
    act( ch, "@GYou begin dancing and your body appears to reflect the light.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "winter" ) ) {
    if(  !ch->get_skill( SKILL_WINTER ) ) {
      send( ch, "You are unskilled in the dance of winter.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_WINTER );
    act( ch, "@GYou spin in the steps of the dance of winter, basking in the cold.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "summer" ) ) {
    if( !ch->get_skill( SKILL_SUMMER ) ) {
      send( ch, "You are unskilled in the dance of summer.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_SUMMER );
    act( ch, "@GWarmth flows through your limbs as you start your dance.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "thorns" ) ) {
    if( !ch->get_skill( SKILL_THORNS ) ) {
      send( ch, "You are unskilled in the dance of thorns.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_THORNS );
    act( ch, "@GAir writhes across your skin as you begin to dance.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "shards" ) ) {
    if( !ch->get_skill( SKILL_SHARDS ) ) {
      send( ch, "You are unskilled in the dance of shards.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_SHARDS );
    act( ch, "@GThe air seems to burst into vicious shards as you start to dance.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "storms" ) ) {
    if( !ch->get_skill( SKILL_STORMS ) ) {
      send( ch, "You are unskilled in the dance of storms.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_STORMS );
    act( ch, "@GAs you dance, energy seems to flow through your arms.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "lord of storms" ) ) {
    if( !ch->get_skill( SKILL_STORMLORD ) ) {
      send( ch, "You are unskilled in the dance of the storm lord.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_STORMLORD );
    act( ch, "@GAs you dance, raw energy flows through your body.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "master of storms" ) ) {
    if( !ch->get_skill( SKILL_STORMMASTER ) ) {
      send( ch, "You are unskilled in the dance of the storm master.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_STORMMASTER );
    act( ch, "@GUsing the power of dance you reach out to channel raw elemental power.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "hundred.strikes" ) ) {
    if( !ch->get_skill( SKILL_H_STRIKES ) ) {
      send( ch, "You are unskilled in the dance of a hundered strikes.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_H_STRIKES );
    act( ch, "@GYou begin to dance, spinning with uncanny speed.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "thousand.strikes" ) ) {
    if( !ch->get_skill( SKILL_T_STRIKES ) ) {
      send( ch, "You are unskilled in the dance of a thousand strikes.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_T_STRIKES );
    act( ch, "@GYou begin to dance, whipping and twirling with increasing speed.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "battle.fiend" ) ) {
    if( !ch->get_skill( SKILL_BATTLEFIEND ) ) {
      send( ch, "You are unskilled in the dance of the battle fiend.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_BATTLEFIEND );
    act( ch, "@GThe air writhes across your skin as you begin to dance.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( exact_match( argument, "destruction" ) ) {
    if( !ch->get_skill( SKILL_DESTRUCTION ) ) {
      send( ch, "You are unskilled in the dance of destruction.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_DESTRUCTION );
    act( ch, "@GThe air writhes across your skin as you begin to dance.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "armageddon" ) ) {
    if( !ch->get_skill( SKILL_ARMAGEDDON ) ) {
      send( ch, "You are unskilled in the dance of armageddon.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_ARMAGEDDON );
    act( ch, "@GThe air writhes across your skin as you begin to dance.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }

  if( fmatches( argument, "mirror image" ) ) {
    if( !ch->get_skill( SKILL_MIRROR_IMAGE ) ) {
      send( ch, "You are unskilled in the dance of mirror images.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_MIRROR_IMAGE );
    act( ch, "@GThe air writhes across your skin as you begin to dance.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }
  
  if( fmatches( argument, "whirling swords" ) ) {
    if( !ch->get_skill( SKILL_WHIRLING_SWORDS ) ) {
      send( ch, "You are unskilled in the dance of whirling swords.\r\n" );
      return;
    }
    set_bit( &ch->dance, DANCE_WHIRLING_SWORDS );
    act( ch, "@GThe air writhes across your skin as you begin to dance.@n\r\n", ch );
    act_notchar( "@G$n has begun to dance.", ch );
  }
*/
  if( is_dancing( ch ) ) {
    set_delay( ch, 32 );
    return;
  }

  // if we get down to here, nuh
  send( ch, "You know of no such dance.\r\n" );  
}

void dance_damage( char_data* ch )
{
  char to_char[ TWO_LINES ] = "@mYou spin and attack in a deadly blur, faster and faster!@n\r\n";
  char to_others[ 3*MAX_STRING_LENGTH ];
  char tmp[ 3*MAX_STRING_LENGTH ];

  char_data* victim = ch->fighting;
  char_data* rch;
  obj_data* wield = get_weapon( ch, WEAR_HELD_R );

  int skill = 10*ch->get_skill( SKILL_H_STRIKES )/( 4*MAX_SKILL_LEVEL )
    + 5*ch->get_skill( SKILL_T_STRIKES )/MAX_SKILL_LEVEL;
  
  skill = number_range( 0, skill );

  if( wield == NULL || skill == UNLEARNT || victim == NULL )
    return;

  if( ch->pcdata != NULL && is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) 
    return;

  if( number_range( 1, 3 ) == 1 ) {
    convert_to_ansi( ch, to_char, tmp, sizeof( tmp ) );
    send( ch, tmp );
    ch->improve_skill( SKILL_H_STRIKES );
    ch->improve_skill( SKILL_T_STRIKES );
    if( ch->array != NULL ) {
      for( int i = 0; i < ch->array->size; i++ ) {
        rch = character( ch->array->list[i] );
        if( rch != NULL && rch != ch && ch->Seen( rch ) ) {
          sprintf( to_others, "@m%s spins and attacks in a deadly blur, faster and faster!@n\r\n", ch->Seen_Name( rch ) );
          convert_to_ansi( rch, to_others, tmp, sizeof( tmp ) );
          send( rch, tmp );
        }
      }
    }
    for( int i = skill; i > 0 ; i-- ) 
      attack( ch, victim, attack_word( wield ), wield, wield->value[2]/2, 0 );
    send( ch, "You slow down your spins and stop your deadly blur.\r\n" );
  }
  return;
}


//  Dancer song funcs...   meow!


void do_sing( char_data* ch, char* argument )
{
  if( ch->species != NULL )
    return;
/*  imms could sing spells and crash mud, so better check put in after the spell
    is determined
  if( ch->pcdata->clss != CLSS_DANCER ) {
    send( ch, "You are untrained in the musical arts.\r\n" );
    return;
  }
*/
  if( is_set( ch->affected_by, AFF_SILENCE ) ) {
    send( ch, "You are silenced and unable to sing!\r\n" );
    return;
  }

  if( ch->position == POS_SLEEPING ) {
    send( ch, "Singing in your sleep would be an impressive feat.\r\n" );
    return;
  }

  if( terrain_table[ ch->in_room->sector_type ].underwater ) {
    send( ch, "You cannot sing underwater.\r\n" );
    return;
  }

  if( matches( argument, "end" ) || matches( argument, "stop" ) ) {
    if( ch->cast == NULL ) {
      send( ch, "You are not currently singing.\r\n" );
      return;
    }
    end_song( ch );
    return;
  }

  if( *argument == '\0' ) {
    if( ch->cast == NULL ) {
      act( ch, "@CYou are not currently singing.@n\r\n", ch );
      return;
    }
    act( ch, "@CYou are currently singing the $t.@n\r\n", ch, NULL, spell_table[ ch->cast->spell ].name );
    return;
  }

  if( ch->cast != NULL ) {
    send( ch, "You can only sing one song at a time.\r\n" );
    return;
  }

  int spell = find_spell( ch, argument, strlen( argument ) );
  if( spell == -1 )
    return;

  if( !spell_table[spell].song ) {
    send( ch, "That is not a song.\r\n" );
    return;
  }

  cast_data* cast = new cast_data;
  cast->spell     = spell;
  cast->mana      = mana_cost( ch, spell );
  cast->cast_type = UPDATE_SONG;

  int skill = ( ch->species != NULL ? 8 : 10*ch->get_skill( SPELL_FIRST + spell )/MAX_SKILL_LEVEL );

  act( ch, "@CYou begin singing the $t.@n\r\n", ch, NULL, spell_table[ spell ].name );
  act_notchar( "@C$n has begun to sing.@n\r\n", ch );

  ( *spell_table[ spell ].function )( ch, ch, NULL, skill, 0 );

  ch->cast = cast;
  
  set_delay( ch, 32 );
  return;
}


void end_song( char_data* ch, bool message )
{
  if( ch->species != NULL || !ch || !ch->pcdata || ch->pcdata->clss != CLSS_DANCER || ch->cast == NULL )
    return;

  char_data* gch;

  ch->cast = NULL;
  check_song( ch );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( gch = character( ch->array->list[i] ) ) != NULL )
      check_song( gch );
  }  

  if( message )
    send( ch, "You stop singing.\r\n" );
}


// need to check song on the room just after you leave it (via summon, acode, etc) so that the leech expires on players left behind

void check_song( char_data* ch )
{
  if( !ch )
    return;

  int song_affects [] = { AFF_TOAD, AFF_BROOK, AFF_RIVER, AFF_HARVEST, AFF_BUCK, AFF_WALRUS, AFF_WAR, AFF_SUMMER, 
    AFF_WINTER, AFF_WHEAT, AFF_BATTLE, AFF_OX, AFF_SERAPHIM, AFF_CELESTIAL, AFF_TIRELESS, AFF_SWIFT, AFF_CHEETAH, 
    AFF_CARNAGE, AFF_ETERNAL_BATTLE, AFF_BLADE_HASTE, AFF_SERENITY, AFF_SHADOW, AFF_AUTUMN, AFF_SPRING, AFF_SUMMER_EQUINOX, 
    AFF_WINTER_EQUINOX, AFF_SEASONS_MASTERY, AFF_LULLABY, AFF_ENCHANTED_SLEEP, AFF_DRAGONS_STANCE,
    AFF_BEARS_STANCE, AFF_CATS_STANCE, AFF_SWIRLING_BLADES, AFF_IRON_RESOLVE, AFF_WHIRLING_DEATH, AFF_SPRING_EQUINOX,
    AFF_HARVEST_EQUINOX, -1 };

  char_data* leecher;

  // this loop strips affects no longer leeched to you
  for( int i = ch->affected.size - 1; i >= 0; i-- ) {
    for( int j = 0; song_affects[ j ] >= 0; j++ ) {
      if( ch->affected[ i ]->type == song_affects[ j ] ) {
        affect_data* affect = ch->affected[ i ];
        leecher = affect->leech;
        if( leecher == NULL || ch->in_room != leecher->in_room || !is_same_group( ch, leecher ) || leecher->cast == NULL ) {
          remove_affect( ch, affect );
        }
      }
    }
  }

  // this loop sees if you can get any new affects, but it is a bit of a hack and as such
  // may cause crashes... we will have to test thorougly to make sure it is OK
  // it loops through the room, checking to see if a char is in the same group as you, if
  // there is, then it checks to see if they are a blade dancer, if they are, then they
  // recast the spell on the room
  if( !ch->array )
    return;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( leecher = character( ch->array->list[i] ) ) != NULL && is_same_group( leecher, ch ) ) {
      if( leecher->pcdata && leecher->pcdata->clss == CLSS_DANCER && leecher->cast != NULL ) {
        int spell = leecher->cast->spell;
        ( *spell_table[ spell ].function )( leecher, NULL, NULL, leecher->pcdata->skill[ SPELL_FIRST + spell ], 0 );
      }
    }
  }
}


void song_message( char_data* ch )
{
  if( !ch || !ch->cast )
    return;

  if( ch->cast->cast_type != UPDATE_SONG )
    return;

  if( number_range( 1, 20 ) == 1 ) {
    cast_data* cast = ch->cast;
    int spell = cast->spell;
    int action = spell_table[spell].action[0];
    act( ch, spell_act_table[action].self_self, ch );
    act_notchar( spell_act_table[action].others_self, ch );
  }

  if( number_range( 1, 5 ) == 1 ) {
    cast_data* cast = ch->cast;
    int spell = cast->spell;
    ch->improve_skill( spell+SPELL_FIRST );
  }
}


int song_leech( char_data* ch )
{
  int regen = 0;

  if( !ch || !ch->cast )
    return 0;

  int song = ch->cast->spell;
  
  regen = song_cost( ch, song );

  return regen;
}


void sonic_damage( char_data* ch )
{
  int damage = 0, mana = 0, level = 0;
  char_data* victim = NULL;
  char_data* leader = ch->leader != NULL ? ch->leader : ch;
  int cast = 0;

  if( ch->cast == NULL )
    return;

  if( ch->cast->cast_type != UPDATE_SONG )
    return;

  cast = ch->cast->spell;

  level  = ch->get_skill( cast+SPELL_FIRST );
  damage = spell_damage( cast+SPELL_FIRST, level, 0 );
  mana   = song_cost( ch, cast );

  switch ( cast+SPELL_FIRST ) {
  case SPELL_BARK_WOLF:
    if( ch->mana <= mana )
      return;
    damage += ch->Wisdom( )/4;
    act( ch, "@bYou feel a feral energy channel through your form.@n\r\n", ch );
    act_notchar( "@b$n's eyes seem to glow with a feral light.@n\r\n", ch );
    for( int i = *ch->array-1; i >=0; i-- ) {
      if( ( victim = character( ch->array->list[i] ) ) != NULL && can_kill( ch, victim, false ) && ( victim->fighting == ch || victim->fighting == leader ) ) {
        damage_element( victim, ch, damage , "loud barking", ATT_PHYSICAL );
      }
    }
    ch->mana -= mana;
    ch->improve_skill( SPELL_BARK_WOLF );
    break;

  case SPELL_MIRE:
    if( ch->mana <= mana )
      return;
    damage += ch->Wisdom( )/3;
    act( ch, "@CYou channel energy at your foes, wrapping them in strands of air.@n\r\n", ch );
    act_notchar( "@C$n channels energy at $s foes, wrapping them in strands of air.@n\r\n", ch );
    for( int i = *ch->array-1; i >=0; i-- ) {
      if( ( victim = character( ch->array->list[i] ) ) != NULL && can_kill( ch, victim, false ) && ( victim->fighting == ch || victim->fighting == leader ) ) {
        damage_element( victim, ch, damage, "squeezing air", ATT_PHYSICAL );
        if( opposed_roll( ch, victim, SPELL_MIRE, WISDOM, STRENGTH ) ) {
          spell_affect( ch, victim, 10, 10, SPELL_MIRE, AFF_SLOW );
        }
      }
    }
    ch->mana -= mana;
    ch->improve_skill( SPELL_MIRE );
    break;

  case SPELL_HOWL_WOLF:
    if( ch->mana <= mana )
      return;
    damage += ch->Wisdom( )/3;
    for( int i = *ch->array-1; i >=0; i-- ) {
      if( ( victim = character( ch->array->list[i] ) ) != NULL && can_kill( ch, victim, false ) && ( victim->fighting == ch || victim->fighting == leader ) ) {
        damage_element( victim, ch, damage, "howl of the wolf", ATT_PHYSICAL );
      }
    }
    ch->mana -= mana;
    ch->improve_skill( SPELL_HOWL_WOLF );
    break;

  case SPELL_QUICKSAND:
    if( ch->mana <= mana )
      return;
    damage += ch->Wisdom( )/2;
    act( ch, "@CYou channel energy at your foes, wrapping them in strands of air.@n\r\n", ch );
    act_notchar( "@C$n channels energy at $s foes, wrapping them in strands of air.@n\r\n", ch );
    for( int i = *ch->array-1; i >=0; i-- ) {
      if( ( victim = character( ch->array->list[i] ) ) != NULL && can_kill( ch, victim, false ) && ( victim->fighting == ch || victim->fighting == leader ) ) {
        damage_element( victim, ch, damage, "squeezing air", ATT_PHYSICAL );
        if( opposed_roll( ch, victim, SPELL_QUICKSAND, WISDOM, STRENGTH ) ) {
          spell_affect( ch, victim, 10, 10, SPELL_QUICKSAND, AFF_SLOW );
        }
      }
    }
    ch->mana -= mana;
    ch->improve_skill( SPELL_QUICKSAND );
    break;

  case SPELL_BALE_HOWL:
    if( ch->mana <= mana )
      return;
    damage += ch->Wisdom( )/2;
    for( int i = *ch->array-1; i >=0; i-- ) {
      if( ( victim = character( ch->array->list[i] ) ) != NULL && can_kill( ch, victim, false ) && ( victim->fighting == ch || victim->fighting == leader ) ) {
        damage_element( victim, ch, damage, "baleful howl", ATT_PHYSICAL );
      }
    }
    ch->mana -= mana;
    ch->improve_skill( SPELL_BALE_HOWL );
    break;

  case SPELL_HARPY_SCREECH:
    if( ch->mana <= mana )
      return;
    damage += ch->Wisdom( );
    for( int i = *ch->array-1; i >=0; i-- ) {
      if( ( victim = character( ch->array->list[i] ) ) != NULL && can_kill( ch, victim, false ) && ( victim->fighting == ch || victim->fighting == leader ) ) {
        damage_element( victim, ch, damage, "loud barking", ATT_PHYSICAL );
      }
    }
    ch->mana -= mana;
    ch->improve_skill( SPELL_HARPY_SCREECH );
    break;

  case SPELL_DEATHS_CRY:
    if( ch->mana <= mana )
      return;
    damage += ch->Wisdom( )/3;
    for( int i = *ch->array-1; i >= 0; i-- ) {
      if( ( victim = character( ch->array->list[i] ) ) != NULL && can_kill( ch, victim, false ) && ( victim->fighting == ch || victim->fighting == leader ) ) {
        damage_element( victim, ch, damage, "siren's wail", ATT_UNHOLY );
      }
    }
    ch->mana -= mana;
    ch->improve_skill( SPELL_DEATHS_CRY );
    break;
  
  case SPELL_MOURNING_WAIL:
    if( ch->mana <= mana )
      return;
    damage += ch->Wisdom( )/2;
    for( int i = *ch->array-1; i >= 0; i-- ) {
      if( ( victim = character( ch->array->list[i] ) ) != NULL && can_kill( ch, victim, false ) && ( victim->fighting == ch || victim->fighting == leader ) ) {
        damage_element( victim, ch, damage, "banshee's wail", ATT_UNHOLY );
      }
    }
    ch->mana -= mana;
    ch->improve_skill( SPELL_MOURNING_WAIL );
    break;    if( ch->mana <= mana )
      return;
    damage += ch->Wisdom( )/2;

  case SPELL_BANSHEES_WAIL:
    if( ch->mana <= mana )
      return;
    damage += ch->Wisdom( );
    for( int i = *ch->array-1; i >= 0; i-- ) {
      if( ( victim = character( ch->array->list[i] ) ) != NULL && can_kill( ch, victim, false ) && ( victim->fighting == ch || victim->fighting == leader ) ) {
        damage_element( victim, ch, damage, "banshee's scream", ATT_UNHOLY );
      }
    }
    ch->mana -= mana;
    ch->improve_skill( SPELL_DEATHS_CRY );
    break;
    
  }

  return;
}


// SONGS

// Utility Songs
bool spell_serenade( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SERENADE ) )
    return TRUE;

  char_data* victim;

  for( int i = *ch->array-1; i >= 0; i-- ) {
    if( ( victim = character( ch->array->list[i] ) ) != NULL && ( victim->fighting == ch->leader || victim->fighting == ch ) && can_kill( ch, victim, false ) ) {
      spell_calm( victim, ch, NULL, level, 0 );
    }
  }
  return TRUE;
}


bool spell_toad( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_TOAD ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_TOAD, AFF_TOAD );
    
  return TRUE;
}

bool spell_brook( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_BROOK ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_BROOK, AFF_BROOK );
    
  return TRUE;
}

bool spell_river( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_RIVER ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_RIVER, AFF_RIVER );
    
  return TRUE;
}

bool spell_wheat( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_WHEAT ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_WHEAT, AFF_WHEAT );
    
  return TRUE;
}

bool spell_harvest( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_HARVEST ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_HARVEST, AFF_HARVEST );
    
  return TRUE;
}

bool spell_renewal( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_RENEWAL ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) ) {
      spell_affect( ch, gch, level, duration, SPELL_RENEWAL, AFF_RIVER );
      spell_affect( ch, gch, level, duration, SPELL_RENEWAL, AFF_HARVEST );
    }
  }
    
  return TRUE;
}


// Buffing songs - regen

bool spell_lullaby( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_LULLABY ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_LULLABY, AFF_LULLABY );
    
  return TRUE;
}

bool spell_dragons_stance( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_DRAGON_STANCE ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_DRAGON_STANCE, AFF_DRAGONS_STANCE );
    
  return TRUE;
}

bool spell_bears_stance( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_BEARS_STANCE ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_BEARS_STANCE, AFF_BEARS_STANCE );
    
  return TRUE;
}

bool spell_cats_stance( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_CATS_STANCE ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_CATS_STANCE, AFF_CATS_STANCE );
    
  return TRUE;
}

bool spell_swirling_blades( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SWIRLING_BLADES ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_SWIRLING_BLADES, AFF_SWIRLING_BLADES );
    
  return TRUE;
}

bool spell_iron_resolve( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_IRON_RESOLVE ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_IRON_RESOLVE, AFF_IRON_RESOLVE );
    
  return TRUE;
}

bool spell_whirling_death( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_WHIRLING_DEATH ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_WHIRLING_DEATH, AFF_WHIRLING_DEATH );
    
  return TRUE;
}

bool spell_enchanted_sleep( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_ENCHANTED_SLEEP ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_ENCHANTED_SLEEP, AFF_ENCHANTED_SLEEP );
    
  return TRUE;
}

bool spell_buck( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_BUCK ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_BUCK, AFF_BUCK );
    
  return TRUE;
}

bool spell_ox( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_OX ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_OX, AFF_OX );
    
  return TRUE;
}

bool spell_walrus( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_WALRUS ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_WALRUS, AFF_WALRUS );
    
  return TRUE;
}

bool spell_serenity( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SERENITY ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_SERENITY, AFF_SERENITY );
    
  return TRUE;
}

bool spell_seraphim( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SERAPHIM ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_SERAPHIM, AFF_SERAPHIM );
    
  return TRUE;
}

bool spell_celestial( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SERENITY ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_CELESTIAL, AFF_CELESTIAL );
    
  return TRUE;
}

bool spell_tireless( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_TIRELESS ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_TIRELESS, AFF_TIRELESS );
    
  return TRUE;
}

bool spell_swift( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SWIFT ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_SWIFT, AFF_SWIFT );
    
  return TRUE;
}

bool spell_cheetah( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_CHEETAH ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_CHEETAH, AFF_CHEETAH );
    
  return TRUE;
}

bool spell_shadow( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SHADOW ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_SHADOW, AFF_SHADOW );
    
  return TRUE;
}


// Buffing songs
// Elemental resist


bool spell_summer( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SUMMER ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_SUMMER, AFF_SUMMER );
    
  return TRUE;
}

bool spell_winter( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_WINTER ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_WINTER, AFF_WINTER );
    
  return TRUE;
}

bool spell_autumn( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_AUTUMN ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_AUTUMN, AFF_AUTUMN );
    
  return TRUE;
}

bool spell_spring( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SPRING ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_SPRING, AFF_SPRING );
    
  return TRUE;
}

bool spell_summer_equinox( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SUMMER_EQUINOX ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_SUMMER_EQUINOX, AFF_SUMMER_EQUINOX );
    
  return TRUE;
}

bool spell_spring_equinox( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SPRING_EQUINOX ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_SPRING_EQUINOX, AFF_SPRING_EQUINOX );
    
  return TRUE;
}

bool spell_harvest_equinox( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_HARVEST_EQUINOX ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_HARVEST_EQUINOX, AFF_HARVEST_EQUINOX );
    
  return TRUE;
}

bool spell_winter_equinox( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_WINTER_EQUINOX ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_WINTER_EQUINOX, AFF_WINTER_EQUINOX );
    
  return TRUE;
}

bool spell_seasons_mastery( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_SEASONS_MASTERY ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_SEASONS_MASTERY, AFF_SEASONS_MASTERY );
    
  return TRUE;
}


// Buffing songs
// Offensive


bool spell_war( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_WAR ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_WAR, AFF_WAR );
    
  return TRUE;
}

bool spell_battle( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_BATTLE ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_BATTLE, AFF_BATTLE );
    
  return TRUE;
}

bool spell_carnage( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_CARNAGE ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_CARNAGE, AFF_CARNAGE );
    
  return TRUE;
}


bool spell_eternal_battle( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_ETERNAL_BATTLE ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_ETERNAL_BATTLE, AFF_ETERNAL_BATTLE );
    
  return TRUE;
}

bool spell_blurring( char_data* ch, char_data*, void*, int level, int duration )
{
  if( null_caster( ch, SPELL_BLURING ) )
    return TRUE;

  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = character( ch->array->list[i] ) ) != NULL && is_same_group( gch, ch ) )
      spell_affect( ch, gch, level, duration, SPELL_BLURING, AFF_BLADE_HASTE );
    
  return TRUE;
}


// Offensive attack songs


bool spell_bark_wolf( char_data* ch, char_data*, void*, int level, int duration )
{
  return TRUE;
}

bool spell_howl_wolf( char_data* ch, char_data*, void*, int level, int duration )
{
  return TRUE;
}

bool spell_bale_howl( char_data* ch, char_data*, void*, int level, int duration )
{
  return TRUE;
}

bool spell_harpy_screech( char_data* ch, char_data*, void*, int level, int duration )
{
  return TRUE;
}

bool spell_mire( char_data* ch, char_data*, void*, int level, int duration )
{
  return TRUE;
}

bool spell_quicksand( char_data* ch, char_data*, void*, int level, int duration )
{
  return TRUE;
}

bool spell_wail_banshee( char_data* ch, char_data*, void*, int level, int duration )
{
  return TRUE;
}

bool spell_wail_mourning( char_data* ch, char_data*, void*, int level, int duration )
{
  return TRUE;
}

bool spell_sirens_cry( char_data* ch, char_data*, void*, int level, int duration )
{
  return TRUE;
}

// - Non dance/song blade skills


void evasion( char_data* ch, char_data* victim, int& damage )
{
  int roll;

  if( victim->species != NULL  )
    return;

  if( victim->get_skill( SKILL_EVASION ) == UNLEARNT )
    return;

  roll = number_range( 1, 10*MAX_SKILL_LEVEL )+victim->get_skill( SKILL_EVASION )
    + victim->Dexterity( ) + victim->Wisdom( );

  if( roll > 120 ) {
    spam_char( victim, "You turn with the blow, reducing its impact.\r\n", ch );
    spam_char( ch, "%s turns with your blow, reducing its impact.\r\n", victim );
    spam_room( "%s turns with %s's attack, reducing its impact.\r\n", victim, ch );
    damage = 8*damage/10;
    victim->improve_skill( SKILL_EVASION );
  }

  return;
}

void mind_body( char_data* victim, int& damage )
{
  if( victim->species != NULL || victim->pcdata->clss != CLSS_DANCER )
    return;

  float percentage = damage*( (victim->Wisdom( )/30)*10*victim->get_skill( SKILL_MIND_BODY )/MAX_SKILL_LEVEL+
    10*victim->get_skill( SKILL_IMPROVED_MIND_BODY )/MAX_SKILL_LEVEL+
    10*victim->get_skill( SKILL_GREATER_MIND_BODY )/MAX_SKILL_LEVEL )/100;

  if( number_range( 0, 2 ) == 0 )
    victim->improve_skill( SKILL_MIND_BODY );
  else if( number_range( 0, 1 ) == 0 )
    victim->improve_skill( SKILL_IMPROVED_MIND_BODY );
  else
    victim->improve_skill( SKILL_GREATER_MIND_BODY );

  damage -= (int) percentage;
  return;
}

bool wischeck( char_data* ch, char_data* victim /*, int roll, int roll2 */)
{
  if( !ch || !victim || !victim->Is_Valid( ) ||!ch->Is_Valid( ) )
    return FALSE;

  if( ( ch->Wisdom( ) + number_range( 1, 20 ) ) >
    ( victim->Wisdom( ) + number_range( 1, 20 ) ) )
    return TRUE;

  return FALSE;
}

void dance_destruction( char_data* ch )
{
  obj_data* weapon    = NULL;
  obj_data* secondary = NULL;
     
  char_data* rch;
  bool found = false;
  int damage = 0;

  if( !ch || !is_dancing( ch ) || !is_set( &ch->dance, DANCE_DESTRUCTION ) )
    return;

  if( ch->pcdata != NULL && is_set( ch->pcdata->pfile->flags, PLR_PARRY ) )
    return;

  weapon    = get_weapon( ch, WEAR_HELD_R );
  secondary = get_weapon( ch, WEAR_HELD_L );

  if( weapon == NULL && secondary == NULL )
    return;

  for( int i = *ch->array-1; i >=0; i-- ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL && ( rch->fighting == ch || ch->fighting == rch ) ) {
      if( !found ) {
        found = true;
        if( weapon != NULL && secondary != NULL ) {
          send( ch, "You drive your weapons into the earth, and it resonates with the impact.\r\n" );
          send_seen( ch, "%s drives %s weapons into the earth, and a tremendous shaking follows.\r\n", ch, ch->His_Her( ) );
        }
        else {
          send( ch, "You drive your weapon into the earth, and it resonates with the impact.\r\n" );
          send_seen( ch, "%s drives %s weapon into the earth, and a tremendous shaking follows.\r\n", ch, ch->His_Her( ) );
        }
      }

      if( weapon != NULL ) 
        damage += roll_dice( weapon->value[1], weapon->value[2] )+20*ch->get_skill( SKILL_ARMAGEDDON )/MAX_SKILL_LEVEL;;
      if( secondary != NULL )
        damage += roll_dice( secondary->value[1], secondary->value[2] )+20*ch->get_skill( SKILL_ARMAGEDDON )/MAX_SKILL_LEVEL;;
      
      damage_element( rch, ch, damage, "dance of destruction", ATT_PHYSICAL );
    }
  }
}

void dance_armageddon( char_data* ch )
{
  obj_data* weapon    = get_weapon( ch, WEAR_HELD_R );
  obj_data* secondary = get_weapon( ch, WEAR_HELD_L );
  char_data* rch;
  bool found = false;
  int damage = 0;

  if( !ch || !is_dancing( ch ) || !is_set( &ch->dance, DANCE_ARMAGEDDON ) )
    return;

  if( ch->pcdata != NULL && is_set( ch->pcdata->pfile->flags, PLR_PARRY ) )
    return;

  if( weapon == NULL && secondary == NULL )
    return;

  for( int i = *ch->array-1; i >=0; i-- ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL && ( rch->fighting == ch || ch->fighting == rch ) ) {
      if( !found ) {
        found = true;
        if( weapon != NULL && secondary != NULL ) {
          send( ch, "You drive your weapons into the earth, looks like armageddon.\r\n" );
          send_seen( ch, "%s drives %s weapons into the earth, looks like an armageddon.\r\n", ch, ch->His_Her( ) );
        }
        else {
          send( ch, "You drive your weapon into the earth, looks like armageddon.\r\n" );
          send_seen( ch, "%s drives %s weapon into the earth, looks like an armageddon.\r\n", ch, ch->His_Her( ) );
        }
      }

      if( weapon != NULL ) 
        damage += roll_dice( weapon->value[1], weapon->value[2] )+30*ch->get_skill( SKILL_ARMAGEDDON )/MAX_SKILL_LEVEL;;
      if( secondary != NULL )
        damage += roll_dice( secondary->value[1], secondary->value[2] )+30*ch->get_skill( SKILL_ARMAGEDDON )/MAX_SKILL_LEVEL;;
    
      damage_element( rch, ch, damage, "dance of armageddon", number_range( ATT_PHYSICAL, ATT_HOLY ) );
    }
  }
}

void dance_swirling( char_data* ch )
{
  obj_data* weapon    = get_weapon( ch, WEAR_HELD_R );
  obj_data* secondary = get_weapon( ch, WEAR_HELD_L );
  char_data* rch;
  bool found = false;
  int damage = 0;

  if( !ch || !is_dancing( ch ) || !is_set( &ch->dance, DANCE_WHIRLING_SWORDS ) )
    return;

  if( weapon == NULL && secondary == NULL )
    return;

  for( int i = *ch->array-1; i >=0; i-- ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL && ( rch->fighting == ch || ch->fighting == rch ) ) {
      if( !found ) {
        found = true;
        send( ch, "You swirl around, striking at all your foes.\r\n" );
        send_seen( ch, "%s swirls around striking at all %s foes.\r\n", ch, ch->His_Her( ) );
        }

      if( weapon != NULL ) 
        damage += roll_dice( weapon->value[1], weapon->value[2] )+10*ch->get_skill( SKILL_WHIRLING_SWORDS )/MAX_SKILL_LEVEL;;
      if( secondary != NULL )
        damage += roll_dice( secondary->value[1], secondary->value[2] )+10*ch->get_skill( SKILL_WHIRLING_SWORDS )/MAX_SKILL_LEVEL;;
      
      damage_element( rch, ch, damage, "dance of swirling weapons", ATT_PHYSICAL );
    }
  }
}



  
