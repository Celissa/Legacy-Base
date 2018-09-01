#include "system.h"


const char*   list_word  ( int, int );


/*
 *   LIST EDITING
 */


const char* list_entry [ MAX_LIST ][ 40 ] =
{
  { "Lists", "Permissions", "Create Food", "LS-Species",
    "LS-Reagent", "FF-Species", "FF-Reagent",
    "RA-Species", "RA-Reagent", "FM-Species", "FM-Reagent", 
    "CE-Species", "CE-Reagent", "CG-Species", "CG-Reagent",
    "AC-Species", "AC-Reagent", "Find Tinder", "Forage",
    "Poison-Ing", "Poison-Type", "LM-Species", "LM-Reagent",
    "SD-Species", "SD-Reagent", "Bane-Species", "Bane-Reagent",
    "SU-Species", "SU-Reagent", "SC-Species", "SC-Reagent",
    "BF-Species", "BF-Reagent", "RD-Species", "RD-Reagent",
    "WD-Species", "WD-Reagent", "Anc-Species", "Anc-Reagent", "" },
  { "All Rooms", "Help", "Mobs", "Objects", "Write All",
    "Edit Players", "Site Names", "Private Email", "Tables", "Lists",
    "All Mobs", "Accounts", "Passwords", "All Objects", "Rooms",
    "Exp", "Ban Sites", "Switch", "" },
  { "30", "Item", "O" }, //Create Food
  { "30", "Mob",  "M" }, //LS-Species
  { "30", "Item", "O" }, //LS-Reagent
  { "30", "Mob",  "M" }, //FF-Species
  { "30", "Item", "O" }, //FF-Reagent
  { "30", "Mob",  "M" }, //RA-Species
  { "30", "Item", "O" }, //RA-Reagent
  { "30", "Mob",  "M" }, //FM-Species
  { "30", "Item", "O" }, //FM-Reagent
  { "30", "Mob",  "M" }, //CE-Species
  { "30", "Item", "O" }, //CE-Reagent
  { "30", "Mob",  "M" }, //CG-Species
  { "30", "Item", "O" }, //CG-Reagent
  { "30", "Mob",  "M" }, //AC-Species
  { "30", "Item", "O" }, //AC-Reagent
  { "30", "Item", "O" }, //Find Tinder
  { "30", "Item", "O" }, //Forage
  { "30", "Item", "O" }, //Poison Ingredients
  { "30", "Item", "O" }, //Poison Types
  { "30", "Mob",  "M" }, //Lesser Mount - Species
  { "30", "Item", "O" }, //Lesser Mount - Reagents
  { "30", "Mob",  "M" }, //Summon Dragon - Species
  { "30", "Item", "O" }, //Summon Dragon - Reagents
  { "30", "Mob",  "M" }, //Bane - Species
  { "30", "Item", "O" }, //Bane - Reagents
  { "30", "Mob",  "M" }, //Summon Undead - Species
  { "30", "Item", "O" }, //Summon Undead - Reagents
  { "30", "Mob",  "M" }, //Summon Cadaver - Species
  { "30", "Item", "O" }, //Summon Cadaver - Reagents
  { "30", "Mob",  "M" }, //Blood Fiend - Species
  { "30", "Item", "O" }, //Blood Fiend - Reagents
  { "30", "Mob",  "M" }, //Return the Dead - Species
  { "30", "Item", "O" }, //Return the Dead - Reagents
  { "30", "Mob",  "M" }, //Walk of the Damned - Species
  { "30", "Item", "O" }, //Walk of the Damned - Reagents
  { "30", "Mob",  "M" }, // Animal Companion - Species
  { "30", "Item", "O" }, // Animal Companion - Species
};

int list_value [ MAX_LIST ][ 38 ];


const char* list_word( int list, int i )
{
  const char*        blank  = "-blank-";
  obj_clss_data*  obj_clss;
  species_data*    species;
 
  if( *list_entry[list][2] == 'O' ) {
    obj_clss = get_obj_index( list_value[list][i] );
    return( obj_clss == NULL ? blank : obj_clss->Name( ) );
  }

  if( *list_entry[list][2] == 'M' ) {
    species = get_species( list_value[list][i] );
    return( species == NULL ? blank : species->Name() );
  }

  return empty_string;
}  


