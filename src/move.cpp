#include "system.h"

extern bool can_hide( char_data*, bool );     // defined in thief.cc

/*
 *   CONSTANTS
 */

const char* leaving_verb [] = { "slithers", "sneaks", "leaves", "flies",
  "swims", "leaves", "wades" }; 

const char* leaving_action [] = { "slither", "sneak", "leave", "fly",
  "swim", "leave", "wade" };

const char* arriving_verb [] = { "slithers in", "sneaks in", "arrives",
  "flies in", "swims in", "arrives", "wades in" };


direction_type dir_table [] =
{ 
  { "north", 2, "the south",  "to the north"  },
  { "east",  3, "the west",   "to the east"   },
  { "south", 0, "the north",  "to the south"  },
  { "west",  1, "the east",   "to the west"   },
  { "up",    5, "below",      "above you"     },
  { "down",  4, "above",      "below you"     },
  { "extra", 6, "??",         "??"            }
};


char_data* leader = NULL;


/*
 *   LOCAL FUNCTIONS
 */

bool        non_water_type    ( room_data* );
bool        passes_drunk      ( char_data* );
bool        trigger_entering  ( char_data*, room_data*, int );
bool        trigger_leaving   ( char_data*, room_data*, int, action_data*& );
bool        can_enter         ( char_data*, room_data*, bool );
bool        can_leave         ( char_data*, int, bool );
bool        handle_terrain    ( char_data*, room_data*, room_data*,
                                int&, int& );
bool        is_exhausted      ( char_data*, int&, int );
int         get_motion        ( char_data* );
int         find_door         ( char_data*, char* );
exit_data*  valid_exit        ( char_data*, int );
void        act_leader        ( const char*, char_data*, char_data* = NULL );
void        add_delays        ( char_data*, int& );

#define rd  room_data
#define cd  char_data

void   arrival_message   ( cd*, rd*, exit_data*, int, action_data* );
void   leaving_message   ( cd*, rd*, exit_data*, int, action_data* );
void   leaving_self      ( cd*, exit_data*, int, action_data* );
void   leaving_other     ( cd*, cd*, rd*, exit_data*, int, action_data* );

#undef rd
#undef cd


/*
 *   MOVEMENT ABILITIES
 */


bool char_data :: can_float( )
{
  if( is_set( affected_by, AFF_FLOAT ) || is_set( affected_by, AFF_WATER_WALK ) )
    return TRUE;

  if( shifted != NULL && ( is_set( shifted->affected_by, AFF_FLOAT ) || is_set( shifted->affected_by, AFF_WATER_WALK ) ) )
    return TRUE;

  return FALSE;
}


bool char_data :: can_swim( )
{
  if( can_breath_underwater( ) )
    return TRUE;

  if( species != NULL )
    return is_set( species->act_flags, ACT_CAN_SWIM );

  if( shifted != NULL )
    return is_set( shifted->act_flags, ACT_CAN_SWIM );

  return( get_skill( SKILL_SWIMMING ) != UNLEARNT );
} 


bool char_data :: can_fly( )
{
  if( species == NULL && ( is_set( affected_by, AFF_FLY ) || is_set( affected_by, AFF_WIND_WALK ) ) )
    return TRUE;

  if( species != NULL && ( is_set( species->act_flags, ACT_CAN_FLY ) || is_set( affected_by, AFF_FLY ) 
    || is_set( affected_by, AFF_WIND_WALK ) ) )
    return TRUE;

  if( shifted != NULL && ( is_set( shifted->act_flags, ACT_CAN_FLY ) || is_set( shifted->affected_by, AFF_FLY )
    || is_set( shifted->affected_by, AFF_WIND_WALK ) ) )
    return TRUE;

  return FALSE;
} 


bool char_data :: can_breath_underwater( )
{
  if( species != NULL )
    return is_set( species->act_flags, ACT_CAN_SWIM );

  if( shifted != NULL )
    return is_set( shifted->act_flags, ACT_CAN_SWIM );

  return FALSE;
}


bool can_climb( char_data* ch )
{
  if( ch->species != NULL && !is_set( ch->species->act_flags, ACT_CAN_CLIMB ) )
    return FALSE;

  if( ch->shifted != NULL && !is_set( ch->shifted->act_flags, ACT_CAN_CLIMB ) )
    return FALSE;
  
  return TRUE;
}

