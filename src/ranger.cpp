#include "system.h"

int  current_condition    ( char_data* );

// We need a global variable to communicate between scan_room and the
// virtual function Char_Data::Seen().
bool tfh_assume_lit = FALSE;

/*
 *   SKIN FUNCTION
 */

// returns true if it extracted the corpse
bool skin_corpse( char_data *ch, obj_data *corpse )
{
  mprog_data*         mprog;
  thing_array*         list;
  species_data*     species;
  char_data*            rch;
  bool                 skin = false;

  if( !corpse || !corpse->Is_Valid() ) {
    roach("skin_corpse: invalid corpse!", ch );
    return false;
  }

  if( corpse->pIndexData->item_type != ITEM_CORPSE ) {
    send( ch, "You can only skin corpses.\r\n" );
    return false;
  }

  if( corpse->pIndexData->vnum == OBJ_CORPSE_PC ) {
    send( ch, "You may not skin the corpses of players or their pets.\r\n" );
    return false;
  }

  if( ( species = get_species( corpse->value[1] ) ) == NULL ) {
    send( ch, "Corpses without a species cannot be skinned.\r\n" );
    return false;
  }

  for( mprog = species->mprog; mprog != NULL; mprog = mprog->next )
    if( mprog->trigger == MPROG_TRIGGER_SKIN ) {
      var_obj  = corpse;
      var_ch   = ch;    
      var_room = ch->in_room;
      skin = execute( mprog );
      if( !skin )
        return false;
    }

  list = get_skin_list( species, ch );

  if( list == (thing_array*) -1 ) {
    send( ch, "You cannot skin %s.\r\n", corpse );
    return false;
  }

  bool dropped = false;

  if( !skin ) {
    if( list == NULL ) {
      fsend( ch, "You skin %s, but it is too mangled to be of any use.", corpse );
      fsend( *ch->array, "%s skins %s but is unable to extract anything of value.", ch, corpse );
    } else {
      fsend( ch, "You skin %s producing %s.", corpse, list );

      for( int i = 0; i < *ch->array; i++ )
        if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) )
          fsend( rch, "%s skins %s producing %s.\r\n", ch, corpse, list );

      for( int i = 0; i < *list; i++ ) {
        obj_data *obj = object(list->list[i]);
        if( !can_carry( ch, obj, false ) ) {
          dropped = true;
          obj->To( ch->array );
        } else {
          obj->To( ch );
        }
        consolidate( obj );
      }

      delete list;
    }
  }

  int quantity = drop_contents( corpse );

  if( quantity || dropped ) {
//    send( ch, "Some objects fall to the ground.\r\n" );
    send( ch, "%s to the ground.\r\n", quantity > 1 ? "Some objects fall" : "An object falls",
      quantity > 1 ? "" : "s" );
    fsend_seen( ch, "%s to the ground.", quantity > 1 ? "Some objects fall" : "An object falls" );
  }

  ch->improve_skill( SKILL_SKINNING );
  corpse->Extract( 1 );
  return true;
}


void do_skin( char_data* ch, char* argument )
{
  obj_data*          corpse;
  thing_data*         thing;

  if( is_confused_pet( ch ) )
    return;

  if( ( thing = one_thing( OBJ_DATA, ch, argument, "skin", ch->array ) ) == NULL )
    return;

  if( ( corpse = object( thing ) ) == NULL ) {
    fsend( ch, "Skinning %s alive would be cruel and unusual.\r\n", thing );
    return;
  } 

  skin_corpse(ch, corpse);
}


/*
 *   SCAN FUNCTIONS
 */

bool scan_room( char_data* ch, room_data* room, int door, const char* word, bool need_return )
{
  char      contents  [ MAX_STRING_LENGTH ];
  const char*   name;
  char_data*     rch;
  bool         found  = FALSE;
  int         length  = 0; 

  action_data* action;
  bool result = true;

  for( action = room->action; action != NULL; action = action->next ) {
    if( action->trigger == TRIGGER_SCAN && is_set( &action->flags, door + 1 ) ) {
      var_ch   = ch; 
      var_room = room;
      result = execute( action );
      break;
    }
  }
 
  if( !result || !action && is_set( &room->room_flags, RFLAG_NO_AUTOSCAN ) )
    return false;

  tfh_assume_lit = !ch->in_room->is_dark();
  room->distance = 0;

  select( room->contents, ch );
  rehash( ch, room->contents );

  // If scanner's room is lit, then assume mobs in adjoining room can be seen.
  for( int i = 0; i < room->contents; i++ ) {
    if( ( rch = character( room->contents[i] ) ) == NULL || rch->shown == 0
      || !rch->Seen( ch ) || ch == rch || ( rch->species != NULL && is_set( rch->species->act_flags, ACT_MIMIC ) ) )
      continue;

    name = rch->Seen_Name( ch, rch->shown );

    if( !found ) {
      sprintf( contents, "%s", name );
      length = strlen( contents );
      found  = TRUE;
    } else {
      length += strlen( name ) + 2;
      if( length > 75 ) {
        length = strlen( name ) + 7;
        sprintf( contents + strlen( contents ), ",\r\n              %s", name );
      } else
        sprintf( contents + strlen( contents ), ", %s", name );
    }
  }

  if( action || found ) {
    char tmp [ 256 ];
    sprintf( tmp, "%12s", word );

    if( need_return )
      send( ch, "\r\n" );

    act( ch, prog_msg( action, scan_msg[0], scan_msg[1] ), ch, NULL, tmp, contents );
  }

  tfh_assume_lit = FALSE;        // Restore global var to its natural state
  return ( action || found );
} 


