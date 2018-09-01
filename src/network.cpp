#include "system.h"

socket_t socket_one = 0;

int bytes_written = 0;
int gigs_written = 0;
int max_players = 0;
int record_players = 0;

bool          wizlock  = FALSE;    



link_array link_list; 

bool   process_output       ( link_data* );
bool   read_link            ( link_data* );
void   stop_idling          ( char_data* );

const    char    echo_off_str    [] = { char(IAC), char(WILL), char(TELOPT_ECHO), '\0' };
const    char    echo_on_str     [] = { char(IAC), char(WONT), char(TELOPT_ECHO), '\0' };

/*
 *   SOCKET ROUTINES
 */

#ifdef MYSTERIA_WINDOWS

//////////////////////////////////////////////////////////////////////////////
// WINDOWS SOCKET ROUTINES
void nonblock(socket_t s)
{
  unsigned long val = 1;
  ioctlsocket(s, FIONBIO, &val);
}


void open_link(int s)
{
  socket_t desc;
  socklen_t i;
  sockaddr_in peer;
  link_data *link;

  // accept the new connection
  i = sizeof(peer);
  if ((desc = accept(s, (sockaddr *) &peer, &i)) == INVALID_SOCKET) {
    log("SYSERR: accept");
    return;
  }

  // keep it from blocking
  nonblock(desc);

  // set the send buffer size
  int opt = MAX_SOCK_BUF;
  if (setsockopt(desc, SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof(opt)) < 0) {
    CLOSE_SOCKET(desc);
    return;
  }

  link             = new link_data;
  link->channel    = desc;
  link->connected  = CON_INTRO;

  write_host( link, (char *) &peer.sin_addr); 
}


struct in_addr *get_bind_addr()
{
  static struct in_addr bind_addr;

  /* Clear the structure */
  memset((char *) &bind_addr, 0, sizeof(bind_addr));

  bind_addr.s_addr = htonl(INADDR_ANY);
  log("Binding to all IP interfaces on this host.\r\n");

  return &bind_addr;
}


int open_port( int portnum )
{
  int s, opt;
  sockaddr_in sa;
  WORD wVersionRequested;
  WSADATA wsaData;

  wVersionRequested = MAKEWORD(1, 1);

  if (WSAStartup(wVersionRequested, &wsaData) != 0) {
    log("SYSERR: WinSock not available!");
    throw(1);
  }

  if ((s = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    log("SYSERR: Error opening network connection: Winsock error #%d", WSAGetLastError());
    throw(1);
  }

  opt = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0){
    log("SYSERR: setsockopt REUSEADDR");
    throw(1);
  }

  opt = MAX_SOCK_BUF;
  if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof(opt)) < 0) {
    log("SYSERR: setsockopt SNDBUF");
    return (-1);
  }

  {
    linger ld;

    ld.l_onoff = 0;
    ld.l_linger = 0;
    if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld)) < 0)
      log("SYSERR: setsockopt SO_LINGER");  /* Not fatal I suppose. */
  }

  memset((char *)&sa, 0, sizeof(sa));

  sa.sin_family = AF_INET;
  sa.sin_port = htons(portnum);
  sa.sin_addr = *(get_bind_addr());

  if (bind(s, (sockaddr *) &sa, sizeof(sa)) < 0) {
    log("SYSERR: bind");
    CLOSE_SOCKET(s);
    throw(1);
  }

  nonblock(s);
  listen(s, 5);

  return s;


  /*
  struct sockaddr_in         server;
  struct linger         sock_linger;
  struct hostent*              host;
  char*                    hostname  = static_string( );
  int                          sock;
  int                             i  = 1;
  int                            sz  = sizeof( int ); 

  WORD wVersionRequested;
  WSADATA wsaData;

  wVersionRequested = MAKEWORD(1, 1);

  if (WSAStartup(wVersionRequested, &wsaData) != 0)
    panic("SYSERR: WinSock not available!");

  sock_linger.l_onoff  = 1;
  sock_linger.l_linger = 0;

  if( gethostname( hostname, THREE_LINES ) != 0 ) 
    panic( "Open_Port: Gethostname failed." );

  log( "Open_Port: gethostname: %s.\r\n", hostname );

  if( ( host = gethostbyname( hostname ) ) == NULL )
    panic( "Open_Port: Error in gethostbyname." );

  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    panic("SYSERR: Error opening network connection: Winsock error #%d", WSAGetLastError());

  if( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sz ) < 0 ) 
    panic( "Open_port: error in setsockopt (SO_REUSEADDR)" );

  if( setsockopt( sock, SOL_SOCKET, SO_LINGER, (char*) &sock_linger, sizeof( sock_linger ) ) < 0 ) 
    panic( "Open_port: error in setsockopt (SO_LINGER)" );

  unsigned long val = 1;
  if (ioctlsocket(sock, FIONBIO, &val) != 0)
    panic ("Open_port: Error setting nonblocking.");

  memset( &server, 0, sizeof( struct sockaddr_in ) );

  server.sin_family      = AF_INET;;
  server.sin_port        = htons( portnum );
  server.sin_addr        = *get_bind_addr();

  if( bind( sock, (struct sockaddr*) ( &server ), sizeof( server ) ) ) 
    panic( "Open_port: Error binding port %d at %s.", portnum, host->h_name );

  if( listen( sock, 3 ) ) 
    panic( "Open_port: error in listen" );

  log( "Binding port %d at %s.\r\n", portnum, host->h_name );

  return sock;
  */
}





