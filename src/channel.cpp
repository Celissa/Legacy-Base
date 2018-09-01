#include "system.h"


/*
 *   LOCAL FUNCTIONS
 */


const char*  say_verb       ( char_data*, char_data* );
void         say_routine    ( char_data*, char*, int );
void         add_tell       ( tell_data*&, const char*, const char*, int = -1 );
void         display        ( tell_data*&, char_data*, const char*, char_data* = NULL );
void         garble_string  ( char*, char*, int );
void         tell_message   ( char_data*, char*, player_data*, int = 0 );


path_func   hear_yell;

tell_data*  ooc_history  = NULL;
tell_data*  atalk_history = NULL;


/*
 *   TELL CLASS AND SUPPORT ROUTINES
 */


Tell_Data :: Tell_Data( const char* pers, const char* txt, int tongue )
{
  record_new( sizeof( tell_data ), MEM_TELL );

  name     = alloc_string( pers, MEM_TELL );
  message  = alloc_string( txt, MEM_TELL ); 
  language = tongue; 
  tm       = time(0);
  *name    = toupper( *name );
  next     = NULL;
}


Tell_Data :: ~Tell_Data( )
{
  record_delete( sizeof( tell_data ), MEM_TELL );

  free_string( name, MEM_TELL );
  free_string( message, MEM_TELL );
}


void tell_display( tell_data* tell, char_data* ch, char_data* victim, bool reverse, bool first )
{
  if( tell == NULL )
    return;

  if( reverse )
    tell_display( tell->next, ch, victim, reverse, false );

  char tmp[ MAX_STRING_LENGTH ], tmp1[ MAX_STRING_LENGTH ];
  sprintf_minutes( tmp1, time( 0 ) - tell->tm );
  sprintf( tmp, "[%s >> %s]\r\n%s%s", tell->name, tmp1, tell->message, ( reverse && first || !reverse && !tell->next ) ? "" : "\r\n" );
  page( ch, tmp );

  if( !reverse )
    tell_display( tell->next, ch, victim, reverse, false );
}


void display( tell_data*& list, char_data* ch, const char* text, char_data* victim )
{
  char         tmp  [ MAX_STRING_LENGTH ];
  int            i;

  if( list == NULL ) {
    if( &list == &ooc_history ) {
      send( ch, "There have been no OOC's since the mud was started.\r\n" );
    } else if( victim == NULL || victim == ch ) {
      send( ch, "You haven't heard any %s since logging in.\r\n", text );
    } else {
      send( ch, "%s hasn't heard any %s since logging in.\r\n", victim, text );
    }
    return;
  }

  sprintf( tmp, "--- " );
  for( i = 0; i < int( strlen( text ) ); i++ )
    tmp[i+4] = toupper( text[i] );
  sprintf( &tmp[ strlen( text )+4 ], " ---" );

  page_centered( ch, tmp );

  tell_display( list, ch, victim, is_set( ch->pcdata->pfile->flags, PLR_REVERSE ), true );
}


void add_tell( tell_data*& list, const char* pers, const char* message, int language )
{
  tell_data*  tell;

  tell = new tell_data( pers, message, language );

  append( list, tell );

  if( count( list ) > 25 ) {
    tell = list;
    list = tell->next;
    delete tell;
  }

  return;
} 


/*
 *   LANGUAGE ROUTINES
 */


void do_language( char_data *ch, char *argument )
{
  int i;

  if( is_confused_pet( ch ) || ch->pcdata == NULL )
    return;

  if( ch->pcdata->speaking < LANG_COMMON || ch->pcdata->speaking >= LANG_MAX )
    ch->pcdata->speaking = LANG_COMMON; 

  if( *argument == '\0' ) {
    send( ch, "You are currently speaking %s.\r\nType 'lang <lang name>' to switch to another language.\r\n", skill_table[ch->pcdata->speaking].name );
    return;
  }

  for( i = LANG_FIRST; i < LANG_MAX; i++ )
    if( fmatches( argument, skill_table[i].name ) ) {
/*
      if( i == LANG_PRIMAL && ch->shdata->level < LEVEL_APPRENTICE ) {
        send( ch, "Only immortals can speak in Primal.\r\n" );
        return;
      }
*/
      if( !ch->get_skill( i ) && !is_set( ch->affected_by, AFF_TONGUES ) ) {
        send( ch, "You don't know that language.\r\n" );
        return;
      }
      ch->pcdata->speaking = i;
      send( ch, "You will now speak in %s.\r\n", skill_table[i].name );
      return;
    }

  send( ch, "Unknown language.\r\n" );
  return;
}


int get_language( char_data* ch, int i )
{
  if( i < LANG_FIRST || i >= LANG_MAX )
    return 0;

  if( ch->pcdata == NULL || is_set( ch->affected_by, AFF_TONGUES ) )
    return MAX_SKILL_LEVEL;

  return ch->get_skill( i );
}


void garble_string( char* output, char* input, int skill )
{
//  skill = (200-10*skill)*10*skill;
  skill = 1000*skill;

  for( ; *input != '\0'; input++, output++ ) {
    if( number_range( 0, 1000*MAX_SKILL_LEVEL ) > skill ) {
      if( ispunct( *input ) || isspace( *input ) ) 
        *output = *input;
      else if( ( *input > 90 ) && ( *input < 97 ) )
        *output = *input-10;
      else
        *output = number_range( 'a', 'z' );
    } else 
      *output = *input;
  }
      
  *output = '\0';
  return;
}


