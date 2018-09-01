#include "system.h"


extern bool tfe_down;     // defined in tfe.cc - causes main loop to exit

const char* imm_title [] = { "Avatar", "Apprentice", "Builder",
  "Architect", "Immortal", "Spirit", "Angel", "Demigod", "God" };

const char* permission_name [ MAX_PERMISSION+1 ] = {
  "none", "all.mobs", "all.objects", "all.rooms", "approve",
  "basic", "build.chan", "commands", "echo", "god.chan",
  "goto", "help.files", "imm.chan",
  "lists", "load.objects", "misc.tables", "mobs", "noteboard",
  "objects", "players", "quests",
  "reimb_exp", "reimb.equip", "rooms", "shutdown", "site.names",
  "snoop", "spells",
  "socials", "transfer", "unfinished", "write.all", "write.areas",
  "avatar.Chan", "clans", "rtables", "disabled", "force.players",
  "object.approval", "mob.approval", "room.approval" };


void   monk_return   ( char_data* );
void   restore       ( char_data*, char_data* );


/*
 *   PERMISSION ROUTINES
 */


bool mortal( char_data* ch, char* command )
{
  if( ch->shdata->level >= LEVEL_APPRENTICE ) 
    return FALSE;

  send( ch, "To prevent abuse you are unable to use %s in mortal form.\r\n", command );

  return TRUE;
}


bool has_permission( char_data* ch, int* flag, bool msg )
{
  wizard_data* imm;
  int               i;

  if( flag[0] == 0 && flag[1] == 0 )
    return TRUE;

  if( ch == NULL || ( imm = wizard( ch ) ) == NULL )
    return FALSE;

  if( is_god( ch ) )
    return TRUE;

  for( i = 0; i < 2; i++ )
    if( ( imm->permission[i] & flag[i] ) != 0 )
      return TRUE;

  if( msg ) 
    send( ch, "Permission denied.\r\n." );

  return FALSE;
}


/*
 *   FIND LOCATION
 */


room_data* find_location( char_data* ch, char* argument )
{
  char_data* victim;

  if( is_number( argument ) )
    return get_room_index( atoi( argument ), FALSE );

  if( ( victim = one_character( ch, argument, "goto", (thing_array*) &player_list, (thing_array*) &mob_list ) ) != NULL )
    return victim->in_room;

  return NULL;
}


/*
 *   DEBUG
 */

#include "dungeon.h"

void do_debug( char_data* ch, char* argument )
{
  // this function is defined for testing purposes only
  if( !is_immortal( ch ) )
    return;

  if( !MySQL.IsConnected( ) ) {
    send( ch, "Not connected to MySQL\r\n" );
    return;
  }

  if( !*argument )
    return;

  for( char* blah = argument; *blah; blah++ )
    if( *blah == '|' )
      *blah = '\n';

  if( !MySQL.ExecuteSQL( argument ) )
    send( ch, "Error in SQL statement.\r\n" );

  send( ch, "SQL Finished\r\n" );

//  send( ch, "Creating dungeon at yer feet..." );
//  new TDungeon( ch->in_room, 10000 );
}


/*
 *   TRUST
 */

 
void do_trust( char_data *ch, char *argument )
{
  char           arg  [ MAX_INPUT_LENGTH ];
  char_data*  victim;
  int          level;
//  wizard_data*   imm = wizard( ch );

  argument = one_argument( argument, arg );

  if( *arg == '\0' || *argument == '\0' || !is_number( argument ) ) {
    send( ch, "Syntax: trust <char> <level>.\r\n" );
    return;
  }

  if( ( victim = one_player( ch, arg, "trust", (thing_array*) &player_list ) ) == NULL )
    return;

  if( victim->pcdata == NULL ) {
    send( ch, "%s is not a player.\r\n", victim );
    return;
  } 

  if( ( level = atoi( argument ) ) < 0 || level > MAX_LEVEL ) {
    send( "Level must be 0 (reset) or 1 to 99.\r\n", ch );
    return;
  }

  if( level > get_trust( ch ) ) {
    send( ch, "Limited to your trust.\r\n" );
    return;
  }

  victim->pcdata->trust = level;
  send( victim, "You are now trusted at level %d.\r\n", level );

  if( ch != victim ) 
    send( ch, "%s is now trusted at level %d.\r\n", victim->real_name( ), level );

  modify_pfile( victim );

  return;
}


/*
 *   REIMB FUNCTION
 */

inline void reimb_array( char_data* ch, thing_array* array, const char* method )
{
  thing_data* thing;
  int items = 0;

  for( int i = array->size - 1; i >= 0; i-- ) {
    thing = array->list[ i ];
    thing->From( thing->number );
    thing->To( ch );
    items++;
  }

  send( ch, "  %d items recovered from %s.\r\n", items, method );
}


