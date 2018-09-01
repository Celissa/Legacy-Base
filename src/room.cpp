#include "system.h"


const char* rflag_name[ MAX_RFLAG ] =
{
  "Lit", "Safe", "Indoors", "No.Mob",
  "No.Recall", "No.Magic", "No.Scan", "Altar", "Services.Shop",
  "Rdesc.Prog", "Town.Scan", "Bank", "Shop", "Pet.Shop", "Office",
  "No.Pray", "Save.Items", "Underground", "Auction.House",
  "Reset0", "Reset1", "Reset2", "Status0", "Status1", "Status2",
  "No.Mount", "Arena", "#approved", "Save.Variables", 
  "No.Summon.In", "No.Summon.Out", "No.Gate"
};

const char* location_name[ MAX_LOCATION ] = { "Outdoors", "Indoors", "Sunlight", "Full Moon", "Forest", "Abovewater" };

terrain_type terrain [ MAX_TERRAIN ] =
{
  {  "room",              1  },
  {  "town street",       1  },
  {  "field",             2  },
  {  "forest",            2  },
  {  "hills",             3  },
  {  "mountain",          4  },
  {  "water",             3  },
  {  "underwater",        5  },
  {  "river",             5  },
  {  "air",               6  },  
  {  "desert",            4  },
  {  "cave",              4  },
  {  "road",              2  }, 
  {  "shallows",          4  },
  {  "marsh",             5  },
  {  "recovery",          1  },
  {  "beach",             4  },
  {  "ethereal",          4  },
  {  "snow",              4  },
};




/*
 *   LOCAL ROUTINES
 */


void load_room_items( room_data* );


/*
 *   DISK ROUTINES
 */


void load_rooms( FILE* fp, area_data* area )
{
  room_data *room;
  room_data *room2;

  if( strcmp( fread_word( fp ), "#ROOMS" ) )
    panic( "Load_rooms: missing header" );

  for( ; ; ) {
    int vnum;
    char letter;

    letter = fread_letter( fp );

    if( letter != '#' ) 
      panic( "Load_rooms: # not found." );

    if( ( vnum = fread_number( fp ) ) == 0 )
       break;

    if( get_room_index( vnum ) != NULL ) 
      panic( "Load_rooms: vnum %d duplicated.", vnum );

    room               = new room_data;
    room->area         = area;
    room->vnum         = vnum;
    room->name         = fread_string( fp, MEM_ROOM );
    room->description  = fread_string( fp, MEM_ROOM );
    room->comments     = fread_string( fp, MEM_ROOM );
    room->room_flags   = fread_number( fp );
    room->sector_type  = fread_number( fp );
    room->size         = fread_number( fp );
    room->reset        = NULL;

//    remove_bit( &room->room_flags, RFLAG_RESERVED2 );

//    if (is_set(&room->room_flags, RFLAG_NO_AUTOSCAN))
//      log("Room #%d is no.scan\r\n", room->vnum);

    if( room->size < 0 || room->size >= MAX_SIZE )
      room->size = SIZE_HORSE;

    fread_number( fp );

    read_exits( fp, room, vnum );
    read_extra( fp, room->extra_descr );
        
    for( ; ; ) {
      letter = fread_letter( fp );

      if( letter == 'S' )
        break;
  
      if( isdigit( letter ) || letter == '-' ) {
        ungetc( letter, fp );
        load( fp, room->reset, false );
        continue;
      }

      if( letter == 'A' ) {
        read( fp, room->action, room );
        continue;
      }
      
      if( letter == 'V' ) {
        char* name = fread_string( fp, MEM_ROOM );
        int   val  = fread_number( fp );
        if( name && val != 0 )
          room->Set_Integer( name, val );
        continue;
      }

      panic( "Load_rooms: vnum %d has flag not 'DESV'.", vnum );
    }
         
    if( ( room2 = room->area->room_first ) == NULL ) {
      room->area->room_first = room;
      room->next = NULL;
    } else {
      if( room2->vnum > room->vnum ) {
        room->next = room2;
        room->area->room_first = room;
      } else {
        for( ; room2->next != NULL && room2->next->vnum <
          room->vnum; room2 = room2->next );
        room->next = room2->next;
        room2->next = room;
      }
    }
  }

  return;
}