char* slang( char_data* ch, int language, bool primal )
{
  if( !is_set( ch->pcdata->pfile->flags, PLR_LANG_ID ) )
    return empty_string;

  if( get_language( ch, language ) == 0 )
    return " (in an unknown tongue)";

  char* tmp = static_string( );

  sprintf( tmp, " (in %s)", language == LANG_COMMON && primal ? "primal" : skill_table[ language ].name );
 
  return tmp;
}


bool munchkin( char_data* ch, char* text )
{
/*
  int length =  strlen( text );
  int  punct =  0;
   
  for( int i = 0; i < length; i++ )    
    if( text[i] == '?' || text[i] == '!' )
      punct++; 

  if( punct > 3 && length/punct < 20 ) { 
    send( ch, "Excessive punctuation is a sure sign that you shouldn't be heard from, please\r\nsurpress your munchkin tendencies.\r\n" );
    return TRUE;
    }
*/
  return FALSE;
}


/*
 *   CHANNEL SUPPORT ROUTINES
 */


inline bool hear_channels( player_data* pc )
{
  if( is_set( pc->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) )
    return FALSE;

  return( pc->position != POS_EXTRACTED && ( pc->link == NULL || pc->link->connected == CON_PLAYING ) );
}


bool subtract_gsp( player_data* ch, const char* text, int cost )
{
  if( get_trust( ch ) < LEVEL_ARCHITECT ) {
    if( ch->gossip_pts < cost ) {
      send( ch, "%s requires %s gossip point%s.\r\n", text, number_word( cost ), cost == 1 ? "" : "s" );
      return FALSE;
    }
    ch->gossip_pts -= cost;
  }

  return TRUE;
}


bool can_talk( char_data* ch, const char* string )
{
  if( ch->pcdata == NULL && !is_set( ch->affected_by, AFF_SILENCE ) )
    return TRUE;
  else if( !is_set( ch->affected_by, AFF_SILENCE ) &&
    !is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) 
    return TRUE;
  
  if( string != NULL ) 
    fsend( ch, "You attempt to %s, but oddly fail to make any noise.", string );

  return FALSE;
}


/*
 *   STANDARD CHANNELS ROUTINES
 */


void do_ooc( char_data* ch, char* argument )
{
  char               buf  [ MAX_STRING_LENGTH ];
  char               tmp  [ MAX_STRING_LENGTH ];
  player_data*    pc;
  player_data*    victim;
  int         max_length;
  int             length;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You can no longer communicate with the world.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    display( ooc_history, ch, "OOC CHANNEL" );
    return;
  }

  if( toggle( ch, argument, "OOC channel", ch->pcdata->pfile->flags, PLR_OOC ) )
    return;

  if( !is_set( ch->pcdata->pfile->flags, PLR_OOC ) ) {
    send( ch, "You have OOC off.\r\n" );
    return;
  }

  if( munchkin( pc, argument ) )
    return;

  if( !subtract_gsp( pc, "Using OOC", 1 ) )
    return;
  
  sprintf( buf, "[OOC] %s:", ch->real_name( ) );
  length = strlen( buf );
  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[ length < max_length ] );

  for( int i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if( hear_channels( victim ) // hear_channels checks to see if player is valid
      && is_set( victim->pcdata->pfile->flags, PLR_OOC )
      && !victim->Filtering( ch ) ) 
      send_color( victim, COLOR_OOC, buf );
  }

  add_tell( ooc_history, ch->real_name( ), tmp );
}


void do_chant( char_data* ch, char* argument )
{
  char             buf  [ MAX_STRING_LENGTH ];
  char             tmp  [ MAX_STRING_LENGTH ];
  player_data*      pc;
  int         language;
  int           length;
  int       max_length;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( ch->pcdata->religion == REL_NONE ) {
    send( ch, "Only those with a religion can chant.\r\n" );
    return;
  }

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You can no longer communicate with the world.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    display( pc->chant, ch, "Chant" );
    return;
  }

  if( toggle( ch, argument, "Chant", ch->pcdata->pfile->flags, PLR_CHANT ) )
    return;

  if( !is_set( ch->pcdata->pfile->flags, PLR_CHANT ) ) {
    send( ch, "You have chant turned off.\r\n" );
    return;
  }

  language = ch->pcdata->speaking;
  garble_string( buf, argument, get_language( ch, language ) );
  format_tell( tmp, buf );
  max_length = 70-strlen( tmp );

  sprintf( buf, "You chant:%s%s", 7 < max_length ? "" : "\r\n", &tmp[7 < max_length] );
  send( ch, buf );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( hear_channels( pc ) // hear_channels checks player is valid
      && pc->pcdata->religion == ch->pcdata->religion ) {
      add_tell( pc->chant, ch->Seen_Name( pc ), tmp, language ); 
      if( pc != ch && is_set( pc->pcdata->pfile->flags, PLR_CHANT ) ) {
        sprintf( buf, "%s chants:", ch->Seen_Name( pc ) );
        length = strlen( buf );
        *buf = toupper( *buf );
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
        send( pc, buf );
      }
    }
  }
}


