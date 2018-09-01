#include "system.h"

#ifdef MYSTERIA_WINDOWS

// windows doesn't have gettimeofday, simulate
void gettimeofday(struct timeval *t, struct timezone *dummy)
{
  DWORD millisec = GetTickCount();

  t->tv_sec = (int) (millisec / 1000);
  t->tv_usec = (millisec % 1000) * 1000;
}

#endif

bool    obj_compiled   = FALSE;
bool    mob_compiled   = FALSE;
bool    room_compiled  = FALSE;

void    affect_update    ( void );
void    affect_update    ( char_data* );
void    char_update      ( void );
void    drunk_update     ( char_data* );
void    light_update     ( char_data* );
void    obj_update       ( void );
bool    disease_update    ( char_data*, int );
void    poison_update    ( char_data* );
void    room_update      ( void );
void    action_update    ( void );
void    random_passive   ( void );
void    random_dance     ( void );

/*
 *   MAIN UPDATE FUNCTION
 */


void update_handler( void )
{
  static int      pulse_clock;
  static int       pulse_area;
  static int     pulse_mobile;
  static int   pulse_violence;
  static int      pulse_point;
  static int       pulse_room;
  static int       pulse_save;
  static int      pulse_spell;
  struct timeval        start;  

  gettimeofday( &start, NULL );

  event_update( );
  delete_list( extracted );

  if( --pulse_save <= 0 ) {
    pulse_save = PULSE_SAVE;
    save_exp();
    save_misc();
  }

  if( --pulse_clock <= 0 ) {
    pulse_clock = PULSE_CLOCK;
    time_update( );
  }

  if( --pulse_area <= 0 ) {
    pulse_area = number_range( PULSE_AREA/2, 3*PULSE_AREA/2 );
    area_update( );
  }

  if( --pulse_mobile <= 0 ) {
    pulse_mobile = PULSE_MOBILE;
    auction_update( );
    regen_update( );
    action_update( );
    random_passive( );
  }

  if( --pulse_point <= 0 ) {
    pulse_point = number_range( PULSE_TICK/2, 3*PULSE_TICK/2 );
    char_update( );
    obj_update( );
  }

  if( --pulse_room <= 0 ) {
    pulse_room = number_range( PULSE_ROOM/2, 3*PULSE_ROOM/2 );
    room_update( );
    w3_who( );
  }

  if( --pulse_spell <= 0 ) {
    pulse_spell = PULSE_SPELL;
    affect_update();
  }

  if( --pulse_violence <= 0 ) {
    pulse_violence = PULSE_VIOLENCE;
    update_queue( );
    random_dance( );
  }

  pulse_time[ TIME_UPDATE ] = stop_clock( start );  
}


/*
 *   UPDATE CHARACTERS
 */


inline void update( char_data* ch )
{
  // maybe should be some kinda validity check here?

  if( !ch || !ch->Is_Valid( ) )
    return;

  if( ch->hit == ch->max_hit && ch->fighting == NULL ) {
    ch->damage       = 0;
    ch->rounds       = 1;
    ch->damage_taken = 0;
    ch->special      = 0;
  }

  light_update( ch );
  // affect_update( ch );

  if( !disease_update( ch, AFF_PLAGUE ) && !disease_update( ch, AFF_TOMB_ROT )
   && !disease_update( ch, AFF_RABIES ) && !disease_update( ch, AFF_BLIGHT ) )
    poison_update( ch );

  return;
}


inline void update( mob_data* mob )
{
  mprog_data* mprog;

  if( !mob || mob->position == POS_EXTRACTED || mob->in_room == NULL )
    return;

  for( mprog = mob->species->mprog; mprog != NULL; mprog = mprog->next ) {
    if( mprog->trigger == MPROG_TRIGGER_TIMER && --mob->timer <= 0 ) {
      var_mob  = mob;
      var_room = mob->in_room;
      execute( mprog );
      mob->timer = mprog->value;
      return;
    } 
    if( mprog->trigger == MPROG_TRIGGER_RANDOM && number_range( 0, 1000 ) <= mprog->value ) {
      var_mob    = mob;
      var_room   = mob->in_room;
      execute( mprog );
      return;
    }
  } 

//mob reset flags here
  update( (char_data*) mob );
}