bool can_sneak( char_data* ch )
{
  if (!can_hide(ch, false) || ch->mount)
    return FALSE;

  // Sneaky mobs ... assume they have a sneak skill of 3.
  if ( ch->species != NULL && is_set( ch->affected_by, AFF_SNEAK ) && number_range( 1, 10 ) <= 3 )
    return TRUE;

  if( ch->shifted != NULL && is_set( ch->shifted->affected_by, AFF_SNEAK ) )
    return TRUE;

  // Sneaky players ...
  if ( ch->pcdata != NULL && is_set( ch->pcdata->pfile->flags, PLR_SNEAK ) && ch->check_skill( SKILL_SNEAK ) )
    return TRUE;
      
  return FALSE;
}

/*
 *   CAN MOVE ROUTINE
 */ 


bool Char_Data :: Can_Move( exit_data* exit )
{
  if( !exit || !exit->to_room )
    return FALSE;

  if( shdata->level > LEVEL_HERO && pcdata != NULL && pcdata->trust > LEVEL_AVATAR )
    return TRUE;

  if( is_set( &exit->exit_info, EX_CLOSED ) 
    && !is_set( affected_by, AFF_PASS_DOOR ) )
    return FALSE;

  if (!can_enter( this, exit->to_room, false ))
    return FALSE;
  
  if( pcdata == NULL && ( is_set( species->act_flags, ACT_SENTINEL ) || is_set( &exit->to_room->room_flags, RFLAG_NO_MOB )
    || ( is_set( species->act_flags, ACT_STAY_AREA ) && exit->to_room->area != in_room->area ) ) )
    return FALSE;

  return TRUE;
}


/*
 *   MAIN MOVEMENT ROUTINE
 */


void move_char( char_data* ch, int door, bool flee )
{
  action_data*   action = NULL;
  char_data*   follower;
  room_data*    in_room  = ch->in_room;
  room_data*    to_room;
  exit_data*       exit;
  char_array*      list  = NULL;
  int              move;
  int              type;

  if (!ch || !ch->Is_Valid())
    return;

  ch->shown = 1;

  if( ch->rider != NULL ) {
    move_char( ch->rider, door, flee );
    return;
  }

  if( ( exit = valid_exit( ch, door ) ) == NULL || !can_leave( ch, door, flee ) )
    return;

  to_room = exit->to_room;

  if( !can_enter( ch, to_room, true ) || !handle_terrain( ch, in_room, to_room, move, type ) )
    return;

  if( !ch->mount )
    add_delays( ch, move );

  if( is_exhausted( ch, move, type ) )
    return;

  if( type == MOVE_SWIM && !ch->mount )
    ch->improve_skill( SKILL_SWIMMING );
 
  if( !trigger_leaving( ch, in_room, door, action ) )
    return;

  if( !passes_drunk( ch ) )
    return;

  if( ch->mount != NULL )
    ch->mount->move -= move;
  else
    ch->move -= move;

  if( flee )
    type = MOVE_FLEE;

  if( leader == NULL ) {
    leader = ch;
    list = followers( ch, ch->array );
  }

  leaving_message( ch, in_room, exit, type, action );
  make_tracks( ch, in_room, door );

  ch->From( );
  ch->To( to_room );

  if( ch->mount != NULL ) {
    ch->mount->From( );
    ch->mount->To( to_room );
    ch->improve_skill( SKILL_RIDING );
  }

  ch->room_position = dir_table[door].reverse;

  arrival_message( ch, to_room, exit, type, action );

  if( list != NULL ) {
    for( int i = 0; i < *list; i++ ) {
      if( ( follower = list->list[i] ) != leader && follower->leader && follower->leader->in_room != follower->in_room && follower->position == POS_STANDING ) {
        set_bit( &follower->status, STAT_FOLLOWER );
        move_char( follower, door, FALSE );
      }
    }

    for( int i = 0; i < *list; i++ ) {
      follower = list->list[i];
      if( follower->in_room == to_room ) {
        send( "\r\n", follower );
        show_room( follower, to_room, TRUE, TRUE );
      }
    }

    for( int i = 0; i < *list; i++ ) {
      follower = list->list[i];
      if( follower->in_room == to_room ) 
        trigger_entering( follower, to_room, dir_table[door].reverse );
      remove_bit( &follower->status, STAT_FOLLOWER );
    }

    delete list;
  }

  if( leader == ch ) 
    leader = NULL;

  if( ch->active.time == -1 )
    add_queue( &ch->active, 5 );
}


/* 
 *   MESSAGE ROUTINES
 */


