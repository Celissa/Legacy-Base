#include "system.h"


const char* arg_type_name[] = { "none", "any", "string", "integer",
  "character", "object", "room", "direction", "nation", "skill", "rflag",
  "stat", "class", "religion", "race", "thing", "element", "wearpos", "layer",
  "affect", "visible", "range", "valid", "clan", "object.value", "object.type",
  "dflag", "dance", "affect.location", "position" };


/*
 *   FUNCTION TABLE
 */


#define nn NONE
#define st STRING
#define ch CHARACTER
#define ob OBJECT
#define in INTEGER
#define rm ROOM
#define sk SKILL
#define sa STAT
#define al AFF_LOCATION
#define di DIRECTION
#define rf RFLAG
#define na NATION
#define cl CLASS
#define re RELIGION
#define rc RACE
#define th THING
#define el ELEMENT
#define wp WEARPOS
#define ly LAYER
#define ra RANGE
#define af AFFECT
#define vi VISIBLE
#define va VALIDITY
#define ov OBJ_VALUE
#define ot OBJ_TYPE
#define df DFLAG_TYPE
#define da DANCE
#define cf Cfunc_Type
#define po POSITION


// args doesn't work that way - when it +='s an arg (eg ch, ch) it just makes it a single 'ch'
// because it's already on the list... fux


Cfunc_Type :: Cfunc_Type ( const char* aname, cfunc* afunc_call, arg_enum atype, ... )
{
  name = aname;
  func_call = afunc_call;
  type = atype;
  args.allow_multiple = true;
  
  va_list arglist;
  va_start( arglist, atype );

  for( int aparam = va_arg( arglist, int ); aparam != nn; aparam = va_arg( arglist, int ) )
    args += (arg_enum) aparam;
}


Cfunc_Type :: ~Cfunc_Type ( )
{
}


cfunc_type cfunc_list [] =
{
  cf( "acode",             &code_acode,             nn, rm, in, nn ),
  cf( "act_area",          &code_act_area,          nn, st, ch, ob, ch, vi, nn ),
  cf( "act_room",          &code_act_room,          nn, st, ch, ob, ch, vi, nn ),
  cf( "act_notchar",       &code_act_notchar,       nn, st, ch, ob, ch, vi, nn ),
  cf( "act_notvict",       &code_act_notchar,       nn, st, ch, ob, ch, nn ),
  cf( "act_tochar",        &code_act_tochar,        nn, st, ch, ob, ch, vi, nn ),
  cf( "act_neither",       &code_act_neither,       nn, st, ch, ob, ch, nn ),
  cf( "add",               &code_add,               in, in, in, in, in, nn ),
  cf( "add_pet",           &code_add_pet,           nn, ch, ch, nn ),
  cf( "add_affect",        &code_add_affect,        nn, ch, af, in, in, nn ),
  cf( "assign_quest",      &code_assign_quest,      nn, ch, in, nn ),
  cf( "atoi",              &code_atoi,              in, st, nn ),
  cf( "attack",            &code_attack,            in, ch, ch, in, st, ra, va, nn ),
  cf( "attack_acid",       &code_attack_acid,       in, ch, ch, in, st, va, nn ),
  cf( "attack_cold",       &code_attack_cold,       in, ch, ch, in, st, va, nn ),
  cf( "attack_element",    &code_attack_element,    in, ch, ch, in, el, st, ra, va, nn ),
  cf( "attack_fire",       &code_attack_fire,       in, ch, ch, in, st, ra, va, nn ),
  cf( "attack_shock",      &code_attack_shock,      in, ch, ch, in, st, ra, va, nn ),
  cf( "attack_room",       &code_attack_room,       nn, ch, in, st, ra, va, nn ),
  cf( "attack_weapon",     &code_attack_weapon,     in, ch, ch, in, st, ra, va, nn ),
  cf( "can_buy_pet",       &code_can_buy_pet,       in, ch, ch, nn ),
  cf( "can_climb",         &code_can_climb,         in, ch, nn ),
  cf( "can_fly",           &code_can_fly,           in, ch, nn ),
  cf( "can_handle",        &code_can_handle,        in, ch, ob, nn ),
  cf( "can_see",           &code_can_see,           in, ch, ch, nn ),
  cf( "can_tame_pet",      &code_can_tame_pet,      in, ch, ch, nn ),
  cf( "cast_spell",        &code_cast_spell,        nn, sk, ch, ch, nn ),
  cf( "cflag",             &code_cflag,             in, in, ch, nn ),
  cf( "char_in_room",      &code_char_in_room,      in, ch, rm, nn ),
  cf( "class",             &code_class,             cl, ch, nn ),
  cf( "close",             &code_close,             nn, rm, di, nn ),
  cf( "coin_value",        &code_coin_value,        in, ob, nn ),
  cf( "current_hp",        &code_current_hit,       in, ch, nn ),
  cf( "current_mana",      &code_current_mana,      in, ch, nn ),
  cf( "current_move",      &code_current_move,      in, ch, nn ),
  cf( "damage",            &code_damage,            in, ch, ch, in, el, st, st, va, nn ),
  cf( "dam_message",       &code_dam_message,       nn, ch, in, st, nn ),
  cf( "dam_message_acid",  &code_dam_message_acid,  nn, ch, in, st, nn ),
  cf( "dam_message_cold",  &code_dam_message_cold,  nn, ch, in, st, nn ),
  cf( "dam_message_fire",  &code_dam_message_fire,  nn, ch, in, st, nn ),
  cf( "dam_message_shock", &code_dam_message_shock, nn, ch, in, st, nn ),
  cf( "dance",             &code_dance,             nn, ch, da, nn ),
  cf( "song",              &code_song,              nn, ch, sk, nn ),
  cf( "dance_end",         &code_dance_end,         nn, ch, da, nn ),
  cf( "song_end",          &code_song_end,          nn, ch, nn ),
  cf( "dice",              &code_dice,              in, in, in, in, nn ),
  cf( "disarm",            &code_disarm,            in, ch, in, nn ),
  cf( "dispel",            &code_dispel,            nn, af, ch, nn ),
  cf( "do_spell",          &code_do_spell,          nn, sk, ch, ch, ob, in, nn ),
  cf( "doing_quest",       &code_doing_quest,       in, ch, in, nn ),
  cf( "done_quest",        &code_done_quest,        in, ch, in, nn ),
  cf( "drain_exp",         &code_drain_exp,         nn, ch, in, nn ),
  cf( "drain_stat",        &code_drain_stat,        nn, ch, al, in, in, nn ),
  cf( "fighting_who",      &code_fighting_who,      ch, ch, nn ), 
  cf( "find_room",         &code_find_room,         rm, in, nn ),
  cf( "find_skill",        &code_find_skill,        in, ch, sk, nn ),
  cf( "find_stat",         &code_find_stat,         in, ch, sa, nn ),
  cf( "find_player",       &code_find_player,       ch, ob, nn ),
  cf( "get_integer",       &code_get_integer,       in, th, st, nn ),
  cf( "get_room",          &code_get_room,          rm, ch, nn ),
  cf( "group",             &code_group,             in, ch, nn ),
  cf( "junk_mob",          &code_junk_mob,          nn, ch, nn ),
  cf( "junk_obj",          &code_junk_obj,          nn, ob, in, nn ),
  cf( "has_obj",           &code_has_obj,           ob, in, ch, nn ),
  cf( "has_quest",         &code_has_quest,         in, ch, in, nn ),
  cf( "has_type",          &code_has_type,          ob, in, ch, nn ),
  cf( "heal",              &code_heal,              nn, ch, in, nn ),
  cf( "itoa",              &code_itoa,              st, in, nn ),
  cf( "improve_skill",     &code_improve_skill,     nn, ch, sk, nn ),
  cf( "assist_char",       &code_assist_char,       st, ch, ch, nn ),
  cf( "is_in_clan",        &code_is_in_clan,        in, ch, st, nn ),
  cf( "is_in_guild",       &code_is_in_guild,       in, ch, st, nn ),
  cf( "inflict",           &code_inflict,           in, ch, ch, in, st, nn ),
  cf( "inflict_acid",      &code_inflict_acid,      in, ch, ch, in, st, nn ),
  cf( "inflict_cold",      &code_inflict_cold,      in, ch, ch, in, st, nn ),
  cf( "inflict_fire",      &code_inflict_fire,      in, ch, ch, in, st, nn ),
  cf( "inflict_shock",     &code_inflict_shock,     in, ch, ch, in, st, nn ),
  cf( "init_attack",       &code_init_attack,       nn, ch, ch, nn ),
  cf( "interpret",         &code_interpret,         nn, ch, st, ch, nn ),
  cf( "is_affected",       &code_is_affected,       in, af, ch, nn ),
  cf( "is_berserk",        &code_is_berserk,        in, ch, nn ),
  cf( "is_blind",          &code_is_blind,          in, ch, nn ),
  cf( "is_chaotic",        &code_is_chaotic,        in, ch, nn ),
  cf( "is_dark_race",      &code_is_dark_race,      in, ch, nn ),
  cf( "is_drunk",          &code_is_drunk,          in, ch, nn ),
  cf( "is_evil",           &code_is_evil,           in, ch, nn ),
  cf( "is_exhausted",      &code_is_exhausted,      in, ch, in, nn ),
  cf( "is_fighting",       &code_is_fighting,       in, ch, nn ),
  cf( "is_follower",       &code_is_follower,       in, ch, nn ),
  cf( "is_good",           &code_is_good,           in, ch, nn ),
  cf( "is_hidden",         &code_is_hidden,         in, ch, nn ),
  cf( "is_invisible",      &code_is_invisible,      in, ch, nn ),
  cf( "is_lawful",         &code_is_lawful,         in, ch, nn ),
  cf( "is_light_race",     &code_is_light_race,     in, ch, nn ),
  cf( "is_locked",         &code_is_locked,         in, rm, di, nn ),
  cf( "is_mob",            &code_is_mob,            in, ch, nn ),
  cf( "is_mounted",        &code_is_mounted,        in, ch, nn ),
  cf( "is_name",           &code_is_name,           in, st, st, nn ),
  cf( "is_n2",             &code_is_n2,             in, ch, nn ),
  cf( "is_neutral",        &code_is_neutral,        in, ch, nn ),
  cf( "is_night",          &code_is_night,          in, nn ),
  cf( "is_open",           &code_is_open,           in, rm, di, nn ),
  cf( "is_pet",            &code_is_pet,            in, ch, nn ),
  cf( "is_player",         &code_is_player,         in, ch, nn ),
  cf( "is_resting",        &code_is_resting,        in, ch, nn ),
  cf( "is_searching",      &code_is_searching,      in, ch, nn ),
  cf( "is_silenced",       &code_is_silenced,       in, ch, nn ),
  cf( "is_sleeping",       &code_is_sleeping,       in, ch, nn ),
  cf( "is_standing",       &code_is_standing,       in, ch, nn ),
  cf( "is_wearing",        &code_is_wearing,        ob, in, ch, nn ),
  cf( "is_pcontrolled",    &code_is_pcontrolled,    in, ch, nn ),
  cf( "leader",            &code_leader,            ch, ch, nn ),
  cf( "lock",              &code_lock,              nn, rm, di, nn ),
  cf( "num_in_room",       &code_num_in_room,       in, rm, nn ),
  cf( "num_mob",           &code_num_mob,           in, in, rm, nn ),
  cf( "num_mob_on_mud",    &code_num_mob_mud,       in, in, nn ),
  cf( "find_mob",          &code_find_mob,          ch, in, nn ),
  cf( "max_hit",           &code_max_hit,           in, ch, nn ),
  cf( "max_mana",          &code_max_mana,          in, ch, nn ),
  cf( "max_move",          &code_max_move,          in, ch, nn ),
  cf( "mpcode",            &code_mpcode,            nn, ch, in, nn ),
  cf( "mload",             &code_mload,             ch, in, rm, nn ),
  cf( "mob_in_room",       &code_mob_in_room,       ch, in, rm, nn ),
  cf( "modify_mana",       &code_modify_mana,       in, ch, in, nn ),
  cf( "modify_move",       &code_modify_move,       in, ch, in, nn ),
  cf( "mult",              &code_mult,              in, in, in, in, nn ),
  cf( "obj_in_room",       &code_obj_in_room,       ob, in, rm, nn ),
  cf( "obj_in_thing",      &code_obj_in_thing,      ob, in, th, nn ),
  cf( "obj_index",         &code_obj_index,         in, ob, nn ),
  cf( "obj_name",          &code_obj_name,          st, ob, nn ),
  cf( "obj_to_char",       &code_obj_to_char,       nn, ob, ch, in, nn ),
  cf( "obj_to_cont",       &code_obj_to_cont,       nn, ob, ob, in, nn ),
  cf( "obj_to_room",       &code_obj_to_room,       nn, ob, rm, in, nn ),
  cf( "obj_level",         &code_obj_level,         in, ob, nn ),
  cf( "obj_condition",     &code_obj_condition,     in, ob, nn ),
  cf( "obj_value",         &code_obj_value,         in, ob, in, nn ),
  cf( "obj_stat",          &code_obj_stat,          in, ob, ov, nn ),
  cf( "obj_in_obj",        &code_object_in_obj,     ob, ob, in, nn ),
  cf( "modify_obj",        &code_modify_obj,        nn, ob, ov, in, nn ),
  cf( "oload",             &code_oload,             ob, in, in, nn ),
  cf( "opcode",            &code_opcode,            nn, ob, in, nn ),
  cf( "open",              &code_open,              nn, rm, di, nn ),
  cf( "open_object",       &code_open_object,       nn, ob, nn ),
  cf( "close_object",      &code_close_object,      nn, ob, nn ),
  cf( "lock_object",       &code_lock_object,       nn, ob, nn ),
  cf( "unlock_object",     &code_unlock_object,     nn, ob, nn ),
  cf( "pet_in_room",       &code_pet_in_room,       ch, in, rm, nn ),
  cf( "plague",            &code_plague,            nn, ch, nn ),
  cf( "player_in_room",    &code_player_in_room,    ch, in, rm, nn ),
  cf( "players_area",      &code_players_area,      in, rm, nn ),
  cf( "players_room",      &code_players_room,      in, rm, nn ),
  cf( "poison",            &code_poison,            nn, ch, nn ),
  cf( "position",          &code_position,          nn, ch, po, in, nn ),
  cf( "race",              &code_race,              rc, ch, nn ),
  cf( "random",            &code_random,            in, in, in, nn ),
  cf( "rand_char",         &code_rand_char,         ch, rm, nn ),
  cf( "rand_player",       &code_rand_player,       ch, rm, nn ),
  cf( "rand_enemy",        &code_random_enemy,      ch, ch, nn ),
//  cf( "rand_tank",         &code_rand_tank,         ch, ch, nn ),
  cf( "recog",             &code_recog,             nn, ch, ch, nn ),
  cf( "religion",          &code_religion,          re, ch, nn ),
  cf( "remove_cflag",      &code_remove_cflag,      nn, in, ch, nn ),
  cf( "remove_rflag",      &code_remove_rflag,      nn, rf, rm, nn ),
  cf( "reputation",        &code_reputation,        in, ch, na, nn ),
  cf( "resistance",        &code_resistance,        in, ch, el, nn ),
  cf( "rflag",             &code_rflag,             in, rf, rm, nn ),
  cf( "rem_exit_flag",     &code_rem_exit_flag,     nn, rm, di, df, nn ),
  cf( "set_exit_flag",     &code_set_exit_flag,     nn, rm, di, df, nn ),
  cf( "send_to_area",      &code_send_to_area,      nn, st, rm, vi, rm, rm, nn ),
  cf( "send_to_char",      &code_send_to_char,      nn, st, ch, vi, nn ),
  cf( "send_to_room",      &code_send_to_room,      nn, st, rm, vi, nn ),
  cf( "set_integer",       &code_set_integer,       nn, th, st, in, nn ),
  cf( "set_cflag",         &code_set_cflag,         nn, in, ch, nn ),
  cf( "set_religion",      &code_set_religion,      nn, ch, re, nn ),
  cf( "set_rflag",         &code_set_rflag,         nn, rf, rm, nn ),
  cf( "sex",               &code_sex,               in, ch, nn ),
  cf( "show",              &code_show,              nn, ch, rm, di, nn ),
  cf( "size",              &code_size,              in, ch, nn ),
  cf( "species",           &code_species,           in, ch, nn ),
  cf( "spoison",           &code_spoison,           nn, ch, al, in, in, af, nn ),
  cf( "summon",            &code_summon,            nn, ch, ch, nn ),
  cf( "tell",              &code_tell,              nn, ch, ch, st, nn ),
  cf( "time",              &code_time,              in, nn ),
  cf( "transfer",          &code_transfer,          nn, ch, rm, in, nn ),
  cf( "transfer_all",      &code_transfer_all,      nn, rm, rm, nn ),
  cf( "unlock",            &code_unlock,            nn, rm, di, nn ),
  cf( "untrap",            &code_untrap,            in, ch, in, nn ),
  cf( "update_quest",      &code_update_quest,      nn, ch, in, nn ),
  cf( "vnum",              &code_vnum,              in, th, nn ),
  cf( "wait",              &code_wait,              nn, in, nn ),
  cf( "weapon_type",       &code_weapon_type,       ob, ch, in, in, in, nn ),
  cf( "wearing",           &code_wearing,           ob, ch, wp, ly, nn ),
  cf( "weight",            &code_weight,            in, th, nn ),
  cf( "mod",               &code_mod,               in, in, in, nn ),
  cf( "object_type",       &code_object_type,       ot, ob, nn ),
  cf( "rtable_object",     &code_rtable_object,     ob, in, nn ),
  cf( "execute_wander",    &code_execute_wander,    nn, ch, nn ),
  cf( "",                  NULL,                    nn, nn )
};


