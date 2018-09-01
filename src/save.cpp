#include "system.h"

/*
 *   save_list types
 *   'S' - string
 *   'I' - array of 32 bit integers
 *   'i' - array of 8 bit integers
 *   'P' - pfile name and ident
 *   'O' - list of objects
 *   
 */


typedef class Save_Entry  save_entry;


class Save_Entry
{
public:
  char*             name;
  char              type;
  int             length;
  void*             pntr;
  void*              std;
};


bool  file_list              ( FILE*, save_entry*,  char*, int = MEM_UNKNOWN );
bool  file_descr             ( FILE*, descr_data*,  char* = empty_string );
bool  file_wizard            ( FILE*, wizard_data*, char* = empty_string );
bool  file_player            ( FILE*, player_data*, char* = empty_string );
bool  file_pfile             ( FILE*, pfile_data*,  char* = empty_string );
bool  file_object            ( FILE*, obj_data*,    char* = empty_string );
bool  file_pet               ( FILE*, char_data*,   char* = empty_string );

void  write_item             ( FILE*, save_entry* );
bool  read_item              ( FILE*, save_entry*, char*, int = MEM_UNKNOWN );
void  read_string            ( FILE*, char**, char, int = MEM_UNKNOWN );
void  write_string           ( FILE*, const char*, char );
void  fread_until            ( FILE*, const char* );
void  write_pet              ( FILE*, char_data* );
bool  read_pet               ( FILE*, player_data*, char* );
void  null_object            ( FILE*, content_array&, char* );
void  fix                    ( player_data* );
void  fix                    ( obj_data* );
bool  read_char              ( FILE*, wizard_data*, player_data* );
/*
 *   EXTERNAL ROUTINES
 */


void   read_affects       ( FILE*, char_data* );
void   write_affects      ( FILE*, char_data* );
void   write_affects      ( FILE*, obj_clss_data* );


/* 
 *   LOCAL CONSTANTS
 */


int zero  = 0;
int one   = 1; 

char_data* oload_owner = NULL;  // for boot log


/*
 *   SUPPORT ROUTINES
 */

// file_list - processes a list of save_entry items either reading or writing them

// fp     - pointer to the open file from which to load from / write to
// item   - the list of save entries
// tmp    - if empty_string then it writes the entire list otherwise it loads the specified item from the list
// memory - type of memory to allocate using
bool file_list( FILE* fp, save_entry* item, char* tmp, int memory )
{
  int i;

  if( tmp == empty_string ) {
    for( i = 0; item[i].name[0] != '\0'; i++ ) 
      write_item( fp, &item[i] );
  } else {
    for( i = 0; item[i].name[0] != '\0'; i++ ) {
      if( !strncmp( tmp, item[i].name, 4 ) ) {
        if( !read_item( fp, &item[i], tmp, memory ) )
          return FALSE;
        *tmp = '\0';
        fread( tmp, 4, 1, fp );
      }
    }
  }

  return TRUE;
}


void write_item( FILE* fp, save_entry* item )
{
  int                i;
  unsigned char      c;
  void*           pntr  = item->pntr;
  int              num  = item->length;
  char            type  = item->type;
  void*            std  = item->std;
  content_array*      list;

  /*-- STRINGS --*/

  if( toupper( type ) == 'S' ) {
    if( pntr == NULL || ( std != NULL && !strcasecmp( *((char**)pntr), *((char**)std) ) ) )
      return;
    fwrite( item->name, 4, 1, fp );
    write_string( fp, *((char**)pntr), type );
    return;
  }

  /*-- INTEGERS --*/

  if( toupper( type ) == 'I' ) {
    if( pntr == NULL )
      return;
    if( std != NULL ) {
      for( i = 0; ((int*)pntr)[i] == ((int*)std)[i]; i++ )
        if( i == num-1 )
          return;
    }
    fwrite( item->name, 4, 1, fp );
    if( type == 'I' ) {
      fwrite( pntr, num*sizeof( int ), 1, fp );
    } else {
      for( i = 0; i < num; i++ ) {
        c = ((int*)pntr)[i];
        fwrite( &c, 1, 1, fp );
      }
    }
    return;
  }

  /*-- PFILES --*/

  if( type == 'P' ) {
    if( *(pfile_data**)pntr != NULL && ( std == NULL || *(pfile_data**)pntr != *(pfile_data**)std ) ) {
      fwrite( item->name, 4, 1, fp );
      fwrite( &(*(pfile_data**)pntr)->ident, sizeof( int ), 1, fp );
    }
    return;
  }

  /*-- OBJECTS --*/

  if( type == 'O' ) {
    list = (content_array*) pntr; 
    if( !is_empty( *list ) ) {
      fwrite( item->name, 4, 1, fp );
      write_object( fp, *list );
    }
  }

  return;
}


bool read_item( FILE* fp, save_entry* item, char* tmp, int memory )
{
  void*           pntr  = item->pntr;
  int              num  = item->length;
  char            type  = item->type;
  unsigned char      c;
  int             i, j;

  /*-- STRINGS --*/

  if( toupper( type ) == 'S' ) {
    read_string( fp, (char**) pntr, type, memory );

  /*-- INTEGERS (8 bit) --*/

  } else if( type == 'i' ) {
    for( i = 0; i < num; i++ ) {
      fread( &c, 1, 1, fp );
      if( pntr != NULL )
        ((int*)pntr)[i] = c;
    }

  /*-- INTEGERS (32 bit) --*/

  } else if( type == 'I' ) {
    if( pntr == NULL ) {
      for( i = 0; i < num; i++ ) 
        fread( &j, sizeof( int ), 1, fp );
    } else
      fread( pntr, sizeof( int ), num, fp );

  /*-- OBJECTS --*/

  } else if( type == 'O' ) {
    content_array* list = (content_array*) pntr;
    return read_object( fp, *list, tmp );

  /*-- PFILES --*/

  } else if( type == 'P' ) {
    fread( &i, sizeof( int ), 1, fp );    
    if( i < 0 || i >= MAX_PFILE ) {
      roach( "Read_Item: Impossible Pfile Index %d.", i );
    } else {     
      pntr = ident_list[i];
    }
  }

  return TRUE;
}


void write_string( FILE* fp, const char* string, char type )
{
  unsigned char  c;
  int            i;

  if( type == 's' ) {
    c = strlen( string ); 
    fwrite( &c, 1, 1, fp ); 
    fwrite( string, int( c ), 1, fp );
  } else {
    i = strlen( string );
    fwrite( &i, sizeof( int ), 1, fp );
    fwrite( string, i, 1, fp );
  }

  return;
}


void read_string( FILE* fp, char** pntr, char type, int memory )
{
  unsigned char  c;
  int            i;
  char*     string;

  if( type == 's' ) {
    fread( &c, 1, 1, fp );
    i = int( c );
  } else {
    fread( &i, sizeof( int ), 1, fp );
  }

  if( i == 0 ) {
    string = empty_string;
  } else {
    string    = new char [ i+1 ];
    string[i] = '\0';
    fread( string, i, 1, fp );
    record_new( i+1, -memory );
  }

  if( pntr != NULL )
    *pntr = string;

  return;
}


void fread_until( FILE* fp, const char* text )
{
  char      tmp  [ ONE_LINE ];
  int    length  = strlen( text );
  int         i;

  for( i = 0; i < length; i++ ) {
    fread( &tmp[i], 1, 1, fp );
    if( tmp[i] != text[i] )
      i = -1;
  }

  return;
}


