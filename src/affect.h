#define APPLY_NONE                  0
#define APPLY_STR                   1
#define APPLY_DEX                   2
#define APPLY_INT                   3
#define APPLY_WIS                   4
#define APPLY_CON                   5
#define APPLY_MAGIC                 6
#define APPLY_FIRE                  7
#define APPLY_COLD                  8
#define APPLY_ELECTRIC              9
#define APPLY_MIND                 10
#define APPLY_AGE                  11 
#define APPLY_MANA                 12
#define APPLY_HIT                  13
#define APPLY_MOVE                 14
 // unused?
 // unused?
#define APPLY_AC                   17
#define APPLY_HITROLL              18
#define APPLY_DAMROLL              19
#define APPLY_MANA_REGEN           20
#define APPLY_HIT_REGEN            21
#define APPLY_MOVE_REGEN           22
#define APPLY_ACID                 23
#define APPLY_POISON               24
#define APPLY_HOLY                 25
#define APPLY_PHYSICAL_DAM         26
#define APPLY_FIRE_DAM             27
#define APPLY_COLD_DAM             28
#define APPLY_ACID_DAM             29
#define APPLY_SHOCK_DAM            30
#define APPLY_MIND_DAM             31
#define APPLY_MAGIC_DAM            32
#define APPLY_POISON_DAM           33
#define APPLY_HOLY_DAM             34
#define APPLY_UNHOLY_DAM           35
#define MAX_AFF_LOCATION           36   // DO NOT INCREASE MAX_AFF_LOCATION WITHOUT ADDING AN ITEM TO const char* affect_location[] IN affect.cpp!


#define AFFECT_INTS                10
#define MAX_AFF_MODS                4

class Affect_Data
{
 public:
  char_data*    leech;
  char_data*    victim;
  int           type;
  int           duration;
  int           level;
  int           mlocation[ MAX_AFF_MODS ];  // i hate doing max_anythings but it seems the easiest way to do it :P
  int           mmodifier[ MAX_AFF_MODS ];
  int           leech_regen;
  int           leech_max;

  Affect_Data( );
  ~Affect_Data( );
};


extern const char* fake_mob [ MAX_FAKE_MOB ];

void   add_affect         ( char_data*, affect_data*, bool = TRUE );
void   add_affect         ( obj_data*, affect_data* );
int    affect_level       ( char_data*, int ); 
int    affect_duration    ( char_data*, int ); 
bool   has_affect         ( char_data*, int );
bool   is_affected        ( obj_data*, int );
void   strip_affect       ( char_data*, int );
bool   strip_affect       ( obj_data*, int );
void   remove_affect      ( char_data* );
void   remove_affect      ( char_data*, affect_data*, bool = TRUE );
void   remove_affect      ( obj_data*, affect_data*, bool = TRUE );
void   read_affects       ( FILE*, obj_clss_data*, bool );
void   read_affects       ( FILE*, obj_data*, bool );
void   read_affects       ( FILE*, char_data*, bool );
void   write_affects      ( FILE*, obj_clss_data* );
void   write_affects      ( FILE*, obj_data* );
void   write_affects      ( FILE*, char_data* );