bool group_message( char_data* ch, char_data* actor )
{
  if( actor == leader || !is_set( &actor->status, STAT_FOLLOWER ) ) 
    return FALSE;    

  if( ch->pcdata == NULL || !is_set( &ch->pcdata->message, MSG_GROUP_MOVE ) )
    return TRUE;
  
  if( actor->Seen( ch ) ) {
    if( actor->leader == ch ) 
      send( ch, "%s follows you.\r\n", actor );
    else
      send( ch, "%s follows %s.\r\n", actor, actor->leader );
    }

  return TRUE;
}


void arrival_message( char_data* ch, room_data* room, exit_data* exit, int type, action_data* )
{
  char_data*        rch;
  room_data*    to_room;
  int              back  = dir_table[ exit->direction ].reverse;

  if( type == MOVE_FLEE ) {
    if( ch->mount != NULL ) 
      fsend( *ch->array, "%s, riding %s, charges blindly in, fleeing something %s.\r\n", ch, ch->mount, dir_table[back].where );
    else
      fsend( *ch->array, "%s arrives, obviously fleeing something %s.\r\n", ch, dir_table[back].where );
    return;
  }

  if ( !can_sneak(ch) ) {
    for( int i = 0; i < room->contents; i++ ) {
      if( ( rch = character( room->contents[i] ) ) != NULL && rch != ch && rch->link != NULL && rch->position > POS_SLEEPING && !group_message( rch, ch ) ) {
        if( ch->Seen( rch ) ) {
          if( ch->mount != NULL ) 
            send( rch, "%s riding %s, has arrived.\r\n", ch, ch->mount );
          else 
            send( rch, "%s %s from %s.\r\n", ch, arriving_verb[type], dir_table[ exit->direction ].arrival_msg );
        }
        /*
        else if( can_hear( rch ) ) 
          send( rch, "You hear someone or something arrive.\r\n" );
        */
      }
    }
  }

  for( int i = 0; i < room->exits; i++ ) {
    to_room = room->exits[i]->to_room;
    for( int j = 0; j < to_room->contents; j++ )  
      if( ( rch = character( to_room->contents[j] ) ) != NULL && is_set( rch->affected_by, AFF_SENSE_DANGER ) )
        send( rch, "You sense %s %s.\r\n", ch, dir_table[ exit->direction ].where );
  }

  return;
}


/* 
 *   LEAVING MESSAGES
 */


const char* leaving_msg [] =
{
  "to_char",  "You $t $T.\r\n",
  "to_room",  "$1 $t $T.\r\n",
  ""
};


void leaving_message( char_data* ch, room_data* room, exit_data* exit, int type,  action_data* action )
{
  char_data* rch;

  if( type == MOVE_FLEE ) {
    if( ch->mount != NULL ) {
      send( ch, "Fleeing the battle, you ride %s %s.\r\n", ch->mount, dir_table[ exit->direction ].name );
      send( *ch->array, "Fleeing the battle, %s rides %s %s.\r\n", ch, ch->mount, dir_table[ exit->direction ].name );
    } else {
      send( ch, "You flee %s.\r\n", dir_table[ exit->direction ].name );
      send( *ch->array, "%s blindly flees %s.\r\n", ch, dir_table[ exit->direction ].name );
    }
    return;
  }

  leaving_self( ch, exit, type, action );

  if ( !can_sneak(ch) ) {
    for( int i = 0; i < room->contents; i++ ) {
      if( ( rch = character( room->contents[i] ) ) != NULL && ch != rch && rch->link != NULL && rch->position > POS_SLEEPING && !group_message( rch, ch ) ) {
        leaving_other( rch, ch, room, exit, type, action );
      }
    }
  }
}


void leaving_self( char_data* ch, exit_data* exit, int type, action_data* action )
{
  if( ch->mount != NULL ) 
    send( ch, "You ride %s %s.\r\n", ch->mount, dir_table[ exit->direction ].name );
  else if( leader != NULL && leader != ch ) 
    send( ch, "You follow %s.\r\n", ch->leader );
  else if( is_set( &exit->exit_info, EX_CLOSED ) ) 
    send( ch, "You %s through %s!\r\n", exit->direction == DIR_UP || exit->direction == DIR_DOWN ? "climb" : "step", exit->name );      
  else
    act( ch, prog_msg( action, leaving_msg[0], leaving_msg[1] ), ch, NULL, leaving_action[type], dir_table[ exit->direction ].name );
        
  return;
}


