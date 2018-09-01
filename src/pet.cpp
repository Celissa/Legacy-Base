#include "system.h"

extern obj_data*   make_corpse     ( char_data*, content_array* );

/*
 *   NAME FUNCTIONS
 */

// Put restrictions on pet names here.  See check_parse_name() in newbie.cc
// for a similar routine.
bool check_pet_name( char_data* ch, char* name )
{
  int length = strlen( name );

  if( length < 3 || length > 12 ) {
    send( ch, "Names must be greater than 2 and less than 13 letters.\r\n" );
    return FALSE;
  }

  if( ispunct( *name ) || ispunct( name[length] ) ) {
    send( ch, "\r\nThe first and last letters of a name may not be punctuation to stop the\r\nsegment of the population who insist on the name --Starbright--.\r\n" );
    return FALSE;
  }
  
  if( isdigit(*name) ) {
    send( ch, "Your pet's name may not start with a number.\r\n" );
    return FALSE;
  }

  if( pntr_search( badname_array, max_badname, name ) >= 0 ) {
    help_link( ch->link, "Bad_Name" );
    return FALSE;
  }

  return TRUE;
}


void do_name( char_data* ch, char* argument )
{
  char           arg  [ MAX_INPUT_LENGTH ];
  char_data*  victim;

  argument = one_argument( argument, arg );

  if( ( victim = one_character( ch, arg, "name", ch->array ) ) == NULL )
    return;

  if( victim->species == NULL || victim->leader != ch || !is_set( &victim->status, STAT_PET ) ) {
    send( ch, "You can only name pets of your own.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "What do you want to name them?\r\n" );
    return;
  }

  if( matches( argument, "blank" ) ) {
    send( ch, "Name removed from %s.\r\n", victim );
    free_string( victim->pet_name, MEM_MOBS );
    victim->pet_name = empty_string;
    return;
  }

  if( !check_pet_name( ch, argument ) )
    return;

  send( ch, "%s is now named '%s'.\r\n", victim, argument );

  free_string( victim->pet_name, MEM_MOBS );
  victim->pet_name = alloc_string( argument, MEM_MOBS );

  return;
}


/*
 *   PET ROUTINES
 */


void do_call( char_data* ch, char * )
{
  player_data* pc   = player( ch );
  
  if( pc == NULL )
    return;

  char_data* victim  = pc->familiar;

  if( victim == NULL ) {
    send( ch, "You do not have a special creature to call to you.\r\n" );
    return;
  }

  if( !is_apprentice( ch ) ) {
    if( !victim->in_room || !victim->in_room->area || !ch->in_room 
      || !ch->in_room->area || victim->in_room->area->continent != ch->in_room->area->continent ) {
      send( ch, "Your spell cannot cross sea or ocean.\r\n");
      return;
    }
  }
  
  if( is_set( victim->affected_by, AFF_SLEEP ) || victim->fighting != NULL ) {
    send( ch, "%s is unable to respond to your call.\r\n", victim->Name( ) );
    return;
  }

  if( victim->position < POS_STANDING )
    interpret( pc->familiar, "stand" );

  send( ch, "Carefully bending down to touch the ground, you concentrate for\r\na brief moment on %s, summoning %s.\r\n", victim->Name( ), victim->Him_Her( ) );

  send_seen( ch, "Carefully bending down to touch the ground, %s concentrates\r\nfor a brief moment.\r\n", ch );

  spell_summon( ch, victim, NULL, 0, 0 );
  return;
}

void abandon( player_data* pc, char_data* pet )
{
  char*             tmp;
  mprog_data*     mprog;
  bool           worked = true;
  affect_data*      aff;

  for( mprog = pet->species->mprog; mprog != NULL; mprog = mprog->next ) {
    if( mprog->trigger == MPROG_TRIGGER_ABANDON ) {
      var_ch   = pet->leader;
      var_mob  = pet;
      var_room = pet->in_room;
      worked = execute( mprog );
      if( !worked )
        return;
    }
  }

  if( is_set( pc->affected_by, AFF_COMPANIONS_STRENGTH ) ) {
    for( int i = pc->affected.size-1;i >= 0; i-- ) {
      aff = pc->affected[i];
      if( aff->type == AFF_COMPANIONS_STRENGTH ) {
        if( aff->mlocation[0] == affect_loc[ pet->species->compan_str ] &&
          aff->mmodifier[0] == pet->species->compan_amt ) {
          strip_affect( pc, AFF_COMPANIONS_STRENGTH );
        }
      }
    }
  }

  pet->shown = 1;
  send( pc, "You abandon %s.\r\n", pet );

  if( pc->familiar == pet )
    pc->familiar = NULL;

  if( pet->shdata->race == RACE_UNDEAD )
    make_corpse( pet, pet->array );

  remove_bit( &pet->status, STAT_PET );
  remove_bit( &pet->status, STAT_AGGR_ALL );
  remove_bit( &pet->status, STAT_AGGR_GOOD );
  remove_bit( &pet->status, STAT_AGGR_EVIL );

  stop_follower( pet );
  free_string( pet->pet_name, MEM_MOBS );
  pet->pet_name = empty_string;
  pet->Set_Integer( "/Squeed/Code/Levels", 0 );

  tmp = static_string( );
  sprintf( tmp, "Abandoned %s [%s] at %s.", pet->Name( ), condition_word( pet ), pet->array->where->Location( ) );
  player_log( pc, tmp );

  if( pet->shdata->race == RACE_UNDEAD ) {
    send( *pet->array, "%s shudders and crumples to the ground lifeless, a thin\r\nmist briefly rising from the fading corpse.\r\n", pet );
    pet->Extract( );  // we made the corpse earlier
  } else {
    pet->old_leader = pc->pcdata->pfile;
    do_stand( pet, "" );
  }
}


void do_pets( char_data* ch, char* argument )
{
  char_data*        pet;
  char_data*     victim  = ch;
  player_data*       pc;
  bool            found  = FALSE;
  int              i, j;

  if( is_mob( ch ) )
    return;

  pc = player( victim );

  if( exact_match( argument, "abandon" ) ) {
    j = atoi( argument );
    for( i = 0; i < ch->followers.size; i++ ) {
      if( is_pet( pet = ch->followers.list[i] ) && --j == 0 ) {
        abandon( pc, pet );
        return;
      }
    }
    send( ch, "You have no pet with that number.\r\n" );
    return;
  }

  if( *argument == '\0' )
    victim = ch;
  else if( ch->shdata->level < LEVEL_APPRENTICE ) {
    send( ch, "Unknown syntax - See help pets.\r\n" );
    return;
  } 
  else {
    if( ( victim = one_player( ch, argument, "pets", (thing_array*) &player_list ) ) == NULL )
      return;
  }


  if( victim->species != NULL ) {
      send( ch, "%s isn't a player and thus won't have pets.\r\n", victim );
      return;
  }     
    
  for( i = j = 0; i < victim->followers.size; i++ ) {
    if( is_pet( pet = victim->followers.list[i] ) ) {
      if( !found ) {
        send_underlined( ch, "Num  Name                      Type       Location\r\n" );
        found = TRUE;
      }
      pet->selected = 1;
      pet->shown    = 1;
      send( ch, "%3d  %-25s %-10s %s\r\n", ++j, pet->pet_name != empty_string ? pet->pet_name : pet->Seen_Name( ch ), 
        is_set( &pet->status, STAT_TAMED ) ? "tamed" :
        is_set( &pet->status, STAT_FAMILIAR ) ? "familiar" : "bought", 
        pet->in_room == NULL ? "nowhere??" : pet->in_room->name );
    } 
  }

  if( !found ) {
    if( ch == victim )
      send( ch, "You have no pets.\r\n" );
    else
      send( ch, "%s has no pets.\r\n", victim );
  }
}


/*
 *   PET SUPPORT FUNCTIONS
 */


bool has_mount( char_data* ch )
{
  char_data*    mount;
  player_data*     pc = player( ch );
  int               i;

  for( i = 0; i < ch->followers.size; i++ ) {
    mount = ch->followers.list[i];
    if( mount->species != NULL && is_set( mount->species->act_flags, ACT_MOUNT ) && pc->familiar != mount ) {
      send( ch, "You are only able to acquire one mount at a time.\r\n" );
      return TRUE;
    }
  }

  return FALSE;
}

bool has_elemental( char_data* ch )
{
  char_data*  buddy;
  int            i;

  for( i = 0; i < ch->followers.size; i++ ) {
    buddy = ch->followers.list[i];
    if( buddy->species != NULL && is_set( buddy->species->act_flags, ACT_ELEMENTAL ) ) {
      return TRUE;
    }
  }

  return FALSE;
}

int number_of_pets( char_data* ch )
{
  char_data*  pet;
  int         num;
  int           i;
  player_data* pc = player( ch );

  for( i = num = 0; i < ch->followers.size; i++ ) {
    pet = ch->followers.list[i];
    if( pc && pc->familiar == pet )
      continue;
    if( is_set( &pet->status, STAT_PET ) && !is_set( &pet->status, STAT_TAMED ) && !is_set( pet->species->act_flags, ACT_MOUNT ) ) 
      num++;
  }

  return num;
}

bool can_level( char_data* ch ) {
  bool tame, fam, mount;
  player_data* pc;
  char_data* p;
  int level, plevel, levels;
  char_data* pet;

  if( !is_pet( ch ) )
    return FALSE;
  level = ch->shdata->level;
  p = ch->leader;
  if( !p )
    return FALSE;  
  pc = player( p );
  if( !pc )
    return FALSE;
  plevel = p->shdata->level;
  
  tame = is_set( &ch->status, STAT_TAMED );
  mount = is_set( ch->species->act_flags, ACT_MOUNT );
  fam = pc && pc->familiar == ch;
  
  if( fam )
    return plevel > level;
  if( tame )
    return plevel > pet_levels( p );
  if( mount )
    return plevel/2 > level;
  for( int i = levels = 0; i < p->followers.size; i++ ) {
    pet = p->followers.list[i];
    if( pc->familiar == pet )
      continue;
    if( is_set( &pet->status, STAT_PET ) && !is_set( &pet->status ,STAT_TAMED) 
      && !is_set( pet->species->act_flags, ACT_MOUNT ))
      levels += pet->shdata->level;
  }
  return plevel/2 > levels; 
}

int pet_levels( char_data* ch )
{
  char_data*    pet;
  int         level;//, fam, mount, pets;
  int             i;
  
  for( i = level = 0; i < ch->followers.size; i++ ) {
    pet = ch->followers.list[i];
    if( is_set( &pet->status, STAT_TAMED ) )
      level += pet->shdata->level;
  }

  return level;
}