void do_scan( char_data* ch, char* argument )
{
  char             tmp [ ONE_LINE ];
  room_data*      room = ch->in_room;
  bool        anything = FALSE;
  bool         is_auto = !strcmp( argument, "shrt" );

  int            level = ( ch && ch->pcdata && ch->pcdata->pfile ? level_setting( &ch->pcdata->pfile->settings, SET_AUTOSCAN ) : 3 );

  if( !ch->Can_See( false ) ) {
    if( !is_auto )
      send( ch, "You are blind currently, how do you expect to scan?!?\r\n" );
    return;
  }

  if( !ch->in_room->Seen( ch ) ) {
    if( !is_auto )
      send( ch, "The room is too dark to scan.\r\n" );
    return;
  }

  // prevents you from scanning out of a no.scan room
  if( is_set( &room->room_flags, RFLAG_NO_AUTOSCAN ) ) {
    if( !is_auto ) {
      anything = scan_room( ch, room, -1, "[Here]", FALSE );
      if( !anything )
        send( ch, "You see nothing in the vicinity.\r\n" );
    }
    return;
  }

  if( is_auto && is_set( &room->room_flags, RFLAG_TOWN ) && level != 3 )
    return;

  if( number_range( 1, 20 ) == 4 )
    ch->improve_skill( SKILL_SCAN );

  if( !is_auto ) 
    anything = scan_room( ch, room, -1, "[Here]", FALSE );

  room->distance = 0;
  
  for( int i = 0; i < room->exits; i++ ) {
    if( is_set( &room->exits[ i ]->exit_info, EX_CLOSED ) )
      continue;

    room_data*  room1 = room->exits[ i ]->to_room;
    int         door1 = room->exits[ i ]->direction;

    anything |= scan_room( ch, room1, dir_table[ door1 ].reverse, dir_table[ door1 ].name, !anything && is_auto );

    if( ch->get_skill( SKILL_SCAN ) != UNLEARNT && ( !is_auto || level > 1 ) && ( !is_auto || !is_set( &room1->room_flags, RFLAG_TOWN ) ) ) {
    for( int j = 0; j < room1->exits; j++ ) {
        if( is_set( &room1->exits[j]->exit_info, EX_CLOSED ) )
          continue;

        room_data*  room2 = room1->exits[j]->to_room;
        int         door2 = room1->exits[j]->direction;

        if( room2->distance == 0 )
          continue;

        sprintf( tmp, "%s %s", door1 == door2 ? "far" : dir_table[ door1 ].name, dir_table[ door2 ].name );
        anything |= scan_room( ch, room2, dir_table[ door2 ].reverse, tmp, !anything && is_auto );
      }
    }
  }

  /*--  CLEANUP DISTANCE --*/
  room->distance = MAX_INTEGER;

  for( int i = 0; i < room->exits; i++ ) { 
    room_data* room1 = room->exits[i]->to_room;
    room1->distance = MAX_INTEGER;
    for( int j = 0; j < room1->exits; j++ )
      room1->exits[j]->to_room->distance = MAX_INTEGER;
  }

  if (!anything && !is_auto) 
    send(ch, "You see nothing in the vicinity.\r\n");
}


/*
 *   SPELLS & SKILLS
 */

bool ranger_skill( char_data* ch, int skill, int moves )
{
  if( null_caster( ch, skill ) )
    return TRUE;

  if( !ch->get_skill( skill ) ) {
  send( ch, "You know very little of %s.\r\n",
    skill == SKILL_FORAGE ? "foraging" : skill == SKILL_DOWSE 
    ? "searching for water" : skill == SKILL_FIND_TINDER 
    ? "objects suitable as a light source from the wilderness"
    : "making a camp site" ); 
    return TRUE;
    }

  if( !ch->Can_See( ) ) {
    send( ch, "How do you expect to %s while unable to see.\r\n", skill == SKILL_FORAGE ? "forage for food" :
      skill == SKILL_FIND_TINDER ? "find a light source" : skill == SKILL_DOWSE ? "search for suitable water" : "create a camp" );
    return TRUE;
  }

  if( ch->in_room != NULL && terrain_table[ ch->in_room->sector_type ].civilized  ) {
    send( ch, "You should go %s some %s in the city.\r\n",
    skill == SKILL_DOWSE || skill == SKILL_CAMP_SITE ? "find" : "buy",
    skill == SKILL_DOWSE ? "water" : skill == SKILL_FORAGE ? "food" 
    : skill == SKILL_FIND_TINDER ? "lights" : "shelter" );
    return TRUE;
    }

  if( ch->move < moves ) {
    send( ch, "You are a little tired to be doing that right now.\r\n" );
    return TRUE;
    }
  
  return FALSE;
}