/*
 *   SAVE/LOAD ROOM ITEMS
 */


void save_room_items( room_data* room )
{
  if( !is_set( &room->room_flags, RFLAG_SAVE_ITEMS ) )
    return;

  FILE*        fp;
  char*      file  = static_string( );

  sprintf( file,   "Room.%d", room->vnum );
  rename_file( ROOM_DIR, file, ROOM_PREV_DIR, file );

  if( ( fp = open_file( ROOM_DIR, file, "wb" ) ) == NULL ) 
    return;

  write_object( fp, room->contents );
  fclose( fp );
}  


void load_room_items( )
{
  log( "Loading Room Items ..." ); 

  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) )
    for( room_data *room = area->room_first; room != NULL; room = room->next ) 
      if( is_set( &room->room_flags, RFLAG_SAVE_ITEMS ) ) 
        load_room_items( room );

  return;
}


void load_room_items( room_data* room )
{ 
  FILE*        fp;
  char*      file  = static_string( );  
  char*       tmp  = static_string( );  
  bool       flag;    
           
  sprintf( file, "Room.%d", room->vnum );

  if( ( fp = open_file( ROOM_DIR, file, "rb", FALSE ) ) != NULL ) {
    flag = read_object( fp, room->contents, tmp );
    fclose( fp );
    if( flag )
      return;  
    roach( "Load_Room_Items: Room %d corrupted.", room->vnum );
  }

  if( ( fp = open_file( ROOM_PREV_DIR, file, "rb", FALSE ) ) == NULL )
    return; 

  if( !read_object( fp, room->contents, tmp ) ) 
    panic( "Previous file also corrupted!" );            

  fclose( fp );
}


/*
 *   GET_ROOM_INDEX FUNCTION
 */


room_data* get_room_index( int vnum, bool err )
{
  room_data*  room;

  area_nav alist(&area_list);
  for (area_data *area = alist.first(), *next; area; area = next) {
    next = alist.next();
    if( next != NULL && next->room_first != NULL && next->room_first->vnum <= vnum )
      continue; 
    for( room = area->room_first; room != NULL; room = room->next )
      if( room->vnum == vnum )
        return room;
  }

  if( err )
    panic( "Get_room_index: bad vnum %d.", vnum );

  return NULL;
}


/*
 *   SUPPORT ROUTINES
 */


bool can_edit( char_data* ch, room_data* room, bool msg )
{
  if( is_set( &room->room_flags, RFLAG_APPROVED ) && !has_permission( ch, PERM_ROOM_APPROVAL ) ) {
    send( ch, "This room is approved and may not be altered.\r\n" );
    return FALSE;
    }

  if( has_permission( ch, PERM_ALL_ROOMS )
    || is_name( ch->descr->name, room->area->creator ) )
    return TRUE;

  if( msg )
    send( ch, "You don't have permission to alter this room.\r\n" );
  return FALSE;
}


const char* room_name( room_data* room )
{
  static char tmp [ 15 ];  

  if( room == NULL )
    return "nowhere??";

  sprintf( tmp, "Room #%d", room->vnum );
  return tmp;
}
  

/*
 *   ONLINE ROOM COMMANDS
 */


void do_rbug( char_data* ch, char* argument ) 
{
  ch->in_room->area->modified = TRUE;

  ch->in_room->comments = edit_string( ch, argument, ch->in_room->comments,
    MEM_ROOM );
}


void do_rdesc( char_data *ch, char *argument ) 
{
  room_data*      room  = ch->in_room;
  wizard_data*  wizard  = (wizard_data*) ch;
  char               tmp  [ MAX_STRING_LENGTH ];
  
  if( argument[0] != '\0' ) {
    if( !can_edit( ch, room ) ) 
      return;
    sprintf( tmp, "Rdesc: %s", argument );
    room_log( ch, room->vnum, tmp );
  }

  room->area->modified = TRUE;

  if( wizard->room_edit == NULL ) 
    room->description = edit_string( ch,
      argument, room->description, MEM_ROOM );
  else 
    wizard->room_edit->text = edit_string( ch, 
      argument, wizard->room_edit->text, MEM_EXTRA );
}

