#include "system.h"


bool   remove           ( char_data*, obj_data*& );
void   wear_obj         ( char_data*, obj_data*, int );
void   wear_trigger     ( char_data*, obj_data* ); 
void   wield_obj        ( char_data*, obj_data* ); 
bool   can_remove       ( char_data*, obj_data*, bool = FALSE );

const char* wear_name [] = {
  "take",
  "light",
  "head",
  "neck",
  "body", 
  "waist",
  "arms",
  "wrist",
  "hands",
  "finger",
  "jewelry",
  "legs",
  "feet",
  "right.hand",
  "left.hand",
  "about",
  "unused1",
  "unused2",
  "unused3",
  "unused4",
  "horse.body",
  "horse.back",
  "horse.foot"
};


const char* layer_name [ MAX_LAYER ] = {
  "bottom",
  "under",
  "base",
  "over",
  "top"
};


const char* curse_name [] = {
  "light",
  "head",
  "neck",
  "body",
  "waist",
  "arms",
  "wrist",
  "wrist",
  "hands",
  "finger",
  "finger",
  "jewelry",
  "legs",
  "feet",
  "hand",
  "hand",
  "back",
  "??",
  "??",
  "??",
  "??",
  "??",
  "body",
  "back",
  "feet"
};

const int wear_index [ MAX_WEAR ] = {
  ITEM_WEAR_FLOATING,
  ITEM_WEAR_HEAD,
  ITEM_WEAR_NECK,
  ITEM_WEAR_BODY,
  ITEM_WEAR_WAIST,
  ITEM_WEAR_ARMS,
  ITEM_WEAR_WRIST,
  ITEM_WEAR_WRIST,
  ITEM_WEAR_HANDS,
  ITEM_WEAR_FINGER,
  ITEM_WEAR_FINGER,
  ITEM_WEAR_JEWELRY,
  ITEM_WEAR_LEGS,
  ITEM_WEAR_FEET,
  ITEM_HELD_R,
  ITEM_HELD_L,
  ITEM_WEAR_ABOUT,
  ITEM_TAKE,
  ITEM_TAKE,
  ITEM_TAKE,
  ITEM_TAKE,
  ITEM_TAKE,
  ITEM_WEAR_HORSE_BODY,
  ITEM_WEAR_HORSE_BACK,
  ITEM_WEAR_HORSE_FEET
};


const int old_mresets[MAX_WEAR] = {
  -2,            // -2 -> Ground (-2)
  WEAR_NONE,     // -1 -> Inventory (-1)
  WEAR_FLOATING, // 0 -> Light
  WEAR_FINGER_R, // 1 -> Finger (1)
  WEAR_FINGER_L, // 2 -> Finger (1)
  WEAR_NECK,     // 3 -> Neck (2)
  WEAR_NECK,     // 4 -> Neck (2)
  WEAR_BODY,     // 5 -> Body (3)
  WEAR_HEAD,     // 6 -> Head (4)
  WEAR_LEGS,     // 7 -> Legs (5)
  WEAR_FEET,     // 8 -> Feet (6)
  WEAR_HANDS,    // 9 -> Hands (7)
  WEAR_ARMS,     // 10 -> Arms (8)
  WEAR_HELD_L,   // 11 -> Left Hand (14)
  WEAR_BODY,     // 12 -> Body (3)
  WEAR_WAIST,    // 13 -> Waist (11)
  WEAR_WRIST_R,  // 14 -> Wrist (12)
  WEAR_WRIST_L,  // 15 -> Wrist (12)
  WEAR_HELD_R,   // 16 -> Right Hand (13)
  WEAR_HELD_L,   // 17 -> Left Hand (14)
  WEAR_HELD_L,   // 18 -> Left Hand (14)
};