void do_forage( char_data* ch, char * )
{
  obj_data*            obj;  
  obj_clss_data*  obj_clss;
  int                 item;
  int                moves;
  int           poss_entry = 0;
  int            possibles [ MAX_FORAGE ];

  moves = move_cost( ch, SKILL_FORAGE );

  if( ranger_skill( ch, SKILL_FORAGE, moves ) )
    return;

  int level = 10*ch->get_skill( SKILL_FORAGE )/MAX_SKILL_LEVEL;

  for( int i = 0; i < MAX_ENTRY_FORAGE; i++ ) {
    if( is_set( &forage_table[i].terrain, ch->in_room->sector_type )
        && forage_table[i].open && level > forage_table[i].level ) {
        possibles[ poss_entry ] = forage_table[i].food[0];
        poss_entry++;
    }
  }

  if( MAX_ENTRY_FORAGE == 0 ) {
    bug( "Forage Table has no entries." );
    send( ch, "Forage has not been completed, or the entries have all been deleted.  Please post a bug note about this.\r\n" );
    return;
  }

  if( poss_entry == 0 ) {
    send( ch, "You are unable to find anything to consume.\r\n" );
    return;
  }

  item = number_range( 0, poss_entry-1 );
  item = possibles[ item ];

  if( ( obj_clss = get_obj_index( item ) ) == NULL ) {
    bug( "Item set to NULL item." );
    send( ch, "Forage item was NULL.\r\n" );
    return;
  }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Forage: NULL object" );
    return;
  }  

  send( ch, "You forage around the %s and find %s.\r\n", 
  terrain_table[ ch->in_room->sector_type ].terrain_name, obj );

  obj->To( ch );

  ch->improve_skill( SKILL_FORAGE );

  int delay = ch->species != NULL ? 6 : 20-5*ch->get_skill( SKILL_FORAGE )/MAX_SKILL_LEVEL;
  set_delay(ch, delay);
  ch->move -= moves;

  return;
}

void do_tinder( char_data* ch, char * )
{
  obj_data*            obj;  
  obj_clss_data*  obj_clss;
  int                 item;
  int                moves = move_cost( ch, SKILL_FIND_TINDER );;
  int                level = 10*ch->get_skill( SKILL_FIND_TINDER )/MAX_SKILL_LEVEL;
  int            possibles [ MAX_FORAGE ];
  int           poss_entry = 0;
  
  if( ranger_skill( ch, SKILL_FIND_TINDER, moves ) )
    return;

  for( int i = 0; i < MAX_ENTRY_TINDER; i++ ) {
    if( is_set( &tinder_table[i].terrain, ch->in_room->sector_type )
        && tinder_table[i].open && level > tinder_table[i].level ) {
        possibles[ poss_entry ] = tinder_table[i].food[0];
        poss_entry++;
    }
  }

  if( MAX_ENTRY_TINDER == 0 ) {
    bug( "Tinder Table has no entries." );
    send( ch, "Tinder has not been completed, or the entries have all been deleted.  Please post a bug note about this.\r\n" );
    return;
  }

  if( poss_entry == 0 ) {
    send( ch, "You are unable to find anything to use as a light source.\r\n" );
    return;
  }

  item = number_range( 0, poss_entry-1 );
  item = possibles[ item ];

  if( ( obj_clss = get_obj_index( item ) ) == NULL ) {
    bug( "Item set to NULL item." );
    send( ch, "Tinder item was NULL.\r\n" );
    return;
  }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Tinder: NULL object" );
    return;
  }  

  send( ch, "You search around the %s and find %s.\r\n", 
    terrain_table[ch->in_room->sector_type].terrain_name, obj );

  obj->To( ch );

  ch->improve_skill( SKILL_FIND_TINDER );

  int delay = ch->species != NULL ? 6 : 20-5*ch->get_skill( SKILL_FIND_TINDER )/MAX_SKILL_LEVEL;
  set_delay(ch, delay);
  ch->move -= moves;
  return;
}

