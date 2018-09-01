#include "system.h"


rtable_array rtable_list;


void  renumber_rtable   ( char_data*, rtable_data*, int );
bool  can_extract       ( rtable_data*, char_data* );
bool  has_reset         ( rtable_data* );
 

/*
 *   RTABLE_DATA NEW AND DELETE
 */


Rtable_Data :: Rtable_Data( )
{
  name  = empty_string;
  reset = NULL;
 
  return;
}


Rtable_Data :: ~Rtable_Data( )
{
  return;
}


/*
 *   SUPPORT ROUTINES
 */


rtable_data *find_rtable( int vnum )
{
  for( int i = 0; i < rtable_list; i++ )
    if( vnum == rtable_list[i]->vnum )
      return rtable_list[i];

  return NULL;
}

rtable_data *find_rtable( char *name )
{
  for( int i = 0; i < rtable_list; i++ )
    if( matches( name, rtable_list[i]->name, false ) )
      return rtable_list[i];

  return NULL;
}

bool can_extract( rtable_data *table, char_data* ch )
{
  if( has_reset( table ) ) {
    send( ch, "That table still has resets and cannot be extracted.\r\n" );
    return FALSE;
  }

  return TRUE;
}


bool has_reset( rtable_data *table )
{
  reset_data*      reset;
  species_data*  species;
  room_data*        room;

  for( int i = 0; i < MAX_SPECIES; i++ ) 
    if( ( species = species_list[i] ) != NULL ) 
      for( reset = species->reset; reset != NULL; reset = reset->next ) 
        if( table == reset->target && !is_set( &reset->flags, RSFLAG_MOB ) && !is_set( &reset->flags, RSFLAG_OBJECT ) ) 
          return TRUE;

  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) )
    for( room = area->room_first; room != NULL; room = room->next ) 
      for( reset = room->reset; reset != NULL; reset = reset->next ) 
        if( table == reset->target && !is_set( &reset->flags, RSFLAG_MOB ) && !is_set( &reset->flags, RSFLAG_OBJECT ) ) 
          return TRUE;

  for( int i = 0; i < rtable_list; i++ )
    for( reset = rtable_list[i]->reset; reset != NULL; reset = reset->next )
      if( table == reset->target && !is_set( &reset->flags, RSFLAG_MOB ) && !is_set( &reset->flags, RSFLAG_OBJECT ) ) 
        return TRUE;
  
  return FALSE;
}


/*
 *  EDITING ROUTINE
 */


void do_rtable( char_data* ch, char* argument )
{
  wizard_data*   wizard  = (wizard_data*) ch;
  rtable_data*   rtable  = wizard->rtable_edit;
  int              i, j;
  bool             flag;

  if( matches( argument, "exit" ) || !strcmp( argument, ".." ) ) {
    if( rtable == NULL )
      send( ch, "You aren't editing a rtable.\r\n" );
    else {
      send( ch, "You stop editing rtable '%s'.\r\n", rtable->name );
      wizard->rtable_edit = NULL;
    }
    return;
  }

  if( rtable != NULL ) {
    edit_reset( ch, argument, rtable->reset, RST_TABLE, rtable->name );
    return;
  }
    
  if( *argument == '\0' ) {
    page_title( ch, "Reset Tables" );
    for( int i = 0; i < rtable_list; i++ ) 
      send( ch, "[%2i] %s\r\n", rtable_list[i]->vnum, rtable_list[i]->name );
    return;
  }

  if( exact_match( argument, "new" ) ) { 
    if( *argument == '\0' ) {
      send( ch, "Name of new table?\r\n" );
      return;
    }
    rtable        = new rtable_data;
    rtable->name  = alloc_string( argument, MEM_MRESET );

    for( rtable->vnum = 1; ; rtable->vnum++ )
      if( !find_rtable( rtable->vnum ) )
        break;

    rtable_list  += rtable; // insert( rtable_list, num_rtable, rtable, num_rtable );
    send( ch, "Rtable '%s' created with vnum %d.\r\n", rtable->name, rtable->vnum );
    return;
  }

  flag = matches( argument, "delete" );

  if( number_arg( argument, i ) ) {
    if( ( rtable = find_rtable( i ) ) == NULL ) { 
      send( ch, "No rtable exists with that index.\r\n" );
      return;
    }

    if( flag ) {
      // delete the rtable
      if( can_extract( rtable, ch ) ) {
        send( ch, "Rtable %d, %s removed.\r\n", rtable->vnum, rtable->name );
        rtable_list -= rtable; // remove( rtable_list, num_rtable, i );
        delete rtable;
      }
      return;
    } 

    if( *argument == '\0' ) {
      wizard->rtable_edit = rtable;
      send( ch, "You are now editing rtable %d, '%s'.\r\n", rtable->vnum, rtable->name );
    } else if( number_arg( argument, j ) ) {
      renumber_rtable( ch, rtable, j );
    } else {
      send( ch, "Rtable %d, '%s' renamed '%s'.\r\n", rtable->vnum, rtable->name, argument );
      free_string( rtable->name, MEM_MRESET );
      rtable->name = alloc_string( argument, MEM_MRESET );
    }
    return;
  }

  send( ch, "Illegal syntax - See help rtable.\r\n" );
}


