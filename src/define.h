/*
 *   GAME PARAMETERS
 */

#ifndef __DEFINE_H__
#define __DEFINE_H__

#ifdef TRUE
#undef TRUE
#undef FALSE
#endif

#define TRUE                                 1
#define FALSE                                0
#define MAX_INTEGER                      64000
#define M_PI            3.14159265358979323846
#define M_E              2.7182818284590452354

/* #define TFE_PORT_NUMBER         4000  */
/* #define TFE_DEBUG_PORT_NUMBER   4001  */

#define MAX_STRING_LENGTH       4096
#define MAX_INPUT_LENGTH         320
#define ONE_LINE                  80
#define TWO_LINES                160
#define THREE_LINES              240
#define FOUR_LINES               320
#define FIVE_LINES               400
#define SIX_LINES                480
#define EIGHT_LINES              640
  
#define MAX_PFILE               5000

#define UNLEARNT                   0


/*
 *  ROOM & MOB NUMBERS OF INTEREST
 */

#define ROOM_CONSTRUCT              1
#define ROOM_GENERATED         500000

#define MOB_BLANK                 133
#define MOB_SKULL_WATCH           111

#define OBJ_CORPSE_PC              11
#define OBJ_BALL_OF_LIGHT          21
#define OBJ_HOLY_FLAIL           3106
#define OBJ_UNHOLY_FLAIL         3111
#define OBJ_CAMPFIRE             3908
#define OBJ_CORPSE_LIGHT         3977
#define OBJ_DEAD_MAN_EYES        3999
#define OBJ_FIREFLY              4389
/*
 *   DEFINITIONS
 */

#define MAX_PLYR_RACE              20
#define MAX_SPELL_WAIT              5
#define MAX_ARMOR                   5
#define MAX_CFLAG                   4
#define MAX_INGRED                 20 
#define MAX_SPECIES              7500
#define MAX_TRAIN                  24
#define MAX_QUEST                 128
#define MAX_OBJ_INDEX           10000 
#define MAX_FAKE_MOB               29
#define MAX_TOOL                    3
#define MAX_CON_INGRED              5

#define COND_ALCOHOL                0
#define COND_FULL                   1
#define COND_THIRST                 2
#define COND_DRUNK                  3

#define CONT_CLOSEABLE              0
#define CONT_PICKPROOF              1
#define CONT_CLOSED                 2
#define CONT_LOCKED                 3
#define CONT_HOLDING                4
#define MAX_CONT                    5

#define FORMAT_NORMAL               0
#define FORMAT_BOLD                 1
#define FORMAT_REVERSE              2
#define FORMAT_UNDERLINE            3
#define FORMAT_RED                  4
#define FORMAT_GREEN                5
#define FORMAT_YELLOW               6
#define FORMAT_BLUE                 7
#define FORMAT_MAGENTA              8
#define FORMAT_CYAN                 9
#define FORMAT_WHITE               10
#define FORMAT_B_RED               11
#define FORMAT_B_GREEN             12
#define FORMAT_B_YELLOW            13
#define FORMAT_B_BLUE              14
#define FORMAT_B_MAGENTA           15
#define FORMAT_B_CYAN              16
#define FORMAT_B_WHITE             17
#define FORMAT_BL_RED              18
#define FORMAT_BL_GREEN            19 
#define FORMAT_BL_YELLOW           20
#define FORMAT_BL_BLUE             21
#define FORMAT_BL_MAGENTA          22
#define FORMAT_BL_CYAN             23
#define FORMAT_BL_WHITE            24
#define FORMAT_BB_RED                 25
#define FORMAT_BB_GREEN               26
#define FORMAT_BB_YELLOW              27
#define FORMAT_BB_BLUE                28
#define FORMAT_BB_MAGENTA             29
#define FORMAT_BB_CYAN                30
#define FORMAT_BB_WHITE               31
#define FORMAT_H_RED                  32
#define FORMAT_H_GREEN                33
#define FORMAT_H_YELLOW               34
#define FORMAT_H_BLUE                 35
#define FORMAT_H_MAGENTA              36
#define FORMAT_H_CYAN                 37
#define FORMAT_H_WHITE                38
#define MAX_FORMAT                    39