cfunc_type op_list [] =
{
  cf( "=",                 &code_set_equal,         nn, in, in, nn ),
  cf( "+=",                &code_plus_equal,        nn, in, in, nn ),
  cf( "-=",                &code_minus_equal,       nn, in, in, nn ),
  cf( "&&",                &code_and,               in, in, in, nn ),
  cf( "||",                &code_or,                in, in, in, nn ),
  cf( "==",                &code_is_equal,          in, in, in, nn ),
  cf( "!=",                &code_not_equal,         in, in, in, nn ),
  cf( ">",                 &code_gt,                in, in, in, nn ),
  cf( "<",                 &code_lt,                in, in, in, nn ),
  cf( ">=",                &code_ge,                in, in, in, nn ),
  cf( "<=",                &code_le,                in, in, in, nn ),
  cf( "",                  NULL,                    nn, nn )
};



#undef nn
#undef st
#undef ch
#undef ob
#undef in
#undef rm
#undef sk
#undef sa
#undef al
#undef di
#undef rf
#undef na
#undef cl
#undef re
#undef rc
#undef th
#undef el
#undef wp
#undef ly
#undef ra
#undef af
#undef vi
#undef va
#undef ov
#undef ot
#undef df
#undef da
#undef cf
#undef po

/*
 *   POSITION ROUTINES
 */

   
void* code_is_mounted( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  return (void*) ( ch->mount != NULL );
}

 
void* code_is_resting( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Resting: NULL character." );
    return FALSE;
  }

  return (void*) ( ch->position == POS_RESTING );
}

void* code_is_sleeping( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Sleeping: NULL character." );
    return FALSE;
  }

  if( ch->position == POS_MEDITATING || ch->position == POS_SLEEPING )
    return (void*) TRUE;
  else
    return (void*) FALSE;
}

void* code_is_standing( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Standing: NULL character" );
    return FALSE;
  }

  if( ch->position == POS_STANDING && ch->fighting == NULL )
    return (void*) TRUE;
  else
    return (void*) FALSE;
}