void do_rflag_find( char_data* ch, char* argument )
{
  int            i = 0;
  area_nav alist(&area_list);
  const char*  arg = argument;

  if( ch == NULL )
    return;

  if( *argument == '\0' ) {
    send( ch, "You must include an argument in this command.\r\n" );
    return;
  }

  for( i = 0; i < MAX_RFLAG; i++ ) {
    const char* flag = rflag_name[i];
    if( exact_match( arg, flag ) )
      break;
  }

  if( i == MAX_RFLAG ) {
    send( ch, "That is an unrecognized rflag, type rflag if you wish to see which are usable.\r\n" );
    return;
  }

  for( area_data *tarea = alist.first( ); tarea; tarea = alist.next( ) ) {
    for( room_data* troom = tarea->room_first; troom != NULL; troom = troom->next ) {
      if( is_set( &troom->room_flags, i ) )
        page( ch, "%s [%i] has %s set.\r\n", troom->name, troom->vnum, rflag_name[i] );
    }
  }

  return;
}


    
void do_rflag( char_data* ch, char* argument )
{
  int              flags;
  int               prev;
  room_data*        room  = ch->in_room;
  const char*   response;

  if( !get_flags( ch, argument, &flags, "a", "Rflag" ) )
    return;

  if( *argument == '\0' ) {
    display_flags( "Room", &rflag_name[0], &rflag_name[1], &room->room_flags, MAX_RFLAG, ch );

    page( ch, "\r\n" );
    page_title( ch, "Variables of %s", room->name );

    if( room->variables.size > 0 ) {
      variable_nav list( &room->variables );
      for( variable_data* var = list.first( ); var; var = list.next( ))
        page( ch, "%39s : %-5d\r\n", var->get_name( ), var->get_value( ));
    }
    else
     page( ch, "<none defined>\r\n" );
 
     return;
  }
     
  if( !can_edit( ch, room ) )
    return;

  prev = room->room_flags;

  if( ( response = set_flags( &rflag_name[0], &rflag_name[1], &room->room_flags, MAX_RFLAG, NULL, ch, argument, FALSE, TRUE ) ) == NULL )  
    return;

  room_log( ch, ch->in_room->vnum, response );
  room->area->modified = TRUE;

  if( !is_set( &flags, 0 ) )
    return;

  for( room = room->area->room_first; room != NULL; room = room->next ) 
    alter_flags( &room->room_flags, &ch->in_room->room_flags, &prev, MAX_RFLAG );

  send( ch, "- Set on Area -\r\n" );
}


void do_rset( char_data* ch, char* argument )
{
  room_data*    room  = ch->in_room;
  area_data*    area  = room->area;
  int          flags;
  int           terr;
  int           size;

  if( !get_flags( ch, argument, &flags, "a", "Rset" ) )
    return;
  
  if( *argument == '\0' ) {
    do_rstat( ch, "" );
    return;
  }

  if( *argument != '\0' ) {
    if( !can_edit( ch, room ) ) 
      return;
    ch->in_room->area->modified = TRUE;
  }

  class string_field string_list [] = {
    { "area",      MEM_AREA,  &area->name,     NULL            },
    { "creator",   MEM_AREA,  &area->creator,  NULL            },
    { "help",      MEM_AREA,  &area->help,     NULL            },
    { "filename",  MEM_AREA,  &area->file,     &set_area_file  },
    { "tax.entry", MEM_AREA,  &area->tax_rate, NULL            },
    { "bonus.entry",MEM_AREA,  &area->bonus,    NULL            },
    { "",          0,         NULL,            NULL            },   
  };

  if( process( string_list, ch, area->name, argument ) )
    return;

  class int_field int_list [] = {
    { "level",            1,     90,  &area->level       },
    { "reset time",       0,    200,  &area->reset_time  },
    { "continent",        0,    100,  &area->continent   },
    { "",                 0,      0,  NULL               }, 
  };
               
  if( process( int_list, ch, area->name, argument ) )
    return;

  terr  = room->sector_type;
  size  = room->size;

#define as( i )   area_status[i]
#define tn( i )   terrain_table[i].entry_name
#define sn( i )   size_name[i]

  class type_field type_list [] = {
    { "status",  MAX_AREA_STATUS,  &as(0),  &as(1),  &area->status       },
    { "terrain", MAX_ENTRY_TERRAIN,&tn(0),  &tn(1),  &room->sector_type  },
    { "size",    MAX_SIZE,         &sn(0),  &sn(1),  &room->size         }, 
    { "",        0,                NULL,    NULL,    NULL                }
  };

#undef as
#undef tn
#undef sn

  if( process( type_list, ch, is_set( &flags, 0 ) ? area->name : "room", argument ) ) {
    if( is_set( &flags, 0 ) ) {
      if( room->size != size ) {
        size = room->size;
        for( room = room->area->room_first; room != NULL; room = room->next ) 
          room->size = size;
      } else if( room->sector_type != terr ) {
        terr = room->sector_type;
        for( room = room->area->room_first; room != NULL; room = room->next ) 
          room->sector_type = terr;
      }
    }
    return;
  }

  send( ch, "Syntax: rset <field> ....\r\n" );
}