void do_reimburse( char_data* ch, char* argument )
{
  char_data*     victim;
  link_data        link;
  char*             dir;
  int             flags;

  if( mortal( ch, "reimburse" ) )
    return;

  if( !get_flags( ch, argument, &flags, "deibprqcs", "reimburse" ) )
    return;

  if( ( victim = one_player( ch, argument, "reimburse", ch->array ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "You can't reimburse yourself.\r\n" );
    return;
  }

  if( flags == 0 ) {
    send( ch, "You must specify what to reimburse, see help.\r\n" );
    return;
  }

  if( is_set( &flags, 0 ) ) {
    int exp = death_exp( victim->shdata->level );
    if( victim->pcdata->max_level != -1 )
      exp = death_exp( victim->shdata->level + 1 );

    send( ch, "You reimburse %s for %d exp and one death.\r\n", victim, exp );
    send( victim, "%s reimburses you for %d exp and one death.\r\n", ch, exp );

    char* tmp = static_string( );
    sprintf( tmp, "Reimbursed 1 death, %d exp by %s.", exp, ch->real_name( ) );
    player_log( victim, tmp );

    victim->shdata->deaths--;
    add_exp( victim, exp );
    return;
  }

  int methods = 0;
  for( int ai = 1; ai <= 4; ai ++ )
    if( is_set( &flags, ai ) )
      methods++;

  if( methods == 0 ) {
    send( ch, "You must specify at least one type of object store to search.\r\n" );
    return;
  }

  int dirs = 0;
  for( int di = 5; di <= 8; di ++ )
    if( is_set( &flags, di ) )
      dirs++;

  if( dirs == 0 ) {
    send( ch, "You must specify at least one directory to search.\r\n" );
    return;
  }

  send( ch, "You scan the annals of history ...\r\n" );
  send( victim, "You feel %s probing your history ...\r\n", ch );

  for( int di = 5; di <= 8; di++ ) {
    if( !is_set( &flags, di ) )
      continue;

    switch( di ) {
    case 5:
      dir = PLAYER_RESTORE_DIR;
      break;
    case 6:
      dir = PLAYER_QUIT_DIR;
      break;
    case 7:
      dir = PLAYER_DEATH_DIR;
      break;
    case 8:
      dir = PLAYER_PREV_DIR;
      break;
    default:
      dir = NULL;
      break;
    }

    if( dir == NULL )
      continue;

    send( ch, "\r\nFile %s%s:\r\n", dir, victim->descr->name );
  
    if( !load_char( &link, victim->descr->name, dir ) ) {
      send( ch, "    no backup found.\r\n" );
      continue;
    }

    for( int ai = 1; ai <= 4; ai ++ ) {
      if( !is_set( &flags, ai ) )
        continue;

      switch( ai ) {
      case 1:
        reimb_array( ch, &link.player->wearing, "equipment" );
        break;

      case 2:
        reimb_array( ch, &link.player->contents, "inventory" );
        break;

      case 3:
        reimb_array( ch, &link.player->locker, "bank" );
        break;

      case 4:
        if( !is_empty( link.player->followers ) ) {
          for( int pi = 0; pi < link.player->followers.size; pi++ ) {
            char_data* pet = link.player->followers[ pi ];
            if( !pet )
              continue;

            char* tmp = static_string( );
            sprintf( tmp, "%s equipment", pet->Name( ) );
            reimb_array( ch, &pet->wearing, tmp );
            sprintf( tmp, "%s inventory", pet->Name( ) );
            reimb_array( ch, &pet->contents, tmp );
          }
        } else {
          send( ch, "  no pets found.\r\n" );
        }
        break;

      default:
        send( ch, "  unknown object store.\r\n" );
        break;
      };
    }

    link.character->Extract( );
  }
}


/*
 *   ROUTINES TO DEAL WITH MALCONTENTS
 */


void do_deny( char_data *ch, char *argument )
{
  char_data*  victim;

  if( ( victim = one_player( ch, argument, "deny", (thing_array*) &player_list ) ) == NULL )
    return;

  if( get_trust( victim ) >= get_trust( ch ) ) {
    send( ch, "You failed.\r\n" );
    return;
  }

  set_bit( victim->pcdata->pfile->flags, PLR_DENY );

  send( victim, "You have earned the displeasure of the gods.\r\n" );
  send( victim, "Your character is now denyed access.\r\n" );
  send( victim, "Good Bye.\r\n" );

  send( ch, "Ok.\r\n" );

  char tmp[ THREE_LINES ];
  sprintf( tmp, "%s banned by %s.", victim->Name( ), ch->Name( ) );
  bug( BUG_SYSLOG, tmp, get_trust( ch ) );

  do_quit( victim, "" );

  return;
}


void do_disconnect( char_data *ch, char *argument )
{
  char_data *victim;

  if( ( victim = one_player( ch, argument, "disconnect", (thing_array*) &player_list ) ) == NULL )
    return;

  if( get_trust( victim ) >= get_trust( ch ) ) {
    send( ch, "You can only disconnect those of lower level.\r\n" );
    return;
  }
    
  if( victim->link == NULL ) {
    send( ch, "%s doesn't have a descriptor.\r\n", victim );
    return;
  }

  close_socket( victim->link );
  send( ch, "Ok.\r\n" );

  char tmp[ THREE_LINES ];
  sprintf( tmp, "%s disconnected by %s.", victim->Name( ), ch->Name( ) );
  bug( BUG_SYSLOG, tmp, get_trust( ch ) );
}


void do_imprison( char_data *ch, char *argument )
{
  char_data *victim;
     
  if( ( victim = one_player( ch, argument, "imprison", (thing_array*) &player_list ) ) == NULL )
    return;

  if( victim->in_room == NULL ) {
    send( ch, "They are in limbo.\r\n" );
    return;
  }

  if( get_trust( victim ) >= get_trust( ch ) ) {
    send( ch, "You can't imprison someone of equal or higher level.\r\n" );
    return;
  }

  send_seen( victim, "%s is mercilessly vaporized.\r\n", victim );

  victim->From( );
  room_data* room = get_town( "#prison", true );
  victim->To( room ? room : get_room_index( ROOM_CONSTRUCT ) );

  set_bit( victim->pcdata->pfile->flags, PLR_FREEZE );

  send_seen( victim, "%s joins you in your pennance.", victim );
  send( victim, "You have been imprisoned!\r\n" );
  show_room( victim, victim->in_room, FALSE, FALSE );
  send( ch, "%s imprisoned.\r\n", victim );

  char tmp[ THREE_LINES ];
  sprintf( tmp, "%s imprisoned by %s.", victim->Name( ), ch->Name( ) );
  bug( BUG_SYSLOG, tmp, get_trust( ch ) );
}


void do_pardon( char_data* ch, char* argument )
{
  /*
  char_data *victim;
  int i;

  if( ( victim = one_player( ch, argument, "pardon", (thing_array*) &player_list ) ) == NULL )
    return;

  if( get_trust( victim ) > get_trust( ch ) ) {
    send( "You can only pardon those of lower or equal level.\r\n", ch );
    return;
  }
    
  if( victim->species != NULL ) {
    send( ch, "Mobs don't need pardoning!\r\n" );
    return;
  }

  for( i = 0; i < MAX_ENTRY_NATION; i++ )
    victim->pcdata->pfile->reputation[i] = 500;

  if( ch != victim )
    send( victim, "%s has pardoned you.\r\n", ch );
  
  send( ch, "Ok.\r\n" );
  */
}


void do_freeze( char_data *ch, char *argument )
{
  player_data*  victim;

  if( ( victim = one_player( ch, argument, "freeze", (thing_array*) &player_list ) ) == NULL )
    return;

  if( get_trust( ch ) <= get_trust( victim ) ) {
    send( ch, "You failed.\r\n" );
    return;
  }

  switch_bit( victim->pcdata->pfile->flags, PLR_FREEZE );

  char tmp[THREE_LINES];
  if( is_set( victim->pcdata->pfile->flags, PLR_FREEZE ) ) {
    send( victim, "You can't do ANYthing!\r\n" );
    send( ch, "FREEZE set.\r\n" );
    sprintf(tmp, "%s frozen by %s.", victim->Name(), ch->Name());
  } else {
    send( victim, "You can play again.\r\n" );
    send( ch, "FREEZE removed.\r\n" );
    sprintf(tmp, "%s thawed by %s.", victim->Name(), ch->Name());
  }

  write( victim );

  bug( BUG_SYSLOG, tmp, get_trust( ch ) );
}


void do_peace( char_data* ch, char* )
{
  char_data *rch;

  if( mortal( ch, "peace" ) )
    return;

  send( ch, "You wave your hand in a gesture for silence.\r\n" );
  send( *ch->array, "%s waves %s hand in a gesture for silence.\r\n", ch, ch->His_Her( ) );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL ) {   
      rch->fighting = NULL;
      clear( rch->aggressive );
      stop_combat_events( rch );
      stop_events( rch, execute_drown );
      disrupt_spell( rch );
    }
  }
}


