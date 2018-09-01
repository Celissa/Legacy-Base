#include "system.h"


#define HELP_IMMORTAL         0
#define HELP_SPELLS           1
#define HELP_FUNCTIONS        2
#define HELP_LOGIN            3


help_data**      help_list;
int               max_help;
bool         help_modified  = TRUE;


void         spell_help      ( char_data*, int );
void         make_w3         ( help_data* help );
bool         spell_shield    ( int );

/*
 *   HELP_DATA CLASS
 */


class Help_Data
{
 public:
  char*      name;
  char*      text;
  char*  immortal;
  int       level  [ 2 ];
  int    category;

  Help_Data   ( );
  ~Help_Data  ( );

  friend char* name( help_data* help ) {
    return help->name;
  }
};


Help_Data :: Help_Data( )
{
  record_new( sizeof( help_data ), MEM_HELP );

  name      = (char*) empty_string;
  text      = (char*) empty_string;
  immortal  = (char*) empty_string;
  level[0]  = 0;
  level[1]  = 0;
  category  = 0;
}


Help_Data :: ~Help_Data( )
{
  int pos;

  record_delete( sizeof( help_data ), MEM_HELP ); 
  free_string( name, MEM_HELP );
  free_string( text, MEM_HELP );
  free_string( immortal, MEM_HELP );

  for( pos = 0; pos < max_help; pos++ )
    if( help_list[pos] == this ) {
      remove( help_list, max_help, pos );
      break;
    }
}


/*
 *   READ/WRITE ROUTINES
 */


void load_helps( void )
{
  help_data*  help;
  FILE*         fp;
  int          pos;

  echo( "Loading Help ...\r\n" );

  help_list = NULL;
  max_help  = 0;

  fp = open_file( HELP_FILE, "rb" );
  if( fp == NULL )
    panic( "Load_helps: file not found" );

  if( strcmp( fread_word( fp ), "#HELPS" ) ) 
    panic( "Load_helps: missing header" );

  for( ; ; ) {
    help           = new help_data;
    help->level[0] = fread_number( fp );
    help->level[1] = fread_number( fp );
    help->category = fread_number( fp );
    help->name     = fread_string( fp, MEM_HELP );

    if( *help->name == '$' ) {
      delete help;
      break;
    }

    help->text     = fread_string( fp, MEM_HELP, true );
    help->immortal = fread_string( fp, MEM_HELP, true );  

    pos = pntr_search( help_list, max_help, help->name );

    if( pos < 0 )
      pos = -pos-1;

    insert( help_list, max_help, help, pos );
  }

  fclose( fp );
}


bool save_help( char_data* ch )
{
  char         buf  [ TWO_LINES ];
  help_data*  help;
  FILE*         fp;
  int          pos;

  if( !help_modified ) 
    return FALSE;

  rename_file( AREA_DIR, HELP_FILE, PREV_DIR, HELP_FILE );

  if( ( fp = open_file( HELP_FILE, "wb" ) ) == NULL )
    return FALSE;

  fprintf( fp, "#HELPS\n" );

  for( pos = 0; pos < max_help; pos++ ) {
    help = help_list[pos];
    fprintf( fp, "%d %d %d %s~\n", help->level[0], help->level[1],
      help->category, help->name );
    fprintf( fp, ".%s~\n\n", help->text );
    fprintf( fp, ".%s~\n\n", help->immortal );
  }
  fprintf( fp, "-1 -1 -1 $~\n" );
  fclose( fp );

  help_modified = FALSE;

  if( ch != NULL ) {
    send( ch, "Help file written.\r\n" );
    sprintf( buf, "Help file written (%s).", ch->real_name() );
    info( "", LEVEL_HELP, buf, IFLAG_WRITES, 1, ch );
  }

  //w3_help( );

  return TRUE;
}


/*
 *   W3 ROUTINES
 */


void smash_color( char* tmp )
{
  int i;
  int j;

  for( i = j = 0; tmp[i] != '\0'; i++ ) {
    if( tmp[i] == '\r' || tmp[i] == '\n' ) 
      continue;
    if( tmp[i] == '@' ) {
      if( tmp[++i] == '\0' )
        break;
      if( tmp[i] != '@' ) 
        continue;
    }
    if( tmp[i] == '<' ) 
      tmp[j++] = '[';
    else if( tmp[i] == '>' ) 
      tmp[j++] = ']';
    else
      tmp[j++] = tmp[i];    
  }

  tmp[j] = '\0';

  return;
}