void do_rname( char_data* ch, char* argument ) 
{
  if( !can_edit( ch, ch->in_room ) ) 
    return;

  if( *argument == '\0' ) {
    send( ch, "You need to call the room something.\r\n" );
    return;
    }

  ch->in_room->area->modified = TRUE;

  free_string( ch->in_room->name, MEM_ROOM );
  ch->in_room->name = alloc_string( argument, MEM_ROOM );
  send( "Room name changed.\r\n", ch );
}


void do_rstat( char_data* ch, char* )
{
  char           tmp  [ MAX_STRING_LENGTH ];
  room_data*    room  = ch->in_room;
  area_data*    area  = room->area;
  bool         found;

  page( ch, "        Name: %s.\r\n", room->name );
  page( ch, "        Area: %s\r\n", area->name );
  page( ch, "    Filename: %s%s\r\n", AREA_DIR, area->file );
  page( ch, "     Creator: %-12s\r\n", area->creator );
  page( ch, "   Help File: %s\r\n", area->help );
  page( ch, "        Vnum: %d\r\n", room->vnum );
  page( ch, "       Light: %-14d Weight: %.2f\r\n", room->Light(0), (float) room->contents.weight/100 );
  page( ch, "  Reset Rate: %-15d Level: %d\r\n", area->reset_time, area->level );
  page( ch, "     Terrain: %-12s Max_Size: %-13s Status: %s\r\n", terrain_table[ room->sector_type ].terrain_name, size_name[room->size], area_status[ area->status ] );
  page( ch, "   Continent: %d\r\n", area->continent );
  page( ch, "    Tax_Rate: %s\r\n", area->tax_rate );
  page( ch, "       Bonus: %s\r\n", area->bonus );

  found = FALSE;
  strcpy( tmp, "       Exits:" );
  for( int i = 0; i < room->exits; i++ ) {
    sprintf( tmp+strlen( tmp ), " %s", dir_table[ room->exits[i]->direction ].name );
    found = TRUE;
  }
  sprintf( tmp+strlen( tmp ), "%s\r\n", found ? "" : " none" );
  page( ch, tmp );

  found = FALSE;
  strcpy( tmp, "  Exits From:" );
  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) )
    for( room = area->room_first; room != NULL; room = room->next )
      for( int i = 0; i < room->exits; i++ )
        if( room->exits[i]->to_room == ch->in_room ) {
          found = TRUE;
          sprintf( tmp+strlen( tmp ), " %d", room->vnum );
        }

  sprintf( tmp+strlen( tmp ), "%s\r\n", found ? "" : " none" );
  page( ch, tmp );

  room = ch->in_room;
  page( ch, scroll_line[0] );

  sprintf( tmp, "Description:\r\n%s", room->description );
  page( ch, tmp );

  show_extras( ch, room->extra_descr );
}