/*
 *   BAMFIN/BAMFOUT
 */


inline const char* bamfin( wizard_data* wizard )
{
  return wizard->bamfin != empty_string ? wizard->bamfin : "materializes from the ether.";
}


inline const char* bamfout( wizard_data* imm )
{
  return imm->bamfout != empty_string ? imm->bamfout : "disappears into the ether.";
}
  

void do_bamfin( char_data* ch, char* argument )
{
  char tmp[ THREE_LINES ];
  wizard_data* imm  = wizard( ch );

  if( imm == NULL )
    return;

  free_string( imm->bamfin, MEM_PLAYER );
  imm->bamfin = alloc_string( argument, MEM_PLAYER );

  send( ch, "Bamfin message set to:\r\n" );
  convert_to_ansi( ch, bamfin(imm), tmp, sizeof(tmp) );
  send( ch, "%s %s\r\n", ch->descr->name, tmp );

  return;
}


void do_bamfout( char_data *ch, char *argument )
{
  char tmp[ THREE_LINES ];
  wizard_data* imm  = wizard( ch );

  if( imm == NULL )
    return;

  free_string( imm->bamfout, MEM_PLAYER );
  imm->bamfout = alloc_string( argument, MEM_PLAYER );

  send( ch, "Bamfout message set to:\r\n" );
  convert_to_ansi( ch, bamfout(imm), tmp, sizeof(tmp) );
  send( ch, "%s %s\r\n", ch->descr->name, tmp );
   
  return;
}


/*
 *   GOTO
 */


void exec_goto( char_data* victim, room_data* room, wizard_data* imm )
{
  char tmp[ THREE_LINES ];

  //convert_to_ansi( victim, bamfout(imm), tmp, sizeof(tmp) );
  //send_seen( victim, "%s %s\r\n", victim, tmp );
  char_data* rch = 0;
  int i = 0;
  if( victim->array != NULL ) {
    for( i = 0; i < victim->array->size; i++ ) {
      rch = character( victim->array->list[i] );
      if( rch != NULL && rch != victim && victim->Seen( rch ) ) {
        convert_to_ansi( rch, bamfout(imm), tmp, sizeof(tmp) );
        send( rch, "%s %s\r\n", victim, tmp );
      }
    }
  }

  victim->From( );
  victim->To( room );

  //convert_to_ansi( victim, bamfin(imm), tmp, sizeof(tmp) );
  //send_seen( victim, "%s %s\r\n", victim, tmp );
  if( victim->array != NULL ) {
    for( i = 0; i < victim->array->size; i++ ) {
      rch = character( victim->array->list[i] );
      if( rch != NULL && rch != victim && victim->Seen( rch ) ) {
        convert_to_ansi( rch, bamfin(imm), tmp, sizeof(tmp) );
        send( rch, "%s %s\r\n", victim, tmp );
      }
    }
  }

  show_room( victim, room, TRUE, TRUE );
  return;
}


