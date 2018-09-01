#include "system.h"

extern bool tfh_assume_lit;      // Global variable defined in ranger.cc

void   room_info          ( char_data*, room_data* );

void   look_in            ( char_data*, char* );
void   look_in            ( char_data*, obj_data* );

void   look_at_character  ( char_data*, char_data* );
void   show_equipment     ( char_data*, char_data* );
void   show_inventory     ( char_data*, char_data* );
void   show_description   ( char_data*, char_data* );


/*
 *   CAN_SEE ROUTINES
 */


bool Char_Data :: Can_See( bool msg )
{
  if( position <= POS_SLEEPING || is_set( affected_by, AFF_BLIND ) ) {
    if( msg )
      send( this, "You can't see a thing!\r\n" );
    return FALSE;
  }

  return TRUE;
}


bool Room_Data :: Seen( char_data* ch )
{
  if( !ch->Can_See( ) )
    return FALSE;

  if( ch->pcdata != NULL && is_set( ch->pcdata->pfile->flags, PLR_HOLYLIGHT ) )
    return TRUE;

  // maybe check infravision?  greyscale rooms?

  if( ch->shifted != NULL )
    return TRUE;

  if( is_dark( ) )
     return FALSE;

  return TRUE;
}


bool Char_Data :: Seen( char_data* ch )
{
  wizard_data* imm;

  if( !In_Game( ) )
    return FALSE;

  if( this == ch || ch == NULL )
    return TRUE;

  if( ( imm = wizard( this ) ) != NULL && is_set( pcdata->pfile->flags, PLR_WIZINVIS ) && ch->shdata->level < imm->wizinvis )
    return FALSE;

  if( !in_character )
    return TRUE;

  if( ch->pcdata != NULL && is_set( ch->pcdata->pfile->flags, PLR_HOLYLIGHT ) )
    return TRUE; // this mob/player can't hide from holylight

  if( !ch->Can_See( ) )
    return FALSE;
/*
  if( ch->species != NULL && ch->link != NULL )
    return TRUE;
*/    
  if( !tfh_assume_lit ) {
    if( in_room != NULL && species != NULL && in_room->is_dark( ) && !is_set( affected_by, AFF_SANCTUARY ) && ch->shifted == NULL && ( !is_set( ch->affected_by, AFF_INFRARED ) || ( species != NULL && !is_set( species->act_flags, ACT_WARM_BLOODED ) ) ) )
      return FALSE;
  }
  
  if( is_set( affected_by, AFF_INVISIBLE ) && !ch->Sees_Invis( ) )
    return FALSE;

  if( ( is_set( &status, STAT_CAMOUFLAGED ) || is_set( affected_by, AFF_CAMOUFLAGE ) ) && !ch->Sees_Camouflage( ) && !includes( seen_by, ch ) )
    return FALSE;

  if( ( is_set( &status, STAT_HIDING ) || is_set( affected_by, AFF_HIDE )
    || is_set( affected_by, AFF_SHADOW ) ) && !ch->Sees_Hidden( ) && !includes( seen_by, ch ) )
    return FALSE;

  return TRUE;
}


bool Obj_Data :: Seen( char_data* ch )
{
  if( !ch || array == &ch->contents || array == &ch->wearing )
    return TRUE;

  if( ch->pcdata != NULL && is_set( ch->pcdata->pfile->flags, PLR_HOLYLIGHT ) )
    return TRUE; // this object can't hide from holylight

  if( !ch->Can_See( ) )
    return FALSE;

  if( ch->in_room && ch->shifted == NULL && ch->in_room->is_dark( ) && pIndexData->item_type != ITEM_LIGHT && pIndexData->item_type != ITEM_LIGHT_PERM )
    return FALSE;

  if( is_set( extra_flags, OFLAG_IS_INVIS ) && !ch->Sees_Invis( ) )
    return FALSE;

  if( is_set( extra_flags, OFLAG_DARK ) )
    return FALSE;
/*
  if( get )
    if( is_set( extra_flags, OFLAG_NOSHOW ) && !is_set( &pIndexData->wear_flags, ITEM_TAKE ) )
      return FALSE;
*/
  return TRUE;
}


/*
 *   ATTRIBUTES
 */


bool char_data :: detects_evil( )
{
  return( is_set( affected_by, AFF_TRUE_SIGHT ) 
    || is_set( affected_by, AFF_DETECT_EVIL ) );
}


