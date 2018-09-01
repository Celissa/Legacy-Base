/*
 *   Spells takes the arguments char_data, char_data, object, level  
 *
 *    Casting: caster, subject, object, level, -1
 *    Zapping: zapper, subject, object, level, duration
 *   Throwing: caster, subject, object, level, -2
 *   Quaffing: NULL, drinker, NULL, level, duration
 *    Dipping: dipper, NULL, object, level, -3
 *   Reacting: NULL, filler, object, level, -4
 *
 */


/*
 *   CASTING
 */


class Cast_Data
{
 public:
  cast_data*      next;
  int             mana;
  int            spell;
  int            times;
  int             wait;
  thing_data*   target;
//  room_data*      room;
  bool         prepare;
  obj_data*    reagent  [ MAX_SPELL_WAIT ];
  int        cast_type;  // what sort of cast is this (concoct, poison, spell, services)
  obj_data*       tool [ MAX_SPELL_WAIT ];
  char_data*   customer; // for use in services
  
  Cast_Data( );
  ~Cast_Data( );
};


/*
 *   LEECH ROUTINES
 */


void   sprintf_leech    ( char*, int );
int    leech_mana       ( int, int, int = 0 );
int    leech_regen      ( int, int, int = 0 );
int    duration         ( int, int, int = 0 );
void   remove_leech     ( affect_data* );
void   remove_leech     ( char_data* );
int    leech_regen      ( char_data* );
int    leech_max        ( char_data* );
int    prep_max         ( char_data* );

/*
 *   ???
 */



bool          null_caster      ( char_data*, int );
bool          null_corpse      ( obj_data*, int );
bool          affected_flames  ( char_data* );
bool          affected_cold    ( char_data* );
bool          affected_holy    ( char_data*, bool = false );
bool          affected_shock   ( char_data* );
bool          affected_acid    ( char_data* );
affect_data*  find_affect( char_data*, int );

int          spell_damage     ( int, int, int = 0 );
void         disrupt_spell    ( char_data*, bool = FALSE );
cast_data*   has_prepared     ( char_data*, int );
bool         makes_save       ( char_data*, char_data*, int, int, int = 0 );
bool         get_target       ( char_data*, cast_data*, char* );
bool         has_reagents     ( char_data*, cast_data* );
bool         can_replicate    ( char_data*, obj_data* obj );
void         spell_affect     ( char_data*, char_data*, int, int, int, int, int = 0 );
//void         spell_affect     ( char_data*, char_data*, int, int, int, char*, int = 0 ); 
void         physical_affect  ( char_data*, char_data*, int, int, int = 0 );
bool         has_ingredients  ( char_data*, cast_data* );
bool         has_tools        ( char_data*, cast_data* );
bool         is_undead        ( char_data* );

int          find_spell( char_data*, char*, int );
int          mana_cost( char_data*, int );
int          song_cost( char_data*, int );
int          move_cost( char_data*, int );

bool         is_dancing( char_data* );
bool         is_singing( char_data* );
int          dance_move( char_data* );
void         end_dance( char_data*, bool = true );
int          song_leech( char_data* );
void         end_song( char_data*, bool = true );
void         check_song( char_data* ch );


/*
 *   SPELL TYPES
 */


#define STYPE_OFFENSIVE             0
#define STYPE_PEACEFUL              1
#define STYPE_SELF_ONLY             2
#define STYPE_OBJECT                3
#define STYPE_ROOM                  4
#define STYPE_CONTINENT             5
#define STYPE_PEACEFUL_OTHER        6
#define STYPE_WEAPON                7
#define STYPE_DRINK_CON             8
#define STYPE_MOB_ONLY              9
#define STYPE_ANNOYING             10
#define STYPE_CORPSE               11
#define STYPE_RECALL               12
#define STYPE_WEAPON_ARMOR         13
#define STYPE_REPLICATE            14
#define STYPE_ASTRAL               15
#define STYPE_NON_COMBAT_HEALING   16
#define STYPE_CROSS_CONTINENT      17
#define STYPE_UNIVERSAL            18
#define STYPE_ANNOYING_EVIL        19
#define STYPE_ANNOYING_GOOD        20
#define STYPE_ANNOYING_UNDEAD      21
#define STYPE_TREE                 22
#define MAX_STYPE                  23


extern const char* stype_name [ MAX_STYPE ];