void do_goto( char_data* ch, char* argument )
{
  wizard_data*      imm  = wizard( ch );
  char_data*        rch;
  room_data*         to;
  room_data*       from;

  if( *argument == '\0' ) {
    send( ch, "Usage: goto <location>\r\n" );
    return;
  }
 
  if( ( to = find_location( ch, argument ) ) == NULL ) 
    return;
  
  if( ( from = ch->in_room ) == to ) {
    send( ch, "You are already there!\r\n" );
    return;
  }

  if( is_set( &to->room_flags, RFLAG_OFFICE ) && !is_god( ch ) && to->vnum != imm->office ) {
    send( ch, "That location belongs to another god.\r\n" );
    return;
  } 

  exec_goto( ch, to, imm );

  for( int i = from->contents-1; i >= 0; i-- ) 
    if( ( rch = character( from->contents[i] ) ) != NULL
      && rch->leader == ch && is_set( &rch->status, STAT_PET ) )
      exec_goto( rch, to, imm );

  return;
}


/*
 *   TRANSFER
 */


void transfer( char_data* ch, char_data* victim, room_data* room )
{
  char              tmp  [ TWO_LINES ];
  content_array*  where  = victim->array;
  char_data*        rch;

  send_seen( victim, "%s disappears in a mushroom cloud.\r\n", victim );
  victim->From( );
  victim->To( room );
  send_seen( victim, "%s arrives from a puff of smoke.\r\n", victim );

  sprintf( tmp, "** %s has transfered you. **\r\n\r\n", ch->Name( victim ) );
  tmp[3] = toupper( tmp[3] );
  send( victim, tmp );
  show_room( victim, room, FALSE, FALSE );

  for( int i = *where-1; i >= 0; i-- ) 
    if( ( rch = character( where->list[i] ) ) != NULL
      && rch->leader == victim && is_set( &rch->status, STAT_PET ) )
      transfer( ch, rch, room );

  return;
}


void do_transfer( char_data* ch, char* argument )
{
  char              arg  [ MAX_INPUT_LENGTH ];
  char_data*     victim;
  player_data*   player;
  room_data*       room;

  if( mortal( ch, "transfer" ) )
    return;

  argument = one_argument( argument, arg );

  if( *arg == '\0' ) {
    send( ch, "Syntax: transfer <char> [room]\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    room = ch->in_room;
  } else {
    if( ( room = find_location( ch, argument ) ) == NULL ) {
      send( ch, "No such location.\r\n" );
      return;
    }
  }

  if( !strcasecmp( arg, "all" ) ) {
    for( int i = 0; i < player_list; i++ ) {
      player = player_list[i];
      if( player->In_Game( )
        && player->shdata->level < LEVEL_APPRENTICE
        && player->in_room != room )
        transfer( ch, player, room );
    }
    send( ch, "All players transfered.\r\n" );
    return;
  }

  if( ( victim = one_character( ch, arg, "transfer", ch->array, (thing_array*) &player_list, (thing_array*) &mob_list ) ) == NULL )
    return;

  if( victim->in_room == NULL ) {
    send( ch, "They are in limbo.\r\n" );
    return;
  }

  if( victim == ch ) {
    send( ch, "You can't transfer yourself!\r\n" );
    return;
  }

  transfer( ch, victim, room );
 
  send( ch, "Ok.\r\n" );
}


/*
 *   ECHO COMMANDS
 */


void echo( const char* string )
{
  int            n  = strlen( string );

  log( string );

  link_nav links( &link_list );
  for( link_data* link = links.first(); link != NULL; link = links.next() )
    socket_write( link->channel, string, n );
}


void do_gecho( char_data *ch, char *argument )
{
  char        tmp[ THREE_LINES ];

  if( mortal( ch, "gecho" ) )
    return;

  if( *argument == '\0' ) {
    send( ch, "Usage: gecho <text>\r\n" );
    return;
  }

  link_nav links( &link_list );
  for( link_data* link = links.first(); link != NULL; link = links.next() )
    if( link->connected == CON_PLAYING ) {
      convert_to_ansi( link->character, argument, tmp, sizeof(tmp) );
      fsend( link->character, tmp );
    }
}


void do_recho( char_data* ch, char* argument )
{
  char_data*  rch;
  char        tmp[ THREE_LINES ];
    
  if( mortal( ch, "recho" ) )
    return;

  if( *argument == '\0' ) {
    send( ch, "Usage: recho <text>\r\n" );
    return;
  }

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL ) {
      //fsend( rch, argument );       // old code
      convert_to_ansi( rch, argument, tmp, sizeof(tmp) );
      fsend( rch, tmp );
    }
  }
}