bool can_extract( room_data* room, char_data* ch )
{
  trainer_data*    trainer;
  char_data*     character;

  if( room->vnum == 1 ) {
    send( ch, "You can't delete the construct.\r\n" );
    return FALSE;
  }

  if( room->area->room_first == room ) {
    send( ch, "You can't delete the first room in an area.\r\n" );
    return FALSE;
  }

  if( room->exits > 0 ) {
    send( ch, "You must remove all doors from the room.\r\n" );
    return FALSE;
  }
  
  if( room->reset != NULL ) {
    send( ch, "Remove all resets first.\r\n" );
    return FALSE;
  }  

  for( int i = 0; i < room->contents; i++ )
    if( ( character = mob( room->contents[i] ) ) != NULL ) {
      send( ch, "You must purge the room first.\r\n" );
      return FALSE;
    }

  for( trainer = trainer_list; trainer != NULL; trainer = trainer->next )
    if( trainer->room == room ) {
      send( ch, "Room still contains a trainer reference.\r\n" );
      return FALSE;
    }

  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) )
    for( room = area->room_first; room != NULL; room = room->next )
      for( int i = 0; i < room->exits; i++ )  
        if( room->exits[i]->to_room == ch->in_room ) {
          send( ch, "Room %d still has a connection to this room.\r\n", room->vnum );
          return FALSE;
        }

  return TRUE;
}


void do_redit( char_data *ch, char *argument )
{
  char               buf   [ TWO_LINES ];
  char_data*         rch;
  wizard_data*    wizard  = (wizard_data*) ch;
  char               arg   [ MAX_STRING_LENGTH ];
  room_data*        copy;
  room_data*        room = ch->in_room;
  action_data*    action;

  if( !can_edit( ch, ch->in_room ) ) 
    return;
 
  ch->in_room->area->modified = TRUE;

  if( matches( argument, "room" ) ) { 
    wizard->room_edit = NULL;
    send( ch, "Rdesc now operates on current room.\r\n" );
    return;
  }
  
  if( matches( argument, "copy" ) ) {
    if( !one_argument( argument, arg ) ) {
      send( ch, "Syntax: redit copy <room number>\r\n" );
      return;
    }
    if( !is_number( argument ) ) {
      send( ch, "<room number> must be an integer.\r\n" );
      return;
    }
    if( ( copy = get_room_index( atoi( argument ) ) ) == NULL ) {
      send( ch, "Error: Room #%s does not exist.\r\n", arg ); 
      return;
    }
    
    room->name        = alloc_string( copy->name, MEM_ROOM );
    room->description = alloc_string( copy->description, MEM_ROOM );
    room->comments    = alloc_string( copy->comments, MEM_ROOM );
    
    int i = copy->room_flags;
    room->room_flags  = i;
    i = copy->sector_type;
    room->sector_type = i;
    i = copy->size;
    room->size        = i;

    room->reset       = NULL;
    room->action      = NULL;

    for( action = copy->action; action != NULL; action = action->next ) {
      if( action != NULL ) {
        action_data* naction;
        naction = new action_data( room );
        naction->command = alloc_string( action->command, MEM_ACTION );
        naction->code    = alloc_string( action->code, MEM_ACTION );
        naction->room    = room;
        naction->target  = alloc_string( action->target, MEM_ACTION );
        int i = action->trigger;
        naction->trigger = i;
        i = action->value;
        naction->value   = i;
        i = action->flags;
        naction->flags   = i;
        for( i = 0; i < action->data; i++ ) {
          extra_data* ed  = new extra_data;
          ed->keyword  = alloc_string( action->data[i]->keyword, MEM_EXTRA );
          ed->text     = alloc_string( action->data[i]->text, MEM_EXTRA );
          naction->data += ed;
        }
        append( room->action, naction );
      }
    }
    
    for( reset_data* reset = copy->reset; reset != NULL; reset = reset->next ) {
      if( reset != NULL ) {
        reset_data*      nreset;
        nreset = new reset_data;
        void* target = reset->target;
        nreset->target = target;;
        int i = reset->flags;
        nreset->flags = i;
        i = reset->chances;
        nreset->chances = i;
        i = reset->value;
        nreset->value = i;
        i = reset->liquid;
        nreset->liquid = i;
        append( room->reset, nreset );
      }
    }

    for( i = 0; i < copy->extra_descr; i++ ) {
      extra_data* ed  = new extra_data;
      ed->keyword = alloc_string( copy->extra_descr[i]->keyword, MEM_EXTRA );
      ed->text    = alloc_string( copy->extra_descr[i]->text, MEM_EXTRA );
      room->extra_descr += ed;
    }

    send( ch, "Copy Complete.\r\n" );

    return; 
  }

  if( !strcasecmp( argument, "delete room" ) ) {
    room_data* room = ch->in_room;
    if( !can_extract( room, ch ) )
      return;

    if( ch->in_room != room ) {
      send( ch, "You somehow managed to try to delete a room your not in.\r\n" );
      return;
    }

    remove( room->area->room_first, room );

    sprintf( buf, "Room deleted: %d (%s)", ch->in_room->vnum, ch->real_name( ) );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES );
    send( ch, "You delete the room.\r\n" );
    
    for( int i = room->contents.size-1; i >= 0; i-- ) {
      if( ( rch = player( room->contents[i] ) ) != NULL ) {
        send( rch, "The room you are in disappears.\r\n" );
        send( rch, "You find yourself in the construct.\r\n" );
        rch->From( );
        rch->To( get_room_index( ROOM_CONSTRUCT ) );
      }
    }

    delete room;
    
    // ummm, hello?
    // delete room perhaps?

    return;
  } 

  edit_extra( ch->in_room->extra_descr, wizard, offset( &wizard->room_edit, wizard ), argument, "rdesc" );
}


