#include "system.h"


const char* color_key = "neEKrRgGyYbBmMcCwW";
int index_ansi [] = { 1, 7, 5, 4, 30, 31, 32, 33, 34, 35, 36, 37, 40, 41, 42, 43, 44, 45, 46, 47 };

const char* color_fields [ MAX_COLOR ] = {
  "default",
  "room.name",
  "tell",
  "say",
  "gossip",
  "players",
  "mobs",
  "objects",
  "immortal",
  "avatar",
  "titles",
  "ctell",
  "chat",
  "ooc",
  "gtell",
  "auction",
  "info",
  "to.self",
  "to.group",
  "by.self",
  "by.group",  
  "mild.emphasis",
  "strong.emphasis",
  "black",
  "red1",
  "red2",
  "green1",
  "green2",
  "yellow1",
  "yellow2",
  "blue1",
  "blue2",
  "magenta1",
  "magenta2",
  "cyan1",
  "cyan2",
  "white1",
  "white2",
  "guild.chat"
};

const char* format_vt100 [] = {
  "none", "bold", "reverse", "underline" };

const int defaults_vt100 [ MAX_COLOR ] = {
  VT100_NORMAL,     VT100_BOLD,   VT100_REVERSE,  VT100_NORMAL,
  VT100_BOLD,       VT100_NORMAL, VT100_NORMAL,   VT100_NORMAL,
  VT100_NORMAL,     VT100_NORMAL, VT100_BOLD,     VT100_NORMAL,
  VT100_NORMAL,     VT100_BOLD,   VT100_NORMAL,   VT100_UNDERLINE,
  VT100_NORMAL,     VT100_BOLD,   VT100_NORMAL,   VT100_UNDERLINE,
  VT100_UNDERLINE,  VT100_BOLD,   VT100_REVERSE,  VT100_NORMAL,
  VT100_NORMAL,     VT100_NORMAL, VT100_NORMAL,   VT100_NORMAL,
  VT100_NORMAL,     VT100_NORMAL, VT100_NORMAL,   VT100_NORMAL,
  VT100_NORMAL,     VT100_NORMAL, VT100_NORMAL };

const int defaults_ansi [ MAX_COLOR ] = {
  ANSI_NORMAL,        // default
  ANSI_BOLD_RED,      // room.name
  ANSI_BOLD_WHITE,    // tell
  ANSI_WHITE,         // say
  ANSI_CYAN,          // gossip
  ANSI_NORMAL,        // players
  ANSI_NORMAL,        // mobs
  ANSI_NORMAL,        // objects
  ANSI_NORMAL,        // immortal
  ANSI_NORMAL,        // avatar
  ANSI_BOLD,          // titles
  ANSI_BOLD_CYAN,     // ctell
  ANSI_CYAN,          // chat
  ANSI_CYAN,          // ooc
  ANSI_BOLD_GREEN,    // gtell
  ANSI_CYAN,          // auction
  ANSI_CYAN,          // info
  ANSI_BOLD_RED,      // to.self
  ANSI_BOLD_YELLOW,   // to.group
  ANSI_BOLD_GREEN,    // by.self
  ANSI_BOLD_BLUE,     // by.group
  ANSI_BOLD,          // mild.emphasis
  ANSI_REVERSE,       // strong.emphasis
  ANSI_REVERSE,       // black
  ANSI_RED,           // red1
  ANSI_BOLD_RED,      // red2
  ANSI_GREEN,         // green1
  ANSI_BOLD_GREEN,    // green2
  ANSI_YELLOW,        // yellow 1
  ANSI_BOLD_YELLOW,   // yellow 2
  ANSI_BLUE,          // blue 1
  ANSI_BOLD_BLUE,     // blue 2
  ANSI_MAGENTA,       // magenta 1
  ANSI_BOLD_MAGENTA,  // magenta 2
  ANSI_CYAN,          // cyan1
  ANSI_BOLD_CYAN,     // cyan2
  ANSI_WHITE,         // white1
  ANSI_BOLD_WHITE,    // white2
  ANSI_NORMAL         // guild chat
};