void do_chat( char_data* ch, char* argument )
{
  char             buf  [ MAX_STRING_LENGTH ];
  char             tmp  [ MAX_STRING_LENGTH ];
  player_data*  victim;
  player_data*      pc;
  int         language;
  int       max_length;
  int           length;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( pc == NULL )
    pc = player( ch->link->character );

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You can no longer communicate with the world.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    display( pc->chat, ch, "chats" );
    return;
  }
 
  if( toggle( ch, argument, "Chat channel", 
    ch->pcdata->pfile->flags, PLR_CHAT ) )
    return;

  if( !is_set( ch->pcdata->pfile->flags, PLR_CHAT ) ) {
    send( "You have chat turned off.\r\n", ch );
    return;
  }

  if( !subtract_gsp( pc, "Chatting", 1 ) )
    return;

  language = ch->pcdata->speaking;
  garble_string( buf, argument, get_language( ch, language ) );

  format_tell( tmp, buf );

  max_length = 70-strlen( tmp );

  sprintf( buf, "You chat%s:", slang( ch, language, is_apprentice(ch) ) );
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
  send_color( ch, COLOR_CHAT, buf );

  bool ch_is_darkRace = (ch->shdata->race > RACE_LIZARD);
  for( int i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if (!victim->Is_Valid())
      continue;

    bool victim_is_darkRace = (victim->shdata->race > RACE_LIZARD);
    if( hear_channels( victim ) && ch_is_darkRace == victim_is_darkRace ) {
      add_tell( victim->chat, who_name( victim, ch ), tmp, language ); 
      if( victim != ch && victim->link != NULL && 
          is_set( victim->pcdata->pfile->flags, PLR_CHAT ) && 
          !victim->Filtering( ch ) ) {
        sprintf( buf, "%s chats%s:", who_name( victim, ch ), slang( victim, language, is_apprentice(ch) ) );
        *buf = toupper( *buf ); 
        length = strlen( buf );
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
        send_color( victim, COLOR_CHAT, buf );
      }
    }
  }

  return;
}


void do_gossip( char_data* ch, char* argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  player_data*       pc;
  player_data*   victim;
  int          language;
  int        max_length;
  int            length;

/*  if( is_mob( ch ) ) 
    return;*/
  pc = player( ch );

  if( pc == NULL ) {
    if( ch->link != NULL && ch->link->player != ch )
      pc = ch->link->player;
    else {
      is_confused_pet( ch );
      return;
    }
  }

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You can no longer communicate with the world.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    display( pc->gossip, ch, "gossips" );
    return;
  }

  if( toggle( ch, argument, "Gossip channel", 
    ch->pcdata->pfile->flags, PLR_GOSSIP ) ) 
    return;

  if( !is_set( ch->pcdata->pfile->flags, PLR_GOSSIP ) ) {
    send( "You have gossip turned off.\r\n", ch );
    return;
  }

  if( munchkin( pc, argument ) )
    return;

  if( !subtract_gsp( pc, "Using gossip", 1 ) )
    return;

  language = ch->pcdata->speaking;
  garble_string( buf, argument, get_language( ch, language ) );
  format_tell( tmp, buf );

  max_length = 70-strlen( tmp );

  sprintf( buf, "You gossip%s:", slang( ch, language, is_apprentice(ch) ) );
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
  send_color( ch, COLOR_GOSSIP, buf );

  for( int i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if( hear_channels( victim ) ) { // hear channels checks player validity
      add_tell( victim->gossip, who_name( victim, ch ), tmp, language ); 
      if( victim != ch && victim->link != NULL
        && is_set( victim->pcdata->pfile->flags, PLR_GOSSIP )
        && !victim->Filtering( ch ) ) {
        sprintf( buf, "%s gossips%s:", who_name( victim, ch ), slang( victim, language, is_apprentice(ch) ) );
        *buf = toupper( *buf ); 
        length = strlen( buf );
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
        send_color( victim, COLOR_GOSSIP, buf );
      }
    }
  }
}


void do_atalk( char_data* ch, char* argument )
{
  char               buf  [ MAX_STRING_LENGTH ];
  char               tmp  [ MAX_STRING_LENGTH ];
  player_data*    pc;
  player_data*    victim;
  int         max_length;
  int             length;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You can no longer communicate with the world.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    display( atalk_history, ch, "atalk" );
    return;
  }

  if( toggle( ch, argument, "Atalk channel",
    ch->pcdata->pfile->flags, PLR_ATALK ) )
    return;

  if( !is_set( ch->pcdata->pfile->flags, PLR_ATALK ) ) {
    send( ch, "You have atalk turned off.\r\n" );
    return;
  }

  if( !subtract_gsp( pc, "Using atalk", 1 ) )
    return;
  
  sprintf( buf, "[ATALK] %s:", ch->real_name( ) );
  length = strlen( buf );
  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", 
    &tmp[ length < max_length ] );

  for( int i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if( hear_channels( victim ) // hear channels checks validity of player
      && is_set( victim->pcdata->pfile->flags, PLR_ATALK )
      && !victim->Filtering( ch ) ) 
      send_color( victim, COLOR_AUCTION, buf );
  }

  add_tell( atalk_history, ch->real_name( ), tmp ); 
}


/*
 *   IMMORTAL CHANNELS
 */


void do_avatar( char_data* ch, char* argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  player_data*   pc;
  wizard_data*   imm  = NULL;
  int            length;
  int        max_length;

  if( *argument == '\0' ) {
    imm = (wizard_data*) ch;
    display( imm->avatar, ch, "avatar channel" );
    return;
  }

  if( toggle( ch, argument, "Avatar channel", 
    ch->pcdata->pfile->flags, PLR_AVATAR ) )
    return;
        
  if( !is_set( ch->pcdata->pfile->flags, PLR_AVATAR ) ) {
    send( "You have the avatar channel turned off.\r\n", ch );
    return;
  }   

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  sprintf( buf, "[AVATAR] %s:", ch->real_name( ) );
  *buf   = toupper( *buf ); 
  length = strlen( buf );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( hear_channels( pc ) // hear channels checks validity of player
      && ( imm = wizard( pc ) ) != NULL
      && has_permission( imm, PERM_AVATAR_CHAN ) ) {
      add_tell( imm->avatar, ch->real_name( ), tmp ); 
      if( is_set( imm->pcdata->pfile->flags, PLR_AVATAR ) && !imm->Filtering( ch ) ) {
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
        send_color( imm, COLOR_AVATAR, buf );
      }
    }
  }
        
  return;
}

