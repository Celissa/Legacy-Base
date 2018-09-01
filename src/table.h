

/*
 *   LOCAL DEFINITIONS
 */


#define MAX_SOCIAL                500
#define MAX_SPELL_ACT             500
#define MAX_LIQUID                150
#define MAX_TOWN                   25
#define MAX_GROUP                 500
#define MAX_RACE                  180
#define MAX_AFF_CHAR              320
#define MAX_AFF_OBJ                50
#define MAX_COMMAND               400
#define MAX_CMD_CAT                30
#define MAX_CLSS                   10

#define MAX_BUILD                  50
#define MAX_HELP_CAT               50
#define MAX_NATION                 32
#define MAX_ASTRAL                 25
#define MAX_TREE                   75
#define MAX_RELIGION               20
#define MAX_ALIGNMENT               9
#define MAX_STARTING       MAX_CLSS + MAX_PLYR_RACE + 1
#define MAX_FIELD                  48
#define MAX_SHAPE_CHANGE          200
#define MAX_FAMILIAR               60
#define MAX_POISON                200
#define MAX_CREATE_FOOD           150
#define MAX_FORAGE                 60
#define MAX_HARVEST               200
#define MAX_CONCOCT               200
#define MAX_TAX_ENTRIES            50
#define MAX_TERRAINS               50
#define MAX_BONUS                 100
#define MAX_SERVICES               32
#define MAX_NEW_SKILL             200
#define MAX_WEAPON_STRING          20
#define MAX_MOB_DIFF               40

#define TABLE_SOC_DEFAULT           0
#define TABLE_SOC_HUMAN             1
#define TABLE_SOC_ELF               2
#define TABLE_SOC_GNOME             3
#define TABLE_SOC_DWARF             4
#define TABLE_SOC_HALFLING          5
#define TABLE_SOC_ENT               6
#define TABLE_SOC_DUERGAR           7
#define TABLE_SOC_LIZARD            8
#define TABLE_SOC_OGRE              9
#define TABLE_SOC_TROLL            10
#define TABLE_SOC_ORC              11
#define TABLE_SOC_GOBLIN           12
#define TABLE_SOC_VYAN             13
#define TABLE_SOC_HALF_ELF         14
#define TABLE_SOC_NEW_RACE         15
#define TABLE_SOC_NEW_RACE1        16
#define TABLE_SOC_NEW_RACE2        17
#define TABLE_SOC_NEW_RACE3        18
#define TABLE_SOC_NEW_RACE4        19
#define TABLE_SOC_NEW_RACE5        20
#define TABLE_SPELL_ACT            21
#define TABLE_LIQUID               22
#define TABLE_SPELL_DATA           23
#define TABLE_TOWN                 24
#define TABLE_SKILL                25
#define TABLE_METAL                26
#define TABLE_NATION               27
#define TABLE_GROUP                28
#define TABLE_RACE                 29
#define TABLE_PLYR_RACE            30
#define TABLE_AFF_CHAR             31
#define TABLE_AFF_OBJ              32
#define TABLE_COMMAND              33
#define TABLE_CMD_CAT              34
#define TABLE_CLSS                 35
#define TABLE_STARTING             36
#define TABLE_TEDIT                37
#define TABLE_BUILD                38
#define TABLE_HELP_CAT             39
#define TABLE_ASTRAL               40
#define TABLE_RELIGION             41
#define TABLE_ALIGNMENT            42
#define TABLE_TREE_GATE            43
#define TABLE_SHAPE_CHANGE         44
#define TABLE_ANIMAL_COMPANION     45
#define TABLE_WALK_DAMNED          46
#define TABLE_SUMMON_CADAVER       47
#define TABLE_SUMMON_DRAGON        48
#define TABLE_RETURN_DEAD          49
#define TABLE_BANE_FIEND           50
#define TABLE_BLOOD_FIEND          51
#define TABLE_SUMMON_UNDEAD        52
#define TABLE_LESSER_MOUNT         53
#define TABLE_ANIMATE_CLAY         54
#define TABLE_CONSTRUCT_GOLEM      55
#define TABLE_CONJURE_ELEMENTAL    56
#define TABLE_FIND_MOUNT           57
#define TABLE_REQUEST_ALLY         58
#define TABLE_FIND_FAMILIAR        59
#define TABLE_LESSER_FAMILIAR      60
#define TABLE_POISON_DATA          61
#define TABLE_CREATE_FOOD          62
#define TABLE_FORAGE               63
#define TABLE_TINDER               64
#define TABLE_HARVEST              65
#define TABLE_CONCOCT              66
#define TABLE_DANCE                67
#define TABLE_TAX                  68
#define TABLE_LICHDOM              69
#define TABLE_TERRAIN              70
#define TABLE_BONUS                71
#define TABLE_SERVICES             72
#define TABLE_ROGUE_SKILL          73
#define TABLE_ASSASSIN_SKILL       74
#define TABLE_CAVALIER_SKILL       75
#define TABLE_BARBARIAN_SKILL      76
#define TABLE_CORE_SKILL           77 // everyone gets these skills
#define TABLE_UNARMED_WORDS        78
#define TABLE_DAGGER_WORDS         79
#define TABLE_SWORD_WORDS          80
#define TABLE_CLUB_WORDS           81
#define TABLE_STAFF_WORDS          82
#define TABLE_POLEARM_WORDS        83
#define TABLE_MACE_WORDS           84
#define TABLE_WHIP_WORDS           85
#define TABLE_AXE_WORDS            86
#define TABLE_BOW_WORDS            87
#define TABLE_SPEAR_WORDS          88
#define TABLE_XBOW_WORDS           89
#define TABLE_SLING_WORDS          90
#define TABLE_AFF_LOCATION         91
#define TABLE_OFLAG                92
#define TABLE_MOB_DIFFICULTY       93
#define TABLE_SCRIBE               94
#define TABLE_CREATE_WAND          95
#define TABLE_FLETCHERY            96
#define MAX_TABLE                  97