void do_dowse( char_data* ch, char* argument )
{
  content_array*      where;
  obj_data*             obj;
  obj_data*             obj2;
  obj_clss_data*   obj_clss;
  int moves;

  moves = move_cost( ch, SKILL_DOWSE );

  if( argument == '\0' ) {
    send( ch, "What do you wish to put the water you find into?\r\n" );
    return;
  }

  if( ( obj = one_object( ch, argument, "to put water into", &ch->contents ) ) == NULL )
    return;

  if( obj->pIndexData->item_type != ITEM_DRINK_CON ) {
    send( ch, "You can't fill that with water, to drink from it.\r\n" );
    return;
  }

  if( ranger_skill( ch, SKILL_DOWSE, moves ) )
    return;

  obj->selected = 1;
  obj->shown = 1;

  obj_clss = obj->pIndexData;
  obj2 = create( obj_clss );
  obj2->value[1] = obj->value[1];

  bool desert = terrain_table[ ch->in_room->sector_type ].desert? true : false;

  where = obj->array;
  obj->From( 1 );

  int water = 400*ch->get_skill( SKILL_DOWSE )/MAX_SKILL_LEVEL;
  if( desert )
    water /= 2;
  obj->value[1] += water;
  obj->value[2] = LIQ_WATER;

  if( obj->value[1] >= obj->value[0] ) {
    send( ch, "You find enough water to fill %s.\r\n", obj2 );
    send_seen( ch, "%s finds enough water to fill %s.\r\n", ch, obj2 );
    obj->value[1] = obj->value[0];
    }
  else {
    send( ch, "You find some water to put in %s.\r\n", obj2 );
    send_seen( ch, "%s finds some water to put in %s.\r\n", ch, obj2 );
  }

  remove_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );
  obj->To( where );
  obj2->Extract( );

  ch->improve_skill( SKILL_DOWSE );

  int delay = ch->species != NULL ? 6 : 20-10*ch->get_skill( SKILL_DOWSE )/MAX_SKILL_LEVEL;
  set_delay(ch, delay);
  ch->move -= moves;
  return;
}

void do_draw( char_data* ch, char* argument )
{
  char_data* victim;
  
  if( null_caster( ch, SKILL_DRAW_POISON ) )
    return;

  if( ch->species != NULL || !ch->get_skill( SKILL_DRAW_POISON ) ) {
    send( ch, "That would be higly dangerous with your skill in that.\r\n" );
    return;
  }

  if( *argument == '\0' )
    victim = ch;
  else if( ( victim = one_character( ch, argument, "draw poison from", ch->array ) ) == NULL )
    return;
  
  if( ch->fighting != NULL ) {
    send( ch, "You can't remove poison while your fighting.\r\n" );
    return;
  }
  
  if( victim->fighting != NULL ) {
    send( ch, "You can't remove poison while %s is fighting.\r\n", victim );
    return;
  }

  if( !is_set( victim->affected_by, AFF_POISON ) ) {
    if( ch == victim )
      send( ch, "You are not poisoned right now.\r\n" );
    else
      send( ch, "%s is not poisoned right now.\r\n", victim );
      return;
  }

  if( victim->hit <= 10 ) {
    send( ch, "%s would probably die if you did that right now.\r\n", ch == victim ? "You" : victim->Seen_Name( ch ) );
    return;
  }

  int damage = number_range( 0, 10 )-5*ch->get_skill( SKILL_DRAW_POISON )/MAX_SKILL_LEVEL;

  if( damage > 0 ) 
    inflict( victim, NULL, damage, "sucking poison from a wound" );

  if( victim == ch ) {
    send( ch, "You make an incision in the poisoned wound and carefully draw the poison from it.\r\n" );
    send( *ch->array, "%s makes an incision in %s poisoned wound and carefully draws the poison from it.\r\n", ch, ch->His_Her( ) );
  }
  else {
    send( ch, "You make an incision close to %s's poisoned wound, and carefully draw the poison from it.\r\n", victim );
    send( victim, "%s makes an incision near your poisoned wound, and carefully draws the poison from it.\r\n", ch );
    send_seen( ch, "%s cuts %s and carefully draws the poison from %s wound.\r\n", ch, victim, victim->His_Her( ) );
  }

  strip_affect( victim, AFF_POISON );

  if( is_set( victim->affected_by, AFF_POISON ) ) {
    if( ch != victim ) 
      send( ch, "%s is still poisoned!?\r\n", victim );
    send( victim, "You are still poisoned!?\r\n" );
    }

  ch->improve_skill( SKILL_DRAW_POISON );

  int delay = ch->species != NULL ? 6 : 20-5*ch->get_skill( SKILL_DRAW_POISON )/MAX_SKILL_LEVEL;
  set_delay(ch, delay);

  return;
}

void do_spot( char_data* ch, char * )
{
  if( null_caster( ch, SKILL_SPOT_HIDDEN ) )
    return;

  int level = ch->get_skill( SKILL_SPOT_HIDDEN );

  if( level == UNLEARNT ) {
    send( ch, "You know nothing of the shadows.\r\n" );
    return;
  }

  if( !ch->Can_See( ) ) {
    send( ch, "How do you expect to do that right now?\r\n" );
    return;
  }

  if( is_set( ch->affected_by, AFF_SPOT_HIDDEN ) ) {
    send( ch, "You are already trying to spot unseen things.\r\n" );
    return;
  }

  spell_affect( ch, ch, level, (level+1)*5, SKILL_SPOT_HIDDEN, AFF_SPOT_HIDDEN );

  ch->improve_skill( SKILL_SPOT_HIDDEN );

  int delay = ch->species != NULL ? 6 : 20-5*ch->get_skill( SKILL_SPOT_HIDDEN )/MAX_SKILL_LEVEL;
  set_delay(ch, delay);

  return;
}  

