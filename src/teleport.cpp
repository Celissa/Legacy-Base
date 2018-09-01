#include "system.h"


/*      
 *   ASTRAL GATE
 */

void make_gate( room_data* from, room_data* to )
{
  static int vnum = -1;
  obj_clss_data* obj;

  if( vnum == -1 || get_obj_index( vnum )->item_type != ITEM_GATE ) {
    vnum = -1;

    for( int i = 0; i < MAX_OBJ_INDEX; i++ ) {
      if( ( obj = obj_index_list[i] ) != NULL && obj->item_type == ITEM_GATE ) {
        vnum = i;
        break;
      }
    }

    if( vnum == -1 ) {
      roach( "No astral gate object defined." );
      send( from->contents, "The astral gate could not be summoned.  Please contact an immortal.\r\n" );
      return;
    }
  }

  obj_data* gate = create( get_obj_index( vnum ) );
  gate->value[1] = to->vnum;
  gate->To( from );

  fsend( from->contents, "\r\n%s slowly rises from the ground.\r\n", gate );
  return;
}

void make_tree( room_data* from, room_data* to )
{
  static int vnum = -1;
  obj_clss_data* obj;

  if( vnum == -1 || get_obj_index( vnum )->item_type != ITEM_ANCIENT_OAK ) {
    vnum = -1;

    for( int i = 0; i < MAX_OBJ_INDEX; i++ ) {
      if( ( obj = obj_index_list[i] ) != NULL && obj->item_type == ITEM_ANCIENT_OAK ) {
        vnum = i;
        break;
      }
    }

    if( vnum == -1 ) {
      roach( "No ancient oak object defined." );
      send( from->contents, "The ancient oak could not be awakened (at all).  Please contact an immortal.\r\n" );
      return;
    }
  }

  obj_data* gate = create( get_obj_index( vnum ) );
  gate->value[1] = to->vnum;
  gate->To( from );

  fsend( from->contents, "\r\n%s slowly awakens, acceding to your request.\r\n", gate );
  return;
}




bool spell_astral_gate( char_data* ch, char_data*, void* vo, int, int )
{
  room_data*  room  = (room_data*) vo;

  if( null_caster( ch, SPELL_ASTRAL_GATE ) )
    return TRUE;

  if( room == NULL ) {
    bug( "Astral_Gate: Null room." );
    return TRUE;
  }

  if( is_set( &ch->in_room->room_flags, RFLAG_NO_GATE ) ) {
    send( "\r\n", ch );
    send( "You fail to create an astral gate.", ch );
    send( "\r\n", ch );
    return TRUE;
  }
 
  make_gate( room, ch->in_room );
  make_gate( ch->in_room, room );

  return TRUE;
}

bool spell_tree_gate( char_data* ch, char_data*, void* vo, int, int )
{
  room_data* room = (room_data*) vo;

  if( null_caster( ch, SPELL_TREE_GATE ) )
    return TRUE;

  if( is_set( &ch->in_room->room_flags, RFLAG_NO_GATE ) ) {
    send( "\r\nYou fail to awaken the great oak tree.\r\n", ch );
    return TRUE;
  }

  make_tree( room, ch->in_room );
  make_tree( ch->in_room, room );

  return TRUE;
}

bool spell_universal_gate( char_data* ch, char_data*, void* vo, int, int )
{
  room_data*  room  = (room_data*) vo;

  if( null_caster( ch, SPELL_ASTRAL_GATE ) )
    return TRUE;

  if( room == NULL ) {
    bug( "Astral_Gate: Null room." );
    return TRUE;
  }

  if( is_set( &ch->in_room->room_flags, RFLAG_NO_GATE ) ) {
    send( "\r\nYou fail to create an astral gate.\r\n", ch );
    return TRUE;
  }
 
  make_gate( room, ch->in_room );
  make_gate( ch->in_room, room );

  return TRUE;
}

/*      
 *   BLINK
 */