#define MAX_ENTRY_FLETCHERY        table_max[ TABLE_FLETCHERY ]
#define MAX_ENTRY_SCRIBE           table_max[ TABLE_SCRIBE ]
#define MAX_ENTRY_CREATE_WAND      table_max[ TABLE_CREATE_WAND ]
#define MAX_ENTRY_MOB_DIFF         table_max[ TABLE_MOB_DIFFICULTY ]
#define MAX_ENTRY_OFLAG            table_max[ TABLE_OFLAG ]
#define MAX_ENTRY_AFF_LOCATION     table_max[ TABLE_AFF_LOCATION ]
#define MAX_ENTRY_UNARMED_WORDS    table_max[ TABLE_UNARMED_WORDS ]
#define MAX_ENTRY_DAGGER_WORDS     table_max[ TABLE_DAGGER_WORDS ]
#define MAX_ENTRY_SWORD_WORDS      table_max[ TABLE_SWORD_WORDS ]
#define MAX_ENTRY_CLUB_WORDS       table_max[ TABLE_CLUB_WORDS ]
#define MAX_ENTRY_STAFF_WORDS      table_max[ TABLE_STAFF_WORDS ]
#define MAX_ENTRY_POLEARM_WORDS    table_max[ TABLE_POLEARM_WORDS ]
#define MAX_ENTRY_MACE_WORDS       table_max[ TABLE_MACE_WORDS ]
#define MAX_ENTRY_WHIP_WORDS       table_max[ TABLE_WHIP_WORDS ]
#define MAX_ENTRY_AXE_WORDS        table_max[ TABLE_AXE_WORDS ]
#define MAX_ENTRY_BOW_WORDS        table_max[ TABLE_BOW_WORDS ]
#define MAX_ENTRY_SPEAR_WORDS      table_max[ TABLE_SPEAR_WORDS ]
#define MAX_ENTRY_XBOW_WORDS       table_max[ TABLE_XBOW_WORDS ]
#define MAX_ENTRY_SLING_WORDS      table_max[ TABLE_SLING_WORDS ]
#define MAX_ENTRY_CORE_SKILL       table_max[ TABLE_CORE_SKILL ]
#define MAX_ENTRY_ROGUE_SKILL      table_max[ TABLE_ROGUE_SKILL ]
#define MAX_ENTRY_ASSASSIN_SKILL   table_max[ TABLE_ASSASSIN_SKIL ]
#define MAX_ENTRY_CAVALIER_SKILL   table_max[ TABLE_CAVALIER_SKILL ]
#define MAX_ENTRY_BARBARIAN_SKILL  table_max[ TABLE_BARBARIAN_SKILL ]
#define MAX_ENTRY_STARTING         table_max[ TABLE_STARTING ]
#define MAX_ENTRY_HARVEST          table_max[ TABLE_HARVEST ]
#define MAX_ENTRY_NATION           table_max[ TABLE_NATION ]
#define MAX_ENTRY_GROUP            table_max[ TABLE_GROUP ]
#define MAX_ENTRY_COMMAND          table_max[ TABLE_COMMAND ]
#define MAX_ENTRY_AFF_CHAR         table_max[ TABLE_AFF_CHAR ]
#define MAX_ENTRY_AFF_OBJ          table_max[ TABLE_AFF_OBJ ]
#define MAX_ENTRY_CMD_CAT          table_max[ TABLE_CMD_CAT ]
#define MAX_ENTRY_LIQUID           table_max[ TABLE_LIQUID ]
#define MAX_ENTRY_BUILD            table_max[ TABLE_BUILD ]
#define MAX_ENTRY_HELP_CAT         table_max[ TABLE_HELP_CAT ]
#define MAX_ENTRY_TOWN             table_max[ TABLE_TOWN ]
#define MAX_ENTRY_ASTRAL           table_max[ TABLE_ASTRAL ]
#define MAX_ENTRY_RACE             table_max[ TABLE_RACE ]
#define MAX_ENTRY_RELIGION         table_max[ TABLE_RELIGION ]
#define MAX_ENTRY_METAL            table_max[ TABLE_METAL ]
#define MAX_ENTRY_ALIGNMENT        table_max[ TABLE_ALIGNMENT ]
#define MAX_ENTRY_PLYR_RACE        table_max[ TABLE_PLYR_RACE ]
#define MAX_ENTRY_CLSS             table_max[ TABLE_CLSS ]
#define MAX_ENTRY_TREE             table_max[ TABLE_TREE_GATE ]
#define MAX_ENTRY_SHAPE_CHANGE     table_max[ TABLE_SHAPE_CHANGE ]
#define MAX_ENTRY_POISON           table_max[ TABLE_POISON_DATA ]
#define MAX_ENTRY_CREATE_FOOD      table_max[ TABLE_CREATE_FOOD ]
#define MAX_ENTRY_FORAGE           table_max[ TABLE_FORAGE ]
#define MAX_ENTRY_DANCE            table_max[ TABLE_DANCE ]
#define MAX_ENTRY_CONCOCT          table_max[ TABLE_CONCOCT ]
#define MAX_ENTRY_LICHDOM          table_max[ TABLE_LICHDOM ]
#define MAX_ENTRY_TERRAIN          table_max[ TABLE_TERRAIN ]
#define MAX_ENTRY_TINDER           table_max[ TABLE_TINDER ]
#define MAX_ENTRY_ANIMAL_COMP      table_max[ TABLE_ANIMAL_COMPANION ]
#define MAX_ENTRY_WALK_DAMNED      table_max[ TABLE_WALK_DAMNED ]
#define MAX_ENTRY_SUM_CADAVER      table_max[ TABLE_SUMMON_CADAVER ]
#define MAX_ENTRY_SUM_DRAGON       table_max[ TABLE_SUMMON_DRAGON ]
#define MAX_ENTRY_RETURN_DEAD      table_max[ TABLE_RETURN_DEAD ]
#define MAX_ENTRY_BANE_FIEND       table_max[ TABLE_BANE_FIEND ]
#define MAX_ENTRY_BLOOD_FIEND      table_max[ TABLE_BLOOD_FIEND ]
#define MAX_ENTRY_SUMMON_UNDEAD    table_max[ TABLE_SUMMON_UNDEAD ]
#define MAX_ENTRY_LESSER_MOUNT     table_max[ TABLE_LESSER_MOUNT ]
#define MAX_ENTRY_ANIMATE_CLAY     table_max[ TABLE_ANIMATE_CLAY ]
#define MAX_ENTRY_CONSTRUCT_GOL    table_max[ TABLE_CONSTRUCT_GOLEM ]
#define MAX_ENTRY_CONJ_ELEMENT     table_max[ TABLE_CONJURE_ELEMENTAL ]
#define MAX_ENTRY_FIND_MOUNT       table_max[ TABLE_FIND_MOUNT ]
#define MAX_ENTRY_REQUEST_ALLY     table_max[ TABLE_REQUEST_ALLY ]
#define MAX_ENTRY_FIND_FAMILIAR    table_max[ TABLE_FIND_FAMILIAR ]
#define MAX_ENTRY_LSSR_FAMILIAR    table_max[ TABLE_LESSER_FAMILIAR ]
#define MAX_ENTRY_BONUS            table_max[ TABLE_BONUS ]
#define MAX_ENTRY_TAX_DATA         table_max[ TABLE_TAX ]
#define MAX_ENTRY_SERVICES         table_max[ TABLE_SERVICES ]