/*
 *   READ/WRITE CHARACTER ROUTINES
 */


bool file_wizard( FILE* fp, wizard_data* imm, char* tmp )
{
  save_entry list [] = {
    { "Bfin", 's', 0,  &imm->bamfin,               NULL     },
    { "Bfot", 's', 0,  &imm->bamfout,              NULL     },
    { "LvTl", 's', 0,  &imm->level_title,          NULL     },
    { "Offc", 'I', 1,  &imm->office,               &zero    },
    { "PrmF", 'I', 2,  imm->permission,            NULL     },
    { "WzIn", 'i', 1,  &imm->wizinvis,             &zero    },
    { "",     ' ', 0,  NULL,                       NULL     }
  };

  return file_list( fp, list, tmp, MEM_WIZARD );
}


bool file_player( FILE* fp, player_data* player, char* tmp )
{
  save_entry list [] = {
    { "Age.", 'I', 1,   &player->base_age,                  NULL  },
    { "Algn", 'i', 1,   &player->shdata->alignment,         NULL  },
    { "Bank", 'I', 1,   &player->bank,                      &zero },
    { "Cflg", 'I', 4,   player->pcdata->cflags,             NULL  },
    { "Clss", 'i', 1,   &player->pcdata->clss,              NULL  },
    { "ColR", 'i', 25,  NULL,                               NULL  },
    { "COLR", 'i', 30,  NULL,                               NULL  },
    { "CoLr", 'I', 35,  NULL,                               NULL  }, 
    { "Colr", 'I', 45,  &player->pcdata->color,             NULL  }, 
    { "Cond", 'I', 4,   player->pcdata->condition,          NULL  },
    { "Dths", 'I', 1,   &player->shdata->deaths,            NULL  },
    { "Exp.", 'I', 1,   &player->exp,                       NULL  },
    { "Fame", 'I', 1,   &player->shdata->fame,              NULL  },
    { "Gssp", 'I', 1,   &player->gossip_pts,                NULL  },
    { "HpMv", 'I', 6,   &player->hit,                       NULL  },
    { "iFlg", 'I', 1,   NULL,                               NULL  },
    { "IFlg", 'I', 2,   player->iflag,                      NULL  },
    { "Inv.", 'O', 0,   &player->contents,                  NULL  },
    { "Lang", 'i', 1,   &player->pcdata->speaking,          NULL  },
    { "Levl", 'i', 1,   &player->shdata->level,             NULL  },
    { "Lock", 'O', 0,   &player->locker,                    NULL  }, 
    { "LvHt", 'I', 4,   &player->pcdata->level_hit,         NULL  },
    { "Msgs", 'I', 1,   &player->pcdata->message,           NULL  },
    { "MsSt", 'I', 1,   &player->pcdata->mess_settings,     NULL  },
    { "Note", 'S', 0,   NULL,                               NULL  },
    { "Piet", 'I', 1,   &player->pcdata->piety,             NULL  },
    { "Play", 'I', 1,   &player->played,                    NULL  },
    { "Posi", 'i', 1,   &player->position,                  NULL  },
    { "Prac", 'I', 1,   &player->pcdata->practice,          NULL  },
    { "Prpt", 'S', 0,   &player->pcdata->prompt,            NULL  },
    { "Pryr", 'I', 1,   &player->prayer,                    NULL  },
    { "QsFl", 'I', 128, player->pcdata->quest_flags,        NULL  },
    { "QsPt", 'I', 1,   &player->pcdata->quest_pts,         NULL  },
    { "Race", 'i', 1,   &player->shdata->race,              NULL  },
    { "Reli", 'i', 1,   &player->pcdata->religion,          NULL  },
    { "Rept", 'i', 13,  NULL,                               NULL  },
    { "RpAl", 'I', 9,   player->reputation.alignment,       NULL  },          
    { "RpNt", 'I', 15,  player->reputation.nation,          NULL  },
    { "RpGd", 'I', 1,   &player->reputation.gold,           NULL  }, 
    { "RpBd", 'I', 1,   &player->reputation.blood,          NULL  }, 
    { "RpMg", 'I', 1,   &player->reputation.magic,          NULL  }, 
    { "Sex ", 'i', 1,   &player->sex,                       NULL  },
    { "Spkg", 'i', 1,   &player->pcdata->speaking,          NULL  },
    { "Stat", 'i', 5,   &player->shdata->strength,          NULL  },
    { "StFg", 'I', 1,   &player->status,                    NULL  },
    { "Term", 'I', 2,   &player->pcdata->terminal,          NULL  },
    { "TmZn", 'i', 1,   &player->timezone,                  NULL  },
    { "TmKy", 'S', 0,   &player->pcdata->tmp_keywords,      NULL  },
    { "TmAp", 'S', 0,   &player->pcdata->tmp_short,         NULL  },
    { "Titl", 's', 0,   &player->pcdata->title,             NULL  },
    { "Trst", 'i', 1,   &player->pcdata->trust,             NULL  },
    { "Wimp", 'I', 1,   &player->pcdata->wimpy,             NULL  },
    { "Worn", 'O', 0,   &player->wearing,                   NULL  },
    { "",     ' ', 0,   NULL,                               NULL  }
  };

  return file_list( fp, list, tmp, MEM_PLAYER );
}


bool file_pfile( FILE* fp, pfile_data* pfile, char* tmp )
{
  save_entry list [] = {
    { "Crtd", 'I', 1,   &pfile->created,    NULL  },
    { "Hmpg", 'S', 0,   &pfile->homepage,   NULL  }, 
    { "Idnt", 'I', 1,   &pfile->ident,      NULL  },
    { "LsHt", 's', 1,   &pfile->last_host,  NULL  },
    { "LtOn", 'I', 1,   &pfile->last_on,    NULL  },
    { "PlFg", 'I', 2,   pfile->flags,       NULL  },
    { "Pswd", 's', 0,   &pfile->pwd,        NULL  },
    { "Repu", 'I', 18,  NULL,               NULL  },
    { "Sett", 'I', 1,   &pfile->settings,   NULL  },
    { "",     ' ', 0,   NULL,               NULL  }
  };

  return file_list( fp, list, tmp, MEM_PFILE );
}


bool file_descr( FILE* fp, descr_data* descr, char* tmp )
{
  save_entry list [] = {
    { "Appr", 's', 0,   &descr->singular,   NULL  },
    { "Kywd", 's', 0,   &descr->keywords,   NULL  },
    { "Long", 'S', 0,   &descr->complete,   NULL  },
    { "Name", 's', 0,   &descr->name,       NULL  },
    { "",     ' ', 0,   NULL,               NULL  }
  };

  return file_list( fp, list, tmp, MEM_DESCR );
}