void do_camp( char_data* ch, char* argument )
{
  obj_data*     obj = ch->in_room ? find_vnum( ch->in_room->contents, OBJ_CAMPFIRE ) : NULL;
  oprog_data* oprog;
  int         move = move_cost( ch, SKILL_CAMP_SITE );

  if( ranger_skill( ch, SKILL_CAMP_SITE, move ) )
    return;

  if( terrain_table[ ch->in_room->sector_type ].underwater ||
    terrain_table[ ch->in_room->sector_type ].water_surface ||
    terrain_table[ ch->in_room->sector_type ].shallow ) {
    send( ch, "How do you expect to make a camp here?\r\n" );
    return;
  }

  if( exact_match( argument, "break" ) ) {
    if( obj != NULL ) {
      send( ch, "You begin breaking down a camp.\r\n" );
      send_seen( ch, "%s begins breaking down a camp.\r\n", ch );
      obj->Extract( 1 );
      return;
    }
    else {
      send( ch, " There is no camp to break down.\r\n" );
      return;
    }
  }
  if( exact_match( argument, "tend" ) ) {
    if( obj == NULL ) {
      send( ch, "There is no camp fire to tend.\r\n" );
      return;
    }
    else 
      for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) 
        if( oprog->trigger == OPROG_TRIGGER_TIMER ) 
          break;

    if( oprog == NULL ) {
      bug( "Object #%i needs a timer program for the fire.", OBJ_CAMPFIRE );
      obj->Extract( );
      return;
    }
    if( obj->timer < 5*ch->get_skill( SKILL_CAMP_SITE )/2+1 ) {
      send( ch, "You begin tending to %s, stoking it for more comfort.\r\n", obj );
      send_seen( ch,  "%s begins tending to %s stoking it for more comfort.\r\n", ch, obj );

      ch->move -= move/5;

      obj->Set_Integer( CAMP_SITE, -ch->pcdata->pfile->ident );
      obj->Set_Integer( CAMP_SITE_REGEN, 50*ch->get_skill( SKILL_CAMP_SITE )/10 );

      obj->timer = 5*number_range( 0, ch->get_skill( SKILL_CAMP_SITE )/2 )+1;

      ch->improve_skill( SKILL_CAMP_SITE );
      return;
    }
    else {
      send( ch, "%s is already providing more comfort than you can manage.\r\n", obj );
      return;
    }
  }

  if( obj != NULL ) {
    send( ch, "There is already a camp site here.  You may try to tend or break it.\r\n" );
    return;
  }

  if( ( obj = create( get_obj_index( OBJ_CAMPFIRE ) ) ) == NULL ) {
    bug( "Camp Fire does not exist, it should be obj #%i.", OBJ_CAMPFIRE );
    return;
  }

  for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next )
    if( oprog->trigger == OPROG_TRIGGER_TIMER )
      break;
  
  if( oprog == NULL ) {
    bug( "The camp fire object #%i needs a timer program.", OBJ_CAMPFIRE );
    obj->Extract( );
    return;
  }

  send( ch, "You begin setting up a comfortable camp.\r\n" );
  send_seen( ch, "%s begins setting up a camp for your comfort.\r\n", ch );

  obj->timer = 5*number_range( 0, ch->get_skill( SKILL_CAMP_SITE ) )/2+1;

  obj->Set_Integer( CAMP_SITE, -ch->pcdata->pfile->ident );
  obj->Set_Integer( CAMP_SITE_REGEN, 50*ch->get_skill( SKILL_CAMP_SITE )/10 );

  obj->To( ch->in_room );
  
  ch->improve_skill( SKILL_CAMP_SITE );

  ch->move -= move;

  int delay = ch->species != NULL ? 20 : 30-ch->get_skill( SKILL_CAMP_SITE );
  set_delay(ch, delay);

  return;
}

void do_ensnare( char_data* ch, char* argument )
{
  int move          = move_cost( ch, SKILL_ENSNARE );
  char_data* victim;
  int level         = ch->get_skill( SKILL_ENSNARE );

  if( null_caster( ch, SKILL_ENSNARE ) )
    return;

  if( level == UNLEARNT ) {
    send( ch, "You know nothing of ensnaring things.\r\n" );
    return;
    }

  if( *argument == '\0' ) 
    victim = ch->fighting;
  else if( ( victim = one_character( ch, argument, "ensnare", ch->array ) ) == NULL )
    return;

  if( ch->species != NULL || level == UNLEARNT ) {
    send( ch, "You have no clue how to set a snare.\r\n" );
    return;
    }

  if( victim == NULL ) {
    send( ch, "You aren't fighting anyone.\r\n" );
    return;
    }

  if( victim->fighting == NULL ) {
    send( ch, "Why would you want to ensnare %s, they aren't fighting anyone.\r\n", victim );
    return;
    }

  if( ch->move < move ) {
    send( ch, "You can't move fast enough to get around %s.\r\n", victim );
    return;
    }

  ch->move -= move;

  physical_affect( ch, victim, SKILL_ENSNARE, AFF_ENSNARE );
  ch->improve_skill( SKILL_ENSNARE );

  disrupt_spell( victim );

  set_delay( ch, 30-10*level/MAX_SKILL_LEVEL );
  return;
}