/*
const int defaults_ansi [ MAX_COLOR ] = {
  ANSI_NORMAL,      ANSI_BOLD_RED,     ANSI_GREEN,      ANSI_BOLD,
  ANSI_BOLD_YELLOW, ANSI_NORMAL,       ANSI_NORMAL,     ANSI_NORMAL,
  ANSI_NORMAL,      ANSI_NORMAL,       ANSI_BOLD,       ANSI_BOLD_CYAN,
  ANSI_NORMAL,      ANSI_NORMAL,       ANSI_BOLD_GREEN, ANSI_WHITE,
  ANSI_NORMAL,      ANSI_BOLD_RED,     ANSI_GREEN,      ANSI_BLUE,  
  ANSI_YELLOW,      ANSI_BOLD,         ANSI_REVERSE,    ANSI_REVERSE, 
  ANSI_RED,         ANSI_BOLD_RED,     ANSI_GREEN,      ANSI_BOLD_GREEN,
  ANSI_YELLOW,      ANSI_BOLD_YELLOW,  ANSI_BLUE,       ANSI_BOLD_BLUE,
  ANSI_MAGENTA,     ANSI_BOLD_MAGENTA, ANSI_CYAN,       ANSI_BOLD_CYAN,
  ANSI_WHITE,       ANSI_BOLD_WHITE  };
*/

const char* format_ansi [] = {
  "bold", "reverse", "blinking", "underlined",
  "black.text", "red.text", "green.text", "yellow.text",
  "blue.text", "magenta.text", "cyan.text", "white.text",
  "black.bg", "red.bg", "green.bg", "yellow.bg", "blue.bg",
  "magenta.bg", "cyan.bg", "white.bg"
};


const int defaults_mxp [ MAX_COLOR ] = {
  ANSI_NORMAL,        // default
  ANSI_BOLD_RED,      // room.name
  ANSI_BOLD_WHITE,    // tell
  ANSI_WHITE,         // say
  ANSI_CYAN,          // gossip
  ANSI_NORMAL,        // players
  ANSI_NORMAL,        // mobs
  ANSI_NORMAL,        // objects
  ANSI_NORMAL,        // immortal
  ANSI_NORMAL,        // avatar
  ANSI_BOLD,          // titles
  ANSI_BOLD_CYAN,     // ctell
  ANSI_CYAN,          // chat
  ANSI_CYAN,          // ooc
  ANSI_BOLD_GREEN,    // gtell
  ANSI_CYAN,          // auction
  ANSI_CYAN,          // info
  ANSI_BOLD_RED,      // to.self
  ANSI_BOLD_YELLOW,   // to.group
  ANSI_BOLD_GREEN,    // by.self
  ANSI_BOLD_BLUE,     // by.group
  ANSI_BOLD,          // mild.emphasis
  ANSI_REVERSE,       // strong.emphasis
  ANSI_REVERSE,       // black
  ANSI_RED,           // red1
  ANSI_BOLD_RED,      // red2
  ANSI_GREEN,         // green1
  ANSI_BOLD_GREEN,    // green2
  ANSI_YELLOW,        // yellow 1
  ANSI_BOLD_YELLOW,   // yellow 2
  ANSI_BLUE,          // blue 1
  ANSI_BOLD_BLUE,     // blue 2
  ANSI_MAGENTA,       // magenta 1
  ANSI_BOLD_MAGENTA,  // magenta 2
  ANSI_CYAN,          // cyan1
  ANSI_BOLD_CYAN,     // cyan2
  ANSI_WHITE,         // white1
  ANSI_BOLD_WHITE,    // white2
  ANSI_NORMAL         // guild-chat
};
  

const char* format_mxp [] = {
  "bold", "reverse", "blinking", "underlined",
  "black.text", "red.text", "green.text", "yellow.text",
  "blue.text", "magenta.text", "cyan.text", "white.text",
  "black.bg", "red.bg", "green.bg", "yellow.bg", "blue.bg",
  "magenta.bg", "cyan.bg", "white.bg"
};
  
  
term_func ansi_codes;
term_func vt100_codes;
term_func mxp_codes;


const term_type term_table [] =
{
  { "dumb",                 0,  NULL,          NULL,         NULL            },
  { "vt100",                4,  format_vt100,  vt100_codes,  defaults_vt100  },
  { "ansi",                20,  format_ansi,   ansi_codes,   defaults_ansi   },
  { "mxp (experimental)",  20,  format_mxp,    mxp_codes,    defaults_mxp    }
};


/*
 *   MXP
 */

