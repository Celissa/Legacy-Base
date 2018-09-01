#define SFLAG_CUSTOM                0
#define SFLAG_SIZE                  1
#define SFLAG_RACE                  2
#define SFLAG_RANDOM                3
#define SFLAG_TINY                  4
#define SFLAG_SMALL                 5
#define SFLAG_MEDIUM                6
#define SFLAG_LARGE                 7
#define SFLAG_GIANT                 8
#define SFLAG_HUMAN                 9
#define SFLAG_ELF                  10
#define SFLAG_GNOME                11
#define SFLAG_DWARF                12    
#define SFLAG_HALFLING             13
#define SFLAG_ENT                  14
#define SFLAG_DUERGAR              15
#define SFLAG_LIZARD               16
#define SFLAG_OGRE                 17 
#define SFLAG_TROLL                18 
#define SFLAG_ORC                  19 
#define SFLAG_GOBLIN               20 
#define SFLAG_VYAN                 21  
#define SFLAG_RELIGION             22
#define MAX_SFLAG                  23


#define WEAR_NONE                  -1  // WEAR_NONE
#define WEAR_FLOATING               0  // WEAR_FLOATING
#define WEAR_HEAD                   1  // WEAR_FINGER_R
#define WEAR_NECK                   2  // WEAR_FINGER_L
#define WEAR_BODY                   3  // WEAR_NECK
#define WEAR_WAIST                  4  // WEAR_UNUSED0
#define WEAR_ARMS                   5  // WEAR_BODY
#define WEAR_WRIST_R                6  // WEAR_HEAD
#define WEAR_WRIST_L                7  // WEAR_LEGS
#define WEAR_HANDS                  8  // WEAR_FEET
#define WEAR_FINGER_R               9  // WEAR_HANDS
#define WEAR_FINGER_L              10  // WEAR_ARMS
#define WEAR_JEWELRY               11  // WEAR_SHIELD
#define WEAR_LEGS                  12  // WEAR_ABOUT
#define WEAR_FEET                  13  // WEAR_WAIST
#define WEAR_HELD_R                14  // WEAR_WRIST_R
#define WEAR_HELD_L                15  // WEAR_WRIST_L
#define WEAR_ABOUT                 16  // WEAR_HELD_R
#define WEAR_UNUSED1               17  // WEAR_HELD_L
#define WEAR_UNUSED2               18  // WEAR_UNKNOWN0
#define WEAR_UNUSED3               19  // WEAR_UNKNOWN1
#define WEAR_UNUSED4               20  // WEAR_UNKNOWN2
#define WEAR_UNUSED5               21  // WEAR_UNKNOWN3
#define WEAR_HORSE_BODY            22  // WEAR_HORSE_BODY
#define WEAR_HORSE_BACK            23  // WEAR_HORSE_BACK
#define WEAR_HORSE_FEET            24  // WEAR_HORSE_FEET
#define MAX_WEAR_HUMANOID          17  // 18
#define MAX_WEAR                   25  // 25

#define LAYER_BOTTOM                0
#define LAYER_UNDER                 1
#define LAYER_BASE                  2
#define LAYER_OVER                  3
#define LAYER_TOP                   4
#define MAX_LAYER                   5


extern const int         wear_index  [ MAX_WEAR ];
extern const int      old_wear_locs  [ MAX_WEAR ];
extern const int        old_mresets  [ MAX_WEAR ];
extern const char*  reset_wear_name  [ ];
extern const char*      wear_abbrev  [ ];
extern const char*        wear_name  [ ];
extern const char*       layer_name  [ ];
extern const char*        wear_verb  [ ];
extern const char**  wear_part_name;


/*
 *   WEAR ROUTINES
 */


bool         can_use     ( char_data*, obj_clss_data*, obj_data*,
                           bool = FALSE );
void         equip       ( char_data*, obj_data* );
void         unequip     ( char_data*, obj_data* );
void         wear        ( char_data*, obj_data* );
void         list_wear   ( char_data*, thing_array* );
const char*  wear_loc    ( obj_data* );


thing_func   needremove;
thing_func   wrongrelig;

/*
 *   INLINE ROUTINES
 */


inline int wear_size( char_data* ch )
{
  return SFLAG_TINY-SIZE_DOG+range( SIZE_DOG, ch->Size( ), SIZE_GIANT );
}


inline bool can_wear( obj_data* obj, int part )
{
  return is_set( &obj->pIndexData->wear_flags, part );
}