bool spell_tame( char_data* ch, char_data* victim, void*, int level, int )
{
  mprog_data* mprog;

  if( null_caster( ch, SPELL_TAME ) )
    return TRUE;

  victim->selected = 1;

  if( is_set( &ch->in_room->room_flags, RFLAG_SAFE ) ) {
    send( ch, "You may not cast that spell in this room.\r\n" );
    return TRUE;
  }

  if( is_aggressive( victim, ch ) && !is_set( victim->affected_by, AFF_CALM )  ) {
    send( ch, "%s is not calm enough to tame right now.\r\n", victim );
    send( *ch->array, "%s is not calm enough for %s to tame.\r\n", victim, ch );
    return TRUE;
  }
    
  if( is_set( &victim->status, STAT_PET ) ) {
    send( ch, "%s is already tame.\r\n", victim );
    return TRUE;
  }

  if( victim->species == NULL || victim->leader != NULL || !is_apprentice(ch)
    && ( !is_set( victim->species->act_flags, ACT_CAN_TAME )
    || makes_save( victim, ch, RES_MIND, SPELL_TAME, level )
    || victim->shdata->level > ch->shdata->level ) )
  {
    send( ch, "%s ignores you.\r\n", victim );
    send( *ch->array, "%s ignores %s.\r\n", victim, ch );
    return TRUE;
  }

  if( victim->shdata->level > ch->shdata->level-pet_levels( ch ) ) {
    send( ch, "You fail as you are unable to control more animals.\r\n" );
    return TRUE;
  }

  if( is_set( victim->species->act_flags, ACT_MOUNT ) && has_mount( ch ) )
    return TRUE;

  if( is_set( victim->species->affected_by, AFF_SANCTUARY ) ) {
    send( ch, "%s ignores you.\r\n", victim );
    return TRUE;
  }

  for( mprog = victim->species->mprog; mprog != NULL; mprog = mprog->next )
    if( mprog->trigger == MPROG_TRIGGER_TAME ) {
      var_ch  = ch;
      var_mob = victim;
      var_room = ch->in_room;
      if( !execute( mprog ) )
      return FALSE;
    }


  if( ch->leader == victim )
    stop_follower( ch );

  victim->hit = current_condition( victim ); 
  victim->base_hit = victim->original_hit;

  update_maxes( victim );

  set_bit( &victim->status, STAT_PET );
  set_bit( &victim->status, STAT_TAMED );
  remove_bit( &victim->status, STAT_AGGR_ALL );
  remove_bit( &victim->status, STAT_AGGR_GOOD );
  remove_bit( &victim->status, STAT_AGGR_EVIL );
  remove_bit( &victim->status, STAT_AGGR_NEUT );

  unregister_reset( victim );

  if( *victim->species->tame_msg != '\0' )
    act( ch, victim->species->tame_msg, ch, victim );
  if( *victim->species->rtame_msg != '\0' )
    act_notchar( victim->species->rtame_msg, ch, victim );

  add_follower( victim, ch );

  return TRUE;
}


bool spell_barkskin( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( is_set( victim->affected_by, AFF_IRONSKIN ) ) {
    send( victim, "You can not be affected by the spell barkskin skin right now.\r\n" );
    return FALSE;
  }

  spell_affect( ch, victim, level, duration, SPELL_BARKSKIN, AFF_BARKSKIN );

  return TRUE;
}


bool spell_thorn_shield( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( !consenting( victim, ch, "thorn shield" ) )
    return TRUE;

  if( affected_shock( victim ) ) {
    send( victim, "The roaring electricity prevents the acid from surrounding you.\r\n" );
    send_seen( victim, "The roaring electricity protecting %s stops the acid.\r\n", victim );
    return TRUE;
  }

  spell_affect( ch, victim, level, duration, SPELL_THORN_SHIELD, AFF_THORN_SHIELD );

  return TRUE;
}

bool spell_companions_strength( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( is_set( ch->affected_by, AFF_COMPANIONS_STRENGTH ) ) {
    send( ch, "You can only use one companions abilities at a time." );
    return FALSE;
  }

  if( victim->leader != ch || !is_pet( victim ) ) {
    send( ch, "%s is not a pet of yours.\r\n", victim );
    return FALSE;
  }

  if( victim->species->compan_str < 0 || victim->species->compan_str > 26
    || victim->species->compan_amt < 0 || victim->species->compan_amt > 1000 ) {
    send( ch, "%s has an invalid quantity, please inform an immortal.\r\n" );
    bug( "Companions Strength - %s has an invalid quantity.", victim );
    bug( "Please set compan_str to 26 to ensure the mud does not crash." );
    return FALSE;
  }
  
  if( affect_loc[ victim->species->compan_str ] == APPLY_NONE || affect_loc[ victim->species->compan_str ] == -1 || victim->species->compan_amt == 0 ) {
    send( ch, "%s has no strength to share with you.\r\n", victim );
    return FALSE;
  }

  if( affect_loc[ victim->species->compan_str ] == -1 ) {
    send( ch, "%s has an invalid stat, please inform an immortal.\r\n" );
    bug( "Companion Strength - %s is using an invalid stat, please change this.\r\n" );
    return FALSE;
  }

  spell_affect( ch, victim, level, duration, SPELL_COMPANIONS_STRENGTH, AFF_COMPANIONS_STRENGTH );

  return TRUE;
}