//////////////////////////////////////////////////////////////////////////////
// UNIX SOCKET ROUTINES
#elif defined MYSTERIA_UNIX
  
#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

void nonblock(socket_t s)
{
  int flags;

  flags = fcntl(s, F_GETFL, 0);
  flags |= O_NONBLOCK;
  if (fcntl(s, F_SETFL, flags) < 0) {
    log("SYSERR: Fatal error executing nonblock (network.cpp)");
    throw(1);
  }
}


void open_link( int port )
{
  link_data*                  link;
  struct sockaddr_in      net_addr;
  unsigned int             addrlen;
  int                      fd_conn;

  addrlen = sizeof( net_addr );
  fd_conn = accept( port, (struct sockaddr *) &net_addr, &addrlen );

  if( fd_conn < 0 )
    return;

  nonblock(fd_conn);

  link             = new link_data;
  link->channel    = fd_conn;
  link->connected  = CON_INTRO;

  write_host( link, (char*) &net_addr.sin_addr ); 
}


int open_port( int portnum )
{
  struct sockaddr_in         server;
  struct linger         sock_linger;
//  struct hostent*              host;
//  char*                    hostname  = static_string( );
  int                          sock;
  int                             i  = 1;
  int                            sz  = sizeof( int ); 

  sock_linger.l_onoff  = 0;  // was 1
  sock_linger.l_linger = 0;

/*
  if( gethostname( hostname, THREE_LINES ) != 0 ) 
    panic( "Open_Port: Gethostname failed." );

  if( ( host = gethostbyname( rstrncasecmp( hostname, ".com", 4 )
    ? hostname : "forestheart.com" ) ) == NULL )
    panic( "Open_Port: Error in gethostbyname." );
*/

  if( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) 
    panic( "Open_port: error in socket call" );

  if( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sz ) < 0 ) 
    panic( "Open_port: error in setsockopt (SO_REUSEADDR)" );

  if( setsockopt( sock, SOL_SOCKET, SO_LINGER, (char*) &sock_linger, sizeof( sock_linger ) ) < 0 ) 
    panic( "Open_port: error in setsockopt (SO_LINGER)" );

  if( fcntl( sock, F_SETFD, 1 ) == -1 )
    panic( "Open_port: Error setting close on Exec." );

  memset( &server, 0, sizeof( struct sockaddr_in ) );

  server.sin_family      = AF_INET;;
  server.sin_port        = htons( portnum );

//  memcpy( &server.sin_addr, host->h_addr_list[0], host->h_length );

  if( bind( sock, (struct sockaddr*) ( &server ), sizeof( server ) ) ) 
    panic( "Open_port: Error %s binding port %d.",
      (errno == EINVAL ? "(already bound)" : (errno == EACCES ? "(no permission)" : "(unknown)")), portnum); //host->h_name );

  if( listen( sock, 5 ) ) 
    panic( "Open_port: error in listen" );

  log( "Binding port %d at %s.\r\n", portnum, "unknown" ); // host->h_name );

  return sock;
}

