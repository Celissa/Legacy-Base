#include "system.h"


/*
 *     DIRECTORY LISTER
 */


#ifdef MYSTERIA_WINDOWS

int dirlist(filelist *list, const char *filespec)
{
  char *item;
  _finddata_t finfo;
  long fhandle;

  if ((fhandle = _findfirst(filespec, &finfo)) != -1) {
    do {
      if (!strcmp(finfo.name, ".") || !strcmp(finfo.name, ".." ) || !strcmp(finfo.name, "create.dir"))
        continue;
      item = new char [strlen(finfo.name) + 1];
      strcpy(item, finfo.name);
      list->add(item);
    } while (!_findnext(fhandle, &finfo));
    _findclose(fhandle);
  }

  return list->size;
}

#elif defined MYSTERIA_UNIX

int dirlist(filelist *list, const char *filespec)
{
  char *item, *dir, *spec;
  DIR *dirp;
  dirent *dp;

  dir = new char [strlen(filespec) + 1];
  memset(dir, '\0', strlen(filespec) + 1);
  strncpy(dir, filespec, (int) (strrchr(filespec, '/') - filespec));

  spec = new char [strlen(filespec) + 1];
  memset(spec, '\0', strlen(filespec) + 1);
  strcpy(spec, (char *) ((int) strrchr(filespec, '*') + 1));

  dirp = opendir(dir);

  for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp)) {
    if(!strcmp( dp->d_name, "." ) || !strcmp( dp->d_name, ".." ) || !strcmp( dp->d_name, "create.dir")) {
      continue;
    } else if (!str_cmp(&dp->d_name[strlen(dp->d_name) - strlen(spec)], spec)) {
      item = new char [strlen(dp->d_name) + 1];
      strcpy(item, dp->d_name);
      list->add(item);
    }
  }

  closedir(dirp);

  delete [] dir;
  delete [] spec;

  return list->size;
}

#else

#error DIRLIST function does not work on your OS.

#endif


/*
 *   FILE SUBROUTINES
 */


FILE* open_file( const char* dir, const char* file, const char* type, bool fatal )
{
  char  tmp  [ TWO_LINES ];

  sprintf( tmp, "%s%s", dir, file );

  return open_file( tmp, type, fatal );
}


FILE* open_file( const char* path, const char* type, bool fatal )
{
  FILE*   fp;

  if( ( fp = fopen( path, type ) ) != NULL )
    return fp;

  roach( "Open_File: %s", strerror( errno ) );
  roach( "-- File = '%s'", path );

  if( fatal )
    throw( 1 );

  return NULL;
}


void copy_file( const char* dir1, const char* file1, const char* dir2, const char* file2 )
{
  delete_file( dir2, file2, false );

  FILE* rfp = open_file( dir1, file1, "rb", false );
  FILE* wfp = open_file( dir2, file2, "wb", false );

  if( !rfp || !wfp )
    return;

  char buf[ 256 ];
  size_t size;

  while( ( size = fread( buf, 1, 256, rfp ) ) > 0 )
    fwrite( buf, 1, size, wfp );

  fclose( rfp );
  fclose( wfp );
}

#ifdef MYSTERIA_WINDOWS

void rename_file( const char* dir1, const char* file1, const char* dir2, const char* file2 )
{
  char* tmp1  = static_string( );
  char* tmp2  = static_string( );

  sprintf( tmp1, "%s%s", dir1, file1 );
  sprintf( tmp2, "%s%s", dir2, file2 );

  remove( tmp2 );
  rename( tmp1, tmp2 );
  remove( tmp1 );
}

void delete_file( const char* dir, const char* file, bool msg )
{
  char* path  = static_string( );
  int      i  = strlen( dir );

  sprintf( path, "%s%s", dir, file );
  path[i] = toupper( path[i] );

  if( remove( path ) == -1 && msg ) {
    roach( "Delete_File: %s", strerror( errno ) );
    roach( "-- File = '%s'", path );
  }
}

#elif defined MYSTERIA_UNIX

