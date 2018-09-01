/* 
 *   OBJ_DATA CLASS
 */


#define ANTI_HUMAN                  0
#define ANTI_ELF                    1
#define ANTI_GNOME                  2
#define ANTI_DWARF                  3
#define ANTI_HALFLING               4
#define ANTI_ENT                    5
#define ANTI_DUERGAR                6
#define ANTI_LIZARD                 7
#define ANTI_OGRE                   8
#define ANTI_TROLL                  9
#define ANTI_ORC                   10
#define ANTI_GOBLIN                11
#define ANTI_VYAN                  12
#define ANTI_HALF_ELF              13
#define ANTI_MAGE                  14
// cleric  15
// thief   16
// warrior 17
// paladin 18
// ranger  19
// dancer  20
// monk    21
// druid   22
#define ANTI_UNUSED1               23
#define ANTI_UNUSED2               24
#define ANTI_UNUSED3               25
#define ANTI_GOOD                  26
#define ANTI_NEUTRAL               27
#define ANTI_EVIL                  28
#define ANTI_LAWFUL                29
#define ANTI_CHAOTIC               31
#define MAX_ANTI                   32


#define ITEM_TAKE                   0  // ITEM_TAKE
#define ITEM_WEAR_FLOATING          1  // ITEM_WEAR_FINGER
#define ITEM_WEAR_HEAD              2  // ITEM_WEAR_NECK
#define ITEM_WEAR_NECK              3  // ITEM_WEAR_BODY
#define ITEM_WEAR_BODY              4  // ITEM_WEAR_HEAD
#define ITEM_WEAR_WAIST             5  // ITEM_WEAR_LEGS 
#define ITEM_WEAR_ARMS              6  // ITEM_WEAR_FEET
#define ITEM_WEAR_WRIST             7  // ITEM_WEAR_HANDS
#define ITEM_WEAR_HANDS             8  // ITEM_WEAR_ARMS
#define ITEM_WEAR_FINGER            9  // ITEM_UNUSED2
#define ITEM_WEAR_JEWELRY          10  // ITEM_UNUSED3
#define ITEM_WEAR_LEGS             11  // ITEM_WEAR_WAIST
#define ITEM_WEAR_FEET             12  // ITEM_WEAR_WRIST
#define ITEM_HELD_R                13  // ITEM_HELD_R
#define ITEM_HELD_L                14  // ITEM_HELD_L
#define ITEM_WEAR_ABOUT            15  // ITEM_WEAR_UNUSED0
#define ITEM_WEAR_UNUSED1          16  // ITEM_WEAR_UNUSED1
#define ITEM_WEAR_UNUSED2          17  // ITEM_WEAR_UNUSED2
#define ITEM_WEAR_UNUSED3          18  // ITEM_WEAR_UNUSED3
#define ITEM_WEAR_UNUSED4          19  // ITEM_WEAR_HORSE_BODY
#define ITEM_WEAR_HORSE_BODY       20  // ITEM_WEAR_HORSE_BACK
#define ITEM_WEAR_HORSE_BACK       21  // ITEM_WEAR_HORSE_FEET
#define ITEM_WEAR_HORSE_FEET       22     
#define MAX_ITEM_WEAR              23  // MAX_ITEM_WEAR


#define ITEM_LIGHT                  1
#define ITEM_SCROLL                 2
#define ITEM_WAND                   3
#define ITEM_STAFF                  4
#define ITEM_WEAPON                 5
#define ITEM_GEM                    6
#define ITEM_SPELLBOOK              7
#define ITEM_TREASURE               8
#define ITEM_ARMOR                  9
#define ITEM_POTION                10
#define ITEM_REAGENT               11 
#define ITEM_FURNITURE             12
#define ITEM_TRASH                 13
#define ITEM_CROSS                 14 
#define ITEM_CONTAINER             15
#define ITEM_LOCK_PICK             16 
#define ITEM_DRINK_CON             17
#define ITEM_KEY                   18
#define ITEM_FOOD                  19
#define ITEM_MONEY                 20
#define ITEM_KEYRING               21
#define ITEM_BOAT                  22
#define ITEM_CORPSE                23
#define ITEM_BAIT                  24
#define ITEM_FOUNTAIN              25
#define ITEM_WHISTLE               26
#define ITEM_TRAP                  27
#define ITEM_LIGHT_PERM            28
#define ITEM_BANDAGE               29
#define ITEM_BOUNTY                30
#define ITEM_GATE                  31
#define ITEM_ARROW                 32 
#define ITEM_SKIN                  33
#define ITEM_BODY_PART             34
#define ITEM_CHAIR                 35
#define ITEM_TABLE                 36
#define ITEM_BOOK                  37
#define ITEM_PIPE                  38
#define ITEM_TOBACCO               39
#define ITEM_DECK_CARDS            40
#define ITEM_FIRE                  41
#define ITEM_GARROTE               42
#define ITEM_SHIELD                43
#define ITEM_HERB                  44
#define ITEM_FLOWER                45
#define ITEM_JEWELRY               46
#define ITEM_ORE                   47
#define ITEM_TREE                  48
#define ITEM_PLANT                 49
#define ITEM_CHARM                 50
#define ITEM_CHARM_BRACELET        51
#define ITEM_TOY                   52
#define ITEM_CLOTHING              53
#define ITEM_DRAGON_SCALE          54
#define ITEM_STONE                 55
#define ITEM_BOLT                  56
#define ITEM_SEARCHABLE_CORPSE     57
#define ITEM_MUSICAL_INSTRUMENT    58
#define ITEM_POISON                59
#define ITEM_ANCIENT_OAK           60
#define ITEM_STATUE                61
#define MAX_ITEM                   62

