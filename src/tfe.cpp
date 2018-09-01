#include "system.h"

unsigned short port = MYSTERIA_PORT;
bool                 tfe_down  = FALSE;  
int                  tfe_reboot = 0;

char            str_boot_time  [ 26 ];

time_t              boot_time;
time_data           last_time;


void  init_memory         ( void );
void  init_variables      ( void );
void  main_loop           ( void );
void  set_time            ( void );
void  wait_pulse          ( void );
void  record_time         ( time_data& );

void sighandler( int sig )
{

#ifdef MYSTERIA_UNIX

  char* name;

  switch( sig ) {
   case SIGPIPE :   name = "broken pipe";  break;
   case SIGBUS  :   name = "bus error";    break;
   default      :   name = "unknown";      break; 
     }

  roach( "Mud received signal %d, %s.", sig, name );
  roach( "Error: %s", strerror( errno ) );

#endif

}  

void sigchld_handler(int)
{

#ifdef MYSTERIA_UNIX

  pid_t a_pid;
  int child_status;
  a_pid = wait(&child_status);
  if ( WIFEXITED(child_status) ) 
  {
     bug( "Daemon exited with status %d.  Restart it!",
          WEXITSTATUS(child_status) );
  }
  else if ( WIFSIGNALED(child_status) )
  {
     if ( WTERMSIG(child_status) != SIGTERM ) 
     {
        bug( "Daemon died with signal %d.  Restart it!", 
             WTERMSIG(child_status) );
     }
  }

#endif

}

/*
 *   MAIN PROCEDURE
 */

#ifdef MYSTERIA_WINDOWS

#include "resource.h"

HWND MainWindow = NULL;
LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam);
HMENU SystrayMenu = NULL;
HICON SystrayIcon = NULL;

int WINAPI WinMain(HINSTANCE NewInstance, HINSTANCE PrevInstance, LPSTR CmdLine, int ShowState)
{
  // Define some variables ------------------------------------------------------------------------
  WNDCLASSEX WindowClassEx;
  ATOM RegisteredClass;
  HINSTANCE MainInstance = NULL;
  NOTIFYICONDATA SystrayData;
  time_t now = 0;

  log(tfe_version);
  log(GetCommandLine());

  MainInstance = NewInstance; // We could also test for an already running instance here if required.
  SystrayMenu = LoadMenu(MainInstance, MAKEINTRESOURCE(SYSTRAY_MENU));
  SystrayIcon = LoadIcon(MainInstance, MAKEINTRESOURCE(SYSTRAY_ICON));

  // Deal with the Window Class -------------------------------------------------------------------
  WindowClassEx.cbSize = sizeof(WNDCLASSEX);
  WindowClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
  WindowClassEx.lpfnWndProc = WindowProc;
  WindowClassEx.cbClsExtra = 0;
  WindowClassEx.cbWndExtra = 0;
  WindowClassEx.hInstance = MainInstance;
  WindowClassEx.hIcon = SystrayIcon;
  WindowClassEx.hIconSm = SystrayIcon;
  WindowClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
  WindowClassEx.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
  WindowClassEx.lpszMenuName = NULL;
  WindowClassEx.lpszClassName = tfe_version;

  RegisteredClass = RegisterClassEx(&WindowClassEx);
  if (RegisteredClass != 0 && SystrayMenu != NULL && SystrayIcon != NULL) { // We have a registered class, woo, now make the main window
    MainWindow = CreateWindowEx (
      WS_EX_TOPMOST | WS_EX_TOOLWINDOW, // extended window style
      (LPCTSTR) RegisteredClass,        // pointer to registered class name
      tfe_version,         // pointer to window name
      WS_OVERLAPPEDWINDOW, // window style
      CW_USEDEFAULT,       // horizontal position of window
      CW_USEDEFAULT,       // vertical position of window
      320,                 // window width
      200,                 // window height
      NULL,                // handle to parent or owner window
      NULL,                // handle to menu, or child-window identifier
      MainInstance,        // handle to application instance
      NULL                 // pointer to window-creation data
    );
  }

  if( MainWindow != NULL ) { // We're ready to rock
    SystrayData.cbSize = sizeof(NOTIFYICONDATA);
    SystrayData.hWnd   = MainWindow;
    SystrayData.uID    = SYSTRAY_ICON;
    SystrayData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    SystrayData.uCallbackMessage = WM_TRAY;
    SystrayData.hIcon  = SystrayIcon;
    strcpy(SystrayData.szTip, tfe_version);

    Shell_NotifyIcon(NIM_ADD, &SystrayData);

    // Show (or rather, hide) the main window
    ShowWindow(MainWindow, SW_HIDE); // (as opposed to ShowState)
    UpdateWindow(MainWindow);

    Sleep(1000);

    try {
      log("Starting game on port %d.", port);

      time_data  start;
      gettimeofday( &start, NULL );

      set_time( );
      init_memory( );
      init_variables( );

      log( "** STARTING MUD **" );

      boot_db();

      socket_one = open_port( port );

      startup_time = stop_clock( start ).tv_sec;
      gettimeofday( &last_time, NULL );

      echo( "Mysteria II Done Booting\r\n" );
      log( "** MUD BOOTED **" );

    } catch (int i) {
      log("THROW: Mud forced termination during bootup (%d).", i);
      tfe_down = 1;

    #ifndef _DEBUG
    } catch (...) {
      log("CRASH: Mud crashed while booting up.");
      tfe_reboot = 0;
    #endif
    }

    try {
      main_loop( );

    } catch (int i) {
      log("THROW: Mud encounted unrecoverable error %d.", i);
      tfe_reboot = 1;

    #ifndef _DEBUG
    } catch (...) {
      log("CRASH: Mud crashed while running game.");
      tfe_reboot = 1;
    #endif
    }

    log("Gigs: %d, Bytes: %d", gigs_written, bytes_written);
    close_port();

    Sleep(1000);
  }

  // Shutdown and clean up ------------------------------------------------------------------
  Shell_NotifyIcon(NIM_DELETE, &SystrayData);

  if (SystrayMenu != NULL)
    DestroyMenu(SystrayMenu);

  if (SystrayIcon != NULL)
    DestroyIcon(SystrayIcon);

  if (RegisteredClass != 0)
    UnregisterClass((LPCTSTR) RegisteredClass, MainInstance);

  if (tfe_reboot) {
    log("Rebooting...");
    if (spawnl(_P_NOWAIT, "mysteria ii.exe", "23", NULL) < 0) {
      MessageBox(NULL, "Reboot FAILED: Unable to restart MUD.", tfe_version, MB_OK);
      log("Error Rebooting: %d", errno);
    }
  }

  return 0;
}

