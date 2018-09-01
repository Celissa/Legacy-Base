#include "system.h"

/*
 *   CONSTANTS
 */


const char* rsflags_mob [] = { "Leader", "Follower", "Sentinel", "Night", "Day", "Aggressive", "" };
const char* rsflags_obj [] = { "Container", "Inside", "Unknown", "Unused1", "Unused2", "" };

const char* reset_pos_name [] = { "sleeping", "meditating", "resting", "standing" }; 
const char* reset_pos_abbrev [] = { "Slee", "Medi", "Rest", "Stan" };
const int rspos_index [] = { POS_SLEEPING, POS_MEDITATING, POS_RESTING, POS_STANDING };


/*
 *   LOCAL FUNCTIONS
 */


void         extract         ( reset_data* );

char*        set_rsflags     ( char_data*, reset_data*, char* );
void         display         ( char_data*, reset_data*, int ); 
const char*  modify_reset    ( char_data*, reset_data*, char* );

void         register_reset  ( char_data*, room_data* );

obj_data*    create          ( obj_clss_data*, reset_data*, mob_data* );
void         mob_setup       ( mob_data*, room_data* );
void         place           ( obj_data*, reset_data*, room_data*, mob_data*, obj_data*& );

void         reset_mob       ( reset_data*, room_data*, mob_data*&, char_data*& );
void         reset_obj       ( reset_data*, room_data*, mob_data*, obj_data*& );
void         reset_table     ( reset_data*, rtable_data*, room_data*, mob_data*, obj_data*& );


int rtable_calls;


/*
 *   RESET_DATA CLASS
 */


Reset_Data :: Reset_Data( )
{
  record_new( sizeof( reset_data ), MEM_MRESET );

  next     = NULL;
  chances  = ( 100 + ( 90 << 24 ) );
  target   = NULL;
//  vnum     = 0;
  flags    = ( 1 << RSFLAG_REROLL );
  count    = 0;
  liquid   = 0;

  return;
}


Reset_Data :: ~Reset_Data( )
{
  record_delete( sizeof( reset_data ), MEM_MRESET );

  return;
}


/*
 *   SUPPORT ROUTINES
 */


void extract( reset_data* reset )
{
  for( int i = 0; i < mob_list; i++ )
    if( mob_list[i]->Is_Valid() && mob_list[i]->reset == reset )
      mob_list[i]->reset = NULL;

  delete reset;
}


char* name( reset_data* reset )
{
  if( reset == NULL )
    return "none";

  return "somewhere";
}


/*
 *   ONLINE EDITING COMMAND
 */


void do_reset( char_data* ch, char* argument )
{
  wizard_data*  wizard;

  wizard = (wizard_data*) ch;

  if( !strcasecmp( argument, "room" ) ) {
    ch->in_room->Reset( );
    send( ch, "Room reset.\r\n" );

    // Search room for a shopkeeper mob.  If found, reset_shop him.
    if ( ch->array != NULL ) {
      for ( int i = ch->array->size - 1; i >= 0; i-- ) {
        mob_data* keeper = mob(ch->array->list[i]);
        if ( keeper != NULL && keeper->pShop != NULL && keeper->reset != NULL ) {
          reset_shop( keeper );
          send( ch, "%s's shop reset.\r\n", keeper );
          break;
        }
      }
    }
    return;
  }

  if( !strcasecmp( argument, "area" ) ) {
    ch->in_room->area->Reset( true );
    send( ch, "Area reset in %0.3fms.\r\n", ch->in_room->area->reset_lag.time( ) / 1000 );
    return;
  }

  if( !strcasecmp( argument, "lag" ) ) {
    if( ch->in_room->reset == NULL ) {
      send( ch, "No resets found.\r\n" );
      return;
    }

    int i = 1;
    for( reset_data* reset = ch->in_room->reset; reset; reset = reset->next )
      page( ch, "[%2d] %0.3f\r\n", i++, reset->lag.time( ) / 1000 );

    return;
  }

  edit_reset( ch, argument, ch->in_room->reset, RST_ROOM );
}


void do_mreset( char_data* ch, char* argument )
{
  species_data*  species;
  wizard_data*    wizard;

  wizard = (wizard_data*) ch;

  if( ( species = wizard->mob_edit ) == NULL ) {
    send( ch, "You aren't editing any mob.\r\n" );
    return;
  }

  const char* info = edit_reset( ch, argument, species->reset, RST_MOB );
  if( info )
    mob_log( ch, species->vnum, info );
}