void* code_is_fighting( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Fighting: NULL character." );
    return (void*) FALSE;
  }

  if( opponent( ch ) == NULL )
    return (void*) FALSE;
  else
    return (void*) TRUE;

}

void* code_is_pet( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Pet:  NULL character." );
    return (void*) FALSE;
  }

  return (void*) ( is_set( &ch->status, STAT_PET ) );
}

void* code_is_player( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Player: NULL character." );
    return (void*) FALSE;
  }

  return (void*) ( ch->species == NULL );
}

void* code_is_mob( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Mob:  NULL character." );
    return (void*) FALSE;
  }

  return (void*) ( ch->species && !is_set( &ch->status, STAT_PET ) && ch->shifted == NULL );
}

/*
 *   ROOM ROUTINES
 */

void* code_resistance( void** argument )
{
  char_data* ch = (char_data*) argument[0];
  int   element = (int) argument[1];

  if( ch == NULL ) {
    code_bug( "Code_Resistance: NULL character." );
    return (void*) 0;
  }

  if( element == 0 ) {
    code_bug( "Code_Resistance: Physical Resistance is not possible yet." );
    return (void*) 0;
  }

  if( element == 1 )
    return (void*) ch->Save_Fire( );

  if( element == 2 )
    return (void*) ch->Save_Cold( );

  if( element == 3 )
    return (void*) ch->Save_Acid( );

  if( element == 4 )
    return (void*) ch->Save_Shock( );

  if( element == 5 )
    return (void*) ch->Save_Mind( );

  if( element == 6 )
    return (void*) ch->Save_Magic( );

  if( element == 7 )
    return (void*) ch->Save_Magic( );

  if( element == 8 )
    return (void*) ch->Save_Holy( );

  return (void*) 0;
}

void* code_rflag( void** argument )
{
  int         flag  = (int)        argument[0];
  room_data*  room  = (room_data*) argument[1]; 

  if( room == NULL ) {
    code_bug( "Code_rflag: NULL room" );
    return (void*) FALSE;
  }

  return (void*) is_set( &room->room_flags, flag );
}


void* code_set_rflag( void** argument )
{
  int         flag  = (int)        argument[0];
  room_data*  room  = (room_data*) argument[1]; 

  if( room == NULL ) {
    code_bug( "Code_rflag: NULL room" );
    return NULL;
  }

  if( flag == RFLAG_APPROVED )
    return NULL;

  set_bit( &room->room_flags, flag );

  return NULL;
}


void* code_remove_rflag( void** argument )
{
  int         flag  = (int)        argument[0];
  room_data*  room  = (room_data*) argument[1]; 

  if( room == NULL ) {
    code_bug( "Code_rflag: NULL room" );
    return NULL;
  }

  if( flag == RFLAG_APPROVED )
    return NULL;

  remove_bit( &room->room_flags, flag );

  return NULL;
}


void* code_get_room( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Get_room: NULL character." );
    return NULL;
  }

  return ch->in_room;
}

void* code_group( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Group:  NULL character." );
    return (void*) -1;
  }

  if( ch->species == NULL ) {
    code_bug( "Group:  You really don't want this checking players." );
    return (void*) -1;
  }

  return (void*) ch->species->group;
}
/*
 *   CHARACTER ROUTINES
 */


void* code_size( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Size: Null argument" );
    return NULL;
  }

  return (void*) ch->Size( );
}


void* code_weight( void** argument )
{
  thing_data*  thing  = (char_data*) argument[0];

  if( thing == NULL ) {
    code_bug( "Weight: Null argument" );
    return NULL;
  }

  return (void*) thing->Weight( );
}


void* code_race( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    //
    // This is being logged unnecessarily, because you can have an acode
    // (e.g., trigger type RANDOM_ALWAYS) without a character defined.
    // 
    //code_bug( "Race: Null character." );
    return NULL;
  }

  return (void*) ch->shdata->race;
}

void* code_rtable_object( void** argument )
{
  int                   i = (int) argument[0];
  obj_clss_data* obj_clss;
  obj_data*           obj;

  int num = reset_table_load( find_rtable( i ) );

  if( num == 0 )
    return NULL;

  if( ( obj_clss = get_obj_index( num ) ) == NULL ) {
    bug( "Rtable object was NULL (Rtable #%i, Obj #%i)", i, num );
    return NULL;
  }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Rtable object NULL object (Rtable #%i, Obj: #%i)", i, num );
    return NULL;
  }

  return obj;
}


void* code_assist_char( void** argument )
{
  char_data*      ch = (char_data*) argument[0];
  char_data*  victim = (char_data*) argument[1];
  
  if( ch == NULL || victim == NULL ) {
    code_bug( "Assist_Char: NULL char." );
    return empty_string;
  }
 
  if( ch->Recognizes( victim ) ) 
    return victim->descr->name;
  else 
    return victim->descr->keywords;
}

void* code_is_in_clan( void** argument )
{
  char_data*   ch = (char_data*) argument[0];
  char*      clan = (char*)      argument[1]; 

  if( ch == NULL ) {
    code_bug( "Is_in_clan: NULL char." );
    return (void*) FALSE;
  }

  if( clan == NULL ) {
    code_bug( "Is_in_clan:  NULL clan." );
    return (void*) FALSE;
  }
      
  if( ch->species != NULL ) 
    return (void*) FALSE;
  
  if( ch->pcdata->pfile->clan2 == NULL )
    return (void*) FALSE;

  char*  clan2 = ch->pcdata->pfile->clan2->abbrev;

  if( strcmp( clan, clan2 ) ) 
    return (void*) FALSE;
  
  return (void*) TRUE;
}

void* code_is_in_guild( void** argument )
{
  char_data*   ch = (char_data*) argument[0];
  char*      clan = (char*)      argument[1]; 

  if( ch == NULL ) {
    code_bug( "Is_in_guild: NULL char." );
    return (void*) FALSE;
  }

  if( clan == NULL ) {
    code_bug( "Is_in_guild:  NULL guild." );
    return (void*) FALSE;
  }
      
  if( ch->species != NULL ) 
    return (void*) FALSE;
  
  if( ch->pcdata->pfile->guild == NULL )
    return (void*) FALSE;

  char*  clan2 = ch->pcdata->pfile->guild->abbrev;

  if( strcmp( clan, clan2 ) ) 
    return (void*) FALSE;
  
  return (void*) TRUE;
}

void* code_class( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Class: Null character." );
    return NULL;
  }

  if( ch->species != NULL ) {
    code_bug( "Class: Non-Player character." );
    return NULL;
  }
   
  return (void*) ch->pcdata->clss;
}

void* code_sex( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Sex: Null character." );
    return NULL;
  }

  return (void*) ch->sex;
}
 
/*
 *   CFLAG ROUTINES
 */


void* code_cflag( void** argument )
{
  int         flag  = (int)        argument[0];
  char_data*    ch  = (char_data*) argument[1]; 

  if( ch == NULL ) {
    code_bug( "Code_Cflag: NULL character." );
    return (void*) FALSE;
  }

  if( flag < 0 || flag >= 32*MAX_CFLAG ) {
    code_bug( "Code_Cflag: flag out of range." );
    return (void*) FALSE;
  }

  if( ch->pcdata == NULL )
    return (void*) FALSE;

  return (void*) is_set( ch->pcdata->cflags, flag );
}


void* code_set_cflag( void** argument )
{
  int        flag  = (int)        argument[0];
  char_data*   ch  = (char_data*) argument[1]; 

  if( ch == NULL ) {
    code_bug( "Code_Set_Cflag: NULL character." );
    return NULL;
  }

  if( flag < 0 || flag >= 32*MAX_CFLAG ) {
    code_bug( "Code_Set_Cflag: flag out of range." );
    return NULL;
  }

  if( ch->pcdata != NULL )
    set_bit( ch->pcdata->cflags, flag );

  return NULL;
}

void* code_religion( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Religion:  Ch is NULL." );
    return NULL;
  }

  if( ch->species != NULL ) {
    code_bug( "Religion: Ch is a mob." );
    return NULL;
  }

  return (void*) ch->pcdata->religion;
}

void* code_remove_cflag( void** argument )
{
  int flag      = (int)        argument[0];
  char_data* ch = (char_data*) argument[1]; 

  if( ch == NULL ) {
    code_bug( "Code_Remove_Cflag: NULL character." );
    return NULL;
  }

  if( flag < 0 || flag >= 32*MAX_CFLAG ) {
    code_bug( "Code_Remove_Cflag: flag out of range." );
    return NULL;
  }

  if( ch->pcdata != NULL )
    remove_bit( ch->pcdata->cflags, flag );

  return NULL;
}


/*
 *   CHARACTER STATUS ROUTINES
 */

void* code_can_handle( void** argument )
{
  char_data*       ch = (char_data*) argument[0];
  obj_data*       obj = (obj_data*)  argument[1];

  if( ch == NULL ) {
    code_bug( "Can_Carry: NULL Character." );
    return (void*) FALSE;
  }

  if( obj == NULL ) {
    code_bug( "Can_Carry: No object selected." );
    return (void*) FALSE;
  }

  if( !can_carry( ch, obj, false ) )
    return (void*) FALSE;

  return (void*) TRUE;
}


void* code_can_climb( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Can_climb: NULL Character." );
    return (void*) FALSE;
  }

  if( ch->species != NULL )
    return (void*) ( is_set( ch->species->act_flags, ACT_CAN_FLY )
      || is_set( ch->species->act_flags, ACT_CAN_CLIMB ) );
  return (void*) FALSE;
}

void* code_can_fly( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Can_Fly: NULL Character." );
    return (void*) FALSE;
  }

  return (void*) ch->can_fly( );
}

void* code_can_see( void** argument )
{
  char_data*  ch     = (char_data*) argument[0];
  char_data*  victim = (char_data*) argument[1];

  if( ch == NULL ) {
    code_bug( "Can_See: NULL Character." );
    return (void*) FALSE;
  }

  if( victim == NULL ) {
    code_bug( "Can_See: NULL Victim." );
    return (void*) FALSE;
  }

  return (void*) victim->Seen(ch);
}