#define VAR_CHAR           0
#define VAR_INT            1
#define VAR_SA             2
#define CNST_CHAR          3
#define VAR_SKILL          4
#define VAR_SIZE           5
#define VAR_FUNC           6
#define VAR_CC             7
#define VAR_POS            8
#define VAR_AFF            9
#define VAR_OBJ           10
#define VAR_DICE          11
#define VAR_SCAT          12
#define VAR_STYPE         13
#define VAR_PERM          14
#define VAR_FORMULA       15
#define VAR_LEECH         16
#define VAR_BOOL          17
#define VAR_ALIGN         18
#define VAR_LANG          19
#define VAR_AFF_LOC       20
#define VAR_LOC           21
#define VAR_CENT          22
#define VAR_SEX           23 
#define VAR_TEMP          24
#define VAR_BLANK         25
#define VAR_DELETE        26
#define VAR_RACE          27
#define VAR_CLSS          28
#define VAR_MOB           29
#define VAR_TER           30
#define VAR_LIQ           31
#define VAR_STAT          32
#define VAR_COLOR         33
#define VAR_MOVE_TYPE     34
#define VAR_ELEMENT       35

/*
 *   TYPE DEFINITIONS
 */

typedef class  Mob_Difficulty_Data  mob_diff_data;
typedef class  Affect_Table_Data    affect_table_data;
typedef class  Weapon_Attack_Data   weapon_attack_data;
typedef class  Clss_Skill_Data      clss_skill_data;
typedef class  Aff_Char_Type        aff_char_type;
typedef class  Aff_Obj_Type         aff_obj_type;
typedef class  Entry_Data           entry_data;
typedef class  Nation_Data          nation_data;
typedef class  Group_Data           group_data;
typedef class  Race_Data            race_data;
typedef class  Spell_Data           spell_data;
typedef class  Social_Type          social_type;
typedef class  Liquid_Type          liquid_type;
typedef class  Town_Type            town_type;
typedef class  Metal_Type           metal_type;
typedef class  Category_Data        category_data;
typedef class  Plyr_Race_Data       plyr_race_data;
typedef class  Clss_Type            clss_type;
typedef class  Command_Type         command_type;
typedef class  Starting_Data        starting_data;
typedef class  Tedit_Data           tedit_data;
typedef class  Spell_Act_Type       spell_act_type;
typedef class  Religion_Data        religion_data;
typedef class  Alignment_Data       alignment_data;
typedef class  Shape_Change_Data    shape_change_data;
typedef class  Familiar_Data        familiar_data;
typedef class  Poison_Data          poison_data;
typedef class  Create_Food_Data     create_food_data;
typedef class  Forage_Data          forage_data;
typedef class  Harvest_Data         harvest_data;
typedef class  Dance_Data           dance_data;
typedef class  Tax_Data             tax_data;
typedef class  Terrain_Data         terrain_data;
typedef class  Bonus_Data           bonus_data;
typedef class  Services_Data        services_data;