const char* edit_reset( char_data* ch, char* argument, reset_data*& list, int type, const char* name )
{
  reset_data*        reset;
  reset_data*         prev;
  int                 i, j;

  if( *argument == '\0' ) {
    if( name != empty_string ) {
      page_centered( ch, "-=- %s -=-", name );
      page( ch, "\r\n" );
    }
    display( ch, list, type );
    return NULL;
  }

  if( type == RST_ROOM )
    ch->in_room->area->modified = TRUE;

  if( number_arg( argument, i ) ) {
    if( *argument == '\0' ) {
      send( ch, "What flag or option do you want to set?\r\n" );
      return NULL;
    }
    
    if( ( reset = locate( list, i ) ) == NULL ) {
      send( ch, "No reset exists with that number.\r\n" );
      return NULL;
    }
    
    if( isdigit( *argument ) && number_arg( argument, j ) ) {
      if( j == i ) {
        send( ch, "Moving a reset to where it already is does nothing interesting.\r\n" ); 
        return NULL;
      }

      if( j == 1 ) {
        remove( list, reset );
        reset->next = list;
        list        = reset;
      } else {
        if( j < 1 || j > count( list ) ) {
          send( ch, "You can only move a reset to a sensible position.\r\n" );
          return NULL;
        }
        remove( list, reset );
        prev        = locate( list, j-1 );
        reset->next = prev->next;
        prev->next  = reset;
      }

      char* tmp = static_string( );
      sprintf( tmp, "Reset %d moved to position %d.", i, j );
      send( ch, "%s\r\n", tmp );
      return tmp;
    }

    argument = set_rsflags( ch, reset, argument ); // not logged yet

    return modify_reset( ch, reset, argument );
  }

  if( matches( argument, "delete" ) ) {
    if( list == NULL ) {
      send( ch, "The list of resets is empty.\r\n" );
      return NULL;
    }
    if( !strcasecmp( argument, "all" ) ) {
      for( reset = list; reset != NULL; reset = list ) {
        list = list->next;
        extract( reset );
      }

      char* tmp = static_string( "All resets deleted." );
      send( ch, "%s\r\n", tmp );
      return tmp;
    }

    i = atoi( argument );

    if( ( reset = locate( list, i ) ) == NULL ) {
      send( ch, "No reset with that number found to delete.\r\n" );
      return NULL;
    }

    char* tmp = static_string( );
    sprintf( tmp, "Reset %d deleted.", i );
    send( ch, "%s\r\n", tmp );
    remove( list, reset );
    extract( reset );
    return tmp;
  }

  if( matches( argument, "table" ) ) {
    rtable_data *rtable = NULL;

    if( *argument == '\0' ) {
      send( ch, "What rtable do you wish to reset?\r\n" );
      return NULL;
    }

    if( number_arg( argument, i ) ) {
      rtable = find_rtable( i );
    } else {
      rtable = find_rtable( argument );
    }

    if( rtable == NULL ) {
      send( ch, "No such rtable exists.\r\n" );
      return NULL;
    }
 
    char* tmp = static_string( );
    sprintf( tmp, "Reset for rtable %d '%s' added.", rtable->vnum, rtable->name );
    send( ch, "%s\r\n", tmp );

    reset           = new reset_data;
    reset->target   = rtable;
    reset->flags    = 0;
    if( type == RST_ROOM )
      reset->value    = -2;
    else if( type == RST_MOB )
      reset->value    = -1;
    else
      reset->value    = 0;

    append( list, reset );
    return tmp;
  }

  thing_data*  thing;
  mob_data*      npc;
  obj_data*      obj;

  if( ( thing = one_thing( THING_DATA, ch, argument, "reset", ch->array, &ch->contents ) ) == NULL )
    return NULL;

  if( ( npc = mob( thing ) ) != NULL ) {
    if( type != RST_ROOM ) {
      send( ch, "You can't reset a mob on a mob or in a table.\r\n" );
      return NULL;
    }

    reset           = new reset_data;
    reset->target   = npc->species;
    reset->flags    = ( 1 << RSFLAG_MOB ) | ( 1 << RSFLAG_REROLL );
    reset->value    = RSPOS_STANDING;

    append( list, reset );

    char* tmp = static_string( );
    sprintf( tmp, "Reset for %s added.", npc->Name( ) );
    send( ch, "%s\r\n", tmp );
    return tmp;
  }

  if( ( obj = object( thing ) ) != NULL ) {
    reset           = new reset_data;
    reset->target   = obj->pIndexData;
    reset->flags    = ( 1 << RSFLAG_OBJECT ) | ( 1 << RSFLAG_REROLL );
    if( type == RST_ROOM )
      reset->value    = -2;
    else
      reset->value    = -1;

    append( list, reset );

    char* tmp = static_string( );
    sprintf( tmp, "Reset for %s added.", obj->Name( ) );
    send( ch, "%s\r\n", tmp );
    return tmp;
  }

  send( ch, "You can't reset %s.\r\n", thing );
  return NULL;
}