#else
#error Write appropriate socket routines for your OS
#endif



void update_links( void )
{
  char_data*            ch;
  text_data*       receive;
  fd_set          read_set;
  fd_set         write_set;
  fd_set          exec_set;
  struct timeval     start;  
  struct timeval   timeout;  

  gettimeofday( &start, NULL );

  timeout.tv_sec  = 1;
  timeout.tv_usec = 0;

  FD_ZERO( &read_set );
  FD_ZERO( &write_set );
  FD_ZERO( &exec_set );

  FD_SET( socket_one, &read_set );

  link_nav links( &link_list );
  for( link_data* link = links.first(); link != NULL; link = links.next() ) {
    FD_SET( link->channel, &read_set  );
    FD_SET( link->channel, &write_set );
    FD_SET( link->channel, &exec_set );
  }

  if( (int) select( FD_SETSIZE, &read_set, &write_set, &exec_set, &timeout ) < 0 ) 
    panic( "Update links: select" );

  if( FD_ISSET( socket_one, &read_set ) )
    open_link( socket_one );

  for( link_data* link = links.first(); link != NULL; link = links.next() ) {
    if( FD_ISSET( link->channel, &exec_set ) ) {
      // exception state
      FD_CLR(link->channel, &read_set);
      FD_CLR(link->channel, &write_set);
      write( link->player ); // save char
      close_socket( link );  // disconnect
      continue;
    }
  
    if( FD_ISSET( link->channel, &read_set ) ) {
      link->idle = 0;
      if( link->player != NULL )
        link->player->timer = time(0);
      if( !read_link( link ) ) {
        // read failed, uh oh
        write( link->player ); // save char
        close_socket( link );  // disconnect
        continue;
      }
    }

    // disconnect idle sockets after 10 minutes
    if( link->idle++ > 600 * PULSE_PER_SECOND && link->connected != CON_PLAYING ) {
      send( link, "\r\n\r\n-- CONNECTION TIMEOUT --\r\n" );
      close_socket( link, TRUE );
    }
  }

  pulse_time[ TIME_READ_INPUT ] = stop_clock( start );  
  gettimeofday( &start, NULL );

  for( link_data* link = links.first(); link != NULL; link = links.next() ) {
    if( link->command = ( ( receive = link->receive ) != NULL ) ) {
      ampersand( receive );
      link->receive  = receive->next;
      link->idle     = 0; 
      if( link->connected == CON_PLAYING ) {
        stop_idling( ch = link->character );
        interpret( link->character, receive->message.text );
      } else
        nanny( link, receive->message.text );
      delete receive;
    }
  }

  pulse_time[ TIME_COMMANDS ] = stop_clock( start );  
  gettimeofday( &start, NULL );

  // send queued output
  for( link_data* link = links.first(); link != NULL; link = links.next() ) {
    if( link->idle % 25 == 0 && FD_ISSET( link->channel, &write_set ) && !process_output( link ) ) {
      write( link->player );
      close_socket( link );
    }
  }  
  
  pulse_time[ TIME_WRITE_OUTPUT ] = stop_clock( start );  

  return;
}


/*
 *   CLOSING OF SOCKETS
 */



void close_port( void )
{
  link_nav links( &link_list );
  link_nav hosts( &host_stack );
  
  for( link_data *link = links.first(); link != NULL; link = links.next() )
    close_socket(link, true);

  for( link_data *link = hosts.first(); link != NULL; link = hosts.next() )
    delete link;

  CLOSE_SOCKET(socket_one);
}
 
 
 
void extract( link_data* prev )
{
  link_nav links( &link_list );
  link_nav hosts( &host_stack );
  
  // unsnoop anyone they were snooping
  for( link_data* link = links.first(); link != NULL; link = links.next() )
    if( link->snoop_by == prev )
      link->snoop_by = NULL;

  // unsnoop the snooper
  send( prev->snoop_by, "Your victim has left the game.\r\n" );

  // remove it from the list
  link_list.remove( prev );

  // remove any account if necessary
  if( prev->account != NULL && prev->account->last_login == -1  ) 
    extract( prev->account );

  // update host stack
  for( link_data *link = hosts.first(); link; link = hosts.next() ) {
    if( link == prev ) {
      prev->hostresolved = -1;
      return;
    }
  }

  delete prev;
}