const char* mxp_initstring =
  MXP_PERM_SECURE
  "<!ELEMENT rname '<color red>' flag='roomname'>"
  "<!ELEMENT rdesc flag='roomdesc'>"
  "<!ELEMENT rexits flag='roomexit'>"
  "<!ELEMENT prompt flag='prompt'>"
  "<!ELEMENT hp flag='set hp'>"
  "<!ELEMENT maxhp flag='set maxhp'>"
  "<!ELEMENT mp flag='set mp'>"
  "<!ELEMENT maxmp flag='set maxmp'>"
  "<!ELEMENT mv flag='set mv'>"
  "<!ELEMENT maxmv flag='set maxmv'>"
  "<!ELEMENT xp flag='set xp'>"
  MXP_LOCK
;


const char* mxp_codes( int code )
{
  static char buffer[ 150 ];
  char* temp = buffer;

  strcpy( temp, MXP_RESET );
  strcat( temp, MXP_SECURE );

  switch( code ) {
  case ANSI_NORMAL:
    return MXP_RESET;

  case ANSI_BOLD:
    strcat( temp, "<high>" );
    break;

  case ANSI_REVERSE:
    strcat( temp, "<italic>" );
    break;

  case ANSI_UNDERLINE:
    strcat( temp, "<underline>" );
    break;

  case ANSI_BLACK:
    strcat( temp, "<color black>" );
    break;

  case ANSI_RED:
    strcat( temp, "<color '#AA0000'>" );
    break;

  case ANSI_GREEN:
    strcat( temp, "<color '#00AA00'>" );
    break;

  case ANSI_YELLOW:
    strcat( temp, "<color '#AAAA00'>" );
    break;

  case ANSI_BLUE:
    strcat( temp, "<color '#0000AA'>" );
    break;

  case ANSI_MAGENTA:
    strcat( temp, "<color '#AA00AA'>" );
    break;

  case ANSI_CYAN:
    strcat( temp, "<color '#00AAAA'>" );
    break;

  case ANSI_WHITE:
    strcat( temp, "<color '#AAAAAA'>" );
    break;

  case ANSI_BOLD_RED:
    strcat( temp, "<color '#FF0000'>" );
    break;

  case ANSI_BOLD_GREEN:
    strcat( temp, "<color '#00FF00'>" );
    break;

  case ANSI_BOLD_YELLOW:
    strcat( temp, "<color '#FFFF00'>" );
    break;

  case ANSI_BOLD_BLUE:
    strcat( temp, "<color '#0000FF'>" );
    break;

  case ANSI_BOLD_MAGENTA:
    strcat( temp, "<color '#FF00FF'>" );
    break;

  case ANSI_BOLD_CYAN:
    strcat( temp, "<color '#00FFFF'>" );
    break;

  case ANSI_BOLD_WHITE:
    strcat( temp, "<color '#FFFFFF'>" );
    break;

  default:
    return MXP_RESET;
  }

  return temp;
}

/*
 *   ANSI
 */


const char* ansi_colors( int code )
{
  char*   tmp  = static_string( );
  int     i,j;

  if( code == 0 ) 
    return "none";

  *tmp = '\0';

  for( i = 0; code != 0 && i < 5; i++ ) {
    for( j = 0; code%64 != index_ansi[j]; j++ )
      if( j == term_table[ TERM_ANSI ].entries - 1 ) 
        return "Impossible Index??";

    sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : " ", format_ansi[j] );
    code = code >> 6;
  }
 
  return tmp;
}


const char* ansi_codes( int code )
{
  static char   tmp  [ 150 ];
  static int   pntr;  

  pntr = (pntr + 1) % 10;
  char* str = tmp + 15*pntr;

  if ( code == 0 ) {
     strcpy( str, "\x1B[0m" );
     return str;
  }
  strcpy( str, "\x1B[" );
  int j = 0;
  char a_number[3];
  for ( int i = 0; i < 5; i++ ) {
     j = code % 64;
     if ( j == 0 )
        break;
     if ( i > 0 )
        strcat( str, ";" );
     sprintf( a_number, "%d", j );
     strcat( str, a_number );
     code >>= 6;
  }
  strcat( str, "m" );

  return str;
}


/*
 *   VT100
 */


const char* vt100_codes( int i )
{
  const char* escape_codes [] = { "\x1Bm","\x1B[1m", "\x1B[7m", "\x1B[4m" };

  return escape_codes[i];
}