void write( player_data* ch )
{
  FILE*                    fp;
  cast_data*          prepare;
  recognize_data*   recognize;
  room_data*             room;
  char_data*              pet;
  wizard_data*            imm;
  int               i, j, val;
  int                  played;
  int                 last_on;

  if( ch == NULL || !ch->Is_Valid() || (ch->link != NULL && ch->link->connected != CON_PLAYING))
    return;

  rename_file( PLAYER_DIR, ch->descr->name, PLAYER_PREV_DIR, ch->descr->name );

  dereference( ch );

  if( ( fp = open_file( PLAYER_DIR, ch->descr->name, "wb" ) ) == NULL ) 
    return;

  played                      = ch->played;
  last_on                     = ch->pcdata->pfile->last_on;
  ch->played                  = ch->time_played( );
  ch->pcdata->pfile->last_on  = time(0);

  if( ( imm = wizard( ch ) ) != NULL && ch->pcdata->trust >= LEVEL_AVATAR ) 
    file_wizard( fp, imm ); 

  file_player( fp, ch );
  file_pfile( fp, ch->pcdata->pfile );
  file_descr( fp, ch->descr );

  /*-- WRITE ROOM --*/

  if( ( room = ch->was_in_room ) != NULL  || ( room = ch->in_room ) != NULL ) {
    fwrite( "Room", 4, 1, fp );
    fwrite( &room->vnum, sizeof( int ), 1, fp );
  }

  /*-- WRITE ACCOUNT --*/

  if( ch->pcdata->pfile->account != NULL ) { 
    fwrite( "Acnt", 4, 1, fp );
    write_string( fp, ch->pcdata->pfile->account->name, 's' );
  } 

  /*-- WRITE SKILLS --*/
  for( i = j = 0; i < MAX_SKILL; i++ )
    if( ch->pcdata->skill[i] != 0 ) 
      j++;

   if( j != 0 ) {
    fwrite( "Skl2", 4, 1, fp );
    fwrite( &j, sizeof( int ), 1, fp );
    for( i = 0; i < MAX_SKILL; i++ ) {
      if( ch->pcdata->skill[i] != 0 ) {
        write_string( fp, skill_table[i].name, 's' );
        val = ch->pcdata->skill[i];
        fwrite( &val, sizeof( int ), 1, fp );
      }
    }
  }

  /*-- WRITE SKILL USAGE --*/
  for( i = j = 0; i < MAX_SKILL; i++ ) 
    if( ch->pcdata->skill_usage[i] != 0 ) 
      j++;

  if( j != 0 ) {
    fwrite( "Usg2", 4, 1, fp );
    fwrite( &j, sizeof( int ), 1, fp );
    for( i = 0; i < MAX_SKILL; i++ ) {
      if( ch->pcdata->skill_usage[i] != 0 ) {
        write_string( fp, skill_table[i].name, 's' );
        val = ch->pcdata->skill_usage[i];
        fwrite( &val, sizeof( int ), 1, fp );
      }
    }
  }

  /*-- WRITE ALIASES --*/

  if( !is_empty( ch->alias ) ) {
    fwrite( "Alas", 4, 1, fp );
    fwrite( &ch->alias.size, sizeof( int ), 1, fp );
    for( i = 0; i < ch->alias; i++ ) {
      write_string( fp, ch->alias[i]->abbrev,  's' );
      write_string( fp, ch->alias[i]->command, 's' );  
    }
  }

  /*-- WRITE VARIABLES --*/

  if( ch->variables.size > 0 ) {
    variable_nav list( &ch->variables );
    for( variable_data* var = list.first( ); var; var = list.next( ) ) {
      fwrite( "Var2", 4, 1, fp );
      write_string( fp, var->get_name(), 's' );
      val = var->get_value( );
      fwrite( &val, sizeof( int ), 1, fp );
    }
  }

  /*-- WRITE RECOGNIZE CODE --*/

  if( ( recognize = ch->pcdata->recognize ) != NULL ) { 
    fwrite( "Recg", 4, 1, fp );
    fwrite( &recognize->size, sizeof( int ), 1, fp );
    fwrite( recognize->list, sizeof( int ), recognize->size, fp );
  }

  /*-- WRITE PREPARED SPELLS --*/

  if( ( prepare = ch->prepare ) != NULL ) {
    fwrite( "Prep", 4, 1, fp );
    i = count( prepare );
    fwrite( &i, sizeof( int ), 1, fp );
    for( ; prepare != NULL; prepare = prepare->next ) {
      fwrite( &prepare->spell, sizeof( int ), 1, fp );
      fwrite( &prepare->times, sizeof( int ), 1, fp );
      fwrite( &prepare->mana,  sizeof( int ), 1, fp );
    }
  }

  /*-- WRITE PETS --*/

  for( i = 0; i < ch->followers.size; i++ ) {
    pet = ch->followers.list[i];
    if( is_set( &pet->status, STAT_PET ) )
      write_pet( fp, pet );
  }

  /*-- WRITE AFFECTS --*/

  write_affects( fp, ch );

  fwrite( "End.", 4, 1, fp );

  fclose( fp );

  ch->played                 = played;
  ch->pcdata->pfile->last_on = last_on;
  ch->save_time              = max( time(0), ch->save_time+60 );
}


bool load_char( link_data* link, char* name, const char* dir )
{
  char              tmp  [ TWO_LINES ];
  pfile_data*     pfile;
  player_data*       ch;
  wizard_data*      imm = NULL;
  FILE*              fp;

  sprintf( tmp, "%s%s", dir, name );

  if( !*name || ( fp = fopen( tmp, "rb" ) ) == NULL )
    return FALSE;

  if( ( pfile = find_pfile_exact( name ) ) == NULL ) {
    pfile = new pfile_data( name );
    imm   = new wizard_data( name );
    ch    = imm;
  } else if( pfile->trust >= LEVEL_AVATAR ) {
    imm = new wizard_data( name );
    ch  = imm;
  } else 
    ch = new player_data( name );
  
  ch->active.owner_name = str_dup( ch->Name( ) );

  link->player      = ch;
  link->character   = ch;
  link->pfile       = pfile;
  ch->pcdata->pfile = pfile;
  ch->link          = link;

  if( !read_char( fp, imm, ch ) ) {
    roach( "Load_Char: File corrupted!" );
    roach( "-- Name: %s", name );
    roach( "-- Dir: %s", dir );

    if( dir != PLAYER_DIR ) {
      // if we're not loading from the player directory, don't try for a prev backup
      // MEMORY LEAK -- need to delete anything newed above
      return FALSE;
    }

    sprintf( tmp, "%s%s", PLAYER_PREV_DIR, name );
    if( ( fp = fopen( tmp, "rb" ) ) == NULL || !read_char( fp, imm, ch ) ) 
      panic( "Load_Char: Prev file corrupted!" );
    roach( "Load_Char: Prev file intact." );
  }

  set_owner( ch->pcdata->pfile, ch->contents );
  set_owner( ch->pcdata->pfile, ch->locker );
  set_owner( ch->pcdata->pfile, ch->wearing ); 

  fix( ch );

  update_maxes( ch );
  calc_resist( ch );
  modify_pfile( ch );

  if( boot_stage == BOOT_COMPLETE )
    reference( ch, ch->contents, -1 );

  return TRUE;
}