void rename_file( const char* dir1, const char* file1, const char* dir2, const char* file2 )
{
  char* tmp1  = static_string( );
  char* tmp2  = static_string( );

  sprintf( tmp1, "%s%s", dir1, file1 );
  sprintf( tmp2, "%s%s", dir2, file2 );

  unlink( tmp2 );
  link( tmp1, tmp2 );
  unlink( tmp1 );
}

void delete_file( const char* dir, const char* file, bool msg )
{
  char* path  = static_string( );
  int      i  = strlen( dir );

  sprintf( path, "%s%s", dir, file );
  path[i] = toupper( path[i] );

  if( unlink( path ) == -1 && msg ) {
    roach( "Delete_File: %s", strerror( errno ) );
    roach( "-- File = '%s'", path );
  }
}

#else

#error RENAME / DELETE FILE function does not work on your OS.

#endif

/*
 *   FREAD ROUTINES
 */


char fread_letter( FILE *fp )
{
  char c;

  do {
    c = getc( fp );
    } while( isspace( c ) );

  return c;
}


int fread_number( FILE *fp )
{
  int number = 0;
  int sign = 1;
  char c;

  do {
    c = getc( fp );
    } while( isspace( c ) );

  switch( c ) {
    case '-' :  sign = -1;
    case '+' :  c = getc( fp );
    }

  if( !isdigit( c ) ) {
    bug( "Fread_number: bad format." );
    log( "%s\n", fread_string( fp, MEM_UNKNOWN ) );
    throw( 1 );
    }

  while( isdigit( c ) ) {
   number = number * 10 + c - '0';
   c = getc( fp );
   }

  number *= sign;

  while( c == ' ' ) {
    c = getc( fp );
    }

  ungetc( c, fp );

  return number;
}


char* fread_string( FILE* fp, int type, bool oldstyle )
{
  char        buf  [ 4*MAX_STRING_LENGTH ];
  int      length  = 0;
  char*    string;
 
  char read;

  do {
    read = getc( fp );
  } while( read == '\r' || read == '\n' || read == ' ' );

  if( oldstyle && read == '.' )
    read = getc( fp );

  while( read != '~' && read != EOF ) {
    if( read == '\r' ) {
      buf[ length++ ] = '\r';
      buf[ length++ ] = '\n';

      read = getc( fp );
      if( read == '\r' )  // some files have been saved with \r\r\n for some weird reason...
        read = getc( fp );
      if( read != '\n' )
        ungetc( read, fp );

    } else if( read == '\n' ) {
      buf[ length++ ] = '\r';
      buf[ length++ ] = '\n';

      read = getc( fp );
      if( read != '\r' )
        ungetc( read, fp );

    } else {
      buf[ length++ ] = read;
    }

    read = getc( fp );
  }

  if( read == EOF )
    panic( "Fread_string: EOF" ); 

  buf[ length ] = '\0';
  string = alloc_string( buf, type );

  return string;
}


void fread_to_eol( FILE *fp )
{
  char c;

  do {
    c = getc( fp );
  } while( c != '\r' && c != '\n' && c != EOF );

  do {
    c = getc( fp );
  } while( c == '\r' || c == '\n' );

  ungetc( c, fp );
  return;
}


char *fread_word( FILE *fp )
{
  static char    buf  [ MAX_STRING_LENGTH ];
  char*        pWord;
  char        letter;
  char             c;

  do {
    letter = getc( fp );
  } while( isspace( letter ) );

  pWord = buf;

  if( letter != '\'') {
    *buf = letter;
    pWord++;
  }

  for( ; pWord < &buf[ MAX_STRING_LENGTH ]; pWord++ ) {
    *pWord = getc( fp );
    if( ( isspace( *pWord ) && letter != '\'' ) || ( *pWord == '\'' && letter == '\'' ) ) {
      *pWord = '\0';
      
      do {
        c = getc( fp );
      } while( c == ' ' );

      ungetc( c, fp );
      return buf;
    }
  }
  
  bug( "Fread_word: word too long." );
  log( "%s\r\n", buf );

  throw( 1 );

  return NULL;
}


