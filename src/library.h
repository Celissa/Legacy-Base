/*
 *   VARIABLES
 */


extern char_data*   var_mob;  
extern char_data*   var_ch;
extern char_data*   var_rch;
extern char_data*   var_victim;
extern char*        var_arg;
extern room_data*   var_room;
extern obj_data*    var_obj;
extern obj_data*    var_robj;
extern obj_data*    var_container;
extern int          var_i;
extern int          var_j;


/*
 *   OPERATORS
 */


cfunc code_and;
cfunc code_eor;
cfunc code_or;
cfunc code_minus_equal;
cfunc code_plus_equal;
cfunc code_set_equal;
cfunc code_not_equal;
cfunc code_is_equal;
cfunc code_gt;
cfunc code_lt;
cfunc code_le;
cfunc code_ge;
cfunc code_mod;

/*
 *   SCRIPT FUNCTIONS
 */


cfunc code_acode;
cfunc code_act_area;
cfunc code_act_notchar;
cfunc code_act_notvict;
cfunc code_act_room;
cfunc code_act_neither;
cfunc code_act_tochar;
cfunc code_add;
cfunc code_add_pet;
cfunc code_assign_quest;
cfunc code_attack;
cfunc code_attack_element;
cfunc code_attack_acid;
cfunc code_attack_cold;
cfunc code_attack_fire;
cfunc code_attack_shock;
cfunc code_attack_room;
cfunc code_attack_weapon;
cfunc code_can_buy_pet;
cfunc code_can_climb;
cfunc code_can_fly;
cfunc code_can_handle;
cfunc code_can_see;
cfunc code_can_tame_pet;
cfunc code_cast_spell;
cfunc code_cflag;
cfunc code_char_in_room;
cfunc code_class;
cfunc code_close;
cfunc code_coin_value;
cfunc code_current_hit;
cfunc code_current_mana;
cfunc code_current_move;
cfunc code_damage;
cfunc code_damage_acid;
cfunc code_damage_cold;
cfunc code_damage_fire;
cfunc code_damage_shock;
cfunc code_dam_message;
cfunc code_dam_message_acid;
cfunc code_dam_message_cold;
cfunc code_dam_message_fire;
cfunc code_dam_message_shock;
cfunc code_dice;
cfunc code_dispel;
cfunc code_do_spell;
cfunc code_doing_quest;
cfunc code_done_quest;
cfunc code_drain_stat;
cfunc code_drain_exp;
cfunc code_find_room;
cfunc code_find_skill;
cfunc code_find_stat;
cfunc code_get_room;
cfunc code_group;
cfunc code_has_obj;
cfunc code_has_quest;
cfunc code_has_type;
cfunc code_weapon_type;
cfunc code_heal;
cfunc code_improve_skill;
cfunc code_inflict;
cfunc code_inflict_acid;
cfunc code_inflict_cold;
cfunc code_inflict_fire;
cfunc code_inflict_shock;
cfunc code_assist_char;
cfunc code_is_in_clan;
cfunc code_is_in_guild;
cfunc code_init_attack;
cfunc code_interpret;
cfunc code_is_affected;
cfunc code_add_affect;
cfunc code_is_berserk;
cfunc code_is_blind;
cfunc code_is_dark_race;
cfunc code_is_drunk;
cfunc code_is_exhausted;
cfunc code_leader;
cfunc code_fighting_who;
cfunc code_is_locked;
cfunc code_is_fighting;
cfunc code_is_follower;
cfunc code_is_hidden;
cfunc code_is_invisible;
cfunc code_is_light_race;
cfunc code_is_mob;
cfunc code_is_mounted;
cfunc code_is_name;
cfunc code_is_night;
cfunc code_is_open;
cfunc code_is_pet;
cfunc code_is_player;
cfunc code_is_resting;
cfunc code_is_searching;
cfunc code_is_silenced;
cfunc code_is_sleeping;
cfunc code_is_standing;
cfunc code_is_wearing;
cfunc code_junk_mob;
cfunc code_junk_obj;
cfunc code_lock;
cfunc code_max_hit;
cfunc code_max_mana;
cfunc code_max_move;
cfunc code_mpcode;
cfunc code_mload;
cfunc code_mob_in_room;
cfunc code_modify_mana;
cfunc code_modify_move; 
cfunc code_mult;
cfunc code_num_in_room;
cfunc code_num_mob;
cfunc code_num_mob_mud;
cfunc code_find_mob;
cfunc code_oload;
cfunc code_obj_name;
cfunc code_obj_in_room;
cfunc code_obj_in_thing;
cfunc code_obj_index;
cfunc code_obj_to_char;
cfunc code_obj_to_cont;
cfunc code_obj_to_room;
cfunc code_obj_value;
cfunc code_opcode;
cfunc code_open;
cfunc code_open_object;
cfunc code_close_object;
cfunc code_lock_object;
cfunc code_unlock_object;
cfunc code_pet_in_room;
cfunc code_plague;
cfunc code_player_in_room;
cfunc code_players_area;
cfunc code_players_room;
cfunc code_poison;
cfunc code_position;
cfunc code_race;
cfunc code_random;
cfunc code_rand_char;
cfunc code_rand_player;
cfunc code_random_enemy;
//cfunc code_rand_tank;
cfunc code_recog;
cfunc code_religion;
cfunc code_remove_cflag;
cfunc code_remove_rflag;
cfunc code_reputation;
cfunc code_resistance;
cfunc code_rflag;
cfunc code_send_to_area;
cfunc code_send_to_char;
cfunc code_send_to_room;
cfunc code_set_cflag;
cfunc code_set_religion;
cfunc code_set_rflag;
cfunc code_sex;
cfunc code_show;
cfunc code_size;
cfunc code_species;
cfunc code_spoison;
cfunc code_summon;
cfunc code_tell;
cfunc code_time;
cfunc code_transfer;
cfunc code_transfer_all;
//cfunc code_transfer_obj;
cfunc code_unlock;
cfunc code_untrap;
cfunc code_update_quest;
cfunc code_wait;
cfunc code_weight;

cfunc code_set_integer;
cfunc code_get_integer;

cfunc code_is_good;
cfunc code_is_neutral;
cfunc code_is_evil;
cfunc code_is_lawful;
cfunc code_is_n2;
cfunc code_is_chaotic;
cfunc code_vnum;
cfunc code_disarm;
cfunc code_is_pcontrolled;

cfunc code_atoi;
cfunc code_itoa;
cfunc code_wearing;
cfunc code_obj_level;
cfunc code_obj_condition;
cfunc code_obj_stat;
cfunc code_modify_obj;
cfunc code_object_in_obj;
cfunc code_object_type;
cfunc code_rtable_object;
cfunc code_execute_wander;
cfunc code_find_player;
cfunc code_set_exit_flag;
cfunc code_rem_exit_flag;
cfunc code_dance;
cfunc code_dance_end;
cfunc code_song;
cfunc code_song_end;
cfunc code_in_leader_room;