/* unused so commented
void do_buildchan( char_data* ch, char* argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  player_data*   pc;
  wizard_data*   imm  = NULL;
  int        max_length;
  int            length;

  if( *argument == '\0' ) {
    imm = (wizard_data*) ch;
    display( imm->build_chan, ch, "builder channel" );
    return;
  }

  if( toggle( ch, argument, "Builder channel", ch->pcdata->pfile->flags, PLR_BUILDCHAN ) )
    return;
        
  if( !is_set( ch->pcdata->pfile->flags, PLR_BUILDCHAN ) ) {
    send( ch, "You have the builder channel turned off.\r\n" );
    return;
  }   

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  sprintf( buf, "%s:", ch->descr->name );
  *buf   = toupper( *buf ); 
  length = strlen( buf );

  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", 
    &tmp[length < max_length] );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( hear_channels( pc ) // hear channels checks validity of player
      && ( imm = wizard( pc ) ) != NULL
      && has_permission( imm, PERM_BUILD_CHAN ) ) {
      add_tell( imm->build_chan, ch->descr->name, tmp, -1 ); 
      if( is_set( imm->pcdata->pfile->flags, PLR_BUILDCHAN ) )
        send_color( imm, COLOR_IMMORTAL, buf );
    }
  }
        
  return;
}
*/

void do_immtalk( char_data* ch, char* argument )
{
  char                buf  [ MAX_STRING_LENGTH ];
  char                tmp  [ MAX_STRING_LENGTH ];
  player_data*     pc;
  wizard_data*     imm  = NULL;
  int          max_length;
  int              length;

  if( *argument == '\0' ) {
    imm = (wizard_data*) ch;
    display( imm->imm_talk, ch, "immortal channel" );
    return;
  }

  if( toggle( ch, argument, "Immortal channel", ch->pcdata->pfile->flags, PLR_IMMCHAN ) )
    return;
        
  if( !is_set( ch->pcdata->pfile->flags, PLR_IMMCHAN ) ) {
    send( ch, "You have the immortal channel turned off.\r\n" );
    return;
  }   

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  sprintf( buf, "[IMM] %s:", ch->descr->name );
  *buf   = toupper( *buf ); 
  length = strlen( buf );

  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( hear_channels( pc ) // hear channels checks validity of player
      && ( imm = wizard( pc ) ) != NULL
      && has_permission( imm, PERM_IMM_CHAN ) ) {
      add_tell( imm->imm_talk, ch->descr->name, tmp ); 
      if( is_set( imm->pcdata->pfile->flags, PLR_IMMCHAN ) ) 
        send_color( imm, COLOR_IMMORTAL, buf );
    }
  }
        
  return;
}


void do_god( char_data* ch, char* argument )
{
  char                buf  [ MAX_STRING_LENGTH ];
  char                tmp  [ MAX_STRING_LENGTH ];
  player_data*     pc;
  wizard_data*     imm  = NULL;
  int          max_length;
  int              length;

  if( *argument == '\0' ) {
    imm = (wizard_data*) ch;
    display( imm->god_talk, ch, "god channel" );
    return;
  }

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  sprintf( buf, "[GOD] %s:", ch->descr->name );
  *buf   = toupper( *buf ); 
  length = strlen( buf );

  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( hear_channels( pc ) // hear channels checks validity of player
      && ( imm = wizard( pc ) ) != NULL
      && has_permission( imm, PERM_GOD_CHAN ) ) {
      add_tell( imm->god_talk, ch->descr->name, tmp ); 
      send_color( imm, COLOR_IMMORTAL, buf );
    }
  }

  return;
}


/*
 *   YELL ROUTINE
 */
 

void do_yell( char_data* ch, char* argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  char_data*        rch;
  player_data*       pc;
  int          language;
  int        max_length;
  int            length;

  if( is_confused_pet( ch ) )
    return;

  pc = player( ch );

  if( !can_talk( ch, "yell" ) )
    return;

  if( *argument == '\0' ) {
    if( pc != NULL )
      display( pc->yell, ch, "yells" );
    return;
  }

  if( ch->pcdata != NULL ) {
    language = ch->pcdata->speaking;
    garble_string( buf, argument, get_language( ch, language ) );
    format_tell( tmp, buf );
  } else {
    language = LANG_COMMON;
    format_tell( tmp, argument );
  }

  max_length = 70-strlen( tmp );

  if( ch->pcdata != NULL ) {
    sprintf( buf, "You yell%s:", slang( ch, language, is_apprentice(ch) ) );
    length = strlen( buf );
    sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
    send( ch, buf );
  }

  if( pc != NULL )
    add_tell( pc->yell, ch->descr->name, tmp, language ); 

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch ) 
      hear_yell( rch, ch, tmp, 0, 0 );

  exec_range( ch, 8, hear_yell, tmp );

  return;
}


const char* yell_name( char_data* ch, char_data* victim, int dir, int distance )
{
  char* tmp;

  if( distance == 0 ) {
    if( ch->Seen( victim ) ) 
      return ch->Seen_Name( victim );
    else
      return "someone nearby";
  }
  
  if( distance > 5 ) 
    return "someone far away";

  tmp = static_string( );
  sprintf( tmp, "someone %s", dir_table[dir].where );

  return tmp;
}