void* code_is_silenced( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Silenced: NULL Character." );
    return (void*) FALSE;
  }

  return (void*) is_set( ch->affected_by, AFF_SILENCE );
}

void* code_is_affected( void** argument )
{
  int         affect = (int) argument[0];
  char_data*      ch = (char_data*) argument[1];

  if( ch == NULL ) {
    code_bug( "Is_Affected: NULL character." );
    return (void*) FALSE;
  }

  if( affect < 0  || affect > MAX_ENTRY_AFF_CHAR ) {
    code_bug( "Is_Affected: Checking non-existent affect." );
    return (void*) FALSE;
  }
  
  return (void*) is_set( ch->affected_by, affect );
}

void* code_init_attack( void** argument )
{
  char_data* ch = (char_data*) argument[0];
  char_data* victim = (char_data*) argument[1];

  if( ch == NULL ) {
    code_bug( "Init_Attack: Char is NULL." );
    return NULL;
  }

  if( victim == NULL ) {
    code_bug( "Init_Attack: Mob is NULL." );
    return NULL;
  }
  
  init_attack( ch, victim );
  return NULL;
}
  
void* code_is_berserk( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Berserk: NULL Character." );
    return (void*) FALSE;
  }

  return (void*) is_set( &ch->status, STAT_BERSERK );
}

void* code_is_blind( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Blind: NULL Character." );
    return (void*) FALSE;
  }

  return (void*) is_set( ch->affected_by, AFF_BLIND );
}

void* code_is_hidden( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Hidden:  NULL Character." );
    return (void*) FALSE;
  }

  return (void*) is_set( ch->affected_by, AFF_HIDE );
}

void* code_is_invisible( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_hidden: NULL Character." );
    return (void*) FALSE;
  }

  return (void*) is_set( ch->affected_by, AFF_INVISIBLE );
}
/*
 *   UNCLASSIFIED 
 */

void* code_recog( void** argument )
{
  char_data*      mob   = (char_data*) argument[0];
  char_data*       ch   = (char_data*) argument[1];

  if( mob == NULL || ch == NULL ) {
    code_bug( "Recog: NULL character" );
    return NULL;
  }

  if( mob->species == NULL && ch->species == NULL ) {
    code_bug( "Recog: You are not allowed to force players to recognize one another." );
    return NULL;
  }

  mob->known_by += ch;

  return NULL;
}


void* code_fighting_who( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Fighting_Who: NULL Character." );
    return NULL;
  }

  return (void*) ch->fighting;
}

void* code_leader( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Leader: NULL Character." );
    return NULL;
  }

  if( ch->leader == NULL )
    return ch;

  return (void*) ch->leader;
}

void* code_improve_skill( void** argument )
{
  char_data*    ch = (char_data*) argument[0];
  int        skill = (int) argument[1]; 

  if( ch == NULL ) {
    code_bug( "Improve_Skill: NULL char." );
    return NULL;
  }

  ch->improve_skill( skill );
  return NULL;
}

void* code_interpret( void** argument )
{
  char_data* ch     = (char_data*) argument[0];
  char* string      = (char*)      argument[1];
  char_data* victim = (char_data*) argument[2];

  char tmp [ MAX_INPUT_LENGTH ];

  if( ch != NULL ) {
    if( victim == NULL ) 
      interpret( ch, string );
    else {
      sprintf( tmp, "%s %s", string, victim->descr->name );
      interpret( ch, tmp );
    }
  }

  return NULL;
}   


void* code_send_to_char( void** argument )
{
  char      *string = (char*)      argument[0];
  char_data *ch     = (char_data*) argument[1];
  int       sense   = (int)        argument[2];

  act( ch, string, ch, NULL, NULL, NULL, sense );
 
  return NULL;
}


void* code_send_to_room( void** argument )
{
  char*      string   = (char*)      argument[0];
  room_data*   room   = (room_data*) argument[1];
  int         sense   = (int)        argument[2];

  if( room != NULL )
    act_room( room, string, NULL, NULL, sense );
 
  return NULL;
}


void* code_send_to_area( void** argument )
{
  char      *string = (char*)      argument[0];
  room_data *room   = (room_data*) argument[1];
  room_data *room2  = (room_data*) argument[4];
  room_data *room3  = (room_data*) argument[3];
  int        sense  = (int)        argument[2];

  if( room != NULL )
   send_to_area(  string, room->area, sense ); 
  
  if( room2 != NULL )
    send_to_area( string, room2->area, sense );

  if( room3 != NULL )
    send_to_area( string, room3->area, sense ); 

  return NULL;
}


void* code_act_room( void** argument )
{
  char*       string  = (char*)      argument[0];
  char_data*      ch  = (char_data*) argument[1];
  obj_data*      obj  = (obj_data*)  argument[2];
  char_data*  victim  = (char_data*) argument[3];  
  int          sense  = (int)        argument[4];

  if( ch != NULL && ch->in_room != NULL )
    act_room( ch->in_room, string, victim == NULL ? ch : victim, obj, sense );

  return NULL;
}


void* code_act_neither( void** argument )
{
  char*       string  = (char*)      argument[0];
  char_data*      ch  = (char_data*) argument[1];
  obj_data*      obj  = (obj_data*)  argument[2];
  char_data*  victim  = (char_data*) argument[3];  
  
  if( ch == NULL || victim == NULL ) {
    code_bug( "Act_Neither: Character = null pointer." );
    return NULL;
  }

  act_neither( string, ch, victim, obj );

  return NULL;
}

void* code_add( void** argument )
{
  int total = 0;

  for( int i = 0; i < 4; i++ ) 
    total += (int) argument[i];   
    
  return (void*) total;
}

void* code_mult( void** argument ) {
  int result = 0;
  int div = (int) argument[0];
  int m = (int) argument[1];
  int n = (int) argument[2];
  
  if( div < 0 && n == 0 ) {
    code_bug( "Code_Mult: Division by zero is -bad-." );
    return (void*) 0;
  }

  if( div > 0 )
    result = m*n;
  if( div < 0 )
    result = m/n;

  return (void*) result;
}
  
void* code_add_pet( void** argument )
{
  char_data* pet    = (char_data*) argument[0];
  char_data* leader = (char_data*) argument[1];

  if( pet == NULL ) {
    code_bug( "Add_Pet:  Non-existent pet." );
    return NULL;
  }

  if( pet->species == NULL ) {
    code_bug( "Add_Pet:  Attempting to make a player a pet?" );
    return NULL;
  }

  if( leader == NULL ) {
    code_bug( "Add_Pet:  Non-existent character." );
    return NULL;
  }

  set_bit( &pet->status, STAT_PET );
  set_bit( &pet->status, STAT_TAMED );
  remove_bit( &pet->status, STAT_AGGR_ALL );
  remove_bit( &pet->status, STAT_AGGR_GOOD );
  remove_bit( &pet->status, STAT_AGGR_EVIL );

  unregister_reset( pet );

  add_follower( pet, leader );

  return NULL;
}

void* code_add_affect( void** argument )
{
  char_data*        ch = (char_data*) argument[0];
  int           affect = (int) argument[1];
  int         duration = (int) argument[2];
  int            level = (int) argument[3];
  affect_data*     paf = NULL;

  if( ch == NULL ) {
    code_bug( "Non-existent character to add an affect to (add_affect)." );
    return NULL;
  }

  if( level <= 0 ) {
    code_bug( "Add_Affect with a negative or zero level." );
    return NULL;
  }

  if( level > 10 )
    level = 10;

  if( affect < 0 || affect >= MAX_ENTRY_AFF_CHAR ) {
    code_bug( "Attempting to add a non-existent affect to a character." );
    return NULL;
  }

  if( duration <= 0 ) {
    code_bug( "Non-existent duration on an affect." );
    return NULL;
  }

  paf->level       = level;
  paf->duration    = duration;
  paf->type        = affect;
  paf->leech       = NULL;
  paf->leech_max   = 0;
  paf->leech_regen = 0;
  
  add_affect( ch, paf );
  return NULL;
}

void* code_can_tame_pet( void** argument )
{
  char_data* pet = (char_data*) argument[0];
  char_data* ch  = (char_data*) argument[1];

  if( ch == NULL ) {
    code_bug( "Can_Add_Pet: NULL Character." );
    return (void*) FALSE;
  }

  if( pet->shdata->level > ch->shdata->level - pet_levels( ch ) )
    return FALSE;

  if( is_set( pet->species->act_flags, ACT_MOUNT ) && has_mount( ch ) )
    return FALSE;

  return (void*) TRUE;
}


void* code_can_buy_pet( void** argument )
{
  char_data* pet = (char_data*) argument[0];
  char_data* ch  = (char_data*) argument[1];

  if( ch == NULL ) {
    code_bug( "Can_Add_Pet: NULL Character." );
    return (void*) FALSE;
  }

  if( pet->shdata->level > ch->shdata->level )
    return FALSE;

  if( is_set( pet->species->act_flags, ACT_MOUNT ) && has_mount( ch ) )
    return FALSE;

  if( number_of_pets( ch ) >= 2 )
    return FALSE;

  return (void*) TRUE;
}


void* code_act_tochar( void** argument )
{
  char*      string  = (char*)      argument[0];
  char_data*     ch  = (char_data*) argument[1];
  obj_data*     obj  = (obj_data*)  argument[2];
  obj_data*    obj2  = NULL;
  char_data* victim  = (char_data*) argument[3];  
  int         sense  = (int)        argument[4];

  act( ch, string, ch, victim, obj, obj2, sense );

  return NULL;
}


void* code_act_area( void** argument )
{
  char*       string  = (char*)      argument[0];
  char_data*      ch  = (char_data*) argument[1];
  obj_data*      obj  = (obj_data*)  argument[2];
  char_data*  victim  = (char_data*) argument[3];  
  int          sense  = (int)        argument[4];

  if( ch == NULL ) {
    code_bug( "Act_Area: Character = null pointer." );
    return NULL;
    }

  act_area( string, ch, victim, obj, sense );

  return NULL;
}