void display( char_data* ch, reset_data* reset, int type ) 
{
  char                  buf  [ TWO_LINES ];
  char                flags  [ 32 ];
  char               liquid  [ 32 ];
  char                 rust  [ 5 ];
  int*              chances;
  int               i, j, k;
  obj_clss_data*   obj_clss;
  species_data*     species;
  rtable_data*       rtable;

  if( reset == NULL ) {
    send( ch, "No resets found.\r\n" );
    return;
  }

  page_underlined( ch, "Nmbr Rrl  1_Ch 2_Ch 3_Ch  Rust %sLiq Flags Vnum Descr\r\n", type == RST_TABLE ? "" : "Posi " );

  for( i = 1; reset != NULL; i++, reset = reset->next ) {
    chances = unpack_int( reset->chances );

    if( is_set( &reset->flags, RSFLAG_OBJECT ) ) {
      obj_clss = (obj_clss_data*) reset->target; // get_obj_index( reset->vnum );
      if( obj_clss == NULL ) {
        sprintf( buf, "[%2d] %c%c%c  %3d%% %3d%% %3d%%  %s %s %-5s <NULL OBJECT>\r\n", 
          i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
          chances[0], chances[1], chances[2], rust,
          liquid, flags);

      } else {
        for( k = 0, j = RSFLAG_LEADER; j < MAX_RSFLAG; j++ ) {
          if( is_set( &reset->flags, j ) )
            flags[k++] = rsflags_obj[ j-RSFLAG_LEADER ][0];
        }

        flags[k] = '\0';

        sprintf( liquid, ( obj_clss != NULL && ( obj_clss->item_type == ITEM_DRINK_CON || obj_clss->item_type == ITEM_FOUNTAIN ) ) ? liquid_table[reset->liquid].name : "   " );
        liquid[3] = '\0';

        if( obj_clss->metal() && !is_set( obj_clss->extra_flags, OFLAG_RUST_PROOF ) )
          sprintf( rust, "%3d%%", chances[3] );
        else
          sprintf( rust, "    " );

        char *temp = str_dup(name_brief(obj_clss));
        if( type == RST_TABLE ) {
          sprintf( buf, "[%2d] %c%c%c  %3d%% %3d%% %3d%%  %s %s %-5s %4d %s\r\n", 
            i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
            is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
            is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
            chances[0], chances[1], chances[2], rust,
            liquid, flags, obj_clss->vnum, /* reset->vnum, */
            truncate( temp, 29 ) );
        } else {
          sprintf( buf, "[%2d] %c%c%c  %3d%% %3d%% %3d%%  %s %s %s %-5s %4d %s\r\n", 
            i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
            is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
            is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
            chances[0], chances[1], chances[2], rust,
            wear_abbrev[ reset->value+2 ], liquid, flags,
            obj_clss->vnum, /* reset->vnum, */ truncate( temp, 29 ) );
        }
        delete [] temp;
      }
    } else if( is_set( &reset->flags, RSFLAG_MOB ) ) {
      species = (species_data*) reset->target; // get_species( reset->vnum );
      if( species == NULL ) {
        sprintf( buf,
          "[%2d] %c%c%c  %3d%% %3d%% %3d%%       %s     %-5s <NULL MOBILE>\r\n", 
          i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
          chances[0], chances[1], chances[2],
          reset_pos_abbrev[ reset->value ], flags);
      
      } else {
        for( k = 0, j = RSFLAG_LEADER; j < MAX_RSFLAG; j++ ) {
          if( is_set( &reset->flags, j ) )
            flags[k++] = rsflags_mob[ j-RSFLAG_LEADER ][0];
        }

        flags[k] = '\0';
        sprintf( buf,
          "[%2d] %c%c%c  %3d%% %3d%% %3d%%       %s     %-5s %4d %s\r\n", 
          i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
          chances[0], chances[1], chances[2],
          reset_pos_abbrev[ reset->value ], flags, species->vnum, /* reset->vnum, */
          species->Name( ) );
      }
    } else {
      rtable = (rtable_data*) reset->target;

      *flags = '\0';
      if( type == RST_TABLE )
        sprintf( buf,
          "[%2d] %c%c%c  %3d%% %3d%% %3d%%           %-5s  TBL %s\r\n", 
          i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
          chances[0], chances[1], chances[2],
          flags, rtable != NULL ? truncate( rtable->name, 29 ) : "<NULL RTABLE>" );
      else
        sprintf( buf,
          "[%2d] %c%c%c  %3d%% %3d%% %3d%%       %s     %-5s  TBL %s\r\n", 
          i, is_set( &reset->flags, RSFLAG_REROLL ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+1 ) ? '*' : ' ',
          is_set( &reset->flags, RSFLAG_REROLL+2 ) ? '*' : ' ',
          chances[0], chances[1], chances[2],
          wear_abbrev[reset->value+2], flags, 
          rtable != NULL ? truncate( rtable->name, 29 ) : "<NULL RTABLE>" );
    }
    page( ch, buf );
  }
}


char* set_rsflags( char_data* ch, reset_data* reset, char* argument )
{
  bool             set;
  int                i;
  const char**   flags;

  flags = ( is_set( &reset->flags, RSFLAG_MOB ) ? rsflags_mob : rsflags_obj );

  for( ; ; ) {
    if( *argument != '+' && *argument != '-' )
      return argument;
 
    set = ( *argument++ == '+' );

    for( ; *argument != '\0' && *argument != ' '; argument++ ) {
      for( i = 0; flags[i][0] != '\0'; i++ ) {
        if( toupper( *argument ) == flags[i][0] ) {
          assign_bit( &reset->flags, RSFLAG_LEADER+i, set );
          send( ch, "%s flag set %s.\r\n", flags[i], true_false( &reset->flags, RSFLAG_LEADER+i ) );
          break;
        }
      }

      if( *argument >= '1' && *argument <= '3' ) {
        i = *argument-'1';
        assign_bit( &reset->flags, RSFLAG_REROLL+i, set );
        send( ch, "Reroll bit %d set %s.\r\n", i+1, true_false( &reset->flags, RSFLAG_REROLL+i ) );
      } else if( flags[i][0] == '\0' ) {
        send( ch, "Unknown flag -- %c\r\n", *argument );
      }
    }  

    for( ; *argument == ' '; argument++ );
  }
}