void leaving_other( char_data* rch, char_data* ch, room_data* room, exit_data* exit, int type, action_data* action )
{
  if( !ch->Seen( rch ) ) {
    /*
    if( can_hear( rch, ch ) ) 
       send( rch, "You hear someone or something leave %s.\r\n", 
             dir_table[door].name );
    */
    return;
  }

  if( is_set( &exit->exit_info, EX_CLOSED ) ) 
    send( rch, "%s %s %s passing through %s!\r\n", ch, leaving_verb[type], dir_table[ exit->direction ].name, exit->name );
  else if( ch->mount == NULL ) 
    act( rch, prog_msg( action, leaving_msg[2], leaving_msg[3] ), ch, NULL, leaving_verb[type], dir_table[ exit->direction ].name );
  else 
    send( rch, "%s rides %s %s.\r\n", ch, ch->mount, dir_table[ exit->direction ].name );
}


/*
 *   SUBROUTINES OF MAIN MOVEMENT FUNCTION
 */


bool can_leave( char_data* ch, int door, bool flee )
{
  char_data*    rch;
  bool        after;

  if( is_set( ch->affected_by, AFF_ENTANGLED ) ) {
    send( "You are entangled in a web and quite stuck.\r\n", ch );
    act_leader("** %s is entangled in a web and unable to follow you. **\r\n", ch );
    return FALSE;
  }

  if( is_set( ch->affected_by, AFF_ENSNARE_TRAPPED ) ) {
    send( "You are trapped by vines and quite stuck.\r\n", ch );
    act_leader("** %s is trapped by vines and unable to follow you. **\r\n", ch );
    return FALSE;
  }

  if( flee )
    return TRUE;

  if( opponent( ch ) != NULL ) {
    send( "You can't walk away from a battle - use flee.\r\n", ch );
    act_leader( "** %s is fighting and unable to follow you. **\r\n", ch );  
    return FALSE;
  }

  if( player( ch ) ) { //  || is_set(&ch->status, STAT_PET)) {
    after = FALSE;
    for( int i = 0; i < *ch->array; i++ ) { 
      if( ( rch = character( ch->array->list[i] ) ) != NULL ) {
        if( rch == ch ) {
          after = TRUE; 
          continue;
        }

        if( rch->species && is_set( rch->species->act_flags, ACT_NO_BLOCK ) )
          continue;

        if( ch->Seen( rch )
          && includes( rch->aggressive, ch )
          && rch->Size( ) >= max( SIZE_GNOME, ch->Size( ) - 2 )
          && rch->position >= POS_FIGHTING
          && ( ( after && rch->room_position == door ) || ( !after && ch->room_position != door ) )
/*          && !is_set( ch->affected_by, AFF_ENTANGLED )
          && !is_set( ch->affected_by, AFF_ENSNARE_TRAPPED ) ) */
          && !is_entangled( ch, "move", true ) )
        {
          spoil_hide( rch );
          rch->shown = 1;
          send( ch, "%s is blocking your exit %s.\r\n", rch, dir_table[ door ].name );
//          send( ch, "%s %s blocking your exit %s.\r\n", rch, ( rch->shown == 1 ? "is" : "are" ), dir_table[ door ].name );
          act_leader( "** %s is blocked by %s and is unable to follow you. **\r\n", ch, rch );
          return FALSE;
        }
      }    
    }
  }

  return TRUE;
}


exit_data* valid_exit( char_data* ch, int door )
{
  exit_data* exit = exit_direction( ch->in_room, door );

  if( exit == NULL || !exit->Seen( ch ) ) {
    if( ch->in_room->Seen( ch ) ) {
      if( *terrain_table[ ch->in_room->sector_type ].no_go == '\0' )
        send( ch, "You see no exit %s.\r\n", 
          dir_table[ door ].where );
      else
        act( ch, terrain_table[ ch->in_room->sector_type ].no_go, ch, NULL );
    }
    else { 
      if( *terrain_table[ ch->in_room->sector_type ].no_go == '\0' ) {
        fsend( ch, "You attempt to move %s but find yourself unable to.\r\n",
          dir_table[door].name );
        fsend_seen( ch,
          "%s attempts to move %s and runs straight into a wall.",
          ch, dir_table[door].name );
      }
      else {
        act( ch, terrain_table[ ch->in_room->sector_type ].no_go, ch, NULL );
        act_notchar( terrain_table[ ch->in_room->sector_type ].rno_go, ch, NULL );
      }
    }
    return NULL;
  }

  if( is_set( &exit->exit_info, EX_CLOSED )
    && !is_set( ch->affected_by, AFF_PASS_DOOR ) && !is_apprentice( ch ) ) {
    send( ch, "%s is closed.\r\n", exit );
    return NULL;
    }

  if( is_set( &exit->exit_info, EX_REQUIRES_CLIMB ) 
    && !can_climb( ch ) && !ch->can_fly( ) ) {
    act_leader( "** %s is unable to climb so can't follow you. **\r\n", ch );
    send( ch, "Leaving %s requires you to climb which you are incapable of.\r\n", dir_table[door].name );
    return NULL;
    }   

  return exit;
}


