#include "system.h"


/*
 *   ACT
 */

void act( char_data* to, const char* text, visible_data* ch, visible_data* obj, const char* string1, const char* string2 )
{
  char  tmp  [ 3*MAX_STRING_LENGTH ];
  char  buf  [ 3*MAX_STRING_LENGTH ];

  if( to == NULL || to->link == NULL || !text || *text == '\0' )
    return;
  
  act_print( buf, text, ch, NULL, obj, NULL, string1, string2, to );   
  convert_to_ansi( to, buf, tmp, sizeof(tmp) );
  send( to, tmp );
}

void act( char_data* to, const char* text, visible_data* ch, visible_data* victim, visible_data* obj1, visible_data* obj2, int sense )
{
  char  tmp  [ 3*MAX_STRING_LENGTH ];
  char  buf  [ 3*MAX_STRING_LENGTH ];

  if( to == NULL || to->link == NULL || !text || *text == '\0' )
    return;

  if( sense != VIS_ALL ) {
    if( to->position == POS_SLEEPING && sense != VIS_TOUCH )
      return;

    if( is_set( to->affected_by, AFF_BLIND ) && sense != VIS_SOUND && sense != VIS_TOUCH && sense != VIS_SIGHT_SOUND )
      return;

    if( is_set( to->affected_by, AFF_SILENCE ) && sense != VIS_SIGHT && sense != VIS_TOUCH && sense != VIS_SIGHT_SOUND )
      return;

    if( sense == VIS_SIGHT_SOUND && is_set( to->affected_by, AFF_SILENCE ) && is_set( to->affected_by, AFF_BLIND ) )
      return;
  }
  
  act_print( buf, text, ch, victim, obj1, obj2, NULL, NULL, to );   
  convert_to_ansi( to, buf, tmp, sizeof(tmp) );
  send( to, tmp );
}


/*
 *   ACT_AREA
 */


void act_area( const char* text, char_data* ch, char_data* victim, visible_data* obj, int sense )
{
  room_data*  room;
  char_data*   rch;
  obj_data*   obj2 = NULL;

  if( !ch || !ch->array || ( room = Room( ch->array->where ) ) == NULL )
    return;

  for( room = room->area->room_first; room != NULL; room = room->next ) 
    if( room != ch->array->where )
      for( int i = 0; i < room->contents; i++ ) 
        if( ( rch = character( room->contents[i] ) ) != NULL )
          act( rch, text, ch, victim, obj, obj2, sense );
}


/*
 *   ACT_NOTCHAR
 */

void act_notchar( const char* text, char_data* ch, visible_data* victim, visible_data* obj1, visible_data* obj2, int sense )
{
  char_data* rch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && rch->Accept_Msg( ch ) ) 
      act( rch, text, ch, victim, obj1, obj2, sense);
}


void act_notchar( const char* text, char_data* ch, const char* string1, const char* string2 )
{
  char_data* rch;

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && rch->position > POS_SLEEPING && rch->Accept_Msg( ch ) )
      act( rch, text, ch, NULL, string1, string2 );
}

void act_notchar( const char* text, char_data* ch, visible_data* obj, const char* string1, const char* string2 )
{
  char_data* rch;

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && rch->position > POS_SLEEPING && rch->Accept_Msg( ch ) )
      act( rch, text, ch, obj, string1, string2 );
}



/* 
 *   ACT_ROOM
 */


void act_room( room_data* room, const char* text, const char* string1, const char* string2 )
{
  char_data* rch;

  for( int i = 0; i < room->contents; i++ )
    if( ( rch = character( room->contents[i] ) ) != NULL && rch->position > POS_SLEEPING ) // what about the blind?
      act( rch, text, NULL, NULL, string1, string2 );
}

void act_room( room_data* room, const char* text, visible_data* ch, visible_data* obj1, int sense )
{
  char_data* rch;

  for( int i = 0; i < room->contents; i++ )
    if( ( rch = character( room->contents[i] ) ) != NULL )
      act( rch, text, ch, NULL, obj1, NULL, sense );
}


/*
 *   ACT_NEITHER
 */


void act_neither( const char* text, char_data* ch, char_data* victim, visible_data* obj1, visible_data* obj2 )
{
  char_data* rch;
 
  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && rch != victim && rch->position > POS_SLEEPING && rch->Accept_Msg( ch ) )
      act( rch, text, ch, victim, obj1, obj2 );
}


/*
 *  MAIN PRINT ROUTINE FOR ACT
 */