void w3_help( )
{
  char    tmp  [ FOUR_LINES ];
  int       i;
  int     pos;
  FILE*    fp;

  sprintf( tmp, "%shelp/index.html", W3_DIR );

  if( ( fp = open_file( tmp, "wb" ) ) == NULL )
    return;

  fprintf( fp, "<html>\n" );
  fprintf( fp, "<body>\n" );

  for( i = 0; i < MAX_ENTRY_HELP_CAT; i++ ) {
    sprintf( tmp, "./%s/index.html", help_cat_table[i].name );
    smash_spaces( tmp );    
    fprintf( fp, "<p><a href=\"%s\"> %s </a></p>\n",
      tmp, help_cat_table[i].name );
    sprintf( tmp, "mkdir %shelp/%s/", W3_DIR, help_cat_table[i].name );
    smash_spaces( &tmp[6] );    
    system( tmp );
  }

  fprintf( fp, "</html>\n" );
  fprintf( fp, "</body>\n" );
  
  fclose( fp );

  for( i = 0; i < MAX_ENTRY_HELP_CAT; i++ ) {
    sprintf( tmp, "%shelp/%s/index.html", W3_DIR, help_cat_table[i].name );
    smash_spaces( tmp );

    if( ( fp = fopen( tmp, "wb" ) ) == NULL ) {
      bug( "W3_Help: Fopen error %s", tmp );
      return;
    }

    fprintf( fp, "<html>\n" );
    fprintf( fp, "<body>\n" );

    for( pos = 0; pos < max_help; pos++ ) {
      if( help_list[pos]->category == i && help_list[pos]->level[0] == 0 && help_list[pos]->level[1] == 0 ) {
        sprintf( tmp, "./%s", help_list[pos]->name );
        smash_spaces( tmp );
        fprintf( fp, "<p><a href=\"%s\"> %s </a></p>\n",
          tmp, help_list[pos]->name );
      }
    }

    fprintf( fp, "</html>\n" );
    fprintf( fp, "</body>\n" );
  
    fclose( fp );
  }

  for( pos = 0; pos < max_help; pos++ ) 
    make_w3( help_list[pos] );
    
  return;
}


void make_w3( help_data* help )
{
  char     tmp  [ FOUR_LINES ];
  char*  input;
  FILE*     fp;
  char*   name;
 
  if( help->level[0] != 0 || help->level[1] != 0 )
    return;

  name = help->name;

  smash_slashes( name );

  sprintf( tmp, "%shelp/%s/%s", W3_DIR,
    help_cat_table[ help->category ].name, name );
  smash_spaces( tmp );

  if( ( fp = fopen( tmp, "wb" ) ) == NULL ) {
    bug( "Make_W3( help ): Fopen error %s", tmp );
    return;
  }

  fprintf( fp, "<html>\n" );
  fprintf( fp, "<body>\n" );

  for( input = help->text; *input != '\0'; ) {
    input = one_line( input, tmp );
    smash_color( tmp );
    if( !strcasecmp( tmp, "Syntax" ) ) 
      fprintf( fp, "<h1> Syntax </h1>\n" );
    else if( !strcasecmp( tmp, "Description" ) ) 
      fprintf( fp, "<h1> Description </h1>\n" );
    else 
      fprintf( fp, "%s<br>\n", tmp );
  }

  fprintf( fp, "</html>\n" );
  fprintf( fp, "</body>\n" );
  
  fclose( fp );

  return;
}


/*
 *   FIND_HELP ROUTINE
 */


bool can_read( char_data* ch, help_data* help )
{
  return has_permission( ch, help->level );
}



help_data* find_help( char_data* ch, const char* argument, bool msg )
{
  help_data*        help;
  int              first  = -2;
  int                pos;

  if( is_number( argument ) ) {
    pos = atoi( argument );
    if( pos < 0 || pos >= max_help ) {
      if( msg ) 
        send( ch, "There is no help file with that index.\r\n" ); 
      return NULL;
    }
    if( !can_read( ch, help_list[pos] ) ) {
      if( msg )
        send( ch, "You do not have the required permission.\r\n" );
      return NULL;
    }
    return help_list[pos];
  }

  pos = pntr_search( help_list, max_help, argument );

  if( pos >= 0 ) {
    if( can_read( ch, help_list[pos] ) )
      return help_list[pos];
    pos++;
  } else
    pos = -pos-1;

  for( ; pos < max_help; pos++ ) {
    help = help_list[pos];
    if( !fmatches( argument, help->name ) )
      break;
    if( can_read( ch, help ) ) {
      if( first != -2 ) {
        if( first != -1 ) {
          if( msg ) {
          page( ch, "More than one match was found - please be more specific in what topic you\r\nwant help on.\r\n\r\n" );
          page( ch, "  [%3d] %s:%15s%s\r\n", first,
            help_cat_table[help_list[first]->category].name, "",
            help_list[first]->name );
          }
          first = -1;
        }
        if( msg )
          page( ch, "  [%3d] %s:%15s%s\r\n", pos,
          help_cat_table[help->category].name, "", help->name );
      } else {
        first = pos;
      }
    }
  }

  if( first >= 0 )
    return help_list[first];

  if( first == -2 )
    if( msg )
      send( ch, "No matching help file was found - use index to see a list of topics for\r\nwhich help exists.\r\n" );
      
  return NULL;
}


