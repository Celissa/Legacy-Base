#include "system.h"


void         save_mail        ( pfile_data* );   
Note_Data*   find_mail        ( char_data*, int );
void         display_mail     ( char_data* );
void         send_mail        ( char_data*, char, char* );    


//
//   Note_Data CLASS
//


Note_Data :: Note_Data( )
{
  record_new( sizeof( Note_Data ), MEM_NOTE );

  next      = NULL;
  from      = empty_string;
  title     = empty_string;
  message   = empty_string;
  date      = 0;
  noteboard = 0;
};


Note_Data :: ~Note_Data( )
{
  record_delete( sizeof( Note_Data ), MEM_NOTE );

  free_string( title,   MEM_NOTE );
  free_string( message, MEM_NOTE );
  free_string( from,    MEM_NOTE );
};



//
//   SUPPORT FUNCTIONS
//


bool acceptable_title( char_data* ch, const char* title )
{ 
  if( strlen( title ) <= 40 ) 
    return TRUE;

  send( "The title of messages must be less than 40 characters.\r\n", ch );
  return FALSE;
}


void reverse( Note_Data*& list )
{
  Note_Data*  temp;
  Note_Data*  pntr;

  pntr = list;
  list = NULL;

  for( ; pntr != NULL; ) {
    temp       = pntr->next;
    pntr->next = list;
    list       = pntr;
    pntr       = temp;
    }

  return;
}



/*
 *   MAIL ROUTINES
 */


const char* message1 =
  "The mail daemon takes the letter, opens it, reads it carefully,\
 rolls on\r\nthe floor laughing, and wanders off to find %s.\r\n";

const char* message2 =
  "The mail daemon takes the letter and sprints off to find %s.\r\n";

const char* message3 =
  "You give a mail daemon your letter.  The mail daemon loiters nearby and\
 whistles a tune.  When he realizes you aren't going to pay postage, he\
 stamps around just out of reach and mumbles rudely about immortals,\
 before slowly wandering off in the wrong direction.";

const char* message4 =
  "You give a mail daemon your letter.  He stands nearby with his hand out\
 for a few minutes and then grumbles and wanders off while chewing on a\
 corner of the letter.";

const char* dream_msg =
  "You really don't want to invite a daemon into your dreams.\r\n";


void mail_message( char_data* ch )
{
  char           tmp  [ TWO_LINES ];
  Note_Data*    note;
  int          total  = 0;
  int         recent  = 0;

  for( note = ch->pcdata->pfile->mail; note != NULL; note = note->next ) {
    total++;
    if( note->date > ch->pcdata->pfile->last_on ) 
      recent++;
    }

  if( total != 0 ) {
    sprintf( tmp, "You have %s mail message%s (%d new).", number_word( total ), total == 1 ? "" : "s", recent );
    send_centered( ch, tmp );
  }

  return;
}


Note_Data* find_mail( char_data* ch, int number )
{
  Note_Data*   note  = ch->pcdata->pfile->mail;
  int          line  = 0;

  if( is_set( ch->pcdata->pfile->flags, PLR_REVERSE ) )
    number = count( note )-number+1;

  for( ; note != NULL; note = note->next )
    if( ++line == number )
      return note;

  send( ch, "You have no mail by that number.\r\n" );

  return NULL;
}


