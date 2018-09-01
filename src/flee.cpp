#include "system.h"


/*
 *   LOCAL FUNCTIONS
 */


obj_data*  has_recall      ( char_data* );

/*
 *   SET WIMPY FUNCTION
 */


const char* wimpy_word [] = { "Flee", "Blink", "Pray", "Recall" };


void do_wimpy( char_data* ch, char* argument )
{
  int*   value;
  int      num;

  if( is_mob( ch ) )
    return;

  value = unpack_int( ch->pcdata->wimpy );

  if( *argument == '\0' ) {
    send_title( ch, "Wimpy Values" );
    for( int i = 0; i < 4; i++ )
      send( ch, "%35s : %2d%% (%3d hp)\r\n", wimpy_word[i], value[i], value[i] * ch->max_hit / 100, ch->max_hit );

    return;
  }

  for( int i = 0; i < 4; i++ ) {
    if( matches( argument, wimpy_word[i] ) ) {
      if( ( num = atoi( argument ) ) < 0 || num > 100 ) {
        send( ch, "You can only set wimpy percentages from 0 to 100.\r\n" );
        return;
      }
      value[i] = num;
      ch->pcdata->wimpy = pack_int( value );
      send( ch, "Your wimpy setting for %c%s set to %d%% (%d hp).\r\n", tolower( *wimpy_word[i] ), &wimpy_word[i][1], num, num * ch->max_hit / 100 );
      return;
    }
  }

  send( ch, "Syntax: wimpy <field> <percent>\r\n" );
}


/*
 *   AUTO-FLEE HANDLER
 */


bool check_wimpy( char_data* ch, char_data* damage_source )
{
  // Make sure character is in a fight
  if( has_enemy( ch ) == NULL || damage_source == NULL ) // if( ch->position < POS_FIGHTING )
    return FALSE;

  /*
  char_data* a_char = opponent(ch);
  if( a_char == NULL )
    return FALSE;
  */

  if( ch->species != NULL ) { // mob may possibly flee if species is wimpy
    if( ch->hit < ch->max_hit / 4 && is_set( ch->species->act_flags, ACT_WIMPY )  
      && number_range( 0,5 ) == 0 || ( is_pet( ch ) &&  
      ch->leader != NULL && ch->leader->in_room != ch->in_room ) )
      return attempt_flee( ch );

    return FALSE;
  }

  // Player can't flee if berzerk, or wimpy not set
  if( !is_set( &ch->status, STAT_WIMPY ) ) // is_set( &ch->status, STAT_BERSERK ) || 
    return FALSE;

  // Get pointer to the 4 wimpy values:  flee, blink, pray, and recall
  // We check them in reverse order!!
  int* value = unpack_int( ch->pcdata->wimpy );
  int percent = ch->hit * 100 / ch->max_hit;

  cast_data* prepare;
  if( value[3] > 0 && percent <= value[3] && has_recall( ch ) && !is_set( &ch->status, STAT_BERSERK ) && !is_set( ch->affected_by, AFF_STARVING_MADNESS ) ) {
    // this should be redundent when the find_temple checks for continent
/*    if( ch->in_room->area->continent != OLD_CONTINENT && is_evil( ch ) )
      do_recite( ch, "recall tar'ippal" );
    else if( ch->in_room->area->continent != OLD_CONTINENT )
      do_recite( ch, "recall talidar" );
    else */
      do_recite( ch, "recall" ); 

    return TRUE;
  
  } else if( value[2] > 0 && percent <= value[2] ) {
    do_pray( ch, "" );
    return TRUE;
  
  } else if( value[1] > 0 && percent <= value[1] && ch->get_skill( SPELL_BLINK ) != UNLEARNT && ( prepare = has_prepared( ch, SPELL_BLINK-SPELL_FIRST ) ) != NULL && !is_set( &ch->status, STAT_BERSERK ) && !is_set( ch->affected_by, AFF_STARVING_MADNESS ) ) {
    do_cast( ch, "blink" );
    return TRUE;    
  
  } else if( value[0] > 0 && percent <= value[0] && !is_set( &ch->status, STAT_BERSERK ) && !is_set( ch->affected_by, AFF_STARVING_MADNESS ) ) {
    attempt_flee( ch );
    return TRUE;
  }

  return FALSE;    
}


obj_data* has_recall( char_data* ch )
{
  obj_data* obj;

  for( int i = 0; i < ch->contents; i++ ) {
    if( ( obj = object( ch->contents[i] ) ) != NULL
      && obj->pIndexData->item_type == ITEM_SCROLL
      && obj->pIndexData->value[0] == SPELL_RECALL - SPELL_FIRST
      && is_set( obj->extra_flags, OFLAG_IDENTIFIED ) )
      return obj;
  }

  return NULL;
}