void hear_yell( char_data* victim, char_data* ch, char* message, int dir, int distance )
{
  char                 buf  [ MAX_STRING_LENGTH ];
  char                 tmp  [ MAX_STRING_LENGTH ];
  int               length;
  int           max_length;
  player_data*          pc  = player( victim );
  const char*         name;
  int             language;

  if( pc == NULL || !can_hear( victim ) ) 
    return;

  name       = yell_name( ch, victim, dir, distance );
  max_length = 70-strlen( message );
  language   = ch->pcdata != NULL ? ch->pcdata->speaking : LANG_COMMON;

  garble_string( tmp, message, get_language( victim, language ) );
  add_tell( pc->yell, name, tmp, language ); 

  sprintf( buf, "%s yells%s:", name, slang( victim, language, is_apprentice(ch) ) );
  *buf = toupper( *buf ); 
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
  send( victim, buf );

  return;
}


/*
 *   SAY ROUTINES
 */


void do_say( char_data* ch, char* argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  char              arg  [ MAX_STRING_LENGTH ];
  char_data*     victim;
  player_data*       pc  = player( ch );
  int          language;
  int            length;
  int        max_length;

  if( pc == NULL ) {
    if( ch->link != NULL && ch->link->player != ch )
      pc = ch->link->player;
    else {
      is_confused_pet( ch );
      return;
    }
  }

  if( !can_talk( ch, "speak" ) )
    return;

  if( *argument == '\0' ) {
    if( pc != NULL ) 
      display( pc->say, ch, "says" );
    return;
    }

  if( ch->pcdata != NULL ) {
    language = ch->pcdata->speaking;
    garble_string( buf, argument, get_language( ch, language ) );
    format_tell( tmp, buf );
  } else {
    language = LANG_COMMON;
    format_tell( tmp, argument );
  }

  max_length = 70-strlen( tmp );

  if( ch->pcdata != NULL ) {
    if( is_set( ch->pcdata->pfile->flags, PLR_SAY_REPEAT ) ) {
      sprintf( buf, "You %s%s:", say_verb( ch, ch ), slang( ch, language, is_apprentice(ch) ) );
      length = strlen( buf );
      sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
      send_color( ch, (is_set(ch->pcdata->pfile->flags, PLR_SAY_REP_COLOR) ? COLOR_SAYS : COLOR_DEFAULT), buf );
    } else
      send( ch, "Ok.\r\n" );

    if( pc != NULL )
      add_tell( pc->say, ch->Name( ), tmp, language ); 
  }

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( victim = character( ch->array->list[i] ) ) == NULL || ch == victim || ( pc = player( victim ) ) == NULL || !can_hear( victim ) )
      continue;
    if (language != LANG_COMMON)
      victim->improve_skill( language );
    sprintf( buf, "%s %s%s:", ch->Name( victim ), say_verb( ch, victim ), slang( victim, language, is_apprentice(ch) ) );
    *buf = toupper( *buf );
    length = strlen( buf );
    garble_string( arg, tmp, get_language( victim, language ) );
    sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &arg[length < max_length] );
    send_color( victim, COLOR_SAYS, buf );
    if( pc != NULL )
      add_tell( pc->say, ch->Name( victim ), arg, language ); 
  }

  return;
}


const char* say_verb( char_data* ch, char_data* victim )
{
  if( ch->shdata->race == RACE_LIZARD ) {
    return( ch == victim ? "hiss" : "hisses" );
  }

  return( ch == victim ? "say" : "says" );
}


/*
 *   SHOUT ROUTINE
 */


void do_shout( char_data* ch, char* argument )
{
  char             buf  [ MAX_STRING_LENGTH ];
  char             tmp  [ MAX_STRING_LENGTH ];
  player_data*  pc;
  player_data*  victim;
  int           length;
  int       max_length;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You are unable to communicate with the world.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    display( pc->shout, ch, "shouts" );
    return;
  }

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_SHOUT ) ) {
    send( ch, "You have been forbidden from shouting by the gods.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    send( "Shout what?\r\n", ch );
    return;
  }

  if( !subtract_gsp( pc, "Shouting", 500 ) )
    return;

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  sprintf( buf, "You shout:%s%s", 10 < max_length ? "" : "\r\n", &tmp[10 < max_length] );
  send( buf, ch );

  add_tell( pc->shout, ch->descr->name, tmp, -1 ); 

  for( int i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if( victim != ch && hear_channels( victim ) ) {  // hear channels checks validity of player
      add_tell( victim->shout, ch->real_name( ),tmp, -1 ); 
      sprintf( buf, "%s shouts:", ch->real_name( ) );
      *buf = toupper( *buf ); 
      length = strlen( buf );
      sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
      send( victim, buf );
    }
  }

  return;
}


/*
 *   TELL ROUTINES
 */


void tell_message( char_data* ch, char* msg, player_data* victim, int i )
{
  char tmp  [ THREE_LINES ];

  sprintf( tmp,   "++ " );  
  sprintf( tmp+3, msg, victim->real_name(), i );
  strcat( tmp, " ++\r\n\r\n" );

  tmp[3] = toupper( tmp[3] );

  send( ch, tmp );
}


