/*
 *   FILE LOCATIONS
 */


#define AREA_DIR           "../area/"
#define AREA_SUF           ".are"

#define BOARD_DIR          "../boards/"
#define BOARD_SUF          ".brd"

#define CLAN_DIR           "../clans/"
#define CLAN_SUF           ".clan"

#define MAIL_DIR           "../mail/"
#define NOTE_DIR           "../notes/"
#define PLAYER_DIR         "../player/"  
#define ROOM_DIR           "../rooms/"
#define TABLE_DIR          "../tables/"
#define W3_DIR             "../"

#define OBJ_LOG_DIR        "../logs/object/"
#define PLAYER_LOG_DIR     "../logs/player/" 
#define ROOM_LOG_DIR       "../logs/room/"
#define IMMORTAL_LOG_DIR   "../logs/immortal/"
#define MOB_LOG_DIR        "../logs/mob/"
#define MOB_EXP_LOG_DIR    "../logs/mobexp/"

#define HELP_FILE          "../area/help.dat"
#define QUEST_FILE         "../area/quest.dat"   // old quests
#define QUESTS_FILE        "../area/quests.dat"  // new quests
#define RTABLE_FILE        "../area/rtable.dat"
#define SHOP_FILE          "../area/shop.dat"
#define TRAINER_FILE       "../area/trainer.dat"

#define XP_FILE            "../files/xp.dat"
#define MYSQL_FILE         "../sql/settings.dat"
#define INIT_FILE          "../sql/init.sql"
#define MISC_FILE          "../files/misc"
#define POLLS_FILE         "../files/polls.dat"
#define ACCOUNT_FILE       "../files/accounts"
#define BADNAME_FILE       "../files/badname.txt"
#define BANNED_FILE        "../files/banned.txt"   

#define BUG_FILE           "./bugs.txt"   

#define MOB_FILE           "mob.mob"
#define OBJECT_FILE        "obj.obj"

#define PREV_DIR           "../prev/"
#define AREA_PREV_DIR      "../prev/area/"
#define PLAYER_PREV_DIR    "../prev/player/"
#define PLAYER_QUIT_DIR    "../prev/player-quit/"
#define PLAYER_DEATH_DIR   "../prev/player-death/"
#define PLAYER_RESTORE_DIR "../prev/player-restore/"
#define ROOM_PREV_DIR      "../prev/rooms/"
#define TABLE_PREV_DIR     "../prev/tables/"

/*
 *   BUG FUNCTIONS
 */


void   panic       (const char* );
void   bug         (int, const char*, int = LEVEL_APPRENTICE);
void   log         (const char *format, ...);

#define BUG_BEETLE  3
#define BUG_SYSLOG  2
#define BUG_ROACH   1


//inline void aphid  ( const char* text )   {  bug( 3, text ); } 
inline void beetle ( const char* text )   {  bug( 3, text ); } 
inline void roach  ( const char* text )   {  bug( 1, text ); }
inline void bug    ( const char* text )   {  bug( 3, text ); }

/*
template < class T >
void aphid( const char* text, T item )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item ) );
  corrupt( tmp, TWO_LINES, "Aphid" );
  
  if( *text == '%' )
    *tmp = toupper( *tmp );

  bug( 3, tmp );

  return;
}
*/


template < class T >
void roach( const char* text, T item )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item, NULL ) );
  corrupt( tmp, TWO_LINES, "Roach" );

  if( *text == '%' )
    *tmp = toupper( *tmp );

  bug( 1, tmp );

  return;
}


template < class S, class T >
void roach( const char* text, S item1, T item2 )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item1 ), tostring( item2 ) );
  corrupt( tmp, TWO_LINES, "Roach" );
  
  if( *text == '%' )
    *tmp = toupper( *tmp );

  bug( 1, tmp );

  return;
}


template < class T >
void bug( const char* text, T item )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item, NULL ) );
  corrupt( tmp, TWO_LINES, "Bug" );

  if( *text == '%' )
    *tmp = toupper( *tmp );

  bug( tmp );

  return;
}


template < class S, class T >
void bug( const char* text, S item1, T item2 )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, tostring( item1, NULL ), tostring( item2, NULL ) );
  bug( tmp );
}


template < class T >
void panic( const char* text, T item )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, item );
  
  if( *text == '%' )
    *tmp = toupper( *tmp );

  panic( tmp );
}


template < class S, class T >
void panic( const char* text, S item1, T item2 )
{
  char   tmp  [ TWO_LINES ];
 
  sprintf( tmp, text, item1, item2 );
  
  if( *text == '%' )
    *tmp = toupper( *tmp );

  panic( tmp );
}


inline void corrupt( const char* msg, int length, const char* func )
{
  if( int( strlen( msg ) ) >= length ) 
    roach( "%s: Randomly corrupting memory.", func );
}


/*
 *   FILE ROUTINES
 */

int dirlist(filelist *list, const char *filespec);

FILE*   open_file         ( const char*, const char*, const char*, bool = FALSE );
FILE*   open_file         ( const char*, const char*, bool = FALSE ); 
void    delete_file       ( const char*, const char*, bool = TRUE );
void    rename_file       ( const char*, const char*, const char*, const char* );
void    copy_file         ( const char*, const char*, const char*, const char* );

char    fread_letter      ( FILE *fp );
int     fread_number      ( FILE *fp );
void    fread_to_eol      ( FILE *fp );
char*   fread_block       ( FILE* );
char*   fread_string      ( FILE*, int, bool = false );
char*   fread_word        ( FILE* );


void write_all  ( bool = FALSE );


/*
 *   LOG ROUTINES
 */


void   immortal_log       ( char_data*, char*, char* );
void   mob_log            ( char_data*, int, const char* ); 
void   obj_log            ( char_data*, int, const char* ); 
void   room_log           ( char_data*, int, const char* ); 
void   player_log         ( char_data*, const char* );
void   player_log         ( pfile_data*, const char* );

template < class S >
void player_log( char_data* ch, const char* text, S item1 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, tostring( item1, ch ) );
  player_log( ch, tmp );

  return;
}

  
template < class S, class T >
void player_log( char_data* ch, const char* text, S item1, T item2 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ) );
  player_log( ch, tmp );

  return;
}

template < class S >
void player_log( pfile_data* ch, const char* text, S item1 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, tostring( item1, ch ) );
  player_log( ch, tmp );

  return;
}

  
template < class S, class T >
void player_log( pfile_data* ch, const char* text, S item1, T item2 )
{
  char tmp  [ SIX_LINES ];

  sprintf( tmp, text, tostring( item1, ch ), tostring( item2, ch ) );
  player_log( ch, tmp );

  return;
}

/*
 *   HELP ROUTINES
 */


bool   save_help          ( char_data* = NULL );
  

/*
 *   QUITTING ROUTINES
 */


void   forced_quit        ( player_data*, bool = FALSE );


/*
 *   WEBPAGE ROUTINES
 */


void w3_help  ( void );
void w3_who   ( void );