bool char_data :: detects_good( )
{
  return( is_set( affected_by, AFF_TRUE_SIGHT ) 
    || is_set( affected_by, AFF_DETECT_GOOD ) );
}


bool Char_Data :: Sees_Invis( )
{
  if( pcdata != NULL
    && is_set( pcdata->pfile->flags, PLR_HOLYLIGHT ) )
    return TRUE;

  return( is_set( affected_by, AFF_SEE_INVIS ) 
    || is_set( affected_by, AFF_TRUE_SIGHT ) );
}

bool Char_Data :: Sees_Hidden( )
{
  if( pcdata != NULL
    && is_set( pcdata->pfile->flags, PLR_HOLYLIGHT ) )
    return TRUE;

  return( is_set( affected_by, AFF_DETECT_HIDDEN ) 
    || is_set( affected_by, AFF_SENSE_LIFE ) 
    || is_set( affected_by, AFF_SPOT_HIDDEN ) );
}

bool Char_Data :: Sees_Camouflage( )
{
  if( pcdata != NULL
    && is_set( pcdata->pfile->flags, PLR_HOLYLIGHT ) )
    return TRUE;

  return( is_set( affected_by, AFF_SEE_CAMOUFLAGE ) );
}

/* 
 *   SHOW OBJECT ROUTINES
 */


void page( char_data* ch, thing_array& array )
{
  thing_data*   thing;
  bool        nothing  = TRUE;

  select( array );
  rehash( ch, array );

  for( int i = 0; i < array; i++ ) {
    thing = array[i];
    if( thing->Seen( ch ) && thing->shown > 0 ) {
      nothing = FALSE;
      page( ch, "  %s\r\n", thing );
      }
    }

  if( nothing )
    page( ch, "  nothing\r\n" );
}


void send( char_data* ch, thing_array& array )
{
  thing_data*   thing;
  bool        nothing  = TRUE;

  select( array );
  rehash( ch, array );

  for( int i = 0; i < array; i++ ) {
    thing = array[i];
    if( thing->shown > 0 ) {
      nothing = FALSE;
      send( ch, "  %s\r\n", thing );
      act( ch, "%s\r\n", thing );
      }
    }

  if( nothing )
    send( ch, "  nothing\r\n" );
}


/* 
 *   TRACK AND SEARCH ROUTINES
 */


void show_secret( char_data* ch ) 
{
  exit_data*  exit;

  if( ch->pcdata == NULL || !is_set( ch->pcdata->pfile->flags, PLR_SEARCHING ) || ch->move == 0 )
    return; 

  ch->move--;

  for( int i = 0; i < ch->in_room->exits; i++ ) {
    exit = ch->in_room->exits[i];
    if( is_set( &exit->exit_info, EX_SEARCHABLE )
     && is_set( &exit->exit_info, EX_CLOSED )
     && is_set( &exit->exit_info, EX_SECRET ) 
     && !includes( ch->seen_exits, exit )
     && ch->check_skill( SKILL_SEARCHING )
    ) {
      ch->improve_skill( SKILL_SEARCHING );
      send( ch, "\r\n%s>> You detect something unusual %s. <<%s\r\n", bold_v( ch ), dir_table[ exit->direction ].name, normal( ch ) );
      ch->seen_exits += exit;
    }
  }
}


/*
 *   LOOK AT
 */


void Char_Data :: Look_At( char_data* ch )
{
  known_by += ch;

  if( ch != this ) {
    if( ch->Seen( this ) )
      send( this, "%s looks at you.\r\n", ch );
    send_seen( ch, "%s looks at %s.\r\n", ch, this );
  } else {
    send_seen( ch, "%s looks at %sself.\r\n", ch, ch->Him_Her( ) );
  }
  
  show_description( ch, this );
  page( ch, scroll_line[0] );
  glance( ch, this );
  page( ch, "\r\n" );
  in_character = FALSE;
  if( this->shifted == NULL )
    show_equipment( ch, this );
  in_character = TRUE;
}


void Exit_Data :: Look_At( char_data* ch )
{
  if( !is_set( &exit_info, EX_ISDOOR ) ) {
    send( ch, "To the %s is %s.\r\n",
       dir_table[ direction ].name,
       to_room->name );
    }
  else {   
    send( ch, "The %s is %s.\r\n", name,
      is_set( &exit_info, EX_CLOSED ) ? "closed" : "open" );
    }
}  