bool read_char( FILE* fp, wizard_data* imm, player_data* ch )
{
  char              tmp  [ TWO_LINES ];
//  bool            valid  = TRUE;
  int         i, j, val;
  char*          string;
  unsigned char       c;
  
  *tmp = '\0';
  fread( tmp, 4, 1, fp );

  oload_owner = ch; // global for oload trace

  if( imm != NULL && !file_wizard( fp, imm, tmp ) )
    return FALSE;

  // wtf do we loop 20 times for?
  for( i = 0; i < 20; i++ ) { 
    file_player( fp, ch, tmp );
    file_pfile( fp, ch->pcdata->pfile, tmp );
    file_descr( fp, ch->descr, tmp ); 
  }

  /*-- READ ROOM --*/

  if( !strncmp( tmp, "Room", 4 ) ) {
    fread( &i, sizeof( int ), 1, fp );
    ch->was_in_room = get_room_index( i );
    fread( tmp, 4, 1, fp );
  }

  /*-- READ ACCOUNT -- */

  if( !strncmp( tmp, "Acnt", 4 ) ) {
    read_string( fp, &string, 's', MEM_UNKNOWN );
    if( ( ch->pcdata->pfile->account = find_account( string ) ) == NULL ) {
      roach(   "Fread_Char: Non-existent account." );
      roach(   "--   Ch = %s", ch->pcdata->pfile->name );
      /*      panic( "-- Acnt = %s", string );*/
    }
    free_string( string, MEM_UNKNOWN );
    fread( tmp, 4, 1, fp );
  }    

  /*-- READ SKILLS --*/

  if( !strncmp( tmp, "Skil", 4 ) ) {
    fread( &i, sizeof( int ), 1, fp );
    for( ; i > 0; i-- ) {
      read_string( fp, &string, 's', MEM_UNKNOWN ); 
      fread( &c, 1, 1, fp );
      if( ( j = skill_index( string ) ) != -1 )
        ch->pcdata->skill[j] = int( c );
      free_string( string, MEM_UNKNOWN );
    }
    fread( tmp, 4, 1, fp );
  }

  if( !strncmp( tmp, "Skl2", 4 ) ) {
    fread( &i, sizeof( int ), 1, fp );
    for( ; i > 0; i-- ) {
      read_string( fp, &string, 's', MEM_UNKNOWN ); 
      fread( &val, sizeof( int ), 1, fp );
      if( ( j = skill_index( string ) ) != -1 )
        ch->pcdata->skill[j] = val;
      free_string( string, MEM_UNKNOWN );
    }
    fread( tmp, 4, 1, fp );
  }

  /*-- READ SKILL USAGE --*/
  
  if( !strncmp( tmp, "Usag", 4 ) ) {
    fread( &i, sizeof( int ), 1, fp );
    for( ; i > 0; i-- ) {
      read_string( fp, &string, 's', MEM_UNKNOWN );
      fread( &c, 1, 1, fp );
      if( ( j = skill_index( string ) ) != -1 )
        ch->pcdata->skill_usage[j] = int( c );
      free_string( string, MEM_UNKNOWN );
    }
    fread( tmp, 4, 1, fp );
  }
  
  if( !strncmp( tmp, "Usg2", 4 ) ) {
    fread( &i, sizeof( int ), 1, fp );
    for( ; i > 0; i-- ) {
      read_string( fp, &string, 's', MEM_UNKNOWN );
      fread( &val, sizeof( int ), 1, fp );
      if( ( j = skill_index( string ) ) != -1 )
        ch->pcdata->skill_usage[j] = val;
      free_string( string, MEM_UNKNOWN );
    }
    fread( tmp, 4, 1, fp );
  }

  /*-- READ ALIASES --*/

  if( !strncmp( tmp, "Alas", 4 ) ) {
    fread( &ch->alias.size, sizeof( int ), 1, fp );
    ch->alias.list = new alias_data* [ ch->alias.size ];
    for( i = 0; i < ch->alias.size; i++ ) {
      ch->alias.list[i] = new alias_data( empty_string, empty_string );
      read_string( fp, &ch->alias[i]->abbrev,  's', MEM_ALIAS ); 
      read_string( fp, &ch->alias[i]->command, 's', MEM_ALIAS );
    }
    fread( tmp, 4, 1, fp );
  }

  /*-- READ VARIABLES --*/

  for( ; !strncmp( tmp, "Vars", 4 ); ) {
    char *name;
    read_string( fp, &name, 's', MEM_UNKNOWN );
    fread( &c, 1, 1, fp );

    if( name ) {
      ch->Set_Integer( name, c );
      free_string( name, MEM_UNKNOWN );
    }

    fread( tmp, 4, 1, fp );
  }

  for( ; !strncmp( tmp, "Var2", 4 ); ) {
    char *name;
    read_string( fp, &name, 's', MEM_UNKNOWN );
    fread( &val, sizeof( int ), 1, fp );

    if( name ) {
      ch->Set_Integer( name, val );
      free_string( name, MEM_UNKNOWN );
    }

    fread( tmp, 4, 1, fp );
  }

  /*-- READ RECOGNIZE CODE --*/

  if( !strncmp( tmp, "Recg", 4 ) ) {
    fread( &i, sizeof( int ), 1, fp );
    ch->pcdata->recognize = new recognize_data( i );
    fread( ch->pcdata->recognize->list, sizeof( int ), i, fp );
    fread( tmp, 4, 1, fp );
  }

  /*-- READ PREPARED SPELLS --*/

  if( !strncmp( tmp, "Prep", 4 ) ) {
    fread( &i, sizeof( int ), 1, fp );
    for( ; i > 0; i-- ) {
      cast_data* prepare = new cast_data;
      fread( &prepare->spell, sizeof( int ), 1, fp );
      fread( &prepare->times, sizeof( int ), 1, fp );
      fread( &prepare->mana,  sizeof( int ), 1, fp );
      append( ch->prepare, prepare );
    }
    fread( tmp, 4, 1, fp );
  }

  /*-- READ PETS --*/

  for( ; !strncmp( tmp, "Pet.", 4 ); ) {
    if( !read_pet( fp, ch, tmp ) ) {
      fclose( fp );
      return FALSE;
    }
    fread( tmp, 4, 1, fp );
  }

  /*-- READ AFFECTS --*/

  if( !strncmp( tmp, "Afft", 4 ) ) {
    // read old-format affects
    read_affects( fp, ch, true );
    fread( tmp, 4, 1, fp );
  }
 
  if( !strncmp( tmp, "Aff2", 4 ) ) {
    // read new-format affects
    read_affects( fp, ch, false );
    fread( tmp, 4, 1, fp );
  }
 
  fclose( fp );

  oload_owner = NULL; // global for oload trace

  if( strncmp( tmp, "End.", 4 ) ) {
    roach( "Load_Char: Missing 'End.'." );
    // roach( "--  Tmp = '%s'", tmp ); 
    roach( "-- File = '%s'", ch->descr->name );
    return FALSE;
  }

  return TRUE;
}