/*
 *   STRUCTURES 
 */

class Mob_Difficulty_Data
{
 public:
   const char*   entry_name;
   int             bonus_xp;
};

class Affect_Table_Data
{
 public:
   char*    name;
   int     level;
};

class Weapon_Attack_Data
{
 public:
  const char* noun;
  const char* verb;
};

class Clss_Skill_Data
{
 public:
   char*          skill_name;
   int                 level;
   int             prac_cost;  // used only in the calculation of total/used
   char*              damage;
   char*             defense;
   int                affect [ AFFECT_INTS ];
   char*    activate_to_char;
   char*     activate_victim;
   char*  deactivate_to_char;
   char*   deactivate_victim;
   char*    activate_neither;
   char*  deactivate_neither;
   char*             Mv_Cost;
   char*             En_Cost;
   char*             Hp_Cost;
   char*           MvRg_Cost;
   char*           EnRg_Cost;
   char*           HpRg_Cost;
   char*                code;
   char*                type;
   int             skill_cat;
   int       max_skill_level;
   int           max_synergy;
   int      predominant_stat;
   int    int_wis_pre_to_max;
   int            difficulty;
   int                  open;  // should be a boolean
};


class Services_Data
{
 public:
  const char*  entry_name;
  char*      display_name;
  int                cost;
  int               spell;
  int          ingredient[10];
  int                tool[10];
  char*             rmsg1;
  char*             rmsg2;
  char*             rmsg3;
  char*             rmsg4;
  char*             rmsg5;
  int                wait;
  int               skill;
  int                open;    // this should be a bool
  int         consume_obj[5]; // this should be 5 bools, will this even work?
};

class Bonus_Data
{
 public:
   char*   entry_name;
   int             on; // should be bool
   int       bonus_xp;
   int       bonus_gp;
};

class Terrain_Data
{
 public:
  const char*   entry_name;
  char*       terrain_name;
  int            move_cost;
  int                color;
  int             guardian;
  char*       drop_message;
  char*        get_message;
  char*           gen_name;
  char*           gen_desc;
  int         water_source[2];
  int             forested;   // should be bool
  int             ethereal;   // should be bool
  int           underwater;   // should be bool
  int              shallow;   // should be bool
  int        water_surface;   // should be bool
  int               desert;   // should be bool
  int            civilized;   // should be bool
  int           spoil_hide;   // should be bool
  int                  fly;
  int       recovery_bonus;
  int            move_type;
  char*              no_go;
  char*             rno_go;
};

class Tax_Data
{
 public:
  char*              name;
  int          global_tax;
  int        global_vault;
  int           local_tax;
  int         local_vault;
};

class Dance_Data
{
 public:
   const char*        dance;
   char*         message_to;
   char*        message_rto;
   int                 open;   // should be bool
   int                skill;
   char*         move_regen;
};

class Harvest_Data
{
 public:
   const char*      name;
   int            object [2];
   int           terrain;
   int        difficulty;
   int              open;   // should be bool
   int             skill;
   int             level;
   char*     harvest_msg;
   char*       found_msg;
   char*    rharvest_msg;
   char*      rfound_msg;
   int              clss;
   int            rtable;
};

class Forage_Data
{
 public:
   const char*      name;
   int              food [2];
   int           terrain;
   int             level;
   int              open;   // should be bool
};

class Create_Food_Data
{
 public:
   const char*       name;
   int               food[2];
   int              level;
   int               open;
};

class Poison_Data
{
 public:
   const char*        name;
   int              poison [2];
   int          ingredient [10];
   int                tool [10];
   char*     first_message;
   char*    second_message;
   char*     third_message;
   char*    fourth_message;
   char*     fifth_message;
   char*    first_rmessage;
   char*   second_rmessage;
   char*    third_rmessage;
   char*   fourth_rmessage;
   char*    fifth_rmessage;
   int               level;        
   int                open;
   int               skill;
   int                wait;
   int              liquid;
   int          skill_name;
   int  second_skill_level;
};

class Familiar_Data 
{
 public:
   const char*        name;
   int                 mob;
   int         reagent [2];
   int           alignment;
   int                open;  // should be boolean
   int               level;
   char*           arrival;
   char*          rarrival;
   char*       investigate;
};

class Shape_Change_Data
{
 public:
   const char*         name;
   int                  mob;
   int                level;
   int                 open;   // should be boolean
   int          skill_level;
   char*              dream;
};

class Alignment_Data
{
 public:
  const char*     name;
  const char*   abbrev;
};


class Aff_Char_Type
{
 public:
  const char*             name;
  const char*     display_name;
  char*                id_line;
  char*             score_name;
  char*                 msg_on;
  char*            msg_on_room;
  char*                msg_off;
  char*           msg_off_room;
  int               level_cost;
  int          mlocation[ MAX_AFF_MODS ];
  char*        mmodifier[ MAX_AFF_MODS ];
};


class Aff_Obj_Type
{
 public:
  const char*       name;
  char*           msg_on;
  char*          msg_off;
  int          mlocation[ MAX_AFF_MODS ];
};