void Extra_Data :: Look_At( char_data* ch )
{
  char tmp [ MAX_STRING_LENGTH ];

  convert_to_ansi( ch, text, tmp, sizeof(tmp) );
  send( ch, tmp );
}

   


/* 
 *   SHOW CHARACTER FUNCTIONS
 */


void do_peek( char_data* ch, char* argument )
{
  char_data *victim;

  if( !ch->Can_See( TRUE ) )
    return;

  if( ch->shdata != NULL && !ch->get_skill( SKILL_PEEK ) ) {
    send( ch, "You do not know how to peek.\r\n" );
    return;
  }

  if( ( victim = one_character( ch, argument, "peek at",
    ch->array ) ) == NULL )
    return;

  show_equipment( ch, victim );
  page( ch, "\r\n" );
  show_inventory( ch, victim );

  ch->improve_skill( SKILL_PEEK );

  return;
}


void do_qlook( char_data *ch, char *argument )
{
  char_data* victim;

  if( !ch->Can_See( TRUE ) )
    return;

  if( *argument == '\0' ) {
    send( ch, "Look quickly at whom?\r\n" );
    return;
  }

  if( ( victim = one_character( ch, argument, "look quickly at",
    ch->array ) ) == NULL )
    return;

  show_description( ch, victim );

  return;
}


void show_inventory( char_data* ch, char_data* victim )
{
  if( ch == victim ) 
    page( ch, "You are carrying:\r\n" );
  else 
    page( ch, "%s is carrying:\r\n", victim );

  page( ch, victim->contents );

  return;
}


void show_description( char_data* ch, char_data* victim )
{
  char tmp  [ 3*MAX_STRING_LENGTH ];
  char buf  [ 3*MAX_STRING_LENGTH ];
  if( victim->shifted == NULL ) {
    if( *victim->descr->complete != '\0' ) {
      act_print( buf, victim->descr->complete, victim, NULL, NULL, NULL, NULL, NULL, NULL  );
      convert_to_ansi( ch, buf, tmp, sizeof( tmp ) );
  //    convert_to_ansi( ch, victim->descr->complete, tmp, sizeof(tmp) );
      page( ch, tmp );
    } else {
      fpage( ch, "You see nothing special about %s.  In fact, you doubt %s has any special distinguishing characteristics at all.", victim->Him_Her( ), victim->He_She( ) );
    }
  }
  else {
    if( *victim->shifted->descr->complete != '\0' ) {
      act_print( buf, victim->shifted->descr->complete, victim, NULL, NULL, NULL, NULL, NULL, NULL );
      convert_to_ansi( ch, buf, tmp, sizeof( tmp ) );
      page( ch, tmp );
    } else {
      fpage( ch, "You see nothing special about %s.  In fact you doubt %s has any special distinguising charateristics at all.", victim->Him_Her( ), victim->He_She( ) );
    }
  }

  return;
}


/*
 *   EQUIPMENT 
 */


void do_equipment( char_data* ch, char* )
{
  in_character = FALSE;
  if( ch->shifted != NULL ) {
    send( ch, "You currently are not in your natural form, how do you expect to do that?\r\n" );
    return;
  }
  show_equipment( ch, ch );
  in_character = TRUE;
}

char* obj_data :: Spaces( char_data* ch )
{
  int length;
  char* tmp = empty_string;
  char color [ TWO_LINES ] = "\0";

  sprintf( color, "%s%s", color_code( ch, this->pIndexData->color ), normal( ch ) );
  length = strlen( color );

  for( unsigned int i = length; i > 0; i++ )
    strcat( tmp, " " );

  return tmp;
}