const int old_wear_locs[MAX_WEAR] = {
  ITEM_TAKE,                // 0
  ITEM_WEAR_FINGER,         // 1
  ITEM_WEAR_NECK,           // 2
  ITEM_WEAR_BODY,           // 3
  ITEM_WEAR_HEAD,           // 4
  ITEM_WEAR_LEGS,           // 5
  ITEM_WEAR_FEET,           // 6
  ITEM_WEAR_HANDS,          // 7
  ITEM_WEAR_ARMS,           // 8
  ITEM_WEAR_UNUSED2,        // 9
  ITEM_WEAR_UNUSED3,        // 10
  ITEM_WEAR_WAIST,          // 11
  ITEM_WEAR_WRIST,          // 12
  ITEM_HELD_R,              // 13
  ITEM_HELD_L,              // 14
  ITEM_WEAR_UNUSED1,        // 15
  ITEM_WEAR_UNUSED2,        // 16
  ITEM_WEAR_UNUSED3,        // 17
  ITEM_WEAR_UNUSED4,        // 18
  ITEM_WEAR_HORSE_BODY,     // 19
  ITEM_WEAR_HORSE_BACK,     // 20
  ITEM_WEAR_HORSE_FEET,     // 21
  MAX_ITEM_WEAR             // 22
};

const char** wear_part_name = &reset_wear_name[2];

const char* wear_abbrev[] =
{
  "Grnd",
  "Inv.",
  "Lght",
  "Head",
  "Neck",
  "Body",
  "Wst.",
  "Arms",
  "rWrs",
  "lWrs",
  "Hand",
  "rFgr",
  "lFgr",
  "Jewl",
  "Legs",
  "Feet",
  "rHnd",
  "lHnd",
  "Abt.",
  "??  ",
  "??  ",
  "??  ",
  "??  ",
  "??  ",
  "hBck",
  "hBdy",
  "hFt."
};
 
const char* reset_wear_name[] = {
  "ground",
  "inventory",
  "floating",
  "head",
  "neck",
  "body",
  "waist",
  "arms",
  "right.wrist",
  "left.wrist",
  "hands",
  "right.finger",
  "left.finger",
  "jewelry",
  "legs",
  "feet",
  "right.hand",
  "left.hand",
  "about",
  "unused1",
  "unused2",
  "unused3",
  "unused4",
  "unused5",
  "horse.body",
  "horse.back",
  "horse.foot"
}; 

const char* wear_verb [] =
{ 
  "toss into the air",
  "place on your head",
  "place around your neck",
  "wear",
  "place around your waist",
  "cover your arms with",
  "put on your right wrist",
  "put on your left wrist",
  "put your hands into",
  "place on your right hand",
  "place on your left hand",
  "wear",
  "cover your legs with", 
  "slip your feet into",
  "hold in your right hand",
  "hold in your left hand",
  "place around your body",
  "??",
  "??",
  "??",
  "??",
  "??",
  "covering body", 
  "placed on back",
  "attached to feet",
};

 
const char* where_name [ MAX_WEAR ] =
{
  "used as light     ",
  "worn on head      ",
  "worn around neck  ",
  "worn on body      ",
  "worn about waist  ",
  "worn on arms      ",
  "right wrist       ",
  "left wrist        ",
  "worn on hands     ", 
  "right hand finger ",
  "left hand finger  ",
  "worn as jewelry   ",
  "worn on legs      ",
  "worn on feet      ",
  "right hand        ",
  "left hand         ",
  "worn about body   ",
  "??                ",
  "??                ",
  "??                ", 
  "??                ",
  "??                ",
  "covering body     ", 
  "placed on back    ",
  "attached to feet  ",
};


/*
 *   SUPPORT ROUTINES
 */ 

const char* wear_loc( obj_data* obj )
{
  if( obj->position == WEAR_HELD_R || obj->position == WEAR_HELD_L ) {
    if( obj->pIndexData->item_type == ITEM_WEAPON )
      return "you are wielding";
    return "you are holding";
  }

  if( obj->position == WEAR_FLOATING )
    return "floating nearby";

  return "you are carrying";
}
  

obj_data* char_data :: Wearing( int slot, int layer )
{
  obj_data* obj;

  for( int j = 0; j < wearing; j++ ) {
    obj = (obj_data*) wearing[j];
    if( obj->position == slot && ( layer == -1 || obj->layer == layer ) ) 
      return obj;
  }

  return NULL;
}