void do_beep( char_data* ch, char* argument )
{
  player_data*  victim;

  if( ( victim = one_player( ch, argument, "sset", (thing_array*) &player_list ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "\7You beep yourself.\7\r\n" );
  } else {
    send( victim, "\7%s has beeped you.\7\r\n", ch );
    send( ch, "Ok.\r\n" );
  }
}


void do_where( char_data* ch, char* )
{
  char            buf  [ MAX_STRING_LENGTH ];
  char_data*   victim = NULL;
  bool          found  = FALSE;

  send( "Players:\r\n", ch );

  link_nav links( &link_list );
  for( link_data* link = links.first(); link != NULL; link = links.next() ) {
    if( link->connected == CON_PLAYING ) {
      victim = link->character;
      if( victim != NULL && !IS_NPC( victim ) && victim->in_room != NULL && victim->Seen( ch ) ) {
        found = TRUE;
        sprintf( buf, "%-20s #%d: %s\r\n", victim->real_name(), victim->in_room->vnum, victim->in_room->name );
        send( buf, ch );
      }
    }
  }

  if( !found )
    send( "None\r\n", ch );

  return;
}


void do_purge( char_data* ch, char* argument )
{
  thing_data*    thing;
  char_data*    victim;
  obj_data*        obj;

  if( mortal( ch, "purge" ) )
    return;

  if( *argument == '\0' || !strcmp( argument, "all" ) ) {
    fsend( *ch->array, "%s utters an ancient rune, engulfing the room in a blinding light.", ch );
    send( ch, "Ok.\r\n" );

    for( int i = *ch->array-1; i >= 0; i-- ) {
      victim = character( ch->array->list[ i ] );
      obj = object( ch->array->list[ i ] );
      if( ( victim && victim->pcdata == NULL && !is_set( &victim->status, STAT_PET ) ) || ( obj && ( obj->pIndexData->vnum != OBJ_CORPSE_PC || *argument ) ) )
        ch->array->list[i]->Extract( );
    }

    return;
  }

  if( ( thing = one_thing( THING_DATA, ch, argument, "purge", ch->array ) ) == NULL )
    return;
  
  if( player( thing ) != NULL ) {
    send( ch, "%s is a player and you can't purge players.\r\n", thing );
    return;
  }

  send( ch, "Ok.\r\n" );
  send( *ch->array, "%s purges %s.\r\n", ch, thing );

  thing->Extract( );
}


void do_advance( char_data* ch, char* argument )
{
  char           arg  [ MAX_INPUT_LENGTH ];
  char_data*  victim;
  int          level;
  int              i;

  argument = one_argument( argument, arg );

  if( *arg == '\0' || *argument == '\0' || !is_number( argument ) ) {
    send( ch, "Usage: advance <char> <level>.\r\n" );
    return;
  }
/*
  if( ( victim = one_player( ch, arg, "advance", (thing_array*) &player_list ) ) == NULL ) 
    return;
*/
  if( ( victim = one_character( ch, arg, "advance", ch->array, (thing_array*) &player_list) ) == NULL )
    return;
  
  if( ( level = atoi( argument ) ) <= 0 ) {
    send( ch, "Advancing them to a non-positive level does not make sense.\r\n" );
    return;
  }

  if( ch != victim && ( !is_pet( victim ) || !is_immortal( ch ) )) {
    if( !is_demigod( ch ) ) {
      send( "You can not advance others.\r\n", ch );
      return;
    } 
    if( !is_god( ch ) && level > 97 ) {
      send( "You can only advance others to 97.\r\n", ch );
      return;
    }
  }

  if( level > get_trust( ch ) ) {
    send( "You can't advance higher than your trust.\r\n", ch );
    return;
  }

  if( !is_pet( victim ) ) {
    if( level < victim->shdata->level ) {
      victim->shdata->level  = 1;
      victim->exp            = 1000;
      victim->base_hit       = 20;
      victim->base_mana      = 100;
      victim->base_move      = 100;

      update_maxes( ch );

      victim->hit      = victim->max_hit;
      victim->mana     = victim->max_mana;
      victim->move     = victim->max_move;

      for( i = 0; i < MAX_SKILL; i++ ) 
        victim->pcdata->skill[i] = UNLEARNT;
      victim->pcdata->skill[LANG_COMMON] = 10;
    }
  }
  
  victim->exp = 0;

  for( i = victim->shdata->level ; i < level; i++ ) 
    advance_level( victim, FALSE );

  if( ch != victim ) {
    send( ch, "You have set %s to level %d.\r\n", victim->real_name( ), level );
    send( victim, "%s has set you to level %d.\r\n", ch->real_name( ), level );
  } else 
    send( ch, "You are now level %d.\r\n", level );

}


/*
 *   RESTORE
 */
 

void do_restore( char_data* ch, char* argument )
{
  char_data* victim;

  if( mortal( ch, "restore" ) ) 
    return;

  if( *argument == '\0' ) {
    send( ch, "Usage: restore <char>\r\n" );
    return;
  }

  if( !strcasecmp( argument, "all" ) ) {
    if( !is_angel( ch ) ) {
      send( ch, "You must be level 97 to restore all.\r\n" );
      return;
    }
    for( int i = 0; i < player_list; i++ ) {
      victim = player_list[i];
      if( victim->In_Game( )
        && victim->shdata->level < LEVEL_APPRENTICE )
        restore( victim, ch );
    }
    send( ch, "All players restored.\r\n" );

    char tmp[THREE_LINES];
    sprintf(tmp, "All players restored by %s.", ch->Name());
    bug( BUG_SYSLOG, tmp, get_trust( ch ) );
    return;
  }
    
  if( ( victim = one_character( ch, argument, "restore", ch->array, (thing_array*) &player_list ) ) == NULL )
    return;

  if( !is_immortal( ch ) && ch != victim ) {
    send( ch, "You can only restore yourself.\r\n" );
    return;
  }
  restore( victim, ch );
 
  if( ch == victim ) 
    send( ch, "You restore yourself.\r\n" );
  else {
    send( ch, "%s restored.\r\n", victim );

    char tmp[THREE_LINES];
    sprintf(tmp, "%s restored by %s.", victim->Name(), ch->Name());
    bug( BUG_SYSLOG, tmp, get_trust( ch ) );
  }

  return;
}


void restore( char_data* victim, char_data* ch )
{
  strip_affect( victim, AFF_POISON );
  strip_affect( victim, AFF_HALLUCINATE );
  strip_affect( victim, AFF_CONFUSED );
  strip_affect( victim, AFF_BLIND );
  strip_affect( victim, AFF_SILENCE );
  strip_affect( victim, AFF_PLAGUE );
  strip_affect( victim, AFF_TOMB_ROT );
  strip_affect( victim, AFF_RABIES );
  strip_affect( victim, AFF_DEATH );
  strip_affect( victim, AFF_SLEEP );
  strip_affect( victim, AFF_BLIGHT );

  update_maxes( victim );

  victim->hit  = victim->max_hit;
  victim->mana = victim->max_mana;
  
  update_max_move( victim );
  victim->move = victim->max_move;

  gain_condition( victim, COND_FULL, 999 );
  gain_condition( victim, COND_THIRST, 999 );

  update_pos( victim );

  if( ch != victim ) 
    send( victim, "%s has restored you.\r\n", ch->real_name() );
}


/*
 *   WIZLOCK
 */


void do_wizlock( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "Wizlock is %s.\r\n", wizlock ? "on" : "off" );
    return;
  }

  bool    flag  = wizlock;
  char*    tmp  = static_string( );

  if( !toggle( ch, argument, "wizlock", ( int* ) &wizlock, 0 ) ) {
    send( ch, "Syntax: wizlock <on|off>\r\n" );
    return;
  }

  if( wizlock != flag ) {
    sprintf( tmp, "** WIZLOCKED %s by %s **", wizlock ? "added" : "removed", ch->Name( ) ); 
    info( tmp, LEVEL_IMMORTAL, tmp, IFLAG_LOGINS, 1, ch );
  }
}


