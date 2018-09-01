#define BOOT_WORLD     0
#define BOOT_PLAYERS   1
#define BOOT_COMPILE   2
#define BOOT_UPDATE    3
#define BOOT_COMPLETE  4

extern int      boot_stage;
extern int      tfe_reboot;
extern bool       tfe_down;
extern bool   in_character;
extern const char* mysteria_about;
extern const char* mysteria_copyright;

/*
 *   INDEX ROUTINES
 */


class Index_Data
{
 public:
   const char*  singular;
   const char*    plural;
   int             value;
};


const char*  lookup     ( index_data*, int, bool = FALSE );


/*
 *   HELP ROUTINES
 */


help_data*  find_help    ( char_data*, const char*, bool = true );


/*
 *   INFO ROUTINES
 */


void   info   ( const char*, int, const char*, int, int = 3,
                char_data* = NULL, clan_data* = NULL );


/*
 *   NOTE/MAIL ROUTINES
 */


void   mail_message     ( char_data* );


/*
 *   STRING FUNCTIONS
 */


#define strlen( string )    int( strlen( string ) )


class String
{
 public:
  char*      text;
  int      length;

  String( const char* msg ) {
    length = strlen( msg );
    text   = new char [ length+1 ];
    memcpy( text, msg, length+1 );
    return;
    }

  ~String( ) {
    delete [] text;
    return;
    }
};


inline const char* name( char* word )
{
  return word;
} 


inline const char* name( const char* word )
{
  return word;
}


/*
 *   THING TYPES
 */

#define VISIBLE_DATA   (1 << 0)
#define THING_DATA     (1 << 1)
#define AUCTION_DATA   (1 << 2)
#define EXIT_DATA      (1 << 3)
#define EXTRA_DATA     (1 << 4)
#define ROOM_DATA      (1 << 5)
#define OBJ_DATA       (1 << 6)
#define CHAR_DATA      (1 << 7)
#define MOB_DATA       (1 << 8)
#define PLAYER_DATA    (1 << 9)
#define WIZARD_DATA    (1 << 10)


/*
 *   TRUST
 */


int    get_trust      ( char_data* );