inline void update( player_data* player )
{
  int idle;

  if( !player || !player->In_Game( ) )
    return;

  if( player->save_time+600 < time(0) ) {
    if( is_set( &player->pcdata->message, MSG_AUTOSAVE ) )
      send( player, "Autosaving...\r\n" );
    write( player );
    reference( player, player->contents, -1 );
  }

  idle = time(0) - player->timer;

  if( player->shdata->level < LEVEL_APPRENTICE ) {
    if( !player->link && ( !player->switched || !player->switched->link ) 
      && ( !player->shape_changed || !player->shape_changed->link ) && idle > 50 ) {
      if( player->was_in_room == NULL && player->in_room != NULL ) {
        if( player->switched != NULL )
          do_return( player->switched, "" );
        player->was_in_room = player->in_room;
        send( player, "You are pulled into nothingness.\r\n" );
        send_seen( player, "%s is pulled into nothingness.\r\n", player );
        write( player );
        player->From( );

        room_data* room = get_town( "#limbo", true );
        player->To( room ? room : get_room_index( ROOM_CONSTRUCT ) );
      
      } else if( idle > 20 * 60 ) {
        // 20 minutes of linkdeath, and you're a gonner
        forced_quit( player );
        return;
      }

    } else if( player->switched && idle > 10 * 60 ) {
      // 10 minutes of switched, and you're out
        do_return( player->switched, "" );
    } else if( player->link && idle > 15 * 60 ) {
      // 15 minutes of idle, and you're out
      send( player, "You have been idle too long.  Closing connection.\r\n" );
      send( player, "Thank you for visiting Legacy's Dawning.\r\n" );
      close_socket( player->link, true );
      return;
    }
  }

  if( player->was_in_room != NULL )
    return;
    
  if( player->gossip_pts < 1000 && number_range( 0, player->pcdata->trust >= LEVEL_AVATAR ? 3 : 5 ) == 0 )
    if( !is_set( player->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) )
    player->gossip_pts++;
  
  player->shdata->fame  = max( --player->shdata->fame, 0 );
  player->whistle       = max( --player->whistle, 0 );
  player->prayer        = min( ++player->prayer, 1000 );

  condition_update( player );

  dream_update( player );

  update( (char_data*) player );
}


void affect_update( void )
{
  struct timeval   start;
  
  gettimeofday( &start, NULL );

  for( int i = 0; i < mob_list; i++ )
    affect_update( mob_list[i] );
   
  for( int i = 0; i < player_list; i++ )
    affect_update( player_list[i] );

  pulse_time[ TIME_CHAR_LOOP ] = stop_clock( start );  

  return;
}


void char_update( void )
{   
  struct timeval   start;
  
  gettimeofday( &start, NULL );

  for( int i = 0; i < mob_list; i++ )
    update( mob_list[i] );
   
  for( int i = 0; i < player_list; i++ )
    update( player_list[i] );

  pulse_time[ TIME_CHAR_LOOP ] = stop_clock( start );  

  return;
}


/*
 *   CHARACTER UPDATE SUBROUTINES
 */


void affect_update( char_data* ch )
{
  affect_data*   aff;

  for( int i = ch->affected.size-1; i >= 0; i-- ) {
    aff = ch->affected[i];
    if( aff->leech == NULL ) {
      if( aff->duration-- == 1 && aff->type == AFF_FIRE_SHIELD && ch->position > POS_SLEEPING ) {
        send( ch, "The flames around you begin to fade.\r\n" );
      } else if( aff->type == AFF_DEATH ) {
        update_max_hit( ch );
        update_max_move( ch );
      } else if( aff->type == AFF_ACID_DEATH ) {
        damage_element( ch, NULL, spell_damage(  SPELL_WITHERING_BLAST, 0, 0 ), "*the remaining acid", ATT_ACID );
      }

      if( aff->duration <= 0 )
        remove_affect( ch, aff );
    }
  }

  return;
}