void* code_act_notchar( void** argument )
{
  char*      string  = (char*)      argument[0];
  char_data*     ch  = (char_data*) argument[1];
  obj_data*     obj  = (obj_data*)  argument[2];
  obj_data*    obj2  = NULL;
  char_data* victim  = (char_data*) argument[3];  
  int         sense  = (int)        argument[4];

  if( ch != NULL )
    act_notchar( string, ch, victim, obj, obj2, sense );

  return NULL;
}


void* code_act_notvict( void** argument )
{
  /*
  char      *string = (char*)      argument[0];
  char_data *ch     = (char_data*) argument[1];
  obj_data  *obj    = (obj_data*)  argument[2];
  char_data *victim = (char_data*) argument[3];  
  
  act( string, ch, obj, victim, TO_NOTVICT );
  act_to( string, ch, obj, victim, var_ch );
  */
  return NULL;
}


void* code_junk_mob( void** argument )
{
  char_data*     ch = (char_data*) argument[0];

  if( ch == NULL )
    return NULL;
 
  if( !IS_NPC( ch ) ) {
    code_bug( "Junk_mob: character is a player??" );
    return NULL;
  } 
 
  ch->Extract( );

  return NULL;
}


void* code_drain_stat( void** argument )
{
  char_data* ch  = (char_data*) argument[0];
  int         i  = (int)        argument[1]; 
  int         j  = (int)        argument[2];
  int         m  = (int)        argument[3];

//  int loc[] = { APPLY_STR, APPLY_INT, APPLY_WIS, APPLY_DEX, APPLY_CON }; 
  affect_data affect;
 
  if( ch == NULL || i < 0 || affect_loc[i] == APPLY_NONE || affect_loc[i] == -1 || j <= 0 || m < -5 || m > 2 )
    return NULL;
 
  affect.type           = AFF_NONE;
  affect.mlocation[ 0 ] = affect_loc[ i ];
  affect.mmodifier[ 0 ] = m;
  affect.duration       = j;
  affect.level          = 5;
  affect.leech          = NULL;

  add_affect( ch, &affect );
  return NULL;
}  

void* code_current_hit( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL )
    return NULL;

  return (void*) ch->hit;
}

void* code_max_hit( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL )
    return NULL;

  return (void*) ch->max_hit;
}

void* code_current_mana( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL )
    return NULL;

  return (void*) ch->mana;
}

void* code_max_mana( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL )
    return NULL;

  return (void*) ch->max_mana;
}

void* code_current_move( void** argument)
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL )
    return NULL;

  return (void*) ch->move;
}

void* code_max_move( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL )
    return NULL;

  return (void*) ch->max_move;
} 

void* code_find_player( void** argument )
{
  obj_data*    corpse = (obj_data*) argument[0];
  int           ident;
  player_data*     pi;

  if( corpse->pIndexData->vnum != 11 )
    return NULL;

  ident = corpse->value[3];

  for( int i = 0; i < player_list; i++ ) {
     pi = player_list[i];
     if( pi->pcdata->pfile->ident == ident )
       return (void*) pi;
  }

  return NULL;
}

void* code_find_stat( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          i  = (int)        argument[1]; 

  if( ch == NULL ) 
    return NULL;

  
  if( i < 0 || i > MAX_STAT ) {
    code_bug( "Find_Stat: Impossible field." );
    return NULL;
    } 

  if( ch->species != NULL ) {
    int value[] = { ch->Strength( ), ch->Intelligence( ),
      ch->Wisdom( ), ch->Dexterity( ), ch->Constitution( ),
      ch->shdata->level, 0, 0, ch->shdata->alignment};
    return (void*) value[i];
    }
  else { 
    int value[] = { ch->Strength( ), ch->Intelligence( ),
      ch->Wisdom( ), ch->Dexterity( ), ch->Constitution( ),
      ch->shdata->level, ch->pcdata->piety,
      ch->pcdata->clss, ch->shdata->alignment };
    return (void*) value[i];
    }
}


void* code_random( void** argument )
{
  int  n1  = (int) argument[0];
  int  n2  = (int) argument[1];

  return (void*) ( number_range( n1,n2 ) );
}


void* code_dice( void** argument )
{
  int  n1  = (int) argument[0];
  int  n2  = (int) argument[1];
  int  n3  = (int) argument[2];

  return (void*) ( roll_dice( n1,n2 )+n3 );
}


void* code_find_skill( void** argument )
{
  char_data*    ch  = (char_data*) argument[0];
  int        skill  = (int)        argument[1];

  if( skill == SKILL_TRACK )
    skill = SKILL_SEARCHING;

  if( ch == NULL ) {
    code_bug( "Find_Skill: NULL character." );
    return NULL;
  }

  if( ch->mount != NULL )
    ch = ch->mount;

  return (void*) ch->get_skill( skill );
}
/*
void* code_stun( void** argument )
{
  char_data*       ch = (char_data*) argument[0];
  int          length = (int)        argument[1];
  char*       to_char = (char*)      argument[2];
  char*      not_char = (char*)      argument[3];
  char_data*      rch;
  char*           tmp = NULL;
  obj_data*       obj = NULL;

  if( ch == NULL ) {
    code_bug( "Stun: NULL character." );
    return NULL;
    }

  char_data* opponent = ch->fighting;

  if( ch->position <= POS_STUNNED ||
    ch->shdata->race == RACE_PLANT )
    return NULL;

  if( to_char == NULL ) {
    sprintf( tmp, "%s momentarily STUNS you!\r\n", 
    opponent->Seen_Name(opponent) );
    act( ch, tmp, ch, opponent, obj );
    }
  else
    act( ch, to_char, ch, opponent, obj );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL
      || rch == ch || rch == opponent || rch->link == NULL
      || !opponent->Seen( rch ) )
      continue;
    if( not_char == NULL ) {
      sprintf( tmp, "%s momentarily STUNS %s.\r\n",
      opponent->Seen_Name(opponent), ch->Name( rch ) );
      act( rch, tmp, rch, opponent, obj );
      }
    else
      act( rch, not_char, rch, opponent, obj );
    }
  disrupt_spell( ch );
  set_delay( ch, length );
  return NULL;
}
*/
void* code_summon( void** argument )
{
  char_data*      ch  = (char_data*) argument[0];
  char_data*  victim  = (char_data*) argument[1];

  if( ch != NULL && ch->pcdata == NULL )
    summon_help( ch, victim );

  return NULL;
}


void* code_reputation( void** argument )
{
  char_data*     ch  = (char_data*) argument[0];
  int        nation  = (int)        argument[1];

  if( ch == NULL ) {
    code_bug( "Reputation: Null character." );
    return NULL;
  }
  player_data* plyr = player(ch);
  int reput = 0; 
  if ( plyr != NULL && nation >= 1 && nation < MAX_ENTRY_NATION ) {
     reput = plyr->reputation.nation[nation];
  }
  return (void*) reput;
}


void* code_find_room( void** argument )
{
  int         vnum   = (int) argument[0];
  room_data*  room;

  if( ( room = get_room_index( vnum, FALSE ) ) == NULL ) {
    code_bug( "Find_Room: Non-existent room." );
    bug( "Find_Room #%i", vnum  );
  }
 
  return room;
}


void* code_cast_spell( void** argument )
{
  int          spell  = (int)        argument[0];
  char_data*      ch  = (char_data*) argument[1];
  char_data*  victim  = (char_data*) argument[2];

  if( ch == NULL || spell < SPELL_FIRST || spell >= WEAPON_FIRST ) {
    code_bug( "Error in spell, or ch" );
    return NULL;
  }
  spell = spell-SPELL_FIRST;

  switch( spell_table[ spell ].type ) {
    case STYPE_SELF_ONLY :
    case STYPE_PEACEFUL :
      if( victim == NULL )
        victim = ch;
      break;
  
    case STYPE_OFFENSIVE :
      if( victim == NULL )
        return NULL;
      break;

    default :
      return NULL;
  }

  ( spell_table[ spell ].function )( ch, victim, NULL, 10, -1 );

  /*
  stop_active( ch );

  event = new cast_event( ch, victim );
  
  event->vo      = victim;
  event->spell   = *spell-SPELL_FIRST;
  event->prepare = FALSE;
  event->wait    = spell_table[*spell-SPELL_FIRST].prepare-1;
  ch->active     = event;

  has_reagents( ch, event );
  execute_cast( event );
  */

  return NULL;  
}

void* code_execute_wander( void** argument )
{
  char_data*     ch = (char_data*) argument;
  exit_data*   exit = NULL;

  if( is_set( &ch->status, STAT_ALERT ) && number_range( 0, 30 ) == 0 )
    remove_bit( &ch->status, STAT_ALERT );

  if(  ch->position == POS_STANDING
    && ch->leader == NULL
    && ( exit = random_movable_exit( ch ) ) != NULL
    && !is_set( &exit->to_room->room_flags, RFLAG_NO_MOB )
    && ( !is_set( ch->species->act_flags, ACT_STAY_AREA )
    || exit->to_room->area == ch->in_room->area ) )
    move_char( ch, exit->direction, FALSE );

  return NULL;
}


void* code_do_spell( void** argument )
{
  int          spell  = (int)        argument[0];
  char_data*      ch  = (char_data*) argument[1];
  char_data*  victim  = (char_data*) argument[2];
  obj_data*      obj  = (obj_data*)  argument[3];
  int          skill  = (int)        argument[4];

  if( ch == NULL )
    return NULL;

  if( victim == NULL )
    victim = ch;
  
  if( skill == 0 )
    skill = 10;

  if( spell >= SPELL_FIRST && spell < SPELL_MAX && !spell_table[ spell-SPELL_FIRST ].song )
    ( spell_table[ spell - SPELL_FIRST ].function )( ch, victim, (void*) obj, skill, -1 );

  return NULL;
}

