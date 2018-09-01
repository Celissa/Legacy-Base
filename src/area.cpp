#include "system.h"

void  fix_exits      ( void );

llist<area_data> area_list;


const char*  area_status [ MAX_AREA_STATUS ] = {
  "open", "worthless", "abandoned", "progressing", "pending",
  "blank", "immortal" };

const char *first_room_name = "The Construct";
const char *first_room_description = 
    "You are standing in a pure white room that extends to infinity.  You see\r\n"
    "no shadows, no ceiling, no walls or boundaries of any sort.  The room is\r\n"
    "empty, yet something keeps you from falling.  There seems to be no way\r\n"
    "out of here, except for divine intervention."
;


/*
 *   AREA_DATA CLASS
 */


Area_Data :: Area_Data( )
{
  record_new( sizeof( area_data ), MEM_AREA );

  room_first   = NULL;
  tax_rate     = empty_string;
  bonus        = empty_string;
  age          = 15;
  nplayer      = 0;
  modified     = TRUE;
  reset_count  = 0;
  reset_lag    = 0;
  total_lag    = 0;
}


void Area_Data :: Show( room_data* room, char_data* ch, bool brief, bool scan )
{  
  char           tmp  [ 3*MAX_STRING_LENGTH ];
  thing_data*  thing;
  obj_data*      obj;
  action_data* action;

  if( ch == NULL || ch->pcdata == NULL || room == NULL )
    return;

  room->ShowInfo( ch );
   
  if( !brief || !is_set( ch->pcdata->pfile->flags, PLR_BRIEF ) ) {
    if( room->Seen( ch ) ) {
      convert_to_ansi( ch, room->description, tmp, sizeof(tmp) );
      if( ch->pcdata->terminal == TERM_MXP ) {
        send( ch, MXP_SECURE "<rdesc>" );
        if( !is_set( &room->room_flags, RFLAG_CODE_RDESC ) )
          send( ch, tmp );
        else {
          if( ( action = room->action ) == NULL ) {
            fsend( ch, "This room has a bugged description please post that room #%i needs work.\r\n", room->vnum );
            return;
          }
          for( action = room->action; action != NULL; action = action->next ) {
            if( action->trigger == TRIGGER_RDESC )
              break;
          }
          if( action == NULL ) {
            fsend( ch, "This room has a bugged description please post that room #%i needs work.\r\n", room->vnum );
            return;
          }
          var_ch = ch;
          var_room = room;
          execute( action );
        }
        send( ch, MXP_SECURE "</rdesc>" );
      } else {
        if( !is_set( &room->room_flags, RFLAG_CODE_RDESC ) )
          send( ch, tmp );
        else {
          if( ( action = room->action ) == NULL ) {
            fsend( ch, "This room has a bugged description please post that room #%i needs work.\r\n", room->vnum );
            return;
          }
          for( action = room->action; action != NULL; action = action->next ) {
            if( action->trigger == TRIGGER_RDESC )
              break;
          }
          if( action == NULL ) {
            fsend( ch, "This room has a bugged description please post that room #%i needs work.\r\n", room->vnum );
            return;
          }
          var_ch = ch;
          var_room = room;
          execute( action );
        }
      }
    } else {
      send( ch, "The area is very dark and you can make out no details.\r\n" );
    }

    if( is_apprentice( ch ) && *room->comments != '\0' ) {
      send( ch, "%37s-----\r\n", "" );
      send( ch, room->comments ); 
    }
  }

  // if character cannot see the room, then don't show secrets / tracks
  if( room->Seen( ch ) ) {
    show_secret( ch );
    show_tracks( ch );
  }

  // show contents of room based on players visiblity
  select( room->contents, ch );

  for( int i = 0; i < room->contents; i++ ) 
    if( ( obj = object( room->contents[i] ) ) != NULL && is_set( obj->extra_flags, OFLAG_NOSHOW ) )
      room->contents[i]->selected = 0; 

  rehash( ch, room->contents );

  bool found = FALSE;

  for( int i = 0; i < room->contents; i++ ) {
    thing = room->contents[i];
    if( thing->shown > 0 && thing != ch ) {
      // this used to only check 'seen' for mobs, but that woulda let ppl see invis objects etc
      // so now it checks seen on everything in the room
      if( !thing->Seen( ch ) )
        continue;

      if( !found ) {
        found = TRUE;
        send( ch, "\r\n" );
      }
      
      send( ch, "%s\r\n", thing->Show( ch, thing->shown ) );
    }
  }

  /* SCAN */

  int level = level_setting( &ch->pcdata->pfile->settings, SET_AUTOSCAN );

  if( scan && level != 0 ) // && ( level == 3 || !is_set( &room->room_flags, RFLAG_NO_AUTOSCAN ) ) )
    do_scan( ch, "shrt" );       
}