#define RACE_HUMAN                  0
#define RACE_ELF                    1
#define RACE_GNOME                  2
#define RACE_DWARF                  3    
#define RACE_HALFLING               4
#define RACE_ENT                    5
#define RACE_DUERGAR                6
#define RACE_LIZARD                 7
#define RACE_OGRE                   8
#define RACE_TROLL                  9
#define RACE_ORC                   10
#define RACE_GOBLIN                11
#define RACE_VYAN                  12
#define RACE_HALF_ELF              13
#define RACE_UNKNOWN_PLAYER1       14
#define RACE_UNKNOWN_PLAYER2       15
#define RACE_UNKNOWN_PLAYER3       16
#define RACE_UNKNOWN_PLAYER4       17
#define RACE_UNKNOWN_PLAYER5       18
#define RACE_UNKNOWN_PLAYER6       19
#define RACE_DEMON                 27
#define RACE_UNDEAD                35
#define RACE_FISH                  36
#define RACE_PLANT                 38
#define RACE_GOLEM                 40
#define RACE_CELESTIAL             51


#define REP_ATTACKED                0
#define REP_KILLED                  1
#define REP_STOLE_FROM              2


#define TO_ROOM                     0
#define TO_NOTVICT                  1
#define TO_ALL                      4
#define TO_ROOM_SEEN                5
#define TO_AREA                     6
#define TO_NOTVICT_SEEN             7
#define TO_VICT_SEEN                8

#define TRAP_POISON                 0
#define TRAP_BLIND                  1
#define MAX_TRAP                    2

#define QUEST_SACRIFICE             0
#define QUEST_KILL                  1
#define QUEST_GIVE                  2
#define QUEST_PRAY                  3
#define MAX_QUEST_TYPE              4

#define RES_STRENGTH               -2
#define RES_DEXTERITY              -1
#define RES_MAGIC                   0
#define RES_FIRE                    1
#define RES_COLD                    2
#define RES_SHOCK                   3
#define RES_MIND                    4
#define RES_ACID                    5
#define RES_POISON                  6
#define RES_HOLY                    7
#define MAX_RESIST                  8 

#define SEX_NEUTRAL                 0
#define SEX_MALE                    1
#define SEX_FEMALE                  2
#define SEX_RANDOM                  3
#define MAX_SEX                     4
 

#define SIZE_ANT                    0
#define SIZE_RAT                    1
#define SIZE_DOG                    2
#define SIZE_GNOME                  3
#define SIZE_HUMAN                  4
#define SIZE_OGRE                   5
#define SIZE_HORSE                  6
#define SIZE_GIANT                  7
#define SIZE_ELEPHANT               8
#define SIZE_DINOSAUR               9
#define MAX_SIZE                   10


#define SKY_CLOUDLESS               0
#define SKY_CLOUDY                  1
#define SKY_RAINING                 2
#define SKY_LIGHTNING               3

#define STAT_PET                    0
#define STAT_IN_GROUP               1
#define STAT_FAMILIAR               2
#define STAT_LEAPING                3
#define STAT_FOLLOWER               4
#define STAT_STUNNED                5 
#define STAT_POLYMORPH              6
#define STAT_ALERT                  7
#define STAT_TAMED                  8
#define STAT_BERSERK                9
#define STAT_GAINED_EXP            10
#define STAT_SENTINEL              11
#define STAT_REPLY_LOCK            12
#define STAT_AGGR_ALL              13
#define STAT_AGGR_GOOD             14
#define STAT_AGGR_EVIL             15
#define STAT_ORDERED               16
#define STAT_HIDING                17
#define STAT_SNEAKING              18
#define STAT_WIMPY                 19
#define STAT_PASSIVE               20
#define STAT_FORCED                21
#define STAT_CAMOUFLAGED           22
#define STAT_STUDYING              23
#define STAT_AGGR_NEUT             24
#define MAX_STAT                   25

