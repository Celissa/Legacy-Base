#include "system.h"


char_data*  list_followers   ( char_data* );
extern int NUM_PETS;

/*
 *   FOLLOWER ROUTINES
 */


void degroup( char_data* ch )
{
  remove_bit( &ch->status, STAT_IN_GROUP );
  ch->looter = NULL;
  check_song( ch );

  for( int i = 0; i < ch->followers; i++ )
    degroup( ch->followers[i] );
}


void do_follow( char_data* ch, char* argument )
{
  char_data* victim;

  if( ( victim = one_character( ch, argument, "follow", ch->array ) ) == NULL )
    return;

  if( is_set( &ch->status, STAT_PET ) ) {
    interpret( ch, "sulk" );
    interpret( ch, "shake" );
    return;
  }

  if( victim == ch ) {
    if( ch->leader == NULL ) {
      send( ch, "You already follow yourself.\r\n" );
      return;
    }
    stop_follower( ch );
    return;
  }
 
  if( victim->species != NULL && victim->link == NULL ) {
    send( ch, "You can only follow players.\r\n" );
    return;
  }

//  if( !associate( ch, victim ) )
//    return;

  if( ch->leader != NULL )
    stop_follower( ch );

  add_follower( ch, victim );
}


void add_follower( char_data* ch, char_data* victim, bool msg )
{
  char_data* leader;
  share_data* share;
  
  if( ch->leader != NULL ) {
    bug( "Add_follower: non-null leader." );
    return;
  }

  for( leader = victim->leader; leader != NULL; leader = leader->leader )
    if( leader == ch ) {
      send( ch, "No loops in follow allowed.\r\n" );
      return;
    }
    
  ch->leader         = victim;
  victim->followers += ch;
  if( is_pet( ch ) && ( ch->Get_Integer( "/Squeed/Code/PetId" ) == 0 ) ) {
    ch->Set_Integer( "/Squeed/Code/PetId", NUM_PETS++ );
    share = new Share_Data();
    share->strength = ch->shdata->strength;
    share->intelligence = ch->shdata->intelligence;
    share->wisdom = ch->shdata->wisdom;
    share->dexterity = ch->shdata->dexterity;
    share->constitution = ch->shdata->constitution;
    share->level = ch->shdata->level;
    share->alignment = ch->shdata->alignment;
    share->race = ch->shdata->race;
    share->kills = ch->shdata->kills;
    share->deaths = ch->shdata->deaths;
    share->fame = ch->shdata->fame;
    for( int i = 0; i < MAX_RESIST; i++ ) {
      share->resist[i] = ch->shdata->resist[i];
    }
    ch->shdata = share;
  }
  
  degroup( ch );

  if( msg ) {
    if( ch->Seen( victim ) )
      send( victim, "%s now follows you.\r\n", ch );
    send( ch, "You now follow %s.\r\n", victim->Seen_Name( ch ) );
    send_seen( ch, "%s starts following %s.\r\n", ch, victim );
  }
}


void stop_follower( char_data* ch, bool msg )
{
  if( ch->leader == NULL ) {
    bug( "Stop_follower: null leader." );
    bug( "-- Ch = %s", ch );
    return;
  }

  if( is_set( &ch->status, STAT_PET ) ) {
    char tmp[ TWO_LINES ];
    remove_bit( &ch->status, STAT_PET );
    remove_bit( &ch->status, STAT_TAMED );
    remove_bit( &ch->status, STAT_FAMILIAR );
    player_data *pc = player( ch->leader );
    if( pc->familiar == ch ) 
      pc->familiar = NULL;
    sprintf( tmp, "%s [PET] stopped following.", ch->descr->name );
    player_log( pc, tmp );
  }

  if( msg ) {
    if( ch->Seen( ch->leader ) )
      send( ch->leader, "%s stops following you.\r\n", ch );
    send( ch, "You stop following %s.\r\n", ch->leader );
  }
  if( group_looter( ch ) == ch ) {
    char_data *leader = group_leader( ch );
    leader->looter = NULL;
    if( ch->Seen( leader ) && msg )
      send( leader, "%s stops looting for you.\r\n", ch);
  }

  remove_bit( &ch->status, STAT_IN_GROUP );
  ch->leader->followers -= ch;
  ch->leader = NULL;
  check_song( ch );
}


/*
 *   GROUP ROUTINES
 */