void do_sset( char_data* ch, char* argument )
{
  char arg1 [MAX_INPUT_LENGTH];
  char arg2 [MAX_INPUT_LENGTH];
  char arg3 [MAX_INPUT_LENGTH];
  char_data *victim;
  int col = 0;
  int sn;
  int value;
  bool fAll;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  for( int x = 0; arg2[x] != '\0'; x++ ) {
    if( arg2[x] == '.' )
      arg2[x] = ' ';
  }

  if( *arg1 == '\0' ) {
    send( ch, "Syntax: sset <char> [name] [value]\r\n" );
    return;
  }
 
  if( ( victim = one_player( ch, arg1, "sset", (thing_array*) &player_list ) ) == NULL )
    return;

  if( !has_permission( ch, PERM_PLAYERS ) && ch != victim ) {
    send( ch, "You can only sset yourself.\r\n" );
    return;
  }

  if( ( value = atoi( arg3 ) ) > MAX_SKILL_LEVEL || value < 0 ) {
    send( ch, "Skill prof. can range from 0 to %d.\r\n", MAX_SKILL_LEVEL );
    return;
  }

  if( arg2[0] == '\0' ) {
    for( sn = 0; sn < MAX_SKILL; sn++ ) {
      if( victim->real_skill( sn ) == UNLEARNT )
        continue;
      page( ch, "%17s (%2d)%s", skill_table[sn].name, victim->real_skill( sn ), ++col%3 == 0 ? "\r\n" : "" );
    }
    if( col % 3 != 0 )
      page( ch, "\r\n" );
    return;
  }

  fAll = !strcasecmp( arg2, "all" );

  for( sn = 0; sn < MAX_SKILL; sn++ ) {
    if( fAll ) {
      victim->pcdata->skill[sn] = value;
      continue;
    }
    if( !strncasecmp( skill_table[sn].name, arg2, strlen( arg2 ) ) ) {
      victim->pcdata->skill[sn] = value;
      send( ch, "%s set on %s to %d.\r\n", skill_table[sn].name, victim->descr->name, value );
      return;
    }
  }

  if( fAll ) {
    send( ch, "All skills on %s set to %d.\r\n", victim, value );
    return;
  } 

  send( ch, "Unknown skill.\r\n" );
  return; 
}