#define MAT_PAPER                   0
#define MAT_WOOD                    1
#define MAT_LEATHER                 2
#define MAT_GEM_STONE               3
#define MAT_CLOTH                   4
#define MAT_GLASS                   5
#define MAT_STONE                   6
#define MAT_BONE                    7
#define MAT_FLESH                   8
#define MAT_ORGANIC                 9
#define MAT_METAL                  10
#define MAT_BRONZE                 11
#define MAT_IRON                   12
#define MAT_STEEL                  13
#define MAT_MITHRIL                14
#define MAT_LIRIDIUM               15
#define MAT_ELECTRUM               16
#define MAT_SILVER                 17
#define MAT_GOLD                   18
#define MAT_COPPER                 19
#define MAT_PLATINUM               20
#define MAT_KRYNITE                21
#define MAT_FIRE                   22
#define MAT_COLD                   23
#define MAT_SHOCK                  24
#define MAT_ACID                   25 
#define MAX_MATERIAL               26

#define OFLAG_GLOW                  0
#define OFLAG_HUM                   1
#define OFLAG_DARK                  2
#define OFLAG_LOCK                  3
#define OFLAG_EVIL                  4
#define OFLAG_IS_INVIS              5
#define OFLAG_MAGIC                 6
#define OFLAG_NODROP                7
#define OFLAG_SANCT                 8
#define OFLAG_FLAMING               9
#define OFLAG_BACKSTAB             10
#define OFLAG_NO_DISARM            11
#define OFLAG_NOREMOVE             12
#define OFLAG_INVENTORY            13
#define OFLAG_NO_SHIELD            14
#define OFLAG_NO_MAJOR             15
#define OFLAG_NOSHOW               16
#define OFLAG_NOSACRIFICE          17
#define OFLAG_WATER_PROOF          18
#define OFLAG_APPRAISED            19
#define OFLAG_NO_SELL              20
#define OFLAG_NO_JUNK              21
#define OFLAG_IDENTIFIED           22
#define OFLAG_RUST_PROOF           23
#define OFLAG_BODY_PART            24 
#define OFLAG_CHAIR                25
#define OFLAG_NOSAVE               26
#define OFLAG_BURNING              27
#define OFLAG_ADDITIVE             28
#define OFLAG_GOOD                 29
#define OFLAG_THE                  30
#define OFLAG_REPLICATE            31
#define OFLAG_KNOWN_LIQUID         32
#define OFLAG_POISON_COATED        33
#define OFLAG_NO_AUCTION           34
#define OFLAG_NO_ENCHANT           35
#define OFLAG_COPIED               36
#define OFLAG_RANDOM_METAL         37
#define OFLAG_COVER                38
#define OFLAG_APPROVED             39
#define OFLAG_NO_PREFIX            40
#define OFLAG_BLACKJACK            41
#define OFLAG_DISEASED             42
#define OFLAG_WEAPON_BLIGHT        43
#define MAX_OFLAG                  44  // if you update this number you must update oflag_name and oflag_ident in obj_imm.cpp

#define RESTR_BLADED                0
#define RESTR_NO_HIDE               1
#define RESTR_NO_SNEAK              2
#define RESTR_DISHONORABLE          3
#define MAX_RESTRICTION             4


/*
 *   OBJ_CLSS_DATA 
 */


extern obj_clss_data* obj_index_list [ MAX_OBJ_INDEX ];

/*
 *   SYNERGY
 */

class Synergy
{
 public:
   const char*   skill;
   int            clss;
   int          amount;

