/*
 *   CLASSES
 */


class Reset_Data 
{
 public:
  reset_data*        next;
  void*              target;
//  int                vnum;
  int                flags;
  int                chances;
  int                value;
  int                count;
  int                liquid;

  time_data          lag;

  Reset_Data( );
  ~Reset_Data( );
};


class Rtable_Data
{
 public:
  char*         name;
  reset_data*   reset;
  int           vnum;

  Rtable_Data( );
  ~Rtable_Data( );
};

/*
 *   DEFINITIONS
 */

extern rtable_array rtable_list;
rtable_data *find_rtable( int );
rtable_data *find_rtable( char* );
int reset_table_load( rtable_data* );

#define RST_ROOM              0
#define RST_MOB               1
#define RST_TABLE             2

#define RSFLAG_REROLL         0
#define RSFLAG_MOB            3
#define RSFLAG_OBJECT         4

#define RSFLAG_LEADER         5
#define RSFLAG_FOLLOWER       6
#define RSFLAG_SENTINEL       7
#define RSFLAG_NIGHT          8
#define RSFLAG_DAY            9
#define RSFLAG_AGGRESSIVE    10

#define RSFLAG_CONTAINER      5
#define RSFLAG_INSIDE         6
#define RSFLAG_UNKNOWN        7
#define MAX_RSFLAG           11


#define RSPOS_SLEEPING     0
#define RSPOS_MEDITATING   1
#define RSPOS_RESTING      2
#define RSPOS_STANDING     3
#define MAX_RESET_POS      4


/*
 *   FUNCTIONS
 */


thing_array*   get_skin_list   ( species_data*, char_data* = NULL );
bool passes( reset_data*, int*, char_data* = NULL );

void   load_rtables    ( void );
void   fix_rtables     ( void );
void   save_rtables    ( void );

void   reset_shop      ( mob_data* );
void   mreset_mob      ( mob_data* );

char*  name            ( reset_data* );

const char* edit_reset ( char_data*, char*, reset_data*&, int,
                         const char* = empty_string );

bool   generic_search  ( char_data*, void*, int, arg_enum, bool );


