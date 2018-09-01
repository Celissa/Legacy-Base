// SYSTEM.H ... DEFINES FOR OPERATING SYSTEMS
#ifndef __MYSTERIA_SYSTEM_H
#define __MYSTERIA_SYSTEM_H

#define tfe_version "Mysteria MUD Engine II: 0.1a"
  ///////////////////////////////////////////////////////////////////////////////////////////////
  // The port to listen by default on (in windows you can't override this on the command line)

  #define MYSTERIA_PORT 2222

  ///////////////////////////////////////////////////////////////////////////////////////////////
  // OS Autoselection (or do it manually if you want)

  #ifdef WIN32
    #define MYSTERIA_WINDOWS
  #else
    #define MYSTERIA_UNIX
  #endif

  ///////////////////////////////////////////////////////////////////////////////////////////////
  // standard #includes and #defines
  #include <stdio.h>
  #include <ctype.h>
  #include <stdarg.h>
  #include <string.h>
  #include <stdlib.h>
  #include <time.h>
  #include <math.h>

  #include <assert.h>
  #include <errno.h>
  #include <fcntl.h>
  #include <limits.h>
  #include <sys/stat.h>
  #include <sys/types.h>

  #ifdef MYSTERIA_WINDOWS
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // WIN32-BASE #includes and #defines
    #define WIN32_LEAN_AND_MEAN  // use lean windoze routines
    #define FD_SETSIZE 1024      // bump up max number of sockets in a fd_set

    #define pid_t int
    #define socklen_t int
    #define ssize_t int

    #include <windows.h>
    #include <shellapi.h>
    #include <process.h>
    #include <io.h>
    #ifndef _WINSOCK2API_        // Winsock1 and Winsock 2 conflict
      #include <winsock2.h>
    #endif

    #include "selt_telnet.h"

    #define IAC             TC_IAC
    #define WILL            TC_WILL
    #define WONT            TC_WONT
    #define TELOPT_ECHO     TO_ECHO

    // close socket definition macro (so we can use the same call in code no matter what OS)
    #define CLOSE_SOCKET(sock)    closesocket(sock)
    typedef SOCKET               socket_t;

    // and some compiler-based junk to shut up some silly errors or something
    #if !defined(__BORLANDC__) && !defined(LCC_WIN32)  // MSVC
      #define chdir _chdir
      #pragma warning(disable:4761)  // Integral size mismatch.
      #pragma warning(disable:4244)  // Possible loss of data.
      #pragma warning(disable:4996)  // unsafe strcpy
      #pragma warning(disable:4355)  // use of 'this' in base class
      //#define for if(0);else for     // Scope of for loop is buggy in MSVC
    #endif

    #if defined(__BORLANDC__)       // Silence warnings we don't care about.
      #pragma warn -par              // to turn off >parameter< 'ident' is never used.
      #pragma warn -pia              // to turn off possibly incorrect assignment. 'if (!(x=a))'
      #pragma warn -sig              // to turn off conversion may lose significant digits.
    #endif

    #if !defined(__GNUC__)
      #define __attribute__(x)       // ignore
    #endif

    #ifndef _BASETSD_H_
      typedef __int32 INT32;        // for those who don't have the base standard types library
    #endif

    #ifdef max
      #undef max
    #endif

    #ifdef min
      #undef min
    #endif

    #define WM_TRAY       WM_USER + 0x100 + 5
    #define WM_HOSTNAME   WM_USER + 0x100 + 6
    extern HWND MainWindow;

  #elif defined MYSTERIA_UNIX
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // UNIX-BASED OS #includes and #defines
    // close socket definition macro (so we can use the same call in code no matter what OS)
    #include <syslog.h>
    #include <sys/time.h>
    #include <unistd.h>
    #include <dirent.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <sys/wait.h>
    #include <sys/resource.h>
    #include <sys/param.h>
    #include <netinet/in.h>
    #include <arpa/telnet.h>
    #include <signal.h>
    #include <pwd.h>

    #define SD_BOTH               SHUT_RDWR
    #define CLOSE_SOCKET(sock)    close(sock)
    typedef int                   socket_t;
    typedef long                  INT32;
    typedef unsigned long         UINT32;
    typedef long long             INT64;
    typedef unsigned long long    UINT64;

  #else
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // OS NOT SUPPORTED
    #error Your OS is not supported in system.h: Please create your own definition and system includes
  #endif

  #define MAX_SOCK_BUF (12 * 1024) // size of socket buffers

  #include <my_global.h>
  #include <mysql.h>

  // undefine some of the stupid mysql defines
  #ifdef sleep
  #undef sleep
  #endif

  #ifdef bool
  #undef bool
  #endif

  #ifdef max
  #undef max
  #endif

  #ifdef min
  #undef min
  #endif

  #define BSTRLIB_DOESNT_THROW_EXCEPTIONS
  #include "bstrwrap.h"

  #include "define.h"
  #include "struct.h"

#endif // __MYSTERIA_SYSTEM_H