char *fread_block( FILE *fp )
{
  static char buf[ MAX_STRING_LENGTH ];
  char *pWord;
  char letter, c;

  do {
    letter = getc( fp );
    } while( isspace( letter ) );

  pWord = buf;

  *buf = letter;
  pWord++;

  for( ; pWord < &buf[ MAX_STRING_LENGTH ]; pWord++ ) {
    *pWord = getc( fp );
    if( isspace( *pWord ) ) {
      *pWord = '\0';
      do {
        c = getc( fp );
        } while( c == ' ' );
      ungetc( c, fp );
      return buf;
      }
    }
  
  bug( "Fread_block: word too long." );
  log( "%s\r\n", buf );
  throw( 1 );

  return NULL;
}


/*
 *   CHANGES
 */


void do_changes( char_data* ch, char* )
{
  species_data*   species;
  int                   i;
 
  do_help( ch, "Changes_Disclaimer" );

  page( ch, "Monsters:\r\n" );

  for( i = 0; i < MAX_SPECIES; i++ ) 
    if( ( species = species_list[i] ) != NULL ) 
      if( species->date > time(0) - 5 * 24 * 60 * 60 ) 
        page( ch, "  %s\r\n", species->Name( ) );

  return;
}


/*
 *   DO_WRITE ROUTINE
 */


void write_all( bool forced )
{
  area_nav alist(&area_list);
  for (area_data *area = alist.first(); area; area = alist.next())
    save_area( area, forced );

  save_mobs( );
  save_exp( );
  save_objects( );
  save_help( );
  save_trainers( );
  save_banned( );
  save_quests( );
  save_shops( );
  save_tables( );
  save_rtables( );
  save_lists( );
  save_clans( );

  for( int i = 0; i < player_list; i++ ) {
    player_data* player = player_list[i];
    if( player->In_Game( ) )
      write( player );
  }
}


void do_load( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "Which file do you wish to load?\r\n" );
    return;
  }

  if( matches( argument, "tables" ) ) {
    load_tables( );
    return;
  }

  send( ch, "Unknown file.\r\n" );
  return;
}