void close_socket( link_data* link, bool process )
{
  char              buf [ MAX_STRING_LENGTH ];
  char_data*         ch;
  int         connected  = link->connected;
  player_data*       pc;

  if( link->channel == -1 ) {
    bug( "Close_Socket: Closing a dead socket??" );
    return;
  }

  if( ( ch = link->player ) != NULL && ch != link->character ) {
    if( ( pc = player( ch ) ) != NULL )
      do_return( link->character, "" );
  }

  if( process ) {
    link->connected = CON_CLOSING_LINK;
    process_output( link );
  }

  if( ch != NULL ) {
    if( connected == CON_PLAYING ) {
      send_seen( ch, "%s has lost %s link.\r\n", ch, ch->His_Her( ) );
      sprintf( buf, "%s has lost link.", ch->descr->name );
      info( buf, LEVEL_IMMORTAL, buf, IFLAG_LOGINS, 1, ch );
      ch->link = NULL;
    } else {
      if( ch->shdata->level == 0 && ch->pcdata->pfile != NULL ) 
        extract( ch->pcdata->pfile, link );
      ch->Extract( );
    }  
  }

  shutdown( link->channel, SD_BOTH );
  CLOSE_SOCKET( link->channel );
  extract( link );
}


/*
 *   INPUT ROUTINES
 */


bool erase_command_line( char_data* ch )
{
  char* tmp  = static_string( );

  if( ch == NULL || ch->pcdata->terminal == TERM_DUMB  
    || ch->link->connected != CON_PLAYING
    || !is_set( ch->pcdata->pfile->flags, PLR_STATUS_BAR ) )
    return TRUE;

  sprintf( tmp, "\x1B[%d;1H\x1B[J", ch->pcdata->lines );

  if( int( socket_write( ch->link->channel, tmp, strlen( tmp ) ) ) == -1 )
    return FALSE; 

  return TRUE;
}


bool read_link( link_data* link )
{
  char            buf  [ 2*MAX_INPUT_LENGTH+100 ];
  text_data*  receive;
  int          length;
  int           nRead;
  char*         input;
  char*        output;

  strcpy( buf, link->rec_pending );

  length  = strlen( buf );
  nRead   = socket_read( link->channel, buf+length, 100 );

  if( nRead <= 0 )
    return FALSE;

  free_string( link->rec_pending, MEM_LINK );
  link->rec_pending = empty_string;

  buf[ length+nRead ] = '\0';

  if( length+nRead > MAX_INPUT_LENGTH-2 ) {
    if( link->connected != CON_PLAYING )
      return FALSE;
    send( link->character, "!! Truncating input !!\r\n" );
    sprintf( buf+MAX_INPUT_LENGTH-3, "\r\n" );
  }

  for( input = output = buf; *input != '\0'; input++ ) {
    if( *input != '\r' && *input != '\n' ) {
      if (*input == '\b' || *input == 127) {
        // handle backspace :P
        if (output > buf)
          *output-- = '\0';
      }
      if (*input >= 0x20 && *input <= 0x7E)
        *output++ = ( *input == '~' ? '-' : *input );
      continue;
    }

    if ((*input == '\r' || *input == '\n') && (*(input+1) == '\r' || *(input+1) == '\n'))
      input++;

    // strip trailing spaces
    for( ; --output >= buf && *output == ' '; );
    *(++output) = '\0';

    if( link->connected != CON_PLAYING )  
      receive = new text_data( buf );
    else if( *buf == '!' )
      receive = new text_data( link->rec_prev );
    else {
      receive = new text_data( subst_alias( link, buf ) );
      free_string( link->rec_prev, MEM_LINK );
      link->rec_prev = alloc_string( receive->message.text, MEM_LINK );
    }
    append( link->receive, receive );
    output = buf;

    if( !erase_command_line( link->character ) )
      return FALSE;
  }

  *output = '\0';
  link->rec_pending = alloc_string( buf, MEM_LINK ); 

  return TRUE;
}