void fix( player_data* ch )
{
  int term             = ch->pcdata->terminal;
  thing_data*  person  = (thing_data*) ch;
  room_data*     room;
  thing_data*   roomt;

  if( not_in_range( ch->pcdata->religion, 0, MAX_ENTRY_RELIGION-1 ) ) {
    bug( "Fix_Player: Impossible religion." );
    bug( "-- Ch = %s", ch->descr->name );
    ch->pcdata->religion = 0;
  }

  if( term != TERM_ANSI && term != TERM_MXP ) 
    for( int i = 0; i < MAX_COLOR; i++ )
      if( ch->pcdata->color[i] < 0 || ch->pcdata->color[i] >= term_table[ term ].entries )
        ch->pcdata->color[i] = 0;

  if( get_language( ch, ch->pcdata->speaking ) == 0 ) // || ( ch->shdata->level < LEVEL_APPRENTICE && ch->pcdata->speaking == LANG_COMMON ) )
    ch->pcdata->speaking = LANG_COMMON; // ( ch->shdata->level >= LEVEL_APPRENTICE ? LANG_COMMON : LANG_HUMANIC+ch->shdata->race );

  for( int j = 0; j < MAX_SKILL; j++ ) {
    if( !is_apprentice( ch ) ) {

      // remove create spells from rangers
      if( ch->pcdata->clss == CLSS_RANGER ) {
        if( j == SPELL_CREATE_FOOD ) {
          if( ch->real_skill( SPELL_CREATE_FOOD ) != 0 )
             ch->pcdata->skill[SKILL_FORAGE] = ch->real_skill( SPELL_CREATE_FOOD );
        }
        
        if( j == SPELL_CREATE_WATER ) {
          if( ch->real_skill( SPELL_CREATE_WATER ) != 0 )
            ch->pcdata->skill[SKILL_DOWSE] = ch->real_skill( SPELL_CREATE_WATER ); 
        }
        
        if( j == SPELL_CREATE_LIGHT ) {
          if( ch->real_skill( SPELL_CREATE_LIGHT ) != 0 )
            ch->pcdata->skill[SKILL_FIND_TINDER] = ch->real_skill( SPELL_CREATE_LIGHT );
        }
        
        if( j == SPELL_CURE_POISON ) {
          if( ch->real_skill( SPELL_CURE_POISON ) != 0 )
            ch->pcdata->skill[SKILL_DRAW_POISON] = ch->real_skill( SPELL_CURE_POISON );
        }
      }
      
      // remove out-of-spec skills from players
      if( ch->real_skill( j ) < 0 || ch->real_skill( j ) > 10 || skill_table[j].level[ch->pcdata->clss] < 0 || skill_table[j].prac_cost[ch->pcdata->clss] < 0 )
        ch->pcdata->skill[j] = 0;
    }
  }

  if( person->Get_Integer( CAMP_SITE ) != 0 )
    if( ( room = get_room_index( person->Get_Integer( CAMP_SITE ), FALSE ) ) != NULL ) {
      roomt = (thing_data*) room;
      roomt->Set_Integer( CAMP_SITE, 0 );
      roomt->Set_Integer( CAMP_SITE_REGEN, 0 );
    }
  person->Set_Integer( CAMP_SITE, 0 );

  if( ch->pcdata->practice > 40+2*ch->shdata->level )
    ch->pcdata->practice = 20+2*ch->shdata->level;

  remove_bit( &ch->status, STAT_PET );
  remove_bit( &ch->status, STAT_IN_GROUP );
  remove_bit( &ch->status, STAT_FAMILIAR );
  remove_bit( &ch->status, STAT_LEAPING );
  remove_bit( &ch->status, STAT_FOLLOWER );
  remove_bit( &ch->status, STAT_STUNNED );
  remove_bit( &ch->status, STAT_POLYMORPH );
  remove_bit( &ch->status, STAT_ALERT );
  remove_bit( &ch->status, STAT_TAMED );
  remove_bit( &ch->status, STAT_BERSERK );
  remove_bit( &ch->status, STAT_GAINED_EXP );
  remove_bit( &ch->status, STAT_SENTINEL );
  remove_bit( &ch->status, STAT_REPLY_LOCK );
  remove_bit( &ch->status, STAT_AGGR_ALL );
  remove_bit( &ch->status, STAT_AGGR_GOOD );
  remove_bit( &ch->status, STAT_AGGR_EVIL );
  remove_bit( &ch->status, STAT_ORDERED );
  remove_bit( &ch->status, STAT_AGGR_NEUT );
}


void backup( player_data* ch, const char* directory )
{
  if( ch == NULL || !ch->Is_Valid() || ( ch->link != NULL && ch->link->connected != CON_PLAYING ) || directory == NULL || *directory == '\0' || strcmp( directory, PLAYER_DIR ) == 0 )
    return;

  copy_file( PLAYER_DIR, ch->descr->name, directory, ch->descr->name );
}

/*
 *   READ/WRITE OBJECT ROUTINES
 */


bool file_object( FILE* fp, obj_data* obj, char* tmp )
{
  obj_clss_data*  oc  = obj->pIndexData;

  save_entry list [] = {
    { "Cond", 'I', 1, &obj->condition,        NULL              },
    { "Cntn", 'O', 0, &obj->contents,         NULL              },
    { "ExFl", 'I', 2, obj->extra_flags,       &oc->extra_flags  },
    { "Labl", 's', 0, &obj->label,            &empty_string     },
    { "Layr", 'I', 1, &obj->layer,            NULL              },
    { "Mat.", 'I', 1, &obj->materials,        &oc->materials    },
    { "Numb", 'I', 1, &obj->number,           &one              },
    { "Ownr", 'p', 0, &obj->owner,            NULL              },
    { "Plur", 's', 0, &obj->plural,           &oc->plural       },
    { "Repa", 'I', 1, &obj->age,              NULL              },
    { "Rust", 'i', 1, &obj->rust,             &zero             },
    { "Sing", 's', 0, &obj->singular,         &oc->singular     },
    { "Size", 'I', 1, &obj->size_flags,       &oc->size_flags   },
    { "Sour", 's', 0, &obj->source,           &empty_string     },
    { "Time", 'I', 1, &obj->timer,            NULL              },
    { "Valu", 'I', 4, obj->value,             NULL              },
    { "Wear", 'I', 1, &obj->position,         NULL              },
    { "Wght", 'I', 1, &obj->weight,           &oc->weight       },
    { "MbVn", 'I', 1, &obj->reset_mob_vnum,   NULL              },
    { "RmVn", 'I', 1, &obj->reset_room_vnum,  NULL              },
    { "ChOn", 'I', 1, &obj->reset_chances[0], NULL              },
    { "ChTw", 'I', 1, &obj->reset_chances[1], NULL              },
    { "ChTh", 'I', 1, &obj->reset_chances[2], NULL              },
    { "",     ' ', 0, NULL,                   NULL              }
  };
 
  return file_list( fp, list, tmp, MEM_OBJECT );
};


void write_object( FILE* fp, content_array& list )
{
  obj_data* obj;
  int val;

  for( int i = 0; i < list; i++ ) {
    if( ( obj = object( list[i] ) ) == NULL || is_set( obj->pIndexData->extra_flags, OFLAG_NOSAVE ) )
      continue;

    if( obj->pIndexData->item_type == ITEM_ARMOR
     || obj->pIndexData->item_type == ITEM_WEAPON 
     || obj->pIndexData->item_type == ITEM_ARROW
     || obj->pIndexData->item_type == ITEM_SHIELD 
     || obj->pIndexData->item_type == ITEM_BOLT
     || obj->pIndexData->item_type == ITEM_STONE ) {
      obj->value[1] = obj->pIndexData->value[1];
      obj->value[2] = obj->pIndexData->value[2];
      obj->weight   = obj->pIndexData->weight;
      obj->size_flags = obj->pIndexData->size_flags;
    }
    fwrite( "Obj.", 4, 1, fp );
    fwrite( &obj->pIndexData->vnum, sizeof( int ), 1, fp );  

    file_object( fp, obj );
    write_affects( fp, obj );

    /*-- WRITE VARIABLES --*/

    if( obj->variables.size > 0 ) {
      variable_nav list( &obj->variables );
      for( variable_data* var = list.first( ); var; var = list.next( ) ) {
        fwrite( "Var2", 4, 1, fp );
        write_string( fp, var->get_name(), 's' );
        val = var->get_value( );
        fwrite( &val, sizeof( int ), 1, fp );
      }
    }

    fwrite( "End!", 4, 1, fp );
  }

  fwrite( "End!", 4, 1, fp );
}