void Area_Data :: Reset( bool force )
{
  struct timeval area_start;
  room_data*     room;

  gettimeofday( &area_start, NULL );
  reset_count++;

  for( room = room_first; room != NULL; room = room->next ) {
    if( force || !player_in_room( room ) )
      room->Reset( force );
  }

  age = number_range( 0, 3 );

// don't save areas in update handler
//    if( area->modified )
//      save_area( area );

  reset_lag = stop_clock( area_start );
  total_lag += reset_lag;
}


char *make_afname( const char *aname )
{
  char *str = static_string(), *write = str;

  for( const char *read = aname; *read; *read++)
    if( isalpha( *read ) )
      *write++ = tolower( *read );

  *write = '\0';
  strcat(str, ".are");

  FILE *fp = NULL;
  int i = 2;
  while( ( fp = open_file( AREA_DIR, str, "rb" ) ) != NULL ) {
    fclose( fp );
   
    str[strlen(str) - 4] = '\0';
    sprintf(str, "%s%d.are", str, i++);
  }

  return str;
}


Area_Data :: Area_Data( char *aname, int vnum, int acontinent )
{
  record_new( sizeof( area_data ), MEM_AREA );

  file = alloc_string( make_afname( aname ), MEM_AREA );
  name = alloc_string( aname, MEM_AREA );
  tax_rate  = empty_string;
  bonus     = empty_string;
  creator   = NULL;
  help      = NULL;
  status    = AREA_PROGRESSING;
  continent = acontinent;
  level = 0;
  age          = 15;
  nplayer      = 0;
  modified     = TRUE;
  reset_count  = 0;
  reset_lag    = 0;
  total_lag    = 0;

  room_data*  room = new room_data;
  room->next = NULL;
  room->area = this;
  room->vnum = vnum;
  room->name = alloc_string( aname, MEM_ROOM );
  room->description = alloc_string( "Under Construction\r\n", MEM_ROOM );
  room->comments = empty_string;
  room->room_flags = 0;
  room->sector_type = SECT_CITY;

//  set_bit(&room->room_flags, RFLAG_NO_PKILL);

  room_first = room;
  area_list.add( this );
}


// Creates Default Area
void make_default_area(void)
{
  area_data *area = new area_data( "Ground Zero", ROOM_CONSTRUCT, 0 );
  room_data *room = area->room_first;

  free_string(room->name, MEM_ROOM);
  free_string(room->description, MEM_ROOM);
  room->name = alloc_string( first_room_name, MEM_ROOM );
  room->description = alloc_string( first_room_description, MEM_ROOM );

  set_bit(&room->room_flags, RFLAG_LIT);
  set_bit(&room->room_flags, RFLAG_SAFE);
  set_bit(&room->room_flags, RFLAG_INDOORS);
  set_bit(&room->room_flags, RFLAG_NO_MAGIC);
  set_bit(&room->room_flags, RFLAG_NO_RECALL);
  set_bit(&room->room_flags, RFLAG_NO_PRAY);
  set_bit(&room->room_flags, RFLAG_NO_SUMMON_IN);
  set_bit(&room->room_flags, RFLAG_NO_SUMMON_OUT);
  set_bit(&room->room_flags, RFLAG_NO_GATE);
}


