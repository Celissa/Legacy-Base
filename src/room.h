/*
 *   AREA HEADER
 */


#define AREA_OPEN           0
#define AREA_WORTHLESS      1
#define AREA_ABANDONED      2
#define AREA_PROGRESSING    3
#define AREA_PENDING        4
#define AREA_BLANK          5
#define AREA_IMMORTAL       6
#define MAX_AREA_STATUS     7

#define OLD_CONTINENT       0

extern  const char*   area_status  [ MAX_AREA_STATUS ];

void    load_areas     ( void );
void    load_area      ( const char* );
void    load_rooms     ( FILE*, area_data* );
bool    save_area      ( area_data*, bool = FALSE );
bool    set_area_file  ( char_data*, const char*, const char* );


class Area_Data
{
public:
//  area_data*         next;
  room_data*   room_first;
  char*              file;
  char*           creator;
  char*              name;
  char*              help;
  bool           modified;
  int             nplayer;
  int               level;
  int          reset_time;
  int                 age;
  int              status;
  int               flags;
  int           continent;
  int         reset_count;
  char*          tax_rate;
  char*             bonus;

  time_data     reset_lag;
  time_data     total_lag;

  Area_Data( );
  Area_Data( char*, int, int );

  virtual void Show( room_data*, char_data*, bool, bool );
  virtual void Reset( bool force = false );
};


room_data*   get_room_index    ( int, bool = FALSE );
bool         can_edit          ( char_data*, room_data*, bool = TRUE );


/*
 *   EXIT HEADER
 */


#define MAX_DOOR                    6


#define EX_ISDOOR                   0
#define EX_CLOSED                   1
#define EX_LOCKED                   2
#define EX_SECRET                   3
#define EX_PICKPROOF                4
#define EX_NO_SHOW                  5
#define EX_NO_OPEN                  6
#define EX_RESET_CLOSED             7      
#define EX_RESET_LOCKED             8
#define EX_RESET_OPEN               9
#define EX_REQUIRES_CLIMB          10
#define EX_SEARCHABLE              11
#define MAX_DFLAG                  12


extern const char* open_msg  [];
extern const char* close_msg [];
extern const char* lock_door_msg [];
extern const char* unlock_door_msg [];
extern const char* knock_door_msg [];
extern const char* untrap_door_msg [];


class Exit_Data : public Visible_Data
{
 public:
  room_data*    to_room;
  int           exit_info;
  int           key;
  int           strength;
  int           size;
  int           direction;
  int           light;
  char*         name;
  char*         keywords;
  
  Exit_Data     ( );
  ~Exit_Data    ( );

  virtual int Type( )  { return EXIT_DATA; }

  virtual const char*   Name       ( char_data*, int = 1, bool = FALSE );
  virtual const char*   Seen_Name  ( char_data*, int = 1, bool = FALSE );
  virtual const char*   Keywords   ( char_data* );
  bool                  Seen       ( char_data* );
  void                  Look_At    ( char_data* );
};


int         exits_prompt       ( char*, char_data* );
void        read_exits         ( FILE*, room_data*, int );
void        autoexit           ( char_data*, int );

exit_data*  exit_direction     ( room_data*, int );
exit_data*  reverse            ( exit_data* );

int         find_door          ( char_data*, char*, bool = TRUE );
int         find_door_always   ( char_data*, char* );

exit_data*  random_exit          ( room_data* );
exit_data*  random_open_exit     ( room_data* );
exit_data*  random_movable_exit  ( char_data* );

bool        open_door          ( char_data*, exit_data* );
bool        close_door         ( char_data*, exit_data* );
bool        lock_door          ( char_data*, exit_data* );
bool        unlock_door        ( char_data*, exit_data* );
bool        pick_door          ( char_data*, exit_data* );
void        bash_door          ( char_data*, exit_data* );
void        knock_door         ( char_data*, exit_data* );


/*
 *   LOCATIONS TYPES
 */


#define LOC_OUTDOORS    0
#define LOC_INDOORS     1
#define LOC_SUNLIGHT    2
#define LOC_FULLMOON    3
#define LOC_FOREST      4
#define LOC_ABOVEWATER  5
#define MAX_LOCATION    6


bool allowed_location   ( char_data*, int*, const char*, const char* );




/*
 *   ROOM HEADER
 */