const char* modify_reset( char_data* ch, reset_data* reset, char* argument )
{
  int*   chances  = unpack_int( reset->chances ); 
  const char* flag;

  if( *argument == '\0' )
    return NULL;

  argument = set_rsflags( ch, reset, argument );

  if( *argument == '\0' )
    return NULL; 

  if( is_set( &reset->flags, RSFLAG_MOB ) ) {
    class type_field type_list[] = {
      { "Position",  MAX_RESET_POS,  &reset_pos_name[0],  &reset_pos_name[1], &reset->value },
      { "",          0,              NULL,     NULL,    NULL          }
    };
    
    flag = process( type_list, ch, "reset", argument );
    if( flag && *flag )
      return flag;

  } else {
    reset->value += 2;
    class type_field type_list[] = {
      { "position",  MAX_ITEM_WEAR-1,   &reset_wear_name[0],    &reset_wear_name[1],    &reset->value   },
      { "liquid",    MAX_ENTRY_LIQUID,  &liquid_table[0].name,  &liquid_table[1].name,  &reset->liquid  },
      { "",          0,                 NULL,     NULL,     NULL            }
    };
    
    flag = process( type_list, ch, "reset", argument );
    reset->value -= 2;
    if( flag && *flag ) 
      return flag;
  }

  class int_field int_list[] = {
//    { "Vnum",              0, 9999,  &reset->vnum    },
    { "1_Chance",          0,  100,  &chances[0]     },
    { "2_Chance",          0,  100,  &chances[1]     },
    { "3_Chance",          0,  100,  &chances[2]     },
    { "Rust",              0,  100,  &chances[3]     },
    { "",                  0,    0,  NULL            }
  };

  flag = process( int_list, ch, "reset", argument );
  if( flag && *flag ) {
    reset->chances = pack_int( chances ); 
    return flag;
  }

  send( ch, "Unknown Field - See help reset.\r\n" );
  return NULL;
}


/*
 *   MAIN ROUTINE TO RESET AN AREA
 */


bool passes( reset_data* reset, int* roll, char_data* ch )
{
  int*  chances;

  // unpack chances[0] thru [3]
  chances = unpack_int( reset->chances );

  // if the reroll bit is set, set the roll to -102 (so that it's rerolled)
  for( int i = 0; i < 3; i++ ) {
    if( is_set( &reset->flags, RSFLAG_REROLL+i ) ) 
      roll[i] = -102;
  }

  for( int i = 0; i < 3; i++ ) {
    if( chances[i] > 0 ) {
      // the chance is greater than 0%
      
      if( roll[i] == -102 ) {
        // roll is -102 which means we should reroll the value
        if( boot_stage == BOOT_UPDATE )
          roll[i] = number_range( 5, 99 );  // prevent good stuff from popping on reboot
        else
          roll[i] = number_range( 0, 99 );
      } 
      else if( roll[i] < 0 ) {
        // roll is already less than 0 meaning something has popped, so set to -101
        // and don't pop anything else until the next reroll bit is set
        roll[i] = -101;
        return FALSE;
      }

      if( ch != NULL && ch->species == NULL ){
        roll[i] -= 5*ch->get_skill( SKILL_SKINNING )/MAX_SKILL_LEVEL;
        ch->improve_skill( SKILL_SKINNING );
      }
      // subtract the percentage from the roll, if it's below zero then the thing's popped
      if( ( roll[i] -= chances[i] ) >= 0 )
        return FALSE;

    } else { 
      // the chance is 0%
      // if the value is -101 then we've failed to pop anyways
      // if the roll is greater than 0 then we've also failed to pop as we haven't got below zero
      if( roll[i] == -101 || roll[i] >= 0 )
        return FALSE;
    }
  }

  return TRUE;
}