   // save.load
   bool Save( FILE* );
   bool Load( FILE* );

   Synergy( );
   ~Synergy( );
};


class Obj_Clss_Data : public Template_Data
{
 public:
  affect_array            affected;
  extra_array          extra_descr;
  oprog_data*                oprog;
  Array< Synergy* >  synergy_array;
  char*                   singular;
  char*                     plural;
  char*                     before;
  char*                      after;
  char*            prefix_singular;
  char*              prefix_plural;
  char*                     long_s;  
  char*                     long_p;
  char*                    creator;
  char*                   last_mod;
  time_t                      date;
//  int                     vnum;
  int                        fakes;
  int                    item_type;
  int                  extra_flags  [ 2 ];
  int                   size_flags;
  int                 restrictions;
  int                   anti_flags;
  int                    materials;
  int                   wear_flags;
  int                  layer_flags;
  int                 affect_flags  [ AFFECT_INTS ];
  int                        count;
  int                        limit;
  int                       weight;
  int                         cost;
  int                        level;
  int                        value  [ 4 ];
  int                       repair;
  int                   durability;   
  int                       blocks;
  int                        light;
  int                 clss_synergy [ MAX_NEW_CLSS ];
  int                        color;
  int               religion_flags;

  // edit handlers for synergies
  void HandleSynergyEdit   ( wizard_data* ch, char* argument );
  void HandleSynergyStat   ( wizard_data* ch );
  void HandleSynergySet    ( wizard_data* ch, char* argument );

  void DisplaySynergyList  ( wizard_data* ch );

  Obj_Clss_Data   ( );
  ~Obj_Clss_Data  ( );
  Obj_Clss_Data  ( const char*, int, const char* );
  Obj_Clss_Data  ( obj_clss_data*, const char*, int, const char* );

  const char*  Name      ( );
  const char*  Name      ( int );
  const char*  Keywords  ( );

  int          metal         ( );
  int          affect_levels ( );
};


bool   can_extract    ( obj_clss_data*, char_data* );
void   load_objects   ( void );
void   junk_obj       ( char_data*, obj_data* );
bool   is_container   ( obj_data* obj );

const char*  name_before   ( obj_clss_data* );
const char*  name_brief    ( obj_clss_data*, int = 1 );
const char*  after_descr   ( obj_clss_data* );
const char*  before_descr  ( obj_clss_data* );


/*
 *   OBJ_DATA 
 */


extern obj_array obj_list;
extern obj_array corpse_list;

class Obj_Data : public Thing_Data
{
 public :
  affect_array         affected;

  extra_data*       extra_descr;
  obj_clss_data*     pIndexData;
  player_data*             save;
  char*                  source;
  char*                   label;
  char*                singular;
  char*                  plural;
  char*                  before;
  char*                   after;
  pfile_data*             owner;
  int               extra_flags  [ 2 ];
  int                size_flags;
  int                 materials;
  int                    weight;
  int                     value  [ 4 ];
  int                     timer;
  int                 condition;
  int                      rust;
  int                       age;  
  int                     layer;
  int                  unjunked; // counter of junkings
  int            reset_mob_vnum;
  int           reset_room_vnum;
  int             reset_chances  [ 3 ];

  Obj_Data( obj_clss_data* );
  virtual ~Obj_Data( );

  /* BASIC */

  int   Type     ( )       { return OBJ_DATA; }
  void  Extract  ( );
  void  Extract  ( int );

  /* NAME/KEYWORDS */

  const char*  Keywords        ( char_data* );
  const char*  Name            ( char_data* = NULL, int = 1, bool = FALSE );
  const char*  Seen_Name       ( char_data*, int = 1, bool = FALSE );
  const char*  Show            ( char_data*, int = 1 );
  const char*  condition_name  ( char_data*, bool = FALSE );
  const char*  repair_condition_name  ( char_data*, bool = FALSE );
  const char*  Location        ( content_array* = NULL );
  void         Look_At         ( char_data* ); 
  bool         Seen            ( char_data* );
  char*  Spaces          ( char_data* );
  
  /* TO/FROM */

  void         To              ( content_array* );
  void         To              ( thing_data* );
  thing_data*  From            ( int = 1 );

  /* PROPERTIES */

  int          Weight          ( int = -1 );
  int          Light           ( int = -1 );
  int          Empty_Weight    ( int = -1 );
  int          Number          ( int = -1 );
  int          Capacity        ( );
  int          Empty_Capacity  ( );
  bool         Damaged         ( );
  int          Cost            ( );
  bool         Belongs         ( char_data* );