#define WSAGETASYNCERROR(lParam)            HIWORD(lParam)
#define WSAGETASYNCBUFLEN(lParam)           LOWORD(lParam)

LRESULT CALLBACK WindowProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam) {
  switch (Message) {
  case WM_TRAY:
    if (lParam == WM_RBUTTONDOWN) {
      POINT pt;
      GetCursorPos(&pt);  //get cursor position
      SetForegroundWindow(Window);  //set window to foreground
  
      //display popup menu
      TrackPopupMenu(GetSubMenu(SystrayMenu, 0), TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, Window, 0);
    } else if (lParam == WM_LBUTTONDBLCLK) {
      PostMessage(Window, WM_COMMAND, (WPARAM) ID_TRAY_ABOUT, 0);
    }
    return 0;

  case WM_HOSTNAME:
  {
    link_nav links( &link_list );
    for (link_data *link = links.first(); link != NULL; link = links.next() ) {
      if (link->hosthandle == (HANDLE) wParam) {
        // do the update
        if (!WSAGETASYNCERROR(lParam) && link->hostbuf) {
          // no error, we should be able to copy out the hostname
          HOSTENT *host = (HOSTENT *) link->hostbuf;
          if (host && host->h_name) {
            free_string(link->host, MEM_LINK);
            link->host = alloc_string(host->h_name, MEM_LINK);
          }

          delete [] link->hostbuf;
          link->hostbuf = NULL;
        }
      }
    }
    return 0;
  }

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case ID_TRAY_EXIT:
      echo( "** Shutdown by Sysadmin. **\r\n" );
      shutdown_sequence( );

      tfe_reboot = 0;
      tfe_down = 1;

      PostQuitMessage(0);
      break;
    
    case ID_TRAY_REBOOT:
      echo( "** Reboot by Sysadmin. **\r\n" );
      shutdown_sequence( );

      tfe_reboot = 1;
      tfe_down = 1;

      PostQuitMessage(0);
      break;

    case ID_TRAY_REBOOTNOW:
      echo( "** Reboot by Sysadmin.  Unsaved Data Discarded. **\r\n" );

      tfe_reboot = 2;
      tfe_down = 1;

      PostQuitMessage(0);
      break;

    case ID_TRAY_ABOUT:
      MessageBox(Window, mysteria_copyright, tfe_version, MB_OK);
      break;
    }
    return 0;

  case WM_QUIT:
    // For some reason WM_QUIT isn't handled by PeekMessage
    // therefore we have to intercept it in the main loop and not here
    // but just in case.. we'll shove one here also mk?
    tfe_down = 1;
    return 0;

  default:
    return DefWindowProc(Window, Message, wParam, lParam);
  }
}