bool can_enter( char_data* ch, room_data* room, bool display )
{
  player_data* pc = player( ch );

  if( ch->pcdata != NULL && room->area->status != AREA_OPEN && room->area->status != AREA_IMMORTAL && !is_apprentice( ch ) ) {
    if( display )
      send( ch, "That area is not open to players yet.\r\n" );
    return FALSE;
  }

  if( ch->Size( ) > room->size && !is_set( &ch->status, STAT_FAMILIAR ) ) {
    if( display ) {
      send( ch, "You are too large to fit in there.\r\n" );
      act_leader( "** %s is too large to follow you there. **\r\n", ch );
    }
    return FALSE;
  }
  
  if( ch->mount != NULL ) {
    if( ch->mount->Size( ) > room->size && ch->mount != pc->familiar ) {
      if( display )
        send( ch, "%s is too large to fit in there.\r\n", ch->mount );
      return FALSE;
    }
    else if( ch->mount->Size( ) > room->size ) {
      if( display )
  send( ch, "You can not enter there while mounted on %s.\r\n", ch->mount );
      return FALSE;
    }
    if( is_set( &room->room_flags, RFLAG_NO_MOUNT ) ) {
      if( display )
        send( "You can not go there while mounted.\r\n", ch );
      return FALSE;
    }
  } 

  return TRUE;
}


/*
 *   DRUNK ROUTINE
 */


const char *drunk_message[] = {
  "You stumble and barely stay on your feet.\r\n",
  "$n stumbles and barely stays on $s feet.",

  "The ground moves quickly sending you reeling.\r\n",
  "$n staggers, obviously intoxicated.",

  "Your legs give way to gravity.\r\n",
  "$n suddenly sits down and looks quite surprised.",

  "You stumble.\r\n",
  "$n has had too much to drink and falls to the ground.",

  "You trip over your left foot.\r\n",
  "The feet of $n decide to fight and down $e falls."
};


bool passes_drunk( char_data* ch )
{
  if( !ch || !ch->in_room || !IS_DRUNK( ch ) || number_range( 1, 20 ) < 18 )
    return TRUE;

  if( ch->mount != NULL ) {
    send( ch, "Too drunk to hold on, you fall from your mount!\r\n" );
    send( *ch->array, "Too drunk to hold on, %s falls from %s mount!\r\n", ch, ch->His_Her() );
    ch->mount->rider = NULL;
    ch->mount = NULL;

    ch->position  = POS_RESTING;
    ch->hit      -= 2;

    update_pos( ch );   

    return FALSE;
  }

  int i = number_range( 0, 4 );

  send( ch, drunk_message[2*i] );
  act_notchar( drunk_message[2*i+1], ch );

  if( i <= 1 )
    return TRUE;

  ch->position  = POS_RESTING;
  ch->hit      -= 2;

  update_pos( ch );   

  return FALSE;
}


/*
 *   EXHAUSTION
 */


void add_delays( char_data* ch, int& move )
{
  int   flag  [] = { PLR_TRACK, PLR_SEARCHING, PLR_SNEAK };
  int   cost  [] = { 2, 3, 2 };
  int      i;

  if( ch->pcdata == NULL ) 
    return;

  for( i = 0; i < 3; i++ ) 
    if( is_set( ch->pcdata->pfile->flags, flag[i] ) ) 
      move += cost[i];
}


bool is_exhausted( char_data* ch, int& move, int type )
{
  if( type == MOVE_SWIM && ch->pcdata != NULL )
    move *= 12-10*ch->get_skill( SKILL_SWIMMING )/MAX_SKILL_LEVEL;

  if( ch->mount != NULL ) {
    if( ch->mount->move < move ) {
      send( ch, "Your mount is exhausted.\r\n" );
      return TRUE;
    }
  } else {
    if( ch->move < move ) {
      send( "You are too exhausted.\r\n", ch );
      act_leader( "** %s is too exhausted to follow you. **\r\n", ch );
      return TRUE;
    }
  }

  return FALSE;
}


/*
 *   TERRAIN FUNCTIONS
 */


