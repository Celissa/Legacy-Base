#include "system.h"

/*
 *   LOCAL VARIABLES
 */
const char* rstat_name[] = { "str", "int", "wis", "dex", "con", "level", "piety", "class", "align", "magic", "fire", 
  "cold", "elec", "mind", "age", "mana", "hit", "move", "ac", "hitroll", "damroll", "maregen", "hpregen", "mvregen",
  "acid", "poison", "holy", "none" };


spell_data         spell_table           [ SPELL_COUNT ];     
social_type        social_table          [ MAX_PLYR_RACE+1 ][ MAX_SOCIAL ];
spell_act_type     spell_act_table       [ MAX_SPELL_ACT ];
liquid_type        liquid_table          [ MAX_LIQUID ];
town_type          town_table            [ MAX_TOWN ];
skill_type         skill_table           [ MAX_SKILL ];
metal_type         material_table        [ MAX_MATERIAL ];
nation_data        nation_table          [ MAX_NATION ];
group_data         group_table           [ MAX_GROUP ];     
race_data          race_table            [ MAX_RACE ];
plyr_race_data     plyr_race_table       [ MAX_PLYR_RACE ];
aff_char_type      aff_char_table        [ MAX_AFF_CHAR ];
aff_obj_type       aff_obj_table         [ MAX_AFF_OBJ ];
command_type       command_table         [ MAX_COMMAND ];
category_data      cmd_cat_table         [ MAX_CMD_CAT ];
clss_type          clss_table            [ MAX_CLSS ];
starting_data      starting_table        [ MAX_STARTING ];
tedit_data         tedit_table           [ MAX_TABLE ];
recipe_data        build_table           [ MAX_BUILD ];
category_data      help_cat_table        [ MAX_HELP_CAT ];
town_type          astral_table          [ MAX_ASTRAL ];
religion_data      religion_table        [ MAX_RELIGION ];
alignment_data     alignment_table       [ MAX_ALIGNMENT ];
town_type          tree_table            [ MAX_TREE ];
shape_change_data  shape_table           [ MAX_SHAPE_CHANGE ];
familiar_data      animal_compan_table   [ MAX_FAMILIAR ];
familiar_data      walk_damned_table     [ MAX_FAMILIAR ];
familiar_data      summon_cadaver_table  [ MAX_FAMILIAR ];
familiar_data      summon_dragon_table   [ MAX_FAMILIAR ];
familiar_data      return_dead_table     [ MAX_FAMILIAR ];
familiar_data      bane_table            [ MAX_FAMILIAR ];
familiar_data      blood_fiend_table     [ MAX_FAMILIAR ];
familiar_data      summon_undead_table   [ MAX_FAMILIAR ];
familiar_data      lesser_mount_table    [ MAX_FAMILIAR ];
familiar_data      animate_clay_table    [ MAX_FAMILIAR ];
familiar_data      construct_golem_table [ MAX_FAMILIAR ];
familiar_data      conjure_element_table [ MAX_FAMILIAR ];
familiar_data      find_mount_table      [ MAX_FAMILIAR ];
familiar_data      request_ally_table    [ MAX_FAMILIAR ];
familiar_data      find_familiar_table   [ MAX_FAMILIAR ];
familiar_data      lesser_familiar_table [ MAX_FAMILIAR ];
poison_data        poison_data_table     [ MAX_POISON ];
create_food_data   create_food_table     [ MAX_CREATE_FOOD ];
forage_data        forage_table          [ MAX_FORAGE ];
forage_data        tinder_table          [ MAX_FORAGE ];
harvest_data       harvest_table         [ MAX_HARVEST ];
poison_data        concoct_table         [ MAX_POISON ];
dance_data         dance_table           [ MAX_DANCE ];
tax_data           tax_table             [ MAX_TAX_ENTRIES ];
shape_change_data  lichdom_table         [ MAX_FAMILIAR ];
terrain_data       terrain_table         [ MAX_TERRAINS ];
bonus_data         bonus_table           [ MAX_BONUS ];
services_data      services_data_table   [ MAX_SERVICES ];
clss_skill_data    rogue_skill_table     [ MAX_NEW_SKILL ];
clss_skill_data    assassin_skill_table  [ MAX_NEW_SKILL ];
clss_skill_data    cavalier_skill_table  [ MAX_NEW_SKILL ];
clss_skill_data    barbarian_skill_table [ MAX_NEW_SKILL ];
clss_skill_data    core_skill_table      [ MAX_NEW_SKILL ];
weapon_attack_data unarmed_attack_table  [ MAX_WEAPON_STRING ];
weapon_attack_data dagger_attack_table   [ MAX_WEAPON_STRING ];
weapon_attack_data sword_attack_table    [ MAX_WEAPON_STRING ];
weapon_attack_data club_attack_table     [ MAX_WEAPON_STRING ];
weapon_attack_data staff_attack_table    [ MAX_WEAPON_STRING ];
weapon_attack_data polearm_attack_table  [ MAX_WEAPON_STRING ];
weapon_attack_data mace_attack_table     [ MAX_WEAPON_STRING ];
weapon_attack_data whip_attack_table     [ MAX_WEAPON_STRING ];
weapon_attack_data axe_attack_table      [ MAX_WEAPON_STRING ];
weapon_attack_data bow_attack_table      [ MAX_WEAPON_STRING ];
weapon_attack_data spear_attack_table    [ MAX_WEAPON_STRING ];
weapon_attack_data xbow_attack_table     [ MAX_WEAPON_STRING ];
weapon_attack_data sling_attack_table    [ MAX_WEAPON_STRING ];
affect_table_data  affect_table          [ MAX_AFF_LOCATION ];
affect_table_data  oflag_table           [ MAX_OFLAG ];
mob_diff_data      mob_diff_table        [ MAX_MOB_DIFF ];
poison_data        scribe_table          [ MAX_CONCOCT ];
poison_data        create_wand_table     [ MAX_CONCOCT ];
poison_data        fletchery_table       [ MAX_CONCOCT ];

int table_max [ MAX_TABLE ];


/*
 *   LOCAL CONSTANTS
 */


const int table_abs_max [ MAX_TABLE ] =
{
  MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL,
  MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL,
  MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, 
  MAX_SOCIAL, MAX_SOCIAL, MAX_SOCIAL, MAX_SPELL_ACT, MAX_LIQUID, 
  SPELL_COUNT, MAX_TOWN,  MAX_SKILL, MAX_MATERIAL, MAX_NATION, MAX_GROUP, 
  MAX_RACE, MAX_PLYR_RACE, MAX_AFF_CHAR, MAX_AFF_OBJ, MAX_COMMAND, MAX_CMD_CAT, 
  MAX_CLSS, MAX_CLSS+MAX_PLYR_RACE+1, MAX_TABLE, MAX_BUILD, MAX_HELP_CAT, 
  MAX_ASTRAL, MAX_RELIGION, MAX_ALIGNMENT, MAX_TREE, MAX_SHAPE_CHANGE,
  MAX_FAMILIAR, MAX_FAMILIAR, MAX_FAMILIAR, MAX_FAMILIAR, MAX_FAMILIAR, 
  MAX_FAMILIAR, MAX_FAMILIAR, MAX_FAMILIAR, MAX_FAMILIAR, MAX_FAMILIAR,
  MAX_FAMILIAR, MAX_FAMILIAR, MAX_FAMILIAR, MAX_FAMILIAR, MAX_FAMILIAR, 
  MAX_FAMILIAR, MAX_POISON, MAX_CREATE_FOOD, MAX_FORAGE, MAX_FORAGE, MAX_HARVEST,
  MAX_POISON, MAX_DANCE, MAX_TAX_ENTRIES, MAX_FAMILIAR, MAX_TERRAINS, MAX_BONUS,
  MAX_SERVICES, MAX_NEW_SKILL, MAX_NEW_SKILL, MAX_NEW_SKILL, MAX_NEW_SKILL,
  MAX_NEW_SKILL, MAX_WEAPON_STRING, MAX_WEAPON_STRING, MAX_WEAPON_STRING,
  MAX_WEAPON_STRING, MAX_WEAPON_STRING, MAX_WEAPON_STRING, MAX_WEAPON_STRING,
  MAX_WEAPON_STRING, MAX_WEAPON_STRING, MAX_WEAPON_STRING, MAX_WEAPON_STRING,
  MAX_WEAPON_STRING, MAX_WEAPON_STRING, MAX_AFF_LOCATION, MAX_OFLAG,
  MAX_MOB_DIFF, MAX_CONCOCT, MAX_CONCOCT, MAX_CONCOCT
};


const int table_size [ MAX_TABLE ] =
{
  sizeof( social_type ),    // 1
  sizeof( social_type ),    // 2
  sizeof( social_type ),    // 3
  sizeof( social_type ),    // 4
  sizeof( social_type ),    // 5
  sizeof( social_type ),    // 6
  sizeof( social_type ),    // 7
  sizeof( social_type ),    // 8
  sizeof( social_type ),    // 9
  sizeof( social_type ),    // 10
  sizeof( social_type ),    // 11
  sizeof( social_type ),    // 12
  sizeof( social_type ),    // 13
  sizeof( social_type ),    // 14 -original socials
  sizeof( social_type ),    // 15 -half-elf
  sizeof( social_type ),    // 16 -newrace1
  sizeof( social_type ),    // 17 -newrace2
  sizeof( social_type ),    // 18 -newrace3
  sizeof( social_type ),    // 19 -newrace4
  sizeof( social_type ),    // 20 -newrace5
  sizeof( social_type ),    // 21 -newrace6
  sizeof( spell_act_type ),
  sizeof( liquid_type ),
  sizeof( spell_data ), 
  sizeof( town_type ),
  sizeof( skill_type ), 
  sizeof( metal_type ),
  sizeof( nation_data ),
  sizeof( group_data ),
  sizeof( race_data ),
  sizeof( plyr_race_data ),
  sizeof( aff_char_type ),
  sizeof( aff_obj_type ),
  sizeof( command_type ),
  sizeof( category_data ),
  sizeof( clss_type ),
  sizeof( starting_data ),
  sizeof( tedit_data ),
  sizeof( recipe_data ),
  sizeof( category_data ),
  sizeof( town_type ),
  sizeof( religion_data ),
  sizeof( alignment_data ),
  sizeof( town_type ),
  sizeof( shape_change_data ),  // shape change
  sizeof( familiar_data ),      // animal companion
  sizeof( familiar_data ),      // walk of the damned
  sizeof( familiar_data ),      // summon cadaver
  sizeof( familiar_data ),      // summon dragon
  sizeof( familiar_data ),      // return the dead
  sizeof( familiar_data ),      // bane fiend
  sizeof( familiar_data ),      // summon undead
  sizeof( familiar_data ),      // blood fiend
  sizeof( familiar_data ),      // lesser mount
  sizeof( familiar_data ),      // animate clay
  sizeof( familiar_data ),      // construct golem
  sizeof( familiar_data ),      // conjure elemental
  sizeof( familiar_data ),      // find mount
  sizeof( familiar_data ),      // request ally
  sizeof( familiar_data ),      // find familiar
  sizeof( familiar_data ),      // lesser familiar
  sizeof( poison_data ),        // poison skill
  sizeof( create_food_data ),
  sizeof( forage_data ),        // forage skill
  sizeof( forage_data ),        // tinder skill
  sizeof( harvest_data ),
  sizeof( poison_data ),        // concoct skill
  sizeof( dance_data ),         // list of dances
  sizeof( tax_data ),           // implementaion of tax info
  sizeof( shape_change_data ),  // lichdom
  sizeof( terrain_data ),
  sizeof( bonus_data ),
  sizeof( services_data ),
  sizeof( clss_skill_data ),   // rogue skill table
  sizeof( clss_skill_data ),   // assassin skill table
  sizeof( clss_skill_data ),   // cavalier skill table
  sizeof( clss_skill_data ),   // barbarian skill table
  sizeof( clss_skill_data ),   // core skill table
  sizeof( weapon_attack_data ),// unarmed words
  sizeof( weapon_attack_data ),// dagger words
  sizeof( weapon_attack_data ),// sword words
  sizeof( weapon_attack_data ),// club words
  sizeof( weapon_attack_data ),// staff words
  sizeof( weapon_attack_data ),// polearm words
  sizeof( weapon_attack_data ),// mace words
  sizeof( weapon_attack_data ),// whip words
  sizeof( weapon_attack_data ),// axe words
  sizeof( weapon_attack_data ),// bow words
  sizeof( weapon_attack_data ),// spear words
  sizeof( weapon_attack_data ),// xbow words
  sizeof( weapon_attack_data ),// sling words
  sizeof( affect_table_data ), // affect table stats
  sizeof( affect_table_data ), // oflag table stats
  sizeof( mob_diff_data ),
  sizeof( poison_data ),       // scribe
  sizeof( poison_data ),       // creating wands
  sizeof( poison_data ),       // fletcher
};


/*
 *  all table_entry's now have versioning control... so when you add a new
 *  entry, its number prefix needs to be the new version of file that it was
 *  added at (one higher than the highest number in any given group of entries
 *  the load_table and save_table functions will automatically upgrade the file
 *  upon next reboot and write of the tables... the default values given will
 *  be '0' for everything numeric and 'empty_string' for everything stringbased
 */

/*
 *  VAR_BOOL entries require 'int' storage space to work correctly ... the 32 bits
 *  are addressed by the void** crap used to do the tedit/tset type stuff ... so
 *  still use VAR_BOOL, just note next to it in the .h file something like:
 *          int myvar;  // should be bool
 */