class Nation_Data
{
 public:
  const char*      name;
  char*          abbrev;
  int            temple;
  int              room  [ 2 ];
  int            nation  [ MAX_NATION ];
  int         alignment  [ MAX_ALIGNMENT ];
};


class Recipe_Data
{
 public:
  char* name;
  int result[ 2 ];
  int ingredient[ 20 ];
  int skill[ 3 ];
  int tool[ 2 ];
};


class Command_Type
{
 public:
  char*             name;
  char*             help;
  char*        func_name;
  do_func*      function;
  int           position;
  int              level  [ 2 ];
  int             reqlen;
  int            disrupt;  // should be bool
  int             reveal;  // should be bool
  int              queue;  // should be bool
  int           category;
  time_data     max_time;
  int              calls;
  time_data   total_time;

  Command_Type( ) {
    function    = NULL;
    max_time    = 0;
    calls       = 0;
    total_time  = 0;
    return;
    };
};


class Entry_Data
{
public:
  int         version;
  const char    *name;
  void        *offset;
  int            type;
};


class Plyr_Race_Data
{
 public:
  const char*     name;
  const char*   plural;
  int             size;
  int           weight;
  int           resist  [ MAX_RESIST ];
  int           affect  [ AFFECT_INTS ];  
  int       stat_bonus  [ 5 ];
  int         hp_bonus;
  int       mana_bonus;
  int       move_bonus;
  int       start_room  [ 3 ];
  int          nc_room  [ 3 ];
  int           portal;
  int        start_age;
  int        life_span;
  int       alignments;
  int         language;
  int         nc_start;  // should be bool
  int             open;  // should be bool
};


class Clss_Type
{
 public:
  const char*        name;
  const char*      abbrev;    
  int             hit_min;    
  int             hit_max;    
  int            mana_min;
  int            mana_max;
  int            move_min;
  int            move_max;
  int           hit_bonus;
  int          mana_bonus;
  int          move_bonus;
  int              resist  [ MAX_RESIST ];
  int          alignments;
  int                open;  // should be bool
  int            nc_start;  // should be bool
  int           new_style;
};


class Group_Data
{
 public:
  const char*    name;
};


class Liquid_Type
{
 public:
  const char*     name;
  char*          color;
  int           hunger;
  int           thirst;
  int          alcohol;
  int             cost;
  int           create;  // should be bool
  int            spell;
};


class Metal_Type
{
 public:
  const char*     name;
  int             cost;
  int           weight;
  int             mana;
  int            armor;
  int          enchant;
  int        save_fire;
  int        save_cold;
  int        save_acid;
  int       save_shock;
  int        save_holy;
  char*       msg_fire;
  char*       msg_cold;
  char*       msg_acid;
  char*      msg_shock;
  char*       msg_holy;
  char*      rust_name;
  char*           rust  [3];   
};


class Race_Data
{
 public:
  const char*      name;
  char*          abbrev;
  char*           track;
}; 

  
class Skill_Type
{
 public:
  const char*       name;
  int          pre_skill  [ 2 ];
  int          pre_level  [ 2 ];
  int           category;
  int          prac_cost  [ MAX_CLSS ];
  int              level  [ MAX_CLSS ];
  int     skill_location;
  int improve_difficulty;
  int          alignment;
  int            synergy [ MAX_NEW_CLSS ];
/*  int              thief;  // should be boolean
  int               mage;  // should be boolean
  int             cleric;  // should be boolean
  int           sorcerer;  // should be boolean
  int              blade;  // should be boolean
  int              druid;  // should be boolean
  int            paladin;  // should be boolean
  int             ranger;  // should be boolean
  int            warrior;  // should be boolean
  int               monk;  // should be boolean
  int           cavalier;  // should be boolean
  int          barbarian;  // should be boolean
  int              rogue;  // should be boolean
  int           assassin;  // should be boolean
  int            defense;  // should be boolean*/
  char*        move_cost;
};


class Social_Type
{
 public:
  char*              name;
  int            position;
  int          aggressive;  // should be bool
  int             disrupt;  // should be bool
  int              reveal;  // should be bool
  char*       char_no_arg;
  char*     others_no_arg;
  char*        char_found;
  char*      others_found;
  char*        vict_found;
  char*        vict_sleep;
  char*         char_auto;
  char*       others_auto;
  char*          dir_self;
  char*        dir_others;
  char*          obj_self;
  char*        obj_others;
  char*       ch_obj_self;
  char*     ch_obj_others;
  char*     ch_obj_victim;
  char*      ch_obj_sleep;
  char*     self_obj_self;
  char*   self_obj_others;
};


class Spell_Act_Type
{
 public:
  char*            name;
  char*      self_other;
  char*    victim_other;
  char*    others_other;
  char*       self_self;
  char*     others_self;
};


class Spell_Data
{
 public:
  char*             name;
  int            prepare;
  int               wait;
  char*           damage;
  char*            regen;
  char*       leech_mana;
  char*        cast_mana;
  char*         duration;
  char*       song_regen;
  int               type;
  int           location;
  spell_func*   function;
  int             action  [ 5 ];
  int            reagent  [ 5 ];
  int               song;  // should be bool right now, will be type later
  int            element;
};


class Tedit_Data
{
 public:
  char*    name;
  int      edit;
  int      new_delete;
};


class Town_Type
{ 
  public:
    char*     name;
    int     recall;
    int  alignment;
    int       race;
};


class Category_Data
{
  public:
    const char*    name;
    int           level;
};