bool spell_blink( char_data* ch, char_data* victim, void*, int level, int )
{
  exit_data*  exit;
  room_data*  room;

  if( ch == NULL )
    ch = victim;

  if( !ch || !ch->array || ( room = Room( ch->array->where ) ) == NULL ) {
    send( ch, "Blink only works in a room.\r\n" );
    return FALSE;
  } 

  if( is_set( &room->room_flags, RFLAG_NO_GATE ) || is_set( &room->room_flags, RFLAG_NO_SUMMON_OUT ) ) {
    send( ch, "As you cast blink, you feel the energy drain from you and nothing happens." );
    return FALSE;
  }

  for( int i = 0; i < level; i++ ) {
    if( ( exit = random_open_exit( room ) ) == NULL )
      break;
    if( ch->array->where != exit->to_room && exit->to_room->area->status == AREA_OPEN && ( !is_set( &exit->to_room->room_flags, RFLAG_NO_GATE )
      || !is_set( &exit->to_room->room_flags, RFLAG_NO_SUMMON_IN ) ) )
      room = exit->to_room;
  }

  if( room == ch->array->where ) {
    send( ch, "You are trapped!\r\n" );
    return TRUE;
  }

  ch->selected = 1;
  ch->shown = 1;

  set_delay( ch, 32 );
  clear( ch->cmd_queue );
  clear_queue( ch );

  send( *ch->array, "%s vanishes in the blink of an eye!\r\n", ch );
  send( ch, "You disappear and suddenly find yourself elsewhere.\r\n\r\n" );

  ch->From( );
  ch->To( room ); 

  send( *ch->array, "%s suddenly appears in a flash of white light!\r\n", ch );
  show_room( ch, room, FALSE, FALSE );

  return TRUE;
}


/*
 *   PASSDOOR
 */


bool spell_pass_door( char_data* ch, char_data* victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_PASS_DOOR, AFF_PASS_DOOR );

  return TRUE;
}


/*
 *   RECALL
 */


bool spell_recall( char_data* ch, char_data*, void* vo, int, int )
{
  char_data* victim = ch; // only allow recall on yourself
  room_data *room = (room_data*) vo;
  thing_array list;
  char_data* rch;

//  if( !consenting( victim, ch, "recalling" ) ) 
//    return TRUE;

  if( room == NULL ) {
    if( victim->species != NULL ) {
      if( !is_set( &victim->status, STAT_PET ) || victim->leader == NULL ) {
        send( ch, "Nothing happens.\r\n" );
        return TRUE;
      }
      room = get_temple( victim->leader, victim->leader->in_room );
    } else
      room = get_temple( victim, victim->in_room );
  }

  if( room->area->continent != ch->in_room->area->continent ) {
    send( ch, "You are unable to cross sea or ocean like that.\r\n" );
    return TRUE;
  }

  int exp = 0;
  if( ch->fighting != NULL && ch->pcdata != NULL && ch->species == NULL ) {
    exp = ch->shdata->level * ch->hit * 5;
    if( ch->shdata->level == 1 )
      exp = min( ch->exp, exp );
    if( exp > 0 ) 
      add_exp( ch, -exp, "You lose %d exp for recalling from battle.\r\n" );

    clear( ch->cmd_queue );
    clear_queue( ch );
  }

  set_delay( ch, 32 );

  list += ch;

  for( int i = 0; i < victim->in_room->contents; i++ )
    if( ( rch = character( victim->in_room->contents[i] ) ) != NULL 
  && rch->leader == victim && rch->species != NULL )
      list += rch;

//  send( *victim->array, "%s disappears in a flash of light.\r\n", victim );
  send_seen( victim, "%s disappear%s in a flash of light.\r\n", &list, list > 1 ? "" : "s" );
/*
  victim->From( );
  victim->To( room );
*/
  for( int i = 0; i < list; i++ ) {
    list[i]->From( );
  list[i]->To( room );
  }

  
  send( "\r\n", victim );
  do_look( victim, "" );

  send_seen( victim, "%s appear%s in a flash of light.\r\n", &list, list > 1 ? "" : "s" );

  return TRUE;
}


/*
 *   SUMMON
 */