void do_mail( char_data* ch, char* argument )
{
  char               tmp  [ 3*MAX_STRING_LENGTH ];
  Note_Data*        note;
  Note_Data*    note_new;
  int                  i;

  if( is_confused_pet( ch ) || ch->pcdata == NULL )
    return;

  if( ch->species != NULL ) {
    send( ch, "You can only use the mail system in human form.\r\n" );
    return;
    }

  if( ( note = ch->pcdata->mail_edit ) == NULL ) {
    if( *argument == '\0' ) {
      display_mail( ch );
      return;
      }

    if( isdigit( *argument ) ) {
      if( ( note = find_mail( ch, atoi( argument ) ) ) == NULL )
        return;
      page( ch, "Title: %s\r\n", note->title );
      page( ch, "From:  %s\r\n\r\n", note->from );
      convert_to_ansi( ch, note->message, tmp, sizeof(tmp) );
      page( ch, tmp );
      return;
    }

    if( exact_match( argument, "delete" ) ) {
      if( ch->pcdata->pfile->mail == NULL ) {
        send( ch, "You have no mail to delete.\r\n" );
        return;
      }
      if( !strcasecmp( argument, "all" ) ) {
        delete_list( ch->pcdata->pfile->mail );
        send( ch, "All mail messages deleted.\r\n" );
      } else {
        if( ( note = find_mail( ch, atoi( argument ) ) ) == NULL )
          return;
        remove( ch->pcdata->pfile->mail, note );
        delete note;
        send( ch, "Mail message deleted.\r\n" );
      }
      save_mail( ch->pcdata->pfile );
      return;
    }

    if( exact_match( argument, "edit" ) ) {
      if( ( note = find_mail( ch, i = atoi( argument ) ) ) == NULL )
        return;
      note_new               = new Note_Data;
      note_new->title        = alloc_string( note->title, MEM_NOTE );
      note_new->message      = alloc_string( note->message, MEM_NOTE );
      note_new->from         = alloc_string( ch->real_name( ), MEM_NOTE );
      note_new->noteboard    = NOTE_PRIVATE;
      ch->pcdata->mail_edit  = note_new;
      send( ch, "Copied message %d to mail buffer.\r\n", i );
      return;
    }

    if( !acceptable_title( ch, argument ) )
      return;
 
    note                   = new Note_Data;
    note->title            = alloc_string( argument, MEM_NOTE );
    note->message          = alloc_string( "", MEM_NOTE );
    note->from             = alloc_string( ch->real_name( ), MEM_NOTE );
    note->noteboard        = NOTE_PRIVATE;
    ch->pcdata->mail_edit  = note;
    send( ch, "Starting new message with subject %s.\r\n", note->title );
    return;
  }

  if( exact_match( argument, "title" ) ) {
    if( !acceptable_title( ch, argument ) )
      return;
    free_string( note->title, MEM_NOTE );
    note->title = alloc_string( argument, MEM_NOTE );
    send( ch, "Mail title changed to '%s'.\r\n", argument );
    return;
    }    

  if( exact_match( argument, "send" ) ) {
    if( *argument == '\0' ) {
      send( ch, "Whom do you want to send the message to?\r\n" );
      return;
      } 
    send_mail( ch, 'S', argument );
    return;
    }

  if( exact_match( argument, "cc" ) ) {
    send_mail( ch, 'C', argument );
    return;
    }

  if( !strcasecmp( argument, "delete" ) ) {
    send( ch, "The message you were editing has been deleted.\r\n" );
    delete note;
    ch->pcdata->mail_edit = NULL;
    return;
    }
  
  sprintf( tmp, "Subject: %s\r\n\r\n", note->title );
  page( ch, tmp );
 
  ch->pcdata->mail_edit->message = edit_string( ch, argument,
    ch->pcdata->mail_edit->message, MEM_NOTE );

  return;
}


void display_mail( char_data* ch )
{
  char          tmp  [ TWO_LINES ];
  Note_Data*   note;
  int          line  = 0;

  if( ch->pcdata->pfile->mail == NULL ) { 
    send( "You have no mail.\r\n", ch );
    return;
    }

  if( is_set( ch->pcdata->pfile->flags, PLR_REVERSE ) )
    reverse( ch->pcdata->pfile->mail );

  for( note = ch->pcdata->pfile->mail; note != NULL; note = note->next ) {
    sprintf( tmp, "[%2d] %-34s %-15s %s\r\n", ++line, note->title,
      note->from, ltime( note->date ) );
    page( ch, tmp );
    }

  if( is_set( ch->pcdata->pfile->flags, PLR_REVERSE ) )
    reverse( ch->pcdata->pfile->mail );

  return;
}


