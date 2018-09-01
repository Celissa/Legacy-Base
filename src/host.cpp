#include "system.h"

link_array host_stack;

pid_t       daemon_pid  = -1;
char       read_buffer  [ FOUR_LINES ];
int          read_pntr  = 0;
int              input  = -1;
int             output  = -1;
int        host_ignore  = 0;

void  connect_link   ( link_data*, const char* );


void broken_pipe( )
{
  roach( "Write_Host: Pipe to host daemon is broken." );
  roach( "Write_Host: Attempting to revive daemon." );
  init_daemon( );
  return;
}


bool init_daemon( )
{

#ifdef MYSTERIA_UNIX

  char       tmp  [ ONE_LINE ];
  int     pipe_a  [ 2 ];
  int     pipe_b  [ 2 ];

  kill_daemon();

  log( "Init_Daemon: Waking Daemon...\r\n" );

  pipe( pipe_a );
  pipe( pipe_b );  

  if( ( daemon_pid = fork( ) ) == (pid_t) 0 ) { 
    dup( pipe_a[0] );
    dup( pipe_b[1] );
    close( pipe_a[1] );
    close( pipe_b[0] );

    log( "Init_Daemon: Pipes are %d and %d.\r\n", pipe_a[0], pipe_b[1] );

    sprintf( &tmp[0], "%d", pipe_a[0] );
    sprintf( &tmp[9], "%d", pipe_b[1] );

    if( execlp( "./daemon", "./daemon", &tmp[0], &tmp[9], (char*) 0 ) == -1 )
      log( "Init Daemon: Error in Execlp.\r\n" ); 
     
    throw( 1 );
  } else if( daemon_pid == -1 ) {
    return FALSE;
  }

  close( pipe_a[0] );
  close( pipe_b[1] );

  input  = pipe_b[0];
  output = pipe_a[1];

  /*
  fcntl( input,  F_SETFL, O_NDELAY );
  fcntl( output, F_SETFL, O_NDELAY );
  */

  /* CLOSE PIPE ON EXEC */

  fcntl( input,  F_SETFD, 1 );
  fcntl( output, F_SETFD, 1 );
 
#endif

  return TRUE;
}


int players_on( )
{
  int          num  = 0;

  link_nav links( &link_list );
  for( link_data* link = links.first(); link != NULL; link = links.next() )
    if( link->connected == CON_PLAYING && !is_set( link->pfile->flags, PLR_WIZINVIS ) )
      num++;

  return num;
}


void write_host( link_data* link, char* name )
{
  char*  tmp1  = static_string( );
  char*  tmp2  = static_string( );
  int    addr;  

  sprintf_minutes( tmp1, time(0) - boot_time );
  sprintf( tmp2, "%s\r\n%d players on.\r\nSystem started %s ago.\r\nGetting site info ...\r\n", MXP_LOCK, players_on( ), tmp1 );

  socket_write( link->channel, tmp2, strlen( tmp2 ) );
  link->hostresolved = 1;  // IP is authorized unless we're doing a lookup
 
#ifdef MYSTERIA_WINDOWS

//  link->hostbuf = new char[MAXGETHOSTSTRUCT];
//  link->hosthandle = WSAAsyncGetHostByAddr(MainWindow, WM_HOSTNAME, name, sizeof(struct in_addr), AF_INET, link->hostbuf, MAXGETHOSTSTRUCT);

#elif defined MYSTERIA_UNIX

  if( host_stack.size > 20 ) {
    beetle( "Write_Host: Host daemon is swamped." );
  } else if( write( output, name, sizeof( struct in_addr ) ) == -1 ) {
    log( "TFE: Error writing to daemon.\r\n");
    perror("write");
    broken_pipe( );
  } else {
    host_stack.add( link );
    link->hostresolved = 0;  // unauthorize IP pending result of lookup
  }

#endif

  memcpy( &addr, name, sizeof( int ) );
  addr = ntohl( addr );

  sprintf( tmp1, "%d.%d.%d.%d", ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF, ( addr >>  8 ) & 0xFF, addr & 0xFF );

  connect_link( link, tmp1 );
  return;
}
 

void read_host( )
{ 
#ifdef MYSTERIA_UNIX
  struct timeval  start;  
  int              i, j;
  int             nRead;

  gettimeofday( &start, NULL );

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(input, &read_fds);
  int selectStatus = select(64, &read_fds, 0, 0, &timeout);
  if ( selectStatus == -1 ) {
    log( "TFE: Error reading from daemon.\r\n");
    perror("read");
    return;
  } else if ( selectStatus == 0 ) {
    return;
  } else { // input received
    nRead = read( input, &read_buffer[read_pntr], FOUR_LINES-read_pntr );
    if( nRead == -1 ) {
      log( "TFE: Error reading from daemon.\r\n");
      perror("read");
      broken_pipe( );
      return;
    } 
    read_pntr += nRead;
  }

  do {
    for( i = 0; i < read_pntr; i++ )
      if( read_buffer[i] == '\0' ) // search for a null
        break;
     
    // If there isn't a complete hostname string in the buffer, give up.
    if( i == read_pntr )
      return;

    // i is pointing to the end of a string now
    if( strcmp( read_buffer, "Alive?" ) && !host_stack.empty ) {
      link_data* slink = host_stack.headitem();
      host_stack.remove( slink );

      // read_buffer is a valid hostname string or IP address string
      // assumes that IP address in host_stack matches what we just read
      if( slink->hostresolved == -1 ) {
        // link was extracted before host was resolved
        delete slink;

      } else {
        link_nav links( &link_list );
        for( link_data* link = links.first(); link; link = links.next() ) {
          if( link == slink ) {
            char tmp [ TWO_LINES ];
            sprintf( tmp, "IP resolved: %s -> %s", link->host, read_buffer );
            info( "", LEVEL_ANGEL, tmp, IFLAG_LOGINS, 3 );
            free_string( link->host, MEM_LINK );
            link->host = alloc_string( read_buffer, MEM_LINK );
            link->hostresolved = 1;
            break;
          }
        }
      }
    }
    
    // copy remainder of the buffer i characters backwards
    for( j = i+1; j < read_pntr; j++ )
      read_buffer[j-i-1] = read_buffer[j];

    read_pntr -= i+1;
  } while ( read_pntr > 0 );

  pulse_time[ TIME_DAEMON ] = stop_clock( start );  
#endif
}


void connect_link( link_data* link, const char* host )
{  
  char  tmp  [ TWO_LINES ];

  link->host = alloc_string( host, MEM_LINK );

  link_list.add( link );
  write_greeting( link );

  sprintf( tmp, "Connection from %s", link->host );
  info( "", LEVEL_ANGEL, tmp, IFLAG_LOGINS, 2 );

  return;
}


void kill_daemon()
{
#ifdef MYSTERIA_UNIX

  if ( daemon_pid > 0 ) {
    log( "Kill_Daemon: Killing hostname daemon...\r\n");
    if ( input != -1 ) {
      close(input);
      close(output);
    }
    
    kill( daemon_pid, SIGTERM );
    daemon_pid = -1;
    
    // Purge the host_stack.
    while( !host_stack.empty ) {
      beetle( "Kill_Daemon: Closing pending connection." );
      link_data* link = host_stack.headitem();
      host_stack.remove( link );
      if( link->channel > 0 )
        CLOSE_SOCKET( link->channel );
      extract( link );  // extract deletes the link
    }
  }

#endif

}