/*
 *   DISK ROUTINES
 */


void load_areas( void )
{
  selt_string filespec;
  filelist list;

  filespec.printf("%s*%s", AREA_DIR, AREA_SUF);
  dirlist(&list, filespec);

  echo( "Loading Areas ...\r\n" );

  while (!list.empty) {
    char *filename = list.headitem();
    list.remove(filename);
    load_area(filename);
    delete [] filename;
  }

  if (area_list.empty) {
    log( "  *** NO AREAS FOUND:  CREATING DEFAULT\r\n" );
    make_default_area();
  }

  fix_exits( );
}


void fix_exits( void )
{
  room_data*  room;
  exit_data*  exit;

  log( "Fixing exits...\r\n" );

  area_nav alist(&area_list);
  for( area_data *area = alist.first( ); area; area = alist.next( ) )
    for( room = area->room_first; room != NULL; room = room->next ) {
      set_bit( &room->room_flags, RFLAG_STATUS0 );
      set_bit( &room->room_flags, RFLAG_STATUS1 );
      set_bit( &room->room_flags, RFLAG_STATUS2 );

      for( int i = room->exits-1; i >= 0; i-- ) {
        exit = room->exits[i];
        room_data* tmp_room = get_room_index( (int) exit->to_room );
        if (tmp_room == NULL) {
          roach( "Fix_Exits: Deleting exit from %d to non-existent %d.", room->vnum, (int) exit->to_room );
          room->exits -= exit;
          delete exit;
        } else {
          exit->to_room = tmp_room;
        }
      }
    }

  return;
}


bool set_area_file( char_data* ch, const char* arg, const char* name )
{
  int i = strlen(arg);

  if( i < 7 || i > 16 || strcasecmp(&arg[i-4], ".are") ) {
    send( ch, "An area file name must be between 7 and 16 letters, ending with '.are'.\r\n" );
    return FALSE;
  }

  for( int j = 0; arg[j] < i; j++ ) {
    if( !isalpha( arg[j] ) ) {
      send( ch, "An area file name may only contain letters.\r\n" );
      return FALSE;
    }
  }

  area_nav alist(&area_list);
  for (area_data *area = alist.first(); area; area = alist.next()) {
    if( !strcasecmp( area->file, arg ) ) {
      send( ch, "There is already an area with that filename.\r\n" );
      return FALSE;
    }
  }
  
  rename_file( AREA_DIR, name, AREA_DIR, arg );

  return TRUE;
}


void load_area( const char* file )
{
  bool boot_old = false;
  int version = 0;

  FILE* fp = open_file( AREA_DIR, file, "rb", TRUE );

  area_data* area = new area_data;
  area->file = alloc_string( file, MEM_AREA );

  char *word = fread_word(fp);
  if( !strcmp( word, "#M2_AREA" ) ) {
    version = fread_number( fp );
  } else if( !strcmp( word, "#AREA" ) ) {
    log( "... old style area file" );
    boot_old = true;
  } else if( !strcmp( word, "#NEW_AREA" ) ) {
    // version 0 area file...
  } else {
    panic( "Load_area: header not found" );
  }

  area->name      = fread_string( fp, MEM_AREA );
  area->creator   = fread_string( fp, MEM_AREA );
  area->help      = fread_string( fp, MEM_AREA );
  if( version >= 1 )
    area->tax_rate = fread_string( fp, MEM_AREA );

  if( version >= 2 )
    area->bonus    = fread_string( fp, MEM_AREA );

  area->level       = fread_number( fp );
  area->reset_time  = fread_number( fp );
  area->status      = fread_number( fp );
  if( boot_old ) {
    area->continent = 0;
  } else {
    area->continent = fread_number( fp );
  }

  log( "  * %-20s : v%d : %s\r\n", file, version, area->name );

  load_rooms( fp, area );
  fclose( fp );

  area_list.add( area );
}