/*
 *   ROOM PROPERTIES
 */


bool Room_Data :: is_dark( )
{
  if( is_set( &room_flags, RFLAG_LIT ) )
     return FALSE;

  return( Light(0) < 19 );
}


bool Room_Data :: is_outside( )
{
  return ( /*!is_set( &room_flags, RFLAG_INDOORS ) &&*/ !is_set( &room_flags, RFLAG_UNDERGROUND ) );
}

/*
 *   LOCATION
 */


bool allowed_location( char_data* ch, int* bit, const char* msg1, const char* msg2 )
{
  if( is_set( bit, LOC_OUTDOORS ) && is_set( &ch->in_room->room_flags, RFLAG_INDOORS ) ) {
    send( ch, "You cannot %s %s inside.\r\n", msg1, msg2 );
    return FALSE;
  } 

  if( is_set( bit, LOC_INDOORS ) && !is_set( &ch->in_room->room_flags, RFLAG_INDOORS ) ) {
    send( ch, "You cannot %s %s outside.\r\n", msg1, msg2 );
    return FALSE;
  } 

  if( is_set( bit, LOC_SUNLIGHT ) && !isday( ) ) {
    send( ch, "You can only %s %s during the day.\r\n", msg1, msg2 );
    return FALSE;
  } 

  if( is_set( bit, LOC_FULLMOON ) && isday( ) ) {
    send( ch, "You can only %s %s while the moon is full.\r\n", msg1, msg2 );
    return FALSE;
  } 

  if( is_set( bit, LOC_FOREST ) && !terrain_table[ ch->in_room->sector_type ].forested ) {
    send( ch, "You can only %s %s while in a forest.\r\n", msg1, msg2 );
    return FALSE;
  }

  if( is_set( bit, LOC_ABOVEWATER ) && terrain_table[ ch->in_room->sector_type ].underwater ) {
    send( ch, "You cannot %s %s underwater.\r\n", msg1, msg2 );
    return FALSE;
  }

  return TRUE;
}


/*
 *   various rooms of interest
 */

room_data* get_town( const char* name, bool system )
{
  room_data* room = NULL;

  for( int i = 0; i < MAX_ENTRY_TOWN ; i++ ) {
    if( !system && *town_table[i].name == '#' )
      continue;
    else if( fmatches( name, town_table[i].name ) ) {
      room = get_room_index( town_table[i].recall );
      break;
    }
  }

  return room;
}