/*
 *   CAN_USE ROUTINES
 */


bool will_fit( char_data* ch, obj_data* obj, int i )
{
  if( !is_set( &obj->pIndexData->wear_flags, wear_index[i] ) ) 
    return FALSE;

  if( ch->species == NULL ) { // player trying to wear it
    // Check validity of wear slot 'i' for player
    if ( wear_index[i] == ITEM_TAKE || i >= MAX_WEAR_HUMANOID ) 
      return FALSE;

    return TRUE;
  }

  // Mob trying to wear it.
  return is_set( &ch->species->wear_part, i );
}


bool can_use( char_data* ch, obj_clss_data* obj_clss, obj_data* obj, bool custom )
{
  if( ch->species != NULL && !is_set( ch->species->act_flags, ACT_CAN_CARRY ) ) 
    return FALSE;

  /*  CUSTOM FIT  */

  if( is_set( &obj_clss->size_flags, SFLAG_CUSTOM ) 
    && ( ch->pcdata == NULL || ( !custom
    && ( ch->pcdata->pfile->ident != obj->Get_Integer( CUSTOM_FIT ) 
    && obj->Get_Integer( CUSTOM_FIT ) != 0 ) ) ) ) 
    return FALSE;

  /*  SIZE */

  if( is_set( &obj_clss->size_flags, SFLAG_RACE )
    && ch->shdata->race < MAX_PLYR_RACE 
    && !is_set( &obj_clss->size_flags, SFLAG_HUMAN+ch->shdata->race ) ) 
    return FALSE;

  if( is_set( &obj_clss->size_flags, SFLAG_SIZE )
    || ( obj != NULL && is_set( &obj_clss->size_flags, SFLAG_RANDOM ) ) ) 
    if( !is_set( obj == NULL ? &obj_clss->size_flags
      : &obj->size_flags, wear_size( ch ) ) ) 
      return FALSE;
 
  /* STOLEN */

    /*
  if( ch->shdata->alignment != ALGN_CHAOTIC_EVIL && obj != NULL
    && !obj->Belongs( ch ) ) 
    return FALSE;
    */

  /*  CLASS RESTRICTIONS */

  if( is_apprentice( ch ) )
    return TRUE;

  if( ch->shdata->level < obj_clss->level ) 
    return FALSE;

  if( ( ch->shdata->race < RACE_UNKNOWN_PLAYER1 && 
    is_set( &obj_clss->anti_flags, ANTI_HUMAN+ch->shdata->race ) )
    || ( ch->species == NULL && is_set( &obj_clss->anti_flags, ANTI_MAGE+ch->pcdata->clss )
    || is_set( &obj_clss->anti_flags, ANTI_GOOD+ch->shdata->alignment%3 )
    || is_set( &obj_clss->anti_flags, ANTI_LAWFUL+ch->shdata->alignment/3 ) ) )
    return FALSE; 

  /* RELIGION RESTRICTIONS */

  if( ch->pcdata != NULL && is_set( &obj_clss->size_flags, SFLAG_RELIGION ) &&
    !is_set( &obj_clss->religion_flags, REL_NONE )  
    && !is_set( &obj_clss->religion_flags, ch->pcdata->religion ) )
    return FALSE;

  return TRUE;


}


/*
 *   WEAR ROUTINES
 */ 


thing_data* cantwear( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data*    obj = (obj_data*) t1;

  for( int i = 0; !is_set( &obj->pIndexData->layer_flags, i ); )
    if( ++i == MAX_LAYER ) 
      return NULL; 
      
  for( int i = 0; i < MAX_WEAR; i++ ) 
    if( will_fit( ch, obj, i ) && !execute_use_trigger( ch, obj, OPROG_TRIGGER_USE ) )
      return obj;

  return NULL;
}