bool read_object( FILE* fp, content_array& array, char* tmp )
{
  obj_data*              obj;
  obj_clss_data*    obj_clss;
  int                 i, val;
  unsigned char            c;

  for( ; ; ) {
    *tmp = '\0';
    fread( tmp, 4, 1, fp );

    if( !strncmp( tmp, "End!", 4 ) )
      break;

    if( strncmp( tmp, "Obj.", 4 ) ) { 
      roach( "Read_Obj: Expected word 'Obj.' missing." );
      return FALSE;
    }

    fread( &i, sizeof( int ), 1, fp );

    *tmp = '\0';
    fread( tmp, 4, 1, fp );

    if( ( obj_clss = get_obj_index( i ) ) == NULL ) {
      roach( "Read_Obj: Non-existent object type!" );
      roach( "-- Vnum = %d", i );
      roach( "--  Loc = %s", location( &array ) );
      null_object( fp, array, tmp );
      continue;
    }

    obj = create( obj_clss, -1 );

    if( !file_object( fp, obj, tmp ) )
      return FALSE;;

    if( !strncmp( tmp, "Afft", 4 ) ) {
      // read old-format affects
      read_affects( fp, obj, true );
      *tmp = '\0';
      fread( tmp, 4, 1, fp );
    }

    if( !strncmp( tmp, "Aff2", 4 ) ) {
      // read new-format affects
      read_affects( fp, obj, false );
      *tmp = '\0';
      fread( tmp, 4, 1, fp );
    }

    if( !strncmp( tmp, "ExFg", 4 ) ) {
      fread( &i, sizeof( int ), 1, fp );
      fread( tmp, 4, 1, fp );
    }

    /*-- READ VARIABLES --*/

    for( ; !strncmp( tmp, "Vars", 4 ); ) {
      char *name;
      read_string( fp, &name, 's', MEM_UNKNOWN );
      fread( &c, 1, 1, fp );

      if( name ) {
        obj->Set_Integer( name, c );
        free_string( name, MEM_UNKNOWN );
      }

      fread( tmp, 4, 1, fp );
    }

    for( ; !strncmp( tmp, "Var2", 4 ); ) {
      char *name;
      read_string( fp, &name, 's', MEM_UNKNOWN );
      fread( &val, sizeof( int ), 1, fp );

      if( name ) {
        obj->Set_Integer( name, val );
        free_string( name, MEM_UNKNOWN );
      }

      fread( tmp, 4, 1, fp );
    }

    if( strncmp( tmp, "End!", 4 ) ) {
      roach( "Read_Obj: Missing 'End!'." );
      return FALSE;
    }

    if( boot_stage < BOOT_COMPLETE )
      obj->pIndexData->count += obj->number;

    obj->To( &array );

    if( boot_stage != BOOT_COMPLETE && obj->source != NULL && obj->source != empty_string )
      log( "%s has oloaded item #%d (%s) [%s]", ( oload_owner ? oload_owner->Name( ) : "???" ), obj->pIndexData->vnum, obj->Name( ), obj->source );

    fix( obj );
  }

  return TRUE;
}


void null_object( FILE* fp, content_array& array, char* tmp )
{
  char      buf1  [ ONE_LINE ];
  char      buf2  [ ONE_LINE ];
  char*    text1  = "End!";
  char*    text2  = "Cntn";
  int          i;
  int          j;

  for( i = j = 0; i < 4; i++, j++ ) {
    if( fread( &buf1[i], 1, 1, fp ) != 1 ) 
      panic( "Null_Object: End of File." );

    buf2[j] = buf1[i];
 
    if( buf1[i] != text1[i] )
      i = -1;
    if( buf2[j] != text2[j] )
      j = -1;

    if( j == 3 ) {
      read_object( fp, array, tmp );
      i = j = -1;
    }
  }

  return;
};


void fix( obj_data* obj )
{
//  obj_clss_data*  obj_clss  = obj->pIndexData;

//  if( !is_set( &obj_clss->size_flags, SFLAG_CUSTOM ) && !is_set( &obj_clss->size_flags, SFLAG_RANDOM ) )
//    obj->size_flags = obj_clss->size_flags;

  return;
}

/*
 *  CROSS-REFERENCE ITEMS
 */


void dereference( player_data* ch )
{
  if (!ch || !ch->Is_Valid())
    return;
  
  obj_array* array  = &ch->save_list;

  for( int i = 0; i < array->size; i++ )
    if( array->list[i] != NULL )
      array->list[i]->save = NULL; 

  clear( *array );

  return;
}


int reference( player_data* ch, thing_array& obj, int i )
{
  /*
  obj_array*  array  = &ch->save_list;

  if( obj == NULL )
    return i;

  if( i == -2 ) {
    for( ; obj != NULL; obj = obj->next_content ) {
      if( obj->save != NULL ) {
        if( is_set( &obj->save->pcdata->message, MSG_AUTOSAVE ) )
          send( obj->save, "Autosave forced by quitting of %s.\r\n", ch );
        write( obj->save );
        }
      reference( ch, obj->contents, -2 );
      } 
    return -2;
    }

  if( i == -1 ) {
    if( array->size != 0 ) 
      panic( "Reference: Non-empty object array." );
    array->size = count_contents( player );
    array->list = new obj_data* [ array->size ]; 
    i = 0;
    }

  for( ; obj != NULL; obj = obj->next_content ) {
    if( obj->save != NULL ) {
      if( is_set( &obj->save->pcdata->message, MSG_AUTOSAVE ) )
        send( obj->save, "Autosave forced by saving of %s.\r\n", ch );
      write( obj->save );
      }
    obj->save = ch;
    array->list[i++] = obj;
    i = reference( ch, obj->contents, i );
    }
  */
  return i;
}


/*
 *   READ/WRITE PET ROUTINES
 */


bool file_pet( FILE* fp, char_data* pet, char* tmp )
{
  save_entry list [] = {
    { "HpMv",  'I', 6,  &pet->hit,        NULL     },
    { "Inv.",  'O', 0,  &pet->contents,   NULL     },
    { "Name",  's', 0,  &pet->pet_name,   NULL     },
    { "Posi",  'i', 1,  &pet->position,   NULL     },
    { "Sex ",  'i', 1,  &pet->sex,        NULL     },
    { "StFg",  'I', 1,  &pet->status,     NULL     },
    { "Time",  'I', 1,  &pet->timer,      NULL     },
    { "Worn",  'O', 0,  &pet->wearing,    NULL     },
    { "",      ' ', 0,  NULL,             NULL     }
  };

  return file_list( fp, list, tmp, MEM_MOBS );
}