void add_group( char_data* ch, char_data* victim, bool msg )
{
  if( victim == ch ) {
    if( msg ) {
      send( ch, "You add yourself to your group.\r\n" );
      send_seen( ch, "%s joins %s own group.\r\n", ch, ch->His_Her( ) );
    }
    set_bit( &ch->status, STAT_IN_GROUP ); 
    return;
  }
  if( !is_set( &ch->status, STAT_IN_GROUP ) ) {
    if( msg ) 
      send( ch, "You need to group yourself first.\r\n" );
    return;
  }

  if( msg ) {
    send( victim, "You join %s's group.\r\n", ch );
    send( ch, "%s joins your group.\r\n", victim );
    send_seen( victim, "%s joins %s's group.\r\n", victim, ch );
  }

  set_bit( &victim->status, STAT_IN_GROUP ); 
  check_song( victim );
}


void group_all( char_data* ch )
{
  char_data*   rch;
  bool       found  = FALSE;

  if( !is_set( &ch->status, STAT_IN_GROUP ) ) {
    add_group( ch, ch );
    found = TRUE;
  }

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch->leader == ch && !is_set(&rch->status, STAT_PET) && !is_set( &rch->status, STAT_IN_GROUP ) && rch->Seen( ch ) ) {
      add_group( ch, rch );
      found = TRUE;
    }

  if( !found ) 
    send( ch, "You and all players following you are already grouped.\r\n" );
}   


void group_pets( char_data* ch )
{
  bool       found  = FALSE;
  char_data *rch = NULL;

  if( !is_set( &ch->status, STAT_IN_GROUP ) ) {
    add_group( ch, ch );
    found = TRUE;
  }

  for( int i = 0; i < ch->followers; i++ ) 
    if( ( rch = character( ch->followers[ i ] ) ) != NULL && is_set( &rch->status, STAT_PET ) && !is_set( &rch->status, STAT_IN_GROUP ) && rch->Seen( ch ) ) {
      rch->selected = 1;
      rch->shown    = 1;

      send( ch, "%s joins your group.\r\n", rch );
      set_bit( &rch->status, STAT_IN_GROUP ); 
      check_song( rch );
      found = TRUE;
    }

  if( !found ) {
    // ungroup pets
    for( int i = 0; i < ch->followers; i++ ) 
      if( ( rch = character( ch->followers[ i ] ) ) != NULL && is_set( &rch->status, STAT_PET ) && is_set( &rch->status, STAT_IN_GROUP ) && rch->Seen( ch ) ) {
        send( ch, "You remove %s from your group.\r\n", rch );
        remove_bit( &rch->status, STAT_IN_GROUP );
        check_song( rch );
        found = TRUE;
      }

    if( !found )
      send( ch, "You have no pets to group or ungroup.\r\n" );
  }
}   


void display_group( char_data* ch )
{
  char               tmp  [ TWO_LINES ];
  char              name  [ TWO_LINES ];
  char                hp  [ 15 ];
  char              move  [ 15 ];
  char            energy  [ 15 ];
  char_data*         gch;
  char_array*      array;
  char_array   incognito;
  bool             found  = FALSE;

  if( !is_set( &ch->status, STAT_IN_GROUP ) ) {
    send( ch, "You aren't in any group.\r\n" );
    return;
  }

  char_data *leader = group_leader(ch);
  char_data *looter = group_looter(ch);

  send( ch, "Leader: %s\r\n", leader );
  send( ch, "Looter: %s\r\n", looter ? looter->Seen_Name(ch) : "no one" );
  send_underlined( ch, "                                      Hits    Energy     Moves          Exp\r\n" );

  array = followers( leader );
  if( !array )
    return;

  for( int i = 0; i < *array; i++ ) {
    gch = array->list[i];

    if( !is_set( &gch->status, STAT_IN_GROUP ) )
      continue; 

    if( gch != ch && gch->pcdata != NULL && is_set( gch->pcdata->pfile->flags, PLR_GROUP_INCOG ) && is_incognito( gch, ch ) ) {
      incognito += gch;
      continue;
    }

    strcpy( name, gch->Seen_Name( ch ) );
    truncate( name, 17 );

    sprintf( hp,     "%d/%d", gch->hit,  gch->max_hit );
    sprintf( energy, "%d/%d", gch->mana, gch->max_mana );
    sprintf( move,   "%d/%d", gch->move, gch->max_move );

    sprintf( tmp, "[ %2d %3s %3s ] %-17s %9s %9s %9s %12d\r\n"
      , gch->shdata->level
      , gch->species != NULL ? "Mob" : gch->shifted != NULL ? "Mob" : clss_table[ gch->pcdata->clss ].abbrev
      , gch->shifted != NULL ? race_table[ gch->shifted->shdata->race ].abbrev : race_table[ gch->shdata->race ].abbrev
      , name, hp, energy, move
     // , gch->pcdata == NULL ? 0 : exp_for_level( gch )-gch->exp
      , exp_for_level( gch )-gch->exp
    );

    tmp[15] = toupper( tmp[15] );
    send( ch, tmp );
  }

  delete array;

  for( int i = 0; i < incognito; i++ ) {
    if( !found ) {
      found = TRUE;
      send( ch, "%46s\r\n", "-*-" );
    }

    selt_string x;
    x = incognito[i]->Name( ch );
    x.truncate( 75 - 20 - 16, true );

    send( ch, "[ Incognito  ] %s %20s\r\n", x.get_text(), condition_word( incognito[i] ) );
  }
}