thing_data* wrongsize( thing_data* t1, char_data* ch, thing_data* )
{
   obj_data* obj;
   if ( (obj = object( t1 )) == NULL )
      return NULL;

   // Return NULL if obj is only wearable by another size or race.
   obj_clss_data* obj_clss = obj->pIndexData;
   if( is_set( &obj_clss->size_flags, SFLAG_CUSTOM ) &&
     obj->Get_Integer( CUSTOM_FIT ) == 0 )
     obj->Set_Integer( CUSTOM_FIT, (ch->pcdata == NULL ? -1 : ch->pcdata->pfile->ident ) );

   // Custom built eq
   if( is_set( &obj_clss->size_flags, SFLAG_CUSTOM ) && 
       ( ch->pcdata == NULL || 
         ch->pcdata->pfile->ident != obj->Get_Integer( CUSTOM_FIT ) ) ) 
       return NULL;

   // Race-specific eq
   if( is_set( &obj_clss->size_flags, SFLAG_RACE ) && 
       ch->shdata->race < MAX_PLYR_RACE && 
       !is_set( &obj_clss->size_flags, SFLAG_HUMAN + ch->shdata->race ) )
       return NULL;

   // Size-specific eq
   if( (is_set( &obj_clss->size_flags, SFLAG_SIZE ) || 
        is_set( &obj_clss->size_flags, SFLAG_RANDOM )) &&
       !is_set( &obj->size_flags, wear_size(ch) ) ) 
       return NULL;
   
   return obj;
}

thing_data* bladed( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data* obj = (obj_data*) t1;

  if( ch->pcdata != NULL && ch->pcdata->clss == CLSS_CLERIC 
    && is_set( &obj->pIndexData->restrictions, RESTR_BLADED ) ) 
    return NULL;

  return obj;
}


thing_data* dishonorable( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data* obj = (obj_data*) t1;

  if( ch->pcdata != NULL && ch->pcdata->clss == CLSS_PALADIN 
    && is_set( &obj->pIndexData->restrictions, RESTR_DISHONORABLE ) ) 
    return NULL;

  return obj;
}


thing_data* levellimit( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data* obj  = (obj_data*) t1;

  if( ch->species != NULL )
  return obj;

  if( ch->shdata->level < obj->pIndexData->level ) 
    return NULL;

  return obj;
}


thing_data* antiobj( thing_data* t1, char_data* ch, thing_data* )
{
  // surely we should just call can_use??

  obj_data*            obj  = (obj_data*) t1;
  obj_clss_data*  obj_clss  = obj->pIndexData;

  if (is_apprentice(ch))
    return obj;

  if( ( ch->shdata->race < MAX_PLYR_RACE && is_set( &obj_clss->anti_flags, ANTI_HUMAN+ch->shdata->race ) )
    || ( ch->species == NULL  && is_set( &obj_clss->anti_flags, ANTI_MAGE+ch->pcdata->clss ) )
    || is_set( &obj_clss->anti_flags, ANTI_GOOD+ch->shdata->alignment%3 )
    || is_set( &obj_clss->anti_flags, ANTI_LAWFUL+ch->shdata->alignment/3 ) ) 
    return NULL; 

  return obj;
}

thing_data* wrongrelig( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data*             obj = (obj_data*) t1;
  obj_clss_data*   obj_clss = obj->pIndexData;

  if( is_apprentice( ch ) || ch->pcdata == NULL )
    return obj;

  if( is_set( &obj_clss->size_flags, SFLAG_RELIGION ) && 
    !is_set( &obj_clss->religion_flags, REL_NONE ) &&
    !is_set( &obj_clss->religion_flags, ch->pcdata->religion ) )
    return NULL;

  return obj;
}