room_data* get_temple( char_data* ch, room_data* death )
{
  room_data*  room  = NULL;
/*  
  send( ch, "Temple Room: %i (good-oc)\r\n", plyr_race_table[ch->shdata->race].start_room[0] );
  send( ch, "Temple Room: %i (neutral-oc)\r\n", plyr_race_table[ch->shdata->race].start_room[1] );
  send( ch, "Temple Room: %i (evil-oc)\r\n", plyr_race_table[ch->shdata->race].start_room[2] );
  send( ch, "Temple Room: %i (good-nc)\r\n", plyr_race_table[ch->shdata->race].nc_room[0] );
  send( ch, "Temple Room: %i (neutral-nc)\r\n", plyr_race_table[ch->shdata->race].nc_room[1] );
  send( ch, "Temple Room: %i (evil-nc)\r\n", plyr_race_table[ch->shdata->race].nc_room[2] ); 
  send( ch, "Death Room: %s  Continent: %i\r\n", death == NULL ? "NULL" : death->name, death == NULL ? -1 : death->area->continent );
  send( ch, "In_Room: %s  Was_In_Room: %s\r\n", ch->in_room == NULL ? "NULL" : ch->in_room->name, ch->was_in_room == NULL ? "NULL" : ch->was_in_room->name );
*/
  if( ch->shdata->race < MAX_PLYR_RACE && ch->start_room == NULL ) {
    if( !clss_table[ ch->pcdata->clss ].nc_start && !plyr_race_table[ ch->shdata->race ].nc_start ) {
//      send( ch, "We're inside NULL was_in_room && !ncstart, room start should be %i\r\n", plyr_race_table[ch->shdata->race].start_room[ch->shdata->alignment%3] );
      room = get_room_index( plyr_race_table[ ch->shdata->race ].start_room[ ch->shdata->alignment%3 ] );
    }
    else {
//      send( ch, "We're inside the NULL was_in_room && this is a ncstart, room should be %i\r\n", plyr_race_table[ch->shdata->race].nc_room[ch->shdata->alignment%3] );
      room = get_room_index( plyr_race_table[ ch->shdata->race ].nc_room[ ch->shdata->alignment%3 ] );
    }
  }
   
  if( room == NULL ) {
    if( is_good( ch ) ) 
      room = get_town( "#start_good", true );
    else if( is_neutral( ch ) ) 
      room = get_town( "#start_neutral", true );
    else if( is_evil( ch ) ) 
      room = get_town( "#start_evil", true );
  }

  if( ch->shdata->race < MAX_PLYR_RACE && death != NULL ) {
//    send( ch, "We are inside the death was true\r\n" );
    if( death->area->continent == OLD_CONTINENT ) {
//      send( ch, "We were on the old continent\r\n" );
      room = get_room_index( plyr_race_table[ ch->shdata->race ].start_room[ ch->shdata->alignment%3] );
    }
    else {
//      send( ch, "We were on the new continent\r\n" );
      room = get_room_index( plyr_race_table[ ch->shdata->race].nc_room[ ch->shdata->alignment%3 ] );
    }
  }
    
  if( room == NULL )
    room = get_town( "#start", true );

  if( room == NULL )
    room = get_room_index( ROOM_CONSTRUCT );

  return room;
}


room_data* get_portal( char_data* ch )
{
  room_data*  room  = NULL;
  
  if( ch->shdata->race < MAX_PLYR_RACE ) 
    room = get_room_index( plyr_race_table[ ch->shdata->race ].portal );

  if( room == NULL ) {
    if( is_good( ch ) )
      room = get_town( "#portal_good", true );
    else if( is_neutral( ch ) )
      room = get_town( "#portal_neutral", true );
    else if( is_evil( ch ) )
      room = get_town( "#portal_evil", true );
  }

  if( room == NULL )
    room = get_town( "#portal", true );

  if( room == NULL )
    room = get_temple( ch );

  return room;
}


room_data* get_death( char_data* ch )
{
  room_data*  room  = NULL;
  
  if( room == NULL ) {
    if( is_good( ch ) )
      room = get_town( "#death_good", true );
    else if( is_neutral( ch ) )
      room = get_town( "#death_neutral", true );
    else if( is_evil( ch ) )
      room = get_town( "#death_evil", true );
  }

  if( room == NULL )
    room = get_town( "#death", true );

  if( room == NULL )
    room = get_temple( ch );

  return room;
}