  bool junkable( )  { return !is_set( extra_flags, OFLAG_NO_JUNK ); }
  bool droppable( ) { return !is_set( extra_flags, OFLAG_NODROP ); }
  bool removable( ) { return !is_set( extra_flags, OFLAG_NOREMOVE ); } 
  bool paper( )     { return  is_set( &materials, MAT_PAPER ); }
  bool glass( )     { return  is_set( &materials, MAT_GLASS ); }
  bool wood( )      { return  is_set( &materials, MAT_WOOD ); }
  bool stone( )     { return  is_set( &materials, MAT_STONE ); }

  int  metal                ( ); 
  int  mana_absorbing_metal ( );
  bool is_cloth             ( );
  bool is_leather           ( );

  /* SAVING THROWS */

  int vs_fire ( );
  int vs_acid ( );
  int vs_cold ( );
  int vs_shock ( );
};


/*
 *   FUNCTIONS
 */


obj_data*    create             ( obj_clss_data*, int = 1 );
obj_data*    duplicate          ( obj_data*, int = 1 );

void         set_owner          ( obj_data*, char_data*, char_data* );
void         set_owner          ( obj_data*, pfile_data* );
void         set_owner          ( pfile_data*, thing_array&, bool = FALSE );
  
bool         read_object        ( FILE*, content_array&, char* );
void         write_object       ( FILE*, content_array& );

bool         is_same            ( obj_data*, obj_data* );

int          armor_class        ( obj_data* );
void         rust_object        ( obj_data*, int );
void         condition_abbrev   ( char*, obj_data*, char_data* );
void         age_abbrev         ( char*, obj_data*, char_data* );
bool         newbie_abuse       ( char_data* );
const char*  light_name         ( int );

void         open_object        ( char_data*, obj_data* );
void         close_object       ( char_data*, obj_data* );


/*
 *   OBJECT ARGUMENTS
 */


obj_data*    find_type         ( thing_array&, int ); 
obj_data*    find_vnum         ( thing_array&, int ); 
obj_data*    find_vnum_value   ( thing_array&, int, int, int );

/*
 *   ARRAY ROUTINES
 */


void         rehash             ( char_data*, thing_array& );
void         rehash_weight      ( char_data*, thing_array& );
void         sort_objects       ( char_data*, thing_array&, thing_data*, int,
                                  thing_array*, thing_func** );

void         page_publ          ( char_data*, thing_array*, const char*,
                                  thing_data* = NULL,
                                  const char* = empty_string,
                                  const char* = empty_string, bool = true );
void         page_priv          ( char_data*, thing_array*, const char*,
                                  thing_data* = NULL,
                                  const char* = empty_string,
                                  const char* = empty_string );
void         send_priv          ( char_data*, thing_array*, const char*,
                                  thing_data* );
void         send_publ          ( char_data*, thing_array*, const char*,
                                  const char* );

thing_func      stolen;
thing_func     no_junk;
thing_data* corpse( thing_data* thing, char_data* ch, thing_data* = NULL );
thing_func      cursed;
thing_func        same;
thing_func    wont_fit;
thing_func   cant_take;
thing_func      sat_on;
thing_func        many;
thing_func       heavy;
thing_data* forbidden( thing_data* thing, char_data* ch, thing_data* = NULL );
thing_func      gotten;
thing_func         put;
thing_func        drop;
thing_func       paper;
thing_func       glass;
thing_func     no_show;

extern const char* unlock_msg [];
extern const char* lock_msg [];


/*
 *   INLINE UTILITIES
 */


inline obj_clss_data* get_obj_index( int vnum )
{
  if( vnum < 0 || vnum >= MAX_OBJ_INDEX ) 
    return NULL;

  return obj_index_list[vnum];
}


inline int repair_condition( obj_data* obj )
{
  return obj->pIndexData->durability-5*obj->age;
};


/*
 *   FOOD ROUTINES
 */


extern const char* cook_word [];


bool eat    ( char_data*, obj_data* );
bool drink  ( char_data*, obj_data* );


/*
 *   NAME ROUTINES
 */


extern bool include_empty;
extern bool include_liquid;
extern bool include_closed;


/*
 *   OBJECT TRANSFER ROUTINES
 */


int      drop_contents      ( obj_data* );


/*
 *   MONEY ROUTINES
 */


int   monetary_value    ( obj_data* );
void  split_money       ( char_data*, int, bool );
void  add_coins         ( char_data*, int, const char* = NULL );


/*
 *   Inventory Utils
 */ 

obj_data *get_bag(char_data *ch, int slot);
obj_data *get_cross(char_data *ch, int slot);
obj_data *get_weapon(char_data *ch, int slot);
obj_data *get_shield(char_data *ch, int slot);