thing_data* needremove( thing_data* t1, char_data* ch, thing_data* t2 )
{
  obj_data*       obj  = (obj_data*) t1;
  thing_array*  array  = (thing_array*) t2;
  obj_data*      temp  = NULL;
  bool           once  = false;

  for (int i = 0; i < MAX_WEAR; i++) {
    if( will_fit( ch, obj, i ) ) {
      if( i == WEAR_HELD_R || i == WEAR_HELD_L ) {
        // already have weapons in both hands, skip this

        // check offhand attack  -- who cares about off-hand if primary hand is empty
        if( i == WEAR_HELD_L && obj->pIndexData->item_type == ITEM_WEAPON ) {
          if( get_weapon( ch, WEAR_HELD_L ) && get_weapon( ch, WEAR_HELD_R ) ) {
            if( !once ) {
              send( ch, "You are wielding items in both hands, so are unable to off-hand %s.\r\n", obj );
              once = true;
            }
            continue;
          }
          if( obj->value[3] == WEAPON_DAGGER - WEAPON_FIRST && !ch->get_skill( SKILL_OFFHAND_ATTACK ) && !ch->get_skill( SKILL_DUAL_WIELD ) && ( get_weapon( ch, WEAR_HELD_L ) || get_weapon( ch, WEAR_HELD_R ) ) ) {
            if( ch->Wearing( WEAR_HELD_L, LAYER_BASE ) == NULL )
              if( !once ) {
                send( ch, "You are unskilled in the ability to off-hand daggers.\r\n" );
                once = true;
              }
            continue;
          }

          if( obj->value[3] == WEAPON_SWORD - WEAPON_FIRST && ( !ch->get_skill( WEAPON_OHSWORD ) ) && ( get_weapon( ch, WEAR_HELD_L ) || get_weapon( ch, WEAR_HELD_R ) ) ) {
            if( ch->Wearing( WEAR_HELD_L, LAYER_BASE ) == NULL )
              if( !once ) {
                send( ch, "You are unskilled in the ability to off-hand swords.\r\n" );
                once = true;
              }
            continue;
          }
  
          if( obj->value[3] == WEAPON_AXE - WEAPON_FIRST && ( !ch->get_skill( WEAPON_OHAXE ) ) && ( get_weapon( ch, WEAR_HELD_L ) || get_weapon( ch, WEAR_HELD_R ) ) ) {
            if( ch->Wearing( WEAR_HELD_L, LAYER_BASE ) == NULL )
              if( !once ) {
                send( ch, "You are unskilled in the ability to off-hand axes.\r\n" );
                once = true;
              }
            continue;
          }

          if ( obj->value[3] == WEAPON_SPEAR - WEAPON_FIRST && ( !ch->get_skill( WEAPON_OHSPEAR ) ) && ( get_weapon( ch, WEAR_HELD_L ) || get_weapon( ch, WEAR_HELD_R ) ) ) {
            if( ch->Wearing( WEAR_HELD_L, LAYER_BASE ) == NULL )
              if( !once ) {
                send( ch, "You are unskilled in the ability to off-hand spears.\r\n" );
                once = true;
              }
            continue;
          }
        }

        obj_data *temp;
        bool two_handed = is_set( obj->pIndexData->extra_flags, OFLAG_NO_SHIELD );
        for( int j = 0; j < MAX_LAYER; j++ ) {
          if( !is_set( &obj->pIndexData->layer_flags, j ) )
            continue; // doesn't fit in this slot

          if( ch->Wearing( i, j ) ) {
            if( !once ) {
              send( ch, "You will need to remove %s to equip %s.\r\n", ch->Wearing( i, j ), obj );
              once = true;
            }
            continue; // already something in this slot
          }

          if( two_handed ) {
            if( ch->Wearing( WEAR_HELD_R, j ) ) {
              if( !once ) {
                send( ch, "You will need to remove %s to equip %s.\r\n", ch->Wearing( WEAR_HELD_R, j ), obj );
                once = true;
              }
              continue; // is two handed and slot's already used
            }
            if( ch->Wearing( WEAR_HELD_L, j ) ) {
              if( !once ) {
                send( ch, "You will need to remove %s to equip %s.\r\n", ch->Wearing( WEAR_HELD_L, j ), obj );
                once = true;
              }
              continue; // is two handed and slots already used
            }
          }

          if( ( ( temp = ch->Wearing( WEAR_HELD_R, j ) ) != NULL ) && is_set( temp->pIndexData->extra_flags, OFLAG_NO_SHIELD ) ) {
            if( !once ) {
              send( ch, "You must remove %s to wear %s.\r\n", temp, obj );
              once = true;
            }
            continue; // slot's already used by a 2 handed weapon in right hand
          }

          if( ( ( temp = ch->Wearing( WEAR_HELD_L, j ) ) != NULL ) && is_set( temp->pIndexData->extra_flags, OFLAG_NO_SHIELD ) ) {
            if( !once ) {
              send( ch, "You must remove %s to wear %s.\r\n", temp, obj );
              once = true;
            }
            continue; // slot's already used by a 2 handed weapon in left hand
          }

          obj_data *worn = (obj_data*) obj->From( 1 );
          worn->position = i;
          worn->layer    = j;
          worn->To( &ch->wearing );
          once = true;
          if( array != NULL )
            *array += worn;

          if( worn == obj || obj->selected == 0 )
            return obj;
        }
      } else {
        for( int j = 0; j < MAX_LAYER; j++ ) {
          if( temp == NULL && is_set( &obj->pIndexData->layer_flags, j ) )
            temp = ch->Wearing( i, j );
          if( ch->Wearing( i, j ) == NULL && is_set( &obj->pIndexData->layer_flags, j ) ) {
            obj_data *worn = (obj_data*) obj->From( 1 );
            worn->position = i;
            worn->layer    = j;
            temp = NULL;
            once = true;
            worn->To( &ch->wearing );
            if( array != NULL )
              *array += worn;

            if( worn == obj || obj->selected == 0 )
              return obj;
          }
        }
      }
    }
  }

  if( temp != NULL && !once ) {
    send( ch, "You must remove %s to equip %s.\r\n", temp, obj );
    once = true;
  }

  return NULL;  
}