void do_write( char_data* ch, char* argument )
{
  char           buf  [ MAX_INPUT_LENGTH ];
  int         number  = 0;
  int          flags;

  if( !get_flags( ch, argument, &flags, "f", "write" ) )
    return;

  if( !strcasecmp( argument, "all" ) ) {
    if( !has_permission( ch, PERM_WRITE_ALL, TRUE ) ) 
      return;
    write_all( is_set( &flags, 0 ) );
    send( ch, "All files written.\r\n" );
    sprintf( buf, "All files written (%s).", ch->descr->name );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    return;
  }

  if( fmatches( argument, "areas" ) ) {
    if( !has_permission( ch, PERM_WRITE_AREAS, TRUE ) )
      return;
    area_nav alist(&area_list);
    for( area_data *area = alist.first( ); area; area = alist.next( ) )
      number += save_area( area, is_set( &flags, 0 ) );
    if( number > 0 ) {
      send( ch, "All areas written. ( %d file )\r\n", number );
      sprintf( buf, "All areas written (%s).", ch->real_name() );
      info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    } else {
      send( ch, "No area needed saving.\r\n" );
    }
    return;
  }

  if( fmatches( argument, "rtables" ) ) {
    if( !has_permission( ch, PERM_RTABLES, TRUE ) ) 
      return;
    save_rtables( );
    send( ch, "Rtables written.\r\n" );
    sprintf( buf, "Rtables written (%s).", ch->real_name() );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    return;
  }

  if( fmatches( argument, "mobs" ) ) {
    if( !has_permission( ch, PERM_MOBS, TRUE ) )
      return;
    save_mobs( );
    save_exp( );
    send( ch, "Ok.\r\n" );
    sprintf( buf, "Mob file written (%s).", ch->real_name() );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    return;
  }

  if( fmatches( argument, "exp" ) || fmatches( argument, "xp" ) ) {
    if( !has_permission( ch, PERM_MOBS, TRUE ) )
      return;
    save_exp( );
    send( ch, "Ok.\r\n" );
    sprintf( buf, "XP file written (%s).", ch->real_name() );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    return;
  }

  if( fmatches( argument, "objects" ) ) {
    if( !has_permission( ch, PERM_OBJECTS, TRUE ) )
      return;
    save_objects( );
    send( ch, "Ok.\r\n" );
    sprintf( buf, "Object file written (%s).", ch->real_name() );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    return;
  }

  if( fmatches( argument, "shops" ) ) {
    if( !has_permission( ch, PERM_ROOMS, TRUE ) )
      return;
    save_shops( );
    send( "Ok.\r\n", ch );
    sprintf( buf, "Shop file written (%s).", ch->real_name() );
    info( "", LEVEL_IMMORTAL, buf , IFLAG_WRITES, 1, ch );
    return;
  }

  if( fmatches( argument, "tables" ) ) {
    if( !has_permission( ch, PERM_MISC_TABLES )
      && !has_permission( ch, PERM_SOCIALS, TRUE ) ) 
      return;
    save_tables( );
    send( "Ok.\r\n", ch );
    sprintf( buf, "Table file written (%s).", ch->real_name() );
    info( "", LEVEL_IMMORTAL, buf, IFLAG_WRITES, 1, ch );
    return;
  }

  if( fmatches( argument, "clans" ) ) {
    if( !has_permission( ch, PERM_CLANS, TRUE ) )
      return;
    save_clans( );
    send( ch, "Ok.\r\n" );
    sprintf( buf, "Clan files written (%s).", ch->real_name() );
    info( "", LEVEL_IMMORTAL, buf, IFLAG_WRITES, 1, ch );
    return;
  }

  if( fmatches( argument, "lists" ) ) {
    if( !has_permission( ch, PERM_LISTS, TRUE ) ) 
      return;
    save_lists( );
    send( "Ok.\r\n", ch );
    sprintf( buf, "Table file written (%s).", ch->real_name() );
    info( "", LEVEL_IMMORTAL, buf , IFLAG_WRITES, 1, ch );
    return;
  }

  if( fmatches( argument, "trainers" ) ) {
    if( !has_permission( ch, PERM_WRITE_ALL, true ) ) {
      send( "You don't have permission.", ch );
      return;
    }
    save_trainers( );
    send( "Ok.\r\n", ch );
    sprintf( buf, "Trainer file written (%s).", ch->real_name() );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    return;
  }

  if( fmatches( argument, "help" ) ) {
    if( !has_permission( ch, PERM_HELP_FILES, TRUE ) )
      return;
    if( !save_help( ch ) ) 
      send( ch, "Help was not modified so was not saved.\r\n" );
    return;
  }

  if( fmatches( argument, "quests" ) ) {
    if( !has_permission( ch, PERM_WRITE_ALL, TRUE ) ) {
      send( ch, "You don't have permission." );
      return;
    }
    save_quests( );
    send( ch, "Ok.\r\n" );
    sprintf( buf, "Quest file written (%s).", ch->real_name() );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    return;
  }

  if( fmatches( argument, "players" ) ) {
    for( int i = 0; i < player_list; i++ ) {
      player_data* player = player_list[i];
      if( player->In_Game( ) )
        write( player );
    }

    send( ch, "Ok.\r\n" );
    sprintf( buf, "Player files written (%s).", ch->real_name() );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    return;
  }

  if( *argument != '\0' ) {
    send( ch, "What do you want to write?\r\n" );
    return;
  }

  if( can_edit( ch, ch->in_room ) ) {
    if( !ch->in_room->area->modified ) {
      send( "Area has not been modified so was not saved.\r\n", ch );
    } else {
      save_area( ch->in_room->area );
      send( ch, "Area written.\r\n" );
    }
  }
}