// Return the intensity of light _lost_ by this object this tick.
int light_obj_update( obj_data* obj, char_data* ch )
{
  if( obj == NULL || ch == NULL || obj->pIndexData == NULL ) 
    return 0;
  
  int intensity = obj->pIndexData->light;
  
  if( obj->pIndexData->item_type != ITEM_LIGHT && obj->pIndexData->item_type != ITEM_LIGHT_PERM )
    return 0; // not actually light
     
  if( obj->value[2] < 0 )
    return 0; // permanent light object

  if( obj->pIndexData->vnum == OBJ_BALL_OF_LIGHT && is_set( ch->affected_by, AFF_CONTINUAL_LIGHT ) )
    return 0; // continual light spell
  
  if( obj->pIndexData->vnum == OBJ_DEAD_MAN_EYES && is_set( ch->affected_by, AFF_DEAD_MAN_EYES ) )
    return 0; //other version of continual

  obj->value[2]--;
  if( obj->value[2] == 1 ) {
    // if( ch->position > POS_SLEEPING ) 
    send( ch, "%s you are carrying flickers briefly.\r\n", obj );
    send( *ch->array, "%s carried by %s flickers briefly.\r\n", obj, ch );
    return 0;
  }

  if( obj->value[2] != 0 || ch->in_room == NULL )
    return 0;

  // Handle light extinguished case.
  if( obj->pIndexData->vnum == OBJ_BALL_OF_LIGHT ) {
    send( ch, "%s which is floating nearby pops and disappears.\r\n", obj );
    send( *ch->array, "%s which is floating near %s pops and disappears.\r\n", obj, ch );
  }
  else if( obj->pIndexData->vnum == OBJ_CORPSE_LIGHT ) {
    send( ch, "%s which is floating nearby, screams as its essence vanishes.\r\n", obj );
    send( *ch->array, "%s which is floating near %s, screams as its essence vanishes.\r\n", obj, ch );
  }
  else if( obj->pIndexData->vnum != OBJ_BALL_OF_LIGHT &&
    obj->pIndexData->vnum != OBJ_CORPSE_LIGHT ) {
  send( ch, "%s you are carrying extinguishes.\r\n", obj );
  send( *ch->array, "%s carried by %s extinguishes.\r\n", obj, ch );
  }

  oprog_data* oprog;
  for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) {
    if( oprog->trigger == OPROG_TRIGGER_TIMER ) {
      var_obj  = obj;
      var_ch   = ch;
      var_room = ch->in_room;
      execute( oprog );
      break;
    }
  }

  if( oprog == NULL && obj->pIndexData->item_type == ITEM_LIGHT )
    obj->Extract( 1 );

  return intensity;
}

void light_update( char_data* ch )
{
  // Loop through the char's wearing[] array looking for lights.  Keep 
  // track of change in extinguished light in intensity.
  int light_lost = 0;       // intensity lost (1 extinguished object)
  int total_light_lost = 0; // intensity lost (all extinguished objects)

  for ( int i = 0 ; i < ch->wearing.size ; i++ ) {
    obj_data* obj = object( ch->wearing[i] );
    if ( obj != NULL ) {
      light_lost = light_obj_update( obj, ch );
      total_light_lost += light_lost;
      if ( light_lost > 0 )
        i--; // because obj->Extract() removes wearing[i] thus shifting the array to the left.
    }
  }

  /*
  // no need for this shit, it's taken care of in remove_weight
  ch->wearing.light -= total_light_lost;
  if (ch->wearing.light < 0)
    ch->wearing.light = 0;

  if (ch->array) {
    ch->array->light -= total_light_lost;
    if (ch->array->light < 0)
      ch->array->light = 0;
  }
  */
  
  return;
}