void do_wear( char_data* ch, char* argument )
{
  thing_array* array;

  if( is_confused_pet( ch ) )
    return; 

  if( ch->shifted != NULL ) {
    send( ch, "You must be in your natural form to do that.\r\n" );
    return;
  }

  if( ( array = several_things( OBJ_DATA, ch, argument, "wear", &ch->contents ) ) == NULL )
    return;

  thing_array   subset  [ 10 ];
  thing_func*     func  [ 10 ]  = { cantwear, wrongsize, wrongrelig,
                                    stolen, bladed, dishonorable, 
                                    antiobj, levellimit, needremove, NULL };

  sort_objects( ch, *array, (thing_data*) &subset[9], 10, subset, func );

  msg_type = MSG_EQUIP;

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't wear" );
  page_priv( ch, &subset[1], NULL, NULL, "doesn't fit you", "don't fit you" );
  page_priv( ch, &subset[2], "are the wrong religion to equip" );
  page_priv( ch, &subset[3], NULL, NULL, "is stolen", "are stolen" );
  page_priv( ch, &subset[4], NULL, NULL, "is a bladed weapon and forbidden to you", "are bladed weapons and forbidden to you" ); 
  page_priv( ch, &subset[5], "you consider", NULL, "dishonorable" );
  page_priv( ch, &subset[6], "sense aversion from" );
  page_priv( ch, &subset[7], "can't cope with" );
//  page_priv( ch, &subset[8], "need to remove something to equip" );
  page_priv( ch, &subset[8], empty_string );
  list_wear( ch, &subset[9] );

  delete array;
}


void equip( char_data* ch, obj_data* obj )
{
  obj_data*    worn;
  affect_data    af;

  if( !is_set( obj->pIndexData->extra_flags, OFLAG_ADDITIVE ) )
    for( int i = 0; i < ch->wearing; i++ )
      if( ( worn = object( ch->wearing[i] ) ) != NULL && worn->pIndexData == obj->pIndexData && worn != obj )
        return;

  if( is_set( obj->extra_flags, OFLAG_NOREMOVE ) ) 
    if( ch->In_Game( ) )
      send( ch, "%s welds itself to your body!\r\n", obj );

  for( int i = 0; i < MAX_ENTRY_AFF_CHAR; i++ )
    if( is_set( obj->pIndexData->affect_flags, i ) ) {
      af.type = i;
      af.level = 7;
      modify_affect( ch, &af, TRUE );
    }

  for( int i = 0; i < obj->pIndexData->affected; i++ )
    modify_affect( ch, obj->pIndexData->affected[i], TRUE );

  // Execute WEAR triggers, unless character is not in a room.
  if( ch->array != NULL ) {
    oprog_data* oprog;
    for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) {
      if( oprog->trigger == OPROG_TRIGGER_AFTER_USE ) {
        var_ch = ch;
        var_obj = obj;
        var_room = ch->in_room;
        execute( oprog );
      }
    }
  }
}