void do_ledit( char_data* ch, char* argument )
{
  wizard_data*  wizard;
  int                i;

  wizard = (wizard_data*) ch;

  if( *argument == '\0' ) {
    display_array( ch, "Lists", &list_entry[0][0], &list_entry[0][1], MAX_LIST );
    return;
  }

  for( i = 0; i < MAX_LIST; i++ )
    if( !strncasecmp( argument, list_entry[0][i], strlen( argument ) ) ) {
      send( ch, "Ledit now operates on list '%s'.\r\n", list_entry[0][i] );
      wizard->list_edit = i;
      return;
    }

  send( ch, "No list with that name found.\r\n" );

  return; 
}


void do_lstat( char_data* ch, char* )
{
  char             tmp  [ TWO_LINES ];
  wizard_data*  imm;
  int             list;
  int                i;
  int           number;

  imm = wizard( ch );
  list   = imm->list_edit; 

  if( list < 0 || list >= MAX_LIST ) {
    bug( "Lstat: List editted out of range" );
    return;
  }

  page( ch, "%s:\r\n", list_entry[0][list] );

  if( isdigit( *list_entry[list][0] ) ) {
    number = atoi( list_entry[list][0] );
    for( i = 0; i < number; i++ ) {
      sprintf( tmp, "%6s[%2d] : %-5d%-20s",
        list_entry[list][1], i, list_value[list][i],
        list_word( list, i ) );
      truncate( tmp, 38 );
      page( ch, tmp );
      if( i%2 == 1 )
        page( ch, "\r\n" );
    }
  } else {
    for( i = 0; *list_entry[list][i] != '\0'; i++ ) {   
      sprintf( tmp, "%18s : %-10d%s",
        list_entry[list][i], list_value[list][i],
        i%2 ? "\r\n" : "" );
      page( ch, tmp );
    }
  }

  if( i%2 == 1 )
    page( ch, "\r\n" );

  return;
}


void do_lset( char_data* ch, char* argument )
{
  char             arg  [ MAX_INPUT_LENGTH ];
  wizard_data*  imm;
  int             list;
  int                i;
  int           number;
  int           length;

  if( *argument == '\0' ) {
    do_lstat( ch, "" );
    return;
    }

  imm = wizard( ch );
  list   = imm->list_edit; 

  if( list_value[0][list] > get_trust( ch ) ) {
    send( ch, "You need to be level %d to edit '%s' list.\r\n", list_value[0][list], list_entry[0][list] );
    return;
  }
      
  argument = one_argument( argument, arg );

  if( isdigit( *list_entry[list][0] ) ) {
    number = atoi( list_entry[list][0] );
    i = atoi( arg );
    if( i < 0 || i >= number ) {
      send( ch, "Index for list out of range.\r\n" );
      return;
      }
    number = atoi( argument );
    send( ch, "List entry %d set to %d.\r\n", i, number );
    }
  else {
    length = strlen( arg );
    for( i = 0; ; i++ ) {
      if( *list_entry[list][i] == '\0' ) {
        send( "Entry not found in list.\r\n", ch );
        return;
        }
      if( !strncasecmp( arg, list_entry[list][i], length ) )
        break;
      }
    number = atoi( argument );
    send( ch, "List entry '%s' set to %d.\r\n",
      list_entry[list][i], number );
    }

  list_value[list][i] = number;

  return;
}


/*
 *   DISK ROUTINES
 */


void save_lists( void )
{
  char      tmp  [ ONE_LINE ];
  FILE*      fp;
  int     i,j,k;

  sprintf( tmp, "%s%s", TABLE_DIR, "Lists" );

  if( ( fp = fopen( tmp, "wb" ) ) == NULL ) {
    perror( tmp );
    throw( 1 );
    }

  fprintf( fp, "%4d \n", 1 ); //version

  for( i = 0; i < MAX_LIST; i++ ) {
    for( j = 0; j < 3; j++ ) {
      for( k = 0; k < 10; k++ )
        fprintf( fp, "%4d ", list_value[i][10*j+k] );
      fprintf( fp, "\n" );
      }
    fprintf( fp, "\n" );
    }

  fclose( fp );

  return;
}


void load_lists( void )
{
  FILE*    fp;
  int    i, j;
  
  log( "Loading Lists...\r\n" );
 
  fp = open_file( TABLE_DIR, "Lists", "rb" );
  
  int version = fread_number( fp );
  
  for( i = 0; i < MAX_LIST; i++ )
    for( j = 0; j < 30; j++ ) {
      if( version < 1 && i > 24 )
  list_value[i][j] = 0;
      else
        list_value[i][j] = fread_number( fp );
    }

  fclose( fp );

  return;
}