int get_motion( char_data* ch )
{
  /* since dodging dex traps became a problem with floaters, its no longer a concern
  if( ch->can_float( ) ) 
    return MOVE_FLOAT;
  */
  if( ch->can_fly( ) ) 
    return MOVE_FLY;

  if( ch->species != NULL && is_set( ch->species->act_flags, ACT_SLITHERS ) ) 
    return MOVE_SLITHERS;

  if( ch->pcdata != NULL && is_set( ch->pcdata->pfile->flags, PLR_SNEAK ) && !ch->mount && can_hide(ch, false) ) {
    ch->improve_skill( SKILL_SNEAK );
    return MOVE_SNEAK;
  }

  return MOVE_WALK;
}

bool non_water_type( room_data* to )
{
  if( terrain_table[ to->sector_type ].shallow || terrain_table[ to->sector_type ].underwater 
    || terrain_table[ to->sector_type ].water_surface )
    return FALSE;

  return TRUE;
}

bool handle_terrain( char_data* ch, room_data* from, room_data* to, int& move, int& type )
{
  char_data*   mount  = ch->mount;

  type = get_motion( mount == NULL ? ch : mount );

  move = int( ( terrain_table[ from->sector_type ].move_cost + terrain_table[ to->sector_type ].move_cost )
    * ( 1 + 5.0 * ch->contents.weight / URANGE( 1, ch->Capacity( ), ch->Empty_Capacity( ) ) ) / 3 );  

  switch( type ) {
    case MOVE_FLOAT:
      move /= 5;
      break;

    case MOVE_FLY:
      move /= 2;
      break;

    default:
      move *= 1;
      break;
  }

  float new_move;

  if( mount != NULL ) {
    new_move = move*(1-ch->get_skill( SKILL_RIDING )/( 2* MAX_SKILL_LEVEL ) );
    move = (int) new_move;
  }

  if( ch->leader != NULL ) {
    ch->leader->improve_skill( SKILL_PATH_FINDING );
    new_move = move*(1-ch->leader->get_skill( SKILL_PATH_FINDING )/( 2*MAX_SKILL_LEVEL ) );
    move = (int) new_move;
  }
  else if( ch->pcdata != NULL && ch->pcdata->clss == CLSS_RANGER ) {
    ch->improve_skill( SKILL_PATH_FINDING );
    new_move = move*(1-ch->get_skill( SKILL_PATH_FINDING )/( 2*MAX_SKILL_LEVEL ) );
    move = (int) new_move;
  }

  if( non_water_type( to ) && ( is_set( ch->affected_by, AFF_MOVE_WATER_ONLY ) || ( ch->shifted != NULL && is_set( ch->shifted->affected_by, AFF_MOVE_WATER_ONLY ) ) ) ) {
    send( ch, "You can not leave the water.\r\n" );
    act_leader( "** %s will not survive if it leaves the water. **\r\n", ch );
    return FALSE;
  }

  if( terrain_table[ to->sector_type ].ethereal ) {
    if( is_set( ch->affected_by, AFF_SOLIDIFY ) )
      return TRUE;
    if( mount != NULL ) {
      send( ch, "Your mount can not cross that terrain.\r\n" );
    } else {
      send( ch, "You can not cross that terrain.\r\n" );
      act_leader( "** %s is unable to cross that terrain. **\r\n", ch );
    }
    return FALSE;
  }

  if( terrain_table[ to->sector_type ].fly ) {
    if( type == MOVE_FLY )
      return TRUE;
    if( mount != NULL ) {
      send( ch, "Your mount does not know how to fly.\r\n" );
    } else {
      send( ch, "You can't fly.\r\n" );
      act_leader("** %s is unable to fly so does not follow you. **\r\n", ch );
    }
    return FALSE;
  }

  if(  terrain_table[ to->sector_type ].underwater ) { 
    if( type <= MOVE_WALK ) {
      if( mount != NULL ) { 
        if( !mount->can_swim( ) ) {
          send( ch, "Your mount does not know how to swim.\r\n" );
          return FALSE;
        }
      } else {
        if( !ch->can_swim( ) ) {
          send( ch, "You don't know how to swim or fly.\r\n" );
          act_leader("** %s can not swim or fly so fails to follow you. **\r\n", ch );
          return FALSE; 
        }
      }
      type = MOVE_SWIM;
    }
    return TRUE;
  }   
  
  if( terrain_table[ to->sector_type ].underwater ) {
    if( mount != NULL ) {
      send( ch, "You can't ride underwater.\r\n" );
      return FALSE;
    }
    if( !ch->can_swim( ) ) {
      send( ch, "You don't how to swim.\r\n" );
      act_leader( "** %s can not swim so fails to follow you. **\r\n", ch ); 
      return FALSE;
    }
    type = MOVE_SWIM;
    return TRUE;
  }

  if( terrain_table[ from->sector_type ].shallow ) {
    type = ( ch->species != NULL && is_set( ch->species->act_flags, ACT_CAN_SWIM ) ) ? MOVE_SWIM : MOVE_WADE;
    if( ch->species != NULL && is_set( ch->species->act_flags, ACT_SLITHERS ) )
      type = MOVE_SLITHERS;
    return TRUE;
  }

  return TRUE;
}


