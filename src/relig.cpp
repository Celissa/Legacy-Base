#include "system.h"

void do_pray( char_data* ch, char* argument )
{
  obj_data*         obj;
  char_data*     victim  = ch;
  player_data*       pc  = player( ch );
  int            prayer;
  int              need;

  if( is_mob( ch ) ) 
    return;

  if( ch->pcdata->religion == REL_NONE && ch->shdata->level > 8 ) {
    send( ch, "You are not the follower of any god and cannot expect aid.\r\n" );
    return;
  }

  if( *argument != '\0' && ( victim = one_character( ch, argument, "pray for", ch->array ) ) == NULL ) 
    return;

  if( is_set( &ch->in_room->room_flags, RFLAG_NO_PRAY ) ) {
    send( ch, "As you pray you feel a cold chill touch your soul, you doubt there will be\r\nany aid forthcoming here.\r\n" );
    return;
  }

  prayer = pc->prayer;

  if( victim != ch )
    need = 0;
  else if( victim->fighting == NULL )
    need = victim->max_hit/3;
  else
    need = victim->max_hit/4;

  if( prayer > 150 && victim->hit < need ) {
    prayer      -= 150;
    victim->hit  = victim->max_hit;
    update_pos( victim );
    update_max_move( victim );
    victim->move = victim->max_move;
    if( victim == ch ) {
      int religion = MAX_ENTRY_RELIGION > 1 ? number_range( 1, MAX_ENTRY_RELIGION - 1 ) : REL_NONE;
      send( ch, "Heeding your prayer, your are bathed in the divine energy of %s, curing your wounds.\r\n", ch->Deity( ch, religion ) );
      send_seen( ch, "Heeding %s prayer, %s is bathed in the divine energy of %s, curing %s wounds.\r\n", ch->His_Her( ), ch, ch->Deity( victim, religion ), ch->His_Her( ) );
    } else {
      int religion = MAX_ENTRY_RELIGION > 1 ? number_range( 1, MAX_ENTRY_RELIGION - 1 ) : REL_NONE;
      send( ch, "Heeding your prayer, %s is bathed in the divine energy of %s, curing %s wounds.\r\n", victim, ch->Deity( ch, religion ), victim->His_Her( ) );
      send( victim, "Heeding %s's prayer, you are bathed in the divine energy of %s, curing your wounds.\r\n", ch, ch->Deity( victim, religion ) );
      send_seen( victim, "Heeding's %s prayer, %s is bathed in the divine energy of %s, curing %s wounds.\r\n", ch, victim, ch->Deity( victim, religion ), victim->His_Her( ) );
    }
  } 

  if( is_set( victim->affected_by, AFF_BLIND ) && prayer > 200 ) {
    prayer -= 200;
    strip_affect( victim, AFF_BLIND );
  }

  if( is_set( victim->affected_by, AFF_POISON ) && prayer > 150 ) {
    prayer -= 150;
    strip_affect( victim, AFF_POISON );
  }

  if( victim == ch ) {
    if( ch->pcdata->condition[COND_FULL] < -10 && !is_set( ch->affected_by, AFF_STARVING_MADNESS ) && prayer > 30 ) {
      prayer -= 30;
      ch->pcdata->condition[COND_FULL] = 30;
      send( ch, "Your stomach feels full.\r\n" );
    }

    if( ch->pcdata->condition[COND_THIRST] < -10 && prayer > 30 ) {
      prayer -= 30;
      ch->pcdata->condition[COND_THIRST] = 30;
      send( ch, "You no longer feel thirsty.\r\n" );
    } 

    if( ch->in_room->is_dark( ) && prayer > 30
      && !is_set( ch->affected_by, AFF_INFRARED )
      && !is_set( ch->affected_by, AFF_BLIND ) ) {
      prayer -= 30;
      obj     = create( get_obj_index( OBJ_BALL_OF_LIGHT ) );
      obj->value[2] = 54;
      send( ch, "%s appears in your hand.\r\n", obj );
      send_seen( ch, "%s appears in %s's hand.\r\n", obj, ch );
      obj->To( ch );
      consolidate( obj );
    }

    if( ch->move < 10 && ch->max_move > 20 && prayer > 20 ) {
      prayer   -= 20;
      ch->move  = ch->max_move;
      send( ch, "You feel rejuvinated.\r\n" );
    }
  }

  if( prayer == pc->prayer ) {
    if( ch == victim )
      send( ch, "You pray to %s to no avail.\r\n", ch->Deity( ch ) );
    else
      fsend( ch, "You pray for the soul of %s to no avail.", victim );
  }

  pc->prayer = max( 0, prayer );
}