bool spell_briartangle( char_data* ch, char_data* victim, void*, int level, int duration )
{
  affect_data* paf = NULL;

  if( null_caster( ch, SPELL_BRIARTANGLE ) )
    return TRUE;

  if( !can_kill( ch, victim ) )
    return TRUE;

  if( is_set( victim->affected_by, AFF_BRIARTANGLE ) ) {
    paf = find_affect( victim, AFF_BRIARTANGLE );
    if( paf != NULL ) {
      bool failed;
      paf->mmodifier[0] += evaluate( aff_char_table[AFF_BRIARTANGLE].mmodifier[0], failed );;   
      paf->duration = max( level/2, paf->duration+1 ); 
    }
    send( "The briars around you thicken.\r\n", victim );
    send( *victim->array, "The briars trapping %s thicken.\r\n", victim );
    record_damage( victim, ch, 20 );
    return TRUE;
  } 
  else {
    send( victim, "Some prickly briars begin to form around you.\r\n" );
    send( *victim->array, "Some prickly briars begin to form around %s.\r\n", victim );

    if( !is_set( victim->affected_by, AFF_BRIARTANGLE ) && 
      makes_save( victim, ch, RES_DEXTERITY, SPELL_BRIARTANGLE, level ) ) { 
      send( victim, "Luckily you avoid becoming entangled.\r\n" );
      send( *victim->array, "%s skillfully avoids the briars.\r\n", victim );
      return TRUE;
    }

    if( is_set( victim->affected_by, AFF_FIRE_SHIELD ) ) {
      send( victim, "As the briars make contact with your fire shield, they burst into flames.\r\n" );
      damage_fire( victim, ch, 5 * number_range(level/2, level), "*The incinerating briars" );
      send( *victim->array, "%s winces in pain as %s fire shield burns the briars into nothingness.\r\n", victim, victim->His_Her() );
      return TRUE;
    }
  }

  damage_magic( victim, ch, spell_damage( SPELL_BRIARTANGLE, level ), "*the entangling briars" );
  spell_affect( ch, victim, level, duration, SPELL_BRIARTANGLE, AFF_BRIARTANGLE );

  return TRUE;
}

bool spell_ironskin( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( is_set( victim->affected_by, AFF_BARKSKIN ) ) {
    send( victim, "You can not be affected by the spell iron skin right now.\r\n" );
    return FALSE;
  }

  spell_affect( ch, victim, level, duration, SPELL_IRONSKIN, AFF_IRONSKIN );

  return TRUE;
}

/*
 *   HEALING SPELLS
 */


bool spell_balm( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_BALM, level ) );

  return TRUE;
}


bool spell_surcease( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_SURCEASE, level ) );

  return TRUE;
}


bool spell_poultice( char_data* ch, char_data* victim, void*, int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_POULTICE, level ) );

  return TRUE;
}

/*
 *    PET ENHANCEMENT SPELLS
 */

bool spell_natures_blessing( char_data* ch, char_data* victim, void*, int level, int duration )
{
  bool found = false;

  if( victim == NULL || ch == NULL )
    return TRUE;

  for( int i = 137; i < AFF_NATURES_CALLING; i++ ) {
    if( is_set( victim->affected_by, i ) )
      found = true;
    }

  if( found ) {
    send( ch, "%s may not have multiples of this spell casted on %s.\r\n", victim, victim->Him_Her( ) );
    return FALSE;
    }

  if( victim->leader != ch ) {
    send( ch, "%s is not following you and can not be blessed.\r\n", victim );
    return FALSE;
    }

  if( !is_set( &victim->status, STAT_PET ) ) {
    send( ch, "%s is not a pet of yours and can not be blessed.\r\n", victim );
    return FALSE;
    }

  spell_affect( ch, victim, level, duration, SPELL_NATURES_BLESSING, AFF_NATURES_BLESSING );
  return TRUE;
}

bool spell_natures_imbued( char_data* ch, char_data* victim, void*, int level, int duration )
{
  bool found = false;

  if( victim == NULL || ch == NULL )
    return TRUE;

  for( int i = 137; i < AFF_NATURES_CALLING; i++ ) {
    if( is_set( victim->affected_by, i ) )
      found = true;
    }

  if( found ) {
    send( ch, "%s may not have multiples of this spell casted on %s.\r\n", victim, victim->Him_Her( ) );
    return FALSE;
    }

  if( victim->leader != ch ) {
    send( ch, "%s is not following you and can not be blessed.\r\n", victim );
    return FALSE;
    }

  if( !is_set( &victim->status, STAT_PET ) ) {
    send( ch, "%s is not a pet of yours and can not be blessed.\r\n", victim );
    return FALSE;
    }

  spell_affect( ch, victim, level, duration, SPELL_NATURES_IMBUED, AFF_NATURES_IMBUED );
  return TRUE;
}