/*
 *   ENTERING/LEAVNG TRIGGERS
 */


bool trigger_leaving( char_data *ch, room_data *room, int door, action_data*& action )
{
  char_data*       npc;
  mprog_data*    mprog;
  bool          result = TRUE;

  for( int i = 0; i < *ch->array; i++ ) { 
    if( ( npc = mob( ch->array->list[i] ) ) == NULL || npc->pcdata != NULL || npc->position < POS_RESTING || npc == ch || !npc->Can_See() )
      continue; 
    for( mprog = npc->species->mprog; mprog != NULL; mprog = mprog->next ) 
      if( mprog->trigger == MPROG_TRIGGER_LEAVING && ( mprog->value == door || mprog->value == -1 ) ) {
        var_ch   = ch;
        var_mob  = npc;
        var_room = room;
        if( !execute( mprog ) || ch->in_room != room )
          return FALSE;
      }
  }
 
  for( action = ch->in_room->action; action != NULL; action = action->next )
    if( action->trigger == TRIGGER_LEAVING && is_set( &action->flags, door ) ) {
      var_ch   = ch;
      var_room = room;
      result = execute( action );
      if( ch->in_room != room ) 
        return FALSE;
      if( result )
        return TRUE;
    }

  return result;
}


bool trigger_entering( char_data *ch, room_data *room, int door )
{
  action_data*   action;
  char_data*        npc;
  mprog_data*     mprog;
  obj_data*         obj;
  oprog_data*     oprog;

  for( int i = 0; i < room->contents; i++ ) {
    if( ( npc = mob( room->contents[i] ) ) == NULL || npc == ch || npc->position < POS_RESTING || !npc->Can_See() )
      continue;
    for( mprog = npc->species->mprog; mprog != NULL; mprog = mprog->next ) 
      if( mprog->trigger == MPROG_TRIGGER_ENTRY && ( mprog->value == door || mprog->value == -1 ) )  {
        var_ch = ch;
        var_mob = npc;
        var_room = room;
        execute( mprog );
        if( ch->in_room != room )
          return FALSE;
      }
  }

  for( action = room->action; action != NULL; action = action->next )
    if( action->trigger == TRIGGER_ENTERING && is_set( &action->flags, door ) ) {
      var_ch   = ch; 
      var_room = room;
      execute( action );
      if( ch->in_room != room )
        return FALSE;
      break;
    }

  for ( int j = 0; j < room->contents.size; j++ ) {
    obj = object( room->contents[j] );
    if ( obj == NULL ) continue;
    for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) {
      if( oprog->trigger == OPROG_TRIGGER_ENTERING ) {
        var_ch = ch;
        var_obj = obj;
        var_room = room;
        execute( oprog );
        if( ch->in_room != room )
          return FALSE;
      }
    }
  }

  return TRUE;
}


/*
 *   LEADER MESSAGE ROUTINES
 */


void act_leader( const char* text, char_data* follower, char_data *blocker )
{
  char buf [ MAX_INPUT_LENGTH ];

  if( leader == NULL )
    return;

  if( follower->leader != leader )
    return;

  if( blocker == NULL )
    sprintf( buf, text, follower->Name( leader ) );
  else
    sprintf( buf, text, follower->Name( leader ), blocker->Name( leader ) );

  buf[3] = toupper( buf[3] );
  send( buf, leader ); 

  return;
}


/*
 *   DO MOVE FUNCTIONS
 */


void do_north( char_data* ch, char* )
{
  move_char( ch, DIR_NORTH, FALSE );
  return;
}


void do_east( char_data *ch, char* )
{
  move_char( ch, DIR_EAST, FALSE );
  return;
}


void do_south( char_data *ch, char* )
{
  move_char( ch, DIR_SOUTH, FALSE );
  return;
}


void do_west( char_data* ch, char* )
{
  move_char( ch, DIR_WEST, FALSE );
  return;
}


void do_up( char_data *ch, char* )
{
  move_char( ch, DIR_UP, FALSE );
  return;
}


void do_down( char_data *ch, char* )
{
  move_char( ch, DIR_DOWN, FALSE );
  return;
};