bool save_area( area_data* area, bool forced )
{
  action_data*  action;
  room_data*      room;
  exit_data*      exit;
  FILE*             fp;

  if( area->room_first && area->room_first->vnum >= ROOM_GENERATED )
    return FALSE;

  if( !forced && !area->modified )
    return FALSE;

  rename_file( AREA_DIR, area->file, AREA_PREV_DIR, area->file );

  if( ( fp = open_file( AREA_DIR, area->file, "wb" ) ) == NULL )
    return FALSE;

  fprintf( fp, "#M2_AREA\n" );
  fprintf( fp, "%d\n\n", 2 ); // version

  fprintf( fp, "%s~\n", area->name );
  fprintf( fp, "%s~\n", area->creator );
  fprintf( fp, "%s~\n", area->help );
  fprintf( fp, "%s~\n", area->tax_rate );
  fprintf( fp, "%s~\n\n", area->bonus );
  

  fprintf( fp, "%d %d\n", area->level, area->reset_time );
  fprintf( fp, "%d\n", area->status );
  fprintf( fp, "%d\n", area->continent );
  fprintf( fp, "#ROOMS\n\n" );

  for( room = area->room_first; room != NULL; room = room->next ) {
    fprintf( fp, "#%d\n", room->vnum );
    fprintf( fp, "%s~\n", room->name );
    fprintf( fp, "%s~\n", room->description );
    fprintf( fp, "%s~\n", room->comments ); 
    fprintf( fp, "%d %d %d 0\n", room->room_flags, room->sector_type, room->size );

    for( int i = 0; i < room->exits; i++ ) {
      exit = room->exits[i];
      if( exit->to_room && exit->to_room->vnum >= ROOM_GENERATED )
        continue;
      fprintf( fp, "D%d\n", exit->direction );
      fprintf( fp, "%s~\n", exit->name );
      fprintf( fp, "%s~\n", exit->keywords );
      fprintf( fp, "%d %d %d %d %d %d\n", exit->exit_info, exit->key, exit->to_room->vnum, exit->strength, exit->light, exit->size );
    }

    write_extras( fp, room->extra_descr );

    for( action = room->action; action != NULL; action = action->next ) 
      write( fp, action ); 

    /*-- WRITE VARIABLES --*/

    if( room->variables.size > 0 && is_set( &room->room_flags, RFLAG_SAVE_VARIABLES ) ) {
      variable_nav list( &room->variables );
      for( variable_data* var = list.first( ); var; var = list.next( ) ) {
        if( var->get_value( ) != 0 ) {
          fprintf( fp, "V\n" );
          fprintf( fp, "%s~\n", var->get_name( ) );
          fprintf( fp, "%d\n", var->get_value( ) );
        }
      }
    }

    write( fp, room->reset );

    fprintf( fp, "S\n\n" );
  }

  fprintf( fp, "#0\n\n" );
  fclose( fp );

  area->modified = FALSE;

  return TRUE;
}


/*
 *   AREA LIST COMMAND
 */


void room_range( area_data* area, int& low, int& high, int &nRooms )
{
  room_data*  room;

  low    = 999999;
  high   = 0;
  nRooms = 0;

  for( room = area->room_first; room != NULL; room = room->next ) {
    nRooms++;
    low  = min( low, room->vnum );
    high = max( high, room->vnum );
  }

  return;
}


/*
 *   AREA/ROOM SUMMARY COMMANDS
 */