void do_force( char_data* ch, char* argument )
{
  char              arg  [ MAX_STRING_LENGTH ];
  char_data*     victim;

  argument = one_argument( argument, arg );

  if( *arg == '\0' || *argument == '\0' ) {
    send( ch, "Syntax: force <char> <command>\r\n" );
    return;
  }

  if( !strcasecmp( arg, "all" ) ) {
    if( !is_demigod( ch ) ) {
      send( ch, "Quite definitely not.\r\n" );
      return;
    }
    send( ch, "You force all players to '%s'.\r\n", argument );
    char tmp[THREE_LINES];
    sprintf(tmp, "All players forced to '%s' by %s.", argument, ch->Name());
    bug( BUG_SYSLOG, tmp, get_trust( ch ) );
    for( int i = 0; i < player_list; i++ ) {
      player_data *player = player_list[i];
      if( player->In_Game( ) && get_trust( player ) < get_trust( ch ) ) {
        set_bit( &player->status, STAT_FORCED );
        send( player, "%s forces you to '%s'.\r\n", ch, argument );
        interpret( player, argument );
      }
    }
    return;
  }

  if( ( victim = one_character( ch, arg, "force", ch->array, (thing_array*) &player_list ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "Forcing yourself doesn't make sense.\r\n" );
    return;
  }

  if( victim->pcdata != NULL && !has_permission( ch, PERM_FORCE_PLAYERS ) ) {
    send( ch, "You do not have permission to force players.\r\n" );
    return;
  }

  if( get_trust( victim ) >= get_trust( ch ) ) {
    send( ch, "You are unable to force them.\r\n" );
    return;
  }

  send( ch, "You force %s to '%s'.\r\n", victim, argument );
  send( victim, "%s forces you to '%s'.\r\n", ch, argument );

  if (player(victim) != NULL) {
    char tmp[ THREE_LINES ];
    sprintf( tmp, "%s forced to '%s' by %s.", victim->Name( ), argument, ch->Name( ) );
    bug( BUG_SYSLOG, tmp, get_trust( ch ) );
  }

  set_bit( &victim->status, STAT_FORCED );
  interpret( victim, argument );

  return;
}


void do_invis( char_data* ch, char* argument )
{
  wizard_data*   imm;
  int            num;

  if( not_player( ch ) )
    return;

  imm = (wizard_data*) ch;

  if( *argument ==  '\0' ) {
    if( !is_set( ch->pcdata->pfile->flags, PLR_WIZINVIS ) ) {
      send_seen( ch, "%s slowly fades into thin air.\r\n", ch );
      send( ch, "You slowly vanish into thin air.\r\n" );
      set_bit( ch->pcdata->pfile->flags, PLR_WIZINVIS );
    } else {
      remove_bit( ch->pcdata->pfile->flags, PLR_WIZINVIS );
      send_seen( ch, "%s slowly fades into existence.\r\n", ch );
      send( ch, "You slowly fade back into existence.\r\n" );
    }
    return;
  }

  num = atoi( argument );
  if( num > ch->shdata->level || num < 1 ) {
    send( "Choose a number between 1 and your level.\r\n", ch );
    return;
  }

  send( ch, "Your wizinvis level has been set to %d.\r\n", num );
  imm->wizinvis = num;
}


void do_holylight( char_data* ch, char* )
{
  if( ch->pcdata == NULL )
    return;
    
  switch_bit( ch->pcdata->pfile->flags, PLR_HOLYLIGHT );
  send( ch, "Holy light mode %s.\r\n", on_off( ch->pcdata->pfile->flags, PLR_HOLYLIGHT ) );
}


/*
 *   SNOOP COMMANDS
 */


void do_snoop( char_data* ch, char* argument)
{
  char buf [ MAX_INPUT_LENGTH ];
  link_data* link;
  char_data* victim;
  link_nav links( &link_list );

  if( argument[0] == '\0' ) {
    bool fFound = FALSE;

    buf[0] = '\0';
    for( link = links.first(); link != NULL; link = links.next() ) 
      if( link->snoop_by != NULL && get_trust( ch ) >= get_trust( link->snoop_by->character ) ) {
        fFound = TRUE;
        sprintf( buf + strlen( buf ), "%15s : %s\r\n", link->character->real_name( ), link->snoop_by->character->real_name( ) );
      }
      
      if( fFound )
        send( buf, ch );
      else
        send( "No active snoops.\r\n", ch );
      return;
  }
  
  if( ( victim = one_character( ch, argument, "snoop", ch->array, (thing_array*)&player_list, (thing_array*)&mob_list ) ) == NULL ) {
    send( "They aren't here.\r\n", ch );
    return;
  }

  if( victim->link == NULL ) {
    send( "No descriptor to snoop.\r\n", ch );
    return;
  }

  if( victim == ch ) {
    send( "Cancelling all snoops.\r\n", ch );
    for( link = links.first(); link != NULL; link = links.next() ) 
      if( link->snoop_by == ch->link )
        link->snoop_by = NULL;
    return;
  }

  if( victim->link->snoop_by != NULL ) {
    send( "Busy already.\r\n", ch );
    return;
  }

  if( get_trust( victim ) >= LEVEL_APPRENTICE ) { // get_trust( ch ) ) {
    send( "You failed.\r\n", ch );
    return;
  }

  if( ch->link != NULL )
    for( link = ch->link->snoop_by; link != NULL; link = link->snoop_by ) 
      if( link->character == victim ) {
        send( "No snoop loops.\r\n", ch );
        return;
      }

  victim->link->snoop_by = ch->link;
  send( "Ok.\r\n", ch );
  return;
}


/*
 *   SHUTDOWN/REBOOT
 */


void shutdown_sequence( char* )
{
  room_data*   room;
  player_data* player;

  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) )
    for( room = area->room_first; room != NULL; room = room->next ) 
      if( is_set( &room->room_flags, RFLAG_SAVE_ITEMS ) ) 
        save_room_items( room );

  for( int i = 0; i < player_list; i++ ) {
    player = player_list[i];
    if( player->In_Game( ) )
      write( player );
  }

  write_all( );
}


void do_shutdown( char_data* ch, char* argument )
{
  int flags;

  if( !get_flags( ch, argument, &flags, "n", "shutdown" ) )
    return;

  echo( "** Shutdown by %s **\r\n", ch->descr->name );
  shutdown_sequence( argument );

//  if( !is_set( &flags, 0 ) )
#ifdef MYSTERIA_WINDOWS
  tfe_reboot = 0;
#else
  // Need to touch file ".killscript", so autorun script won't restart us.
  system("touch .killscript");
#endif

  tfe_down = TRUE; // main loop will exit
}


void do_reboot( char_data* ch, char* argument )
{
  int flags;
  if( !get_flags( ch, argument, &flags, "d", "reboot" ) )
    return;

  if( is_set( &flags, 0 ) ) {
    if( init_daemon( ) ) 
      send( ch, "Host daemon rebooted.\r\n" );
    else 
      send( ch, "Reboot of daemon failed.\r\n" );
    return;
    }

  echo( "** Reboot by %s. **\r\n", ch->descr->name );

  shutdown_sequence( argument );

  tfe_reboot = 1;
  tfe_down = TRUE; // main loop will exit, autorun script will restart us
}


/*
 *   RENAME
 */


