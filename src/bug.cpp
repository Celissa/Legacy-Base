#include "system.h"


/*
 *   BUG ROUTINES
 */


inline void mob_bugs( char_data* ch, species_data* species, bool& found )
{
  mprog_data*   mprog;
  int               i  = 1;
  int               j  = 0;

  if( species->attack->corrupt ) {
    found = TRUE;
    page( ch, "  Attack program on mob #%d (%s) is not compiling.\r\n", species->vnum, species->Name() );
  }

  for( j = 0; j < MAX_ARMOR; j++ ) {
    if( species->chance[j] == 1000 )
      break;
  }
  if( j == MAX_ARMOR ) {
    found = TRUE;
    page( ch, "  Marmor on mob #%d (%s) is not set properly.\r\n", species->vnum, species->Name( ) );
  }


  for( mprog = species->mprog; mprog != NULL; i++, mprog = mprog->next ) {
    if( mprog->corrupt ) {
      found = TRUE;
      page( ch, "  Mprog #%d on mob #%d (%s) is not compiling.\r\n", i, species->vnum, species->Name() );
    }
  }

  return;
}


inline void obj_bugs( char_data* ch, obj_clss_data* obj, bool& found )
{
  bool           either  = FALSE;
  bool            after  = FALSE;
  bool           before  = FALSE;
  const char*   keyword;
  int               i  = 1;

  for( oprog_data* oprog = obj->oprog; oprog != NULL; i++, oprog = oprog->next ) {
    if( oprog->corrupt ) {
      found = TRUE;
      page( ch, "  Oprog #%d on obj #%d (%s) is not compiling.\r\n", i, obj->vnum, obj->Name() );
    }
  }

  for( int i = 0; i < obj->extra_descr; i++ ) {
    keyword = obj->extra_descr[i]->keyword;     
    either |= !strcasecmp( keyword, "either" );
    after  |= !strcasecmp( keyword, "after" );
    before |= !strcasecmp( keyword, "before" );
  }

  if( !either && ( !after || ( !before && obj->fakes == obj->vnum ) ) ) {
    page( ch, "  Object #%-4d (%s) missing description oextra.\r\n", obj->vnum, obj->Name( ) );
    found = TRUE;
  }

  if( is_set( obj->extra_flags, OFLAG_NOSHOW ) && is_set( &obj->wear_flags, ITEM_TAKE ) ) {
    page( ch, "  Object #%-4d (%s) takable and 'no.show'.\r\n", obj->vnum, obj->Name( ) );
    found = TRUE;
  }

  return;
}


inline void room_bugs( char_data* ch, room_data* room, bool& found )
{
  action_data*  action;
  int             i, j;

  for( i = 1, action = room->action; action != NULL; i++, action = action->next ) {
    if( action->corrupt ) {
      found = TRUE;
      page( ch, "  Acode #%d in room %d is not compiling.\r\n", i, room->vnum );
      continue;
    }

    if( action->trigger == TRIGGER_ENTERING || action->trigger == TRIGGER_LEAVING ) {
      for( j = 0; j < MAX_DOOR; j++ )
        if( is_set( &action->flags, j ) )
          break;
        
      if( j == MAX_DOOR ) {
        found = TRUE;
        page( ch, "  Acode #%d in room %d has no direction flag checked.\r\n", i, room->vnum );
      }
    }
  }
 
  return;
}


void do_bugs( char_data* ch, char* argument )
{
  reset_data*        reset;
  room_data*          room;
  species_data*    species;
  trainer_data*    trainer;
  obj_clss_data*       obj;
  bool               found  = FALSE;
  bool             compile;
  int                    i;
  int                flags;

  if( !get_flags( ch, argument, &flags, "omtrchf", "Bugs" ) )
    return;                             

  if( ( flags & 0x6f ) == 0 )
    flags = 0x6f;

  compile = is_set( &flags, 4 );
  if( compile ) {
    send( ch, "Please use the 'compile' function to compile programs.\r\n" );
    return;
  }

  page( ch, "Bugs Found:\r\n" );

  /*-- OBJECTS --*/

  if( is_set( &flags, 0 ) ) {
    for( i = 0; i < MAX_OBJ_INDEX; i++ ) 
      if( ( obj = obj_index_list[i] ) != NULL )  
        obj_bugs( ch, obj, found );
  }

  /*-- TRAINERS --*/

  if( is_set( &flags, 2 ) ) {
    for( trainer = trainer_list; trainer != NULL; trainer = trainer->next ) {
      if( trainer->room == NULL ) {
        page( ch, "  Trainer nowhere??\r\n" );
        found = TRUE;
        continue;
      }

      if( ( species = get_species( trainer->trainer ) ) == NULL ) {
        page( ch, "  Trainer in room #%d with non-existent species.\r\n", trainer->room->vnum );
        found = TRUE;
        continue;
      }

      for( reset = trainer->room->reset; reset != NULL; reset = reset->next )
        if( reset->target == species && is_set( &reset->flags, RSFLAG_MOB ) ) 
          break;

      if( reset == NULL ) {
        page( ch, "  Trainer entry in room %d for mob %d with no reset.\r\n", trainer->room->vnum, trainer->trainer ); 
        found = TRUE;
      }
    }
  }

  /*-- MOBILE BUGS --*/

  if( is_set( &flags, 1 ) ) {
    for( i = 0; i < MAX_SPECIES; i++ ) 
      if( ( species = species_list[i] ) != NULL ) 
        mob_bugs( ch, species, found );
  }

  /*-- AREAS --*/

  if( is_set( &flags, 3 ) ) {
    area_nav alist(&area_list);
    for( area_data *area = alist.first( ); area; area = alist.next( ) )
      if( area->status == AREA_OPEN )
        for( room = area->room_first; room != NULL; room = room->next ) 
          room_bugs( ch, room, found );
  }

  /*-- SKILL HELP FILES MISSING --*/

  help_data* help = NULL;

  if( is_set( &flags, 5 ) ) {
    for( int i = 0; i < MAX_SKILL; i++ ) {
      if( ( help = find_help( ch, skill_table[i].name, false ) ) == NULL ) {
        page( ch, "  Skill with no help file '%s'\r\n", skill_table[i].name );
        found = true;
      }
    }
  }

  /*-- FUNCTION HELP FILES MISSING --*/

  if( is_set( &flags, 6 ) ) {
    for( int i = 0; cfunc_list[i].name[0] != '\0'; i++ ) {
      if( ( help = find_help( ch, cfunc_list[i].name, false ) ) == NULL ) {
        page( ch, "  Function with no help file '%s'\r\n", cfunc_list[i].name );
        found = true;
      }
    }
  }

  if( !found )
    send( "  none\r\n", ch );
}