#define RFLAG_LIT                   0
#define RFLAG_SAFE                  1
#define RFLAG_INDOORS               2
#define RFLAG_NO_MOB                3
#define RFLAG_NO_RECALL             4
#define RFLAG_NO_MAGIC              5
#define RFLAG_NO_AUTOSCAN           6
#define RFLAG_ALTAR                 7
#define RFLAG_SERVICE_SHOP          8
#define RFLAG_CODE_RDESC            9   // altar
#define RFLAG_TOWN                 10
#define RFLAG_BANK                 11
#define RFLAG_SHOP                 12
#define RFLAG_PET_SHOP             13
#define RFLAG_OFFICE               14
#define RFLAG_NO_PRAY              15
#define RFLAG_SAVE_ITEMS           16
#define RFLAG_UNDERGROUND          17
#define RFLAG_AUCTION_HOUSE        18
#define RFLAG_RESET0               19
#define RFLAG_RESET1               20
#define RFLAG_RESET2               21
#define RFLAG_STATUS0              22
#define RFLAG_STATUS1              23
#define RFLAG_STATUS2              24
#define RFLAG_NO_MOUNT             25
#define RFLAG_ARENA                26
#define RFLAG_APPROVED             27
#define RFLAG_SAVE_VARIABLES       28  // no.pkill
#define RFLAG_NO_SUMMON_IN         29
#define RFLAG_NO_SUMMON_OUT        30
#define RFLAG_NO_GATE              31
#define MAX_RFLAG                  32


class Room_Data : public Thing_Data, public Template_Data
{
 public:
  exit_array           exits; 
  extra_array    extra_descr;
  room_data*            next;
  room_data*       next_list;
  area_data*            area;
  action_data*        action;
  reset_data*          reset;
  track_data*          track;
  char*                 name;
  char*          description;
  char*             comments;
//  int                   vnum;
  int             room_flags;
  int            sector_type;
  int               distance;
  int                counter;
  int                   size;
  int                x, y, z;

  int            reset_count;
  time_data        reset_lag;
  time_data        total_lag;

  struct timeval room_start;
  struct timeval room_extracted;
  struct timeval room_resetted;
  struct timeval room_flagged;
  struct timeval room_end;

  Room_Data( ) {
    record_new( sizeof( room_data ), MEM_ROOM );
    track        = NULL;
    action       = NULL;
    area         = NULL;
    reset        = NULL;
    vnum         = -1;
    name         = empty_string;
    description  = empty_string;
    comments     = empty_string;
    room_flags   = 0;
    sector_type  = 0;
    size         = SIZE_DINOSAUR;
    distance     = MAX_INTEGER;
    valid        = ROOM_DATA;
    reset_count  = 0;
    reset_lag    = 0;
    total_lag    = 0;
  }

  virtual ~Room_Data( ) {
    record_delete( sizeof( room_data ), MEM_ROOM );
    free_string( name, MEM_ROOM );
    free_string( description, MEM_ROOM );
    free_string( comments, MEM_ROOM );
  }

  virtual int Type    ( )        { return ROOM_DATA; }
          int Light   ( int );
  
  virtual void Reset( bool force = false );

  const char*  Location        ( content_array* = NULL );
  bool         is_dark         ( );
  bool         Seen            ( char_data* );
  bool         is_outside      ( );
  void         ShowInfo        ( char_data* );
  const char*  RoomFlagString  ( );
};


void  save_room_items  ( room_data* );
void  load_room_items  ( void );


/*
 *   TERRAIN HEADER
 */


#define SECT_INSIDE                 0  
#define SECT_CITY                   1
#define SECT_FIELD                  2
#define SECT_FOREST                 3
#define SECT_HILLS                  4
#define SECT_MOUNTAIN               5
#define SECT_WATER_SURFACE          6
#define SECT_UNDERWATER             7
#define SECT_RIVER                  8
#define SECT_AIR                    9
#define SECT_DESERT                10
#define SECT_CAVES                 11
#define SECT_ROAD                  12
#define SECT_SHALLOWS              13
#define SECT_MARSH                 14
#define SECT_REGEN                 15
#define SECT_BEACH                 16
#define SECT_ETHEREAL              17
#define SECT_SNOW                  18
#define MAX_TERRAIN                19


class Terrain_Type 
{
 public:
   const char*      name;
   int     movement_cost;
};


extern Terrain_Type terrain  [ MAX_TERRAIN ];


bool         water_logged   ( room_data* );
bool         deep_water     ( room_data* );
bool         is_submerged   ( char_data* );
room_data*   get_town       ( const char*, bool = FALSE);
room_data*   get_temple     ( char_data*, room_data* = NULL );
room_data*   get_portal     ( char_data* );
room_data*   get_death      ( char_data* );