class Starting_Data
{
  public:
    char*     name;
    int       skill  [ 5 ];
    int       level  [ 5 ];
    int      object  [ 10 ]; 
}; 


class Religion_Data
{
  public:
    const char*          name;
    int             sex;
    int      alignments;
    int         classes;
    int          galign;
};


/*
 *   DEFINITIONS
 */


#define AFF_NONE                  -1 
#define AFF_ARMOR                  0
#define AFF_BLESS                  1
#define AFF_BLIND                  2
#define AFF_DISPLACE               3
#define AFF_CHILL                  4
#define AFF_CURSE                  5
#define AFF_DETECT_EVIL            6
#define AFF_DETECT_HIDDEN          7
#define AFF_SEE_INVIS              8 
#define AFF_DETECT_MAGIC           9
#define AFF_FAERIE_FIRE           10
#define AFF_FIRE_SHIELD           11
#define AFF_HIDE                  12
#define AFF_INFRARED              13 
#define AFF_INVISIBLE             14
#define AFF_WRATH                 15
#define AFF_POISON                16
#define AFF_PROTECT               17
#define AFF_SANCTUARY             18
#define AFF_SLEEP                 19
#define AFF_SNEAK                 20
#define AFF_REGENERATION          21
#define AFF_SPEED                 22
#define AFF_WATER_WALKING         23
#define AFF_WATER_BREATHING       24
#define AFF_INVULNERABILITY       25
#define AFF_ENTANGLED             26
#define AFF_CONFUSED              27
#define AFF_HALLUCINATE           28
#define AFF_SLOW                  29
#define AFF_PROT_PLANTS           30
#define AFF_VITALITY              31
 
#define AFF_DETECT_GOOD           32
#define AFF_LIFE_SAVING           33
#define AFF_SLEEP_RESIST          34
#define AFF_RESIST_POISON         35
#define AFF_OGRE_STRENGTH         36
#define AFF_SILENCE               37
#define AFF_TONGUES               38
#define AFF_CONTINUAL_LIGHT       39
#define AFF_PLAGUE                40
#define AFF_TOMB_ROT              41
#define AFF_RABIES                42 
#define AFF_PARALYSIS             43
#define AFF_FLOAT                 44
#define AFF_BARKSKIN              45
#define AFF_PASS_DOOR             46
#define AFF_AXE_PROF              47
#define AFF_SWORD_PROF            48
#define AFF_BOW_PROF              49
#define AFF_LIGHT_SENSITIVE       50
#define AFF_DEATH                 51
#define AFF_SENSE_DANGER          52
#define AFF_RESIST_FIRE           53
#define AFF_RESIST_COLD           54
#define AFF_HASTE                 55
#define AFF_PROTECT_EVIL          56
#define AFF_PROTECT_GOOD          57
#define AFF_FLY                   58
#define AFF_SENSE_LIFE            59
#define AFF_TRUE_SIGHT            60
#define AFF_RESIST_ACID           61
#define AFF_RESIST_SHOCK          62
#define AFF_THORN_SHIELD          63

#define AFF_CHOKING               64
#define AFF_ION_SHIELD            65
#define AFF_CAMOUFLAGE            66
#define AFF_SEE_CAMOUFLAGE        67
#define AFF_ENSNARE               68
#define AFF_ENSNARE_TRAPPED       69
#define AFF_ALL                   70
#define AFF_MOVE_WATER_ONLY       71
#define AFF_COMPANIONS_STRENGTH   72
#define AFF_BRIARTANGLE           73
#define AFF_IRONSKIN              74
#define AFF_SPOT_HIDDEN           75
#define AFF_PAIN                  76
#define AFF_HOAR_FROST            77
#define AFF_VIGOR                 78
#define AFF_HOLY_AURA             79
#define AFF_UNHOLY_AURA           80
#define AFF_UNCEASING_VIGILANCE   81
#define AFF_DIVINE_MIGHT          82
#define AFF_HARDINESS             83
#define AFF_HAMSTRING             84
#define AFF_BANDAGE               85
#define AFF_GLOOM                 86
#define AFF_SOUL_TEAR             87
#define AFF_BLIGHT                88
#define AFF_GIFT_NIGHT            89
#define AFF_GHOST_ARMOR           90
#define AFF_FORSAKEN              91
#define AFF_SKIN_CADAVER          92
#define AFF_STARVING_MADNESS      93
#define AFF_GHOST_CHAINS          94
#define AFF_MIND_BLANK            95

#define AFF_BANE                  96
#define AFF_SHIELD_OF_BONES       97
#define AFF_DEATH_MASK            98
#define AFF_DEATH_SKIN            99
#define AFF_BLOOD_BOND           100
#define AFF_DOOM                 101
#define AFF_DEAD_MAN_EYES        102
#define AFF_SKULL_WATCH          103
#define AFF_GHOST_SHIELD         104
#define AFF_DEMON_SHIELD         105
#define AFF_CALM                 106
#define AFF_SOLIDIFY             107
#define AFF_SHATTER_RESOLVE      108
#define AFF_SIMULCRUM            109
#define AFF_TOAD                 110
#define AFF_BROOK                111
#define AFF_WAR                  112
#define AFF_SUMMER               113
#define AFF_WINTER               114
#define AFF_WHEAT                115
#define AFF_BATTLE               116
#define AFF_SPRING               117
#define AFF_SERENITY             118
#define AFF_SHADOW               119
#define AFF_RIVER                120
#define AFF_HARVEST              121
#define AFF_BUCK                 122
#define AFF_OX                   123
#define AFF_WALRUS               124
#define AFF_SERAPHIM             125
#define AFF_CELESTIAL            126
#define AFF_TIRELESS             127