/* 
 *   SPELL FUNCTIONS
 */

void   concoct_update            ( char_data* );
void   poison_concoct_update     ( char_data* );
void   scribe_update             ( char_data* );
void   spell_update              ( char_data* );
void   heal_victim               ( char_data*, char_data*, int );
void   service_update            ( char_data* );
void   fletch_update             ( char_data* );
void   empower_update            ( char_data* );

spell_func spell_acid_arrow;
spell_func spell_acid_blast;
spell_func spell_acid_storm;
spell_func spell_alter_magnetism;
spell_func spell_animate_clay;
spell_func spell_animate_dead; 
spell_func spell_amnesia;
spell_func spell_armor;
spell_func spell_astral_gate;
//spell_func spell_augury;
spell_func spell_balm;
spell_func spell_banishment;
spell_func spell_barkskin;
spell_func spell_bless;
spell_func spell_blind;
spell_func spell_blinding_light;
spell_func spell_blink;
spell_func spell_burning_hands;
spell_func spell_call_lightning;
spell_func spell_calm;
spell_func spell_chain_lightning;
spell_func spell_chilling_touch;
spell_func spell_chromatic_orb;
spell_func spell_confuse;
spell_func spell_continual_light;
spell_func spell_create_food;
spell_func spell_create_light;
spell_func spell_create_water;
spell_func spell_cure_light;
spell_func spell_cure_blindness;
spell_func spell_cure_critical;
spell_func spell_cure_disease;
spell_func spell_cure_poison;
spell_func spell_cure_serious;
spell_func spell_detect_invisible; 
spell_func spell_detect_magic;
spell_func spell_displace; 
spell_func spell_drain_life;
spell_func spell_eagle_eye;
spell_func spell_fear;
spell_func spell_findpath;
spell_func spell_find_familiar;
spell_func spell_find_mount;
spell_func spell_fireball;
spell_func spell_fire_shield;
spell_func spell_flame_strike;
spell_func spell_float;
spell_func spell_fly;
spell_func spell_freeze;
spell_func spell_gem_burst;
spell_func spell_gift_of_tongues;
spell_func spell_greater_animation;
spell_func spell_hallucinate;
spell_func spell_haste;
spell_func spell_heal;
spell_func spell_hoar_frost;
spell_func spell_holy_wrath;
spell_func spell_horrid_wilting;
spell_func spell_ice_storm;
spell_func spell_identify;
spell_func spell_ignite_weapon;
spell_func spell_invulnerability;
spell_func spell_know_alignment;
spell_func spell_lesser_summoning;
spell_func spell_lightning_bolt;
spell_func spell_magic_mapping;
spell_func spell_magic_missile;
spell_func spell_maelstrom;
spell_func spell_minor_enchantment;
spell_func spell_major_enchantment;
spell_func spell_mists_sleep;
spell_func spell_mind_blade;
spell_func spell_neutralize;
spell_func spell_ogre_strength;
spell_func spell_pass_door;
//spell_func spell_polymorph;
spell_func spell_prismic_missile;
spell_func spell_protect_evil;
spell_func spell_protect_good;
spell_func spell_protect_life;
spell_func spell_recall;
spell_func spell_remove_curse;
spell_func spell_replicate;
spell_func spell_request_ally;
spell_func spell_resist_acid;
spell_func spell_resist_cold;
spell_func spell_resist_fire;
spell_func spell_resist_shock;
//spell_func spell_resurrect;
spell_func spell_revitalize;
spell_func spell_sanctify;
spell_func spell_sense_danger;
spell_func spell_sense_life;
spell_func spell_shock;
spell_func spell_silence;
spell_func spell_slay; 
spell_func spell_sleep;
spell_func spell_slow; 
spell_func spell_summon;
spell_func spell_surcease;
spell_func spell_tame;
spell_func spell_restoration;
spell_func spell_thorn_shield;
//spell_func spell_transfer;
spell_func spell_true_sight;
spell_func spell_turn_undead;
spell_func spell_vitality;
spell_func spell_water_breathing;
spell_func spell_web;
spell_func spell_acid_arrow;
spell_func spell_youth;