/*
 *   OUTPUT ROUTINES
 */


bool process_output( link_data* link )
{
  text_data*      output;
  text_data*        next;
  char_data*          ch  = link->character;
  bool        status_bar;

  if( link->connected == CON_PLAYING && ch == NULL ) {
    bug( "Process_Output: Link playing with null character." );
    bug( "--     Host = '%s'", link->host );
    bug( "-- Rec_Prev = '%s'", link->rec_prev );
    return FALSE;
  }

  status_bar = ( link->connected == CON_PLAYING
    && is_set( ch->pcdata->pfile->flags, PLR_STATUS_BAR )
    && ch->pcdata->terminal != TERM_DUMB );

  if( link->send == NULL && !link->command )
    return TRUE;

  /* SAVE CURSOR */

  if( status_bar ) {
    next       = link->send;
    link->send = NULL;
    scroll_window( ch );
    if( next != NULL )
      send( ch, "\r\n" );
    cat( link->send, next );
    prompt_ansi( link );
    command_line( ch );
  } else {
    if( !link->command ) {
      next       = link->send;
      link->send = NULL;
      send( ch, "\r\n" );
      cat( link->send, next );
    }  
    if( link->connected == CON_PLAYING && link->receive == NULL ) 
      prompt_nml( link );
  }
 
  /* SEND OUTPUT */

  for( ; ( output = link->send ) != NULL; ) {
    if( socket_write( link->channel, output->message.text, output->message.length ) == -1 )
      return FALSE; 
    link->send = output->next;
    delete output;
  }

  return TRUE;
}


/*
 *   LOGIN ROUTINES
 */


typedef void login_func( link_data*, char* );


struct login_handle
{
  login_func*  function;
  int          state;
};


void nanny( link_data* link, char* argument )
{
  login_handle nanny_list [] = {
    { nanny_intro,              CON_INTRO              },
    { nanny_acnt_name,          CON_ACNT_NAME          },
    { nanny_acnt_password,      CON_ACNT_PWD           },
    { nanny_acnt_email,         CON_ACNT_EMAIL         },
    { nanny_acnt_enter,         CON_ACNT_ENTER         },
    { nanny_acnt_confirm,       CON_ACNT_CONFIRM       },
    { nanny_acnt_check,         CON_ACNT_CHECK         },
    { nanny_acnt_check_pwd,     CON_ACNT_CHECK_PWD     },
    { nanny_old_password,       CON_PASSWORD_ECHO      },
    { nanny_old_password,       CON_PASSWORD_NOECHO    },
    { nanny_about,              CON_READ_ABOUT         },
    { nanny_motd,               CON_READ_MOTD          },
    { nanny_imotd,              CON_READ_IMOTD         },
    { nanny_new_name,           CON_GET_NEW_NAME       },
    { nanny_acnt_request,       CON_ACNT_REQUEST       },
    { nanny_acnt_menu,          CON_ACNT_MENU          },
    { nanny_confirm_password,   CON_CONFIRM_PASSWORD   },
    { nanny_set_term,           CON_SET_TERM           },
    { nanny_show_rules,         CON_READ_GAME_RULES    },
    { nanny_agree_rules,        CON_AGREE_GAME_RULES   },
    { nanny_alignment,          CON_GET_NEW_ALIGNMENT  },
    { nanny_help_alignment,     CON_HELP_ALIGNMENT     },
    { nanny_disc_old,           CON_DISC_OLD           },
    { nanny_help_class,         CON_HELP_CLSS          },
    { nanny_class,              CON_GET_NEW_CLSS       },
    { nanny_help_race,          CON_HELP_RACE          },
    { nanny_race,               CON_GET_NEW_RACE       },
    { nanny_stats,              CON_DECIDE_STATS       },  
    { nanny_help_sex,           CON_HELP_SEX           },
    { nanny_sex,                CON_GET_NEW_SEX        },   
    { nanny_new_password,       CON_GET_NEW_PASSWORD   },
    { nanny_acnt_enter,         CON_CE_ACCOUNT         },
    { nanny_acnt_check_pwd,     CON_CE_PASSWORD        },
    { nanny_acnt_email,         CON_CE_EMAIL           },
    { nanny_acnt_enter,         CON_VE_ACCOUNT         },
    { nanny_ve_validate,        CON_VE_VALIDATE        },
    { nanny_acnt_confirm,       CON_VE_CONFIRM         },
    { NULL,                     -1                     }
  };

  skip_spaces( argument );

  for( int i = 0; nanny_list[i].function != NULL; i++ ) {
    if( link->connected == nanny_list[i].state ) {
      nanny_list[i].function( link, argument );
      return;
    }
  }

  if( link->connected == CON_PAGE ) {
    write_greeting( link );
    link->connected = CON_INTRO;
    return;
  }

  if( link->connected == CON_FEATURES ) {
    help_link( link, "Features_2" );
    link->connected = CON_PAGE;
    return;
  }  

  if( link->connected == CON_POLICIES ) {
    help_link( link, "Policy_2" );
    link->connected = CON_POLICIES2;
    return;
  }
  
  if( link->connected == CON_POLICIES2 ) {
  help_link( link, "Policy_3" );
  link->connected = CON_PAGE;
  return;
  }

  bug( "Nanny: bad link->connected %d.", link->connected );
  close_socket( link );

  return;
}