void renumber_rtable( char_data* ch, rtable_data *rtable, int j )
{
  room_data*         room;
  reset_data*       reset;

  if( find_rtable( j ) != NULL ) {
    send( ch, "An rtable already exists with that vnum.\r\n" );
    return;
  }

  if( rtable->vnum == j ) {
    send( ch, "TIMMY!!!!\r\n" );
    return;
  }

  rtable->vnum = j;

  send( ch, "Rtable '%s' renumbered to %d.\r\n", rtable->name, rtable->vnum );

  /* RENUMBER RESETS */

  area_nav alist(&area_list);
  for( area_data *area = alist.first( ); area; area = alist.next( ) )
    for( room = area->room_first; room != NULL; room = room->next ) 
      for( reset = room->reset; reset != NULL; reset = reset->next ) 
        if( !is_set( &reset->flags, RSFLAG_OBJECT ) && !is_set( &reset->flags, RSFLAG_MOB ) ) // && renumber( reset->vnum, i, j ) )
          area->modified = TRUE;

/*
  for( k = 0; k < MAX_SPECIES; k++ ) 
    if( ( species = species_list[k] ) != NULL ) 
      for( reset = species->reset; reset != NULL; reset = reset->next ) 
        if( !is_set( &reset->flags, RSFLAG_OBJECT ) && !is_set( &reset->flags, RSFLAG_MOB ) )
          mobs_modified = TRUE; // renumber( reset->vnum, i, j );

  return;
*/
}


/*
 *   DISK ROUTINES
 */


void load_rtables( )
{
  FILE*              fp;
  int                 j;

  log( "Loading Reset Tables...\r\n" );
 
  fp = open_file( RTABLE_FILE, "rb" );

  char *word = fread_word(fp);
  if (strcmp(word, "#RTABLES")) {
    panic( "Load_rtables: header not found" );
  }

  for(;;) {
    char *name = fread_string( fp, MEM_MRESET );
    if( !strcmp(name, "#")) {
      free_string( name, MEM_MRESET );
      break;
    }

    rtable_data *rtable = new rtable_data;
    rtable->name   = name;
    rtable->vnum   = fread_number( fp );
    rtable_list += rtable;
    for( ; ; ) {
      if( ( j = fread_number( fp ) ) == -1 )
        break;
      reset_data *reset = new reset_data;
      reset->target  = (void *) j;
      reset->flags   = fread_number( fp );
      reset->chances = fread_number( fp );
      reset->value   = fread_number( fp );
      append( rtable->reset, reset );
    }
  }

  fclose( fp );
}


void fix_rtables ( )
{
  log( "Fixing Reset Tables...\r\n" );

  for( int i = 0; i < rtable_list; i++) {
    rtable_data *rtable = rtable_list[i];
    for( reset_data *reset = rtable->reset; reset; reset = reset->next ) {
      if (is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        // fix object
        reset->target = get_obj_index((int) reset->target);
      } else if (is_set( &reset->flags, RSFLAG_MOB ) ) {
        // fix mob
        reset->target = get_species((int) reset->target);
      } else {
        // fix rtable
        reset->target = find_rtable((int) reset->target);
      }
    }
  }
}