/*
 *   MAIN COLOR ROUTINE
 */


void do_color( char_data* ch, char* argument )
{
  char      tmp1  [ MAX_INPUT_LENGTH ];
  char      tmp2  [ MAX_INPUT_LENGTH ];
  char      tmp3  [ MAX_INPUT_LENGTH ];
  int       i, j;
  int*     color;
  int       term;
  int       code;

  if( ch->link == NULL ) 
    return;

  color = ch->pcdata->color;
  term  = ch->pcdata->terminal;

  if( *argument == '\0' ) {
    page_title( ch, "Terminal Types" );
    int paged = 0;
    for( i = 0; i < MAX_TERM; i++ ) {
      if( i == TERM_MXP && get_trust( ch ) == 0 )
        continue;
      sprintf( tmp1, "%s (%1c)", term_table[ i ].name, term == i ? '*' : ' ' );
      sprintf( tmp2, "%%%ds%%s", 18 + strlen( tmp1 ) - ( strlen( term_table[ i ].name ) + 4 ) );
      sprintf( tmp3, tmp2, tmp1, i%4 == 3 ? "\r\n" : "" );
      page( ch, tmp3 );
      paged++;
    }
    if( paged%4 != 0 )
      page( ch, "\r\n" );
    page( ch, "\r\n" );

    if( term == TERM_DUMB )
       return;

    page( ch, "%35sLines: %d\r\n\r\n", "", ch->pcdata->lines );

    page_title( ch, "Color Options" );

    for( i = 0; i < term_table[ term ].entries; i++ ) {
      sprintf( tmp1, "%s", term_table[ term ].format[ i ] );
      sprintf( tmp2, "%%%ds%%s", 18 + strlen( tmp1 ) - strlen( term_table[ term ].format[ i ] ) );
      sprintf( tmp3, tmp2, tmp1, i%4 == 3 ? "\r\n" : "" );
      page( ch, tmp3 );
    }
    if( i%4 != 0 )
      page( ch, "\r\n" );      
    page( ch, "\r\n" );

    page_title( ch, "Color Settings" );
    for( i = 0; i < MAX_COLOR; i++ ) {
      page( ch, "%16s : %s%s%s\r\n", color_fields[i],
        color_code( ch, i ),
        term == TERM_VT100 ? term_table[ TERM_VT100 ].format[
        ch->pcdata->color[i] ] : ansi_colors( ch->pcdata->color[i] ),
        normal( ch ) );
    }

    return;
  }

  if( matches( argument, "lines" ) ) {
    if( ( i = atoi( argument ) ) < 10 || i > 500 ) {
      send( ch, "Number of screen lines must be from 10 to 500.\r\n" );
      return;
    }
    ch->pcdata->lines = i;
    setup_screen( ch );
    send( ch, "Number of lines set to %d.\r\n", i );
    return;
  }

  for( i = 0; i < MAX_TERM; i++ ) {
    if( i == TERM_MXP && get_trust( ch ) == 0 )
      continue;

    if( matches( argument, term_table[i].name ) ) {
      if( i != TERM_DUMB ) {
        for( j = 0; j < MAX_COLOR; j++ )
          color[j] = term_table[ i ].defaults[ j ];
      }
      reset_screen( ch );
      ch->pcdata->terminal = i;
      setup_screen( ch );
      send( ch, "Terminal type set to %s.\r\n", term_table[i].name );
      return;
    }
  }

  for( i = 0; i < MAX_COLOR; i++ )
    if( matches( argument, color_fields[i] ) )
      break;
 
  if( i == MAX_COLOR ) {
    send( ch, "Unknown item to color terminal type.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "To what color do you wish to set %s.\r\n", color_fields[i] );
    return;
  }

  if( !strcasecmp( argument, "none" ) ) {
    ch->pcdata->color[i] = 0;
    send( ch, "Color for %s removed.\r\n", color_fields[i] );
    return;
  }

  if( term == TERM_ANSI ) {
    for( code = 0; *argument != '\0'; ) {
      for( j = 0; !matches( argument, term_table[ term ].format[ j ] ); j++ ) {
        if( j == term_table[ term ].entries - 1 ) {
          send( ch, "Unknown ansi format.\r\n" );
          return;
        } 
      }
      code = ( code << 6 ) + index_ansi[ j ];
    }
    ch->pcdata->color[i] = code;
    send( ch, "Color for %s set to %s.\r\n", color_fields[i], term == TERM_VT100 ? term_table[ TERM_VT100 ].format[ code ] : ansi_colors( code ) );
    return;
  }
      
  for( j = 0; j < term_table[term].entries; j++ )
    if( matches( argument, term_table[term].format[j] ) )
      break;

  if( j == term_table[term].entries ) {
    send( ch, "Unknown format.\r\n" );
    return;
  }

  ch->pcdata->color[i] = j;

  send( ch, "Format of %s set to %s.\r\n", color_fields[i], term_table[term].format[j] );

  return;
}
  

/*
 *   CONVERT TEXT TO COLOR CODES
 */


void convert_to_ansi( char_data* ch, const char* input, char* output, int output_room )
{
  int          term;
  int             i;
  int  max_code_len[ MAX_TERM ] = { 0, 4, 12, 20 };

  if( ch->pcdata == NULL || !input) {
    *output = '\0';
    return;
  }

  term = ch->pcdata->terminal;

  for( ; *input != '\0' && output_room > ( 2 * max_code_len[ term ] + 2 ) ; input++ ) {
    if( *input != '@' ) {
      *output++ = *input;
      output_room--;
      continue;
    }

    input++;
  
    if( *input == '\0' ) // good work GC for forgetting this
      break;

    switch( *input ) {
    case '@' :       // @@ represents an at-sign
      *output++ = '@';
      output_room--;
      continue;

    case 'I' :       // @I represents two spaces?  should this be a tab?
      *output++ = ' ';
      *output++ = ' ';
      output_room -= 2;
      continue;
    }

    if( term == TERM_DUMB )
      continue;
 
    for( i = 0; color_key[i] != '\0'; i++ ) {
      if( color_key[i] == *input ) {
        strcpy( output, term_table[term].codes( ch->pcdata->color[ i == 0 ? COLOR_DEFAULT : COLOR_MILD + i - 1 ] ) );
        output += strlen(output);
        output_room -= strlen(output);
        break;
      }
    }
  } // end for loop over input

  if( term == TERM_DUMB ) {
    // terminate dumb terminal output here (the strcpy will terminate normal output below)
    *output = '\0';
    return;
  }

  strcpy( output, color_code( ch, COLOR_DEFAULT ) );

  return;
}

void convert_to_ansi( char_data* ch, const char* input, selt_string* output )
{
  if( ch->pcdata == NULL || !input) {
    *output = "";
    return;
  }

  int term = ch->pcdata->terminal;

  for( ; *input != '\0'; input++ ) {
    if( *input != '@' ) {
      *output += *input;
      continue;
    }

    input++;

    if( *input == '\0' ) // good work GC for forgetting this
      break;
  
    switch( *input ) {
    case '@' :       // @@ represents an at-sign
      *output += '@';
      continue;

    case 'I' :       // @I represents two spaces?  should this be a tab?
      *output += "  ";
      continue;
    }

    if( term == TERM_DUMB )
      continue;
 
    for( int i = 0; color_key[ i ] != '\0'; i++ ) {
      if( color_key[i] == *input ) {
        *output += term_table[ term ].codes( ch->pcdata->color[ i == 0 ? COLOR_DEFAULT : COLOR_MILD + i - 1 ] );
        break;
      }
    }
  } // end for loop over input

  if( term == TERM_DUMB )
    return;

  *output += color_code( ch, COLOR_DEFAULT );
}


/*
 *  WINDOW OPERATIONS
 */


void scroll_window( char_data* ch )
{
  int lines = ch->pcdata->lines;

  lock_keyboard( ch );
  save_cursor( ch );
  cursor_off( ch );
  scroll_region( ch, 1, lines-2 );
  move_cursor( ch, lines-2, 1 );
  
  return;
}


void command_line( char_data* ch )
{
  int lines = ch->pcdata->lines;

  scroll_region( ch, lines, lines );
  restore_cursor( ch );
  cursor_on( ch );
  unlock_keyboard( ch );

  return;
}


void setup_screen( char_data* ch )
{
  int lines = ch->pcdata->lines;

  reset_screen( ch );
  clear_screen( ch );

  if( ch->pcdata->terminal == TERM_MXP )
    send( ch, mxp_initstring );

  if( ch->pcdata->terminal == TERM_DUMB || !is_set( ch->pcdata->pfile->flags, PLR_STATUS_BAR ) ) 
    return;

  move_cursor( ch, lines, 1 );
  scroll_window( ch );

  return;
}


void clear_screen( char_data* ch )
{
  if( ch->pcdata->terminal != TERM_DUMB )
    send( ch, "\x1B[2J\x1B[1;1H" );
  else
    send( ch, "\r\n" );

  return;
}


void reset_screen( char_data* ch )
{
  if( ch->pcdata->terminal != TERM_DUMB ) {
    send( ch, "\x1B\x63" );
    send( ch, "\x1B[2J\x1B[1;1H" );
  }

  return;
}


/*
 *   SEND_COLOR
 */


void send_color( char_data* ch, int type, const char* msg )
{
  int   term  = ch->pcdata->terminal;
  int  color  = ch->pcdata->color[type];

  if( term != TERM_DUMB ) 
    send( ch, "%s%s%s", term_table[term].codes( color ), msg, term_table[term].codes( ch->pcdata->color[ COLOR_DEFAULT ] ) );
  else 
    send( ch, msg );

  return;
}


void page_color( char_data* ch, int type, const char* msg )
{
  int   term  = ch->pcdata->terminal;
  int  color  = ch->pcdata->color[type];

  if( term != TERM_DUMB ) 
    page( ch, "%s%s%s",
      term_table[term].codes( color ), msg,
      term_table[term].codes( ch->pcdata->color[ COLOR_DEFAULT ] ) );
  else 
    page( ch, msg );

  return;
}


/*
 *   VT100 COLOR ROUTINES
 */


const char* bold_v( char_data* ch )
{
  if( ch->pcdata == NULL )
    return empty_string;

  switch( ch->pcdata->terminal ) {
    case TERM_VT100:  return vt100_codes( VT100_BOLD );
    case TERM_ANSI:   return  ansi_codes( ANSI_BOLD*64+ANSI_WHITE );
    }

  return empty_string;
}


const char* bold_red_v( char_data* ch )
{
  if( ch->pcdata == NULL )
    return empty_string;

  switch( ch->pcdata->terminal ) {
    case TERM_VT100:  return vt100_codes( VT100_BOLD );
    case TERM_ANSI:   return  ansi_codes( ANSI_BOLD*64+ANSI_RED );
    }

  return empty_string;
}


const char* bold_cyan_v( char_data* ch )
{
  if( ch->pcdata == NULL )
    return empty_string;

  switch( ch->pcdata->terminal ) {
    case TERM_VT100:  return vt100_codes( VT100_BOLD );
    case TERM_ANSI:   return  ansi_codes( ANSI_BOLD*64+ANSI_CYAN );
    }

  return empty_string;
}


/*
 *   ANSI COLOR ROUTINES
 */


const char* normal( char_data* ch )
{
  if( ch == NULL || ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_DEFAULT ] );
}