spell_func spell_group_serious;
spell_func spell_cause_light; 
spell_func spell_cause_serious; 
spell_func spell_cause_critical; 
spell_func spell_harm; 
spell_func spell_faerie_fire; 
spell_func spell_invisibility; 
spell_func spell_protection_plants; 
spell_func spell_curse; 
spell_func spell_locust_swarm; 
spell_func spell_poison_cloud; 
spell_func spell_detect_hidden; 
spell_func spell_detect_good; 
spell_func spell_detect_evil; 
spell_func spell_infravision; 
spell_func spell_mystic_shield; 

spell_func spell_ion_shield;
spell_func spell_meteor_swarm;
spell_func spell_ice_lance;
spell_func spell_purify;
//spell_func spell_wither;
spell_func spell_construct_golem;
spell_func spell_conflagration;
spell_func spell_conjure_elemental;
spell_func spell_poultice;
//spell_func spell_scry;
spell_func spell_create_feast;
//spell_func spell_hawks_view;
spell_func spell_group_critical;
spell_func spell_obscure;
spell_func spell_companions_strength;
spell_func spell_briartangle;
spell_func spell_ironskin;
spell_func spell_vigor;
spell_func spell_gryphons_watch;
spell_func spell_universal_gate;
spell_func spell_restore_item;
spell_func spell_earthquake;
spell_func spell_holy_aura;
spell_func spell_unholy_aura;
spell_func spell_greater_summon;
spell_func spell_unceasing_vigilance;
spell_func spell_holy_word;
spell_func spell_unholy_word;
spell_func spell_holy_flail;
spell_func spell_unholy_flail;
spell_func spell_lesser_mount;
spell_func spell_divine_might;
spell_func spell_summon_dragon;
spell_func spell_disperse_undead;
spell_func spell_bone_dart;
spell_func spell_skull_watch;
spell_func spell_gift_night;
spell_func spell_blight;
spell_func spell_ghost_armor;
spell_func spell_bone_wrench;
spell_func spell_starving_madness;
spell_func spell_doom;
spell_func spell_ghost_chains;
spell_func spell_withering_flesh;
spell_func spell_summon_cadaver;
spell_func spell_shield_bones;
spell_func spell_mind_crush;
spell_func spell_weapon_blight;
spell_func spell_destructive_touch;
spell_func spell_bane;
spell_func spell_mind_destruction;
spell_func spell_shuddering_touch;
spell_func spell_dead_man_eyes;
spell_func spell_soul_tear;
spell_func spell_summon_undead;
spell_func spell_death_mask;
spell_func spell_volley_bones;
spell_func spell_blood_bond;
spell_func spell_death_skin;
spell_func spell_desolation;
spell_func spell_bane_fiend;
spell_func spell_forsaken;
spell_func spell_feast_life;
spell_func spell_mind_blank;
spell_func spell_bless_taraskatt;
spell_func spell_walk_damned;
spell_func spell_unholy_fire;
spell_func spell_lichdom;
spell_func spell_decaying_touch;
spell_func spell_blood_fiend;
spell_func spell_feast_of_dead;
spell_func spell_mind_snap;
spell_func spell_skin_of_cadaver;
spell_func spell_skin_wrench;
spell_func spell_dark_chill;
spell_func spell_gloom;
spell_func spell_corpse_light;
spell_func spell_return_the_dead;
spell_func spell_corpse_harvest;
spell_func spell_ghost_shield;
spell_func spell_demon_shield;
spell_func spell_body_rot;
spell_func spell_ethereal_bridge;
spell_func spell_shatter_resolve;
spell_func spell_liquify_bones;
spell_func spell_mind_blow;
spell_func spell_enfeeblement;
spell_func spell_simulcrum;
spell_func spell_toad;
spell_func spell_brook;
spell_func spell_war;
spell_func spell_summer;
spell_func spell_winter;
spell_func spell_wheat;
spell_func spell_battle;
spell_func spell_buck;
spell_func spell_serenity;
spell_func spell_shadow;
spell_func spell_bark_wolf;
spell_func spell_howl_wolf;
spell_func spell_bale_howl;
spell_func spell_harpy_screech;
spell_func spell_autumn;
spell_func spell_summer_equinox;
spell_func spell_winter_equinox;
spell_func spell_seasons_mastery;
spell_func spell_mire;
spell_func spell_quicksand;
spell_func spell_carnage;
spell_func spell_eternal_battle;
spell_func spell_blurring;
spell_func spell_tireless;
spell_func spell_ox;
spell_func spell_seraphim;
spell_func spell_swift;
spell_func spell_walrus;
spell_func spell_celestial;
spell_func spell_cheetah;
spell_func spell_river;
spell_func spell_harvest;
spell_func spell_renewal;
spell_func spell_spring;
spell_func spell_bug;
spell_func spell_natures_blessing;
spell_func spell_natures_imbued;
spell_func spell_natures_infuse;
spell_func spell_natures_spirit;
spell_func spell_natures_calling;
spell_func spell_calling_beast;
spell_func spell_souls_balm;
spell_func spell_souls_respite;
spell_func spell_souls_recovery;
spell_func spell_souls_return;
spell_func spell_deaths_recall;
spell_func spell_body_snatch;
spell_func spell_summon_corpse;
spell_func spell_theft_of_death;
spell_func spell_sirens_cry;
spell_func spell_wail_mourning;
spell_func spell_wail_banshee;
spell_func spell_serenade;
spell_func spell_enchanted_sleep;
spell_func spell_lullaby;
spell_func spell_dragons_stance;
spell_func spell_bears_stance;
spell_func spell_cats_stance;
spell_func spell_swirling_blades;
spell_func spell_iron_resolve;
spell_func spell_whirling_death;
spell_func spell_flame_shield;
spell_func spell_fiery_shield;
spell_func spell_inferno_shield;
spell_func spell_frost_shield;
spell_func spell_hoar_shield;
spell_func spell_absolute_zero;
spell_func spell_spark_shield;
spell_func spell_shock_shield;
spell_func spell_lightning_shield;
spell_func spell_erosion_shield;
spell_func spell_corrosive_shield;
spell_func spell_caustic_shield;
spell_func spell_spring_equinox;
spell_func spell_harvest_equinox;
spell_func spell_wicked_aura;
spell_func spell_dreadful_aura;
spell_func spell_evil_aura;
spell_func spell_blessed_aura;
spell_func spell_hallowed_aura;
spell_func spell_saintly_aura;
spell_func spell_bone_prison;
spell_func spell_life_tap;
spell_func spell_wraith_form;
spell_func spell_natures_shielding;
spell_func spell_defoliation;
spell_func spell_green_finger;
spell_func spell_natures_boon;
spell_func spell_tree_form;
spell_func spell_natures_fury;
spell_func spell_parching_grasp;
spell_func spell_entangle;
spell_func spell_harness_winds;
spell_func spell_summer_touch;
spell_func spell_dancing_tallow;
spell_func spell_winters_gale;
spell_func spell_winters_touch;
spell_func spell_hail_stones;
spell_func spell_trail_blaze;
spell_func spell_path_find;
spell_func spell_owls_gift;
spell_func spell_caress_nature;
spell_func spell_bears_gift;
spell_func spell_snakes_gift;
spell_func spell_lions_gift;
spell_func spell_caress_gaia;
spell_func spell_hawks_gift;
spell_func spell_wyrms_gift;
spell_func spell_wilderness_lore;
spell_func spell_animal_companion;
spell_func spell_bramble_skin;
spell_func spell_thistle_skin;
spell_func spell_breath_serpent;
spell_func spell_autumn_touch;
spell_func spell_frog_tongue;
spell_func spell_acidic_water;
spell_func spell_acid_rain;
spell_func spell_electric_eel;
spell_func spell_electrical_storm;
spell_func spell_fire_ant;
spell_func spell_river_magma;
spell_func spell_circle_healing;
spell_func spell_druidic_ring;
spell_func spell_good_berry;
spell_func spell_deep_energy;
spell_func spell_tree_gate;
spell_func spell_blessed_fruit;
spell_func spell_gift_of_the_land;
spell_func spell_caress_of_spring;
spell_func spell_breath_of_life;
spell_func spell_embrace_of_the_world;
spell_func spell_wind_walk;
spell_func spell_whirlwind;
spell_func spell_sunburst;
spell_func spell_poison_touch;
spell_func spell_spike_growth;
spell_func spell_insect_plague;
spell_func spell_water_walk;
spell_func spell_obscuring_mist;
spell_func spell_summon_swarm;
spell_func spell_shillelagh;
spell_func spell_firefly;
spell_func spell_natures_health;
spell_func spell_companion_health;
spell_func spell_natures_restoration;
spell_func spell_touch_healing;
spell_func spell_withering_blast;
spell_func spell_inferno;
spell_func spell_solid;
spell_func spell_static_charge;