void do_rename( char_data* ch, char* argument )
{
  char           arg  [ MAX_INPUT_LENGTH ];
  char           tmp  [ THREE_LINES ];
  char_data*  victim;
  char*         name;

  in_character = FALSE;

  argument = one_argument( argument, arg );

  if( ( victim = one_player( ch, arg, "rename", (thing_array*) &player_list ) ) == NULL )
    return;

  if( victim != ch && get_trust( victim ) >= get_trust( ch ) ) {
    send( ch, "You can only rename those of lower level.\r\n" );
    return;
  } 

  if( victim == ch && get_trust( ch ) < LEVEL_SPIRIT ) {
    send( ch, "To avoid temptation you are unable to rename yourself.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "What do you want to rename them?\r\n" );
    return;
  }  

  *argument = toupper( *argument );

  if( !check_parse_name( ch->link, argument ) )
    return;

  name = victim->descr->name;

  // Currently, player_log template func can't format over 2 variables.
  //player_log( victim, "Name changed from %s to %s.", name, argument ); 
  player_log( victim, "Name changed from %s by %s.", name, ch->real_name() ); 
  player_log( ch, "Renamed %s to %s.", name, argument ); 

  rename_file( PLAYER_DIR,         name, PLAYER_DIR,         argument );
  rename_file( PLAYER_PREV_DIR,    name, PLAYER_PREV_DIR,    argument );
  rename_file( PLAYER_QUIT_DIR,    name, PLAYER_QUIT_DIR,    argument );
  rename_file( PLAYER_DEATH_DIR,   name, PLAYER_DEATH_DIR,   argument );
  rename_file( PLAYER_RESTORE_DIR, name, PLAYER_RESTORE_DIR, argument );
  rename_file( PLAYER_LOG_DIR,     name, PLAYER_LOG_DIR,     argument );
  rename_file( MAIL_DIR,           name, MAIL_DIR,           argument );

  remove_list( pfile_list, max_pfile, victim->pcdata->pfile );

  if( ch == victim ) {
    send( ch, "You rename yourself %s.\r\n", argument );
  } else {
    send( ch, "%s renamed %s.\r\n", name, argument );
    send( victim, "You have been renamed %s by %s.\r\n", argument, ch->descr->name );
  }

  sprintf( tmp, "%s renamed %s by %s.", name, argument, ch->descr->name );
  info( "", LEVEL_APPRENTICE, tmp, IFLAG_WRITES, 2, ch );

  free_string( victim->pcdata->pfile->name, MEM_PFILE );
  free_string( victim->descr->name,         MEM_DESCR );

  victim->pcdata->pfile->name = alloc_string( argument, MEM_PFILE );
  victim->descr->name         = alloc_string( argument, MEM_DESCR );
 
  add_list( pfile_list, max_pfile, victim->pcdata->pfile );
}



void do_nowhere( char_data* ch, char* argument )
{
  int found;

  if( !*argument ) {
    //------------- find awol objects
    page_underlined( ch, "Awol Objects:\r\n" );
    found = 0;

    for( int i = 0; i < obj_list; i++ ) {
      obj_data* obj = obj_list[ i ];
      room_data* room = NULL;
      char_data* owner = NULL;   
      obj_data* container = NULL;

      for( thing_data* where = obj; ; ) {
        if( where->array == NULL || ( where = where->array->where ) == NULL )
          break; 
        if( Room( where ) != NULL )
          room = (room_data*) where;
        else if( object( where ) != NULL )
          container = (obj_data*) where;
        else if( character( where ) != NULL )
          owner = (char_data*) where;
      } 

      if( room != NULL )
        continue;

      page( ch, "%d, %s\r\n", obj->pIndexData->vnum, obj->Name( ) );
      found++;
    }

    if( found == 0 )
      page( ch, "  none found.\r\n\r\n" );
    else
      page( ch, "  %d found.\r\n\r\n", found );

    //------------- find awol mobs
    page_underlined( ch, "Awol Mobs:\r\n" );
    found = 0;

    for( int i = 0; i < mob_list; i++ ) {
      mob_data* mob = mob_list[ i ];

      if( mob->in_room != NULL )
        continue;

      page( ch, "%d, %s\r\n", mob->species->vnum, mob->Name( ) );
      found++;
    }

    if( found == 0 )
      page( ch, "  none found.\r\n" );
    else
      page( ch, "  %d found.\r\n", found );

    return;

  } else if( exact_match( argument, "transfer" ) ) {
    //------------- transfer awol objects
    page_underlined( ch, "Awol Objects Retrieved:\r\n" );
    found = 0;

    for( int i = 0; i < obj_list; i++ ) {
      obj_data* obj = obj_list[ i ];
      room_data* room = NULL;
      char_data* owner = NULL;   
      obj_data* container = NULL;

      for( thing_data* where = obj; ; ) {
        if( where->array == NULL || ( where = where->array->where ) == NULL )
          break; 
        if( Room( where ) != NULL )
          room = (room_data*) where;
        else if( object( where ) != NULL )
          container = (obj_data*) where;
        else if( character( where ) != NULL )
          owner = (char_data*) where;
      } 

      if( room != NULL )
        continue;

      page( ch, "%d, %s\r\n", obj->pIndexData->vnum, obj->Name( ) );
      found++;

      obj->To( ch->in_room );
    }

    if( found == 0 )
      page( ch, "  none retrieved.\r\n\r\n" );
    else
      page( ch, "  %d retrieved.\r\n\r\n", found );

    //------------- transfer awol mobs
    page_underlined( ch, "Awol Mobs Retrieved:\r\n" );
    found = 0;

    for( int i = 0; i < mob_list; i++ ) {
      mob_data* mob = mob_list[ i ];

      if( mob->in_room != NULL )
        continue;

      page( ch, "%d, %s\r\n", mob->species->vnum, mob->Name( ) );
      found++;

      mob->To( ch->in_room );
    }

    if( found == 0 )
      page( ch, "  none retrieved.\r\n" );
    else
      page( ch, "  %d retrieved.\r\n", found );
  }
}