bool disease_update( char_data* ch, int type )
{
  affect_data*  aff;
  int           stat;
  affect_data   affect;
  char* d;
  char* b;
//   char_data* rch;
  affect_data affectb;
  
  if( !is_set( ch->affected_by, type ) ) 
    return FALSE;

  if( ch->species != NULL && ( ch->shdata->race == RACE_UNDEAD ||
    is_set( ch->species->act_flags, ACT_GHOST ) ) )
    return FALSE;

  for( int i = 0; ; i++ ) {
    if( i >= ch->affected ) {
      bug( "%s a disease with no affect??", ch->real_name( ) );
      return FALSE;
    }
    aff = ch->affected[i];
    if( aff->type == type )
      break; 
  }

  if( ch->species != NULL && ch->fighting == NULL ) {
    strip_affect( ch, aff->type );
    return FALSE;
  }

  aff->level++;
  switch( type ) {
  case AFF_PLAGUE:
    stat = ch->Constitution( );
    d = "the plague";
    b = "body";
    break;
  case AFF_TOMB_ROT:
    stat = ch->Strength( );
    d = "tomb rot";
    b = "body";
    break;
  case AFF_RABIES:
    stat = ch->Intelligence( );
    d = "rabies";
    b = "mind";
    break;
  case AFF_BLIGHT:
    stat = ch->Dexterity( );
    d = "the blight";
    b = "body";
    damage_element( ch, NULL, spell_damage( SPELL_BLIGHT, aff->level ), "the blight", ATT_POISON ); 
    break;
  default:
    return FALSE;
  }
 
  if( !ch || !ch->Is_Valid( ) )
    return FALSE;

  update_max_hit( ch );
  update_max_move( ch );

  affect.type = -1;
  
  if( stat == 3  ) {
    switch( type ) {
    case AFF_PLAGUE:
      death( ch, NULL, "the plague" );
      return TRUE;
    case AFF_TOMB_ROT:
      affect.type = AFF_SLEEP;
      break;
    case AFF_RABIES:
      affect.type = AFF_CONFUSED;
      break;
    case AFF_BLIGHT:
      affect.type = AFF_SLEEP;
      break;
    }
  }

  if( affect.type != -1 ) {
    affect.level = aff->level;
    affect.leech = NULL;
    affect.duration = aff->duration;
    add_affect( ch, &affect );
    return FALSE;
  }

  if( aff->level < 3 ) {
    send( *ch->array, "%s coughs violently.\r\n", ch );
    if( aff->level == 1 ) 
      send( ch, "You cough violently.\r\n" );
    else 
      send( ch, "You cough, your throat burning in pain.\r\nThis is much worse than any cold, you have %s!\r\n", d );
  } else {
    send( ch, "The disease continues to destroy your %s.\r\n", b );
    send( *ch->array, "%s coughs violently, %s has obviously contracted a disease!\r\n", ch, ch->He_She( ) );
  }

  /*
  if( number_range( 1, 19 ) != 1 )
    return FALSE;
  
  if( ch->in_room != NULL && !is_set( &ch->in_room->room_flags, RFLAG_SAFE ) ) {
    rch = random_pers( ch->in_room );
    if( rch != NULL && !is_set( rch->affected_by, AFF_SANCTUARY ) && rch != ch && !is_set( rch->affected_by, type ) ) {
      send( rch, "%s, has infected you with %s!\r\n", ch, d );
      affectb.type = type;
      affectb.level = 0;
      affectb.leech = NULL;
      affectb.duration = 11*number_range( 3, 7 );
      add_affect( rch, &affectb );
    }
  }
  */

  return FALSE;
}


void poison_update( char_data* ch )
{
  if( ch->species != NULL && ch->fighting == NULL ) {
    strip_affect( ch, AFF_POISON );
    return;
  }

  if( is_set( ch->affected_by, AFF_POISON ) ) {
    send( ch, "Your condition deteriorates from a poison affliction.\r\n" );
    send_seen( ch, "%s's condition deteriorates from a poison affliction.\r\n", ch );
    inflict( ch, NULL, 2, "poison" );
    return;
  }

  if( ch->position == POS_INCAP ) 
    inflict( ch, NULL, 1, "[BUG] incap??" );
  else if( ch->position == POS_MORTAL ) 
    inflict( ch, NULL, 2, "bleeding to death" );

  return;
}


/*
 *   TIMED ACTIONS
 */



void room_update( void )
{
  action_data*   action;
  struct timeval  start;
  
  gettimeofday( &start, NULL );

  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) ) {
    for( room_data *room = area->room_first; room != NULL; room = room->next ) {
      // maybe don't update room if there's no player in the room
      for( action = room->action; action != NULL; action = action->next ) {
        if( ( ( action->trigger == TRIGGER_RANDOM ) || action->trigger == TRIGGER_RANDOM_ALWAYS ) && number_range( 0, 999 ) < action->value ) {
          var_room   = room; 
          execute( action );
        }
      }
    }  
  }  

  pulse_time[ TIME_RNDM_ACODE ] = stop_clock( start );  
     
  return;
}


/*
 *   RESETTING OF AREAS
 */


void area_update( void )
{
  struct timeval         start;

  gettimeofday( &start, NULL );

  for( int i = 0; i < clan_list; i++ ) 
    if( clan_list[i]->modified )
      save_clans( clan_list[i] );

  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) ) {
    if( ++area->age < 15 && ( area->nplayer > 0 || area->age < 5 ) )
      continue;

    area->Reset( );
  }
  
  shop_update( );
  pulse_time[ TIME_RESET ] = stop_clock( start );
}


/*
 *  OBJECT UPDATE
 */