void do_group( char_data* ch, char* argument )
{
  char_data*  victim;

  if( *argument == '\0' ) {
    display_group( ch );
    return;
  }

  if( ch->leader != NULL ) {
    send( ch, "But you are following someone else!\r\n" );
    return;
  }

  if( !strcasecmp( argument, "all" ) ) {
    group_all( ch );
    return;
  } else if( !strcasecmp( argument, "pet" ) || !strcasecmp( argument, "pets" ) ) {
    group_pets( ch );
    return;
  } else if( matches( argument, "looter" ) ) {
    if( !*argument ) {
      send( ch, "%s is currently looting for the group.\r\n", ch->looter ? ch->looter->Seen_Name( ch ) : "Nobody" );
      return;
    } else if( matches( argument, "off" ) ) {
      send( ch, "Group looter turned off.\r\n" );
      ch->looter = NULL;
      return;
    }

    if( ( victim = one_player( ch, argument, "set as group looter", ch->array, (thing_array*) &ch->followers ) ) == NULL )
      return;

    if( victim->leader != ch && ch != victim ) {
      send( ch, "%s isn't following you.\r\n", victim );
      return;
    }
    
    if( !is_set( &victim->status, STAT_IN_GROUP ) ) {
      send( ch, "%s isn't a member of your group.\r\n", victim );
      return;
    }

    ch->looter = victim;
    if( victim == ch ) {
      send( ch, "You appoint yourself as the group looter.\r\n" );
      send( *(thing_array*) &ch->followers, "%s appoints %sself as the group looter.\r\n", ch, ch->Him_Her( ) );
    } else {
      send( ch, "You appoint %s as the group looter.\r\n", victim );
      send( victim, "%s appoints you as the group looter.\r\n", ch );
      send( *(thing_array*) &ch->followers, "%s appoints %s as the group looter.\r\n", ch, ch->looter );
    }
    return;
  }


  if( ( victim = one_character( ch, argument, "group", ch->array, (thing_array*) &ch->followers ) ) == NULL )
    return;

  if( victim->leader != ch && ch != victim ) {
    send( ch, "%s isn't following you.\r\n", victim );
    return;
  }

  if( is_set( &victim->status, STAT_IN_GROUP ) ) {
    if( victim != ch ) {
      send( ch, "You remove %s from your group.\r\n", victim );
      send( victim, "%s removes you from %s group.\r\n", ch, ch->His_Her( ) ); 
      send( *ch->array, "%s removes %s from %s group.\r\n", ch, victim, ch->His_Her( ) );
      if( !is_set( &victim->status, STAT_PET ) )
        stop_follower( victim );

    } else {
      char_array* array = followers( ch );
      if( array ) {
        char_data* rch;
        for( int i = 0; i < *array; i++ ) {
          rch = array->list[i];
          if( !is_pet( rch ) && rch != ch ) 
            stop_follower( rch );
          remove_bit( &rch->status, STAT_IN_GROUP );
          if( rch != ch ) {
            send( ch, "You remove %s from your group.\r\n", rch );
            send( rch, "%s removes you from %s group.\r\n", ch, ch->His_Her( ) ); 
            send( *ch->array, "%s removes %s from %s group.\r\n", ch, rch, ch->His_Her( ) );
          }
        }
      }
        
      send( "You remove yourself from your group.\r\n", ch );
      send_seen( ch, "%s removes %sself from %s group.\r\n", ch, ch->Him_Her( ), ch->His_Her( ) );
    }

    remove_bit( &victim->status, STAT_IN_GROUP );
    check_song( ch );
    return;
  }

  add_group( ch, victim );
}