void process_tell( player_data* ch, player_data* victim, char* argument )
{
  char             buf  [ 3*MAX_STRING_LENGTH ];
  char             tmp  [ 3*MAX_STRING_LENGTH ];
  int           length;
  int       max_length;

  char_data* csock = ch->switched ? ch->switched : ch->shape_changed ? ch->shape_changed : ch;
  char_data* vsock = victim->switched ? victim->switched : victim->shape_changed ? victim->shape_changed : victim;

  if( victim->Filtering( ch ) ) {
    fsend( ch, "%s is filtering you - please leave %s in peace.", victim->real_name( ), victim->Him_Her( ) );
    return; 
  }
  
  if( ch->Filtering( victim ) ) {
    fsend( ch, "You are filtering %s and only a chebucto would want to converse with someone they are filtering.", victim );
    return;
  } 

  if( vsock->link == NULL )
    tell_message( csock, "%s is link dead", victim );
  else if( victim->timer+60 < time(0) )
    tell_message( csock, "%s has been idle for %d seconds", victim, time(0) - victim->timer );
  else if( victim->array != csock->array && opponent( victim ) != NULL )
    tell_message( ch, "%s is in battle", victim );

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  if( is_set( ch->pcdata->pfile->flags, PLR_SAY_REPEAT ) ) {
    sprintf( buf, "You tell %s:", victim->real_name( ) );
    length = strlen( buf );
    sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
    send_color( csock, (is_set(ch->pcdata->pfile->flags, PLR_SAY_REP_COLOR) ? COLOR_TELLS : COLOR_DEFAULT), buf );
  } else
    send( csock, "Ok.\r\n" );

  sprintf( buf, "%s tells you:", ch->real_name( ) );
  *buf = toupper( *buf );
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );

  if( !is_set( &victim->status, STAT_REPLY_LOCK ) ) 
    victim->reply = ch;

  send_color( vsock, COLOR_TELLS, buf );
  add_tell( victim->tell, ch->real_name( ), tmp );
}


void do_tell( char_data *ch, char *argument )
{
  char             arg  [ MAX_STRING_LENGTH ];
  player_data*    victim;

  // handle switched
  player_data*        pc = player( ch );
  if( pc == NULL ) {
    if( ch->link != NULL && ch->link->player != ch )
      pc = ch->link->player;
    else {
      is_confused_pet( ch );
      return;
    }
  }

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You can no longer communicate with the world.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    if( pc != NULL )
      display( pc->tell, ch, "tells" );
    return;
  }

  argument = one_argument( argument, arg );

  if( *arg == '\0' || *argument == '\0' ) {
    send( ch, "Tell whom what?\r\n" );
    return;
  }

  in_character = FALSE;

  if( ( victim = one_player( ch, arg, "tell", (thing_array*) &player_list ) ) == NULL )
    return;

  if( pc == victim ) {
    send( ch, "Talking to yourself is pointless.\r\n" );
    return;
  }

  if( victim->Ignoring( pc ) ) {
    fsend( ch, "%s has ignore set to level %d and you cannot tell to %s.",
      victim->real_name(), level_setting( &victim->pcdata->pfile->settings, SET_IGNORE ),
      victim->Him_Her( ) );
    return;
  }

  process_tell( pc, victim, argument );
}


void do_reply( char_data* ch, char* argument )
{
  player_data* pc = player( ch );

  if( pc == NULL ) {
    if( ch->link != NULL && ch->link->player != ch )
      pc = ch->link->player;
    else {
      is_confused_pet( ch );
      return;
    }
  }

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You are unable to communicate with the world.\r\n" );
    return;
  }

  char*   noone_msg   = "No one has told to you or the person who last told to you has quit.\r\n";
  player_data* victim = pc->reply;

  in_character = FALSE;
 
  if( *argument == '\0' ) {
    if( pc->reply == NULL ) {
      send( ch, "Reply points to no one.\r\n" );
    } else {
      send( ch, "Reply %s to %s.\r\n", is_set( &pc->status, STAT_REPLY_LOCK ) ? "is locked" : "points", victim->real_name( ) );
    }
    return;
  }

  if( !strcasecmp( argument, "lock" ) ) {
    if( victim == NULL ) { 
      send( ch, noone_msg );
    } else if( is_set( &pc->status, STAT_REPLY_LOCK ) ) {
      send( ch, "Your reply is already locked to %s.\r\n", victim->real_name( ) );
    } else {
      set_bit( &pc->status, STAT_REPLY_LOCK );
      send( ch, "Reply locked to %s.\r\n", victim->real_name( ) );    
    }
    return;
  }

  if( !strcasecmp( argument, "unlock" ) ) {
    if( !is_set( &pc->status, STAT_REPLY_LOCK ) ) {
      send( ch, "Your reply is already unlocked.\r\n" );
    } else {
      send( ch, "Reply unlocked.\r\n" );
      remove_bit( &pc->status, STAT_REPLY_LOCK );
    }
    return;
  }

  if( !strcasecmp( argument, "purge" ) ) {
    send(ch, "All reply locks purged.\r\n");
    for( int i = 0; i < player_list; i++ ) {
      player_data *rch = player_list[i];
      if( rch && rch->Is_Valid() && rch->reply == pc ) {
        send( rch, "Reply pointer removed from %s.\r\n", pc );
        rch->reply = NULL;
        remove_bit( &rch->status, STAT_REPLY_LOCK );
      }
    }
    return;
  }

  if( victim == NULL ) {
    send( ch, noone_msg );
    return;
  }

  process_tell( pc, victim, argument );
}


/*
 *   WHISPER/TO
 */


void trigger_say( char_data* ch, char_data* mob, char* argument )
{
  mprog_data* mprog;
  char buf [ MAX_STRING_LENGTH ] = "\0";

  if( !ch || !ch->Is_Valid( ) || !ch->array || !mob || !mob->Is_Valid( ) || mob->array != ch->array )
    return;

  garble_string( buf, argument, get_language( ch, ch->species == NULL ? ch->pcdata->speaking : LANG_COMMON ) );

  for( mprog = mob->species->mprog; mprog != NULL; mprog = mprog->next )
    if( mprog->trigger == MPROG_TRIGGER_TELL && ( is_name( buf, mprog->string ) || *mprog->string == '\0' ) ) {
      var_ch   = ch;
      var_mob  = mob;
      var_arg  = argument;
      var_room = Room( ch->array->where );
      execute( mprog );
      return;
    }
}