void obj_update( void )
{   
  affect_data*          affect;
  obj_data*                obj;
  obj_data*            content  = NULL;
  //obj_data*       content_next;
  obj_clss_data*          fire;
  oprog_data*            oprog;
  room_data*              room;
  char*                message;
  struct timeval         start;


  gettimeofday( &start, NULL );

  for( int k = 0; k < obj_list.size; k++ ) {
    obj = obj_list[k];
    
    if( !obj || !obj->Is_Valid() || obj->array == NULL )
      continue;

    for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) {
      if( ( oprog->trigger == OPROG_TRIGGER_RANDOM ) && number_range( 0, 999 ) < oprog->value ) {
        if( !obj || !obj->Is_Valid( ) || !obj->array ) {
          log( "obj_update: NULL object in trigger %d %s", oprog->obj_vnum, oprog->command );
          break;
        }

        for( thing_data* where = obj; ; ) {
          if( where->array == NULL || ( where = where->array->where ) == NULL || !where->Is_Valid( ) )
            break; 
          if( Room( where ) != NULL )
            var_room = (room_data*) where;
          else if( object( where ) != NULL )
            var_container = (obj_data*) where;
          else if( character( where ) != NULL )
            var_ch = (char_data*) where;
        } 

        var_obj = obj;

        // special handling - make object random's not fire from banks / junked
        player_data *pc = player( var_ch );
        if( pc && ( ( var_container && ( var_container->array == &pc->locker || var_container->array == &pc->junked ) ) || ( var_obj->array == &pc->locker || var_obj->array == &pc->junked ) ) )
          continue;

        execute( oprog );
        break;
      }
    }
  }

  for( int j = 0; j < obj_list.size; j++ ) {
    obj = obj_list[j];

    if( !obj || !obj->Is_Valid() )
      continue;

    for( int i = obj->affected.size-1; i >= 0; i-- ) {
      affect = obj->affected[i];
      if( affect->duration > 0 && --affect->duration == 0 )
        remove_affect( obj, affect );     
    }

    if ( obj->array == NULL )
      continue;

    room = Room( obj->array->where );
    //room = ( obj->array == NULL ? NULL : obj->array->room( ) );

    if( obj->pIndexData->item_type == ITEM_FIRE && obj->timer < 5 &&
      !is_set( obj->pIndexData->extra_flags, OFLAG_NOSHOW ) ) {
      obj->shown = 1;
      obj->selected = 1;
      act_room( room, "@R$p looks in need of some tending.@n", obj );
    }

    // if timer never set, or timer not expired, go on to next item.
    if( room == NULL || obj->timer <= 0 || --obj->timer > 0 ) 
      continue;

    for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next )
      if( oprog->trigger == OPROG_TRIGGER_TIMER ) {
        var_obj  = obj;
        var_room = room;
        execute( oprog );
        break;
      }

    if( oprog != NULL ) // we executed an oprog, go on to next item.
      continue;

    // didn't execute an oprog, so the object vanishes.
    if( obj->pIndexData->item_type == ITEM_FIRE ) {
      if( ( fire = get_obj_index( obj->value[1] ) ) != NULL && ( content = create( fire ) ) != NULL )
        content->To( room ); // this appends one to the end of the obj_list

      obj->Extract( );  // this shifts the obj_list array backwards
      j--;
      //return;
      break;
    }

    switch( obj->pIndexData->item_type ) {
    case ITEM_GATE:
      message = "$1 shrinks to a point and vanishes."; 
      break;

    case ITEM_ANCIENT_OAK:
      message = "$1 breathes a sigh of relief and goes back to sleep.";
      break;

    case ITEM_FOUNTAIN:
      message = "$1 dries up."; 
      break;
    
    case ITEM_SEARCHABLE_CORPSE:
    case ITEM_CORPSE:
      message = "$1 rots, and is quickly eaten by a grue.";
      break;
    
    case ITEM_FOOD:
      message = "$1 decomposes.";
      break;

    default: 
      message = "$1 vanishes.";
      break;

    }

    act_room( room, message, obj );

    /*
     * Why take objects out of a corpse?  No free looting!!!
     *
     * if( obj->pIndexData->item_type == ITEM_CORPSE_PC ) {
     *   for( content = obj->contents; content != NULL;
     *        content = content_next ) {
     *      content_next = content->next_content;
     *      if( number_range( 1, 3 ) != 3 ) {
     *         remove( content, content->number ); 
     *         put_obj( content, room );
     *      }
     *   }
     * }
     *
     */

    obj->Extract( );
    j--; // compensate since Extract shifts the object array to the left

  } // end of big for loop over objects

  pulse_time[ TIME_OBJ_LOOP ] = stop_clock( start );  

  return;
}