/*
 *   FLEE ROUTINE
 */


void do_flee( char_data* ch, char* argument )
{
  room_data* room;
  exit_data* exit  = NULL;

  if (!ch) {
    roach("do_flee: NULL character");
    return;
  }

  if( has_enemy( ch ) == NULL ) {
    send( ch, "No one is attacking you.\r\n" );
    return;
  }

  if( !ch->array || ( room = Room( ch->array->where ) ) == NULL ) {
    send( ch, "You are in a room which is confusing.\r\n" );
    return;
  }

  if( *argument != '\0' ) {
    if( ( exit = (exit_data*) one_thing( EXIT_DATA, ch, argument, "flee", (thing_array*) &room->exits ) ) == NULL )
      return;
    if( !ch->Can_Move( exit ) ) {
      send( ch, "You can't flee %s.\r\n", dir_table[ exit->direction ].name );
      return;
    }
  }

  attempt_flee( ch, exit );
}


bool attempt_flee( char_data* ch, exit_data* exit )
{
  content_array*  was_in;
  content_array*  now_in;
  int                exp;
  char_data*     opponent = has_enemy( ch );
  char_data*         pet;

  if( is_set( ch->affected_by, AFF_STARVING_MADNESS ) ) {
    if( number_range( 5, 60 ) > ch->Wisdom( ) ) {
       send( ch, "Your raving hunger prevents you from fleeing.\r\n" );
       return FALSE;
    }
  }

  if( is_set( &ch->status, STAT_BERSERK ) ) {
    send( ch, "You can never flee while the battle lust rages within you!\r\n" );
    return FALSE;
  }
  
  if( is_entangled( ch, "flee from", true ) ) {
    send( ch, "You can't flee while entangled!\r\n" );
    return FALSE;
  }

/* 
  if( is_set( ch->affected_by, AFF_ENTANGLED ) 
    || is_set( ch->affected_by, AFF_BRIARTANGLE )
    || is_set( ch->affected_by, AFF_ENSNARE_TRAPPED ) 
    || is_set( ch->affected_by, AFF_ENSNARE ) ) {
    send( ch, "You can't flee while entangled!\r\n" );
    return FALSE;
  }
*/
  if( ch->position < POS_FIGHTING ) {
    send( ch, "To flee, you need to be on your feet.\r\n" );
    return FALSE;
  }

  if( exit == NULL && ( exit = random_movable_exit( ch ) ) == NULL )
    return FALSE;

  int flee = 0;
  if( opponent != NULL )
    if( opponent->position < POS_FIGHTING || is_set( opponent->affected_by, AFF_ENSNARE ) ) 
      flee = 1000;

  if( number_range( 0, 100 ) < 10+20*ch->get_burden()-flee ) {
    send( ch, "You attempt to flee, but fail to escape the battle!\r\n" );
    set_delay( ch, 32 );
    return TRUE;
  }

  was_in = ch->array;
  move_char( ch, exit->direction, TRUE );

  if( !ch->Is_Valid( ) || ch->hit <= 0 )
    return TRUE;

  if( ch->array == was_in ) {
    send( ch,"You flee but for some reason end up where you started!!\r\n" );
    return TRUE;
  }

  for( int i = 0; i < ch->followers.size; i++ ) {
    pet = ch->followers[i];
    if( is_pet( pet ) && pet->fighting != NULL && pet->array == was_in ) {
      was_in = pet->array;
      move_char( pet, exit->direction, TRUE );
      now_in = pet->array;
      pet->array = was_in;
      clear( pet->cmd_queue );
      clear_queue( pet );
      pet->array = now_in;
    }
  }
    
  now_in    = ch->array;
  ch->array = was_in;

//  send_seen( ch, "%s flees %s!\r\n", ch, dir_table[ exit->direction ].name );

  ch->array = now_in;

  player_data *p = player( ch );
  if( !p && ch->rider )
    p = player( ch->rider );

  if( p ) {
    exp = (p->shdata->level * p->hit) / 2;
    if( p->shdata->level == 1 )
      exp = min( p->exp, exp );
    float loss = exp*( 1-.1*ch->get_skill( SKILL_RETREAT )/MAX_SKILL_LEVEL );
    if( exp > 0 ) {
      exp = (int) loss;
      add_exp( p, -exp, "You lose %d exp for fleeing.\r\n" );
    }
  }

  ch->improve_skill( SKILL_RETREAT );
  set_delay( ch, 32 );
  clear( ch->cmd_queue );
  clear_queue( ch );
  
  return TRUE;
}