bool spell_natures_infuse( char_data* ch, char_data* victim, void*, int level, int duration )
{
  bool found = false;

  if( victim == NULL || ch == NULL )
    return TRUE;

  for( int i = 137; i < AFF_NATURES_CALLING; i++ ) {
    if( is_set( victim->affected_by, i ) )
      found = true;
    }

  if( found ) {
    send( ch, "%s may not have multiples of this spell casted on %s.\r\n", victim, victim->Him_Her( ) );
    return FALSE;
    }

  if( victim->leader != ch ) {
    send( ch, "%s is not following you and can not be blessed.\r\n", victim );
    return FALSE;
    }

  if( !is_set( &victim->status, STAT_PET ) ) {
    send( ch, "%s is not a pet of yours and can not be blessed.\r\n", victim );
    return FALSE;
    }

  spell_affect( ch, victim, level, duration, SPELL_NATURES_INFUSE, AFF_NATURES_INFUSE );
  return TRUE;
}

bool spell_natures_spirit( char_data* ch, char_data* victim, void*, int level, int duration )
{
  bool found = false;

  if( victim == NULL || ch == NULL )
    return TRUE;

  for( int i = 137; i < AFF_NATURES_CALLING; i++ ) {
    if( is_set( victim->affected_by, i ) )
      found = true;
    }

  if( found ) {
    send( ch, "%s may not have multiples of this spell casted on %s.\r\n", victim, victim->Him_Her( ) );
    return FALSE;
    }

  if( victim->leader != ch ) {
    send( ch, "%s is not following you and can not be blessed.\r\n", victim );
    return FALSE;
    }

  if( !is_set( &victim->status, STAT_PET ) ) {
    send( ch, "%s is not a pet of yours and can not be blessed.\r\n", victim );
    return FALSE;
    }

  spell_affect( ch, victim, level, duration, SPELL_NATURES_SPIRIT, AFF_NATURES_SPIRIT );
  return TRUE;
}

bool spell_natures_calling( char_data* ch, char_data* victim, void*, int level, int duration )
{
  bool found = false;

  if( victim == NULL || ch == NULL )
    return TRUE;

  for( int i = 137; i < AFF_NATURES_CALLING; i++ ) {
    if( is_set( victim->affected_by, i ) )
      found = true;
    }

  if( found ) {
    send( ch, "%s may not have multiples of this spell casted on %s.\r\n", victim, victim->Him_Her( ) );
    return FALSE;
    }

  if( victim->leader != ch ) {
    send( ch, "%s is not following you and can not be blessed.\r\n", victim );
    return FALSE;
    }

  if( !is_set( &victim->status, STAT_PET ) ) {
    send( ch, "%s is not a pet of yours and can not be blessed.\r\n", victim );
    return FALSE;
    }

  spell_affect( ch, victim, level, duration, SPELL_NATURES_CALLING, AFF_NATURES_CALLING );
  return TRUE;
}

//Calling of the true beast functions


bool can_call_beast( char_data* ch, species_data* beast, char_data* victim )
{
  if( beast == NULL ) {   
    send( ch, "You fail to call the true beast forth as it seems it is unable to\r\ntouch its true self.\r\n" );
    return FALSE;
  }
  
  if( beast->shdata->level > ch->shdata->level ) {
    send( ch, "You are not powerful enough to control the spirit of that creature.\r\n" );
    return FALSE;
  }

  if( beast->shdata->level + pet_levels( ch ) > ch->shdata->level ) {
    send( ch, "Controlling that creature in conjunction with the pets you have already\r\ntamed is beyond your abilities.\r\n" );
    return FALSE;
  }

  if( victim->leader != ch ) {
    send( ch, "%s is not following you so this spell fails.\r\n", victim );
    return FALSE;
    }

  if( victim->leader != ch || !is_pet( victim ) ) {
    send( ch, "%s is not a pet of yours.\r\n", victim );
    return FALSE;
  }

  return TRUE;
}

bool spell_calling_beast( char_data* ch, char_data* victim, void*, int level, int duration )
{
  species_data*     beast;
  char_data*     creature;

  if( null_caster( ch, SPELL_CALLING_BEAST ) )
    return TRUE;

  if( is_good( ch ) )
    beast = get_species( victim->species->celestial );
  else if( is_neutral( ch ) )
    beast = get_species( victim->species->dire );
  else
    beast = get_species( victim->species->fiendish );

  if( !can_call_beast( ch, beast, victim ) )
    return TRUE;

  creature = create_mobile( beast );
  
  set_bit( &creature->status, STAT_PET );
  set_bit( &creature->status, STAT_TAMED );
  remove_bit( &creature->status, STAT_AGGR_ALL );
  remove_bit( &creature->status, STAT_AGGR_GOOD );
  remove_bit( &creature->status, STAT_AGGR_EVIL );

  creature->To( ch->array );
  add_follower( creature, ch, FALSE );

  send( ch, "%s slowly vanishes as %s supplants %s.\r\n", victim, creature, victim->Him_Her( ) );
  fsend_seen( victim, "%s slowly vanishes and %s starts to follow %s.", victim, creature, ch );

  victim->Extract( );
  return TRUE;
}