#define AFF_SWIFT                128
#define AFF_CHEETAH              129
#define AFF_AUTUMN               130
#define AFF_SUMMER_EQUINOX       131
#define AFF_WINTER_EQUINOX       132
#define AFF_SEASONS_MASTERY      133
#define AFF_CARNAGE              134
#define AFF_ETERNAL_BATTLE       135
#define AFF_BLADE_HASTE          136
#define AFF_NATURES_BLESSING     137
#define AFF_NATURES_IMBUED       138
#define AFF_NATURES_INFUSE       139
#define AFF_NATURES_SPIRIT       140
#define AFF_NATURES_CALLING      141
#define AFF_LULLABY              142
#define AFF_ENCHANTED_SLEEP      143
#define AFF_DRAGONS_STANCE       144
#define AFF_BEARS_STANCE         145
#define AFF_CATS_STANCE          146
#define AFF_SWIRLING_BLADES      147
#define AFF_IRON_RESOLVE         148
#define AFF_WHIRLING_DEATH       149
#define AFF_FLAME_SHIELD         150
#define AFF_FIERY_SHIELD         151
#define AFF_INFERNO_SHIELD       152
#define AFF_FROST_SHIELD         153
#define AFF_HOAR_SHIELD          154
#define AFF_ABSOLUTE_ZERO        155
#define AFF_SPARK_SHIELD         156
#define AFF_SHOCK_SHIELD         157
#define AFF_LIGHTNING_SHIELD     158
#define AFF_EROSION_SHIELD       159

#define AFF_CORROSIVE_SHIELD     160
#define AFF_CAUSTIC_SHIELD       161
#define AFF_SPRING_EQUINOX       162
#define AFF_HARVEST_EQUINOX      163
#define AFF_EVIL_AURA            164
#define AFF_DREADFUL_AURA        165
#define AFF_WICKED_AURA          166
#define AFF_BLESSED_AURA         167
#define AFF_HALLOWED_AURA        168
#define AFF_SAINTLY_AURA         169
#define AFF_BONE_PRISON          170
#define AFF_LIFE_TAP             171
#define AFF_WRAITH_FORM          172
#define AFF_GREEN_FINGER         173
#define AFF_NATURES_BOON         174
#define AFF_TREE_FORM            175
#define AFF_SLOW_ENTANGLE        176
#define AFF_WEB_ENTANGLE         177
#define AFF_NATURES_SHIELDING    178
#define AFF_TRAIL_BLAZE          179
#define AFF_PATH_FIND            180
#define AFF_GIFT_OWL             181
#define AFF_CARESS_NATURE        182
#define AFF_GIFT_BEAR            183
#define AFF_GIFT_SNAKE           184
#define AFF_GIFT_LION            185
#define AFF_CARESS_GAIA          186
#define AFF_GIFT_HAWK            187
#define AFF_GIFT_WYRM            188
#define AFF_WILDERNESS_LORE      189
#define AFF_BRAMBLE_SKIN         190
#define AFF_THISTLE_SKIN         191

#define AFF_CIRCLE_HEALING       192
#define AFF_DRUIDIC_RING         193
#define AFF_WATER_WALK           194
#define AFF_WIND_WALK            195
#define AFF_OBSCURING_MIST       196
#define AFF_ACID_DEATH           197

#define AFF_BURNING                0
#define AFF_FLAMING                1
#define AFF_POISONED               2
#define AFF_SHILLELAGH             3

#define CLSS_DEFENSIVE            -1 // just a define for defensive skills, not a real class
#define CLSS_MAGE                  0
#define CLSS_CLERIC                1
#define CLSS_THIEF                 2
#define CLSS_WARRIOR               3
#define CLSS_PALADIN               4
#define CLSS_RANGER                5
#define CLSS_DANCER                6
#define CLSS_MONK                  7
#define CLSS_SORCERER              8
#define CLSS_DRUID                 9

#define CLSS_CAVALIER             10
#define CLSS_BARBARIAN            11
#define CLSS_ROGUE                12
#define CLSS_ASSASSIN             13

#define GROUP_NONE                  0

#define LIQ_WATER                   0
#define LIQ_ACID                   22
#define LIQ_POISON                 23
#define LIQ_HOLY_WATER             24

#define NATION_NONE                 0    // noncitizen mobs have this nation
#define NATION_SECOMBER             1 


#define REL_NONE                    0


/*
 *   VARIABLES
 */

#define ec extern class

