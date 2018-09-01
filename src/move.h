#define DIR_RANDOM                -1  // mostly used for dungeon
#define DIR_NORTH                  0
#define DIR_EAST                   1
#define DIR_SOUTH                  2
#define DIR_WEST                   3
#define DIR_UP                     4
#define DIR_DOWN                   5
#define DIR_EXTRA                  6

#define  MOVE_FLEE        -1
#define  MOVE_SLITHERS     0
#define  MOVE_SNEAK        1
#define  MOVE_WALK         2
#define  MOVE_FLY          3
#define  MOVE_SWIM         4
#define  MOVE_FLOAT        5
#define  MOVE_WADE         6
#define  MAX_MOVE          7

extern const char* leaving_verb [];
extern const char* leaving_action [];
extern const char* arriving_verb [];

class direction_type
{
 public:
  const char*       name;
  int            reverse;
  char*      arrival_msg;  
  char*            where;
};


extern direction_type  dir_table    [ ];


/*
 *   MOVEMENT
 */


void   move_char          ( char_data*, int, bool );
void   enter_water        ( char_data* );


extern const char*           leaving_msg  [ ];
extern const char*           scan_msg [ ];


/*
 *   PATHS
 */


class Path_Data
{
 public:
  char_data*  summoner;
  int             step;
  int           length;
  int*      directions;  

  Path_Data( );
  ~Path_Data( );
};


void   exec_range      ( char_data*, int, path_func*,
                         char* = empty_string );