/*
 *   REMOVE ROUTINES
 */


thing_data* noremove( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = (obj_data*) thing;

  return( is_set( obj->extra_flags, OFLAG_NOREMOVE )
    ? NULL : obj );
}


thing_data* nocatch( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = (obj_data*) thing;

  if( obj->position == WEAR_FLOATING && ( obj->pIndexData->vnum == OBJ_CORPSE_LIGHT ||
    obj->pIndexData->vnum == OBJ_DEAD_MAN_EYES ) || obj->pIndexData->vnum == OBJ_FIREFLY )
    return NULL; 

  return obj;
}


thing_data* remove( thing_data* thing, char_data* ch, thing_data* )
{
  thing = thing->From( 1 );
  thing->position = WEAR_NONE;
  thing->To( ch );

  return thing;
}


void remove_obj_from( char_data* ch, char_data* pet_char, obj_data* obj )
{
  if( !is_demigod( ch ) && ( pet_char->leader != ch || !is_set( &pet_char->status, STAT_PET ) ) ) {
    send( ch, "%s isn't a pet of yours.\r\n", pet_char );
    return;
  }

  // Will this work correctly if the character can't take any more items?
  // (either # items is maxed out or carrying weight is maxed out)
  (void) remove( obj, ch, NULL );
  send(ch, "You remove %s from %s.\r\n", obj, pet_char);
  send(pet_char, "%s removes %s from you.\r\n", ch, obj);
  send_seen(ch, "%s removes %s from %s.\r\n", ch, obj, pet_char);
}


void do_remove( char_data* ch, char* argument )
{
  char            arg  [ MAX_INPUT_LENGTH ];
  thing_array*  array;

  if( is_confused_pet( ch ) ) 
    return;

  if( ch->shifted != NULL ) {
    send( ch, "You must be in your natural form to do that.\r\n" );
    return;
  }

  if( contains_word( argument, "from", arg ) ) {
    char_data* pet_char;
    obj_data* removed_obj;
    if( ( pet_char = one_character( ch, argument, "remove", ch->array ) ) != NULL ) {
      if( ( removed_obj = one_object( ch, arg, "remove", &pet_char->wearing ) ) == NULL )  {
        send( ch, "%s isn't holding anything like that.\r\n", pet_char );
      }
      else {
        remove_obj_from( ch, pet_char, removed_obj ); 
      }
    }
    return;
  }

  if( ( array = several_things( OBJ_DATA, ch, argument, "remove", &ch->wearing ) ) == NULL )
    return;

  thing_array   subset  [ 3 ];
  thing_func*     func  [ 3 ]  = { noremove, nocatch, remove };

  sort_objects( ch, *array, NULL, 3, subset, func );

  msg_type = MSG_EQUIP;

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't remove" );
  page_priv( ch, &subset[1], "can't catch" );
  page_publ( ch, &subset[2], "remove" );

  consolidate( subset[2] );

  delete array;
}


void unequip( char_data* ch, obj_data* obj )
{
  obj_data*    worn;
  affect_data    af;

  if( !is_set( obj->pIndexData->extra_flags, OFLAG_ADDITIVE ) )
    for( int i = 0; i < ch->wearing; i++ )
      if( ( worn = object( ch->wearing[i] ) ) != NULL && worn->pIndexData == obj->pIndexData && worn != obj )
        return;

  for( int i = 0; i < MAX_ENTRY_AFF_CHAR; i++ )
    if( is_set( obj->pIndexData->affect_flags, i ) ) {
      af.type = i;
      af.level = 7;
      modify_affect( ch, &af, FALSE  );
    }

  for( int i = 0; i < obj->pIndexData->affected; i++ )
    modify_affect( ch, obj->pIndexData->affected[i], FALSE );
}