#define DANCE_DRAGONFLY             0
#define DANCE_SPRING                1
#define DANCE_MISTY_DAWN            2
#define DANCE_DRAKE                 3
#define DANCE_SUMMER                4
#define DANCE_FIREFLY               5
#define DANCE_WINTER                6
#define DANCE_STORMS                7
#define DANCE_STORMLORD             8
#define DANCE_STORMMASTER           9
#define DANCE_FURY                 10
#define DANCE_BATTLEFIEND          11
#define DANCE_DESTRUCTION          12
#define DANCE_ARMAGEDDON           13
#define DANCE_H_STRIKES            14
#define DANCE_T_STRIKES            15
#define DANCE_THORNS               16
#define DANCE_SHARDS               17
#define DANCE_ELEMENTS             18
#define DANCE_MIRROR_IMAGE         19
#define DANCE_WHIRLING_SWORDS      20

#define MAX_DANCE                  32

#define SUN_DARK                    0
#define SUN_RISE                    1
#define SUN_LIGHT                   2
#define SUN_SET                     3 

#define UPDATE_CAST                 0
#define UPDATE_CONCOCT              1
#define UPDATE_POISON               2
#define UPDATE_SERVICE              3
#define UPDATE_SONG                 4
#define UPDATE_SCRIBE               5
#define UPDATE_WAND                 6
#define UPDATE_FLETCH               7

#define PORTAL_NEW_CONT  "/Cayln/NewCont/PortalFound"
#define CUSTOM_FIT       "/hardcode/custom_fit"
#define FAMILIAR_DEATH   "/hardcode/familiar_death"
#define GOOD_BERRY       "/hardcode/goodberry/skill"
#define HARVEST_ATTEMPT  "/hardcode/harvestattempt"
#define CASH_BOOST       "/hardcode/cash_boost"

/*
 *   MACROS FOR PRIVATE VARIABLES
 */

extern char *str_dup(const char *source);

#define SET(type, variable)                 virtual void set_##variable(type value)  { variable = value; }
#define SET_ALLOC(type, variable)           virtual void set_##variable(type *value) { if (variable) delete variable;    variable = value; }
#define SET_CHAR(variable)                  virtual void set_##variable(const char *value) { if (variable) delete [] variable; variable = (value && *value ? str_dup(value) : NULL); }
#define SET_BOOL(variable)                  SET(bool, variable)
#define SET_INT(variable)                   SET(int, variable)

#define GET(type, variable)                 virtual type get_##variable() { return variable; }
#define GET_CHAR(variable)                  GET(char *, variable)
#define GET_BOOL(variable)                  GET(bool, variable)
#define GET_INT(variable)                   GET(int, variable)

#define VARIABLE(type, variable)            protected: type  variable; public: SET(type, variable)        GET(type, variable)    protected:
#define VARIABLE_POINTER(type, variable)    protected: type *variable; public: SET(type *, variable)      GET(type *, variable)  protected:
#define VARIABLE_ALLOC(type, variable)      protected: type *variable; public: SET_ALLOC(type, variable)  GET(type *, variable)  protected:
#define VARIABLE_CHAR(variable)             protected: char *variable; public: SET_CHAR(variable)         GET_CHAR(variable)     protected:
#define VARIABLE_BOOL(variable)             protected: bool  variable; public: SET_BOOL(variable)         GET_BOOL(variable)     protected:
#define VARIABLE_INT(variable)              protected: int   variable; public: SET_INT(variable)          GET_INT(variable)      protected:

#define SET_MEM_CHAR(variable,memory)       virtual void set_##variable(const char *value) { if( variable ) free_string( variable, memory ); variable = (value && *value ? alloc_string( value, memory ) : NULL); }
#define VARIABLE_MEM_CHAR(variable,memory)  protected: char *variable; public: SET_MEM_CHAR(variable,memory)  GET_CHAR(variable)     protected:

/*
 *   DEFINED FUNCTIONS
 */

#define ABS( a )                 ( a < 0 ? -a : a )  
#define UMIN( a, b )             ( a < b ? a : b )
#define UMAX( a, b )             ( a > b ? a : b )
#define URANGE( a, b, c )        ( b < a ? a : ( b > c ) ? c : b )

#define IS_AFFECTED( ch, bit )   ( is_set( ch->affected_by, bit ) )

#define IS_NPC( ch )             ( ch->species != NULL )

#define IS_DRUNK( ch )           ( !IS_NPC( ch ) && ch->pcdata->condition[ COND_DRUNK ] > 10 )


#endif /* __DEFINE.H__ */