entry_data table_entry [ MAX_TABLE-MAX_PLYR_RACE ][ MAX_FIELD ] =
{
  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &social_table[0][0].name,            VAR_CHAR    },
    { 0, "position",           &social_table[0][0].position,        VAR_POS     },
    { 0, "aggressive",         &social_table[0][0].aggressive,      VAR_BOOL    },
    { 0, "disrupt",            &social_table[0][0].disrupt,         VAR_BOOL    },
    { 0, "reveal",             &social_table[0][0].reveal,          VAR_BOOL    },
    { 0, "no_arg.self",        &social_table[0][0].char_no_arg,     VAR_CHAR    },
    { 0, "no_arg.others",      &social_table[0][0].others_no_arg,   VAR_CHAR    },
    { 0, "ch.self",            &social_table[0][0].char_found,      VAR_CHAR    },
    { 0, "ch.others",          &social_table[0][0].others_found,    VAR_CHAR    },
    { 0, "ch.victim",          &social_table[0][0].vict_found,      VAR_CHAR    },
    { 0, "ch.sleep",           &social_table[0][0].vict_sleep,      VAR_CHAR    },
    { 0, "self.self",          &social_table[0][0].char_auto,       VAR_CHAR    },
    { 0, "self.others",        &social_table[0][0].others_auto,     VAR_CHAR    },
    { 0, "obj.self",           &social_table[0][0].obj_self,        VAR_CHAR    },
    { 0, "obj.others",         &social_table[0][0].obj_others,      VAR_CHAR    },
    { 0, "dir.self",           &social_table[0][0].dir_self,        VAR_CHAR    },
    { 0, "dir.others",         &social_table[0][0].dir_others,      VAR_CHAR    },
    { 0, "ch/obj.self",        &social_table[0][0].ch_obj_self,     VAR_CHAR    },
    { 0, "ch/obj.victim",      &social_table[0][0].ch_obj_victim,   VAR_CHAR    },
    { 0, "ch/obj.others",      &social_table[0][0].ch_obj_others,   VAR_CHAR    },
    { 0, "ch/obj.sleep",       &social_table[0][0].ch_obj_sleep,    VAR_CHAR    },
    { 0, "self/obj.self",      &social_table[0][0].self_obj_self,   VAR_CHAR    },
    { 0, "self/obj.others",    &social_table[0][0].self_obj_others, VAR_CHAR    },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &spell_act_table[0].name,            VAR_CHAR    },
    { 0, "self.other",         &spell_act_table[0].self_other,      VAR_CHAR    },
    { 0, "victim.other",       &spell_act_table[0].victim_other,    VAR_CHAR    },
    { 0, "others.other",       &spell_act_table[0].others_other,    VAR_CHAR    },
    { 0, "self.self",          &spell_act_table[0].self_self,       VAR_CHAR    },
    { 0, "others.self",        &spell_act_table[0].others_self,     VAR_CHAR    },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  { 
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &liquid_table[0].name,               VAR_CHAR    },
    { 0, "color",              &liquid_table[0].color,              VAR_CHAR    },   
    { 0, "hunger",             &liquid_table[0].hunger,             VAR_INT     },
    { 0, "thirst",             &liquid_table[0].thirst,             VAR_INT     },
    { 0, "alcohol",            &liquid_table[0].alcohol,            VAR_INT     },
    { 0, "cp/litre",           &liquid_table[0].cost,               VAR_INT     },
    { 0, "creatable",          &liquid_table[0].create,             VAR_BOOL    },
    { 0, "spell",              &liquid_table[0].spell,              VAR_SKILL   },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    // version 1 - song regen-penalty soft-coded
    // version 2 - added a spell-type to it, right now it'll just be a VAR_BOOL spell/song
    // version 3 - added element to tables, for increase/decrease due to items with that element
    { 0, "name",               &spell_table[0].name,                CNST_CHAR   },
    { 0, "prepare",            &spell_table[0].prepare,             VAR_INT     },
    { 0, "wait",               &spell_table[0].wait,                VAR_INT     },
    { 0, "type",               &spell_table[0].type,                VAR_STYPE   },
    { 0, "damage",             &spell_table[0].damage,              VAR_FORMULA },
    { 0, "cast.mana",          &spell_table[0].cast_mana,           VAR_FORMULA },
    { 0, "leech.mana",         &spell_table[0].leech_mana,          VAR_FORMULA },
    { 0, "regen",              &spell_table[0].regen,               VAR_FORMULA },
    { 0, "duration",           &spell_table[0].duration,            VAR_FORMULA },
    { 1, "song.regen",         &spell_table[0].song_regen,          VAR_FORMULA },
    { 0, "location",           &spell_table[0].location,            VAR_LOC     },
    { 0, "1.action",           &spell_table[0].action[0],           VAR_SA      },
    { 0, "2.action",           &spell_table[0].action[1],           VAR_SA      },
    { 0, "3.action",           &spell_table[0].action[2],           VAR_SA      },
    { 0, "4.action",           &spell_table[0].action[3],           VAR_SA      },
    { 0, "5.action",           &spell_table[0].action[4],           VAR_SA      },
    { 0, "1.reagent",          &spell_table[0].reagent[0],          VAR_INT     },
    { 0, "2.reagent",          &spell_table[0].reagent[1],          VAR_INT     },
    { 0, "3.reagent",          &spell_table[0].reagent[2],          VAR_INT     },
    { 0, "4.reagent",          &spell_table[0].reagent[3],          VAR_INT     },
    { 0, "5.reagent",          &spell_table[0].reagent[4],          VAR_INT     },
    { 2, "song.type",          &spell_table[0].song,                VAR_BOOL    },
    { 3, "element",            &spell_table[0].element,             VAR_ELEMENT },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    // version 1 - added race and alignment info
    { 0, "name",               &town_table[0].name,                 VAR_CHAR    },
    { 0, "location",           &town_table[0].recall,               VAR_INT     },
    { 1, "alignments",         &town_table[0].alignment,            VAR_ALIGN   },
    { 1, "races",              &town_table[0].race,                 VAR_RACE    },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  { 
    // version 0 - standard mysteria2 tables (post table breakup)
    // version 1 - added thief/cleric/mage/sorcerer/blade skill booleans
    // version 2 - added move costs (bladedancer/ranger) skills
    // version 3 - added druid level/cost
    // version 4 - added general druid synergy
    // version 5 - added all classes to general synergies - including the
    //    revised warrior and thief classes
    // switched general synergies to use an array rather than individuals
    { 0, "name",               &skill_table[0].name,                         VAR_CHAR  },
    { 0, "1.prereq",           &skill_table[0].pre_skill[0],                 VAR_SKILL },
    { 0, "1.level",            &skill_table[0].pre_level[0],                 VAR_INT   },
    { 0, "2.prereq",           &skill_table[0].pre_skill[1],                 VAR_SKILL },
    { 0, "2.level",            &skill_table[0].pre_level[1],                 VAR_INT   },
    { 0, "category",           &skill_table[0].category,                     VAR_SCAT  },
    { 0, "cost.mage",          &skill_table[0].prac_cost[ CLSS_MAGE ],       VAR_INT   },  
    { 0, "cost.cleric",        &skill_table[0].prac_cost[ CLSS_CLERIC ],     VAR_INT   },  
    { 0, "cost.thief",         &skill_table[0].prac_cost[ CLSS_THIEF ],      VAR_INT   },  
    { 0, "cost.warrior",       &skill_table[0].prac_cost[ CLSS_WARRIOR ],    VAR_INT   },  
    { 0, "cost.paladin",       &skill_table[0].prac_cost[ CLSS_PALADIN ],    VAR_INT   },  
    { 0, "cost.ranger",        &skill_table[0].prac_cost[ CLSS_RANGER ],     VAR_INT   },  
    { 0, "cost.monk",          &skill_table[0].prac_cost[ CLSS_MONK ],       VAR_INT   },  
    { 0, "cost.dancer",        &skill_table[0].prac_cost[ CLSS_DANCER ],     VAR_INT   },
    { 0, "cost.sorcerer",      &skill_table[0].prac_cost[ CLSS_SORCERER ],   VAR_INT   },
    { 3, "cost.druid",         &skill_table[0].prac_cost[ CLSS_DRUID ],      VAR_INT   },
    { 0, "level.mage",         &skill_table[0].level[ CLSS_MAGE ],           VAR_INT   },
    { 0, "level.cleric",       &skill_table[0].level[ CLSS_CLERIC ],         VAR_INT   },  
    { 0, "level.thief",        &skill_table[0].level[ CLSS_THIEF ],          VAR_INT   },  
    { 0, "level.warrior",      &skill_table[0].level[ CLSS_WARRIOR ],        VAR_INT   },  
    { 0, "level.paladin",      &skill_table[0].level[ CLSS_PALADIN ],        VAR_INT   },  
    { 0, "level.ranger",       &skill_table[0].level[ CLSS_RANGER ],         VAR_INT   },  
    { 0, "level.monk",         &skill_table[0].level[ CLSS_MONK ],           VAR_INT   },  
    { 0, "level.dancer",       &skill_table[0].level[ CLSS_DANCER ],         VAR_INT   },
    { 0, "level.sorcerer",     &skill_table[0].level[ CLSS_SORCERER ],       VAR_INT   },
    { 3, "level.druid",        &skill_table[0].level[ CLSS_DRUID ],          VAR_INT   },
    { 0, "skill.location",     &skill_table[0].skill_location,               VAR_INT   },
    { 0, "improve.diff",       &skill_table[0].improve_difficulty,           VAR_INT   },
    { 0, "alignment",          &skill_table[0].alignment,                    VAR_ALIGN },
    { 1, "thief.skill",        &skill_table[0].synergy[ CLSS_THIEF+1 ],      VAR_BOOL  },
    { 1, "cleric.skill",       &skill_table[0].synergy[ CLSS_CLERIC+1 ],      VAR_BOOL  },
    { 1, "mage.skill",         &skill_table[0].synergy[ CLSS_MAGE+1 ],       VAR_BOOL  },
    { 1, "sorcerer.skill",     &skill_table[0].synergy[ CLSS_SORCERER+1 ],   VAR_BOOL  },
    { 1, "blade.skill",        &skill_table[0].synergy[ CLSS_DANCER+1 ],     VAR_BOOL  },
    { 4, "druid.skill",        &skill_table[0].synergy[ CLSS_DRUID+1 ],      VAR_BOOL  },
    { 5, "ranger.skill",       &skill_table[0].synergy[ CLSS_RANGER+1 ],     VAR_BOOL  },
    { 5, "paladin.skill",      &skill_table[0].synergy[ CLSS_PALADIN+1 ],    VAR_BOOL  },
    { 5, "warrior.skill",      &skill_table[0].synergy[ CLSS_WARRIOR+1 ],    VAR_BOOL  },
    { 5, "monk.skill",         &skill_table[0].synergy[ CLSS_MONK+1 ],       VAR_BOOL  },
    { 5, "cavalier.skill",     &skill_table[0].synergy[ CLSS_CAVALIER+1 ],   VAR_BOOL  },
    { 5, "barbarian.skill",    &skill_table[0].synergy[ CLSS_BARBARIAN+1 ],  VAR_BOOL  },
    { 5, "rogue.skill",        &skill_table[0].synergy[ CLSS_ROGUE+1 ],      VAR_BOOL  },
    { 5, "assassin.skill",     &skill_table[0].synergy[ CLSS_ASSASSIN+1 ],   VAR_BOOL  },
    { 5, "defense.skill",      &skill_table[0].synergy[ CLSS_DEFENSIVE+1 ],  VAR_BOOL  },
    { 2, "move.cost",          &skill_table[0].move_cost,                    VAR_FORMULA },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  { 
    // version 0 - standard mysteria2 tables (post table breakup)
    // version 1 - added save.shock/message.shock
    // version 2 - added save.holy/message.holy
    { 0, "name",               &material_table[0].name,                      CNST_CHAR  },
    { 0, "cost",               &material_table[0].cost,                      VAR_INT    },
    { 0, "weight",             &material_table[0].weight,                    VAR_INT    },
    { 0, "mana",               &material_table[0].mana,                      VAR_INT    },
    { 0, "armor",              &material_table[0].armor,                     VAR_INT    },
    { 0, "enchant",            &material_table[0].enchant,                   VAR_INT    },
    { 0, "save.fire",          &material_table[0].save_fire,                 VAR_INT    },
    { 0, "save.cold",          &material_table[0].save_cold,                 VAR_INT    },
    { 0, "save.acid",          &material_table[0].save_acid,                 VAR_INT    },
    { 1, "save.shock",         &material_table[0].save_shock,                VAR_INT    },
    { 2, "save.holy",          &material_table[0].save_holy,                 VAR_INT    },
    { 0, "message.fire",       &material_table[0].msg_fire,                  VAR_CHAR   },
    { 0, "message.cold",       &material_table[0].msg_cold,                  VAR_CHAR   },
    { 0, "message.acid",       &material_table[0].msg_acid,                  VAR_CHAR   },
    { 1, "message.shock",      &material_table[0].msg_shock,                 VAR_CHAR   },
    { 2, "message.holy",       &material_table[0].msg_holy,                  VAR_CHAR   },
    { 0, "rust.name",          &material_table[0].rust_name,                 VAR_CHAR   },
    { 0, "rust.slight",        &material_table[0].rust[0],                   VAR_CHAR   },
    { 0, "rust.normal",        &material_table[0].rust[1],                   VAR_CHAR   },
    { 0, "rust.critical",      &material_table[0].rust[2],                   VAR_CHAR   },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &nation_table[0].name,                        VAR_CHAR  },
    { 0, "abbrev",             &nation_table[0].abbrev,                      VAR_CHAR  },
    { 0, "temple",             &nation_table[0].temple,                      VAR_INT   },   // 28 integers
    { 0, "1.room",             &nation_table[0].room[0],                     VAR_INT   },
    { 0, "2.room",             &nation_table[0].room[1],                     VAR_INT   },
    { 0, "rela.human",         &nation_table[0].nation[0],                   VAR_INT   },
    { 0, "rela.elf",           &nation_table[0].nation[1],                   VAR_INT   },
    { 0, "rela.gnome",         &nation_table[0].nation[2],                   VAR_INT   },
    { 0, "rela.dwarf",         &nation_table[0].nation[3],                   VAR_INT   },
    { 0, "rela.halfling",      &nation_table[0].nation[4],                   VAR_INT   },
    { 0, "rela.ent",           &nation_table[0].nation[5],                   VAR_INT   },
    { 0, "rela.duergar",       &nation_table[0].nation[6],                   VAR_INT   },
    { 0, "rela.lizardman",     &nation_table[0].nation[7],                   VAR_INT   },
    { 0, "rela.ogre",          &nation_table[0].nation[8],                   VAR_INT   },
    { 0, "rela.troll",         &nation_table[0].nation[9],                   VAR_INT   },
    { 0, "rela.orc",           &nation_table[0].nation[10],                  VAR_INT   },
    { 0, "rela.goblin",        &nation_table[0].nation[11],                  VAR_INT   },
    { 0, "rela.vyan",          &nation_table[0].nation[12],                  VAR_INT   },
    { 0, "rela.undefined1",    &nation_table[0].nation[13],                  VAR_INT   },
    { 0, "rela.undefined2",    &nation_table[0].nation[14],                  VAR_INT   },
    { 0, "rela.undefined3",    &nation_table[0].nation[15],                  VAR_INT   },
    { 0, "rela.lg",            &nation_table[0].alignment[0],                VAR_INT   },
    { 0, "rela.ln",            &nation_table[0].alignment[1],                VAR_INT   },
    { 0, "rela.le",            &nation_table[0].alignment[2],                VAR_INT   },
    { 0, "rela.ng",            &nation_table[0].alignment[3],                VAR_INT   },
    { 0, "rela.pn",            &nation_table[0].alignment[4],                VAR_INT   },
    { 0, "rela.ne",            &nation_table[0].alignment[5],                VAR_INT   },
    { 0, "rela.cg",            &nation_table[0].alignment[6],                VAR_INT   },
    { 0, "rela.cn",            &nation_table[0].alignment[7],                VAR_INT   },
    { 0, "rela.ce",            &nation_table[0].alignment[8],                VAR_INT   },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },  

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &group_table[0].name,                       VAR_CHAR  },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &race_table[0].name,                        VAR_CHAR  },
    { 0, "abbrev",             &race_table[0].abbrev,                      VAR_CHAR  },
    { 0, "tracks",             &race_table[0].track,                       VAR_CHAR  },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    // version 1 - added new continent start
    // version 2 - added holy resist 
    { 0, "name",               &plyr_race_table[0].name,                   VAR_CHAR  },
    { 0, "plural",             &plyr_race_table[0].plural,                 VAR_CHAR  },
    { 0, "hitpoints",          &plyr_race_table[0].hp_bonus,               VAR_INT   },
    { 0, "mana",               &plyr_race_table[0].mana_bonus,             VAR_INT   },
    { 0, "move",               &plyr_race_table[0].move_bonus,             VAR_INT   },
    { 0, "size",               &plyr_race_table[0].size,                   VAR_SIZE  },
    { 0, "weight",             &plyr_race_table[0].weight,                 VAR_CENT  },
    { 0, "strength",           &plyr_race_table[0].stat_bonus[0],          VAR_INT   },
    { 0, "intelligence",       &plyr_race_table[0].stat_bonus[1],          VAR_INT   },
    { 0, "wisdom",             &plyr_race_table[0].stat_bonus[2],          VAR_INT   },
    { 0, "dexterity",          &plyr_race_table[0].stat_bonus[3],          VAR_INT   },
    { 0, "constitution",       &plyr_race_table[0].stat_bonus[4],          VAR_INT   },
    { 0, "magic",              &plyr_race_table[0].resist[0],              VAR_INT   }, 
    { 0, "fire",               &plyr_race_table[0].resist[1],              VAR_INT   }, 
    { 0, "cold",               &plyr_race_table[0].resist[2],              VAR_INT   }, 
    { 0, "electricity",        &plyr_race_table[0].resist[3],              VAR_INT   }, 
    { 0, "mind",               &plyr_race_table[0].resist[4],              VAR_INT   }, 
    { 0, "acid",               &plyr_race_table[0].resist[5],              VAR_INT   }, 
    { 0, "poison",             &plyr_race_table[0].resist[6],              VAR_INT   },
    { 2, "holy",               &plyr_race_table[0].resist[7],              VAR_INT   },
    { 0, "affect",             &plyr_race_table[0].affect[0],              VAR_AFF   },
    { 0, "start.good",         &plyr_race_table[0].start_room[0],          VAR_INT   },
    { 0, "start.neutral",      &plyr_race_table[0].start_room[1],          VAR_INT   },
    { 0, "start.evil",         &plyr_race_table[0].start_room[2],          VAR_INT   },
    { 0, "portal",             &plyr_race_table[0].portal,                 VAR_INT   },
    { 1, "start.good.nc",      &plyr_race_table[0].nc_room[0],             VAR_INT   },
    { 1, "start.neutral.nc",   &plyr_race_table[0].nc_room[1],             VAR_INT   },
    { 1, "start.evil.nc",      &plyr_race_table[0].nc_room[2],             VAR_INT   },
    { 0, "start.age",          &plyr_race_table[0].start_age,              VAR_INT   },
    { 0, "life.span",          &plyr_race_table[0].life_span,              VAR_INT   },
    { 0, "alignment",          &plyr_race_table[0].alignments,             VAR_ALIGN },
    { 0, "language",           &plyr_race_table[0].language,               VAR_LANG  },
    { 0, "open",               &plyr_race_table[0].open,                   VAR_BOOL  },
    { 1, "nc.start",           &plyr_race_table[0].nc_start,               VAR_BOOL  },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    // version 1 - added multiple affect locations + modifiers for MAX_AFF_MODS == 3
    // version 2 - added display.name to eliminate the . in names
    // version 3 - added level cost for estimating level determination for objects
    { 0, "name",               &aff_char_table[0].name,                    VAR_CHAR    },
    { 2, "display.name",       &aff_char_table[0].display_name,            VAR_CHAR    },
    { 0, "id.line",            &aff_char_table[0].id_line,                 VAR_CHAR    },
    { 0, "score.name",         &aff_char_table[0].score_name,              VAR_CHAR    },
    { 0, "msg.on",             &aff_char_table[0].msg_on,                  VAR_CHAR    },
    { 0, "msg.on.room",        &aff_char_table[0].msg_on_room,             VAR_CHAR    },
    { 0, "msg.off",            &aff_char_table[0].msg_off,                 VAR_CHAR    },
    { 0, "msg.off.room",       &aff_char_table[0].msg_off_room,            VAR_CHAR    },
    { 3, "level.cost",         &aff_char_table[0].level_cost,              VAR_INT     },
    { 0, "1.location",         &aff_char_table[0].mlocation[0],            VAR_AFF_LOC },
    { 1, "2.location",         &aff_char_table[0].mlocation[1],            VAR_AFF_LOC },
    { 1, "3.location",         &aff_char_table[0].mlocation[2],            VAR_AFF_LOC },
    { 1, "4.location",         &aff_char_table[0].mlocation[3],            VAR_AFF_LOC },
    { 0, "1.modifier",         &aff_char_table[0].mmodifier[0],            VAR_CHAR    },
    { 1, "2.modifier",         &aff_char_table[0].mmodifier[1],            VAR_CHAR    },
    { 1, "3.modifier",         &aff_char_table[0].mmodifier[2],            VAR_CHAR    },
    { 1, "4.modifier",         &aff_char_table[0].mmodifier[3],            VAR_CHAR    },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    // version 1 - added multiple affect locations + modifiers for MAX_AFF_MODS == 3
    { 0, "name",               &aff_obj_table[0].name,                     VAR_CHAR    },
    { 0, "msg.on",             &aff_obj_table[0].msg_on,                   VAR_CHAR    },
    { 0, "msg.off",            &aff_obj_table[0].msg_off,                  VAR_CHAR    },
    { 0, "1.location",         &aff_obj_table[0].mlocation[0],             VAR_INT     },
    { 1, "2.location",         &aff_obj_table[0].mlocation[1],             VAR_INT     },
    { 1, "3.location",         &aff_obj_table[0].mlocation[2],             VAR_INT     },
    { 1, "4.location",         &aff_obj_table[0].mlocation[3],             VAR_INT     },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &command_table[0].name,                     CNST_CHAR },
    { 0, "help",               &command_table[0].help,                     VAR_CHAR  },
    { 0, "function",           &command_table[0].func_name,                VAR_FUNC  },
    { 0, "level",              &command_table[0].level[0],                 VAR_PERM  },
    { 0, "reqlen",             &command_table[0].reqlen,                   VAR_INT   },
    { 0, "position",           &command_table[0].position,                 VAR_POS   },
    { 0, "category",           &command_table[0].category,                 VAR_CC    },
    { 0, "disrupt",            &command_table[0].disrupt,                  VAR_BOOL  },
    { 0, "reveal",             &command_table[0].reveal,                   VAR_BOOL  },
    { 0, "queue",              &command_table[0].queue,                    VAR_BOOL  },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },
 
  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &cmd_cat_table[0].name,                     VAR_CHAR  },
    { 0, "level",              &cmd_cat_table[0].level,                    VAR_INT   },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    // version 1 - adding new continent recall/starts to tables
    // version 2 - added holy damage
    { 0, "name",               &clss_table[0].name,                        VAR_CHAR  },
    { 0, "abbrev",             &clss_table[0].abbrev,                      VAR_CHAR  },
    { 0, "hit.min",            &clss_table[0].hit_min,                     VAR_INT   },
    { 0, "hit.max",            &clss_table[0].hit_max,                     VAR_INT   },
    { 0, "mana.min",           &clss_table[0].mana_min,                    VAR_INT   },
    { 0, "mana.max",           &clss_table[0].mana_max,                    VAR_INT   },
    { 0, "move.min",           &clss_table[0].move_min,                    VAR_INT   },
    { 0, "move.max",           &clss_table[0].move_max,                    VAR_INT   },
    { 0, "hit.regen",          &clss_table[0].hit_bonus,                   VAR_INT   },
    { 0, "mana.regen",         &clss_table[0].mana_bonus,                  VAR_INT   },
    { 0, "move.regen",         &clss_table[0].move_bonus,                  VAR_INT   },
    { 0, "magic",              &clss_table[0].resist[0],                   VAR_INT   },
    { 0, "fire",               &clss_table[0].resist[1],                   VAR_INT   },
    { 0, "cold",               &clss_table[0].resist[2],                   VAR_INT   },
    { 0, "electricity",        &clss_table[0].resist[3],                   VAR_INT   },
    { 0, "mind",               &clss_table[0].resist[4],                   VAR_INT   },
    { 0, "acid",               &clss_table[0].resist[5],                   VAR_INT   },
    { 0, "poison",             &clss_table[0].resist[6],                   VAR_INT   },
    { 2, "holy",               &clss_table[0].resist[7],                   VAR_INT   },
    { 0, "alignment",          &clss_table[0].alignments,                  VAR_ALIGN },
    { 0, "open",               &clss_table[0].open,                        VAR_BOOL  },
    { 1, "nc.start",           &clss_table[0].nc_start,                    VAR_BOOL  },
    { 3, "new.style",          &clss_table[0].new_style,                   VAR_BOOL  },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "class",              &starting_table[0].name,                    VAR_CHAR  },
    { 0, "1.object",           &starting_table[0].object[0],               VAR_OBJ   },
    { 0, "2.object",           &starting_table[0].object[2],               VAR_OBJ   },
    { 0, "3.object",           &starting_table[0].object[4],               VAR_OBJ   },
    { 0, "4.object",           &starting_table[0].object[6],               VAR_OBJ   },
    { 0, "5.object",           &starting_table[0].object[8],               VAR_OBJ   },
    { 0, "1.skill",            &starting_table[0].skill[0],                VAR_SKILL },
    { 0, "2.skill",            &starting_table[0].skill[1],                VAR_SKILL },
    { 0, "3.skill",            &starting_table[0].skill[2],                VAR_SKILL },
    { 0, "4.skill",            &starting_table[0].skill[3],                VAR_SKILL },
    { 0, "5.skill",            &starting_table[0].skill[4],                VAR_SKILL },
    { 0, "1.level",            &starting_table[0].level[0],                VAR_INT   },
    { 0, "2.level",            &starting_table[0].level[1],                VAR_INT   },
    { 0, "3.level",            &starting_table[0].level[2],                VAR_INT   },
    { 0, "4.level",            &starting_table[0].level[3],                VAR_INT   },
    { 0, "5.level",            &starting_table[0].level[4],                VAR_INT   },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },
  
  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &tedit_table[0].name,                       CNST_CHAR },
    { 0, "lvl.edit",           &tedit_table[0].edit,                       VAR_INT   },
    { 0, "lvl.new/delete",     &tedit_table[0].new_delete,                 VAR_INT   },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &build_table[0].name,                       VAR_CHAR  },
    { 0, "result",             &build_table[0].result[0],                  VAR_OBJ   },
    { 0, "1.ingred",           &build_table[0].ingredient[0],              VAR_OBJ   },
    { 0, "2.ingred",           &build_table[0].ingredient[2],              VAR_OBJ   },
    { 0, "3.ingred",           &build_table[0].ingredient[4],              VAR_OBJ   },
    { 0, "4.ingred",           &build_table[0].ingredient[6],              VAR_OBJ   },
    { 0, "5.ingred",           &build_table[0].ingredient[8],              VAR_OBJ   },
    { 0, "6.ingred",           &build_table[0].ingredient[10],             VAR_OBJ   },
    { 0, "7.ingred",           &build_table[0].ingredient[12],             VAR_OBJ   },
    { 0, "8.ingred",           &build_table[0].ingredient[14],             VAR_OBJ   },
    { 0, "9.ingred",           &build_table[0].ingredient[16],             VAR_OBJ   },
    { 0, "10.ingred",          &build_table[0].ingredient[18],             VAR_OBJ   },
    { 0, "1.skill",            &build_table[0].skill[0],                   VAR_SKILL },
    { 0, "2.skill",            &build_table[0].skill[1],                   VAR_SKILL },
    { 0, "3.skill",            &build_table[0].skill[2],                   VAR_SKILL },
    { 0, "1.tool",             &build_table[0].tool[0],                    VAR_OBJ   },
    { 0, "2.tool",             &build_table[0].tool[1],                    VAR_OBJ   },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &help_cat_table[0].name,                    VAR_CHAR  },
    { 0, "level",              &help_cat_table[0].level,                   VAR_INT   },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &astral_table[0].name,                      VAR_CHAR  },
    { 0, "location",           &astral_table[0].recall,                    VAR_INT   },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - standard mysteria2 tables (post table breakup)
    // version 1 = added the actual gods alignment
    { 0, "name",               &religion_table[0].name,                    VAR_CHAR   },
    { 0, "sex",                &religion_table[0].sex,                     VAR_SEX    },
    { 0, "alignment",          &religion_table[0].alignments,              VAR_ALIGN  },
    { 0, "class",              &religion_table[0].classes,                 VAR_CLSS   },
    { 1, "galignment",         &religion_table[0].galign,                  VAR_ALIGN  },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  { 
    // version 0 - standard mysteria2 tables (post table breakup)
    { 0, "name",               &alignment_table[0].name,                   VAR_CHAR   },
    { 0, "abbrev",             &alignment_table[0].abbrev,                 VAR_CHAR   },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - new tree gate tables 
    { 0, "name",               &tree_table[0].name,                        VAR_CHAR   },
    { 0, "location",           &tree_table[0].recall,                      VAR_INT    },
    { -1, "", NULL, VAR_INT }  // must be last entry
  },

  {
    // version 0 - new shape change tables
    // version 1 - added level, and open (whether mob is finished or not)
    // version 2 - required skill level
    // version 3 - dream message for hinted shape
    { 0, "name",               &shape_table[0].name,                       VAR_CHAR    },
    { 0, "mob",                &shape_table[0].mob,                        VAR_MOB     },
    { 1, "level",              &shape_table[0].level,                      VAR_INT     },
    { 1, "open",               &shape_table[0].open,                       VAR_BOOL    },
    { 2, "skill.level",        &shape_table[0].skill_level,                VAR_INT     },
    { 3, "dream",              &shape_table[0].dream,                      VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Animal Companion )
    // version 1 - added level component, druids only have 1 familiar spell
    // version 2 - arrival messages
    // version 3 - investigate message of reagent
    { 0, "entry.name",         &animal_compan_table[0].name,               VAR_CHAR    },
    { 0, "animal.compan",      &animal_compan_table[0].mob,                VAR_MOB     },
    { 0, "reagent",            &animal_compan_table[0].reagent[0],         VAR_OBJ     },
    { 0, "alignment",          &animal_compan_table[0].alignment,          VAR_ALIGN   },
    { 0, "open",               &animal_compan_table[0].open,               VAR_BOOL    },
    { 1, "level",              &animal_compan_table[0].level,              VAR_INT     },
    { 2, "arrival",            &animal_compan_table[0].arrival,            VAR_CHAR    },
    { 2, "rarrival",           &animal_compan_table[0].rarrival,           VAR_CHAR    },
    { 3, "investigate",        &animal_compan_table[0].investigate,        VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Walk of the Damned )
    // version 1 - arrival messages
    // version 2 - investigate messages of reagent
    { 0, "entry.name",         &walk_damned_table[0].name,                 VAR_CHAR    },
    { 0, "undead",             &walk_damned_table[0].mob,                  VAR_MOB     },
    { 0, "reagent",            &walk_damned_table[0].reagent[0],           VAR_OBJ     },
    { 0, "alignment",          &walk_damned_table[0].alignment,            VAR_ALIGN   },
    { 0, "open",               &walk_damned_table[0].open,                 VAR_BOOL    },
    { 1, "arrival",            &walk_damned_table[0].arrival,              VAR_CHAR    },
    { 1, "rarrival",           &walk_damned_table[0].rarrival,             VAR_CHAR    },
    { 2, "investiage",         &walk_damned_table[0].investigate,          VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Summon Cadaver )
    // version 1 - arrival messages
    // version 2 - investiage messages of reagent
    { 0, "entry.name",         &summon_cadaver_table[0].name,              VAR_CHAR    },
    { 0, "undead",             &summon_cadaver_table[0].mob,               VAR_MOB     },
    { 0, "reagent",            &summon_cadaver_table[0].reagent[0],        VAR_OBJ     },
    { 0, "alignment",          &summon_cadaver_table[0].alignment,         VAR_ALIGN   },
    { 0, "open",               &summon_cadaver_table[0].open,              VAR_BOOL    },
    { 1, "arrival",            &summon_cadaver_table[0].arrival,           VAR_CHAR    },
    { 1, "rarrival",           &summon_cadaver_table[0].rarrival,          VAR_CHAR    },
    { 2, "investiage",         &summon_cadaver_table[0].investigate,       VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Summon Dragon )
    // version 1 - arrival messages
    // version 2 - investigate messages
    { 0, "entry.name",         &summon_dragon_table[0].name,               VAR_CHAR    },
    { 0, "dragon",             &summon_dragon_table[0].mob,                VAR_MOB     },
    { 0, "reagent",            &summon_dragon_table[0].reagent[0],         VAR_OBJ     },
    { 0, "alignment",          &summon_dragon_table[0].alignment,          VAR_ALIGN   },
    { 0, "open",               &summon_dragon_table[0].open,               VAR_BOOL    },
    { 1, "arrival",            &summon_dragon_table[0].arrival,            VAR_CHAR    },
    { 1, "rarrival",           &summon_dragon_table[0].rarrival,           VAR_CHAR    },
    { 2, "investigate",        &summon_dragon_table[0].investigate,        VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

    {
    // version 0 - converting summon spells to tables for easy expansion ( Return the Dead )
    // version 1 - arrival messages
    // version 2 - investigate message
    { 0, "entry.name",         &return_dead_table[0].name,                 VAR_CHAR    },
    { 0, "undead",             &return_dead_table[0].mob,                  VAR_MOB     },
    { 0, "reagent",            &return_dead_table[0].reagent[0],           VAR_OBJ     },
    { 0, "alignment",          &return_dead_table[0].alignment,            VAR_ALIGN   },
    { 0, "open",               &return_dead_table[0].open,                 VAR_BOOL    },
    { 1, "arrival",            &return_dead_table[0].arrival,              VAR_CHAR    },
    { 1, "rarrival",           &return_dead_table[0].rarrival,             VAR_CHAR    },
    { 2, "investigate",        &return_dead_table[0].investigate,          VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Bane Fiend )
    // version 1 - arrival messages
    // version 2 - investigate message
    { 0, "entry.name",         &bane_table[0].name,                       VAR_CHAR    },
    { 0, "demon",              &bane_table[0].mob,                        VAR_MOB     },
    { 0, "reagent",            &bane_table[0].reagent[0],                 VAR_OBJ     },
    { 0, "alignment",          &bane_table[0].alignment,                  VAR_ALIGN   },
    { 0, "open",               &bane_table[0].open,                       VAR_BOOL    },
    { 1, "arrival",            &bane_table[0].arrival,                    VAR_CHAR    },
    { 1, "rarrival",           &bane_table[0].rarrival,                   VAR_CHAR    },
    { 2, "investigate",        &bane_table[0].investigate,                VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Blood Fiend )
    // version 1 - arrival messages
    // version 2 - investigate messages
    { 0, "entry.name",         &blood_fiend_table[0].name,                 VAR_CHAR    },
    { 0, "demon",              &blood_fiend_table[0].mob,                  VAR_MOB     },
    { 0, "reagent",            &blood_fiend_table[0].reagent[0],           VAR_OBJ     },
    { 0, "alignment",          &blood_fiend_table[0].alignment,            VAR_ALIGN   },
    { 0, "open",               &blood_fiend_table[0].open,                 VAR_BOOL    },
    { 1, "arrival",            &blood_fiend_table[0].arrival,              VAR_CHAR    },
    { 1, "rarrival",           &blood_fiend_table[0].rarrival,             VAR_CHAR    },
    { 2, "investigate",        &blood_fiend_table[0].investigate,          VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Summon Undead )
    // version 1 - arrival messages
    // version 2 - investigate messages
    { 0, "entry.name",         &summon_undead_table[0].name,               VAR_CHAR    },
    { 0, "undead",             &summon_undead_table[0].mob,                VAR_MOB     },
    { 0, "reagent",            &summon_undead_table[0].reagent[0],         VAR_OBJ     },
    { 0, "alignment",          &summon_undead_table[0].alignment,          VAR_ALIGN   },
    { 0, "open",               &summon_undead_table[0].open,               VAR_BOOL    },
    { 1, "arrival",            &summon_undead_table[0].arrival,            VAR_CHAR    },
    { 1, "rarrival",           &summon_undead_table[0].rarrival,           VAR_CHAR    },
    { 2, "investigate",        &summon_undead_table[0].investigate,        VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Lesser Mount )
    // version 1 - arrival messages
    // version 2 - investigate messages
    { 0, "entry.name",         &lesser_mount_table[0].name,                 VAR_CHAR    },
    { 0, "mount",              &lesser_mount_table[0].mob,                  VAR_MOB     },
    { 0, "reagent",            &lesser_mount_table[0].reagent[0],           VAR_OBJ     },
    { 0, "alignment",          &lesser_mount_table[0].alignment,            VAR_ALIGN   },
    { 0, "open",               &lesser_mount_table[0].open,                 VAR_BOOL    },
    { 1, "arrival",            &lesser_mount_table[0].arrival,              VAR_CHAR    },
    { 1, "rarrival",           &lesser_mount_table[0].rarrival,             VAR_CHAR    },
    { 2, "investigate",        &lesser_mount_table[0].investigate,          VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Animate Clay )
    // version 1 - arrival messages
    // version 2 - investigate message
    { 0, "entry.name",         &animate_clay_table[0].name,                 VAR_CHAR    },
    { 0, "clay",               &animate_clay_table[0].mob,                  VAR_MOB     },
    { 0, "reagent",            &animate_clay_table[0].reagent[0],           VAR_OBJ     },
    { 0, "alignment",          &animate_clay_table[0].alignment,            VAR_ALIGN   },
    { 0, "open",               &animate_clay_table[0].open,                 VAR_BOOL    },
    { 1, "arrival",            &animate_clay_table[0].arrival,              VAR_CHAR    },
    { 1, "rarrival",           &animate_clay_table[0].rarrival,             VAR_CHAR    },
    { 2, "investigate",        &animate_clay_table[0].investigate,          VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Construct Golem )
    // version 1 - arrival messages
    // version 2 - investigate message
    { 0, "entry.name",         &construct_golem_table[0].name,              VAR_CHAR    },
    { 0, "golem",              &construct_golem_table[0].mob,               VAR_INT     },
    { 0, "reagent",            &construct_golem_table[0].reagent[0],        VAR_OBJ     },
    { 0, "alignment",          &construct_golem_table[0].alignment,         VAR_ALIGN   },
    { 0, "open",               &construct_golem_table[0].open,              VAR_BOOL    },
    { 1, "arrival",            &construct_golem_table[0].arrival,           VAR_CHAR    },
    { 1, "rarrival",           &construct_golem_table[0].rarrival,          VAR_CHAR    },
    { 2, "investigate",        &construct_golem_table[0].investigate,       VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Conjure Elemental )
    // version 1 - arrival messages
    // version 2 - investigate message
    { 0, "entry.name",         &conjure_element_table[0].name,              VAR_CHAR    },
    { 0, "elemental",          &conjure_element_table[0].mob,               VAR_MOB     },
    { 0, "reagent",            &conjure_element_table[0].reagent[0],        VAR_OBJ     },
    { 0, "alignment",          &conjure_element_table[0].alignment,         VAR_ALIGN   },
    { 0, "open",               &conjure_element_table[0].open,              VAR_BOOL    },
    { 1, "arrival",            &conjure_element_table[0].arrival,           VAR_CHAR    },
    { 1, "rarrival",           &conjure_element_table[0].rarrival,          VAR_CHAR    },
    { 2, "investigate",        &conjure_element_table[0].investigate,       VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Find Mount )
    // version 1 - arrival messages
    // version 2 - investigate messages
    { 0, "entry.name",         &find_mount_table[0].name,                   VAR_CHAR    },
    { 0, "mount",              &find_mount_table[0].mob,                    VAR_MOB     },
    { 0, "reagent",            &find_mount_table[0].reagent[0]  ,           VAR_OBJ     },
    { 0, "alignment",          &find_mount_table[0].alignment,              VAR_ALIGN   },
    { 0, "open",               &find_mount_table[0].open,                   VAR_BOOL    },
    { 1, "arrival",            &find_mount_table[0].arrival,                VAR_CHAR    },
    { 1, "rarrival",           &find_mount_table[0].rarrival,               VAR_CHAR    },
    { 2, "investigate",        &find_mount_table[0].investigate,            VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Request Ally )
    // version 1 - arrival messages
    // version 2 - investigate messages
    { 0, "entry.name",         &request_ally_table[0].name,                 VAR_CHAR    },
    { 0, "ally",               &request_ally_table[0].mob,                  VAR_MOB     },
    { 0, "reagent",            &request_ally_table[0].reagent[0],           VAR_OBJ     },
    { 0, "alignment",          &request_ally_table[0].alignment,            VAR_ALIGN   },
    { 0, "open",               &request_ally_table[0].open,                 VAR_BOOL    },
    { 1, "arrival",            &request_ally_table[0].arrival,              VAR_CHAR    },
    { 1, "rarrival",           &request_ally_table[0].rarrival,             VAR_CHAR    },
    { 2, "investigate",        &request_ally_table[0].investigate,          VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Find Familiar )
    // version 1 - arrival messages
    // version 2 - investigate messages
    { 0, "entry.name",         &find_familiar_table[0].name,                VAR_CHAR    },
    { 0, "familiar",           &find_familiar_table[0].mob,                 VAR_MOB     },
    { 0, "reagent",            &find_familiar_table[0].reagent[0],          VAR_OBJ     },
    { 0, "alignment",          &find_familiar_table[0].alignment,           VAR_ALIGN   },
    { 0, "open",               &find_familiar_table[0].open,                VAR_BOOL    },
    { 1, "arrival",            &find_familiar_table[0].arrival,             VAR_CHAR    },
    { 1, "rarrival",           &find_familiar_table[0].rarrival,            VAR_CHAR    },
    { 2, "investigate",        &find_familiar_table[0].investigate,         VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - converting summon spells to tables for easy expansion ( Lesser Familiar )
    // version 1 - arrival messages
    // version 2 - investigate message
    { 0, "entry.name",         &lesser_familiar_table[0].name,              VAR_CHAR    },
    { 0, "familiar",           &lesser_familiar_table[0].mob,               VAR_MOB     },
    { 0, "reagent",            &lesser_familiar_table[0].reagent[0],        VAR_OBJ     },
    { 0, "alignment",          &lesser_familiar_table[0].alignment,         VAR_ALIGN   },
    { 0, "open",               &lesser_familiar_table[0].open,              VAR_BOOL    },
    { 1, "arrival",            &lesser_familiar_table[0].arrival,           VAR_CHAR    },
    { 1, "rarrival",           &lesser_familiar_table[0].rarrival,          VAR_CHAR    },
    { 2, "investigate",        &lesser_familiar_table[0].investigate,       VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },
  
  {
    // version 0 - converting poisons to table for easy expansion, and addition of soft-code messgeas
    // version 1 - added room actions
    // version 2 - added wait
    // version 3 - liquid types (for concoct, not poison)
    // version 4 - required skill level, and experience level
    { 0, "poison.name",        &poison_data_table[0].name,                  VAR_CHAR    },
    { 0, "poison.object",      &poison_data_table[0].poison[0],             VAR_OBJ     },
    { 0, "1.ingredient",       &poison_data_table[0].ingredient[0],         VAR_OBJ     },
    { 0, "2.ingredient",       &poison_data_table[0].ingredient[2],         VAR_OBJ     },
    { 0, "3.ingredient",       &poison_data_table[0].ingredient[4],         VAR_OBJ     },
    { 0, "4.ingredient",       &poison_data_table[0].ingredient[6],         VAR_OBJ     },
    { 0, "5.ingredient",       &poison_data_table[0].ingredient[8],         VAR_OBJ     },
    { 0, "1.tool",             &poison_data_table[0].tool[0],               VAR_OBJ     },
    { 0, "2.tool",             &poison_data_table[0].tool[2],               VAR_OBJ     },
    { 0, "3.tool",             &poison_data_table[0].tool[4],               VAR_OBJ     },
    { 5, "4.tool",             &poison_data_table[0].tool[6],               VAR_OBJ     },
    { 5, "5.tool",             &poison_data_table[0].tool[8],               VAR_OBJ     },
    { 0, "1.action",           &poison_data_table[0].first_message,         VAR_CHAR    },
    { 0, "2.action",           &poison_data_table[0].second_message,        VAR_CHAR    },
    { 0, "3.action",           &poison_data_table[0].third_message,         VAR_CHAR    },
    { 0, "4.action",           &poison_data_table[0].fourth_message,        VAR_CHAR    },
    { 0, "5.action",           &poison_data_table[0].fifth_message,         VAR_CHAR    },
    { 1, "1.room.action",      &poison_data_table[0].first_rmessage,        VAR_CHAR    },
    { 1, "2.room.action",      &poison_data_table[0].second_rmessage,       VAR_CHAR    },
    { 1, "3.room.action",      &poison_data_table[0].third_rmessage,        VAR_CHAR    },
    { 1, "4.room.action",      &poison_data_table[0].fourth_rmessage,       VAR_CHAR    },
    { 1, "5.room.action",      &poison_data_table[0].fifth_rmessage,        VAR_CHAR    },
    { 0, "open",               &poison_data_table[0].open,                  VAR_BOOL    },
    { 2, "wait",               &poison_data_table[0].wait,                  VAR_INT     },
    { 3, "liquid",             &poison_data_table[0].liquid,                VAR_LIQ     },
    { 4, "level",              &poison_data_table[0].level,                 VAR_INT     },
    { 4, "skill.level",        &poison_data_table[0].skill,                 VAR_INT     },
    { -1, "", NULL, VAR_INT }
  },

  {
    // version 0 - converting create food to table for easy expansion and addition
    { 0, "entry.name",         &create_food_table[0].name,                  VAR_CHAR    },
    { 0, "food",               &create_food_table[0].food[0],               VAR_OBJ     },
    { 0, "skill.level",        &create_food_table[0].level,                 VAR_INT     },
    { 1, "open",               &create_food_table[0].open,                  VAR_BOOL    },
    { -1, "", NULL, VAR_INT }
  },

  {
    // version 0 - converting forage to a table
    // version 1 - added required skill level
    { 0, "entry.name",         &forage_table[0].name,                       VAR_CHAR    },
    { 0, "food",               &forage_table[0].food[0],                    VAR_OBJ     },
    { 0, "terrain",            &forage_table[0].terrain,                    VAR_TER     },
    { 1, "skill.level",        &forage_table[0].level,                      VAR_INT     },
    { 0, "open",               &forage_table[0].open,                       VAR_BOOL    },
    { -1, "", NULL, VAR_INT }
  },

  {
    // version 0 - converting tinder to a table
    // version 1 - added required skill level
    { 0, "entry.name",         &tinder_table[0].name,                       VAR_CHAR    },
    { 0, "light",              &tinder_table[0].food[0],                    VAR_OBJ     },
    { 0, "terrain",            &tinder_table[0].terrain,                    VAR_TER     },
    { 1, "skill.level",        &tinder_table[0].level,                      VAR_INT     },
    { 0, "open",               &tinder_table[0].open,                       VAR_BOOL    },
    { -1, "", NULL, VAR_INT }
  },

  {
    // version 0 - harvest table
    // version 1 - added open for new entries
    // version 2 - added a msg for when the item is harvested
    { 0, "entry.name",        &harvest_table[0].name,                       VAR_CHAR    },
    { 0, "harvest.object",    &harvest_table[0].object[0],                  VAR_OBJ     },
    { 0, "terrain",           &harvest_table[0].terrain,                    VAR_TER     },
    { 0, "difficulty",        &harvest_table[0].difficulty,                 VAR_INT     },
    { 0, "skill.level",       &harvest_table[0].skill,                      VAR_INT     },
    { 0, "char.level",        &harvest_table[0].level,                      VAR_INT     },
    { 2, "beg.message",       &harvest_table[0].harvest_msg,                VAR_CHAR    },
    { 2, "beg.rmessage",      &harvest_table[0].rharvest_msg,               VAR_CHAR    },
    { 2, "harvest.class",     &harvest_table[0].clss,                       VAR_CLSS    },
    { 1, "open",              &harvest_table[0].open,                       VAR_BOOL    },
    { 3, "rtable",            &harvest_table[0].rtable,                     VAR_INT     },
    { -1, "", NULL, VAR_INT }
  },

  {
    // version 0 - concoct table
    // version 1 - room actions
    // version 2 - skill/char level
    // version 3 - wait added
    // version 4 - added liquid type
    // version 5 - added tools 4 and 5
    { 0, "concoct.name",       &concoct_table[0].name,                     VAR_CHAR    },
    { 0, "concoct.object",     &concoct_table[0].poison[0],                VAR_OBJ     },
    { 0, "1.ingredient",       &concoct_table[0].ingredient[0],            VAR_OBJ     },
    { 0, "2.ingredient",       &concoct_table[0].ingredient[2],            VAR_OBJ     },
    { 0, "3.ingredient",       &concoct_table[0].ingredient[4],            VAR_OBJ     },
    { 0, "4.ingredient",       &concoct_table[0].ingredient[6],            VAR_OBJ     },
    { 0, "5.ingredient",       &concoct_table[0].ingredient[8],            VAR_OBJ     },
    { 0, "1.tool",             &concoct_table[0].tool[0],                  VAR_OBJ     },
    { 0, "2.tool",             &concoct_table[0].tool[2],                  VAR_OBJ     },
    { 0, "3.tool",             &concoct_table[0].tool[4],                  VAR_OBJ     },
    { 5, "4.tool",             &concoct_table[0].tool[6],                  VAR_OBJ     },
    { 5, "5.tool",             &concoct_table[0].tool[8],                  VAR_OBJ     },
    { 0, "1.action",           &concoct_table[0].first_message,            VAR_CHAR    },
    { 0, "2.action",           &concoct_table[0].second_message,           VAR_CHAR    },
    { 0, "3.action",           &concoct_table[0].third_message,            VAR_CHAR    },
    { 0, "4.action",           &concoct_table[0].fourth_message,           VAR_CHAR    },
    { 0, "5.action",           &concoct_table[0].fifth_message,            VAR_CHAR    },
    { 1, "1.room.action",      &concoct_table[0].first_rmessage,           VAR_CHAR    },
    { 1, "2.room.action",      &concoct_table[0].second_rmessage,          VAR_CHAR    },
    { 1, "3.room.action",      &concoct_table[0].third_rmessage,           VAR_CHAR    },
    { 1, "4.room.action",      &concoct_table[0].fourth_rmessage,          VAR_CHAR    },
    { 1, "5.room.action",      &concoct_table[0].fifth_rmessage,           VAR_CHAR    },
    { 2, "skill.level",        &concoct_table[0].skill,                    VAR_INT     },
    { 2, "char.level",         &concoct_table[0].level,                    VAR_INT     },
    { 0, "open",               &concoct_table[0].open,                     VAR_BOOL    },
    { 3, "wait",               &concoct_table[0].wait,                     VAR_INT     },
    { 4, "liquid",             &concoct_table[0].liquid,                   VAR_LIQ     },
    { -1, "", NULL, VAR_INT }
  },

  {
    // version 0 - dance table, soft-coded messages, and easier start/finish
    // version 1 - moved dance move costs to dance table from skill table
    { 0, "dance.name",         &dance_table[0].dance,                      VAR_CHAR    },
    { 0, "to.message",         &dance_table[0].message_to,                 VAR_CHAR    },
    { 0, "room.message",       &dance_table[0].message_rto,                VAR_CHAR    },
    { 0, "skill",              &dance_table[0].skill,                      VAR_SKILL   },
    { 0, "open",               &dance_table[0].open,                       VAR_BOOL    },
    { 1, "move.regen",         &dance_table[0].move_regen,                 VAR_CHAR    },
    { -1, "", NULL, VAR_INT }
  },

  {
    // version 0 - tax table
    { 0, "entry.name",         &tax_table[0].name,                         VAR_CHAR    },
    { 0, "global.tax",         &tax_table[0].global_tax,                   VAR_CENT    },
    { 0, "global.vault",       &tax_table[0].global_vault,                 VAR_INT     },
    { 0, "local.tax",          &tax_table[0].local_tax,                    VAR_CENT    },
    { 0, "local.vault",        &tax_table[0].local_vault,                  VAR_INT     },
    { -1, "", NULL, VAR_INT }
  },

  {
    // version 0 - lichdom table
    { 0, "name",               &shape_table[0].name,                       VAR_CHAR    },
    { 0, "mob",                &shape_table[0].mob,                        VAR_MOB     },
    { 0, "level",              &shape_table[0].level,                      VAR_INT     },
    { 0, "open",               &shape_table[0].open,                       VAR_BOOL    },
    { 0, "skill.level",        &shape_table[0].skill_level,                VAR_INT     },
    { 0, "dream",              &shape_table[0].dream,                      VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // version 0 - terrain table
    // version 1 - spoil hide entry
    // version 2 - You can't go thata way customizable messages
    { 0, "entry.name",         &terrain_table[0].entry_name,               VAR_CHAR       },
    { 0, "terrain.name",       &terrain_table[0].terrain_name,             VAR_CHAR       },
    { 0, "move.cost",          &terrain_table[0].move_cost,                VAR_INT        },
    { 0, "color",              &terrain_table[0].color,                    VAR_COLOR      },
    { 0, "guardian",           &terrain_table[0].guardian,                 VAR_MOB        },
    { 0, "drop.message",       &terrain_table[0].drop_message,             VAR_CHAR       },
    { 0, "get.message",        &terrain_table[0].get_message,              VAR_CHAR       },
    { 0, "generic.name",       &terrain_table[0].gen_name,                 VAR_CHAR       },
    { 0, "generic.rdesc",      &terrain_table[0].gen_desc,                 VAR_CHAR       },
    { 0, "water.source",       &terrain_table[0].water_source,             VAR_OBJ        },
    { 0, "forested",           &terrain_table[0].forested     ,            VAR_BOOL       },
    { 0, "ethereal",           &terrain_table[0].ethereal,                 VAR_BOOL       },
    { 0, "underwater",         &terrain_table[0].underwater,               VAR_BOOL       },
    { 0, "fly",                &terrain_table[0].fly,                      VAR_BOOL       },
    { 0, "shallow",            &terrain_table[0].shallow,                  VAR_BOOL       },
    { 0, "surface.water",      &terrain_table[0].water_surface,            VAR_BOOL       },
    { 0, "desert",             &terrain_table[0].desert,                   VAR_BOOL       },
    { 0, "civilized",          &terrain_table[0].civilized,                VAR_BOOL       },
    { 1, "spoil.hide",         &terrain_table[0].spoil_hide,               VAR_BOOL       },
    { 0, "recovery.bonus",     &terrain_table[0].recovery_bonus,           VAR_INT        },
    { 0, "move.type",          &terrain_table[0].move_type,                VAR_MOVE_TYPE  },
    { 2, "no.go.msg",          &terrain_table[0].no_go,                    VAR_CHAR       },
    { 2, "rno.go.msg",         &terrain_table[0].rno_go,                   VAR_CHAR       },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // bonux xp/gp table
    { 0, "entry.name",         &bonus_table[0].entry_name,                 CNST_CHAR   },
    { 0, "on",                 &bonus_table[0].on,                         VAR_BOOL    },
    { 0, "bonus.xp",           &bonus_table[0].bonus_xp,                   VAR_CENT    },
    { 0, "bonus.gp",           &bonus_table[0].bonus_gp,                   VAR_CENT    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // services table (for things like minor enchant/major etc
    { 0, "entry.name",         &services_data_table[0].entry_name,          VAR_CHAR    },
    { 0, "display.name",       &services_data_table[0].display_name,        VAR_CHAR    },
    { 0, "cost",               &services_data_table[0].cost,                VAR_INT     },
    { 0, "skill/spell",        &services_data_table[0].spell,               VAR_SKILL   },
    { 0, "1.ingredient",       &services_data_table[0].ingredient[0],       VAR_OBJ     },
    { 0, "2.ingredient",       &services_data_table[0].ingredient[2],       VAR_OBJ     },
    { 0, "3.ingredient",       &services_data_table[0].ingredient[4],       VAR_OBJ     },
    { 0, "4.ingredient",       &services_data_table[0].ingredient[6],       VAR_OBJ     },
    { 0, "5.ingredient",       &services_data_table[0].ingredient[8],       VAR_OBJ     },
    { 0, "1.tool",             &services_data_table[0].tool[0],             VAR_OBJ     },
    { 0, "2.tool",             &services_data_table[0].tool[2],             VAR_OBJ     },
    { 0, "3.tool",             &services_data_table[0].tool[4],             VAR_OBJ     },
    { 0, "4.tool",             &services_data_table[0].tool[6],             VAR_OBJ     },
    { 0, "5.tool",             &services_data_table[0].tool[8],             VAR_OBJ     },
    { 0, "skill.level",        &services_data_table[0].skill,               VAR_INT     },
    { 0, "1.message",          &services_data_table[0].rmsg1,               VAR_CHAR    },
    { 0, "2.message",          &services_data_table[0].rmsg2,               VAR_CHAR    },
    { 0, "3.message",          &services_data_table[0].rmsg3,               VAR_CHAR    },
    { 0, "4.message",          &services_data_table[0].rmsg4,               VAR_CHAR    },
    { 0, "5.message",          &services_data_table[0].rmsg5,               VAR_CHAR    },
    { 0, "wait",               &services_data_table[0].wait,                VAR_INT     },
    { 0, "1.objconsume",       &services_data_table[0].consume_obj[0],      VAR_BOOL    },
    { 0, "2.objconsume",       &services_data_table[0].consume_obj[1],      VAR_BOOL    },
    { 0, "3.objconsume",       &services_data_table[0].consume_obj[2],      VAR_BOOL    },
    { 0, "4.objconsume",       &services_data_table[0].consume_obj[3],      VAR_BOOL    },
    { 0, "5.objconsume",       &services_data_table[0].consume_obj[4],      VAR_BOOL    },
    { 0, "open",               &services_data_table[0].open,                VAR_BOOL    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // rogue skill table
    { 0, "name",               &rogue_skill_table[0].skill_name,            VAR_CHAR    },
    { 0, "level.character",    &rogue_skill_table[0].level,                 VAR_INT     },
    { 0, "prac.cost",          &rogue_skill_table[0].prac_cost,             VAR_INT     },
    { 0, "damage.mod",         &rogue_skill_table[0].damage,                VAR_CHAR    },
    { 0, "defense.mod",        &rogue_skill_table[0].defense,               VAR_CHAR    },
    { 0, "affect",             &rogue_skill_table[0].affect[0],             VAR_AFF     },
    { 0, "act.char",           &rogue_skill_table[0].activate_to_char,      VAR_CHAR    },
    { 0, "act.victim",         &rogue_skill_table[0].activate_victim,       VAR_CHAR    },
    { 0, "act.neither",        &rogue_skill_table[0].activate_neither,      VAR_CHAR    },
    { 0, "deact.char",         &rogue_skill_table[0].deactivate_to_char,    VAR_CHAR    },
    { 0, "deact.victim",       &rogue_skill_table[0].deactivate_victim,     VAR_CHAR    },
    { 0, "deact.neither",      &rogue_skill_table[0].deactivate_neither,    VAR_CHAR    },
    { 0, "move.cost",          &rogue_skill_table[0].Mv_Cost,               VAR_CHAR    },
    { 0, "energy.cost",        &rogue_skill_table[0].En_Cost,               VAR_CHAR    },
    { 0, "hitpoint.cost",      &rogue_skill_table[0].Hp_Cost,               VAR_CHAR    },
    { 0, "move.regen",         &rogue_skill_table[0].MvRg_Cost,             VAR_CHAR    },
    { 0, "energy.regen",       &rogue_skill_table[0].EnRg_Cost,             VAR_CHAR    },
    { 0, "hitpoint.regen",     &rogue_skill_table[0].HpRg_Cost,             VAR_CHAR    },
    { 0, "code (beta",         &rogue_skill_table[0].code,                  VAR_CHAR    },
    { 0, "type",               &rogue_skill_table[0].type,                  VAR_SCAT    },
    { 0, "skill.cat",          &rogue_skill_table[0].skill_cat,             VAR_STYPE   },
    { 0, "max.skill.level",    &rogue_skill_table[0].max_skill_level,       VAR_INT     },
    { 0, "max.synergy",        &rogue_skill_table[0].max_synergy,           VAR_INT     },
    { 0, "primary.stat",       &rogue_skill_table[0].predominant_stat,      VAR_STAT    },
    { 0, "stat.total.max",     &rogue_skill_table[0].int_wis_pre_to_max,    VAR_INT     },
    { 0, "difficulty",         &rogue_skill_table[0].difficulty,            VAR_INT     },
    { 0, "open",               &rogue_skill_table[0].open,                  VAR_BOOL    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // assassin skill table
    { 0, "name",               &assassin_skill_table[0].skill_name,            VAR_CHAR    },
    { 0, "level.character",    &assassin_skill_table[0].level,                 VAR_INT     },
    { 0, "prac.cost",          &assassin_skill_table[0].prac_cost,             VAR_INT     },
    { 0, "damage.mod",         &assassin_skill_table[0].damage,                VAR_CHAR    },
    { 0, "defense.mod",        &assassin_skill_table[0].defense,               VAR_CHAR    },
    { 0, "affect",             &assassin_skill_table[0].affect[0],             VAR_AFF     },
    { 0, "act.char",           &assassin_skill_table[0].activate_to_char,      VAR_CHAR    },
    { 0, "act.victim",         &assassin_skill_table[0].activate_victim,       VAR_CHAR    },
    { 0, "act.neither",        &assassin_skill_table[0].activate_neither,      VAR_CHAR    },
    { 0, "deact.char",         &assassin_skill_table[0].deactivate_to_char,    VAR_CHAR    },
    { 0, "deact.victim",       &assassin_skill_table[0].deactivate_victim,     VAR_CHAR    },
    { 0, "deact.neither",      &assassin_skill_table[0].deactivate_neither,    VAR_CHAR    },
    { 0, "move.cost",          &assassin_skill_table[0].Mv_Cost,               VAR_CHAR    },
    { 0, "energy.cost",        &assassin_skill_table[0].En_Cost,               VAR_CHAR    },
    { 0, "hitpoint.cost",      &assassin_skill_table[0].Hp_Cost,               VAR_CHAR    },
    { 0, "move.regen",         &assassin_skill_table[0].MvRg_Cost,             VAR_CHAR    },
    { 0, "energy.regen",       &assassin_skill_table[0].EnRg_Cost,             VAR_CHAR    },
    { 0, "hitpoint.regen",     &assassin_skill_table[0].HpRg_Cost,             VAR_CHAR    },
    { 0, "code (beta",         &assassin_skill_table[0].code,                  VAR_CHAR    },
    { 0, "type",               &assassin_skill_table[0].type,                  VAR_SCAT    },
    { 0, "skill.cat",          &assassin_skill_table[0].skill_cat,             VAR_STYPE   },
    { 0, "max.skill.level",    &assassin_skill_table[0].max_skill_level,       VAR_INT     },
    { 0, "max.synergy",        &assassin_skill_table[0].max_synergy,           VAR_INT     },
    { 0, "primary.stat",       &assassin_skill_table[0].predominant_stat,      VAR_STAT    },
    { 0, "stat.total.max",     &assassin_skill_table[0].int_wis_pre_to_max,    VAR_INT     },
    { 0, "difficulty",         &assassin_skill_table[0].difficulty,            VAR_INT     },
    { 0, "open",               &assassin_skill_table[0].open,                  VAR_BOOL    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // cavalier skill table
    { 0, "name",               &cavalier_skill_table[0].skill_name,            VAR_CHAR    },
    { 0, "level.character",    &cavalier_skill_table[0].level,                 VAR_INT     },
    { 0, "prac.cost",          &cavalier_skill_table[0].prac_cost,             VAR_INT     },
    { 0, "damage.mod",         &cavalier_skill_table[0].damage,                VAR_CHAR    },
    { 0, "defense.mod",        &cavalier_skill_table[0].defense,               VAR_CHAR    },
    { 0, "affect",             &cavalier_skill_table[0].affect[0],             VAR_AFF     },
    { 0, "act.char",           &cavalier_skill_table[0].activate_to_char,      VAR_CHAR    },
    { 0, "act.victim",         &cavalier_skill_table[0].activate_victim,       VAR_CHAR    },
    { 0, "act.neither",        &cavalier_skill_table[0].activate_neither,      VAR_CHAR    },
    { 0, "deact.char",         &cavalier_skill_table[0].deactivate_to_char,    VAR_CHAR    },
    { 0, "deact.victim",       &cavalier_skill_table[0].deactivate_victim,     VAR_CHAR    },
    { 0, "deact.neither",      &cavalier_skill_table[0].deactivate_neither,    VAR_CHAR    },
    { 0, "move.cost",          &cavalier_skill_table[0].Mv_Cost,               VAR_CHAR    },
    { 0, "energy.cost",        &cavalier_skill_table[0].En_Cost,               VAR_CHAR    },
    { 0, "hitpoint.cost",      &cavalier_skill_table[0].Hp_Cost,               VAR_CHAR    },
    { 0, "move.regen",         &cavalier_skill_table[0].MvRg_Cost,             VAR_CHAR    },
    { 0, "energy.regen",       &cavalier_skill_table[0].EnRg_Cost,             VAR_CHAR    },
    { 0, "hitpoint.regen",     &cavalier_skill_table[0].HpRg_Cost,             VAR_CHAR    },
    { 0, "code (beta",         &cavalier_skill_table[0].code,                  VAR_CHAR    },
    { 0, "type",               &cavalier_skill_table[0].type,                  VAR_SCAT    },
    { 0, "skill.cat",          &cavalier_skill_table[0].skill_cat,             VAR_STYPE   },
    { 0, "max.skill.level",    &cavalier_skill_table[0].max_skill_level,       VAR_INT     },
    { 0, "max.synergy",        &cavalier_skill_table[0].max_synergy,           VAR_INT     },
    { 0, "primary.stat",       &cavalier_skill_table[0].predominant_stat,      VAR_STAT    },
    { 0, "stat.total.max",     &cavalier_skill_table[0].int_wis_pre_to_max,    VAR_INT     },
    { 0, "difficulty",         &cavalier_skill_table[0].difficulty,            VAR_INT     },
    { 0, "open",               &cavalier_skill_table[0].open,                  VAR_BOOL    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // barbarian skill table
    { 0, "name",               &barbarian_skill_table[0].skill_name,            VAR_CHAR    },
    { 0, "level.character",    &barbarian_skill_table[0].level,                 VAR_INT     },
    { 0, "prac.cost",          &barbarian_skill_table[0].prac_cost,             VAR_INT     },
    { 0, "damage.mod",         &barbarian_skill_table[0].damage,                VAR_CHAR    },
    { 0, "defense.mod",        &barbarian_skill_table[0].defense,               VAR_CHAR    },
    { 0, "affect",             &barbarian_skill_table[0].affect[0],             VAR_AFF     },
    { 0, "act.char",           &barbarian_skill_table[0].activate_to_char,      VAR_CHAR    },
    { 0, "act.victim",         &barbarian_skill_table[0].activate_victim,       VAR_CHAR    },
    { 0, "act.neither",        &barbarian_skill_table[0].activate_neither,      VAR_CHAR    },
    { 0, "deact.char",         &barbarian_skill_table[0].deactivate_to_char,    VAR_CHAR    },
    { 0, "deact.victim",       &barbarian_skill_table[0].deactivate_victim,     VAR_CHAR    },
    { 0, "deact.neither",      &barbarian_skill_table[0].deactivate_neither,    VAR_CHAR    },
    { 0, "move.cost",          &barbarian_skill_table[0].Mv_Cost,               VAR_CHAR    },
    { 0, "energy.cost",        &barbarian_skill_table[0].En_Cost,               VAR_CHAR    },
    { 0, "hitpoint.cost",      &barbarian_skill_table[0].Hp_Cost,               VAR_CHAR    },
    { 0, "move.regen",         &barbarian_skill_table[0].MvRg_Cost,             VAR_CHAR    },
    { 0, "energy.regen",       &barbarian_skill_table[0].EnRg_Cost,             VAR_CHAR    },
    { 0, "hitpoint.regen",     &barbarian_skill_table[0].HpRg_Cost,             VAR_CHAR    },
    { 0, "code (beta",         &barbarian_skill_table[0].code,                  VAR_CHAR    },
    { 0, "type",               &barbarian_skill_table[0].type,                  VAR_SCAT    },
    { 0, "skill.cat",          &barbarian_skill_table[0].skill_cat,             VAR_STYPE   },
    { 0, "max.skill.level",    &barbarian_skill_table[0].max_skill_level,       VAR_INT     },
    { 0, "max.synergy",        &barbarian_skill_table[0].max_synergy,           VAR_INT     },
    { 0, "primary.stat",       &barbarian_skill_table[0].predominant_stat,      VAR_STAT    },
    { 0, "stat.total.max",     &barbarian_skill_table[0].int_wis_pre_to_max,    VAR_INT     },
    { 0, "difficulty",         &barbarian_skill_table[0].difficulty,            VAR_INT     },
    { 0, "open",               &barbarian_skill_table[0].open,                  VAR_BOOL    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // core skill table
    { 0, "name",               &core_skill_table[0].skill_name,            VAR_CHAR    },
    { 0, "level.character",    &core_skill_table[0].level,                 VAR_INT     },
    { 0, "prac.cost",          &core_skill_table[0].prac_cost,             VAR_INT     },
    { 0, "damage.mod",         &core_skill_table[0].damage,                VAR_CHAR    },
    { 0, "defense.mod",        &core_skill_table[0].defense,               VAR_CHAR    },
    { 0, "affect",             &core_skill_table[0].affect[0],             VAR_AFF     },
    { 0, "act.char",           &core_skill_table[0].activate_to_char,      VAR_CHAR    },
    { 0, "act.victim",         &core_skill_table[0].activate_victim,       VAR_CHAR    },
    { 0, "act.neither",        &core_skill_table[0].activate_neither,      VAR_CHAR    },
    { 0, "deact.char",         &core_skill_table[0].deactivate_to_char,    VAR_CHAR    },
    { 0, "deact.victim",       &core_skill_table[0].deactivate_victim,     VAR_CHAR    },
    { 0, "deact.neither",      &core_skill_table[0].deactivate_neither,    VAR_CHAR    },
    { 0, "move.cost",          &core_skill_table[0].Mv_Cost,               VAR_CHAR    },
    { 0, "energy.cost",        &core_skill_table[0].En_Cost,               VAR_CHAR    },
    { 0, "hitpoint.cost",      &core_skill_table[0].Hp_Cost,               VAR_CHAR    },
    { 0, "move.regen",         &core_skill_table[0].MvRg_Cost,             VAR_CHAR    },
    { 0, "energy.regen",       &core_skill_table[0].EnRg_Cost,             VAR_CHAR    },
    { 0, "hitpoint.regen",     &core_skill_table[0].HpRg_Cost,             VAR_CHAR    },
    { 0, "code (beta",         &core_skill_table[0].code,                  VAR_CHAR    },
    { 0, "type",               &core_skill_table[0].type,                  VAR_SCAT    },
    { 0, "skill.cat",          &core_skill_table[0].skill_cat,             VAR_STYPE   },
    { 0, "max.skill.level",    &core_skill_table[0].max_skill_level,       VAR_INT     },
    { 0, "max.synergy",        &core_skill_table[0].max_synergy,           VAR_INT     },
    { 0, "primary.stat",       &core_skill_table[0].predominant_stat,      VAR_STAT    },
    { 0, "stat.total.max",     &core_skill_table[0].int_wis_pre_to_max,    VAR_INT     },
    { 0, "difficulty",         &core_skill_table[0].difficulty,            VAR_INT     },
    { 0, "open",               &core_skill_table[0].open,                  VAR_BOOL    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },
  
  {
    // weapon string data (can be modified for a skill table like the others
    { 0, "noun",               &unarmed_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &unarmed_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // dagger string data (can be modified for a skill table like the others
    { 0, "noun",               &dagger_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &dagger_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // sword string data (can be modified for a skill table like the others
    { 0, "noun",               &sword_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &sword_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // club string data (can be modified for a skill table like the others
    { 0, "noun",               &club_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &club_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // staff string data (can be modified for a skill table like the others
    { 0, "noun",               &staff_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &staff_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // polearm string data (can be modified for a skill table like the others
    { 0, "noun",               &polearm_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &polearm_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // mace string data (can be modified for a skill table like the others
    { 0, "noun",               &mace_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &mace_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // whip string data (can be modified for a skill table like the others
    { 0, "noun",               &whip_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &whip_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // axe string data (can be modified for a skill table like the others
    { 0, "noun",               &axe_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &axe_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // bow string data (can be modified for a skill table like the others
    { 0, "noun",               &bow_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &bow_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // spear string data (can be modified for a skill table like the others
    { 0, "noun",               &spear_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &spear_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // xbow string data (can be modified for a skill table like the others
    { 0, "noun",               &xbow_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &xbow_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // sling string data (can be modified for a skill table like the others
    { 0, "noun",               &sling_attack_table[0].noun,               VAR_CHAR    },
    { 0, "verb",               &sling_attack_table[0].verb,               VAR_CHAR    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // affect table for helping estimate levels based on affects
    { 0, "name",               &affect_table[0].name,                     VAR_CHAR    },
    { 0, "level",              &affect_table[0].level,                    VAR_CENT    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // oflag table for helping estimate levels based on affects
    { 0, "name",                &oflag_table[0].name,                     VAR_CHAR    },
    { 0, "level",               &oflag_table[0].level,                    VAR_CENT    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // mob difficulty table
    { 0, "entry.name",          &mob_diff_table[0].entry_name,            VAR_CHAR    },
    { 0, "bonus.xp",            &mob_diff_table[0].bonus_xp,              VAR_CENT    },
    { -1, "", NULL, VAR_INT } // must be last entry
  },

  {
    // scribe initials
    { 0, "scroll.name",        &scribe_table[0].name,                  VAR_CHAR    },
    { 0, "scroll.object",      &scribe_table[0].poison[0],             VAR_OBJ     },
    { 0, "1.ingredient",       &scribe_table[0].ingredient[0],         VAR_OBJ     },
    { 0, "2.ingredient",       &scribe_table[0].ingredient[2],         VAR_OBJ     },
    { 0, "3.ingredient",       &scribe_table[0].ingredient[4],         VAR_OBJ     },
    { 0, "4.ingredient",       &scribe_table[0].ingredient[6],         VAR_OBJ     },
    { 0, "5.ingredient",       &scribe_table[0].ingredient[8],         VAR_OBJ     },
    { 0, "1.tool",             &scribe_table[0].tool[0],               VAR_OBJ     },
    { 0, "2.tool",             &scribe_table[0].tool[2],               VAR_OBJ     },
    { 0, "3.tool",             &scribe_table[0].tool[4],               VAR_OBJ     },
    { 0, "4.tool",             &scribe_table[0].tool[6],               VAR_OBJ     },
    { 0, "5.tool",             &scribe_table[0].tool[8],               VAR_OBJ     },
    { 0, "1.action",           &scribe_table[0].first_message,         VAR_CHAR    },
    { 0, "2.action",           &scribe_table[0].second_message,        VAR_CHAR    },
    { 0, "3.action",           &scribe_table[0].third_message,         VAR_CHAR    },
    { 0, "4.action",           &scribe_table[0].fourth_message,        VAR_CHAR    },
    { 0, "5.action",           &scribe_table[0].fifth_message,         VAR_CHAR    },
    { 0, "1.room.action",      &scribe_table[0].first_rmessage,        VAR_CHAR    },
    { 0, "2.room.action",      &scribe_table[0].second_rmessage,       VAR_CHAR    },
    { 0, "3.room.action",      &scribe_table[0].third_rmessage,        VAR_CHAR    },
    { 0, "4.room.action",      &scribe_table[0].fourth_rmessage,       VAR_CHAR    },
    { 0, "5.room.action",      &scribe_table[0].fifth_rmessage,        VAR_CHAR    },
    { 0, "open",               &scribe_table[0].open,                  VAR_BOOL    },
    { 0, "wait",               &scribe_table[0].wait,                  VAR_INT     },
    { 0, "level",              &scribe_table[0].level,                 VAR_INT     },
    { 0, "scribe.skill",       &scribe_table[0].skill,                 VAR_INT     },
    { 0, "required.skill",     &scribe_table[0].skill_name,            VAR_SKILL   },
    { 0, "required.level",     &scribe_table[0].second_skill_level,    VAR_INT     },
    { -1, "", NULL, VAR_INT }
  },

  {
    // create wand initials
    { 0, "wand.name",          &create_wand_table[0].name,                  VAR_CHAR    },
    { 0, "wand.object",        &create_wand_table[0].poison[0],             VAR_OBJ     },
    { 0, "1.ingredient",       &create_wand_table[0].ingredient[0],         VAR_OBJ     },
    { 0, "2.ingredient",       &create_wand_table[0].ingredient[2],         VAR_OBJ     },
    { 0, "3.ingredient",       &create_wand_table[0].ingredient[4],         VAR_OBJ     },
    { 0, "4.ingredient",       &create_wand_table[0].ingredient[6],         VAR_OBJ     },
    { 0, "5.ingredient",       &create_wand_table[0].ingredient[8],         VAR_OBJ     },
    { 0, "1.tool",             &create_wand_table[0].tool[0],               VAR_OBJ     },
    { 0, "2.tool",             &create_wand_table[0].tool[2],               VAR_OBJ     },
    { 0, "3.tool",             &create_wand_table[0].tool[4],               VAR_OBJ     },
    { 0, "4.tool",             &create_wand_table[0].tool[6],               VAR_OBJ     },
    { 0, "5.tool",             &create_wand_table[0].tool[8],               VAR_OBJ     },
    { 0, "1.action",           &create_wand_table[0].first_message,         VAR_CHAR    },
    { 0, "2.action",           &create_wand_table[0].second_message,        VAR_CHAR    },
    { 0, "3.action",           &create_wand_table[0].third_message,         VAR_CHAR    },
    { 0, "4.action",           &create_wand_table[0].fourth_message,        VAR_CHAR    },
    { 0, "5.action",           &create_wand_table[0].fifth_message,         VAR_CHAR    },
    { 0, "1.room.action",      &create_wand_table[0].first_rmessage,        VAR_CHAR    },
    { 0, "2.room.action",      &create_wand_table[0].second_rmessage,       VAR_CHAR    },
    { 0, "3.room.action",      &create_wand_table[0].third_rmessage,        VAR_CHAR    },
    { 0, "4.room.action",      &create_wand_table[0].fourth_rmessage,       VAR_CHAR    },
    { 0, "5.room.action",      &create_wand_table[0].fifth_rmessage,        VAR_CHAR    },
    { 0, "open",               &create_wand_table[0].open,                  VAR_BOOL    },
    { 0, "wait",               &create_wand_table[0].wait,                  VAR_INT     },
    { 0, "level",              &create_wand_table[0].level,                 VAR_INT     },
    { 0, "create.wand.level",  &create_wand_table[0].skill,                 VAR_INT     },
    { 0, "required.skill",     &create_wand_table[0].skill_name,            VAR_SKILL   },
    { 0, "required.level",     &create_wand_table[0].second_skill_level,       VAR_INT     },
    { -1, "", NULL, VAR_INT }
  },

  {
    // fletching initials
    { 0, "arrow.name",         &fletchery_table[0].name,                  VAR_CHAR    },
    { 0, "arrow.object",       &fletchery_table[0].poison[0],             VAR_OBJ     },
    { 0, "1.ingredient",       &fletchery_table[0].ingredient[0],         VAR_OBJ     },
    { 0, "2.ingredient",       &fletchery_table[0].ingredient[2],         VAR_OBJ     },
    { 0, "3.ingredient",       &fletchery_table[0].ingredient[4],         VAR_OBJ     },
    { 0, "4.ingredient",       &fletchery_table[0].ingredient[6],         VAR_OBJ     },
    { 0, "5.ingredient",       &fletchery_table[0].ingredient[8],         VAR_OBJ     },
    { 0, "1.tool",             &fletchery_table[0].tool[0],               VAR_OBJ     },
    { 0, "2.tool",             &fletchery_table[0].tool[2],               VAR_OBJ     },
    { 0, "3.tool",             &fletchery_table[0].tool[4],               VAR_OBJ     },
    { 0, "4.tool",             &fletchery_table[0].tool[6],               VAR_OBJ     },
    { 0, "5.tool",             &fletchery_table[0].tool[8],               VAR_OBJ     },
    { 0, "1.action",           &fletchery_table[0].first_message,         VAR_CHAR    },
    { 0, "2.action",           &fletchery_table[0].second_message,        VAR_CHAR    },
    { 0, "3.action",           &fletchery_table[0].third_message,         VAR_CHAR    },
    { 0, "4.action",           &fletchery_table[0].fourth_message,        VAR_CHAR    },
    { 0, "5.action",           &fletchery_table[0].fifth_message,         VAR_CHAR    },
    { 0, "1.room.action",      &fletchery_table[0].first_rmessage,        VAR_CHAR    },
    { 0, "2.room.action",      &fletchery_table[0].second_rmessage,       VAR_CHAR    },
    { 0, "3.room.action",      &fletchery_table[0].third_rmessage,        VAR_CHAR    },
    { 0, "4.room.action",      &fletchery_table[0].fourth_rmessage,       VAR_CHAR    },
    { 0, "5.room.action",      &fletchery_table[0].fifth_rmessage,        VAR_CHAR    },
    { 0, "open",               &fletchery_table[0].open,                  VAR_BOOL    },
    { 0, "wait",               &fletchery_table[0].wait,                  VAR_INT     },
    { 0, "level",              &fletchery_table[0].level,                 VAR_INT     },
    { 0, "fletch.level",       &fletchery_table[0].skill,                 VAR_INT     },
    { -1, "", NULL, VAR_INT }
  }
};

const char* table_name [ MAX_TABLE ] =
{
  "Soc.Default", "Soc.Human", "Soc.Elf", "Soc.Gnome", "Soc.Dwarf",
  "Soc.Halfling", "Soc.Ent", "Soc.Duergar", "Soc.Lizard", "Soc.Ogre",
  "Soc.Troll", "Soc.Orc", "Soc.Goblin", "Soc.Vyan", "Soc.Half-Elf",
  "Soc.New1", "Soc.New2", "Soc.New3", "Soc.New4", "Soc.New5", "Soc.New6",
  "Spell.Actions", "Liquids", "Spell.Data", "Towns", "Skills", "Materials",
  "Nations", "Groups", "Races", "Player.Races", "Aff.Char", "Aff.Obj", 
  "Commands", "Cmd.Categories", "Classes", "Starting", "Tables", "Build",
  "Help.Categories", "Astral", "Religions", "Alignments", "Tree.Gate",
  "Shape.Change", "Animal.Companion", "Walk.Damned", "Summon.Cadaver",
  "Summon.Dragon", "Return.Dead", "Bane.Fiend", "Blood.Fiend", "Summon.Undead",
  "Lesser.Mount", "Animate.Clay", "Construct.Golem", "Conjure.Elemental",
  "Find.Mount", "Request.Ally", "Find.Familiar", "Lesser.Familiar", "Poison.Data",
  "Create.Food", "Forage", "Tinder", "Harvest", "Concoct", "Dance", "Tax.Data",
  "Lichdom", "Terrain", "Bonus", "Services", "Rogue.Skill", "Assassin.Skill",
  "Cavalier.Skill", "Barbarian.Skill", "Core.Skill", "Unarmed.Words",
  "Dagger.Words", "Sword.Words", "Club.Words", "Staff.Words", "Polearm.Words",
  "Mace.Words", "Whip.Words", "Axe.Words", "Bow.Words", "Spear.Words",
  "Crossbow.Words", "Sling.Words", "Affect.Table", "Oflag.Table", "Mob.Challenge",
  "Scribe.Table", "Wand.Table", "Fletchery.Table"
};


/*
 *   LOCAL FUNCTIONS
 */


int    find_table      ( char_data*, char*&, const char*,
                         const char* = empty_string );
int    find_entry      ( char_data*, char*&, int, const char* = empty_string );
void   init_entry      ( int, char* );
void   init_commands   ( void );
void   remove_entry    ( char_data*, int, int );
void   sort_table      ( char_data*, int );
void   swap_entries    ( int, int, int );


inline char*   entry_name   ( int, int );


/*
 *   SUPPORT ROUTINES
 */

bool edit_table( char_data* ch, int table )
{
  int flag;

  switch( table ) {
    case TABLE_COMMAND     :  flag = PERM_COMMANDS;       break;
    case TABLE_SPELL_DATA  :
    case TABLE_SPELL_ACT   :  flag = PERM_SPELLS;         break;
    default                :  flag = PERM_MISC_TABLES;    break; 
  }

  if( table <= TABLE_SOC_NEW_RACE5  && table == ch->shdata->race+1 )
    flag = PERM_SOCIALS;

  if( !has_permission( ch, flag ) ) {
    send( ch, "You do not have permission to edit that table.\r\n" );
    return FALSE;
  }

  if( table > TABLE_SOC_NEW_RACE5 ) {
    if( tedit_table[ table-TABLE_SOC_NEW_RACE5-1 ].edit > ch->shdata->level ) {
      send( ch, "You do not have permission to edit that table.\r\n" );
      return FALSE;
    }
  }

  return TRUE;
}


int find_table( char_data* ch, char*& argument, const char* msg1, const char* msg2 )
{
  int       i;

  if( *argument == '\0' ) {
    if( msg2 != empty_string ) 
      send( ch, msg2 );
    return -2;
  }

  for( i = 0; i < MAX_TABLE; i++ ) 
    if( matches( argument, table_name[i] ) )  
      return i;

  send( msg1, ch );
  return -1;
}


int find_entry( char_data* ch, char*& argument, int table, const char* msg )
{
  int       i;

  if( *argument == '\0' ) {
    if( msg != empty_string )
      send( ch, msg );
    return -2;
  }

  for( i = 0; i < table_max[table]; i++ ) 
    if( exact_match_all_args( argument, entry_name( table, i ) ) ) 
      return i;

  for( i = 0; i < table_max[table]; i++ ) 
    if( fmatches( argument, entry_name( table, i )) ) 
      return i;

  send( ch, "The %s table does not contain such an entry.\r\n", table_name[table] );

  return -1;
}


/*
 *   ENTRY INFO
 */


inline char* entry_name( int table, int col )
{
  void* pntr;

  if( table <= MAX_PLYR_RACE )
    return social_table[table][col].name;

  pntr = (void *) ( ( (int) table_entry[ table - MAX_PLYR_RACE ][ 0 ].offset ) + col * table_size[ table ] );

  return (char*) *( (void**) pntr );
}


int column_type( int table, int entry )
{
  table = max( 0, table - MAX_PLYR_RACE );
 
  return table_entry[ table ][ entry ].type;
}


int column_version( int table, int entry )
{
  table = max( 0, table - MAX_PLYR_RACE );
 
  return table_entry[ table ][ entry ].version;
}


const char* column_name( int table, int entry )
{
  table = max( 0, table - MAX_PLYR_RACE );
 
  return table_entry[ table ][ entry ].name;
}


void* table_pntr( int table, int entry, int col )
{
  if( table <= MAX_PLYR_RACE ) 
    return (void *) ((int) &social_table[table][col] - (int) &social_table[0][0] + (int) table_entry[0][entry].offset);

  return (void *) ((int) table_entry[table - MAX_PLYR_RACE][entry].offset + col * table_size[table]);
}


int get_table_version( int table )
{
  int version = 0;

  for( int j = 0; j < table_max[ table ]; j++ ) {
    for( int k = 0; column_version( table, k ) >= 0; k++ ) {
      if( column_version( table, k ) > version )
        version = column_version( table, k );
    }
  }

  return version;
}

/*
 *   DISK ROUTINES
 */


void load_tables( )
{
  FILE*         fp;
  int      i, j, k;
  char**      pntr;
  char*     string;
  int        table;
  int      version;

  echo( "Loading Tables ...\r\n" );
 
  for( i = -1; i < MAX_TABLE; i++ ) {
    table = ( i == -1 ? TABLE_SKILL : i );
    fp = open_file( TABLE_DIR, table_name[table], "rb", TRUE );

    char *word = fread_word( fp );
    if( !strcmp( word, "#M2_TABLE" ) ) {
      version = fread_number( fp );
    } else if( !strcmp( word, "#TABLE" ) ) {
      version = 0;
    } else {
      panic( "Load_tables: header not found" );
      return; // not necessary, because panic calls exit( ) but it shuts up the compiler warnings
    }

    log( "  * %-20s : v%d : \r\n", table_name[table], version );

    table_max[table] = fread_number( fp );

    if( table_max[table] > table_abs_max[table] ) 
      panic( "Load_Tables: Number of entries in table %s > table max.", table_name[table] );

    for( j = 0; j < table_max[ table ]; j++ ) {
      for( k = 0; column_version( table, k ) >= 0; k++ ) {
        pntr = (char **) table_pntr( table, k, j );

        switch( column_type( table, k ) ) {
        case VAR_TEMP :
          *pntr = (char *) 0;
          break;
        
        case VAR_BLANK :
          *pntr = empty_string;
          break;
        
        case VAR_FORMULA :
        case VAR_CHAR :
        case CNST_CHAR : 
        case VAR_FUNC :
          if( column_version( table, k ) > version )
            *pntr = empty_string;
          else
            *pntr = fread_string( fp, MEM_TABLE );
          break;
        
        case VAR_BOOL :
        case VAR_LEECH :
        case VAR_SIZE :
        case VAR_SA :
        case VAR_INT :
        case VAR_CC :
        case VAR_SCAT :
        case VAR_DICE :
        case VAR_STYPE :
        case VAR_ALIGN :
        case VAR_RACE :
        case VAR_CLSS :
        case VAR_LANG :
        case VAR_AFF_LOC :
        case VAR_LOC :
        case VAR_CENT :
        case VAR_SEX :
        case VAR_TER:
        case VAR_LIQ:
        case VAR_STAT:
        case VAR_COLOR:
        case VAR_MOVE_TYPE:
        case VAR_ELEMENT:
          if( column_version( table, k ) > version )
            *pntr = (char *) 0;
          else
            *pntr = (char *) fread_number( fp );
          break;
        
        case VAR_POS :
          if( column_version( table, k ) > version )
            *pntr = (char *) 0;
          else if( (*pntr = (char *) fread_number( fp ) ) < 0 || ((int) *pntr) >= MAX_POSITION ) 
            panic( "Load_Tables: Impossible position." );
          break;
        
        case VAR_DELETE :
          if( column_version( table, k ) > version )
            *pntr = (char *) 0;
          else
            fread_number( fp );
          break;
        
        case VAR_SKILL :
          if( column_version( table, k ) > version )
            *pntr = (char *) -1;
          else {
            string = fread_string( fp, MEM_UNKNOWN );
            if( i != -1 ) 
              *pntr = (char *) skill_index( string );
            free_string( string, MEM_UNKNOWN );
          }
          break;
        
        case VAR_MOB:
          if( column_version( table, k ) > version ) {
            *pntr     = (char *) 0;
          } else {
            *pntr     = (char *) fread_number( fp );
          }
          break;

        case VAR_PERM :
        case VAR_OBJ  :
          if( column_version( table, k ) > version ) {
            *pntr     = (char *) 0;
            *(pntr+1) = (char *) 0;
          } else {
            *pntr     = (char *) fread_number( fp );
            *(pntr+1) = (char *) fread_number( fp );
          }
          break;
       
        case VAR_AFF  :
          if( column_version( table, k ) > version ) {
            // its a new column, just set all affs to 0
            for( int i = 0; i < AFFECT_INTS; i++ )
              *(pntr+i) = (char *) 0;

          } else {
            // load existing column
            char c = fread_letter( fp );
            int count = 3;

            // detect new aff loading or not
            if( c == 'A' )
              count = fread_number( fp );
            else
              ungetc( c, fp );

            for( int i = 0; i < AFFECT_INTS; i++ ) {
              if( i < count )
                *(pntr+i) = (char *) fread_number( fp );
              else
                *(pntr+i) = (char *) 0;
            }
          }
          break;
        }
      }
    }

    fclose( fp );
  }

  if( table_max[ TABLE_SKILL ] != MAX_SKILL ) {
    roach( "Load_Tables: Entries in skill table != max_skill." );
    roach( "-- Max_Skill = %d", MAX_SKILL );
    panic( "--   Entries = %d", table_max[ TABLE_SKILL ] );
  }

  init_commands( );
  init_spells( );
}


void save_tables(  )
{
  FILE*          fp;
  char**       pntr;
  int       i, j, k;

  for( i = 0; i < MAX_TABLE; i++ ) {
    rename_file( TABLE_DIR, table_name[i], TABLE_PREV_DIR, table_name[i] );

    fp = open_file( TABLE_DIR, table_name[i], "wb" );

    fprintf( fp, "#M2_TABLE\n\n" );
    fprintf( fp, "%d\n%d\n\n", get_table_version( i ), table_max[i] );

    for( j = 0; j < table_max[ i ]; j++ ) {
      for( k = 0; column_version( i, k ) >= 0; k++ ) {
        pntr = (char **) table_pntr( i,k,j );
        switch( column_type( i,k ) ) {
        case VAR_DELETE :
          break;

        case VAR_BLANK :
        case CNST_CHAR :
        case VAR_FUNC :
        case VAR_CHAR :
        case VAR_FORMULA :
          fprintf( fp, "%s~\n", (char*) *pntr );
          break;

        case VAR_BOOL :
        case VAR_SIZE :
        case VAR_INT : 
        case VAR_SA :
        case VAR_CC:
        case VAR_POS:
        case VAR_DICE :
        case VAR_SCAT :
        case VAR_STYPE :
        case VAR_LEECH :
        case VAR_TEMP :
        case VAR_ALIGN :
        case VAR_RACE :
        case VAR_CLSS :
        case VAR_LANG :
        case VAR_AFF_LOC :
        case VAR_LOC :
        case VAR_CENT :
        case VAR_SEX :
        case VAR_TER:
        case VAR_LIQ:
        case VAR_STAT:
        case VAR_COLOR:
        case VAR_MOVE_TYPE:
        case VAR_ELEMENT:
          fprintf( fp, "%d\n", (int) *pntr ); 
          break;

        case VAR_SKILL :
          if( (int) *pntr < 0 )
            fprintf( fp, "~\n" );
          else
            fprintf( fp, "%s~\n", skill_table[ (int) *pntr ].name );
          break;

        case VAR_MOB:
          fprintf( fp, "%d\n", (int) *pntr );
          break;

        case VAR_PERM :
        case VAR_OBJ :
          fprintf( fp, "%d %d\n", (int) *pntr, (int) *(pntr+1) );
          break;

        case VAR_AFF:
          fprintf( fp, "A%d ", AFFECT_INTS );
          for( int i = 0; i < AFFECT_INTS; i++ )
            fprintf( fp, "%d ", (int) *(pntr+i) );
          fprintf( fp, "\n" );
          break;
        }  
      }
      fprintf( fp, "\n" ); 
    }
    fprintf( fp, "\n" ); 
    fclose( fp );
  }
}


/*
 *   TEDIT ROUTINE
 */


void do_tedit( char_data* ch, char* argument )
{
  wizard_data*  imm  = (wizard_data*) ch;
  int             i  = -1;
  int             j  = -1;

  if( *argument == '\0' ) {
    display_array( ch, "Tables", &table_name[0], &table_name[1], MAX_TABLE );
    return;
  }

  if( exact_match( argument, "new" ) ) {
    if( ( i = find_table( ch, argument, "Table not found.", "Syntax: tedit new <table> <entry>" ) ) < 0 )
      return;
    if( table_max[ i ] == table_abs_max[ i ] ) {
      send( ch, "That table has no open slots.\r\n" );
      return;
    }
    if( !edit_table( ch, i ) )
      return;

    if( i > TABLE_SOC_NEW_RACE5 ) {
      if( tedit_table[i-TABLE_SOC_NEW_RACE5-1].new_delete > ch->shdata->level ) {
        send( ch, "You are not allowed to create new entries on that table.\r\n" );
        return;
      }
    }
    
    init_entry( i, argument );
    imm->table_edit[ 0 ] = i;
    imm->table_edit[ 1 ] = table_max[ i ] - 1;
    send( ch, "Table entry added and you are now editing it.\r\n" );
    return;
  }

  if( exact_match( argument, "delete" ) ) {
    if( ( i = find_table( ch, argument, "Table not found.\r\n" ) ) != -1 && ( j = find_entry( ch, argument, i, "Syntax: tedit delete <table> <name>\r\n" ) ) >= 0 ) {
      if( i > TABLE_SOC_NEW_RACE5 ) {
        if( tedit_table[i-TABLE_SOC_NEW_RACE5-1].new_delete > ch->shdata->level ) {
          send( ch, "You are not allowed to delete entries on that table.\r\n" );
          return;
        }
      }
      remove_entry( ch, i, j );
    }
    return;
  }

  if( exact_match( argument, "sort" ) ) {
    if( ( i = find_table( ch, argument, "Table not found.\r\n", "Which table do you wish to sort?\r\n" ) ) < 0 )
      return;
    
    sort_table( ch, i );
    return;
  }        

  if( ( i = find_table( ch, argument, "Table not found.\r\n" ) ) == -1 )
    return;

  if( *argument == '\0' ) {
    display_array( ch, table_name[i], (const char**) table_pntr( i,0,0 ), (const char**) table_pntr( i,0,1 ), table_max[i] );
    return;
  }

  if( ( j = find_entry( ch, argument, i ) ) == -1 )
    return;

  imm->table_edit[0] = i;
  imm->table_edit[1] = j;

  send( ch, "Tstat and tset now work on the %s %s.\r\n", table_name[ i ], entry_name( i, j ) );
} 


void do_tset( char_data* ch, char* argument )
{
  char           arg  [ MAX_INPUT_LENGTH ];
  char           tmp  [ MAX_INPUT_LENGTH ];  
  void**        pntr;
  wizard_data*   imm  = (wizard_data*) ch;
  int         length;
  int              i  = imm->table_edit[0];
  int              j  = imm->table_edit[1];
  int           k, n;
  int*         ipntr; 
  bool         error  = FALSE;
  thing_array* array;
  obj_data*      obj;
  mob_data*      npc;
  thing_data*  thing;

  if( *argument == '\0' ) {
    do_tstat( ch, argument );
    return;
  }

  if( !edit_table( ch, i ) )
    return;

  argument = one_argument( argument, arg );

  length = strlen( arg );
  for( k = 0; column_version( i, k ) >= 0; k++ ) 
    if( !strncasecmp( arg, column_name( i, k ), length ) )
      break;

  if( column_version( i, k ) < 0 ) {
    send( "Syntax: tset <field> <value>\r\n", ch );
    return;
  }

  pntr = (void**) table_pntr( i, k, j );

  switch( column_type( i, k ) ) {
  case VAR_TEMP :
    send( "That is a unsorted variable and may not be set.\r\n", ch );
    return;

  case VAR_SIZE : {
    class type_field size_field = { "size", MAX_SIZE, &size_name[0], &size_name[1], (int*) pntr };
    pntr = (void **) table_pntr( i, 0, j );
    size_field.set( ch, (char*) *pntr, argument ); 
    return;
  }

  case VAR_STYPE : {
    class type_field stype_field = { "type", MAX_STYPE, &stype_name[0], &stype_name[1], (int*) pntr };
    pntr = (void **) table_pntr( i, 0, j );
    stype_field.set( ch, (char*) *pntr, argument ); 
    return;
  }

  case VAR_COLOR : {
    class type_field color_field = { "color", MAX_COLOR, &color_fields[0], &color_fields[1], (int*) pntr };
    pntr = (void**) table_pntr( i, 0, j );
    color_field.set( ch, (char*) *pntr, argument );
    return;
  }

  case VAR_MOVE_TYPE: {
    class type_field move_type = { "move_type", MAX_MOVE, &leaving_verb[0], &leaving_verb[1], (int*) pntr };
    pntr = (void**) table_pntr( i, 0, j );
    move_type.set( ch, (char*) *pntr, argument );
    return;
  }
                 
  case VAR_AFF_LOC : {
    class type_field stype_field = { "type", MAX_AFF_LOCATION, &affect_location[0], &affect_location[1], (int*) pntr };
    pntr = (void **) table_pntr( i, 0, j );
    stype_field.set( ch, (char*) *pntr, argument ); 
    return;
  }

  case VAR_BOOL :
    ipntr = (int*) pntr;
    set_bool( ch, argument, column_name( i, k ), *ipntr ); 
    return;

  case VAR_ALIGN :
    align_flags.set( ch, argument, (int*) pntr );
    return;     

  case VAR_RACE :
    race_flags.set( ch, argument, (int*) pntr );
    return;     

  case VAR_CLSS :
    clss_flags.set( ch, argument, (int*) pntr );
    return;     

  case VAR_LANG :
    lang_flags.set( ch, argument, (int*) pntr );
    return;      

  case VAR_AFF :
    affect_flags.set( ch, argument, (int*) pntr );
    return;

  case VAR_PERM :
    permission_flags.set( ch, argument, (int*) pntr );
    return;

  case VAR_LOC :
    location_flags.set( ch, argument, (int*) pntr );
    return;

  case VAR_POS : {
    class type_field pos_field = { "position", MAX_POSITION, &position_name[0], &position_name[1], (int*) pntr };
    pntr = (void **) table_pntr( i, 0, j );
    pos_field.set( ch, (char*) *pntr, argument ); 
    return;
  }

  case VAR_SEX : {
    class type_field sex_field = { "sex", MAX_SEX-1, &sex_name[0], &sex_name[1], (int*) pntr };
    pntr = (void **) table_pntr( i, 0, j );
    sex_field.set( ch, (char*) *pntr, argument ); 
    return;
  }

  case VAR_FORMULA :
    evaluate( argument, error );
    if( error ) {
      send( ch, "Expression fails to evaluate.\r\n" );
      return;
    }

  case VAR_BLANK :
  case VAR_CHAR :
  case VAR_FUNC  :
    free_string( (char*) *pntr, MEM_TABLE );
    *pntr = (void *) alloc_string( argument, MEM_TABLE );
    pntr = (void **) table_pntr( i, 0, j );
    sprintf( tmp, "%s on %s %s set to:\r\n%s\r\n", column_name( i, k ), table_name[ i ], (char*) *pntr, argument );
    break;

  case VAR_INT :
    n            = atoi( argument );
    *pntr = (void *) n;
    pntr = (void **) table_pntr( i, 0, j );
    sprintf( tmp, "%s on %s %s set to %d.\r\n", column_name( i, k ), table_name[ i ], (char*) *pntr, n );
    break;

  case VAR_CENT :
    n            = (int) (100 * atof( argument ));
    *pntr = (void *) n;
    pntr = (void **) table_pntr( i, 0, j );
    sprintf( tmp, "%s on %s %s set to %.2f.\r\n", column_name( i, k ), table_name[ i ], (char*) *pntr, (float) n/100 );
    break;

  case VAR_CC : {
    class type_field cat_field = { "category", MAX_ENTRY_CMD_CAT, &cmd_cat_table[0].name, &cmd_cat_table[1].name, (int*) pntr };
    pntr = (void **) table_pntr( i, 0, j );
    cat_field.set( ch, (char*) *pntr, argument ); 
    return;
  } 
    
  case VAR_SKILL :
    length = strlen( argument );
    for( n = 0; n < table_max[TABLE_SKILL]; n++ ) 
      if( !strncasecmp( argument, skill_table[n].name, length ) )
        break;
    if( n == table_max[ TABLE_SKILL ] ) {
      if( strncasecmp( argument, "none", length ) ) {
        send( "Unknown skill.\r\n", ch );
        return;
      } 
      n = -1;
    }
    *pntr = (void *) n;
    pntr = (void **) table_pntr( i,0,j );
    sprintf( tmp, "%s on %s %s set to %s.\r\n", column_name( i, k ), table_name[ i ], (char*) *pntr, n == -1 ? "none" : skill_table[n].name );
    break;

  case VAR_SA :
    length = strlen( argument );
    for( n = 0; n < table_max[TABLE_SPELL_ACT]; n++ ) 
      if( !strncasecmp( argument, spell_act_table[n].name, length ) ) 
        break;
    if( n == table_max[TABLE_SPELL_ACT] ) {
      send( "No spell action matching that name found.\r\n", ch );
      return;
    }     
    *pntr = (void *) n;
    pntr = (void **) table_pntr( i,0,j );
    sprintf( tmp, "%s on %s %s set to %s.\r\n", column_name( i, k ), table_name[ i ], (char*) *pntr, spell_act_table[n].name );
    break;

  case VAR_SCAT : {
    class type_field scat_field = { "category", MAX_SKILL_CAT, &skill_cat_name[0],  &skill_cat_name[1], (int*) pntr };
    pntr = (void **) table_pntr( i,0,j );
    scat_field.set( ch, (char*) *pntr, argument ); 
    return;
  }

  case VAR_ELEMENT: {
    class type_field element_cat = { "element category", MAX_ATTACK+1, &element_name[0], &element_name[1], (int*) pntr };
    pntr = (void**) table_pntr( i,0,j );
    element_cat.set( ch, (char*) *pntr, argument );
    return;
  }

  case VAR_OBJ :
    if( !strcasecmp( argument, "nothing" ) ) {
      send( ch, "Field set to nothing.\r\n" );
      *pntr     = (void *) 0;
      *(pntr+1) = (void *) 1; 
      return;
    }
    if( ( array = several_things( OBJ_DATA, ch, argument, "tset" , &ch->contents ) ) == NULL ) {
      send( "You don't have that object.\r\n", ch );
      return;
      }
    if( array->size != 1 ) {
      send( ch, "You may only set one type of item in a slot.\r\n" );
      return;
    }
    if( (obj = object( array->list[0] ) ) == NULL ) {
      send( ch, "Only objects can be in this slot.\r\n" );
      return;
    }
    
    *pntr     = (void *) obj->pIndexData->vnum;
    *(pntr+1) = (void *) obj->selected; 
    send( ch, "Object set.\r\n" );
    return;

  case VAR_MOB :
    if( !strcasecmp( argument, "nothing" ) ) {
      send( ch, "Field set to nothing.\r\n" );
      *pntr      = (void *) 0;
      return;
    } 
    if( ( thing = one_thing( THING_DATA, ch, argument, "reset", ch->array, &ch->contents ) ) == NULL ) {
      send( "That mob is not in the room.\r\n", ch );
      return;
    }
    if( ( npc = mob( thing ) ) == NULL ) {
      send( ch, "Only mobs may be set in this slot.\r\n" );
      return;
    }

    *pntr       = (void *) npc->species->vnum;
    send( ch, "Mob set.\r\n" );
    return;

  case VAR_LEECH :
  case VAR_DICE : {
    class dice_field entry = { column_name( i, k ), LEVEL_MOB, (int*) pntr };
    pntr = (void **) table_pntr( i,0,j );
    entry.set( ch, (char*) *pntr, argument );
    return;
  }

  case CNST_CHAR :
    send( ch, "That entry is not setable.\r\n" );
    return;

  case VAR_TER:
    terrain_flags.set( ch, argument, (int*) pntr );
    return;

  case VAR_LIQ:
    length = strlen( argument );
    for( n = 0; n < table_max[TABLE_LIQUID]; n++ ) 
      if( !strncasecmp( argument, liquid_table[n].name, length ) )
        break;
    if( n == table_max[ TABLE_LIQUID ] ) {
      if( strncasecmp( argument, "none", length ) ) {
        send( "Unknown liquid.\r\n", ch );
        return;
      } 
      n = -1;
    }
    *pntr = (void *) n;
    pntr = (void **) table_pntr( i,0,j );
    sprintf( tmp, "%s on %s %s set to %s.\r\n", column_name( i, k ), table_name[ i ], (char*) *pntr, n == -1 ? "none" : liquid_table[n].name );
    break;

  case VAR_STAT:
    length = strlen( argument );
      for( n = 0; n < 28; n++ )
        if( !strncasecmp( argument, rstat_name[n], length ) )
          break;
      if( n == 28 ) {
        if( strncasecmp( argument, "none", length ) ) {
          send( "Unknown stat.\r\n", ch );
          return;
        }
        n = -1;
      }
      *pntr = (void*) n;
      pntr = (void **) table_pntr( i, 0, j );
      sprintf( tmp, "%s on %s %s set to %s.\r\n", column_name( i, k ), table_name[ i ], (char*) *pntr, n == -1 ? "none" : rstat_name[n] );
      break;
  }

  *tmp = toupper( *tmp );
  send( ch, tmp );  
}


void do_tstat( char_data* ch, char* )
{
  char                 tmp  [ FOUR_LINES ];
  char                 buf  [ FOUR_LINES ];
  wizard_data*         imm  = (wizard_data*) ch;
  obj_clss_data*  obj_clss;
  species_data*           mob;
  int                table  = imm->table_edit[0];
  int                entry  = imm->table_edit[1];
  int            col, k, n;
  void**              pntr;
  bool              single;

  single = ( table != TABLE_SKILL && table != TABLE_CLSS
    && table != TABLE_PLYR_RACE && table != TABLE_SPELL_DATA
    && table != TABLE_NATION );

  for( k = col = 0; column_version( table, k ) >= 0; k++, col++ ) {
    pntr = (void **) table_pntr( table,k,entry );
    sprintf( tmp, "%15s : ", column_name( table, k ) );

    switch( column_type( table,k ) ) {
    case VAR_FORMULA :
    case VAR_CHAR :
    case CNST_CHAR :
    case VAR_FUNC :
    case VAR_BLANK :
      strcpy( &tmp[18], (char*) *pntr );
      break;
    
    case VAR_BOOL :
      sprintf( &tmp[18], "%s", *pntr ? "true" : "false" ); 
      break;
   
    case VAR_TEMP :
    case VAR_INT :
      sprintf( &tmp[18], "%d", (int) *pntr );
      break;
 
    case VAR_CENT :
      n = (int) *pntr; 
      sprintf( &tmp[18], "%.2f", (float) n/100 );
      break;
 
    case VAR_AFF_LOC :
      strcpy( &tmp[18], affect_location[ int( *pntr ) ] );
      break;

    case VAR_SIZE :
      strcpy( &tmp[18], size_name[ (int) *pntr ] );
      break;

    case VAR_SCAT :
      if( int( *pntr ) >= 0 && int( *pntr ) < MAX_SKILL_CAT )
        strcpy( &tmp[18], skill_cat_name[ (int) *pntr ] );
      else
        sprintf( &tmp[18], "%d (Bug)", int( *pntr ) );
      break;

    case VAR_STYPE :
      if( int( *pntr ) >= 0 && int( *pntr ) < MAX_STYPE )
        strcpy( &tmp[18], stype_name[ (int) *pntr ] );
      else
        sprintf( &tmp[18], "%d (Bug)", int( *pntr ) );
      break;

    case VAR_ELEMENT:
      if( int( *pntr ) >= 0 && int( *pntr ) < MAX_ATTACK+1 )
        strcpy( &tmp[18], element_name[ (int) *pntr ] );
      else
        sprintf( &tmp[18], "%d (Bug)", int( *pntr ) );
      break;

    case VAR_SKILL :
      sprintf( &tmp[18], "%s", (int) *pntr == -1 ? "none" : skill_table[(int) *pntr].name );
      break;

    case VAR_SA :
      if( int( *pntr ) >= 0 && int( *pntr ) < table_max[ TABLE_SPELL_ACT ] )   
        strcpy( &tmp[18], spell_act_table[(int) *pntr].name );
      else
        sprintf( &tmp[18], "%d (Bug)", int( *pntr ) );
      break;

    case VAR_CC :
      strcpy( &tmp[18], cmd_cat_table[(int) *pntr].name );
      break;

    case VAR_POS :
      strcpy( &tmp[18], position_name[ int(*pntr) ] );
      break;

    case VAR_SEX :
      strcpy( &tmp[18], sex_name[ int(*pntr) ] );
      break;

    case VAR_OBJ :
      obj_clss = get_obj_index( int( *pntr ) );
      strcpy( &tmp[18], obj_clss == NULL ? "nothing" : obj_clss->Name( int( *(pntr+1) ) ) );
      break;

    case VAR_MOB: {
      mob = get_species( int( *pntr ) );
      strcpy( &tmp[18], mob == NULL ? "nothing" : mob->descr->name == empty_string ? mob->descr->singular : mob->descr->name );
      break;
    }

    case VAR_AFF :
      affect_flags.sprint( &tmp[18], (int*) pntr );
      break;

    case VAR_ALIGN :
      abv_align_flags.sprint( &tmp[18], (int*) pntr );
      break;

    case VAR_RACE :
      race_flags.sprint( &tmp[18], (int*) pntr );
      break;

    case VAR_CLSS :
      clss_flags.sprint( &tmp[18], (int*) pntr );
      break;

    case VAR_LANG :
      lang_flags.sprint( &tmp[18], (int*) pntr );
      break;

    case VAR_PERM :
      permission_flags.sprint( &tmp[18], (int*) pntr );
      break;

    case VAR_LOC :
      if( *((int*) pntr) == 0 ) 
        strcpy( &tmp[18], "anywhere" ); 
      else
        location_flags.sprint( &tmp[18], (int*) pntr );
      break;

    case VAR_TER :
      if( *((int*) pntr) == 0 ) 
        strcpy( &tmp[18], "anywhere" ); 
      else
        terrain_flags.sprint( &tmp[18], (int*) pntr );
      break;

    case VAR_COLOR:
      if( int( *pntr ) >= 0 && int( *pntr ) < MAX_COLOR ) {
        sprintf( buf, "%s%s%s", color_code( ch, (int) *pntr ), color_fields[ (int) *pntr ], normal( ch ) ); 
        strcpy( &tmp[18], buf );
      }
      else
        sprintf( &tmp[18], "%d (Bug)", int( *pntr ) );
      break;

    case VAR_MOVE_TYPE:
      if( int( *pntr ) >= 0 && int( *pntr ) < MAX_MOVE )
        strcpy( &tmp[18], leaving_verb[ (int) *pntr ] );
      else
        sprintf( &tmp[18], "%d (Bug)", int( *pntr ) );
      break;
      
    case VAR_LIQ :
      sprintf( &tmp[18], "%s", (int) *pntr == -1 ? "none" : liquid_table[(int) *pntr].name );
      break;

    case VAR_STAT:
      sprintf( &tmp[18], "%s", (int) *pntr == -1 ? "none" : rstat_name[(int) *pntr] );
      break;

    case VAR_DICE :
      sprintf_dice( &tmp[18], int( *pntr ) );
      break; 

    case VAR_LEECH :
      sprintf_leech( &tmp[18], int( *pntr ) );
      break;
    }

    if( single ) {
      strcat( tmp, "\r\n" );
      page( ch, tmp );
    } else {
      if( strlen( tmp ) > 30 ) {
        page( ch, "%s     %s\r\n", col%2 ==1 ? "\r\n" : "", tmp );  
        col = 1;
      } else if( col%2 != 1 ) 
        page( ch, "     %-30s", tmp );
      else
        page( ch, "%s\r\n", tmp );
    }
  }

  if( !single && col%2 == 1 )
    page( ch, "\r\n" );

  if( table != TABLE_SPELL_ACT )
    return;

  bool found = FALSE;

  page( ch, "\r\nUsed By:\r\n" );

  for( int i = 0; i < SPELL_COUNT; i++ )
    for( int j = 0; j < spell_table[i].wait; j++ )
      if( spell_table[i].action[j] == entry ) {
        found = TRUE;
        page( ch, "  %s\r\n", spell_table[i].name );
        break;
      }

  if( !found ) 
    page( ch, "  nothing\r\n" );
}


/*
 *   ADDING/REMOVING ENTRIES
 */


void init_entry( int i, char* name )
{
  void**  pntr;
  int        j  = table_max[i];
  int        k;

  pntr = (void **) table_pntr( i,0,j );
  *pntr = (void*) alloc_string( name, MEM_TABLE );

  for( k = 1; column_version( i, k ) >= 0; k++ ) { 
    pntr = (void**) table_pntr( i,k,j );
    switch( column_type( i,k ) ) {
    case VAR_CHAR :
    case VAR_FUNC :
      *pntr = (void*) empty_string;
      break;

    case VAR_INT :
    case VAR_SA :
    case VAR_SIZE :
    case VAR_CC :
    case VAR_OBJ :
    case VAR_STYPE :
    case VAR_MOB:
    case VAR_ALIGN:
    case VAR_CLSS:
      *pntr = 0;
      break;

    case VAR_PERM:
      *(pntr+1) = (void*) 8;
      break;
    }
  }

  table_max[i]++;

  return;
}


void sort_table( char_data* ch, int table )
{
  int  max  = table_max[table];
  int  min;
  int    i;
  int    j;

  if( table > TABLE_SOC_VYAN  && table != TABLE_COMMAND && table != TABLE_SPELL_ACT ) {
    send( ch, "That would destroy the universe as we now know it.\r\n" );
    return;
  }

  for( i = 0; i < max-1; i++ ) {
    min = i;
    for( j = i; j < max; j++ ) 
      if( strcasecmp( entry_name( table, j ), entry_name( table, min ) ) < 0 )
        min = j;
      
      if( min != i ) 
        swap_entries( table, i, min );
  }

  if( table == TABLE_COMMAND )
    init_commands();

  send( ch, "%s table sorted.\r\n", table_name[table] );
}


/*
 *  DELETING ENTRIES
 */


bool extract_nation( char_data* ch, int nation )
{
  int i;

  if( nation+1 != table_max[ TABLE_NATION ] ) {
    send( ch, "You may only delete the last entry of that table.\r\n" );
    return FALSE;
  }

  for( i = 0; i < MAX_SPECIES; i++ )
    if( species_list[i] != NULL && species_list[i]->nation == nation ) {
      send( ch, "Mobs belonging to that nation still exist.\r\n" );
      return FALSE;
    }

  return TRUE;
}


void remove_entry( char_data* ch, int table, int entry )
{
  wizard_data*  imm;
  int i;

  if( table > TABLE_SOC_VYAN && table != TABLE_COMMAND
    && table != TABLE_NATION && table < TABLE_ALIGNMENT 
    && table != TABLE_SERVICES && table != TABLE_MOB_DIFFICULTY ) {
    send( ch, "That table cannot have entries deleted from it.\r\n" );
    return;
  }

  if( table == TABLE_NATION && !extract_nation( ch, entry ) ) 
    return;

  send( ch, "Entry %s removed from table %s.\r\n", entry_name( table, entry ), table_name[table] );

  for( i = 0; i < player_list; i++ ) {
    if( player_list[i]->Is_Valid( )
      && ( imm = wizard( player_list[i] ) ) != NULL
      && imm->table_edit[0] == table
      && imm->table_edit[1] == entry ) {
      send( imm, "The table entry you were editing was deleted.\r\n" );
      imm->table_edit[0] = 0;
      imm->table_edit[1] = 0;
    }
  }
   
  for( i = entry+1; i < table_max[table]; i++ )
    swap_entries( table, i-1, i );

  table_max[table]--;
}


/*
 *   SWAPPING OF ENTRIES
 */


void swap_entries( int table, int e1, int e2 )
{
  int           size  = table_size[table];
  char*         temp;
  void*        pntr1  = table_pntr( table,0,e1 );
  void*        pntr2  = table_pntr( table,0,e2 );
  wizard_data*   imm;

  temp = new char[size];

  memcpy( temp,  pntr1, size );
  memcpy( pntr1, pntr2, size );
  memcpy( pntr2, temp,  size );

  delete [] temp;

  for( int i = 0; i < player_list; i++ )
    if( player_list[i]->Is_Valid( ) 
      && ( imm = wizard( player_list[i] ) ) != NULL
      && imm->table_edit[0] == table )
      exchange( imm->table_edit[1], e1, e2 ); 
    
  if( table == TABLE_SPELL_ACT ) 
    for( int i = 0; i < SPELL_COUNT; i++ ) 
      for( int j = 0; j < 5; j++ )
        exchange( spell_table[i].action[j], e1, e2 );
}