ec Social_Type           social_table          [ MAX_PLYR_RACE+1 ][ MAX_SOCIAL ];
ec Spell_Act_Type        spell_act_table       [ MAX_SPELL_ACT ];
ec Liquid_Type           liquid_table          [ MAX_LIQUID ];
ec Town_Type             town_table            [ MAX_TOWN ]; 
ec Spell_Data            spell_table           [ SPELL_COUNT ];     
ec Skill_Type            skill_table           [ MAX_SKILL ];
ec Metal_Type            material_table        [ MAX_MATERIAL ];
ec Nation_Data           nation_table          [ MAX_NATION ];
ec Group_Data            group_table           [ MAX_GROUP ];
ec Race_Data             race_table            [ MAX_RACE ];
ec Plyr_Race_Data        plyr_race_table       [ MAX_PLYR_RACE ];
ec Aff_Char_Type         aff_char_table        [ MAX_AFF_CHAR ];
ec Aff_Obj_Type          aff_obj_table         [ MAX_AFF_OBJ ];
ec Command_Type          command_table         [ MAX_COMMAND ];
ec Category_Data         cmd_cat_table         [ MAX_CMD_CAT ];
ec Clss_Type             clss_table            [ MAX_CLSS ];
ec Starting_Data         starting_table        [ MAX_CLSS+MAX_PLYR_RACE+1 ];
ec Tedit_Data            tedit_table           [ MAX_TABLE ];
ec Recipe_Data           build_table           [ MAX_BUILD ];
ec Category_Data         help_cat_table        [ MAX_HELP_CAT ];
ec Town_Type             astral_table          [ MAX_ASTRAL ]; 
ec Religion_Data         religion_table        [ MAX_RELIGION ]; 
ec Alignment_Data        alignment_table       [ MAX_ALIGNMENT ];
ec Shape_Change_Data     shape_table           [ MAX_SHAPE_CHANGE ]; 
ec Familiar_Data         animal_compan_table   [ MAX_FAMILIAR ];
ec Familiar_Data         walk_damned_table     [ MAX_FAMILIAR ];
ec Familiar_Data         summon_cadaver_table  [ MAX_FAMILIAR ];;
ec Familiar_Data         summon_dragon_table   [ MAX_FAMILIAR ];
ec Familiar_Data         return_dead_table     [ MAX_FAMILIAR ];
ec Familiar_Data         bane_table            [ MAX_FAMILIAR ];
ec Familiar_Data         blood_fiend_table     [ MAX_FAMILIAR ];
ec Familiar_Data         summon_undead_table   [ MAX_FAMILIAR ];
ec Familiar_Data         lesser_mount_table    [ MAX_FAMILIAR ];
ec Familiar_Data         animate_clay_table    [ MAX_FAMILIAR ];
ec Familiar_Data         construct_golem_table [ MAX_FAMILIAR ];
ec Familiar_Data         conjure_element_table [ MAX_FAMILIAR ];
ec Familiar_Data         find_mount_table      [ MAX_FAMILIAR ];
ec Familiar_Data         request_ally_table    [ MAX_FAMILIAR ];
ec Familiar_Data         find_familiar_table   [ MAX_FAMILIAR ];
ec Familiar_Data         lesser_familiar_table [ MAX_FAMILIAR ];
ec Poison_Data           poison_data_table     [ MAX_POISON ];
ec Create_Food_Data      create_food_table     [ MAX_CREATE_FOOD ];
ec Forage_Data           forage_table          [ MAX_FORAGE ];
ec Forage_Data           tinder_table          [ MAX_FORAGE ];
ec Poison_Data           concoct_table         [ MAX_POISON ];
ec Harvest_Data          harvest_table         [ MAX_HARVEST ];
ec Dance_Data            dance_table           [ MAX_DANCE ];
ec Town_Type             tree_table            [ MAX_TREE ];
ec Tax_Data              tax_table             [ MAX_TAX_ENTRIES ];
ec Shape_Change_Data     lichdom_table         [ MAX_FAMILIAR ];
ec Terrain_Data          terrain_table         [ MAX_TERRAINS ];
ec Bonus_Data            bonus_table           [ MAX_BONUS ];
ec Services_Data         services_data_table   [ MAX_SERVICES ];
ec Clss_Skill_Data       rogue_skill_table     [ MAX_NEW_SKILL ];
ec Clss_Skill_Data       assassin_skill_table  [ MAX_NEW_SKILL ];
ec Clss_Skill_Data       cavalier_skill_table  [ MAX_NEW_SKILL ];
ec Clss_Skill_Data       barbarian_skill_table [ MAX_NEW_SKILL ];
ec Clss_Skill_Data       core_skill_table      [ MAX_NEW_SKILL ];
ec Weapon_Attack_Data    unarmed_attack_table  [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    dagger_attack_table   [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    sword_attack_table    [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    club_attack_table     [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    staff_attack_table    [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    polearm_attack_table  [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    mace_attack_table     [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    whip_attack_table     [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    axe_attack_table      [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    bow_attack_table      [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    spear_attack_table    [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    xbow_attack_table     [ MAX_WEAPON_STRING ];
ec Weapon_Attack_Data    sling_attack_table    [ MAX_WEAPON_STRING ];
ec Affect_Table_Data     affect_table          [ MAX_AFF_LOCATION ];
ec Affect_Table_Data     oflag_table           [ MAX_OFLAG ];
ec Mob_Difficulty_Data   mob_diff_table        [ MAX_MOB_DIFF ];
ec Poison_Data           scribe_table          [ MAX_CONCOCT ];
ec Poison_Data           create_wand_table     [ MAX_CONCOCT ];
ec Poison_Data           fletchery_table       [ MAX_CONCOCT ];

ec Entry_Data            table_entry           [ MAX_TABLE-MAX_PLYR_RACE ][ MAX_FIELD ];

#undef ec

//extern const char**             table_field     [ MAX_TABLE ];
extern int                      table_max       [ MAX_TABLE ];


/*
 *   FUNCTIONS
 */


void  init_commands      ( void );
void  init_spells        ( void );
void  sort_socials       ( void );