void write_pet( FILE* fp, char_data* pet )
{  
  room_data* room;
  int val;

  fwrite( "Pet.", 4, 1, fp );
  fwrite( &pet->species->vnum, sizeof( int ), 1, fp );

  file_pet( fp, pet );

  /*-- WRITE ROOM --*/

  if( ( room = pet->in_room ) != NULL || ( room = pet->was_in_room ) != NULL ) {
    fwrite( "Room", 4, 1, fp );
    fwrite( &room->vnum, sizeof( int ), 1, fp );
  }

  write_affects( fp, pet );

  /*-- WRITE VARIABLES --*/

  if( pet->variables.size > 0 ) {
    variable_nav list( &pet->variables );
    for( variable_data* var = list.first( ); var; var = list.next( ) ) {
      fwrite( "Var2", 4, 1, fp );
      write_string( fp, var->get_name(), 's' );
      val = var->get_value( );
      fwrite( &val, sizeof( int ), 1, fp );
    }
  }

  fwrite( "End&", 4, 1, fp );

  return;
};


bool read_pet( FILE* fp, player_data* player, char* tmp )
{
  char_data*            pet;
  species_data*     species;
  int                i, val;
  unsigned char           c;

  fread( &i, sizeof( int ), 1, fp );

  if( ( species = get_species( i ) ) == NULL ) {
    roach( "Read_Pet: Unknown species." );
    roach( "-- Vnum = %d", i );    
    fread_until( fp, "End&" );
    return TRUE;
  } 

  pet = create_mobile( species );

  fread( tmp, 4, 1, fp );

  if( !file_pet( fp, pet, tmp ) ) 
    return FALSE;

  /*-- READ ROOM --*/

  if( !strncmp( tmp, "Room", 4 ) ) {
    fread( &i, sizeof( int ), 1, fp );
    pet->was_in_room = get_room_index( i );
    fread( tmp, 4, 1, fp );
  }

  /*-- READ AFFECTS --*/

  if( !strncmp( tmp, "Afft", 4 ) ) {
    // read old-format affects
    read_affects( fp, pet, true );
    fread( tmp, 4, 1, fp );
  }

  if( !strncmp( tmp, "Aff2", 4 ) ) {
    // read new-format affects
    read_affects( fp, pet, false );
    fread( tmp, 4, 1, fp );
  }

  /*-- READ VARIABLES --*/

  for( ; !strncmp( tmp, "Vars", 4 ); ) {
    char *name;
    read_string( fp, &name, 's', MEM_UNKNOWN );
    fread( &c, 1, 1, fp );

    if( name ) {
      pet->Set_Integer( name, c );
      free_string( name, MEM_UNKNOWN );
    }

    fread( tmp, 4, 1, fp );
  }

  for( ; !strncmp( tmp, "Var2", 4 ); ) {
    char *name;
    read_string( fp, &name, 's', MEM_UNKNOWN );
    fread( &val, sizeof( int ), 1, fp );

    if( name ) {
      pet->Set_Integer( name, val );
      free_string( name, MEM_UNKNOWN );
    }

    fread( tmp, 4, 1, fp );
  }

  if( strncmp( tmp, "End&", 4 ) ) {
    roach( "Read_Pet: Missing 'End&'." );
    return FALSE;
  }

  set_bit( &pet->status, STAT_PET );
  remove_bit( &pet->status, STAT_AGGR_ALL );
  remove_bit( &pet->status, STAT_AGGR_GOOD );
  remove_bit( &pet->status, STAT_AGGR_EVIL );

  pet->Set_Integer( "/Squeed/Code/PetId", 0 );
  pet->exp = pet->Get_Integer( "/Squeed/Code/Exp" );
  //bug( "reading %s\r\n", pet );
  //bug( "xp set to %d\r\n", pet->exp );
  add_follower( pet, player, FALSE );
  pet->shdata->level += pet->Get_Integer( "/Squeed/Code/Levels" );
  update_maxes( pet );

  if( is_set( &pet->status, STAT_FAMILIAR ) )
    player->familiar = pet;
  
  return TRUE;
}


/*
 *   DATA FILE SAVE ROUTINES
 */


void save_mobs( )
{
  FILE*               fp;
  descr_data*      descr;
  mprog_data*      mprog;
  species_data*  species;
  share_data*     shdata;

  rename_file( AREA_DIR, MOB_FILE, AREA_PREV_DIR, MOB_FILE );

  if( ( fp = open_file( AREA_DIR, MOB_FILE, "wb" ) ) == NULL )
    return;

  fprintf( fp, "#M2_MOBILES\n" );
  fprintf( fp, "%d\n\n", 9 ); // version

  for( int i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL )
      continue;

    shdata = species->shdata;
    descr  = species->descr;

    fprintf( fp, "\n#%d\n", species->vnum );
    fprintf( fp, "%s~\n", descr->name );
    fprintf( fp, "%s~\n", descr->keywords );
    fprintf( fp, "%s~\n", descr->singular );
    fprintf( fp, "%s~\n", descr->prefix_s );
    fprintf( fp, "%s~\n", descr->adj_s );
    fprintf( fp, "%s~\n", descr->long_s );
    fprintf( fp, "%s~\n", descr->plural );
    fprintf( fp, "%s~\n", descr->prefix_p );
    fprintf( fp, "%s~\n", descr->adj_p );
    fprintf( fp, "%s~\n", descr->long_p );
    fprintf( fp, "%s~\n", descr->complete );
    fprintf( fp, "%s~\n", species->creator );
    fprintf( fp, "%s~\n", species->tame_msg );
    fprintf( fp, "%s~\n", species->rtame_msg );
    fprintf( fp, "%s~\n", species->no_bash_msg );
    fprintf( fp, "%s~\n", species->attack->code );
    
    write_extras( fp, species->attack->data );
    fprintf( fp, "!\n" );

    fprintf( fp, "%d %d %d %d %d %d %d %d\n",
      species->nation, species->group, shdata->race,
      species->adult, species->maturity,
      species->skeleton, species->zombie, species->corpse );
    fprintf( fp, "%d %d %d %d %d %d %d %d %d %d %d %d\n",
      species->price, shdata->kills, shdata->deaths,
      species->wander, species->date, species->light,
      species->color, species->compan_str, species->compan_amt,
      species->celestial, species->dire, species->fiendish );
    fprintf( fp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", species->act_flags[0],
      species->act_flags[1], species->affected_by[0],
      species->affected_by[1], species->affected_by[2],
      species->affected_by[3], species->affected_by[4],
      species->affected_by[5], species->affected_by[6],
      species->affected_by[7], species->affected_by[8],
      species->affected_by[9], species->affected_by[10],
      shdata->alignment );
    fprintf( fp, "%d\n", shdata->level );
    fprintf( fp, "%d %d %d %d %d\n",
      shdata->strength, shdata->intelligence,
      shdata->wisdom, shdata->dexterity,
      shdata->constitution );
    fprintf( fp, "%d %d %d %d %d %d %d %d\n",
      shdata->resist[RES_MAGIC], shdata->resist[RES_FIRE],
      shdata->resist[RES_COLD], shdata->resist[RES_SHOCK],
      shdata->resist[RES_MIND], shdata->resist[RES_ACID],
      shdata->resist[RES_POISON], shdata->resist[RES_HOLY] );

    fprintf( fp, "%d %d %d %d %d %d %d %d %d %d\n",
      shdata->modify_damage[ ATT_PHYSICAL ],
      shdata->modify_damage[ ATT_FIRE ],
      shdata->modify_damage[ ATT_COLD ],
      shdata->modify_damage[ ATT_ACID ],
      shdata->modify_damage[ ATT_SHOCK ],
      shdata->modify_damage[ ATT_MIND ],
      shdata->modify_damage[ ATT_MAGIC ],
      shdata->modify_damage[ ATT_POISON ],
      shdata->modify_damage[ ATT_HOLY ],
      shdata->modify_damage[ ATT_UNHOLY ] );

    fprintf( fp, "%d %d\n", species->low_challenge, species->high_challenge );
    
    for( int j = 0; j < MAX_ARMOR; j++ )
      fprintf( fp, "%d %d %s~\n", species->chance[j],
        species->armor[j], species->part_name[j] );
    fprintf( fp, "%d\n", species->wear_part );

    fprintf( fp, "%d %d\n", species->hitdice, species->movedice );
    // fprintf( fp, "%d %d %d %d %d\n", species->damage, species->rounds, species->special, species->damage_taken, species->exp );
    fprintf( fp, "%c %d %d %d\n",
      toupper( sex_name[species->sex][0] ),
      species->gold, species->size, species->weight );

    write( fp, species->reset );

    for( mprog = species->mprog; mprog != NULL; mprog = mprog->next ) {
      fprintf( fp, "%d %d\n", mprog->trigger, mprog->value );
      fprintf( fp, "%s~\n", mprog->string );
      fprintf( fp, "%s~\n", mprog->code );
      write_extras( fp, mprog->data );
      fprintf( fp, "!\n" );
    }

    fprintf( fp, "-1\n" );   
  }     

  fprintf( fp, "#0\n\n" );
  fprintf( fp, "#$\n" );
  fclose( fp );

  return;
}