const char* red( char_data* ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_RED ] );
}


const char* green( char_data *ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_GREEN ] );
}


const char* cyan( char_data *ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_CYAN ] );
}


const char* blue( char_data* ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_BOLD_BLUE ] );
}


const char* yellow( char_data* ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_YELLOW ] );
}


/*
 *   COLOR SCALE 
 */


const char* color_scale( char_data* ch, int i )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal != TERM_ANSI )
    return empty_string;

  i = range( 0, i, 6 );

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_WHITE-2*i ] );
}


/*
 *   BATTLE COLOR FUNCTIONS
 */


const char* damage_color( char_data* rch, char_data* ch, char_data* victim )
{
  int term;

  if( rch->pcdata == NULL || ( term = rch->pcdata->terminal ) == TERM_DUMB )
    return empty_string;

  if( rch == victim )
    return term_table[term].codes( rch->pcdata->color[ COLOR_TO_SELF ] );

  if( is_same_group( rch, victim ) )
    return term_table[term].codes( rch->pcdata->color[ COLOR_TO_GROUP ] );

  if( rch == ch )
    return term_table[term].codes( rch->pcdata->color[ COLOR_BY_SELF ] );

  if( is_same_group( rch, ch ) )
    return term_table[term].codes( rch->pcdata->color[ COLOR_BY_GROUP ] );
 
  return empty_string;
}


const char* by_self( char_data* ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_BY_SELF ] );
}


const char* to_self( char_data* ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_TO_SELF ] );
}