bool spell_summon( char_data* ch, char_data* victim, void*, int, int )
{
  thing_array   list;
  char_data*     rch;

  if( null_caster( ch, SPELL_SUMMON ) )
    return TRUE;
 
  // sanity check - victim has to be in a room
  if( victim->in_room == NULL || victim->in_room == ch->in_room ) {
    send( ch, "Nothing happens.\r\n" );
    return TRUE;
  }

  if( victim->shdata->level >= LEVEL_BUILDER
    || ( victim->species != NULL && ( victim->leader != ch || !is_set( &victim->status, STAT_FAMILIAR ) ) )
    || is_set( &victim->in_room->room_flags, RFLAG_NO_RECALL )
    || is_set( &victim->in_room->room_flags, RFLAG_NO_SUMMON_OUT )
    || is_set( &ch->in_room->room_flags, RFLAG_NO_SUMMON_IN )
    || is_set( &victim->in_room->room_flags, RFLAG_NO_MAGIC ) ) {
    send( ch, "You fail to summon them.\r\n" );
    send( victim, "The world comes back to focus as the summoning fails.\r\n" );
    return TRUE;
  }

  if( victim->pcdata && is_set( victim->pcdata->pfile->flags, PLR_NO_SUMMON ) ) {
    send( ch, "%s has no.summon set.\r\n", victim );
    return TRUE;
  }

  if( !consenting( victim, ch, "summoning" ) ) 
    return TRUE;

  /* MAKE LIST */

  list += victim;

  for( int i = 0; i < victim->in_room->contents; i++ ) 
    if( ( rch = character( victim->in_room->contents[i] ) ) != NULL
      && rch->leader == victim && rch->species != NULL ) 
      list += rch;

  /* TRANSFER CHARACTERS */

  set_delay( victim, 32 );
  clear( victim->cmd_queue );
  clear_queue( victim );

  send_seen( victim, "%s slowly fade%s out of existence.\r\n", &list, list > 1 ? "" : "s" );

  for( int i = 0; i < list; i++ ) {
    list[i]->From( );
    list[i]->To( ch->in_room );
  }

  send( victim, "\r\n** You feel yourself pulled to another location. **\r\n\r\n" );

  show_room( victim, ch->in_room, FALSE, FALSE );
  send_seen( victim, "%s slowly fade%s into existence.\r\n", &list, list > 1 ? "" : "s" );

  return TRUE;
}


bool spell_greater_summon( char_data* ch, char_data* victim, void*, int, int )
{
  thing_array   list;
  char_data*     rch;

  if( null_caster( ch, SPELL_SUMMON ) )
    return TRUE;
 
  // sanity check - victim has to be in a room
  if( victim->in_room == NULL || victim->in_room == ch->in_room ) {
    send( ch, "Nothing happens.\r\n" );
    return TRUE;
  }

  if( victim->shdata->level >= LEVEL_BUILDER
    || ( victim->species != NULL && ( victim->leader != ch || !is_set( &victim->status, STAT_PET ) ) )
    || is_set( &victim->in_room->room_flags, RFLAG_NO_RECALL )
    || is_set( &victim->in_room->room_flags, RFLAG_NO_SUMMON_OUT )
    || is_set( &ch->in_room->room_flags, RFLAG_NO_SUMMON_IN )
    || is_set( &victim->in_room->room_flags, RFLAG_NO_MAGIC ) ) {
    send( ch, "You fail to summon them.\r\n" );
    send( victim, "The world comes back to focus as the summoning fails.\r\n" );
    return TRUE;
  }

  if( victim->pcdata && is_set( victim->pcdata->pfile->flags, PLR_NO_SUMMON ) ) {
    send( ch, "%s has no.summon set.\r\n", victim );
    return TRUE;
  }

  if( !consenting( victim, ch, "summoning" ) ) 
    return TRUE;

  /* MAKE LIST */

  list += victim;

  for( int i = 0; i < victim->in_room->contents; i++ ) 
    if( ( rch = character( victim->in_room->contents[i] ) ) != NULL
      && rch->leader == victim && rch->species != NULL ) 
      list += rch;

  /* TRANSFER CHARACTERS */

  set_delay( victim, 32 );
  clear( victim->cmd_queue );
  clear_queue( victim );

  send_seen( victim, "%s slowly fade%s out of existence.\r\n", &list, list > 1 ? "" : "s" );

  for( int i = 0; i < list; i++ ) {
    list[i]->From( );
    list[i]->To( ch->in_room );
  }

  send( victim, "\r\n** You feel yourself pulled to another location. **\r\n\r\n" );

  show_room( victim, ch->in_room, FALSE, FALSE );
  send_seen( victim, "%s slowly fade%s into existence.\r\n", &list, list > 1 ? "" : "s" );

  return TRUE;
}

/*
 *   TRANSFER
 */

/*
bool spell_transfer( char_data* ch, char_data*, void*, int, int )
{
  if( null_caster( ch, SPELL_TRANSFER ) )
    return TRUE;
 
  return TRUE;
}
*/