#elif defined MYSTERIA_UNIX

int main( int argc, char **argv )
{
  time_data  start;

  system("touch .killscript");

  if( argc > 1 ) 
    port = atoi( argv[1] );

  struct passwd* process_owner = getpwuid(geteuid());
  if ( process_owner != NULL ) {
    log( "Process Owner: %s\n", process_owner->pw_name );
  }

/*  sigset( SIGPIPE, sighandler ); */
  signal( SIGPIPE, sighandler );
  signal( SIGBUS, sighandler );
   // Disable the sigchld handler until we can replace system() calls with
   // fork()/exec().  (See system man page).  wait() was hanging the mud.
/*  signal( SIGCHLD, sigchld_handler ); */

  gettimeofday( &start, NULL );

  set_time( );
  init_memory( );
  init_variables( );

  bug( -1, "** STARTING MUD **" );

  init_daemon( );
  boot_db( );

  socket_one = open_port( port );

  startup_time = stop_clock( start ).tv_sec;
  gettimeofday( &last_time, NULL );

  echo( "Mysteria II Done Booting\r\n" );
  bug( -1, "** MUD BOOTED **" );
  system("rm .killscript");

  main_loop( );

  close_port();
  kill_daemon();
  return 0;
}

#endif


void main_loop( void )
{
  while( !tfe_down ) {

/* Win32 ----> */ #ifdef MYSTERIA_WINDOWS
    MSG WinMsg;

    while (PeekMessage(&WinMsg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&WinMsg);
      DispatchMessage(&WinMsg);
    }

    if (WinMsg.message == WM_QUIT)
      tfe_down = 1;
/* <---- Win32 */ #endif

    update_handler( );
    read_host( );
    update_links( );
    wait_pulse( );
  }

  return;
}


/*
 *   TIME HANDLING ROUTINES
 */


void set_time( void )
{
  int i;

  gettimeofday( &last_time, NULL );

  boot_time    = time(0);

  strcpy( str_boot_time, ctime( &boot_time ) );
  srand( boot_time );

  for( i = 0; i < MAX_TIME; i++ ) {
    critical_time[i] = 0;
    total_time[i]    = 0;
    pulse_time[i]    = 0;
  }

  return;
}


void wait_pulse( void )
{
  time_data   now_time;
  int                i;

  gettimeofday( &now_time, NULL );

  last_time                 -= now_time;
  total_time[ TIME_ACTIVE ] -= last_time;

  now_time.tv_sec  = 0;
  now_time.tv_usec = 1000000/PULSE_PER_SECOND;
 
  last_time += now_time; 

  record_time( last_time );

  if( last_time.tv_sec == 0 && last_time.tv_usec > 0 ) {
    total_time[ TIME_WAITING ] += last_time; 

#ifdef MYSTERIA_WINDOWS
    Sleep( ( last_time.tv_sec * 1000 + last_time.tv_usec ) / 1000 );
#elif defined MYSTERIA_UNIX
    if( select( 0, NULL, NULL, NULL, &last_time ) < 0 ) 
      bug( "Wait_Pulse: error in select" );
#else
#error No Sleep function defined for your OS
#endif

  } else {
    critical_time[ TIME_ACTIVE ]  -= last_time;
    critical_time[ TIME_ACTIVE ]  += now_time;
    critical_time[ TIME_WAITING ] -= last_time;

    for( i = TIME_WAITING+1; i < MAX_TIME; i++ )
      critical_time[i] += pulse_time[i];
  } 

  for( i = TIME_WAITING+1; i < MAX_TIME; i++ ) {
    total_time[i] += pulse_time[i];
    pulse_time[i] = 0;
  }

  gettimeofday( &last_time, NULL );

  return;
}


void record_time( time_data& time )
{
  int  lag  = -10*time.tv_sec-time.tv_usec/100000;
  int    i  = 0;

  if( !link_list.empty ) {
    for( i = 0; lag > 0 && i < 9; lag /= 2, i++ );
    time_history[i]++;
  }

  return;
}
 

/*
 *   ROUTINE TO INIT CONSTANTS
 */


void init_variables( void )
{
  int i;  

  for( i = 0; i < MAX_PFILE; i++ ) 
    ident_list[i] = NULL;

  for( i = 0; i < 5000; i++ )
    event_queue[i] = NULL;

  for( i = 0; i < MAX_IFLAG; i++ )
    info_history[i] = NULL;

  return;
}