void stop_idling( char_data* ch )
{
  if( ch == NULL
    || ch->link == NULL
    || ch->link->connected != CON_PLAYING
    || ch->was_in_room == NULL )
    return;

  ch->timer = time(0);
  if( ch->array != NULL )
    ch->From( );
  ch->To( ch->was_in_room );
  ch->was_in_room = NULL;

  send_seen( ch, "%s has returned from the void.\r\n", ch );

  return;
}


void write_greeting( link_data* link )
{
  help_link( link, "greeting" );
  send( link, "                   Choice: " );

  return;
}



#ifdef MYSTERIA_WINDOWS

ssize_t socket_write(socket_t desc, const char *txt, size_t length)
{
  ssize_t result;

  result = send(desc, txt, length, 0);

  if (result >= 0) {
    // Write was sucessful
    bytes_written += result;
    if (bytes_written > 1073741824) {
      gigs_written++;
      bytes_written -= 1073741824;
    }
    return result;
  }

  if (result == 0) {
    // This should never happen!
    log("Error: socket_write returned 0!");
    return -1;
  }

  // if result < 0, an error has occurred
  if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
    return 0;  // only blocking, no worries

  // fatal error!
  return -1;
}

#else

ssize_t socket_write(socket_t desc, const char *txt, size_t length)
{
  ssize_t result;

  result = write(desc, txt, length);

  if (result >= 0) {
    // Write was sucessful
    bytes_written += result;
    if (bytes_written > 1073741824) {
      gigs_written++;
      bytes_written -= 1073741824;
    }
    return (result);
  }

  if (result == 0) {
    // This should never happen!
    log("Error: socket_write returned 0!");
    return -1;
  }

  // check blocking errors
#ifdef EAGAIN    /* POSIX */
  if (errno == EAGAIN)
    return (0);
#endif

#ifdef EWOULDBLOCK  /* BSD */
  if (errno == EWOULDBLOCK)
    return (0);
#endif

  // fatal error, drop out
  return -1;
}

#endif
   

ssize_t socket_read(socket_t desc, char *read_point, size_t space_left)
{
  ssize_t ret;

#ifdef MYSTERIA_WINDOWS
  ret = recv(desc, read_point, space_left, 0);
#else
  ret = read(desc, read_point, space_left);
#endif

  if (ret > 0)
    return ret; // successful read
  
  if (ret == 0) { // they disconnected
//    log("EOF on socket read (connection broken by peer)");
    return -1;
  }

  // otherwise fatal error
  
#if defined(CIRCLE_WINDOWS)  /* Windows */
  if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
    return (0);
#else
  #ifdef EINTR    /* Interrupted system call - various platforms */
    if (errno == EINTR)
      return (0);
  #endif

  #ifdef EAGAIN    /* POSIX */
    if (errno == EAGAIN)
      return (0);
  #endif

  #ifdef EWOULDBLOCK  /* BSD */
    if (errno == EWOULDBLOCK)
      return (0);
  #endif /* EWOULDBLOCK */
#endif

  return 0;
}