void process_ic_tell( char_data* ch, char_data* victim, char* argument, char *verb )
{
  char              arg  [ MAX_STRING_LENGTH ];
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  int          language;
  player_data*       pc;
  int            length;
  int        max_length;

  /*
  fsend_seen( ch, "%s whispers something to %s.\r\n", ch, victim );
  */

  if( ch->pcdata != NULL ) {
    if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
      send( ch, "You are unable to communicate with the world.\r\n" );
      return;
    }
    language = ch->pcdata->speaking;
    garble_string( buf, argument, get_language( ch, language ) );
    format_tell( tmp, buf );
  } else {
    language = LANG_COMMON;
    format_tell( tmp, argument );
  }

  max_length = 70-strlen( tmp );

  if( ch->pcdata != NULL ) {
    if( is_set( ch->pcdata->pfile->flags, PLR_SAY_REPEAT ) ) {
      sprintf( buf, "You %s to %s%s:", verb, victim->Name( ch ), slang( ch, language, is_apprentice(ch) ) );
      length = strlen( buf );
      sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
      send_color( ch, (is_set(ch->pcdata->pfile->flags, PLR_SAY_REP_COLOR) ? COLOR_SAYS : COLOR_DEFAULT), buf );
    } else
      send( ch, "Ok.\r\n" );
  }

  if( ( pc = player( victim ) ) == NULL ) {
    trigger_say( ch, victim, argument );
    return;
  }

  if( language != LANG_COMMON )
    victim->improve_skill( language );

  sprintf( buf, "%s %ss to you%s:", ch->Name( victim ), verb, slang( victim, language, is_apprentice(ch) ) );
  buf[0] = toupper( buf[0] );
  length = strlen( buf );
  garble_string( arg, tmp, get_language( victim, language ) );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &arg[length < max_length] );

  if( ch->pcdata == NULL ) {
    convert_to_ansi( victim, buf, tmp, sizeof(tmp) );
    send( victim, tmp );
    add_tell( !str_cmp(verb, "say") ? pc->to : pc->whisper, ch->Seen_Name( pc ), arg ); 
    return;
  } 

  send_color( victim, COLOR_SAYS, buf );
  add_tell( !str_cmp(verb, "say") ? pc->to : pc->whisper, ch->Seen_Name( pc ), arg ); 
}


void ic_tell( char_data* ch, char* argument, char* verb )
{ 
  char              arg  [ MAX_STRING_LENGTH ];
  char_data*     victim;

  argument = one_argument( argument, arg );

  if( ( victim = one_character( ch, arg, verb, ch->array ) ) == NULL )
    return;
 
  if( *argument == '\0' ) {
    send( ch, "%s what to %s?\r\n", verb, victim );
    return;
  }

  if( ch == victim ) {
    send( ch, "%sing something to yourself does nothing useful.\r\n", verb );
    return;
  }

  if( !victim->is_awake( ) ) {
    send( ch, "They are not in a state to hear you.\r\n" );
    return;
  }

  process_ic_tell(ch, victim, argument, verb);
}


void do_to( char_data* ch, char* argument )
{
  player_data* pc;

  // Don't let a pet or switched mage use this command
  if( is_mob( ch ) )
    return;

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You are unable to communicate with the world.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    if( ( pc = player( ch ) ) != NULL )
      display( pc->to, ch, "To" );
    return;
    }

  ic_tell( ch, argument, "say" );
}


void do_whisper( char_data* ch, char* argument )
{  
  player_data*  pc  = player( ch );

  if( is_confused_pet( ch ) )
    return;

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You are unable to communicate with the world.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    if( pc != NULL )  
      display( pc->whisper, ch, "whispers" );
    return;
    }

  ic_tell( ch, argument, "whisper" );
}


/*
 *   EMOTE ROUTINE
 */


void do_emote( char_data *ch, char *argument )
{
  char_data*    rch;
  bool        space;

  if( ch->pcdata != NULL
    && ( is_set( ch->pcdata->pfile->flags, PLR_NO_EMOTE ) 
    || is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) ) {
    send( ch, "You can't show your emotions.\r\n" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Emote what?\r\n" );
    return;
    }

  space = strncmp( argument, "'s ", 3 ) && strncmp( argument, ", ", 2 );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL 
      || rch->position <= POS_SLEEPING )
      continue; 
    fsend( rch, "%s%s%s\r\n",
      ch->Name( rch ), space ? " " : "", argument );
    }

  return;
}


/*
 *   GROUP TELL
 */


void do_gtell( char_data* ch, char* argument )
{
  char              tmp  [ MAX_STRING_LENGTH ];
  char              buf  [ MAX_STRING_LENGTH ];
  char_data*     leader;
  player_data*       pc  = player( ch );
  int          language;
  int            length;
  int        max_length;

  if( pc == NULL ) {
    if( ch->link != NULL && ch->link->player != ch )
      pc = ch->link->player;
    else {
      is_confused_pet( ch );
      return;
    }
  }

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You are unable to communicate with the world.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    display( pc->gtell, ch, "group tells" );
    return;
  }

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_TELL ) ) {
    send( ch, "You are banned from using tell.\r\n" );
    return;
  }

  if( ( leader = group_leader( ch ) ) == NULL ) {
    send( ch, "You aren't in a group.\r\n" );
    return;
  }

  language = ch->pcdata->speaking;
  garble_string( buf, argument, get_language( ch, language ) );

  format_tell( tmp, buf );

  max_length = 70-strlen( tmp );

  sprintf( buf, "You tell your group%s:", slang( ch, language, is_apprentice(ch) ) );
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
  send_color( ch , COLOR_GTELL, buf );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( pc->position != POS_EXTRACTED && ( pc->link == NULL || pc->link->connected == CON_PLAYING ) && leader == group_leader( pc ) ) {
      add_tell( pc->gtell, ch->Seen_Name( pc ), tmp, language );
      if( pc != ch && pc->link != NULL ) {
        sprintf( buf, "%s tells the group%s:", ch->Seen_Name( pc ), slang( pc, language, is_apprentice(ch) ) );
        buf[0] = toupper( buf[0] );
        length = strlen( buf );
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
        send_color( pc, COLOR_GTELL, buf );
      }
    }
  }

  return;
}