void Room_Data :: Reset( bool force )
{
  char_data*       leader  = NULL;
  mob_data*           mob  = NULL;
  int                roll  [ 3 ];
  int               count  = -1; 
  exit_data*         exit;
  exit_data*         back;
  obj_data*     container  = NULL;
  obj_data*           obj;
  int*                 w1;
  int*                 w2; 
  struct timeval reset_start;

  if( !is_set( &room_flags, RFLAG_APPROVED ) &&
    area->status == AREA_OPEN )
    set_bit( &room_flags, RFLAG_APPROVED );

  if( is_set( &room_flags, RFLAG_SAVE_ITEMS ) ) {
    save_room_items( this );
    return;
  }

  gettimeofday( &room_start, NULL );
  reset_count++;

  for( int i = contents-1; i >= 0; i-- ) {
    if( ( obj = object( contents[i] ) ) != NULL && obj->pIndexData->vnum != OBJ_CORPSE_PC )
      obj->Extract( );
  }

  gettimeofday( &room_extracted, NULL );

  for( int i = 0; i < 3; i++ )
    roll[i] = -102;

  for( reset_data* treset = reset; treset != NULL; treset = treset->next ) {
    gettimeofday( &reset_start, NULL );

    if( count < 0 || ( is_set( &treset->flags, RSFLAG_REROLL ) && ( is_set( &treset->flags, RSFLAG_MOB ) || treset->value == -2 ) ) )
      count = treset->count;
    if( count <= 0 && passes( treset, roll ) ) {
      if( is_set( &treset->flags, RSFLAG_MOB ) ) {
        reset_mob( treset, this, mob, leader );
      } else if( is_set( &treset->flags, RSFLAG_OBJECT ) ) {
        reset_obj( treset, this, mob, container );
      } else {
        rtable_calls = 0;
        reset_table( treset, (rtable_data*) treset->target, this, mob, container );
      }
    }
    
    treset->lag = stop_clock( reset_start );
  }

  gettimeofday( &room_resetted, NULL );

  /* OPEN, CLOSE DOORS */

  int reset_flags [] = { EX_RESET_CLOSED, EX_RESET_LOCKED, EX_RESET_OPEN };
  int status_flags [] = { EX_CLOSED, EX_LOCKED, EX_CLOSED };
  bool value [] = { true, true, false }; 

  for( int i = 0; i < exits; i++ ) {
    exit = exits[i];
//    if( is_set( &exit->exit_info, EX_ISDOOR ) ) {
      w1 = &exit->exit_info;
      if( ( back = reverse( exit ) ) != NULL ) {
        if( player_in_room( exit->to_room ) )
          continue;
        w2 = &back->exit_info;
      } else {
        w2 = w1;
      } 
      
      for( int j = 0; j < 3; j++ ) {
        if( is_set( w1, reset_flags[j] ) ) {
          assign_bit( w1, status_flags[j], value[j] );
          if( is_set( w2, reset_flags[j] ) )
            assign_bit( w2, status_flags[j], value[j] );
        }
      }
//    } 
  }
  
  set_bit( &room_flags, RFLAG_RESET0 );
  set_bit( &room_flags, RFLAG_RESET1 );
  set_bit( &room_flags, RFLAG_RESET2 );

  this->Set_Integer( HARVEST_ATTEMPT, 0 );

  gettimeofday( &room_flagged, NULL );

  // repop trigger
  for( action_data* taction = action; taction != NULL; taction = taction->next ) {
    if( taction->trigger == TRIGGER_REPOP ) {
      var_room = this;
      execute( taction );
    }
  }

  gettimeofday( &room_end, NULL );

  reset_lag = stop_clock( room_start );
  total_lag += reset_lag;
}    


/*
 *   OBJECT RESET FUNCTION
 */


obj_data* create( obj_clss_data* obj_clss, reset_data* reset, mob_data* mob )
{
  obj_data*    obj;
  int*     chances;

  obj     = create( obj_clss );
  chances = unpack_int( reset->chances );

  enchant_object( obj ); 
  set_alloy( obj, 10 );
  rust_object( obj, chances[3] );

  if( is_set( &obj_clss->size_flags, SFLAG_RANDOM ) ) 
    set_bit( &obj->size_flags, mob == NULL ? number_range( SFLAG_TINY, SFLAG_GIANT ) : wear_size( mob ) ); 

  if( obj_clss->item_type == ITEM_DRINK_CON || obj_clss->item_type == ITEM_FOUNTAIN )
    obj->value[2] = reset->liquid;

  return obj;
}    


void place( obj_data* obj, reset_data* reset, room_data* room, mob_data* mob, obj_data*& container )
{
  if( is_set( &reset->flags, RSFLAG_INSIDE ) ) {
    if( container == NULL ) {
      bug( "Reset_Obj: Inside flag with no container - %s %d", room != NULL ? "Room" : "Species", room != NULL ? room->vnum : mob->species->vnum );
      obj->Extract( );
      return;
    }
    obj->To( container );
  } else if( reset->value == -2 ) {
    if( room == NULL ) {
      bug( "Reset_Obj: Ground Object with NULL room - Species %d", mob->species->vnum );
      obj->Extract( );
      return;
    }
    obj->To( room );
    stop_events( obj, execute_decay );
  } else {
    if( mob == NULL ) {
      bug( "Reset_Obj: Wear loc with null mob - Room %d", room->vnum );
      obj->Extract( );
      return;
    }
    if( reset->value >= 0 ) {
      obj->position = reset->value; 

      if( is_set( &obj->size_flags, SFLAG_SIZE ) ) {
        for( int i = SFLAG_TINY; i < SFLAG_GIANT; i++ )
          remove_bit( &obj->size_flags, i );

        set_bit( &obj->size_flags, wear_size( mob ) );
      }

      obj->To( &mob->wearing );
    } else 
      obj->To( mob );
  }

  if( is_set( &reset->flags, RSFLAG_CONTAINER ) )
    container = obj;
}