/*
 *   MAIN HELP ROUTINE
 */


void do_help( char_data* ch, char* argument )
{
  char          tmp  [ 3*MAX_STRING_LENGTH ];
  help_data*   help;
  int             i;
  int        length  = strlen( argument );

  if( pet_help( ch ) || ch->link == NULL )
    return;
  
  if( ( help = find_help( ch,
    *argument == '\0' ? "summary" : argument ) ) == NULL ) 
    return;

  if( ch->link->connected != CON_PLAYING ) {
    convert_to_ansi( ch, help->text, tmp, sizeof(tmp) );
    send( ch, tmp );
    send( ch, "\r\n" );
    return;
  }

  if( help->category == HELP_SPELLS ) 
    for( i = 0; i < SPELL_COUNT; i++ ) 
      if( !strncasecmp( spell_table[i].name, argument, length ) ) {
        spell_help( ch, i );
        return;
      }
 
  page( ch, "Topic: %s\r\n", help->name );

  if( is_apprentice( ch ) ) {
    strcpy( tmp, "Level: " );
    permission_flags.sprint( &tmp[7], help->level );
    strcat( tmp, "\r\n" );
    page( ch, tmp );
    page( ch, "\r\n" );
  }

  page( ch, "\r\n" );
  convert_to_ansi( ch, help->text, tmp, sizeof(tmp) );
  page( ch, tmp );
  page( ch, "\r\n" );

  if( help->immortal != empty_string && is_apprentice( ch ) ) {
    page( ch, "\r\n" );
    convert_to_ansi( ch, help->immortal, tmp, sizeof(tmp) );
    page( ch, tmp );
    page( ch, "\r\n" );
  }

  return;
}


void help_link( link_data* link, const char* argument )
{
  char         tmp  [ 3*MAX_STRING_LENGTH ];
  int          pos;

  pos = pntr_search( help_list, max_help, argument );

  if( pos < 0 ) {
    send( link, "Help subject %s not found.\r\n", argument );
  } else if( link->player != NULL && link->player->pcdata != NULL ) {
    convert_to_ansi( link->player, help_list[pos]->text, tmp, sizeof(tmp) );
    if( link->connected == CON_PLAYING ) {
      page(link->player, tmp);
      page(link->player, "\r\n");
    } else {
      send(link, tmp);
      send(link, "\r\n");
    }
  } else {
    send(link, help_list[pos]->text);
    send(link, "\r\n");
  }

  return;
}


void do_motd( char_data* ch, char* )
{
  do_help( ch, "motd" );
  return;  
}


/*
 *   INDEX COMMAND
 */


void do_index( char_data* ch, char* argument )
{
  char           tmp  [ MAX_STRING_LENGTH ];
  int         length  = strlen( argument ); 
  help_data*    help;
  int           i, j;
  int            pos;
  int          trust  = get_trust( ch );

  if( *argument =='\0' ) {
    page_title( ch, "Index Categories" );
    for( i = j = 0; i < MAX_ENTRY_HELP_CAT; i++ ) 
      if( help_cat_table[i].level <= trust ) {
        sprintf( tmp, "%18s%s", help_cat_table[i].name, (j++)%4 != 3 ? "" : "\r\n" );
        page( ch, tmp );
      }
    page( ch, "\r\n%s", j%4 != 0 ? "\r\n" : "" );
    page_centered( ch, "[ Type index <category> to see a list of help files in that category. ]" );
    return;
  }

  for( i = 0; i < MAX_ENTRY_HELP_CAT; i++ ) 
    if( help_cat_table[i].level <= trust && !strncasecmp( argument, help_cat_table[i].name, length ) ) 
      break;

  if( i == MAX_ENTRY_HELP_CAT ) {
    send( ch, "Unknown help category.\r\n" );
    return;
  }

  page_title( ch, "Help Files - %s", help_cat_table[i].name );      

  for( j = 0, pos = 0; pos < max_help; pos++ ) {
    help = help_list[pos];
    if( help->category != i || !can_read( ch, help ) )
      continue;
    page( ch, "%18s%s", help->name, ++j%3 ? "        " : "\r\n" );
  }
  if( j%3 != 0 )
    page( ch, "\r\n" );
  page( ch, "\r\n" );
  page_centered( ch, "[ Type help <file> to read a help file. ]" );

  return;
}


/*
 *   ONLINE EDITING OF HELP
 */


