// ********************************************************************
// File Name:     daemon.cc
//
// Routines:      sigalrm_handler()
//                main()
//                update()
// *********************************************************************

#include <arpa/telnet.h>
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include "signal.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

// ************** Forward declarations *****************
void update( void );

// ************** Global variables ********************
char  read_buffer[ 160 ];
int   read_pntr = 0; // index into read_buffer
int   idle_time = 0;
int   input;         // file descriptor
int   output;        // file descriptor

void sigalrm_handler(int)
{
  fprintf(stderr, "[Daemon] Hostname resolution interrupted! (over 5 sec)\n");
}

int main( int, char **argv )
{
  fd_set read_fds ;
  struct timeval timeout;
  signal( SIGPIPE, SIG_IGN );
  signal( SIGALRM, sigalrm_handler );

  input   = atoi( argv[1] );
  output  = atoi( argv[2] );

  fprintf( stderr, "[Daemon] Starting with ports %d and %d\n", input, output );

  fcntl( input, F_SETFL, O_NDELAY );

  for( ; ; ) {
     timeout.tv_usec = 100000;
     timeout.tv_sec  = 0;
     FD_ZERO(&read_fds);
     FD_SET(input, &read_fds);
     int selectStatus = select(64, &read_fds, NULL, NULL, &timeout);
     if( selectStatus == -1 ) {
        fprintf( stderr, "[BUG] Delay: error in select.\n" );
        exit( 1 );
     }
     else if ( selectStatus == 0 ) { // timeout
        idle_time++;
     }
     else {  // input received
        if ( FD_ISSET(input, &read_fds) ) {
           update( );
        }
     }
  }
    
  return 0;
}

void update( )
{
  #define sa  int( sizeof( struct in_addr ) )

  // Read up to 4 IP addresses at a time
  int nRead = read( input, &read_buffer[read_pntr], 4*sa ); 
  if ( nRead > 0 ) {
     read_pntr += nRead;
  }
  else if ( nRead < 0 ) { // read error
     perror("read");
     fprintf( stderr, "[Daemon] Exiting due to read error\n" );
     exit( 1 );
  }
  else if ( nRead == 0 ) { // eof
    fprintf( stderr, "[Daemon] Exiting on end of file\n" );
    exit( 1 );
  }

  if( read_pntr < sa ) { // incomplete read, check to see if socket went bad
    if( ++idle_time > 10000 ) {
      if( write( output, "Alive?", 7 ) < 0 ) {
        fprintf( stderr, "[Daemon] Exiting\n" );
        exit( 1 );
      }
      idle_time = 0;
    }
    return;
  }

  int addr;
  char buf[ 80 ];
  struct hostent* from;
  do {
     memcpy( &addr, read_buffer, sizeof( int ) );
     addr = ntohl( addr );

     sprintf( buf, "%d.%d.%d.%d", ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
                                ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF );
     fprintf(stderr, "[Daemon] Read IP address %s\n", buf);

     // Time the gethostbyaddr to see how long hostname lookup takes (DEBUG)
     struct timeval time_before;
     gettimeofday(&time_before, NULL);
     //alarm(5);                          // interrupt after 5 seconds
     from = gethostbyaddr( read_buffer, sa, AF_INET );
     // This isn't working.  Somehow a SIGALRM is causing daemon to
     // terminate!  It is unpredictable as well.
     //alarm(0);                          // cancel the interrupt
     struct timeval time_after;
     gettimeofday(&time_after, NULL);
     int time_elapsed = (time_after.tv_sec - time_before.tv_sec)*1000000
          + (time_after.tv_usec - time_before.tv_usec); // usec
 
     if( from != NULL ) {
        write( output, from->h_name, 1+strlen( from->h_name ) );
        fprintf(stderr, "[Daemon] Wrote host name %s (%d msec)\n", from->h_name,
          time_elapsed/1000);
     }
     else { // DNS failed, instead send dot-separated IP address string
        write( output, buf, 1+strlen( buf ) );
        fprintf(stderr, "[Daemon] Wrote IP address %s (%d msec)\n", buf,
                time_elapsed/1000);
     }

     // Copy buffer backward 4 bytes, and push back pointer
     for( int i = sa; i < read_pntr; i++ ) {
        read_buffer[i-sa] = read_buffer[i];
     }
     read_pntr -= sa; 
  }
  while ( read_pntr >= sa ); // continue as long as we have a complete IP addr

  #undef sa

  return;
}




