#define LIST_LEVELS         0
#define LIST_PERMISSIONS    1
#define LIST_CREATE_FOOD    2
#define LIST_LS_SPECIES     3
#define LIST_LS_REAGENT     4
#define LIST_FF_SPECIES     5
#define LIST_FF_REAGENT     6
#define LIST_RA_SPECIES     7
#define LIST_RA_REAGENT     8
#define LIST_FM_SPECIES     9
#define LIST_FM_REAGENT    10
#define LIST_CE_SPECIES    11
#define LIST_CE_REAGENT    12
#define LIST_CG_SPECIES    13
#define LIST_CG_REAGENT    14
#define LIST_AC_SPECIES    15
#define LIST_AC_REAGENT    16
#define LIST_FIND_TINDER   17
#define LIST_FORAGE        18
#define LIST_POISON_INGRED 19
#define LIST_POISON        20
#define LIST_LM_SPECIES    21
#define LIST_LM_REAGENT    22
#define LIST_SD_SPECIES    23
#define LIST_SD_REAGENT    24
#define LIST_BANE_SPECIES            25
#define LIST_BANE_REAGENT            26
#define LIST_SUMMON_UNDEAD           27
#define LIST_SUMMON_UNDEAD_REAGENT   28
#define LIST_SUMMON_CADAVER          29
#define LIST_SUMMON_CADAVER_REAGENT  30
#define LIST_BLOOD_FIEND             31
#define LIST_BLOOD_FIEND_REAGENT     32
#define LIST_RETURN_DEAD             33
#define LIST_RETURN_DEAD_REAGENT     34
#define LIST_WALK_DAMNED             35 
#define LIST_WALK_DAMNED_REAGENT     36
#define LIST_ANC_SPECIES             37
#define LIST_ANC_REAGENT             38
#define MAX_LIST                     39  //Do -NOT- increase this without adding to list.cpp in list_entry

extern int list_value [ MAX_LIST ][ 38 ];
extern const char* list_entry [ MAX_LIST ][ 40 ];


void   save_lists   ( void );
void   load_lists   ( void );