void do_areas( char_data* ch, char* argument )
{
  char         tmp  [ TWO_LINES ];
  int          min;
  int          max;
  int       nRooms;
  int       status;
  int       length  = strlen( argument );
  int        flags;

  if( ch->link == NULL )
    return;

  area_nav alist(&area_list);

  if( !get_flags( ch, argument, &flags, "fnod", "Areas" ) )
    return;
  if( flags == 0 ) {
    if( *argument == '\0' ) {
      page_title( ch, "Areas" );
      int i = 0;
      for( area_data *area = alist.first( ); area; area = alist.next( ) ) {
        if( area->help != empty_string && area->status == AREA_OPEN && area->room_first && area->room_first->vnum < ROOM_GENERATED ) 
          page( ch, "%24s%s", area->name, (i++)%3 != 2 ? "" : "\r\n" );
      }
      if( i%3 != 0 )
        page( ch, "\r\n" );
      page( ch, "\r\n" );
      page_centered( ch, "[ Type area <name> to see more information. ]" );
      page_centered( ch, "[ You can flag area with -n/-o to limit the list. ]" );
     return;
   }
  }

  if( is_set( &flags, 1 ) ) {
    page_title( ch, "New Continent Areas" );
    int i = 0;
    for( area_data *area = alist.first(); area; area = alist.next() ) {
      if( area->help != empty_string && area->status == AREA_OPEN && area->continent != OLD_CONTINENT && area->room_first && area->room_first->vnum < ROOM_GENERATED )
        page( ch, "%24s%s", area->name, (i++)%3 != 2 ? "" : "\r\n" );
      }
    if( i%3 != 0 )
      page( ch, "\r\n" );
    page( ch, "\r\n" );
    page_centered( ch, "[ Type area <name> to see more information. ]" );
    return;
  }

  if( is_set( &flags, 2 ) ) {
    page_title( ch, "Old Continent Areas" );
    int i = 0;
    for( area_data *area = alist.first(); area; area = alist.next() ) {
      if( area->help != empty_string && area->status == AREA_OPEN && area->continent == OLD_CONTINENT && area->room_first && area->room_first->vnum < ROOM_GENERATED ) 
        page( ch, "%24s%s", area->name, (i++)%3 != 2 ? "" : "\r\n" );
    }
    if( i%3 != 0 )
      page( ch, "\r\n" );
    page( ch, "\r\n" );
    page_centered( ch, "[ Type area <name> to see more information. ]" );
    return;
  }

  if( is_set( &flags, 3 ) ) {
    page_title( ch, "Dynamic Areas" );
    int i = 0;
    for( area_data *area = alist.first(); area; area = alist.next() ) {
      if( area->help != empty_string && area->status == AREA_OPEN && area->room_first && area->room_first->vnum >= ROOM_GENERATED ) 
        page( ch, "%24s%s", area->name, (i++)%3 != 2 ? "" : "\r\n" );
      }
    if( i%3 != 0 )
      page( ch, "\r\n" );
    page( ch, "\r\n" );
    page_centered( ch, "[ Type area <name> to see more information. ]" );
    return;
  }

  if( is_apprentice( ch ) ) {
    status = -1;
    if( matches( argument, "lag", true ) ) {
      page( ch, "%25s %6s %6s %11s %11s %11s\r\n", "Area Name", "Base", "Resets", "Last", "Average", "Total" );

      for( area_data *area = alist.first( ); area; area = alist.next( ) )
        page( ch, "%25s %6d %6d %11.3f %11.3f %11.3f\r\n", area->name, area->room_first ? area->room_first->vnum : -1, area->reset_count, area->reset_lag.time( ) / 1000, area->total_lag.time( ) / (1000 * area->reset_count ), area->total_lag.time( ) / 1000 );

      return;
    }

    if( strncasecmp( argument, "summary", length ) )
      for( status = 0; status < MAX_AREA_STATUS; status++ )
        if( !strncasecmp( argument, area_status[status], length ) )
          break;

    if( status != MAX_AREA_STATUS ) {
      sprintf( tmp, "%25s   %11s   %5s  %s\r\n", "Area Name", "Vnum Range", "#Plyr", "Creator" );
      page_underlined( ch, tmp );
  
      for( int i = 0; i < MAX_AREA_STATUS; i++ ) {
        if( status != -1 && i != status )
          continue;
        *tmp = '\0';
        for( area_data *area = alist.first( ); area; area = alist.next( ) ) {
          if( area->status != i )
            continue;
          if( *tmp == '\0' ) {
            page( ch, "\r\n" );
            sprintf( tmp, "--- %s ---", area_status[i] );
            tmp[4] = toupper( tmp[4] );
            page_centered( ch, tmp ); 
            page( ch, "\r\n" );
          }
          room_range( area, min, max, nRooms );
          if( min == ROOM_GENERATED ) {
            page( ch, "\r\n" );
            sprintf( tmp, "--- %s ---", "Dynamic Areas" );
            tmp[4] = toupper( tmp[4] );
            page_centered( ch, tmp ); 
            page( ch, "\r\n" );
          }
          page( ch, "%25s  %6d-%-6d  %3d    %s\r\n", area->name, min, max, area->nplayer, area->creator );
        }
      }
      return;
    } 
  }

  if( is_demigod( ch ) ) {
    if( matches( argument, "new", true ) ) {
      int continent = -1, vnum = -1;

      if( !number_arg( argument, continent ) || !number_arg( argument, vnum ) || *argument == '\0' ) {
        send( ch, "Syntax: area new <continent> <base vnum> <area name>\r\n" );
        return;
      }

      capitalize( argument );

      // create new area
      bool abort = false;

      for( area_data *area = alist.first( ); area; area = alist.next( ) ) {
        room_range( area, min, max, nRooms );
        if( vnum >= min && vnum <= max ) {
          send( ch, "You cannot create an area inside another area's vnum range.\r\n" );
          return;
        } else if( vnum >= min - 100 && vnum <= max + 100 ) {
          send( ch, "New area is too close to %s (%d).\r\n", area->name, min);
          abort = true;
        }
      }

      if( abort && !is_set( &flags, 0 ) ) {
        send( ch, "To allow for expansion, a 100 room vnum buffer should be left on\r\n"
                  "either side of a new area.  To create anyway, use the -f flag.\r\n" );
        return;
      }

      area_data *area = new area_data( argument, vnum, continent );
      send( ch, "New area created %s [%s] (%d).\r\n", area->name, area->file, vnum );
      return;

    } else if( matches( argument, "renumber", true ) ) {
      int old_vnum, new_vnum, min, max, num;
      area_data *area;

      if( !number_arg( argument, old_vnum ) || !number_arg( argument, new_vnum ) ) {
        send( ch, "Syntax: oedit renumber <vnum_old> <vnum_new>.\r\n" );
        return;
      }

      for( area = alist.first( ); area; area = alist.next( ) ) {
        if( area->room_first && area->room_first->vnum == old_vnum )
          break;
      }

      if( !area ) {
        send( ch, "No area found starting at %d.", old_vnum );
        return;
      }

      room_range( area, min, max, num );
 
      bool abort = false;

      for( area_data *tarea = alist.first( ); tarea; tarea = alist.next( ) ) {
        int tmin, tmax, tnum;
        room_range( tarea, tmin, tmax, tnum );

        if( new_vnum >= tmin && new_vnum <= tmax || tmin >= new_vnum && tmin <= new_vnum + num ) {
          send( ch, "You cannot renumber an area inside %s vnum range.\r\n", area == tarea ? "its own" : "another area's" );
          return;
        } else if( area != tarea && new_vnum + num >= tmin - 100 && new_vnum <= tmax + 100 ) {
          send( ch, "New vnum range is too close to %s (%d).\r\n", tarea->name, tmin );
          abort = true;
        }
      }

      if( abort && !is_set( &flags, 0 ) ) {
        send( ch, "To allow for expansion, a 100 room vnum buffer should be left on\r\n"
                  "either side of a renumbered area.  To renumber anyway, use the -f flag.\r\n" );
        return;
      }

      selt_string temp;
      int offset = 0, total = 0;
      for( room_data* room = area->room_first; room != NULL; room = room->next ) {
        int replaced = 0;

        // search for imm->offices
        for( int i = 0; i < player_list; i++ ) {
          wizard_data* imm = wizard( player_list[ i ] );
          if( imm && room->vnum == imm->office ) {
            imm->office = new_vnum + offset;
            replaced++;
          }
        }

        // search player start rooms
        for( int i = 0; i < MAX_PLYR_RACE; i++ ) {
          for( int j = 0; j < 3; j++ ) {
            if( plyr_race_table[ i ].start_room[ j ] == room->vnum ) {
              plyr_race_table[ i ].start_room[ j ] = new_vnum + offset;
              replaced++;
            }
            if( plyr_race_table[ i ].nc_room[ j ] == room->vnum ) {
              plyr_race_table[ i ].nc_room[ j ] = room->vnum +offset;
              replaced++;
            }
          }

          if( plyr_race_table[ i ].portal == room->vnum ) {
            plyr_race_table[ i ].portal = new_vnum + offset;
            replaced++;
          }
        }

        // search towns
        for( int i = 0; i < MAX_ENTRY_TOWN; i++ ) {
          if( town_table[ i ].recall == room->vnum ) {
            town_table[ i ].recall = new_vnum + offset;
            replaced++;
          }
        }

        // search astral
        for( int i = 0; i < MAX_ENTRY_ASTRAL; i++ ) {
          if( astral_table[ i ].recall == room->vnum ) {
            astral_table[ i ].recall = new_vnum + offset;
            replaced++;
          }
        }

        // search acodes
        for( area_data* tarea = alist.first( ); tarea; tarea = alist.next( ) ) {
          for( room_data* troom = tarea->room_first; troom != NULL; troom = troom->next ) {
            action_data* action;
            int j;

            for( j = 1, action = troom->action; action != NULL; j++, action = action->next ) {
              if( action->binary != NULL && search_progs( action->binary, room->vnum, ROOM ) ) {
                temp._printf( "// room %d needs references changed to %d\r\n", room->vnum, new_vnum + offset );
                temp += action->code;
                free_string( action->code, MEM_ACTION );
                action->code = alloc_string( temp.get_text( ), MEM_ACTION );

                page( ch, "  Used in action #%d in room #%d.\r\n", j, troom->vnum );
                replaced++;
              }
            }
          }
        }

        // search mobs
        for( int i = 0; i < MAX_SPECIES; i++ ) {
          species_data* species;
          mprog_data* mprog;
          int j;

          if( ( species = species_list[ i ] ) == NULL )
            continue;

          if( search_progs( species->attack->binary, room->vnum, ROOM ) ) {
            temp._printf( "// room %d needs references changed to %d\r\n", room->vnum, new_vnum + offset );
            temp += species->attack->code;
            free_string( species->attack->code, MEM_MPROG );
            species->attack->code = alloc_string( temp.get_text( ), MEM_MPROG );

            page( ch, "  Used in attack prog on mob #%d.\r\n", i );
            replaced++;
          }

          for( j = 1, mprog = species->mprog; mprog != NULL; j++, mprog = mprog->next ) {
            if( search_progs( mprog->binary, room->vnum, ROOM ) ) {
              temp._printf( "// room %d needs references changed to %d\r\n", room->vnum, new_vnum + offset );
              temp += mprog->code;
              free_string( mprog->code, MEM_MPROG );
              mprog->code = alloc_string( temp.get_text( ), MEM_MPROG );

              page( ch, "  Used in mprog #%d on mob #%d.\r\n", j, i );
              replaced++;
            }
          }
        }

        // search objs
        for( int i = 0; i < MAX_OBJ_INDEX; i++ ) {
          obj_clss_data* obj_clss;
          oprog_data* oprog;
          int j;

          if( ( obj_clss = obj_index_list[ i ] ) == NULL )
            continue;

          for( j = 1, oprog = obj_clss->oprog; oprog != NULL; j++, oprog = oprog->next ) {
            if( search_progs( oprog->binary, room->vnum, ROOM ) ) {
              temp._printf( "// room %d needs references changed to %d\r\n", room->vnum, new_vnum + offset );
              temp += oprog->code;
              free_string( oprog->code, MEM_OPROG );
              oprog->code = alloc_string( temp.get_text( ), MEM_OPROG );
        
              page( ch, "  Used in oprog #%d on object #%d.\r\n", j, i );
              replaced++;
            }
          }
        }

        page( ch, "Room #%d renumbered to #%d in %d places.\r\n", room->vnum, new_vnum + offset, replaced );

        total += replaced;
        room->vnum = new_vnum + offset;

        offset++;
      }

      area_list.remove( area );
      area_list.add( area );

      page( ch, "Area '%s' starting %d renumbered to %d.\r\n%d rooms renumbered in %d places.\r\n", area->name, old_vnum, new_vnum, num, total );
      return;
    }
  }

  area_data *area = NULL;
  for (area = alist.first(); area; area = alist.next())
    if( area->help != empty_string && area->status == AREA_OPEN
      && !strncasecmp( area->name, argument, length ) )
      break;

  if( area == NULL ) {
    send( ch, "No area matching that name found.\r\n" );
    return;
  }

  page( ch, "         Name: %s\r\n", area->name );
  page( ch, "      Creator: %s\r\n", area->creator );

  if( is_apprentice( ch ) ) {
    room_range( area, min, max, nRooms );
    page( ch, "        Rooms: %d\r\n", nRooms );
    page( ch, "   Vnum Range: %d to %d\r\n", min, max );
  }

  page( ch, "Approx. Level: %d\r\n\r\n", area->level );

  help_link( ch->link, area->help );

  return;
}