void save_rtables( )
{
  FILE*            fp;

  rename_file( AREA_DIR, RTABLE_FILE, PREV_DIR, RTABLE_FILE );

  if( ( fp = open_file( RTABLE_FILE, "wb" ) ) == NULL ) 
    return;

  fprintf( fp, "#RTABLES\n\n" );

  for( int i = 0; i < rtable_list; i++ ) {
    fprintf( fp, "%s~\n", rtable_list[i]->name );
    fprintf( fp, "%d\n", rtable_list[i]->vnum );
    for( reset_data *reset = rtable_list[i]->reset; reset != NULL; reset = reset->next ) {
      int vnum = 0;
      if (is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        // object vnum
        vnum = ((obj_clss_data*) reset->target)->vnum;
      } else if (is_set( &reset->flags, RSFLAG_MOB ) ) {
        // mob vnum
        vnum = ((species_data*) reset->target)->vnum;
      } else {
        // rtable number
        vnum = ((rtable_data*) reset->target)->vnum;
      }
      fprintf( fp, "%d %d %d %d\n", vnum, reset->flags, reset->chances, reset->value );
    }
    fprintf( fp, "-1\n\n" );
  }

  fprintf( fp, "#~\n" );
  fclose( fp );
}


/*
 *   RTWHERE
 */


void do_rtwhere( char_data* ch, char* argument )
{
  reset_data*         reset;
  rtable_data*       rtable;
  room_data*           room;
  species_data*     species;
  bool                found;
  int                     i;

  if( *argument == '\0' ) {
    send( ch, "Syntax: rtwhere <table>\r\n" );
    return;
  } else if( number_arg( argument, i ) ) {
    rtable = find_rtable( i );
  } else {
    rtable = find_rtable( argument );
  }

  if( rtable == NULL ) {
    send( ch, "There is no such rtable.\r\n" );
    return;
  }

  found = FALSE;

  /* SEARCH MRESETS */

  for( i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL ) 
      continue;
    for( reset = species->reset; reset != NULL; reset = reset->next ) {
      if( reset->target == rtable ) { // == reset->vnum && !is_set( &reset->flags, RSFLAG_OBJECT ) && !is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        page( ch, "  On %d, %s\r\n", i, species->Name() );
        found = TRUE;
      }
    }
  }

  /* SEARCH ROOM RESETS */

  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) ) {
    for( room = area->room_first; room != NULL; room = room->next ) {
      species = NULL;
      for( reset = room->reset; reset != NULL; reset = reset->next ) {
        if( is_set( &reset->flags, RSFLAG_MOB ) ) {
          species = (species_data*) reset->target; // get_species( reset->vnum );
          continue;
        }
        
        if( reset->target != rtable ) // != reset->vnum || is_set( &reset->flags, RSFLAG_OBJECT ) )
          continue;
        found = TRUE;
        if( reset->value == -2 ) {
          page( ch, "  At room %d, %s\r\n", room->vnum, room->name );
        } else if( species == NULL ) {
          page( ch, "  [BUG] Illegal reset structure [%d]\r\n", room->vnum );
        } else {
          page( ch, "  On %s at %s [%d]\r\n", species->Name(), room->name, room->vnum );
        }
      }
    }
  }

  /* SEARCH TABLES */

  for( i = 0; i < rtable_list; i++ )
    for( reset = rtable_list[i]->reset; reset != NULL; reset = reset->next )
      if( reset->target == rtable ) { // reset->vnum == index && !is_set( &reset->flags, RSFLAG_OBJECT ) ) {
        page( ch, "  In rtable %d, %s.\r\n", rtable_list[i]->vnum, rtable_list[i]->name );
        found = TRUE;
      }

  if( !found )
    page( ch, "  no resets found\r\n" ); 

  return;
}
