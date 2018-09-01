#include "system.h"

clan_array    clan_list;

void           rename_clan       ( clan_data*, char* );
void           create_clan       ( pfile_data*, char*, bool );
bool           good_clan_name    ( char_data*, char* );
void           display           ( player_data*, clan_data* );
void           extract           ( clan_data* );
bool           edit_clan         ( player_data*, clan_data*, bool = FALSE );
void           expell            ( char_data*, clan_data*, pfile_data* );
clan_data*     load_clan         ( const char* );
bool           verify            ( clan_data* );
bool           verify            ( title_data* );

const char* clan_flags [ MAX_CLAN_FLAGS ] = { "#approved", "Exist.Known", "Members.Known" };


/*
 *   CLAN ROUTINES
 */

Clan_Data::Clan_Data( ) {
  record_new( sizeof( clan_data ), MEM_CLAN );
  abbrev      = empty_string;
  name        = empty_string;
  charter     = empty_string;
  guild       = false;
  flags[0]    = 0;
  flags[1]    = 0;
  min_level   = 5;
  classes     = 0;
  races       = 0;
  alignments  = 0;
  sexes       = 0;
  modified    = FALSE;
  noteboard   = NULL;
  date        = time(0);
  tells       = NULL;
  insert( clan_list, this, clan_list );
}


Clan_Data::~Clan_Data( ) {
  record_delete( sizeof( clan_data ), MEM_CLAN );
  free_string( name, MEM_CLAN );
  free_string( abbrev, MEM_CLAN );
  free_string( charter, MEM_CLAN );
  if( noteboard )
    delete noteboard;
  delete_list( tells );
}


clan_data* find_clan( player_data* ch, char* argument )
{
  clan_data*  clan;

  for( int i = 0; i < clan_list.size; i++ ) 
    if( knows( ch, clan = clan_list[i] ) && ( fmatches( argument, clan->name ) || fmatches( argument, clan->abbrev ) ) )
      return clan;

  send( ch, "No such clan exists.\r\n" );

  return NULL;
}


bool edit_clan( player_data* ch, clan_data* clan, bool msg )
{
  title_data*  title;

  if( clan == NULL ) {
    if( msg ) 
      send( ch, "You are not in a clan so editing it makes no sense.\r\n" );
    return FALSE;
  }

  if( !has_permission( ch, PERM_CLANS ) ) {
    if( is_set( clan->flags, CLAN_APPROVED ) ) {
      if( msg )
        send( ch, "After the clan is approved it may not be edited.\r\n" );
      return FALSE;
    }

    if( ( title = get_title( clan, ch->pcdata->pfile ) ) == NULL || !is_set( title->flags, TITLE_EDIT_CHARTER ) ) {
      if( msg )
        send( ch, "You don't have permission to edit the clan.\r\n" );
      return FALSE;
    }
  }

  return TRUE;
}


void clan_options( player_data* ch, clan_data* clan, char* argument )
{
  const char* response;

  #define types 5

  if( clan == NULL ) {
    send( ch, "You cannot edit or view options for a clan you are not in.\r\n" );
    return;
  }    

  const char* title [types] = { "*Options", "*Races", "*Classes", "*Alignments", "*Sexes" };
  int max [types] = { MAX_CLAN_FLAGS, MAX_PLYR_RACE, MAX_CLSS, MAX_ENTRY_ALIGNMENT, 2 };
  const char** name1 [types] = { &clan_flags[0], &race_table[0].name, &clss_table[0].name, &alignment_table[0].name, &sex_name[ SEX_MALE ] }; 
  const char** name2 [types] = { &clan_flags[1], &race_table[1].name, &clss_table[1].name, &alignment_table[1].name, &sex_name[ SEX_FEMALE ] }; 
  int* flag_value [types] = { clan->flags, &clan->races, &clan->classes, &clan->alignments, &clan->sexes };
  int can_edit [types] = { 1, -1, -1, -1, -1 };
  int not_edit [types] = { 1,  1,  1,  1,  1 };

  response = flag_handler( title, name1, name2, flag_value, max, edit_clan( ch, clan ) ? can_edit : not_edit,
    has_permission( ch, PERM_CLANS ) ? (const char*) NULL : "You do not have permission to alter that flag.\r\n",
    ch, argument, types );

  clan->modified = TRUE;

  #undef types

  return;
}