void save_objects( )
{
  FILE*                 fp;
  obj_clss_data*  obj_clss;
  oprog_data*        oprog;
  int                    i;
  int                count = 0;

  rename_file( AREA_DIR, OBJECT_FILE, AREA_PREV_DIR, OBJECT_FILE );

  if( ( fp = open_file( AREA_DIR, OBJECT_FILE, "wb" ) ) == NULL ) 
    return;

  fprintf( fp, "#M2_OBJECTS\n" );
  fprintf( fp, "%d\n\n", 12 ); // version

  for( i = 0; i < MAX_OBJ_INDEX; i++ ) {
    if( ( obj_clss = obj_index_list[i] ) == NULL )
      continue;

    fprintf( fp, "\n#%d\n",    obj_clss->vnum );
    fprintf( fp, "%s~\n",      obj_clss->singular );
    fprintf( fp, "%s~\n",      obj_clss->plural );
    fprintf( fp, "%s~\n",      obj_clss->before );
    fprintf( fp, "%s~\n",      obj_clss->after );
    fprintf( fp, "%s~\n",      obj_clss->long_s );
    fprintf( fp, "%s~\n",      obj_clss->long_p );
    fprintf( fp, "%s~\n",      obj_clss->prefix_singular );
    fprintf( fp, "%s~\n",      obj_clss->prefix_plural );
    fprintf( fp, "%s~\n",      obj_clss->creator );
    fprintf( fp, "%s~\n",      obj_clss->last_mod );

    fprintf( fp, "%d %d %d %d %d %d\n",
      obj_clss->item_type,         obj_clss->fakes,
      obj_clss->extra_flags[0],    obj_clss->extra_flags[1],
      obj_clss->wear_flags,        obj_clss->anti_flags );

    fprintf( fp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
      obj_clss->restrictions,      obj_clss->size_flags,
      obj_clss->materials,
      obj_clss->affect_flags[0],   obj_clss->affect_flags[1],
      obj_clss->affect_flags[2],   obj_clss->affect_flags[3],
      obj_clss->affect_flags[4],   obj_clss->affect_flags[5],
      obj_clss->affect_flags[6],   obj_clss->affect_flags[7],
      obj_clss->affect_flags[8],   obj_clss->affect_flags[9],
      obj_clss->layer_flags
    );

    fprintf( fp, "%d %d %d %d\n", obj_clss->value[0],
      obj_clss->value[1], obj_clss->value[2], obj_clss->value[3] );
       
    fprintf( fp, "%d %d %d %d %d %d %d %d %d %d\n",
      obj_clss->weight,
      obj_clss->cost, obj_clss->level, obj_clss->limit,
      obj_clss->repair, obj_clss->durability, obj_clss->blocks,
      obj_clss->light, obj_clss->color, obj_clss->religion_flags ); 
/*
    for( int i = 0; i < MAX_SKILL; i++ ) {
      if( obj_clss->skill_modifier[i] != 0 )
        count++;
    }
*/
    fprintf( fp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", 
      obj_clss->clss_synergy[ CLSS_THIEF+1 ], 
      obj_clss->clss_synergy[ CLSS_MAGE+1 ],
      obj_clss->clss_synergy[ CLSS_SORCERER+1 ], 
      obj_clss->clss_synergy[ CLSS_CLERIC+1 ], 
      obj_clss->clss_synergy[ CLSS_DANCER+1 ], 
      obj_clss->clss_synergy[ CLSS_DRUID+1 ], 
      obj_clss->clss_synergy[ CLSS_WARRIOR+1 ],
      obj_clss->clss_synergy[ CLSS_CAVALIER+1 ], 
      obj_clss->clss_synergy[ CLSS_BARBARIAN+1 ],      
      obj_clss->clss_synergy[ CLSS_PALADIN+1 ],
      obj_clss->clss_synergy[ CLSS_RANGER+1 ], 
      obj_clss->clss_synergy[ CLSS_DEFENSIVE+1 ],
      obj_clss->clss_synergy[ CLSS_MONK+1 ], 
      obj_clss->clss_synergy[ CLSS_ROGUE+1 ], 
      obj_clss->clss_synergy[ CLSS_ASSASSIN+1 ], 
      count );

    // Saving Skill Synergies (hopefully)

/*
    int col = 0;
    for( int i = 0; i < MAX_SKILL; i++ )
      fprintf( fp, "%d%s", obj_clss->skill_modifier[i], ++col%40 == 0 ? "\n" : " " );
    fprintf( fp, "\n" );

    if( count > 0 ) {
      for( int i = 0; i < MAX_SKILL; i++ ) {
        if( obj_clss->skill_modifier[i] != 0 ) {
          fprintf( fp, "%s~\n", skill_table[i].name );
          fprintf( fp, "%d\n", obj_clss->skill_modifier[i] );
        }
      }
    }
    count = 0; */

    fprintf( fp, "%d\n", obj_clss->synergy_array.size );

    for( int i = 0; i < obj_clss->synergy_array; i++ )
      obj_clss->synergy_array[i]->Save( fp );

    fprintf( fp, "%d\n", int( obj_clss->date ) );

    write_affects( fp, obj_clss );
    write_extras( fp, obj_clss->extra_descr );

    fprintf( fp, "P\n" );
    for( oprog = obj_clss->oprog; oprog != NULL; oprog = oprog->next ) {
      fprintf( fp, "%d %d\n", oprog->trigger, oprog->obj_act == NULL ? -1 : oprog->obj_act->vnum );
      fprintf( fp, "%d\n", oprog->value );
      fprintf( fp, "%s~\n", oprog->command );
      fprintf( fp, "%s~\n", oprog->target );
      fprintf( fp, "%s~\n", oprog->code );
      write_extras( fp, oprog->data );
      fprintf( fp, "!\n" );
    }

    fprintf( fp, "-1\n" );   
  }     
      
  fprintf( fp, "#0\n\n" );
  fprintf( fp, "#$\n" );
  fclose( fp );
}