void act_print( char* out, const char* in, visible_data* ch1, visible_data* ch2, visible_data* o1, visible_data* o2,
                const char* string1, const char* string2, char_data* to )
{
  char_data*       ch = NULL;
  char_data*   victim = NULL;
  obj_data*      obj1 = NULL;
  obj_data*      obj2 = NULL;

  const char*     sub;
  char*         start = out;
  *out = '\0';

  if( !in || !*in )
    return;

  // attempt to find appropriate vars
  ch = character( ch1 );
  if( !ch ) {
    ch = character( ch2 );
    if( !ch ) {
      ch = character( o1 );
      if( !ch ) {
        ch = character( o2 );
      } else {
        victim = character( o2 );
      }
    } else {
      victim = character( o1 );
      if( !victim ) {
        victim = character( o2 );
      }
    }
  } else {
    victim = character( ch2 );
    if( !victim ) {
      victim = character( o1 );
      if( !victim ) {
        victim = character( o2 );
      }
    }
  }
  
  obj1 = object( ch1 );
  if( !obj1 ) {
    obj1 = object( ch2 );
    if( !obj1 ) {
      obj1 = object( o1 );
      if( !obj1 ) {
        obj1 = object( o2 );
      } else {
        obj2 = object( o2 );
      }
    } else {
      obj2 = object( o1 );
      if( !obj2 ) {
        obj2 = object( o2 );
      }
    }
  } else {
    obj2 = object( ch2 );
    if( !obj2 ) {
      obj2 = object( o1 );
      if( !obj2 ) {
        obj2 = object( o2 );
      }
    }
  }

  for( ; ; ) {
    if( *in != '$' ) {
      *out++ = *in;
    } else {
      in++;
      char_data* pers = islower( *in ) ? ch : victim ;
      obj_data*   obj = islower( *in ) ? obj1 : obj2 ;

      switch( toupper( *in ) ) {
      default:
        sub = "[BUG]";
        break;
      
      case '$':
        sub = "$";
        break;

      // raw names
      case '1':
        sub = ( ch1 == NULL ? "[BUG]" : ch1->Name( to, ch1->selected ) );
        break;
      
      case '2':
        sub = ( ch2 == NULL ? "[BUG]" : ch2->Name( to, ch2->selected ) );
        break;

      case '3':
        sub = ( o1 == NULL ? "[BUG]" : o1->Name( to, o1->selected ) );
        break;

      case '4':
        sub = ( o2 == NULL ? "[BUG]" : o2->Name( to, o2->selected ) );
        break;

      case 'N':
        sub = ( pers == NULL ? "[BUG]" : pers->Name( to ) );
        break;
      
      case 'R':
        sub = ( pers == NULL ? "[BUG]" : pers->real_name( ) );
        break;

      case 'O':
        sub = ( obj == NULL ? "[BUG]" : obj->Name( to, obj->selected ) );
        break;

      case 'P':
        in_character = FALSE;
        sub = ( obj == NULL ? "[BUG]" : obj->Name( to, obj->selected ) );
        in_character = TRUE;
        break;
      
      case 'D' :
      case 'T' :
        sub = ( *in == 't' ? string1 : string2 );
        if( sub == NULL )
          sub = "[BUG]";
        break;
      
      case 'G':
        sub = ( pers == NULL ? "[BUG]" : pers->Deity( to ) );
        break;
      
      case 'E':
        sub = ( pers == NULL ? "[BUG]" : pers->He_She( to ) );
        break;
      
      case 'M':
        sub = ( pers == NULL ? "[BUG]" : pers->Him_Her( to ) );
        break;
      
      case 'S':
        sub = ( pers == NULL ? "[BUG]" : pers->His_Her( to ) );
        break;

      case 'L':
        sub = ( pers == NULL ? "[BUG]" : pers->Lad_Lass( to ) );
        break;

      case 'I':
        sub = ( pers == NULL ? "[BUG]" : pers->Sir_Maam( to ) );
        break;

      case 'B':
        sub = ( pers == NULL ? "[BUG]" : pers->Boy_Girl( to ) );
        break;

      case 'W':
        sub = ( pers == NULL ? "[BUG]" : pers->Man_Woman( to ) );
        break;

      }
      
      strcpy( out, sub );
      out += strlen( sub );
    }
    
    if( *in++ == '\0' )
      break;
  }

  if( *start == '@' ) { // allow for one color code at start of line
     *(start+2) = toupper( *(start+2) );
  } else { // normal case
    *start = toupper( *start );
  }

  out--;
  while( out > start && ( *out == '\n' || *out == '\r' || *out == '\0' ) )
    *out-- = '\0';

  strcpy( ++out, "\r\n" );
}