void do_roomlist( char_data* ch, char* argument )
{
  room_data*  room;
  char tmp[ TWO_LINES ];

  if( !ch || !ch->array || ( room = Room( ch->array->where ) ) == NULL ) {
    send( ch, "You aren't in a room.\r\n" );
    return;
  } 

  if( !strcasecmp( argument, "lag" ) ) {
    page( ch, "%-6s %11s %11s %11s %11s %11s\r\n", "Vnum", "Total", "Extract", "Resets", "Flags", "Triggers" );
    time_data temp;

    for( room = room->area->room_first; room != NULL; room = room->next ) {
      page( ch, "%-6d %11.3f", room->vnum, room->reset_lag.time( ) / 1000 );

      temp = 0;
      temp.tv_sec = room->room_extracted.tv_sec - room->room_start.tv_sec;
      temp.tv_usec = room->room_extracted.tv_usec - room->room_start.tv_usec;
      page( ch, " %11.3f", temp.time( ) / 1000 );
      
      temp = 0;
      temp.tv_sec = room->room_resetted.tv_sec - room->room_extracted.tv_sec;
      temp.tv_usec = room->room_resetted.tv_usec - room->room_extracted.tv_usec;
      page( ch, " %11.3f", temp.time( ) / 1000 );

      temp = 0;
      temp.tv_sec = room->room_flagged.tv_sec - room->room_resetted.tv_sec;
      temp.tv_usec = room->room_flagged.tv_usec - room->room_resetted.tv_usec;
      page( ch, " %11.3f", temp.time( ) / 1000 );

      temp = 0;
      temp.tv_sec = room->room_end.tv_sec - room->room_flagged.tv_sec;
      temp.tv_usec = room->room_end.tv_usec - room->room_flagged.tv_usec;
      page( ch, " %11.3f", temp.time( ) / 1000 );

      page( ch, "\r\n" );
    }

    return;
  }

  page( ch, "%-6s %-25s %6s %11s %11s %11s\r\n", "Vnum", "Name", "Resets", "Last", "Average", "Total" );

  for( room = room->area->room_first; room != NULL; room = room->next ) {
    strcpy( tmp, room->name );
    truncate( tmp, 25 );
    page( ch, "%-6d %-25s %6d %11.3f %11.3f %11.3f\r\n", room->vnum, tmp, room->reset_count, room->reset_lag.time( ) / 1000, room->total_lag.time( ) / (1000 * room->reset_count ), room->total_lag.time( ) / 1000 );
  }
}