/*
 *   CLAN TELL
 */


void perform_ctell( player_data* pc, clan_data* clan, char* argument )
{
  char               buf  [ MAX_STRING_LENGTH ];
  char               tmp  [ MAX_STRING_LENGTH ];
  int           language;
  int         max_length;
  int             length;

  if( is_set( pc->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( pc, "You are unable to communicate with the world.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    display( clan->tells, pc, clan->guild ? "guild chats" : "clan tells" );
    return;
  }

  int flag = clan->guild ? PLR_GCHAT : PLR_CTELL;

  if( toggle( pc, argument, clan->guild ? "Guild channel" : "Clan channel", pc->pcdata->pfile->flags, flag ) )
    return;

  if( !is_set( pc->pcdata->pfile->flags, flag ) ) {
    send( pc, "You have %s turned off.\r\n", clan->guild ? "gchat" : "ctell" );
    return;
  }

  language = pc->pcdata->speaking;
  garble_string( buf, argument, get_language( pc, language ) );

  format_tell( tmp, buf );

  max_length = 70-strlen( tmp );

  sprintf( buf, "You %s%s:", clan->guild ? "chat to the guild" : "tell the clan", slang( pc, language, is_apprentice( pc ) ) );
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
  send_color( pc, clan->guild ? COLOR_GUILD_CHAT : COLOR_CTELL, buf );
  add_tell( clan->tells, pc->Name( ), tmp );

  for( int i = 0; i < player_list; i++ ) {
    player_data* tpc = player_list[i];
    if( tpc->In_Game( ) && ( clan->guild && tpc->pcdata->pfile->guild == clan ) || ( !clan->guild && tpc->pcdata->pfile->clan2 == clan ) ) {
      if( tpc != pc && tpc->link != NULL && is_set( tpc->pcdata->pfile->flags, flag ) ) {
        sprintf( buf, "%s %s%s:", pc->Seen_Name( tpc ), clan->guild ? "guild-chats" : "ctells", slang( tpc, language, is_apprentice( pc ) ) );
        *buf = toupper( *buf );
        length = strlen( buf );
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\r\n", &tmp[length < max_length] );
        send_color( tpc, clan->guild ? COLOR_GUILD_CHAT : COLOR_CTELL, buf );
      }
    }
  }
}


void do_ctell( char_data* ch, char* argument )
{
  if( is_mob( ch ) )
    return;

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You are unable to communicate with the world.\r\n" );
    return;
  }

  player_data* pc = player( ch );

  if( ch->pcdata->pfile->clan2 == NULL ) {
    send( ch, "You aren't in a clan.\r\n" );
    return;
  }

  perform_ctell( pc, ch->pcdata->pfile->clan2, argument );
}


void do_gchat( char_data* ch, char* argument )
{
  if( is_mob( ch ) )
    return;

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) ) {
    send( ch, "You are unable to communicate with the world.\r\n" );
    return;
  }

  player_data* pc = player( ch );

  if( ch->pcdata->pfile->guild == NULL ) {
    send( ch, "You aren't in a guild.\r\n" );
    return;
  }

  perform_ctell( pc, ch->pcdata->pfile->guild, argument );
}


/*
 *   REVIEW COMMAND
 */


void do_review( char_data* ch, char* argument )
{
  char              arg  [ MAX_INPUT_LENGTH ];
  player_data*   victim;

  argument = one_argument( argument, arg );

  if( *arg == '\0' ) {
    send( ch, "For whom do you wish to review recent conversation?\r\n" );
    return;
  }

  if( ( victim = (player_data*) one_character( ch, arg, "review", (thing_array*) &player_list ) ) == NULL )
    return;

  if( ch == victim ) {
    send( ch, "There are simplier ways to review your own conversations.\r\n" );
    return;
  }

  if( get_trust( victim ) >= LEVEL_APPRENTICE ) { // get_trust( ch ) ) {
    send( ch, "You are unable to review %s's conversations.\r\n", victim );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "Which conversation channel do you want to review?\r\n" );
    return;
  }

  if( matches( argument, "say" ) ) {
    display( victim->say, ch, "says", victim );
    return;
  }

  if( matches( argument, "tell" ) ) {
    display( victim->tell, ch, "tells", victim );
    return;
  }

  if( matches( argument, "ctell" ) ) {
    if( !victim->pcdata->pfile->clan2 )
      send( ch, "%s isn't in a clan.\r\n", victim );
    else
      display( victim->pcdata->pfile->clan2->tells, ch, "ctells", victim );
    return;
  }

  if( matches( argument, "gchat" ) ) {
    if( !victim->pcdata->pfile->guild )
      send( ch, "%s isn't in a guild.\r\n", victim );
    else
      display( victim->pcdata->pfile->guild->tells, ch, "gchats", victim );
    return;
  }

  if( matches( argument, "gtell" ) ) {
    display( victim->gtell, ch, "gtells", victim );
    return;
  }

  send( ch, "Unknown history type - see help review.\r\n" );

  return;
}