void do_hedit( char_data *ch, char *argument )
{
  help_data*    help  = ch->pcdata->help_edit;
  wizard_data*   imm;
  int            pos;  

  if( *argument == '\0' ) {
    send( ch, "What help file do you wish to edit?\r\n" );
    return;
  }

  if( !strncasecmp( argument, "delete", 6 ) ) {
    if( help == NULL ) {
      send( ch, "You aren't editing any help file.\r\n" );
      return;
    }
    for( int i = 0; i < player_list; i++ ) {
      if( ( imm = wizard( player_list[i] ) ) != NULL
        && imm->Is_Valid( )
        && imm->pcdata->help_edit == help && imm != ch ) {
        send( ch, "The help file you are editing was just deleted.\r\n" );
        imm->pcdata->help_edit = NULL;
      }
    }

    help_modified         = TRUE;
    ch->pcdata->help_edit = NULL;
    send( ch, "Help file '%s' removed.\r\n", help->name );
    delete help;
    return; 
  }

  if( !strncasecmp( argument, "new ", 4 ) ) {
    argument += 4;
    if( argument[0] == '\0' ) {
      send( "On what subject do you want to create a help?\r\n", ch );
      return;
    }

    help        = new help_data;
    help->name  = alloc_string( argument, MEM_HELP );

    pos = pntr_search( help_list, max_help, "blank" );
    if( pos >= 0 ) 
      help->text = alloc_string( help_list[pos]->text, MEM_HELP );
    else
      help->text = (char*) empty_string;

    pos = pntr_search( help_list, max_help, argument );

    if( pos < 0 )
      pos = -pos-1;

    insert( help_list, max_help, help, pos );
    ch->pcdata->help_edit = help;
    send( "Help subject created.\r\n", ch );
    help_modified = TRUE;
    return;
  }

  if( ( help = find_help( ch, argument ) ) == NULL ) 
    return;

  ch->pcdata->help_edit = help;
  send( ch, "Hdesc and hset now operate on %s.\r\n", help->name );
}  


void do_hdesc( char_data *ch, char *argument ) 
{
  help_data* help = ch->pcdata->help_edit;

  if( help == NULL ) {
    send( ch, "You are not editing any subject.\r\n" );
    return;
  }

  help->text    = edit_string( ch, argument, help->text, MEM_HELP );
  help_modified = TRUE;
}


void do_hbug( char_data* ch, char* argument ) 
{
  help_data* help = ch->pcdata->help_edit;

  if( help == NULL ) {
    send( ch, "You are not editing any subject.\r\n" );
    return;
  }

  help->immortal = edit_string( ch, argument, help->immortal, MEM_HELP );
  help_modified  = TRUE;
}

void do_hstat( char_data* ch, char* argument )
{
  help_data* help = ch->pcdata->help_edit;
  if ( help == NULL ) {
    send( ch, "You are not editing any subject.\r\n" );
    return;
  }
  page( ch, "       Name: %s\r\n", help->name );
  char tmp[MAX_STRING_LENGTH];
  permission_flags.sprint( tmp, help->level );
  page( ch, "      Level: %s\r\n", tmp );
  page( ch, "   Category: %s\r\n", help_cat_table[ help->category ].name );
}