void send_mail( char_data* ch, char letter, char* argument )    
{
  char             tmp  [ TWO_LINES ];
  Note_Data*      note;
  Note_Data*  note_new;
  pfile_data*    pfile;

  note = ch->pcdata->mail_edit;

  if( *argument == '\0' ) {
    send( ch, "%s the message to whom?\r\n", letter == 'C' ? "Cc" : "Send" );
    return;
  }

  if( ( pfile = find_pfile( argument ) ) == NULL ) {
    send( ch, "There is no one by that name.\r\n" );
    return;
  } 

  if( ch->position == POS_SLEEPING ) {
    send( ch, dream_msg );
    return;
  }

  if( ch->pcdata->pfile != pfile ) {
    sprintf( tmp, "You give a mail daemon your letter and" );
    if( !remove_coins( ch, 10, tmp ) ) {
      if( ch->shdata->level < LEVEL_APPRENTICE ) {
        send( ch, "You don't have the silver coin required to mail a letter.\r\n" );
        return;
      }
      fsend( ch, number_range( 0, 1 ) == 0 ? message3 : message4 );
    } else {
      send( ch, number_range( 0, 50 ) == 0 ? message1 : message2, pfile->name );
    }
  } else {
    send( ch, "You %s the letter to yourself.\r\n", letter == 'S' ? "send" : "cc a copy of" );
  }

  if( letter != 'S' ) {
    note_new              = new Note_Data;
    note_new->title       = alloc_string( note->title,    MEM_NOTE );
    note_new->message     = alloc_string( note->message,  MEM_NOTE );
    note_new->from        = alloc_string( note->from,     MEM_NOTE );
    note_new->noteboard   = NOTE_PRIVATE;
    note                  = note_new;
  } else {
    ch->pcdata->mail_edit = NULL;
  }

  note->date = time(0);

  append( pfile->mail, note );
  save_mail( pfile );

  if( ch->pcdata->pfile != pfile ) {
    link_nav links( &link_list );
    for( link_data* link = links.first(); link != NULL; link = links.next() ) {
      if( link->character != NULL && link->connected == CON_PLAYING && link->character->pcdata->pfile == pfile ) {
        send( link->character, "A mail daemon runs up and hands you a letter from %s.\r\n", ch );
        break;
      }
    }
  }
}


/*
 *   DISK ROUTINES
 */


void read_mail( pfile_data *pfile )
{
  char          tmp  [ TWO_LINES ];
  Note_Data*   note;
  char*       title;
  FILE*          fp;

  sprintf( tmp, "%s%s", MAIL_DIR, pfile->name );

  if( ( fp = fopen( tmp, "rb" ) ) != NULL ) { 
    if( strcmp( fread_word( fp ), "#MAIL" ) ) {
      bug( "Read_mail: missing header", 0 );
      throw( 1 );
      }

    for( ; ; ) {
      title = fread_string( fp, MEM_NOTE );
      if( title[0] == '$' ) {
        free_string( title, MEM_NOTE );
        break;
        }

      if( strlen( title ) > 40 )
        title[40] = '\0';

      note            = new Note_Data;
      note->title     = title;
      note->from      = fread_string( fp, MEM_NOTE );
      note->message   = fread_string( fp, MEM_NOTE ); 
      note->noteboard = NOTE_PRIVATE;
      note->date      = fread_number( fp ); 

      append( pfile->mail, note );
    }
    fclose( fp );
  }

  return;
}


void save_mail( pfile_data *pfile )
{
  FILE*         fp;
  Note_Data*  note;

  if( pfile->mail == NULL ) {
    delete_file( MAIL_DIR, pfile->name, FALSE );
    }     
  else if( ( fp = open_file( MAIL_DIR, pfile->name, "wb" ) ) != NULL ) {
    fprintf( fp, "#MAIL\n\n" );

    for( note = pfile->mail; note != NULL; note = note->next ) {
      fprintf( fp, "%s~\n", note->title );
      fprintf( fp, "%s~\n", note->from );
      fprintf( fp, "%s~\n", note->message );
      fprintf( fp, "%d\n",  int( note->date ) );
      }

    fprintf( fp, "$~\n" );
    fclose( fp );
    }

  return;
}