/*
 *   UTILITY ROUTINES
 */


void add_followers( char_data* ch, char_array& array, content_array* where )
{
  if( !ch || !ch->Is_Valid() || where != NULL && ch->array != where )
    return;

  array += ch;

  for( int i = 0; i < ch->followers; i++ ) 
    add_followers( ch->followers[i], array, where );
}


char_array* followers( char_data* leader, content_array* where )
{
  if( !leader || !leader->Is_Valid() ) //  || !where )
    return NULL;

  char_array* array = new char_array;
  add_followers( leader, *array, where );
  return array;
}


int min_group_move( char_data* ch )
{
  char_data*   rch;
  int         move  = ch->move;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL && is_same_group( ch, rch ) && rch != ch )
      move = min( move, rch->move );

  return move;
}


bool is_same_group( char_data* ach, char_data* bch )
{
  if( ach == NULL || bch == NULL )
    return FALSE;

  if( ach == bch )
    return TRUE;

  if( !is_set( &ach->status, STAT_IN_GROUP ) || !is_set( &bch->status, STAT_IN_GROUP ) )
    return FALSE;

  for( ; ach->leader != NULL; ach = ach->leader );
  for( ; bch->leader != NULL; bch = bch->leader );

  return( ach == bch );
}


/*
 *   ORDER COMMAND
 */


void do_order( char_data* ch, char* argument )
{
  char          arg  [ MAX_INPUT_LENGTH ];
  char_data*    rch;
  thing_array  list;
//  mprog_data* mprog;
  room_data*   room  = ch->in_room;
  bool        found  = FALSE;

  if( !two_argument( argument, "to", arg ) ) {
    argument = one_argument( argument, arg );
    if( *argument == '\0' ) {
      send( ch, "Syntax: Order <pet|all> [to] <command>\r\n" );
      return;
    }
  }

  if( strcasecmp( arg, "all" ) ) {
    if( ( rch = one_character( ch, arg, "order", ch->array ) ) == NULL )
      return;

    if( rch == ch ) {
      send( ch, "Ordering yourself makes no sense.\r\n" );
      return;
    }
         
    send( ch, "You order %s to '%s'.\r\n", rch, argument );
    if( rch->position > POS_SLEEPING )
      send( rch, "%s orders you to '%s'.\r\n", ch, argument );
    send( *ch->array, "%s orders %s to '%s'.\r\n", ch, rch, argument );

    if( !is_set( &rch->status, STAT_PET ) || rch->leader != ch ) {
      send( ch, "%s ignores you.\r\n", rch );
      return;
    }

    set_bit( &rch->status, STAT_ORDERED );
    interpret( rch, argument );
    remove_bit( &rch->status, STAT_ORDERED );
    return;
  }
/*
  rch = one_character( ch, arg, "order", ch->array );
  for( mprog = rch->species->mprog; mprog != NULL; mprog = mprog->next )
    if( mprog->trigger == MPROG_TRIGGER_ORDER ) {
      var_ch = ch;
      var_mob = rch;
      var_room = ch->in_room;
      execute( mprog );
      return;
      }
*/
  copy( list, *ch->array );

  for( int i = 0; i < list; i++ ) {
    if( ( rch = character( list[i] ) ) == NULL
      || rch->array != &room->contents
      || !is_set( &rch->status, STAT_PET )
      || rch->leader != ch )
      continue;

    if( !found ) {
      send( *ch->array, "%s orders all %s followers to '%s'.\r\n", ch, ch->His_Her( ), argument );
      fsend( ch, "You order all your followers to '%s'.\r\n", argument );
      found = TRUE;
    }

    rch->shown = 1;
    rch->selected = 1;
    set_bit( &rch->status, STAT_ORDERED );
    interpret( rch, argument );
    remove_bit( &rch->status, STAT_ORDERED );
  }
 
  if( !found ) 
    send( ch, "You have no followers here.\r\n"  );
}