void reset_obj( reset_data* reset, room_data* room, mob_data* mob, obj_data*& container )
{
  obj_clss_data*  obj_clss;
  obj_data*            obj;
  int*             chances = unpack_int( reset->chances );

  if( ( obj_clss = (obj_clss_data*) reset->target ) == NULL || ( room == NULL && is_set( obj_clss->extra_flags, OFLAG_BODY_PART ) ) )
    return;

  if( mob != NULL && mob->pShop != NULL && reset->value == -1 )
    return;

  obj = create( obj_clss, reset, mob );

  if( mob ) {
    obj->reset_mob_vnum = mob->species->vnum;
    if( is_set( mob->species->act_flags, ACT_APPROVED ) &&
      !is_set( obj->pIndexData->extra_flags, OFLAG_APPROVED ) )
      set_bit( obj->pIndexData->extra_flags, OFLAG_APPROVED );
  }
  if( room ) {
    obj->reset_room_vnum = room->vnum;
    if( room->area->status == AREA_OPEN &&
      !is_set( obj->pIndexData->extra_flags, OFLAG_APPROVED ))
      set_bit( obj->pIndexData->extra_flags, OFLAG_APPROVED );
  }

  for( int i = 0; i < 3; i++ )
    obj->reset_chances[i] = chances[i];

  place( obj, reset, room, mob, container );

  return;
}


/*
 *   RESET TABLE FUNCTION
 */


void reset_table( reset_data* base_reset, rtable_data *rtable, room_data* room, mob_data* mob, obj_data*& container )
{
  obj_clss_data*   obj_clss;
  obj_data*             obj;
  reset_data*         reset;
  int                  roll  [ 3 ];
  int                     i;

  if( rtable == NULL ) {
    bug( "Reset_Table: Invalid table." );
    return;
  }

  rtable_calls++;

  if( rtable_calls >= 100 ) {
    if( rtable_calls == 100 )
      bug( "Reset_Table: Infinite lookup chain (rtable #%d).", rtable ? rtable->vnum : -1 );
    return;
  }

  for( i = 0; i < 3; i++ )
    roll[i] = -102;

  for( reset = rtable->reset; reset != NULL; reset = reset->next ) 
    if( passes( reset, roll ) ) {
      if( is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        if( ( obj_clss = (obj_clss_data*) reset->target /* get_obj_index( reset->vnum ) */ ) == NULL ) 
          return;
        obj = create( obj_clss, reset, mob );
        place( obj, base_reset, room, mob, container );
      } else {
        reset_table( base_reset, (rtable_data*) reset->target, room, mob, container );
      }
    }

  return;
}

int reset_table_load( rtable_data *table )
{
  int         rtable_calls = 0;
  int                 roll [ 3 ];
  obj_clss_data*  obj_clss;
  reset_data*        reset;

  if( table == NULL ) {
    bug( "Reset_Table: Invalid Table (harvest)" );
    return 0;
  }

  rtable_calls++;

  if( rtable_calls >= 100 ) {
    if( rtable_calls == 100 )
      bug( "Reset table load infinite loop (#%d)", table->vnum );
    return 0;
  }

  for( int i = 0; i < 3; i++ )
    roll[i] = -102;

  for( reset = table->reset; reset != NULL; reset = reset->next ) {
    if( passes( reset, roll ) ) {
      if( is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        if( ( obj_clss = (obj_clss_data*) reset->target ) == NULL ) 
          return 0;
        return obj_clss->vnum;
      }
      else {
        reset_table_load( (rtable_data*) reset->target );
      }
    }
  }

  return 0;
}

  
/*
 *   MOB RESET FUNCTIONS
 */


void reset_mob( reset_data* reset, room_data* room, mob_data*& mob, char_data*& leader )
{
  species_data* species;

  if( ( is_set( &reset->flags, RSFLAG_NIGHT ) && isday( ) ) || ( is_set( &reset->flags, RSFLAG_DAY ) && !isday( ) ) )
    return;

  if( ( species = (species_data*) reset->target /* get_species( reset->vnum ) */ ) == NULL )
    return;

  mob = create_mobile( species, room );

  if(  is_set( &reset->flags, RSFLAG_SENTINEL ) || is_set( mob->species->act_flags, ACT_SENTINEL ) ) { 
    set_bit( &mob->status, STAT_SENTINEL );    
  } else {
    delay_wander( new event_data( execute_wander, mob ) );
  }

  if( is_set( &reset->flags, RSFLAG_AGGRESSIVE ) )
    set_bit( &mob->status, STAT_AGGR_ALL );    

  mob->To( room );
  
  mob->position = rspos_index[ reset->value ];
  mob->reset    = reset;

  mob_setup( mob, room );
  register_reset( mob, room );
  mreset_mob( mob );

  if( room->area->status == AREA_OPEN && !is_set( mob->species->act_flags, ACT_APPROVED ) )
    set_bit( mob->species->act_flags, ACT_APPROVED );

  if( leader != NULL && is_set( &reset->flags, RSFLAG_FOLLOWER ) )
    add_follower( mob, leader );

  if( is_set( &reset->flags, RSFLAG_LEADER ) )
    leader = mob;
}