bool search_object( char_data* ch, obj_data* corpse )
{
  obj_data* obj;

  if( corpse == NULL )
    return FALSE;

  if( corpse->pIndexData->item_type != ITEM_SEARCHABLE_CORPSE ) {
    send( ch, "You can't search %s.\r\n", corpse );
    return TRUE;
  }

  if( corpse->contents.size > 0 ) {
    send( ch, "You search through %s.\r\n", corpse );
    send_seen( ch, "%s searches through %s.\r\n", ch, corpse );
    for( int i = corpse->contents.size-1; i >= 0; i-- ) {
      if( ( obj = object( corpse->contents[i] ) ) != NULL ) {
        obj->From( obj->number );
        if( can_carry( ch, obj, false ) ) {
          send( ch, "You pick up %s and add it to your items.\r\n", obj );
          send_seen( ch, "%s picks up %s and adds it to %s inventory.\r\n", ch, obj, ch->His_Her( ) );
          obj->To( ch );
        }
        else {
          send( *ch->array, "You place %s on the ground.\r\n", obj );
          obj->To( ch->array );
        }
      }
    }
  }
  else 
    send( ch, "You find nothing of interest in %s.\r\n", corpse );

  send( ch, "You dispose of %s in the process.\r\n", corpse );
  corpse->Extract( 1 );

  return TRUE;
}

void do_search( char_data* ch, char* argument )
{
  action_data*  action;
  room_data*      room  = ch->in_room;
  obj_data*     corpse;

  if( not_player( ch ) )
    return;

  if( !room->Seen( ch ) ) {
    send( ch, "How do you expect to search what you can't see?!?\r\n" );
    return;
  }

  for( action = ch->in_room->action; action != NULL; action = action->next ) {
    if( action->trigger == TRIGGER_SEARCHING && ( ( *action->target == '\0' && *argument == '\0' ) || ( *argument != '\0' && is_name( argument, action->target ) ) ) ) {
      var_ch   = ch;
      var_room = ch->in_room;
      if( !execute( action ) || ch->in_room != room ) 
        return;
      ch->improve_skill( SKILL_SEARCHING );
      break;
    }
  }

  corpse = one_object( ch, argument, empty_string, ch->array );

  if( search_object( ch, corpse ) )
    return;

  if ( ch->get_skill( SKILL_SEARCHING ) != UNLEARNT ) {
    if( toggle( ch, argument, "Searching", ch->pcdata->pfile->flags, PLR_SEARCHING ) )
      return;
  }

  if( *argument == '\0' )
    send( "You rummage around but find nothing interesting.\r\n", ch );
  else
    send( "Whatever that is, searching it results in nothing interesting.\r\n", ch ); 
}

  
/*
 *   SPEED WALKING
 */
int speed_number( const char*& argument )
{
  const char* str  = argument;
  int           j  = 0;

  for( ; ; ) {
    if( *str == '\0' || !isdigit( *str ) )
      break;
    j = 10*j+(*str++)-'0';
  }

  argument = str;
  return j;
}


struct speedwalk_data
{
  int num;
  char dir[2];
};

void do_speedwalk( char_data* ch, char* argument )
{
  speedwalk_data path[128];
  memset( path, 0, 128 * sizeof( speedwalk_data ) );

  int j = 0, step = 0, total = 0;
  for( int i = 0; argument[i] != '\0'; i++ ) {
    if( isdigit( argument[i] ) ) {
      j = 10 * j + (argument[i] - '0');
    } else {
      switch( toupper( argument[i] ) ) {
      case 'S':
      case 'N':
      case 'U':
      case 'D':
      case 'E':
      case 'W':
        break;

      default:
        send( ch, "Speedwalk path not valid.\r\n" );
        return;
      }

      path[step].dir[0] = argument[i];
      path[step].num = ( j == 0 ? 1 : j );

      total += j;
      if( total > 100 ) {
        send( ch, "Speedwalk path too long (100 steps maximum).\r\n" );
        return;
      }

      j = 0;
      step++;
    }
  }

  send( ch, "Speedwalking: %s\r\n", argument );

  bool dq = false;
  if( ch->pcdata ) {
    dq = is_set( &ch->pcdata->message, MSG_QUEUE );
    remove_bit( &ch->pcdata->message, MSG_QUEUE );
  }

  for( int i = 0; path[i].dir[0] != '\0'; i++ ) {
    int j = path[i].num;
    do {
      interpret( ch, path[i].dir );
      j--;
    } while( j > 0 );
  }

  if( dq )
    set_bit( &ch->pcdata->message, MSG_QUEUE );
}