void* code_dispel( void** argument )
{
  int affect    = (int) argument[0];
  char_data* ch = (char_data*) argument[1];
  
  affect_data* spell;

  if( ch == NULL )
    return NULL;

  if( affect < 0  || affect > MAX_ENTRY_AFF_CHAR ) {
    code_bug( "Improper affect attempting to be dispelled." );
    return NULL;
  }

  if( affect == AFF_ALL ) {
    for( int i = ch->affected.size-1; i >= 0; i-- ) {
      spell = ch->affected[i];
      remove_affect( ch, spell );
    }
  } else {
    for( int i = ch->affected.size-1; i >= 0; i -- ) {
      if( ch->affected[i]->type == affect ) {
        spell = ch->affected[i];
        remove_affect( ch, spell );
      }
    }
  }

  return NULL;
}

void* code_num_in_room( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  int          num  = 0;

  if( room != NULL )
    for( int i = 0; i < room->contents; i++ )
      num += ( character( room->contents[i] ) != NULL );

  return (void*) num;
}  


void* code_players_area( void** argument )
{
  room_data*  room  = (room_data*) argument[0];

  if( room == NULL ) {
    code_bug( "Players_Area: NULL room." );
    return NULL;
  }

  return (void*) room->area->nplayer;
}  


void* code_players_room( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  int          num  = 0;  

  if( room == NULL ) {
    code_bug( "Players_Room: NULL room." );
    return NULL;
  }

  player_data* a_player;
  for( int i = 0; i < room->contents; i++ ) {
    a_player = player( room->contents[i] );
    if( a_player != NULL && a_player->shdata != NULL && !is_set( a_player->pcdata->pfile->flags, PLR_WIZINVIS ) )
      num++;
  }

  return (void*) num;
}  


void* code_num_mob( void** argument )
{
  int         vnum  = (int)        argument[0];
  room_data*  room  = (room_data*) argument[1];
  mob_data*    rch;
  int          num  = 0;  

  if( room != NULL )
    for( int i = 0; i < room->contents; i++ )
      if( ( rch = mob( room->contents[i] ) ) != NULL && rch->species->vnum == vnum )
        num++;

  return (void*) num;
}  

void* code_num_mob_mud( void** argument )
{
  int             vnum = (int) argument[0];
  int             num = 0;
  char_data*      mob;

  for( int i = mob_list.size-1; i >= 0; i-- )
    if( ( mob = mob_list[i] ) != NULL && mob->Is_Valid( ) )
      if( mob->species->vnum == vnum )
        num++;

  return (void*) num;
}

void* code_find_mob( void** argument )
{
  int             vnum = (int) argument[0];
  char_data*       mob = NULL;

  for( int i = 0; i < mob_list; i++ ) {
    if( ( mob = mob_list[i] ) != NULL && mob->species->vnum == vnum )
      if(  Room( mob->array->where ) != NULL )
        return (void*) mob;
  }

  return NULL;
}

void* code_transfer( void** argument )
{
  char_data*    ch  = (char_data*) argument[0];
  room_data*  room  = (room_data*) argument[1];
  int         scan  = (int)        argument[2];

//  transfer naughty imm to prison if they enter tyrone's abode
//  if( room != NULL && room->vnum == 4 ) 
//    room = get_room_index( ROOM_PRISON );

  if( room == NULL ) {
    send( "A script attempts to send you to a non-existent room??\r\n", ch );
    return NULL;
  }

  if( ch == NULL )
    return NULL;

  if( ch->rider != NULL )
    ch = ch->rider;

  ch->From( );
  ch->To( room );

  if( ch->mount != NULL ) {
    ch->mount->From( );
    ch->mount->To( room );
  }

  if( scan == 2 ) {
    send( "\r\n", ch );
    do_look( ch, "" );
    return NULL;
  } else if( scan != 1 ) {
    send( "\r\n", ch );
    do_look( ch, "" );
    send( "\r\n", ch );
    do_scan( ch, "" );
    return NULL;
  }
  return NULL;
}


void* code_transfer_all( void** argument )
{
  room_data*       from  = (room_data*) argument[0];
  room_data*         to  = (room_data*) argument[1];
  player_data*      rch;
  thing_array      list;

  if( to == NULL )
    return NULL;

  copy( list, from->contents );

  // MEM 7/10/99 - Transfer all players, not all characters.
  for( int i = 0; i < list; i++ )
    if( ( rch = player( list[i] ) ) != NULL ) {
      rch->From( );
      rch->To( to );
    }

  for( int i = 0; i < list; i++ )
    if( ( rch = player( list[i] ) ) != NULL ) {
      send( "\r\n", rch );
      show_room( rch, to, FALSE, FALSE );
    }

  return NULL;
}
/*
void* code_transfer_obj( void** argument )
{
  obj_data* obj = (obj_data*) argument[0];
  room_data* room = (room_data*) argument[1];

  if( obj == NULL )
    return NULL;

  if( room == NULL ) {
    code_bug( "Attempting to transfer an object to NULL room?" );
    return NULL;
    }

  obj->To( room );
  return NULL;
}
*/ 
void* code_mload( void** argument )
{
  int               vnum  = (int)        argument[0];
  room_data*        room  = (room_data*) argument[1];
  mob_data*          mob;
  species_data*  species;

  if( room == NULL || ( species = get_species( vnum ) ) == NULL ) {
    code_bug( "Mload: non-existent species or null room." );
    return NULL;
  }

  mob = create_mobile( species );
  mreset_mob( mob );
 
  mob->To( room );

  if( is_set( mob->species->act_flags, ACT_SENTINEL ) ) { 
    set_bit( &mob->status, STAT_SENTINEL );    
  } else {
    delay_wander( new event_data( execute_wander, mob ) );
  }

  return mob;
}


void* code_rand_char( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  char_data*   rch;

  if( room == NULL )
    return NULL;

  rch = random_pers( room );
  
  return rch;
}

void* code_random_enemy( void** argument )
{
  char_data* ch = (char_data*) argument[0];
  char_data* rch;
  
  if( ch == NULL ) {
    code_bug( "Code: Random_Enemy - NULL Character" );
    return NULL;
  }

  if( ch->in_room == NULL ) {
    code_bug( "Ch is in a NULL room?!?" );
    return NULL;
  }

  room_data* room = ch->in_room;

  rch = random_enemy( room, ch );

  return rch;
}

void* code_rand_player( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  char_data*   rch;

  if( room == NULL )
    return NULL;

  rch = rand_player( room );
  
  return rch;
}


void* code_player_in_room( void** argument )
{
  int           id  = (int) argument[ 0 ];
  room_data*  room  = (room_data*) argument[ 1 ];

  if( room == NULL )
    return NULL;

  for( int j = 0; j < room->contents; j++ ) {
    player_data* a_player = player( room->contents[ j ] );
    if( a_player != NULL && a_player->Is_Valid( ) && a_player->pcdata->pfile->ident == -id )
      return a_player;
  }

  return NULL;
}


/*
void* code_rand_tank( void** argument )
{
  char_data* leader = (char_data*) argument[1];
  room_data* room   = leader->in_room;
  char_data* rch;

  if( room == NULL || leader == NULL )
    return NULL;

  rch = rand_tank( room, leader );

  return rch;
}
*/
 
void* code_obj_in_room( void** argument )
{
  int         vnum  = (int)       argument[0];
  room_data*  room  = (room_data*) argument[1];

  if( room == NULL ) {
    code_bug( "Obj_in_room: NULL room." );
    return NULL;
  }

  return find_vnum( room->contents, vnum );
}

void* code_obj_level( void** argument )
{
  obj_data*    obj = (obj_data*) argument[0];

  code_bug( "Switch this to obj_stat." );

  if( obj == NULL )
    return NULL;

  return (void*) obj->pIndexData->level;
}

void* code_obj_condition( void** argument )
{
  obj_data*         obj = (obj_data*) argument[0];
  int         condition = 0;

  code_bug( "Switch this to obj_stat( )." );

  if( obj == NULL )
    return (void*) condition;

  if( obj->pIndexData->durability == 0 )
    return (void*) condition;

  condition = 100*obj->condition/obj->pIndexData->durability;

  return (void*) condition;
}


void* code_vnum( void** argument )
{
  thing_data* thing = (thing_data*) argument[0];

  mob_data* npc;
  player_data* pc;
  obj_data* obj;
  room_data* room;

  if( thing == NULL ) {
    code_bug( "Code_Vnum: NULL thing." );
    return (void*) 0;
  } else if( ( npc = mob( thing ) ) != NULL && npc->Is_Valid( ) ) {
    return (void*) npc->species->vnum;
  } else if( ( obj = object( thing ) ) != NULL && obj->Is_Valid( ) ) {
    return (void*) obj->pIndexData->vnum;
  } else if( ( room = Room( thing ) ) != NULL && room->Is_Valid( ) ) {
    return (void*) room->vnum;
  } else if( ( pc = player( thing ) ) != NULL && pc->Is_Valid( ) ) {
    return (void*) (-pc->pcdata->pfile->ident);
  }

  return (void*) 0;
}


void* code_species( void** argument )
{
  char_data*   ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Code_Species: NULL character." );
    return (void*) 0;
  } else if( !ch->species ) {
    code_bug( "Code_Species: ch is not a mob." );
    return (void*) 0;
  }

  return (void*) ch->species->vnum;
}


void* code_mob_in_room( void** argument )
{
  int         vnum  = (int)        argument[0];
  room_data*  room  = (room_data*) argument[1];
  char_data*   rch;

  if( room == NULL ) {
    code_bug( "Mob_in_Room: Null Room??" );
    return NULL;
  }

  for( int i = 0; i < room->contents; i++ )
    if( ( rch = mob( room->contents[i] ) ) != NULL && rch->species->vnum == vnum && !is_set( &rch->status, STAT_PET ) )
      return rch; 
  
  return NULL;
}