void do_hset( char_data *ch, char *argument )
{
  help_data*   help  = ch->pcdata->help_edit;

  if( help == NULL ) {
    send( ch, "You are not editing any subject.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    do_hstat( ch, "" );
    return;
  }

  help_modified = TRUE;

  if( matches( argument, "level" ) ) {
    permission_flags.set( ch, argument, help->level );
    return;
  }

#define hcn( i )   help_cat_table[i].name

  class type_field type_list[] = {
    { "category",  MAX_ENTRY_HELP_CAT,  &hcn(0),  &hcn(1),  &help->category },
    { "",          0,                   NULL,     NULL,     NULL            }
  };

#undef hcn

  if( process( type_list, ch, help->name, argument ) )
    return;

  if( matches( argument, "name" ) ) {
    if( *argument == '\0' ) {
      send( ch, "What do you want to set the help name to?\r\n" );
      return;
    }

    for (int pos = 0; pos < max_help; pos++)
      if (help_list[pos] == help) {
        remove(help_list, max_help, pos);
        break;
      }

    send(ch, "Help name changed from %s to %s.\r\n", help->name, argument);
    free_string(help->name, MEM_HELP);
    help->name = alloc_string(argument, MEM_HELP);

    int pos = pntr_search(help_list, max_help, help->name);
    if (pos < 0)
      pos = -pos - 1;

    insert( help_list, max_help, help, pos );
    return;
  }

  send( ch, "See help hset.\r\n" );
}


/*
 *  FUNCTION HELP
 */


void do_fwhere( char_data* ch, char* argument )
{
  char     tmp  [ MAX_STRING_LENGTH ];
  bool   found  = FALSE;
  int    flags;
  
  if( !get_flags( ch, argument, &flags, "v", "Fwhere" ) )
    return;

  if( *argument == '\0' ) {
    send( ch, "Syntax: fwhere <function name>\r\n" );
    return;
  }

  for( int i = 0; cfunc_list[i].name[0] != '\0'; i++ ) {
    if( !fmatches( argument, cfunc_list[i].name ) || cfunc_list[i].func_call == NULL )
      continue;

    if( !found )
      found = TRUE;
    else {
      send( ch, "You may only search for one function at a time.\r\n" );
      return;
    }

    sprintf( tmp, "%-15s\r\n---------------\r\n", cfunc_list[i].name );
    page( ch, tmp );

    if( !cfunc_search( ch, cfunc_list[i].func_call, is_set( &flags, 0 ) ) )
      page( ch, " < not used in any scripts >\r\n" );

    page( ch, "\r\n" );
  }

  if( !found ) 
    send( ch, "No matching function found.\r\n" );
}


void do_functions( char_data* ch, char* argument )
{
  char     tmp  [ MAX_STRING_LENGTH ];
  char     buf  [ MAX_STRING_LENGTH ];
  bool   found  = FALSE;
  int   length  = strlen( argument );
  int      pos;
  int    flags;

  pos = pntr_search( help_list, max_help, argument );
  
  if( !get_flags( ch, argument, &flags, "a", "functions" ) )
    return;

  if( is_set( &flags, 0 ) ) {
    if( fmatches( argument, "none" ) ) {
      page( ch, "None argument means no argument, leave this blank.\r\n" );
      return;
    }
    else if( fmatches( argument, "any" ) ) {
      page( ch, "Any means anything can go in here.\r\n" );
      return;
    }
    else if( fmatches( argument, "string" ) ) {
      page( ch, "String means any text line, it must be surrounded by quotes.\r\n" );
      return;
    }
    else if( fmatches( argument, "integer" ) ) {
      page( ch, "Integer means a number.\r\n" );
      return;
    }
    else if( fmatches( argument, "character" ) ) {
      page( ch, "Character means either a player or a mob (this includes pets).\r\n" );
      return;
    }
    else if( fmatches( argument, "object" ) ) {
      page( ch, "Object means an object (like the weapons or armor your wearing).\r\n" );
      return;
    }
    else if( fmatches( argument, "room" ) ) {
      page( ch, "Room means a room (like the one your standing in).\r\n" );
      return;
    }
    else if( fmatches( argument, "direction" ) ) {
      page( ch, "Direction means up, down, north, south, east, or west.\r\n" );
      return;
    }
    else if( fmatches( argument, "clan" ) ) {
      page( ch, "Clan means one of these:\r\n" );
      for( int i = 0; i < clan_list.size; i++ ) {
        page( ch, "     %s\r\n", clan_list[i]->abbrev );
      }
      return;
    }
    else if( fmatches( argument, "skill" ) ) {
      page( ch, "Skill means one of these.\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_SKILL; i++, col++ ) {
        page( ch, "%27s", skill_table[i].name );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "rflag" ) ) {
      page( ch, "Rflag means one of these: \r\n" );
      int col = 1;
      for( int i = 0; i < MAX_RFLAG; i++, col++ ) {
        page( ch, "%20s", rflag_name[i] );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "stat" ) ) {
      page( ch, "Stat means one of these:\r\n" );
      page( ch, "Str     Int     Wis     Dex    Con    Level    Piety\r\n" );
      page( ch, "Class   Align   Magic   Fire   Cold   Elec     Mind\r\n" );
      page( ch, "Age     Mana    Hit     Move   Ac     Hitroll  Damroll\r\n" );
      page( ch, "Maregen Hpregen Mvregen Acid   Poison Holy\r\n" );
      page( ch, "None\r\n" );
      return;
    }
    else if( fmatches( argument, "class" ) ) {
      page( ch, "Class means one of the classes of the mud:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_ENTRY_CLSS; i++, col++ ) {
        page( ch, "%20s", clss_table[i].name );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "religion" ) ) {
      page( ch, "Religion means one of the religions of the mud:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_ENTRY_RELIGION; i++, col++ ) {
        page( ch, "%20s", religion_table[i].name );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "race" ) ) {
      page( ch, "Race means one of the races of the general race table:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_ENTRY_RACE; i++, col++ ) {
        page( ch, "%20s", race_table[i].name );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "thing" ) ) {
      page( ch, "Thing means any object, mob, player or room.\r\n" );
      return;
    }
    else if( fmatches( argument, "element" ) ) {
      page( ch, "Element means one of the following:\r\n" );
      page( ch, "Physical     Fire     Cold     Acid     Shock     Mind\r\n" );
      page( ch, "Magic        Poison   Holy     Unholy\r\n" );
    }
    else if( fmatches( argument, "wearpos" ) ) {
      page( ch, "Wearpos means any of the wear positions:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_ITEM_WEAR; i++, col++ ) {
        page( ch, "%20s", wear_part_name[i] );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "layer" ) ) {
      page( ch, "Layer means any of the layers an object can be put:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_LAYER; i++, col++ ) {
        page( ch, "%20s", layer_name[i] );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "affect" ) ) {
      page( ch, "Affect means any of the various affect flags:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_ENTRY_AFF_CHAR; i++, col++ ) {
        page( ch, "%20s", aff_char_table[i].name );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "visible" ) ) {
      page( ch, "Visible means any of the various sensory datas:\r\n" );
      page( ch, "All     Sight     Sound     Touch     Sight_sound\r\n" );
      return;
    }
    else if( fmatches( argument, "range" ) ) {
      page( ch, "Range means close up or far away:\r\n" );
      page( ch, "Physical     Ranged\r\n" );
      return;
    }
    else if( fmatches( argument, "valid" ) ) {
      page( ch, "Valid means whether the caster is supposed to be a valid target or not:\r\n" );
      page( ch, "True    False\r\n" );
      return;
    }
    else if( fmatches( argument, "nation" ) ) {
      page( ch, "This is one of the various nations that mobs can belong to:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_ENTRY_NATION; i++, col++ ) {
        page( ch, "%20s", nation_table[i].name );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "object.value" ) ) {
      page( ch, "Object.Value is one of the various values on an object:\r\n" );
      page( ch, "lvl   - this is the level of the object.\r\n" );
      page( ch, "cond  - this is the current condition of the object.\r\n" );
      page( ch, "dur   - this is the duratbility of the object.\r\n" );
      page( ch, "cost  - this is the cost value of the object.\r\n" );
      page( ch, "val0  - this is value[0].\r\n" );
      page( ch, "val1  - this is value[1].\r\n" );
      page( ch, "val2  - this is value[2].\r\n" );
      page( ch, "val3  - this is value[3].\r\n" );
      page( ch, "rust  - this is the rust condition of an object.\r\n" );
      page( ch, "repr  - this is the repair level of the object.\r\n" );
      page( ch, "ligt  - this is the light value of the object.\r\n" );
      page( ch, "age   - this is the age of the object.\r\n" );
      page( ch, "vnum  - this is the vnum of the object.\r\n" );
      page( ch, "cval  - this is the coin value of the object, modified by number.\r\n" );
      page( ch, "cont  - this is the number of the contents of an object.\r\n" );
      page( ch, "numb  - this is the number of that object in a pile.\r\n" );
      page( ch, "sour  - this is the source of the object (true/false).\r\n" );
      page( ch, "value - this is the value of the object in its current condition.\r\n" );
      return;
    }
    else if( fmatches( argument, "object.type" ) ) {
      page( ch, "Object Type means any of the types that an object can be:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_ITEM; i++, col++ ) {
        page( ch, "%20s", item_type_name[i] );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "dflag" ) ) {
      page( ch, "Dflag means any of the various flags that can be set upon an exit:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_DFLAG; i++, col++ ) {
        page( ch, "%20s", dflag_name[i] );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "dance" ) ) {
      page( ch, "Dance means any of the appropriate dances:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_ENTRY_DANCE; i++, col++ ) {
        page( ch, "%20s", dance_table[i].dance );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "affect.location" ) ) {
      page( ch, "Affect.Location is one of the locations that can be modified by affects:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_AFF_LOCATION; i++, col++ ) {
        page( ch, "%20s", affect_location[i] );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else if( fmatches( argument, "position" ) ) {
      page( ch, "One of the different positions that a character can be put into:\r\n" );
      int col = 1;
      for( int i = 0; i < MAX_POSITION; i++, col++ ) {
        page( ch, "%20s", position_name[i] );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
      return;
    }
    else {
      page( ch, "That is an invalid argument:\r\n" );
      int col = 1;
      for( int i = 0; i < 30; i++, col++ ) {
        page( ch, "%20s", arg_type_name[i] );
        if( col == 3 ) {
          page( ch, "\r\n" );
          col = 0;
        }
      }
      if( col != 0 )
        page( ch, "\r\n" );
    }
    return;
  }

  if( pos >= 0 && help_list[pos]->category == HELP_FUNCTIONS ) {
    convert_to_ansi( ch, help_list[pos]->text, buf, sizeof(buf) );
    page( ch, buf );
    return;
  }

  for( int i = 0; cfunc_list[i].name[0] != '\0'; i++ ) {
    if( strncasecmp( argument, cfunc_list[i].name, length ) )
      continue;
    if( !found ) {
      found = TRUE;
      sprintf( tmp, "%-20s %-45s %s\r\n", "Function Name", "Arguments", "Returns" );
      page_underlined( ch, tmp );
    }
    sprintf( tmp, "%-20s (", cfunc_list[i].name );

    bool header = false;
    for( int j = 0; j < cfunc_list[i].args; j++ ) {
      int len = strlen( tmp );
      const char* arg = arg_type_name[ cfunc_list[ i ].args[ j ] ];
      int arglen = strlen( arg );

      if( ( len + arglen ) > 65 ) {
        strcat( tmp, "," );
        sprintf( buf, "%-66s %s\r\n", tmp, arg_type_name[ cfunc_list[i].type ] );
        page( ch, buf );
        header = true;
        sprintf( tmp, "%-20s   %s", "", arg );
      } else {
        sprintf( tmp+strlen( tmp ), "%s %s", j == 0 ? "" : ",", arg );
      }
    }

    sprintf( tmp+strlen( tmp ), " )" ); 
    sprintf( buf, "%-66s %s\r\n", tmp, ( header ? "" : arg_type_name[ cfunc_list[i].type ] ) );
    page( ch, buf );
  }

  if( !found ) 
    send( ch, "No matching function found.\r\n" );
}


/*
 *   SPELL HELP ROUTINE
 */


void do_spells( char_data* ch, char* argument )
{
  int i;

  if( pet_help( ch ) || ch->link == NULL )
    return;

  if( argument[0] == '\0' ) {
    send( "What spell do you want info on?\r\n[To get a list of spells type index spells.]\r\n", ch );
    return;
  }

  for( i = 0; i < SPELL_COUNT; i++ ) {
    if( !strncasecmp( spell_table[ i ].name, argument, strlen( argument ) ) ) {
      for( int j = 0; j < MAX_CLSS; j++ )
        if( skill_table[ SPELL_FIRST + i ].level[ j ] > 0 && clss_table[ j ].open ) {
          spell_help( ch, i );
          return;
        }

      if( is_apprentice( ch ) ) { 
        spell_help( ch, i );
        return;
      }
    }
  }

  send( "Unknown Spell.\r\n", ch );
}


void spell_help( char_data* ch, int i )
{
  char                tmp  [ 3*MAX_STRING_LENGTH ];
  bool              found  = FALSE;
  obj_clss_data*  reagent;
  int                j, k;
  int                 pos;
  int              length;
  int               level;
  help_data*         help;

  sprintf( tmp,
    "%10sName: %s\r\n   Energy Cost: %s\r\n Turns to Cast: %d\r\n",
    "", spell_table[i].name, spell_table[i].cast_mana,
    spell_table[i].wait + 1);
  page( ch, tmp );

  if( spell_table[i].duration != empty_string )
    page( ch, "      Duration: %s\r\n", spell_table[i].duration );
  if( spell_table[i].damage != empty_string ) 
    page( ch, "        %s: %s\r\n", spell_table[i].type == STYPE_NON_COMBAT_HEALING ? "  Heal" : spell_table[i].type == STYPE_PEACEFUL ? spell_shield( i ) ? "Damage" : "  Heal" : "Damage", spell_table[i].damage );
  if( spell_table[i].leech_mana != empty_string ) 
    page( ch, "  Leech of Max: %s\r\n", spell_table[i].leech_mana );
  if( spell_table[i].regen != empty_string ) 
    page( ch, "   Regen Leech: %s\r\n", spell_table[i].regen );

  if( is_apprentice( ch ) )
    page( ch, "%10sSlot: %d\r\n", "", i );

  strcpy( tmp, "   Class/Level:" );
  for( j = 0; j < MAX_CLSS; j++ )
    if( ( level = skill_table[SPELL_FIRST+i].level[j] ) > 0 && clss_table[j].open ) {
      length = strlen( tmp );
      sprintf( tmp+length, "%s %s %d%s", found ? "," : "", clss_table[j].name, level, number_suffix( level ) );
      length += found+1;
      tmp[length] = toupper( tmp[length] );
      found = TRUE;
    }

  sprintf( tmp+strlen( tmp ), "%s\r\n", found ? "" : " none" ); 
  page( ch, tmp );

  found = FALSE;

  page( ch, "--------------------------------\r\n" );
  page( ch, "Description:\r\n\r\n" );

  pos = pntr_search( help_list, max_help, spell_table[i].name );

  if( pos >= 0 ) {
    help = help_list[pos];
    convert_to_ansi( ch, help->text, tmp, sizeof(tmp) );
    page( ch, tmp );

    if( help->immortal != empty_string && is_apprentice( ch ) ) {
      page( ch, "\r\n" );
      convert_to_ansi( ch, help->immortal, tmp, sizeof(tmp) );
      page( ch, tmp );
    }
  } else
    page( ch, "none\r\n" );

  page( ch, "\r\nReagents:\r\n" );  
  for( j = 0; j < MAX_SPELL_WAIT; j++ ) {
    for( k = 0; k < j; k++ )
      if( abs( spell_table[i].reagent[j] ) == abs( spell_table[i].reagent[k] ) )
        break;
    if( k != j )
      continue;
    if( ( reagent = get_obj_index( abs( spell_table[i].reagent[j] ) ) )  == NULL )
      continue;
    found = TRUE;
    page( ch, "   %s\r\n", reagent->Name( ) );
  }

  if( !found ) 
    page( ch, "  none\r\n" );

  return;
}

bool spell_shield( int shield )
{
  shield += SPELL_FIRST;

  if( shield == SPELL_FIRE_SHIELD || shield == SPELL_HOAR_FROST || shield == SPELL_GHOST_SHIELD
    || shield == SPELL_HOLY_AURA || shield == SPELL_UNHOLY_AURA || shield == SPELL_ION_SHIELD
    || shield == SPELL_THORN_SHIELD || shield == SPELL_DEMON_SHIELD 
    || shield == SPELL_WHIRLING_DEATH || shield == SPELL_FLAME_SHIELD
    || shield == SPELL_FIERY_SHIELD || shield == SPELL_INFERNO_SHIELD
    || shield == SPELL_FROST_SHIELD || shield == SPELL_HOAR_SHIELD
    || shield == SPELL_ABSOLUTE_ZERO || shield == SPELL_CORROSIVE_SHIELD
    || shield == SPELL_EROSION_SHIELD || shield == SPELL_CAUSTIC_SHIELD 
    || shield == SPELL_BLESSED_AURA || shield == SPELL_EVIL_AURA
    || shield == SPELL_HALLOWED_AURA || shield == SPELL_DREADFUL_AURA
    || shield == SPELL_SAINTLY_AURA || shield == SPELL_WICKED_AURA 
    || shield == SPELL_BRAMBLE_SKIN || shield == SPELL_THISTLE_SKIN )
    return TRUE;

  return FALSE;
}

bool note_to_help( char_data *ch, note_data *note )
{
  char *name = NULL;
  textblock text;
  textblock immortal;
  int level[2] = { 0, 0 };
  int category = -1;

  // parse note to get the stuff out of it
  textblock *temp = note->get_text();
  bool read_imm = false;
  bool failed = false;
  bool level_found = false;

  for (textblock_line *tline = temp->get_text(); tline; tline = tline->get_next()) {
    char *line = tline->get_text();
    bool processed = false;
    if (line && *line == '#') {
      // potential command
      processed = true;
      char command [MAX_INPUT_LENGTH];
      char *data = one_argument(line, command);

      if (!str_cmp(command, "#level")) {
        level_found = true;
        data = one_argument(data, command);
        if (!str_cmp(command, "none")) {
          send(ch, "No Level Specified\r\n");
        } else {
          while (*command) {
            if (command[strlen(command) - 1] == ',')
              command[strlen(command) - 1] = '\0';

            if (!permission_flags.set(ch, command, level)) {
              send(ch, "--- Undefined Level %s!\r\n", command);
              failed = true; 
            }
            data = one_argument(data, command);
          }
        }
      } else if (!str_cmp(command, "#category")) {
        #define hcn( i )   help_cat_table[i].name
        class type_field type_list[] = {
          { "category",  MAX_ENTRY_HELP_CAT,  &hcn(0),  &hcn(1),  &category       },
          { "",          0,                   NULL,     NULL,     NULL            }
        };
        #undef hcn
        
        if (!process(type_list, ch, note->get_title(), line + 1)) {
          send(ch, "--- Undefined Category %s!\r\n", data);
          failed = true;
        }
      } else if (!str_cmp(command, "#name")) {
        name = str_dup(data);
      } else if (!str_cmp(command, "#text")) {
        read_imm = false;
      } else if (!str_cmp(command, "#immortal")) {
        read_imm = true;
      } else {
        processed = false;
      }
    }
    
    if (!processed) {
      if (read_imm)
        immortal.add_text(line);
      else
        text.add_text(line);       
    }
  }

  if (!failed && name && level_found && category != -1) {
    /*
    send(ch, "Title: %s\r\n", name);
    send(ch, "Category: %s\r\n", help_cat_table[category].name);

    char tmp[MAX_STRING_LENGTH];
    permission_flags.sprint(tmp, level);
    send(ch, "Level: %s\r\n", tmp);

    send(ch, text.get_string());
    send(ch, "\r\n-----\r\n");

    if (immortal.has_text()) {
      send(ch, immortal.get_string());
      send(ch, "\r\n");
    }
    */

    /*
    int pos = pntr_search( help_list, max_help, "blank" );
    if( pos >= 0 ) 
      help->text = alloc_string( help_list[pos]->text, MEM_HELP );
    else
      help->text = (char*) empty_string;
    */

    int pos = pntr_search( help_list, max_help, name );
    if (pos < 0)
      pos = -pos - 1;

    help_data *help = new help_data;
    help->name = alloc_string(name, MEM_HELP);
    help->level[0] = level[0];
    help->level[1] = level[1];
    help->category = category;
    char *temp = text.get_string(true);
    help->text = alloc_string(temp ? temp : "", MEM_HELP);
    temp = immortal.get_string(true);
    help->immortal = alloc_string(temp ? temp : "", MEM_HELP);

    ch->pcdata->help_edit = help;
    insert( help_list, max_help, help, pos );
    help_modified = TRUE;
  } else {
    failed = true;
    if (!name)
      send(ch, "No Name Found\r\n");
    if (category == -1)
      send(ch, "No Category Found\r\n");
    if (!level_found)
      send(ch, "No Level Found\r\n");
  }

  if (name)
    delete [] name;

  return !failed;
}