/*
 *   SACRIFICE 
 */


void do_sacrifice( char_data* ch, char* argument )
{
  char             arg  [ MAX_INPUT_LENGTH ];
  action_data*  action;
  obj_data*        obj;
  player_data*      pc;
  int                i = ch->pcdata == NULL ? 0 : ch->pcdata->religion;

  if( is_mob( ch ) ) 
    return;

  if( is_set( ch->affected_by, AFF_BLIND ) ) {
    send( ch, "How do you expect to do that while you can't see the altar?\r\n" );
    return;
    }

  if( !is_set( &ch->in_room->room_flags, RFLAG_ALTAR ) ) {
    send( ch, "Sacrifices will only be recognized at altars.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "The gods do not value empty sacrifices.\r\n" );
    return;
  }

  if( !two_argument( argument, "to", arg ) ) {
    send( ch, "Syntax: sacrifice <object> to <deity>.\r\n" );
    return;
  }
  
  for( i = 1; ; i++ ) {
    if( i == MAX_ENTRY_RELIGION ) {
      send( ch, "Unknown deity.\r\n" );
      return;
    }
    if( matches( argument, religion_table[i].name ) )
      break;
  }

  if( ( obj = one_object( ch, arg, "sacrifice", ch->array, &ch->contents ) ) == NULL )
    return;
 
  if( obj->array != ch->array ) {
    send( ch, "You must drop %s first to sacrifice it.\r\n", obj );
    return;
  }  

  if( is_set( obj->pIndexData->extra_flags, OFLAG_NOSACRIFICE ) ) {
    send( ch, "%s grows angry at your impudence.\r\n", religion_table[i].name );
    return;
  }

  if( !is_set( &religion_table[i].alignments, ch->shdata->alignment ) ) {
    send( ch, "You may not sacrifice to %s.\r\n", religion_table[i].name );
    return;
  }

  if( religion_table[i].classes != 0 && ch->pcdata != NULL && 
    !is_set( &religion_table[i].classes, ch->pcdata->clss ) ) {
    send( ch, "Your class is forbidden to follow %s by %s teachings.\r\n", religion_table[i].name, religion_table[i].sex == 0 ? "his" :religion_table[i].sex == 1 ? "his" : "her" );
    return;
  }

  if( !can_wear( obj, ITEM_TAKE ) ) {
    send( ch, "%s is immovable and this makes the required ritual impossible.\r\n", obj );
    return;
  } 

  if( !forbidden( obj, ch ) ) {
    send( ch, "You are forbidden from sacrificing %s.\r\n", obj );
    return;
  } 

  for( action = ch->in_room->action; action != NULL; action = action->next ) 
    if( action->trigger == TRIGGER_SACRIFICE && ( action->value == 0 || obj->pIndexData->vnum == action->value ) ) {
      var_ch = ch;
      var_obj = obj;
      var_room = ch->in_room;
      if( !execute( action ) )         // returns 'false' on an 'end'
        return;

      if( !obj || !obj->Is_Valid( ) )  // sanity check
        return;
    }

  // Don't let players sacrifice cheap items (unless corpses or magical)
  int cost = obj->Cost();
  if ( cost < ch->shdata->level && obj->pIndexData->item_type != ITEM_CORPSE && !is_set( obj->extra_flags, OFLAG_MAGIC ) ) {
    fsend( ch, "%s rejects %s as an unworthy sacrifice.\r\n", religion_table[i].name, obj );
    return;
  }

  ch->pcdata->religion = i;

  fsend( ch, "You sacrifice %s to %s.\r\n", obj, religion_table[i].name );
  fsend_seen( ch, "%s sacrifices %s to %s.\r\n", ch, obj, religion_table[i].name );

  if( ( pc = player( ch ) ) != NULL ) {
    pc->reputation.gold += cost;
    if( obj->pIndexData->item_type == ITEM_CORPSE )
      pc->reputation.blood++;
    if( is_set( obj->extra_flags, OFLAG_MAGIC ) )
      pc->reputation.magic += 1 + cost/1000;
  }

  obj->Extract( 1 );
}




