void* code_pet_in_room( void** argument )
{
  int         vnum  = (int)        argument[0];
  room_data*  room  = (room_data*) argument[1];
  char_data*   rch;

  if( room == NULL ) {
    code_bug( "Pet_in_Room: Null Room??" );
    return NULL;
  }

  for( int i = 0; i < room->contents; i++ )
    if( ( rch = mob( room->contents[i] ) ) != NULL && rch->species->vnum == vnum && is_set( &rch->status, STAT_PET ) )
      return rch; 
  
  return NULL;
}


void* code_is_pcontrolled( void** argument )
{
  char_data*    ch = (char_data*) argument[0];

  if( ch == NULL )
    return NULL;

  return (void*)( ch->shifted != NULL );
}


void* code_plague( void** argument )
{
  char_data*        ch  = (char_data*) argument[0];
  affect_data   affect;
 
  if( ch == NULL ) // || ch->save_vs_poison( 5 ) )
    return NULL;

  if( var_mob != NULL && var_mob->species != NULL && !is_set( ch->affected_by, AFF_PLAGUE ) )
    var_mob->species->special += 20;

  affect.type      = AFF_PLAGUE;
  affect.duration  = 11 * number_range( 3, 7 );
  affect.level     = 0;
  affect.leech     = NULL;

  add_affect( ch, &affect );

  return NULL;
}


void* code_poison( void** argument )
{
  char_data*        ch  = (char_data*) argument[0];
  affect_data   affect;
  int           chance  = 0;
/*
  chance = ch->Save_Poison( );
  if( var_mob != NULL )
   chance += var_mob->shdata->level-ch->shdata->level;
*/

  if( ch == NULL )
    return NULL;

  if( is_undead( ch ) )
    return NULL;

  chance = ch->Save_Poison( );
  if( var_mob != NULL )
    chance += var_mob->shdata->level-ch->shdata->level;

  if( var_mob != NULL && var_mob->species != NULL && !is_set( ch->affected_by, AFF_POISON ) ) 
    var_mob->species->special += 10;

  if( number_range( 0, 99 ) < chance )
    return NULL;

  affect.type           = AFF_POISON;
  affect.duration       = number_range(3,7) * 10;
  affect.level          = 8;
  affect.leech          = NULL;
  affect.mlocation[ 0 ] = APPLY_NONE;
  affect.mmodifier[ 0 ] = 0;

  add_affect( ch, &affect );

  return NULL;
}

void* code_spoison( void** argument )
{
  affect_data    affect;
  affect_data   affectb;
  char_data*     victim = (char_data*) argument[0];
  int            poison = (int) argument[1];
  int            chance = 0;
  int               mod = (int) argument[2];
  int              save = (int) argument[3];
  char_data*         ch = var_ch;
  obj_data*         obj = var_obj;
  int           saffect = (int) argument[4];
  int duration, level, modifier, hits; 
  affect_data*      paf = NULL;

  char* modify_amount[] = { "slightly", "somewhat", "quite a bit", "much", "**MUCH**" };
  char* affect_name [ MAX_AFF_LOCATION ] = {
    "",                                                   //  0  no location
    "weaker",                                             //  1  apply strength
    "less agile",                                         //  2  apply dex
    "less intelligent",                                   //  3  apply int
    "less wise",                                          //  4  apply wis
    "less tough",                                         //  5  apply con
    "weaker to magic",                                    //  6  apply magic
    "weaker to fire",                                     //  7  apply fire
    "weaker to cold",                                     //  8  apply cold
    "weaker to electricity",                              //  9  apply electric
    "weaker to mind",                                     // 10  apply mind
    "younger",                                            // 11  apply age
    "",                                                   // 12  apply mana
    "",                                                   // 13  apply hitpoints
    "",                                                   // 14  apply movement
    "",                                                   // 15  unknown
    "",                                                   // 16  unknown
    "less armored",                                       // 17  apply armor
    "",                                                   // 18  apply hitroll
    "",                                                   // 19  apply damroll
    "",                                                   // 20  apply mana regen
    "",                                                   // 21  apply hit regen
    "",                                                   // 22  apply move regen
    "weaker to acid",                                     // 23  apply acid  
    "weaker to poison",                                   // 24  apply poison
    "weaker to (un)holy",                                 // 25  apply holy
    "",                                                   // 26  apply physical damage
    "",                                                   // 26  apply fire damage
    "",                                                   // 27  apply cold damage
    "",                                                   // 28  apply acid damage
    "",                                                   // 29  apply shock damage
    "",                                                   // 30  apply mind damage
    "",                                                   // 31  apply magic damage
    "",                                                   // 32  apply poison damage
    "",                                                   // 33  apply holy damage
    "",                                                   // 34  apply unholy damage
  };
  
  if( poison >= MAX_AFF_LOCATION || poison < 0 ) {
    code_bug( "Code_Spoison: Invalid Poison Location" );
    return NULL;
  }

  if( affect_loc[poison] == -1 ) {
    code_bug( "Code_Spoison: Invalid Poison Affect" );
    return NULL;
  }
  
  if( victim == NULL )
    return NULL;

  if( is_undead( victim ) )
    return NULL;

  if( ch == victim )
    ch = NULL;
  
  if( save == 1 ) {
    chance = victim->Save_Poison( );
    if( var_mob != NULL )
      chance += var_mob->shdata->level-victim->shdata->level;

    if( var_mob != NULL && var_mob->species != NULL && !is_set( victim->affected_by, AFF_POISON ) ) 
      var_mob->species->special += 10;

    if( number_range( 0, 99 ) < chance )
      return NULL;
  }

  hits = 0;
  duration = ( obj == NULL || ( hits = obj->Get_Integer( POISON_HITS ) ) == 0 ) ? number_range( 3, 7 ) * 10 : number_range( 5, 5 * hits );
  level = ch == NULL ? 8 : 10*ch->get_skill( TRADE_POISON )/MAX_SKILL_LEVEL;

  if( mod > 10 || mod < -20 ) {
    code_bug( "Code_Spoison: Mod is to high." );
    mod = 0;
  }
  
  if( mod > 0 )
    mod = number_range( 1, mod );
  else
    mod = number_range( mod, -1 );

  modifier = number_range( -hits, -1 ) + mod;

  if( modifier >= 0 && saffect <= 0 )
    return NULL;

  if( saffect > 0 && is_set( victim->affected_by, saffect ) )
    saffect = -1; 

  paf = find_affect( victim, AFF_POISON );

  if( paf != NULL ) {
    int  mod_loc = 0;

    affect.type     = AFF_POISON;
    affect.duration = max( duration, paf->duration );
    affect.level    = max( level, paf->level );
    affect.leech    = NULL;
    
    // find the first open slot. maka sure that location isn't already affected
    // and increase the penalty. copy the rest

    for( ; mod_loc < MAX_AFF_MODS; mod_loc++ ) {
      // copy the original affect first
      affect.mlocation[ mod_loc ] = paf->mlocation[ mod_loc ];
      affect.mmodifier[ mod_loc ] = paf->mmodifier[ mod_loc ];
    }

    for( mod_loc = 0; mod_loc < MAX_AFF_MODS; mod_loc++ ) {
      // find the previous affect with same location or next empty
      if( ( paf->mlocation[ mod_loc ] == poison && poison != 0 ) || paf->mlocation[ mod_loc ] == 0 ) {
        break;
      }
    }

    // this poor bastard is as poisoned as it gets
    if( mod_loc == MAX_AFF_MODS )
      return NULL;

    // same as a previous poison (stack the affects)
    if( paf->mlocation[ mod_loc ] == poison && poison != 0 ) {
      affect.mlocation[ mod_loc ] = paf->mlocation[ mod_loc ];
      affect.mmodifier[ mod_loc ] = paf->mmodifier[ mod_loc ] + modifier;
    
      send( victim, "[ The poison makes you feel %s %s. ]\r\n", modify_amount[ min( abs( modifier ), 5 ) ], affect_name[ affect.mlocation[ mod_loc ] ] );
    }
    // this is a new location or affect
    else {
      affect.mlocation[ mod_loc ] = saffect <= 0 ? affect_loc[ poison ] : saffect;
      affect.mmodifier[ mod_loc ] = saffect <= 0 ? modifier: 1;

      if( saffect <= 0 )
        send( victim, "[ The poison makes you feel %s %s. ]\r\n", modify_amount[ min( abs( affect.mmodifier[ mod_loc ] ), 5 ) ], affect_name[ affect.mlocation[ mod_loc ] ] );

      if( saffect > 0 ) {
        affectb.type     = saffect;
        affectb.duration = duration;
        affectb.level    = level;
        affectb.leech    = NULL;
        add_affect( victim, &affectb );
      }
    }

    remove_affect( victim, paf, FALSE );
    add_affect( victim, &affect, FALSE );

    return NULL;
  }

  affect.type           = AFF_POISON;
  affect.duration       = duration;
  affect.level          = level;
  affect.leech          = NULL;
  affect.mlocation[ 0 ] = saffect <= 0 ? affect_loc[ poison ] : saffect;
  affect.mmodifier[ 0 ] = saffect <= 0 ? modifier: 1;
  
  add_affect( victim, &affect, true );

  if( saffect <= 0 )
    send( victim, "[ The poison makes you feel %s %s. ]\r\n", modify_amount[ min( abs( affect.mmodifier[ 0 ] ), 5 ) -1 ], affect_name[ affect.mlocation[ 0 ] ] );

  if( saffect > 0 ) { /*

    switch( saffect ) {
    case AFF_SLOW:
      spell_slow( ch, victim, NULL, level, duration );
      return NULL;
    }
    */
    affectb.type     = saffect;
    affectb.duration = duration;
    affectb.level    = level;
    affectb.leech    = NULL;
    add_affect( victim, &affectb );
  }

  return NULL;
}

void* code_position( void** argument )
{
  char_data*   ch = (char_data*) argument[0];
  int         pos = (int) argument[1];
  int   nodisrupt = (int) argument[2];

  if( ch == NULL ) {
    code_bug( "Position:  Ch is NULL" );
    return NULL;
  }

  if( ch->mount )
    return NULL;
  
  if( pos < POS_SLEEPING || pos > POS_STANDING || pos == POS_FIGHTING ) {
    code_bug( "Position:  Illegal Position" );
    return NULL;
  }

  if( !nodisrupt && pos != POS_STANDING ) {
    if( is_dancing( ch ) ) {
      send( ch, "Your dance has been interrupted.\r\n" );
      end_dance( ch );
    }
    disrupt_spell( ch );
    set_delay( ch, 32 );
  }

  ch->position = pos;

  return NULL;
}