void register_reset( char_data* mob, room_data* room )
{
  reset_data*   mark = room->reset;
  reset_data*  reset;

  for( reset = room->reset; ; reset = reset->next ) {
    if( is_set( &reset->flags, RSFLAG_REROLL ) )  
      mark = reset;
    if( reset == mob->reset )
      break;
  }

  mark->count++;
  mob->reset = mark;
}


void unregister_reset( char_data* mob )
{
  if( mob->reset != NULL ) {
    mob->reset->count--;
    mob->reset = NULL;
  }
} 


void mob_setup( mob_data* mob, room_data* room )
{
  shop_data*        shop;

  if( room->is_dark( ) )
    set_bit( mob->affected_by, AFF_INFRARED );

  for( shop = shop_list; shop != NULL; shop = shop->next ) 
    if( shop->room == room && shop->keeper == mob->species->vnum )
      mob->pShop = shop;

  set_trainer( mob, room );
}


/*
 *   MRESET FUNCTIONS
 */


void mreset_mob( mob_data* mob )
{
  reset_data*      reset;
  obj_data*    container  = NULL;
  int               roll  [ 3 ];
  int                  i;

  for( i = 0; i < 3; i++ )
    roll[i] = -102;

  for( reset = mob->species->reset; reset != NULL; reset = reset->next ) 
    if( passes( reset, roll ) ) {
      if( is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        reset_obj( reset, NULL, mob, container );
      } else {
        rtable_calls = 0;
        reset_table( reset, (rtable_data*) reset->target, NULL, mob, container );
      }
    }

  return;
}


thing_array* get_skin_list( species_data* species, char_data* ch )
{
  obj_data*            obj;
  obj_clss_data*  obj_clss  = NULL;
  thing_array*        list  = NULL;
  reset_data*        reset;
  int                 roll  [ 3 ];

  for( int i = 0; i < 3; i++ )
    roll[i] = -102;

  for( reset = species->reset; reset != NULL; reset = reset->next )
    if( is_set( &reset->flags, RSFLAG_OBJECT ) && ( obj_clss = (obj_clss_data*) reset->target ) != NULL && is_set( obj_clss->extra_flags, OFLAG_BODY_PART ) )
      break;

  if( reset == NULL )
    return (thing_array*) -1; 

  for( reset = species->reset; reset != NULL; reset = reset->next ) {
    if( !is_set( &reset->flags, RSFLAG_OBJECT ) || !passes( reset, roll, ch ) || ( obj_clss = (obj_clss_data*) reset->target ) == NULL || !is_set( obj_clss->extra_flags, OFLAG_BODY_PART ) )
      continue;
    if( list == NULL )
      list = new thing_array;
    obj = create( obj_clss );
    *list += obj;
  }

  return list;
}


/*
 *   SHOP RESET FUNCTION
 */


void reset_shop( mob_data* ch )
{
  reset_data*  reset;
  obj_data*      obj;
  int           roll  [ 3 ];

  if (!ch || !ch->Is_Valid()) {
    roach("reset_shop: NULL mob!");
    return;
  }

  for( int i = 0; i < 3; i++ )
    roll[i] = -102;

  for( reset = ch->reset->next; reset != NULL; reset = reset->next ) {
    if( !is_set( &reset->flags, RSFLAG_OBJECT ) || reset->value == -2 )
      break;
  
    if( passes( reset, roll ) && reset->value == -1 ) {
      obj_clss_data *clss = (obj_clss_data*) reset->target;  /* get_obj_index( reset->vnum ) */
      if (!clss) {
        roach("reset_shop: NULL object class on mob %d", ch->species->vnum);
        continue;
      }

      obj = create( clss );
      if (!obj || !obj->Is_Valid()) {
        roach("reset_shop: NULL object %d on mob %d", clss->vnum, ch->species->vnum);
        continue;
      }

      set_bit( obj->extra_flags, OFLAG_IDENTIFIED );    
      set_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );
      if( obj->pIndexData->item_type == ITEM_DRINK_CON )
        obj->value[2] = reset->liquid; 
      obj->To( ch );
      consolidate( obj );
    }
  }
}


/*
 *   DISK ROUTINES
 */


void load( FILE* fp, reset_data*& list, bool translate_mob = false)
{
  reset_data*   reset;
  int               i;

  for( ; ; ) {
    if( ( i = fread_number( fp ) ) == -1 )
      break;

    reset = new reset_data;

    reset->flags    = fread_number( fp );
    reset->chances  = fread_number( fp );

    if( is_set( &reset->flags, RSFLAG_OBJECT ) )
      reset->target = get_obj_index( i );
    else if ( is_set( &reset->flags, RSFLAG_MOB ) )
      reset->target = get_species( i );
    else
      reset->target = find_rtable( i );

    if (translate_mob) {
      int value = fread_number(fp);
      reset->value = old_mresets[value + 2];
    } else {
      reset->value = fread_number( fp );
    }

    reset->liquid = fread_number( fp );

    if( reset->target != NULL )
      append( list, reset );
    else
      log( "    - deleting reset with NULL target" );
  }

  return;
}