void clan_titles( char_data* ch, clan_data* clan, char* argument )
{
  title_data*  title;
  int              i;

  if( clan == NULL ) {
    send( ch, "Since you aren't in a clan the T option is meaningless.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    page_underlined( ch, "%-20s %s\r\n", "Title", "Character" );
    for( i = 0; i < clan->titles.size; i++ ) {
      title = clan->titles.list[i];
      page( ch, "%-20s %s\r\n", title->name, title->pfile == NULL ? "No one" : title->pfile->name );
    }
    return;
  }

  send( ch, "Editting of titles is not yet enabled.\r\n" );
}


void create_clan( pfile_data* pfile, char* name, bool guild )
{
  *name = toupper( *name );

  clan_data *clan  = new clan_data;
  clan->abbrev = alloc_string(name, MEM_CLAN);
  clan->guild = guild;
  title_data *title  = new title_data( "Founder", pfile );

  add_member( clan, pfile );
  insert( clan->titles.list, clan->titles.size, title, 0 );

  set_bit( title->flags, TITLE_SET_FLAGS    );
  set_bit( title->flags, TITLE_EDIT_CHARTER );
  set_bit( title->flags, TITLE_RECRUIT      );
  set_bit( title->flags, TITLE_REMOVE_NOTES );

  set_bit( &clan->classes, pfile->clss );
  set_bit( &clan->races,   pfile->race );

  char temp[256], temp2[256];
  sprintf( temp, "%s_%s", guild ? "guild" : "clan", name );
  sprintf( temp2, "%s_%s.%s", guild ? "guild" : "clan", name, BOARD_SUF );

  clan->noteboard = new noteboard_data( temp, temp2, 97, 97, 97, 1, 4096 );
  save_noteboard( clan->noteboard );

  save_clans( clan );
}


const char* create_msg = "Summoning a scribe daemon you request that the\
 paperwork be drawn up to form a %s by the name of '%s'.  He politely\
 nods his head, scribbles a few lines on the back of an envelope, hands you\
 the envelope, and then informs you that your bank account will be deducted\
 20000 cp for services rendered.";


void do_clans( char_data* bob, char* argument )
{
  clan_data*      clan;
  title_data*    title;
  pfile_data*    pfile;
  int            flags;
  int                i;

  if( is_mob( bob ) )
    return;

  player_data* ch = player( bob );

  if( !get_flags( ch, argument, &flags, "fotcnag", "Clans" ) )
    return;

  if( is_set( &flags, 6 ) )
    clan = ch->pcdata->pfile->guild;
  else
    clan = ch->pcdata->pfile->clan2;

  if( exact_match( argument, "expell" ) ) {
    if( ( title = get_title( clan, ch->pcdata->pfile ) ) == NULL || !is_set( title->flags, TITLE_RECRUIT ) ) {
      send( ch, "You don't have permission to expell %s members.\r\n", clan->guild ? "guild" : "clan" );
      return;
    }
    if( *argument == '\0' ) {
      send( ch, "Who do you wish to expell from the %s?\r\n", clan->guild ? "guild" : "clan" );
      return;
    }
    if( ( pfile = find_pfile( argument, ch ) ) != NULL ) 
      expell( ch, clan, pfile );
    return;
  }

  if( has_permission( ch, PERM_CLANS ) && exact_match( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      send( ch, "Which clan do you wish to delete?\r\n" );
    } else if( ( clan = find_clan( ch, argument ) ) != NULL ) {
      extract( clan );
      send( ch, "Clan deleted.\r\n" );
    }
    return;
  }

  if( is_set( &flags, 0 ) ) {
    if( ( !is_set( &flags, 6 ) && ch->pcdata->pfile->clan2 != NULL ) || ( is_set( &flags, 6 ) && ch->pcdata->pfile->guild != NULL ) ) {
      send( ch, "You cannot form a %s if already a member of one.\r\n", is_set( &flags, 6 ) ? "guild" : "clan" );
      return;
    }

    if( ch->shdata->level < 10 ) {
      send( ch, "You must be at least 10th level to form a %s.\r\n", is_set( &flags, 6 ) ? "guild" : "clan" );
      return;
    }

    if( ch->bank < 20000 ) {
      send( ch, "There is a 20000cp charge, withdrawn from bank, for forming a %s and you lack\r\nsufficent funds.\r\n", is_set( &flags, 6 ) ? "guild" : "clan" );
      return;
    }

    if( *argument == '\0' ) {
      send( ch, "What name do you wish for the %s?\r\n", is_set( &flags, 6 ) ? "guild" : "clan" );
      return;
    }

    if( good_clan_name( ch, argument ) ) {
      create_clan( ch->pcdata->pfile, argument, is_set( &flags, 6 ) );
      ch->bank -= 20000;
      fsend( ch, create_msg, is_set( &flags, 6 ) ? "guild" : "clan", argument );
    }
    return;
  }

  if( is_set( &flags, 2 ) ) {
    clan_titles( ch, clan, argument );
    return;
  } 

  if( is_set( &flags, 1 ) ) {
    clan_options( ch, clan, argument );
    return;
  }

  if( is_set( &flags, 4 ) ) {
    if( !edit_clan( ch, clan, TRUE ) )
      return;
    if( *argument == '\0' ) {
      send( ch, "What do you want to set the name of the %s to?\r\n", clan->guild ? "guild" : "clan" );
      return;
    }
    free_string( clan->name, MEM_CLAN );
    clan->name = alloc_string( argument, MEM_CLAN );
    send( ch, "Name of %s set to %s.\r\n", clan->guild ? "guild" : "clan", argument );
    return;
  }    

  if( is_set( &flags, 5 ) ) {
    if( !edit_clan( ch, clan, TRUE ) )
      return;
    if( *argument == '\0' ) {
      send( ch, "What do you want to set the abbreviation for the %s to?\r\n", clan->guild ? "guild" : "clan" );
      return;
    }
    if( !good_clan_name( ch, argument ) )
      return; 
    rename_clan( clan, argument );
    send( ch, "Abbreviation for the %s changed to %s.\r\n", clan->guild ? "guild" : "clan", argument );
    return;
  }

  if( is_set( &flags, 3 ) ) {
    if( !edit_clan( ch, clan, TRUE ) )
      return;
    clan->modified = TRUE;
    clan->charter  = edit_string( ch, argument, clan->charter, MEM_CLAN );
    return;
  }

  if( *argument == '\0' ) {
    page_underlined( ch, "Abrv Type   %-50s Members\r\n", "Clan" );
    for( i = 0; i < clan_list.size; i++ ) 
      if( knows( ch, clan = clan_list[i] ) ) 
        page( ch, "%4s %-5s  %-50s %s\r\n", clan->abbrev, clan->guild ? "Guild" : "Clan", clan->name, knows_members( ch, clan ) ? number_word( clan->members.size ) : "???" );
    return;
  }

  if( ( clan = find_clan( ch, argument ) ) != NULL ) 
    display( ch, clan ); 
}


/*
 *   CLAN EDITTING
 */


void display_edit( char_data* ch, clan_data* clan )
{
  title_data** list  = clan->titles.list;
  char*       flags  = "FCRN";
  char*         tmp  = static_string( );
  char*      letter;
  int          i, j;

  send( ch, "    Name: %s\r\n", clan->name );
  send( ch, "  Abbrev: %s\r\n\r\n", clan->abbrev );

  send_underlined( ch, "Nmbr  Title                                   Holder              Flags\r\n" );  

  for( i = 0; i < clan->titles.size; i++ ) { 
    letter = tmp;
    for( j = 0; j < MAX_TITLE; j++ )
      if( is_set( list[i]->flags, j ) )
        *letter++ =  flags[j];
    *letter = '\0';
    send( ch, "%-6d%-40s%-20s%s\r\n", i+1, list[i]->name, list[i]->pfile == NULL ? "no one" : list[i]->pfile->name, tmp );
  } 

  return;
}


void perform_cedit( player_data* ch, char* argument, bool guild )
{
  title_data*  title;
  pfile_data*  pfile;
  int              i;

  clan_data* clan = guild ? ch->pcdata->pfile->guild : ch->pcdata->pfile->clan2;

  if( clan == NULL ) {
    send( ch, "Cedit operates on the %s you are in and you aren't in a %s.\r\n", guild ? "guild" : "clan", guild ? "guild" : "clan" );
    return;
  }

  if( *argument == '\0' ) {
    display_edit( ch, clan );
    return;
  }

  if( matches( argument, "new" ) ) {
    if( *argument == '\0' ) {
      send( ch, "What should the new title be called?\r\n" );
      return;
    }

    if( clan->titles.size > 9 ) {
      send( ch, "A clan is restricted to ten titles.\r\n" );
      return;
    }

    insert( clan->titles.list, clan->titles.size, new title_data( argument, NULL ), clan->titles.size );
    send( ch, "New title %s added.\r\n", argument );
    return;
  }

  if( matches( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      send( ch, "Which title do you wish to delete?\r\n" );
      return;
    }

    if( ( i = atoi( argument ) ) < 1 || i > clan->titles.size ) {
      send( ch, "No such title exists.\r\n" );
      return;
    }

    title = clan->titles.list[--i];
    send( ch, "Title '%s' removed.\r\n", title->name );
    remove( clan->titles.list, clan->titles.size, i );
    delete title;
    return;
  }

  if( number_arg( argument, i ) ) {
    if( i < 1 || i > clan->titles.size ) {
      send( ch, "No such title exists.\r\n" );
      return;
    }

    title = clan->titles.list[--i]; 
    set_flags( ch, argument, title->flags, "FCRN" );
    if( matches( argument, "title" ) ) {
      set_string( ch, argument, title->name, "title", MEM_CLAN );
      return;
    }

    if( matches( argument, "holder" ) ) {
      if( ( pfile = find_pfile( argument, ch ) ) != NULL ) {
        if( ( guild ? pfile->guild : pfile->clan2 ) != clan ) {
          fsend( ch, "%s isn't a member of %s.\r\n", pfile->name, clan->name );
        } else {
          title->pfile = pfile;
          send( ch, "%s set to %s.\r\n", title->name, title->pfile->name );
        }
      }
      return;
    }
  }

  send( ch, "Unknown syntax - see help cedit.\r\n" );
}


void do_cedit( char_data* ch, char* argument )
{
  if( is_mob( ch ) )
    return;

  player_data* pc = player( ch );

  perform_cedit( pc, argument, false );
}


void do_gedit( char_data* ch, char* argument )
{
  if( is_mob( ch ) )
    return;

  player_data* pc = player( ch );

  perform_cedit( pc, argument, true );
}

/*
 *   ALLEGIANCE ROUTINES
 */


bool can_join( char_data* ch, clan_data* clan )
{
  const char*  word;
  int             i;
  
  if( has_permission( ch, PERM_CLANS ) )
    return TRUE;

  if( !is_set( clan->flags, CLAN_APPROVED ) ) {
    fsend( ch, "%s has not been approved so may not accept members.", name( clan ) );
    return FALSE;
  }

  if( ch->shdata->level < clan->min_level ) {
    fsend( ch, "To join %s you must be at least level %d.", name( clan ), clan->min_level );
    return FALSE;
  }

  #define types 4

  const char** name1 [types] = { &race_table[0].name, &clss_table[0].name, &alignment_table[0].name, &sex_name[ SEX_MALE ] }; 
  const char** name2 [types] = { &race_table[1].name, &clss_table[1].name, &alignment_table[1].name, &sex_name[ SEX_FEMALE ] }; 
  int* flag_value [types] = { &clan->races, &clan->classes, &clan->alignments, &clan->sexes };
  int player_value [types] = { ch->shdata->race, ch->pcdata->clss, ch->shdata->alignment, ch->sex - SEX_MALE };

  for( i = 0; i < types; i++ ) {
    if( !is_set( flag_value[i], player_value[i] ) ) {
      word = *(name1[i]+player_value[i]*(name2[i]-name1[i]));
      fsend( ch, "As %s %s you would not be allowed to join %s.\r\n", isvowel( *word ) ? "an" : "a", word, name( clan ) );
      return FALSE;
    }
  }

  #undef types

  return TRUE;
}


void perform_allegiance( player_data* pc, char* argument, bool guild )
{
  clan_data* clan = guild ? pc->pcdata->pfile->guild : pc->pcdata->pfile->clan2;
  player_data* victim;

  if( *argument == '\0' ) {
    if( clan == NULL ) {
      if( guild )
        send( pc, "Sign up to who's guild?\r\n" );
      else
        send( pc, "Swear allegiance to whom?\r\n" );
    } else {
      if( guild ) {
        fsend( pc, "You are already a member of %s.\r\n", name( clan ) );
        send( pc, "Change your membership to who's guild?\r\n" );
      } else {
        fsend( pc, "You have sworn allegiance to %s.\r\n", name( clan ) );
        send( pc, "Switch your allegiance to whom?\r\n" );
      }
    }
    return; 
  }

  if( !strcasecmp( argument, "none" ) ) {
    if( clan == NULL ) {
      if( guild )
        send( pc, "You aren't a member of a guild.\r\n" );
      else
        send( pc, "You have sworn allegiance to no one already.\r\n" );
    } else {
      fsend( pc, "You revoke your %s %s.\r\n", guild ? "membership with" : "allegience to", name( clan ) );
      remove_member( clan, pc );
      remove_member( clan, pc->pcdata->pfile );
    }
    return;
  }

  if( clan != NULL ) {
    fsend( pc, "You must first revoke your %s %s.\r\n", guild ? "membership with" : "allegience to", name( clan ) );
    return;
  }

  if( ( victim = one_player( pc, argument, guild ? "sign up with" : "swear allegiance to", pc->array ) ) == NULL )
    return;

  if( pc == victim ) {
    if( guild )
      send( pc, "You've already signed yourself up for the long haul.\r\n" );
    else
      send( pc, "Allegiance to yourself is assumed.\r\n" );
    return;
  }

  clan = guild ? victim->pcdata->pfile->guild : victim->pcdata->pfile->clan2;

  if( clan == NULL || ( !knows_members( pc, clan ) && !victim->Befriended( pc ) ) ) {
    send( pc, "They are not a member of a %s.\r\n", guild ? "guild" : "clan" );
    return;
  }

  if( !can_join( pc, clan ) )
    return;

  if( !consenting( victim, pc ) ) {
    if( guild )
      fsend( pc, "Signing up with %s, who has not consented to you joining %s guild is pointless.", victim, victim->His_Her( ) );
    else
      fsend( pc, "Swearing allegiance to %s, who has not consented to you joining %s clan is pointless.", victim, victim->His_Her( ) );
    return;
  }

  title_data* title = get_title( clan, victim->pcdata->pfile );

  if( title == NULL || !is_set( title->flags, TITLE_RECRUIT ) ) {
    send( pc, "%s is not permitted to recruit members.\r\n", victim );
    return;
  }

  add_member( clan, pc->pcdata->pfile );
  save_clans( clan );

  if( guild ) {
    fsend( pc, "You carefully sign the membership papers of %s.", name( clan ) );
    fsend( *victim->array, "%s carefully signs the membership papers of %s.", pc, name( clan ) );
  } else {
    fsend( pc, "You kneel down and swear allegiance to %s.", name( clan ) );
    fsend( *victim->array, "%s kneels down and swears allegiance to %s.", pc, name( clan ) );
  }
}


void do_allegiance( char_data* ch, char* argument )
{
  if( is_mob( ch ) )
    return;

  player_data* pc = player( ch );

  perform_allegiance( pc, argument, false );
}


void do_sign( char_data* ch, char* argument )
{
  if( is_mob( ch ) )
    return;

  player_data* pc = player( ch );

  perform_allegiance( pc, argument, true );
}

/*
 *   INTERNAL ROUTINES
 */


bool good_clan_name( char_data* ch, char* name ) 
{
  int i;

  if( strlen( name ) != 4 ) {
    send( ch, "Clan abbreviation must be exactly 4 letters.\r\n" );
    return FALSE;
  }

  for( i = 0; i < 4; i++ ) 
    if( !isalpha( name[i] ) ) {
      send( ch, "The clan name may only contain letters.\r\n" );
      return FALSE;
    }

  for( i = 0; i < clan_list.size; i++ )
    if( !strncasecmp( name, clan_list[i]->abbrev, 4 ) ) {
      send( ch, "There is already a clan using that abbreviation.\r\n" );
      return FALSE;
    }

  return TRUE;
} 


title_data* get_title( clan_data* clan, pfile_data* pfile )
{
  if( !clan || !pfile )
    return NULL;

  for( int i = 0; i < clan->titles.size; i++ )
    if( clan->titles.list[i]->pfile == pfile )
      return clan->titles.list[i];

  return NULL;
}


/*
 *   ADDING/REMOVING MEMBERS
 */


void add_member( clan_data* clan, pfile_data* pfile )
{
  if( clan->guild ) {
    if( pfile->guild != NULL ) {
      bug( "Add_Member: Character is already in a guild." );
      return;
    }

    clan->members += pfile;
    pfile->guild    = clan;
  } else {
    if( pfile->clan2 != NULL ) {
      bug( "Add_Member: Character is already in a clan." );
      return;
    }

    clan->members += pfile;
    pfile->clan2   = clan;
  }
}


void remove_member( clan_data* clan, player_data* player )
{
  if( !clan || !player || !player->pcdata )
    return;

  pfile_data* pfile = player->pcdata->pfile;

  if( ( clan->guild ? pfile->guild : pfile->clan2 ) == NULL )
    return;

  if( player->get_current_board( ) == clan->noteboard ) {
    player->set_current_board( NULL );
    if( player->get_note_buffer( ) ) {
      delete player->get_note_buffer( );
      player->set_note_buffer( NULL );
      send( player, "The note you were working on has been deleted.\r\n" );
    }
  }
} 


void remove_member( clan_data* clan, pfile_data* pfile )
{
  if( clan == NULL || ( clan->guild ? pfile->guild : pfile->clan2 ) == NULL )
    return;

  clan->members -= pfile;

  for( int i = 0; i < clan->titles.size; i++ ) 
    if( clan->titles.list[i]->pfile == pfile )
      clan->titles.list[i]->pfile = NULL;

  if( clan->guild )
    pfile->guild = NULL;
  else
    pfile->clan2 = NULL;

  if( is_empty( clan->members ) ) {
    extract( clan );
    return;
  }

  save_clans( clan );
}


void extract( clan_data* clan )
{
  player_data*  pc;
  int                i;

  for( i = 0; i < clan_list.size; i++ ) 
    if( clan_list[i] == clan )
      break;

  if( i >= clan_list ) {
    bug( "Extract( Clan ): Non-existent clan!?" );
    return;
  }

  remove( clan_list, i );

  for( i = 0; i < player_list; i++ ) {
    pc = player_list[i]; 
    if( pc->Is_Valid( ) && ( clan->guild ? pc->pcdata->pfile->guild : pc->pcdata->pfile->clan2 ) == clan )
      remove_member( clan, pc );
  }

  if( clan->guild ) {
    for( i = 0; i < clan->members; i ++ )
      clan->members[i]->guild = NULL;
  } else {
    for( i = 0; i < clan->members; i ++ )
      clan->members[i]->clan2 = NULL;
  }

  selt_string filename;
  filename.printf( "%s%s", clan->abbrev, CLAN_SUF );
  filename.make_lowercase( );
  delete_file( CLAN_DIR, filename );

  delete clan;
} 


void expell( char_data* ch, clan_data* clan, pfile_data* pfile )
{
  if( !ch || !clan || !pfile )
    return;

  if( ( clan->guild ? pfile->guild : pfile->clan2 ) != clan ) {
    send( ch, "%s isn't in your %s.\r\n", pfile->name, clan->guild ? "guild" : "clan" );
    return;
  }

  if( ch->pcdata->pfile == pfile ) {
    send( ch, "You can't expell yourself - use allegiance none.\r\n" );
    return;
  }

  for( int i = 0; i < player_list; i++ ) { 
    player_data* pc = player_list[i];
    if( pc->Is_Valid( ) && pc->pcdata->pfile == pfile ) {
      send( pc, ">> %s has expelled you from your %s. <<\r\n", ch, clan->guild ? "guild" : "clan" );
      remove_member( clan, pc );
      break;
    }
  }

  remove_member( clan, pfile );
  send( ch, "%s expelled.\r\n", pfile->name );

  char* tmp = static_string( );
  sprintf( tmp, "%s has been expelled from the %s by %s.", pfile->name, clan->guild ? "guild" : "clan", ch->descr->name );
  info( tmp, 1, tmp, IFLAG_CLANS, 3, ch, clan );
}


/*
 *   DISPLAY ROUTINES
 */


void display( player_data* ch, clan_data* clan )
{
  char           tmp  [ 3*MAX_STRING_LENGTH ];
  pfile_data*  pfile;
  int           i, j;

  page( ch, scroll_line[0] );
  page_centered( ch, name( clan ) );
  page( ch, scroll_line[0] );

  page( ch, "\r\n" );

  if( knows_members( ch, clan ) ) {
    page_title( ch, "Leaders" );
    for( i = 0; i < clan->titles.size; i++ ) {
      pfile = clan->titles.list[i]->pfile;
      page( ch, "%38s :: %s\r\n", clan->titles.list[i]->name, pfile == NULL ? "No one" : pfile->name );
    }

    page( ch, "\r\n" );
    page_title( ch, "Members" );
    for( i = j = 0; i < clan->members.size; i++ ) 
      page( ch, "%18s%s",  clan->members.list[i]->name, (j++)%4 != 3 ? "" : "\r\n" );
    page( ch, "\r\n%s", j%4 != 0 ? "\r\n" : "" );
  }

  page_title( ch, "Charter" );

  convert_to_ansi( ch, clan->charter, tmp, sizeof(tmp) );
  page( ch, tmp );
}

//////// BELOW CODE IS SELT-OLC-DATABASE FUNCTIONS ////////
// load all clans from the clan directory
void load_clans(void)
{
  selt_string filespec;
  filelist list;

  echo( "Loading Clans ...\r\n" );

  filespec.printf( "%s*%s", CLAN_DIR, CLAN_SUF );
  dirlist( &list, filespec );

  while( !list.empty ) {
    char *filename = list.headitem( );
    list.remove( filename );

    clan_data *clan = load_clan( filename );
    if( clan )
      clan_list.add( clan );
    else
      log( "SYSERR: Clan '%s' did not boot correctly.", filename );

    delete [] filename;
  }
}

// Clan Saver  'Front-End'
bool save_clans( clan_data *clan )
{
  if( clan == NULL ) {
    for( int i = 0; i < clan_list.size; i++ )
      save_clans( clan_list[i] );
    return true;
  }

  selt_string filename;
  filename.printf( "%s%s%s", CLAN_DIR, clan->abbrev, CLAN_SUF );

  sdb_output *ss = new sdb_output( filename );

  bool success = ss->save( clan );
  delete ss;  // Killing the sdb_output closes the file and cleans up.
  return success;
}

// Clan Loader  'Front End'
clan_data *load_clan( const char *filename )
{
  clan_data *clan = NULL;

  if (filename == NULL || *filename == '\0')
    return NULL;

  selt_string tfilename;
  tfilename.printf( "%s%s", CLAN_DIR, filename );

  sdb_input *ss = new sdb_input( tfilename );
  int type = ss->get_type( );

  if( type == SDB_CLAN ) {
    clan = (clan_data *) ss->get_data( );
  } else if( type == SDB_UNDEFINED ) {
    log( "File '%s' probably doesn't exist.  Clan not loaded.", tfilename.get_text( ) );
  } else {
    log( "File '%s' is masquerading as a clan file (appears to be of SDB type: '%s')", tfilename.get_text( ), lookup( ss->get_type( ), &sdb_types[ 0 ] ) );
  }

  delete ss;  // Killing the sdb_input closes the file and cleans up.
  return clan;
}


//// SDB OUTPUT FUNCTIONS ////
// clan saver
bool sdb_output::save(clan_data *clan)
{
  bool success = TRUE;

  if( !clan )
    return false;

  // mark new record
  if( begin( lookup( SDB_CLAN, &sdb_types[ 0 ] ) ) == FALSE )
    success = FALSE;

  // save data
  if( clan->name )
    if( save( "Name", clan->name ) == FALSE )
      success = FALSE;

  if( clan->abbrev )
    if( save( "Abbrev", clan->abbrev ) == FALSE )
      success = FALSE;

  if( clan->charter )
    if( save( "Charter", clan->charter ) == FALSE )
      success = FALSE;

  if( clan->noteboard && clan->noteboard->get_name( ) )
    if( save( "Board", clan->noteboard->get_name( ) ) == FALSE )
      success = FALSE;

  if( save( "Flags0", clan->flags[ 0 ] ) == FALSE )
    success = FALSE;
  if( save( "Flags1", clan->flags[ 1 ] ) == FALSE )
    success = FALSE;
  if( save( "Level", clan->min_level ) == FALSE )
    success = FALSE;
  if( save( "Races", clan->races ) == FALSE )
    success = FALSE;
  if( save( "Classes", clan->classes ) == FALSE )
    success = FALSE;
  if( save( "Alignments", clan->alignments ) == FALSE )
    success = FALSE;
  if( save( "Sexes", clan->sexes ) == FALSE )
    success = FALSE;
  if( save( "Date", clan->date ) == FALSE )
    success = FALSE;
  if( save( "Guild", clan->guild ? 1 : 0 ) == FALSE )
    success = FALSE;

  for( int i = 0; i < clan->titles; i++ )
    if( save( clan->titles[ i ] ) == FALSE )
      success = FALSE;

  for( int i = 0; i < clan->members; i++ )
    if( save( "Member", clan->members[ i ]->ident ) == FALSE )
      success = FALSE;

  // end record
  end( );

  return success;
}

// clan parser
bool sdb_input::parse(clan_data *clan, int datatype, const char *tag, void *data)
{
  switch (datatype) {
  case SDB_INT: {
    int value = (int) data;
    if (tag && !str_cmp(tag, "Member")) {
      // special case - member id
      pfile_data *pfile;
      if( ( pfile = get_pfile( value ) ) != NULL ) 
        add_member( clan, pfile );
    } else if (tag && !str_cmp(tag, "Flags0")) {
      clan->flags[0] = value;
    } else if (tag && !str_cmp(tag, "Flags1")) {
      clan->flags[1] = value;
    } else if (tag && !str_cmp(tag, "Level")) {
      clan->min_level = value;
    } else if (tag && !str_cmp(tag, "Races")) {
      clan->races = value;
    } else if (tag && !str_cmp(tag, "Classes")) {
      clan->classes = value;
    } else if (tag && !str_cmp(tag, "Alignments")) {
      clan->alignments = value;
    } else if (tag && !str_cmp(tag, "Sexes")) {
      clan->sexes = value;
    } else if (tag && !str_cmp(tag, "Date")) {
      clan->date = value;
    } else if (tag && !str_cmp(tag, "Guild")) {
      clan->guild = value != 0 ? true : false;
    } else {
      log("SDB_INPUT: parse_clan doesn't know what to do with the (%s) tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
                } break;

  case SDB_STRING:
    if (tag && !str_cmp(tag, "Name")) {
      clan->name = (char *) data;
    } else if (tag && !str_cmp(tag, "Board")) {
      // locate the noteboard
      board_nav blist(&noteboards);
      for (noteboard_data *board = blist.first(); board; board = blist.next()) {
        if (fmatches((char *) data, board->get_name())) {
          clan->noteboard = board;
          break;
        }
      }
    } else if (tag && !str_cmp(tag, "Abbrev")) {
      clan->abbrev = (char *) data;
    } else if (tag && !str_cmp(tag, "Charter")) {
      clan->charter = (char *) data;
    } else {
      log("SDB_INPUT: parse_clan doesn't know what to do with the (%s) tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
    break;

  case SDB_TEXTBLOCK:
    if (tag && !str_cmp(tag, "Charter")) {
      clan->charter = ((textblock *) data)->get_string();
      delete (textblock *) data; // no leaks
    } else {
      log("SDB_INPUT: parse_clan doesn't know what to do with the (%s) tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
    break;

  case SDB_TITLE: {
    title_data *title = (title_data *) data;
    if (title) {
      insert(clan->titles, title, clan->titles);
    } else {
      log("SDB_INPUT: parse_clan didn't find the title supposedly passed to it.");
      return FALSE;
    }
                  } break;

  case SDB_END:
    return verify(clan);

  default:
    log("SDB_INPUT: parse_clan doesn't know what to do with the %s datatype.", lookup(datatype, &sdb_types[0]));
    return FALSE;
  }

  return TRUE;
}

// title saver
bool sdb_output::save(title_data *title)
{
  bool success = TRUE;

  if (!title)
    return false;

  // mark new record
  if (begin(lookup(SDB_TITLE, &sdb_types[0])) == FALSE)
    success = FALSE;

  // save data
  if (title->name)
    if (save("Name", title->name) == FALSE)
      success = FALSE;

  if (save("Pfile", title->pfile == NULL ? -1 : title->pfile->ident) == FALSE)
    success = FALSE;
  if (save("Flags0", title->flags[0]) == FALSE)
    success = FALSE;
  if (save("Flags1", title->flags[1]) == FALSE)
    success = FALSE;

  // end record
  end();

  return success;
}

// title parser
bool sdb_input::parse(title_data *title, int datatype, const char *tag, void *data)
{
  switch (datatype) {
  case SDB_INT: {
    int value = (int) data;
    if (tag && !str_cmp(tag, "Pfile")) {
      // special case - member id
      title->pfile = get_pfile( value );
    } else if (tag && !str_cmp(tag, "Flags0")) {
      title->flags[0] = value;
    } else if (tag && !str_cmp(tag, "Flags1")) {
      title->flags[1] = value;
    } else {
      log("SDB_INPUT: parse_title doesn't know what to do with the (%s) tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
                } break;

  case SDB_STRING:
    if (tag && !str_cmp(tag, "Name")) {
      title->name = (char *) data;
    } else {
      log("SDB_INPUT: parse_title doesn't know what to do with the (%s) tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
    break;

  case SDB_END:
    return verify(title);

  default:
    log("SDB_INPUT: parse_title doesn't know what to do with the %s datatype.", lookup(datatype, &sdb_types[0]));
    return FALSE;
  }

  return TRUE;
}

bool verify( clan_data *clan )
{
  if( !clan )
    return FALSE;

  return TRUE;
}

bool verify( title_data *title )
{
  if( !title )
    return FALSE;

  return TRUE;
}

void rename_clan( clan_data* clan, char* new_name )
{
  selt_string temp1, temp2;
  temp1.printf("%s%s", clan->abbrev, CLAN_SUF);
  temp1.make_lowercase( );
  temp2.printf("%s%s", new_name, CLAN_SUF);
  temp2.make_lowercase( );

  rename_file( CLAN_DIR, temp1, CLAN_DIR, temp2 );
  free_string( clan->abbrev, MEM_CLAN );
  clan->abbrev = alloc_string( new_name, MEM_CLAN );
  save_clans( clan );

  if( clan->noteboard ) {
    char temp[ 256 ], temp2[ 256 ];
    sprintf( temp, "clan_%s", new_name );
    sprintf( temp2, "clan_%s.%s", new_name, BOARD_SUF );
    clan->noteboard->rename( temp, temp2 );
  }

  save_clans( clan );
}