void* code_heal( void** argument )
{
  char_data* ch = (char_data*) argument[0];
  int i         = (int)        argument[1];

  if( ch != NULL ) {
    ch->hit += i;
    ch->hit = min( ch->hit, ch->max_hit );
    }

  return NULL;
}


void* code_modify_mana( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          i  = (int)        argument[1];

  if( ch == NULL || ch->mana+i < 0 ) 
    return (void*) FALSE;

  ch->mana += i;
  ch->mana = min( ch->mana, ch->max_mana );

  return (void*) TRUE;
}


void* code_drain_exp( void** argument )
{
  char_data *ch = (char_data*) argument[0];
  int i         = (int)       argument[1];

  if( ch != NULL && ch->species == NULL ) {
//    add_exp( ch, -i, "You lose %d exp!!\r\n" );
    add_exp( ch, -i );
    send( ch, "You %s %i exp!!\r\n", i >= 0 ? "lose" : "gain", abs( i ) ); 
  }

  return NULL;
}


/*
 *   FUNCTION CALLS
 */


void* code_acode( void** argument )
{
  room_data *room = (room_data*) argument[0];
  int i           = (int)        argument[1];

  action_data* action; 
  int               j  = 1;

  if( room == NULL ) {
    code_bug( "Code_acode: NULL room." );
    return NULL;
  }

  for( action = room->action; action != NULL; action = action->next ) 
    if( j++ == i ) {
      push( );
      execute( action );
      pop( );
      break;
    }

  if( action == NULL ) 
    code_bug( "Code_acode: NULL action." );

  return NULL;
}


void* code_mpcode( void** argument )
{
  char_data* mob = (char_data*) argument[0];
  int i          = (int)       argument[1];

  mprog_data* mprog;
  int             j  = 1;

  if( mob == NULL || mob->species == NULL ) {
    code_bug( "Code_mpcode: NULL mob or mob is a player." );
    return NULL;
    }

  for( mprog = mob->species->mprog; mprog != NULL; mprog = mprog->next ) 
    if( j++ == i ) {
      push( );
      execute( mprog );
      pop( );
      break;
      }

  if( mprog == NULL ) 
    code_bug( "Code_mpcode: NULL mprog." );

  return NULL;
}


void* code_opcode( void** argument )
{
  obj_data *obj = (obj_data*) argument[0];
  int i         = (int)       argument[1];

  oprog_data* oprog;
  int             j  = 1;

  if( obj == NULL ) {
    code_bug( "Opcode: NULL obj." );
    return NULL;
    }
 
  for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) 
    if( j++ == i ) {
      push( );
      execute( oprog );
      pop( );
      break;
      }

  if( oprog == NULL )
    code_bug( "Opcode: NULL oprog." );

  return NULL;
}


void* code_wait( void** argument )
{
  int i = (int) argument[0];

  queue_data* queue = new queue_data;

  end_prog   = TRUE;
  queue_prog = TRUE;
  
  queue->time = i;

  queue->room = var_room;
  queue->mob  = var_mob;
  queue->ch   = var_ch;
  queue->obj  = var_obj;
  queue->i    = var_i;

  queue->arg  = curr_arg;
  queue->next = queue_list;
  queue_list  = queue;

  return NULL;
}

void* code_is_drunk( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL || ch->species != NULL ) {
    code_bug( "Is_drunk: ch is NULL or a mob" );
    return FALSE;
    }

  return (void*) ( ch->pcdata->condition[COND_DRUNK] > 0 );
}

void* code_is_searching( void** argument )
{
  char_data*   ch  = (char_data*) argument[0];

  return (void*) ( ch != NULL && ch->pcdata != NULL
    && is_set( ch->pcdata->pfile->flags, PLR_SEARCHING ) );
}


void* code_is_follower( void** argument )
{
  char_data*   ch  = (char_data*) argument[0];
  
  return (void*) ( ch == NULL || is_set( &ch->status, STAT_FOLLOWER ) );
/*
  char_data*   leader;
  
  //    return (void*) ( ch == NULL || ch->leader != NULL );
  if( ch == NULL )
    return NULL;

  leader = ch->leader;
  return (void*) ( ch == NULL || ( leader != NULL && ( ch->in_room ==
    leader->in_room ) ) );
*/
}
  
void* code_is_dark_race( void** argument )
{
  char_data*   ch  = (char_data*) argument[0];

  return (void*) ( ch->shdata != NULL && ch->shdata->race > RACE_LIZARD );
}
  
void* code_is_light_race( void** argument )
{
  char_data*   ch  = (char_data*) argument[0];

  return (void*) ( ch->shdata != NULL && ch->shdata->race <= RACE_LIZARD );
}

void* code_char_in_room( void** argument )
{
  char_data*    ch  = (char_data*) argument[0];
  room_data*  room  = (room_data*) argument[1]; 

  return (void*) ( ch != NULL && ch->in_room == room );
}


void* code_is_name( void** argument )
{
  char*       str  = (char*) argument[0];
  char*  namelist  = (char*) argument[1];

  if( str == NULL || namelist == NULL )
    return (void*) FALSE;

  return (void*) is_name( str, namelist );
}


void* code_set_religion( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          i  = (int)        argument[1];

  if( ch == NULL || ch->pcdata == NULL )
    return NULL;

  if( i < 0 || i >= MAX_RELIGION ) {
    code_bug( "Code_set_religion: religion value out of range." );
    return NULL;
  } 

  ch->pcdata->religion = i;

  return NULL;
}  


void* code_tell( void** argument )
{
  char_data*      ch  = (char_data*) argument[0];
  char_data*  victim  = (char_data*) argument[1];
  char*       string  = (char*)      argument[2];

  char      tmp  [ 3*MAX_STRING_LENGTH ];

  if( victim == NULL || ch == NULL || victim->pcdata == NULL )
    return NULL;

  act_print( tmp, string, victim, ch, NULL, NULL, NULL, NULL, victim );
  process_ic_tell( ch, victim, tmp, "say" );

  return NULL;
}

void* code_time( void** argument )
{
  return (void*) ( weather.hour*60+weather.minute );
}

void* code_obj_name( void** argument )
{
  obj_data* obj = (obj_data*) argument[0];

  if( obj == NULL )
    return NULL;

  return obj->singular;
} 

void* code_is_night( void** argument )
{
  int time = weather.hour*60+weather.minute;

  return (void*) ( time < 300 || time > 1159 );
}

void* code_disarm( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          j  = (int)        argument[1];

  if( ch == NULL ) {
    code_bug( "Disarm: NULL character or level." );
    return NULL;
  }

  code_bug( "Disarm: Script should use 'untrap' function not 'disarm' function." );

  return (void*) ( ch->check_skill( SKILL_UNTRAP ) && number_range( 0, 20 ) > j );
}


void* code_untrap( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          j  = (int)        argument[1];

  if( ch == NULL ) {
    code_bug( "Untrap: NULL character or level." );
    return NULL;
  }

  return (void*) ( ch->check_skill( SKILL_UNTRAP ) && number_range( 0, 20 ) > j );
}


/*
 *   MOVEMENT ROUTINES
 */


void* code_modify_move( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          i  = (int)        argument[1];

  if( ch == NULL || ch->move+i < 0 ) 
    return (void*) FALSE;

  ch->move += i;
  ch->move  = min( ch->move, ch->max_move );
 
  return (void*) TRUE;
}


void* code_is_exhausted( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          i  = (int)        argument[1];

  if( ch == NULL )
    return (void*) TRUE;
  
  if( ch->move-i < 0 ) {
    send( ch, "You are too exhausted.\r\n" );
    return (void*) TRUE;
    }

  ch->move -= i;
 
  return (void*) FALSE;
}


/*
 *    Alignment Related
 */

void* code_is_good( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Good: NULL Character." );
    return (void*) FALSE;
  }

  return (void*) is_good( ch );
}

void* code_is_neutral( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Neutral: NULL Character." );
    return (void*) FALSE;
  }

  return (void*) is_neutral( ch );
}

void* code_is_evil( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Evil: NULL Character." );
    return (void*) FALSE;
  }

  return (void*) is_evil( ch );
}

void* code_is_lawful( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Lawful: NULL Character." );
    return (void*) FALSE;
  }

  return (void*) is_lawful( ch );
}

void* code_is_n2( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_N2: NULL Character." );
    return (void*) FALSE;
  }

  return (void*) is_n2( ch );
}

void* code_is_chaotic( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Chaotic: NULL Character." );
    return (void*) FALSE;
  }

  return (void*) is_chaotic( ch );
}

/*
 *   Variables
 */

void* code_get_integer( void** argument )
{
  thing_data*  thing  = (thing_data*) argument[0];
  char*        var    = (char*)       argument[1];

  if( thing == NULL ) { // || !thing->Is_Valid( ) ) {
    code_bug( "Get_Integer: Null thing" );
    return NULL;
  }

  return (void*) thing->Get_Integer( var );
}

void* code_set_integer( void** argument )
{
  thing_data*  thing  = (thing_data*) argument[0];
  char*        var    = (char*)       argument[1];
  int          value  = (int)         argument[2];

  if( thing == NULL ) { // || !thing->Is_Valid( ) ) {
    code_bug( "Set_Integer: Null argument" );
    return NULL;
  }

  thing->Set_Integer( var, value );

  return NULL;
}


/*
 *   Utilz
 */

void* code_atoi( void** argument )
{
  const char* string = (const char*) argument[ 0 ];

  if( string && is_number( string ) )
    return (void*) atoi( string );

  return (void*) 0;
}

void* code_itoa( void** argument )
{
  int i = (int) argument[ 0 ];

  return (void*) atos( i );
}