void write( FILE* fp, reset_data* reset )
{
  for( ; reset != NULL; reset = reset->next ) {
    int vnum;

    if( reset->target && is_set( &reset->flags, RSFLAG_OBJECT ) )
      vnum = ((obj_clss_data*) reset->target)->vnum;
    else if( reset->target && is_set( &reset->flags, RSFLAG_MOB ) )
      vnum = ((species_data*) reset->target)->vnum;
    else if( reset->target )
      vnum = ((rtable_data*) reset->target)->vnum;
    else
      continue;

    fprintf( fp, "%d %d %d %d %d\n", vnum, reset->flags, reset->chances, reset->value, reset->liquid );
  }

  fprintf( fp, "-1\n" );
}

/* 
 *   ROWHERE ROUTINES
 */


void do_rowhere( char_data* ch, char* argument )
{
  char                  tmp  [ TWO_LINES ];
  const char*          name;
  bool                first  = TRUE;
  int                 flags;
  obj_clss_data*   obj_clss;

  if( !get_flags( ch, argument, &flags, "p", "Rowhere" ) )
    return;

  if( *argument == '\0' ) {
    send( ch, "Syntax: rowhere <object>\r\n" );
    return;
  } else if( isnumber( argument ) ) {
    obj_clss = get_obj_index( atoi( argument ) );
    if( obj_clss == NULL ) {
      send( ch, "No object has that vnum." );
      return;
    }

    page( ch, "\r\n" );
    sprintf( tmp, "--- %s (%d) ---", obj_clss->Name(), obj_clss->vnum );
    tmp[4] = toupper( tmp[4] );
    page_centered( ch, tmp ); 
    page( ch, "\r\n" );

    bool found = generic_search( ch, obj_clss, obj_clss->vnum, OBJECT, is_set( &flags, 0 ) );

    if( !found )
      page_centered( ch, "  None found" );

    return;
  }

  int hits = 0;
  for( int index = 0; index < MAX_OBJ_INDEX; index++ ) {
    if( ( obj_clss = obj_index_list[index] ) == NULL )
      continue; 
 
    name = obj_clss->Name( );

    if( !is_name( argument, name ) )
      continue;

    page( ch, "\r\n" );
    sprintf( tmp, "--- %s (%d) ---", name, index );
    tmp[4] = toupper( tmp[4] );
    page_centered( ch, tmp ); 
    page( ch, "\r\n" );

    first = FALSE;
    bool found = generic_search( ch, obj_clss, obj_clss->vnum, OBJECT, is_set( &flags, 0 ) );

    if( !found )
      page_centered( ch, "  None found" );

    if( hits++ > 10 ) {
      page_centered( ch, "**** Too many hits.  Please refine search. ****" );
      return;
    }
  }
  
  if( first )
    send( ch, "Nothing like that in hell, earth, or heaven.\r\n" );
}


void do_rmwhere( char_data* ch, char* argument )
{
  char                  tmp  [ TWO_LINES ];
  bool                first = TRUE;
  int                 flags;
  species_data*     species;

  if( !get_flags( ch, argument, &flags, "p", "Rmwhere" ) )
    return;

  if( *argument == '\0' ) {
    send( ch, "Syntax: rmwhere <mob>\r\n" );
    return;
  } else if( isnumber( argument ) ) {
    species = get_species( atoi( argument ) );
    if( species == NULL ) {
      send( ch, "No mob has that vnum." );
      return;
    }

    page( ch, "\r\n" );
    sprintf( tmp, "--- %s (%d) ---", species->Name(), species->vnum );
    tmp[4] = toupper( tmp[4] );
    page_centered( ch, tmp ); 
    page( ch, "\r\n" );

    bool found = generic_search( ch, species, species->vnum, CHARACTER, is_set( &flags, 0 ) );

    if( !found )
      page_centered( ch, "  None found" );

    return;
  }

  int hits = 0;
  for( int index = 0; index < MAX_SPECIES; index++ ) {
    if( ( species = species_list[index] ) == NULL )
      continue; 
 
    const char* name = species->Name( );

    if( !is_name( argument, name ) )
      continue;

    page( ch, "\r\n" );
    sprintf( tmp, "--- %s (%d) ---", name, index );
    tmp[4] = toupper( tmp[4] );
    page_centered( ch, tmp ); 
    page( ch, "\r\n" );

    first = FALSE;
    bool found = generic_search( ch, species, species->vnum, CHARACTER, is_set( &flags, 0 ) );

    if( !found )
      page_centered( ch, "  None found" );

    if( hits++ > 10 ) {
      page_centered( ch, "**** Too many hits.  Please refine search. ****" );
      return;
    }
  }
  
  if( first )
    send( ch, "Nothing like that in hell, earth, or heaven.\r\n" );
}