void show_equipment( char_data* ch, char_data* victim )
{
  char*       format  = "%-22s %-42s %s\r\n";
  char*          tmp  = static_string( );
  bool         found  = FALSE;
  obj_data**    list  = (obj_data**) victim->wearing.list;
  int           i, j;

  for( i = 0; i < victim->wearing; i = j ) { 
    if( victim != ch )  
      for( j = i+1; j < victim->wearing && list[i]->position == list[j]->position; j++ )
        if( is_set( list[j]->pIndexData->extra_flags, OFLAG_COVER ) )
          i = j;

    for( j = i; j < victim->wearing && list[i]->position == list[j]->position; j++ ) {
      if( ch == victim || list[j]->Seen( ch ) ) {
        if( !found ) {
          page_centered( ch, "+++ Equipment +++" );
          sprintf( tmp, format, "Body Location", "Item", "Condition" );
          page_underlined( ch, tmp );
          found = TRUE;
        }
        page( ch, format, j == i ? where_name[ list[i]->position ] : "", list[j]->Name( ch, 1, TRUE ), list[j]->condition_name( ch, TRUE ) );
      }
    }
  }

  if( !found ) {
    if( ch == victim )
      page( ch, "You have nothing equipped.\r\n" );
    else
      page( ch, "%s has nothing equipped.\r\n", victim );
  } else {
    if( ch == victim ) 
      page( ch, "\r\nWeight: %.2f lbs\r\n", (float) ch->wearing.weight/100 );
  }
} 

/*
 *   LOOK IN OBJECT
 */


thing_data* cant( thing_data* thing, char_data*, thing_data* )
{
  return object( thing );
}


thing_data* notcontainer( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = (obj_data*) thing;

  if( is_container( obj ) || obj->pIndexData->item_type == ITEM_FOUNTAIN )
    return obj;

  return NULL;
}


thing_data* closed( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = (obj_data*) thing;

  if( obj->pIndexData->item_type == ITEM_CONTAINER && is_set( &obj->value[1], CONT_CLOSED ) )
    return NULL;

  return obj;
}
 
    
thing_data* empty( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = (obj_data*) thing;
    
  if( obj->pIndexData->item_type == ITEM_FOUNTAIN )
    return thing;

  else if( obj->pIndexData->item_type == ITEM_DRINK_CON )
    return ( obj->value[1] == 0 ?  NULL : thing );

  return( is_empty( thing->contents ) ? NULL : thing );
}


thing_data* lookin( thing_data* thing, char_data*, thing_data* )
{
  return thing;
}


void look_in( char_data* ch, char* argument )
{
  thing_array* array;

  if( *argument == '\0' ) {
    send( ch, "Look in what?\r\n" );
    return;
  }

  if( ( array = several_things( OBJ_DATA, ch, argument, "look in", ch->array, &ch->contents, &ch->wearing ) ) == NULL ) 
    return;

  thing_array   subset  [ 5 ];
  thing_func*     func  [ 5 ]  = { cant, notcontainer, closed, empty, lookin };

  sort_objects( ch, *array, NULL, 5, subset, func );

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't look in" );
  page_priv( ch, &subset[1], NULL, NULL, "isn't a container", "aren't containers" );
  page_priv( ch, &subset[2], NULL, NULL, "is closed", "are closed" );
  page_priv( ch, &subset[3], NULL, NULL, "is empty", "are empty" );

  for( int i = 0; i < subset[4]; i++ ) 
    look_in( ch, (obj_data*) subset[4].list[i] );

  delete array;
}


void look_in( char_data* ch, obj_data* obj )
{
  switch( obj->pIndexData->item_type ) {
  default:
    page( ch, "%s is not a container.\r\n", obj );
    return;

  case ITEM_SPELLBOOK :
    page( ch, "The spellbook is blank.\r\n" );
    return;

  case ITEM_DRINK_CON:
    page( ch, "It's %sfull of %s.\r\n",
      obj->value[1] < obj->pIndexData->value[0] / 2 ? "less than half " : ( obj->value[1] < 3* obj->pIndexData->value[0] / 4 ? "more than half " : "" ),
      obj->value[2] < 0 || obj->value[2] >= table_max[TABLE_LIQUID] ? "[BUG]" : liquid_table[obj->value[2]].color );
    return;

  case ITEM_FOUNTAIN:
    page( ch, "%s is full of %s.\r\n", obj->Name( ch ), obj->value[2] < 0 || obj->value[2] >= table_max[TABLE_LIQUID] ? "[BUG]" : liquid_table[obj->value[2]].color );
    return;

  case ITEM_CHARM_BRACELET :
  case ITEM_KEYRING :
  case ITEM_CONTAINER:
  case ITEM_CORPSE :
  case ITEM_TABLE :
    break; 
  }

  page( ch, "%s %s:\r\n", obj, obj->pIndexData->item_type != ITEM_TABLE ? "contains" : "has on it" );
  page( ch, obj->contents );
}


/*
 *   MAIN LOOK ROUTINE
 */


void do_look( char_data* ch, char* argument )
{
  visible_data* visible;

  if( ch->link == NULL || !ch->Can_See( TRUE ) )
    return;

  if( *argument == '\0' ) {
    show_room( ch, ch->in_room, FALSE, FALSE );
    return;
  }

  if( !strncasecmp( argument, "in ", 3 ) || !strncasecmp( argument, "i ", 2 ) ) {
    argument += ( argument[1] == 'n' ? 3 : 2 );
    look_in( ch, argument );
    return;
  }

  if( !strncasecmp( argument, "at ", 3 ) )
    argument += 3;

  if( ( visible = one_visible( THING_DATA, ch, argument, "look at",
    (visible_array*) ch->array,
    (visible_array*) &ch->contents,
    (visible_array*) &ch->wearing,  
    (visible_array*) &ch->in_room->extra_descr,
    (visible_array*) &ch->in_room->exits ) ) == NULL )
      return;

  visible->Look_At( ch );
}


void show_room( char_data* ch, room_data* room, bool brief, bool scan )
{
  if( ch == NULL || ch->pcdata == NULL || room == NULL )
    return;

  if( room->area )
    room->area->Show( room, ch, brief, scan );
  else
    roach( "room without area!  (%d)", room->vnum );
}


/*
 *   ROOM INFO BOX
 */


const char* Room_Data :: RoomFlagString( )
{
  char* tmp = static_string( );

  if( is_set( &room_flags, RFLAG_SAFE ) )
    sprintf( tmp, "safe" );
  else
    *tmp = '\0';

  if( is_set( &room_flags, RFLAG_NO_MOB ) ) 
    sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : ", ", "no.mob" );

  if( is_set( &room_flags, RFLAG_INDOORS ) ) 
    sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : ", ", "inside" );

  if( *tmp == '\0' )
    return "--";

  return tmp;
}


void Room_Data :: ShowInfo( char_data* ch )
{
  char*    tmp  = static_string( );
  bool can_see  = Seen( ch );
  int     term  = ch->pcdata->terminal;
  int   detail;
  int        i;

  const char* name = can_see ? this->name : "DARKNESS";  // was ch->in_room->name
  detail = level_setting( &ch->pcdata->pfile->settings, SET_ROOM_INFO );

  if( detail < 3 ) {
    if( term == TERM_MXP ) {
      if( /* is_apprentice( ch ) && */ detail == 2 ) {
        send( ch, "#%d : %s<rname>%s%s</rname>\r\n", vnum, MXP_SECURE, name, MXP_SECURE );
      } else {
        send( ch, "%s<rname>%s%s</rname>\r\n", MXP_SECURE, name, MXP_SECURE );
      }
    } else {
      if( /* is_apprentice( ch ) && */ detail == 2 ) {
        send_color( ch, COLOR_ROOM_NAME, "#%d : %s\r\n", vnum, name );
      } else {
        send_color( ch, COLOR_ROOM_NAME, "%s\r\n", name );
      }
    }
    if( detail >= 1 && can_see )
      autoexit( ch, detail );
    send( ch, "\r\n" );
    return;
  }

  if( term != TERM_DUMB ) {
    sprintf( tmp, "%%%ds%s%%s%s\r\n", 40-strlen( name )/2, term_table[term].codes( ch->pcdata->color[ COLOR_ROOM_NAME ] ), normal( ch ) );
    send( ch, tmp, "", name );
  } else {
    send_centered( ch, name );
  }

  send( ch, scroll_line[2] );

  send( ch, "|   Lighting: %-15s Time: %-16s  Terrain: %-13s |\r\n",
    light_name( Light( 0 ) ), is_outside( ) ? sky_state( ) : "???",
    terrain_table[ sector_type ].terrain_name );

  i = exits_prompt( tmp, ch );
  add_spaces( tmp, 12-i );

  send( ch, "|      Exits: %s Weather: %-15s Room Size: %-13s |\r\n", tmp, is_outside( ) ? "Clear" : "???", size_name[ size ] );

  if( is_apprentice( ch ) )
    send( ch, "|       Vnum: %-14d Flags: %-40s |\r\n", vnum, RoomFlagString( ) );
  
  send( ch, scroll_line[2] );
}


