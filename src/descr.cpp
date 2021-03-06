#include "system.h"


void  send_descr        ( char_data*, const char* );
void  extract           ( extra_data*, wizard_data*, char );


/*
 *   READ/WRITE EXTRAS
 */


void read_extra( FILE* fp, extra_array& list )
{
  extra_data*   extra;
  char         letter;

  for( ; ; ) {
    if( ( letter = fread_letter( fp ) ) != 'E' )
      break;
    extra           = new extra_data;
    extra->keyword  = fread_string( fp, MEM_EXTRA );
    extra->text     = fread_string( fp, MEM_EXTRA );
    list += extra;
  }

  if( letter != '!' )
    ungetc( letter, fp );
}


void write_extras( FILE* fp, extra_array& list )
{
  for( int i = 0; i < list; i++ ) {
    fprintf( fp, "E\n" );
    fprintf( fp, "%s~\n", list[i]->keyword );
    fprintf( fp, "%s~\n", list[i]->text );
  }
}


/* 
 *   SUPPORT ROUTINES
 */


void copy_extras( extra_array& dest, extra_array &source )
{
  if( is_empty( source ) )
    return;

  for( int i = 0; i < source; i++ ) {
    extra_data* extra = new extra_data;
    extra->keyword    = alloc_string( source[ i ]->keyword, MEM_EXTRA );
    extra->text       = alloc_string( source[ i ]->text, MEM_EXTRA );
    dest += extra;
  }

  return;
}


void show_extras( char_data* ch, extra_array& list )
{
  char tmp [ 3*MAX_STRING_LENGTH ];

  if( is_empty( list ) )
    return;

  page( ch, "\r\nExtra Descriptions:\r\n" );

  for( int i = 0; i < list; i++ ) {
    sprintf( tmp, "[ %s ]\r\n%s", list[i]->keyword, list[i]->text );
    page( ch, tmp );
  }

  return;
}


void page_descr( char_data* ch, const char* text )
{
  char tmp  [ 3*MAX_STRING_LENGTH ];

  convert_to_ansi( ch, text, tmp, sizeof(tmp) );
  page( ch, tmp );

  return;
}


/*
 *   SUBROUTINES
 */


const char* Extra_Data :: Keywords( char_data* )
{
  return keyword;
}


void Obj_Data :: Look_At( char_data* ch )
{
  page_descr( ch, is_set( extra_flags, OFLAG_IDENTIFIED ) 
    ? after_descr( pIndexData ) : before_descr( pIndexData ) ); 

  if( pIndexData->item_type == ITEM_ARMOR || pIndexData->item_type == ITEM_SHIELD ) {
    page( ch, "\r\n  Condition: %s\r\n", condition_name( ch, TRUE ) );
  } else if( pIndexData->item_type == ITEM_REAGENT ) {
    page( ch, "Charges: %d", value[0]+number_range( -1, 1 ) );
  } else if( pIndexData->item_type == ITEM_WEAPON ) {
    page( ch, "\r\n    Condition: %s\r\n Weapon Class: %s\r\n", condition_name( ch, TRUE ), weapon_class( pIndexData ) );
  } else if( pIndexData->item_type == ITEM_DECK_CARDS ) { 
    look_cards( ch, this );
  } else if( pIndexData->item_type == ITEM_KEYRING ) {
    look_in( ch, this );
  } else if( pIndexData->item_type == ITEM_DRINK_CON ) {
    page( ch, "\r\nVolume: %.2f liters\r\n",
      float( value[0]/100. ) );
  } else if( pIndexData->item_type == ITEM_CONTAINER ) {
    if( !is_set( &value[1], CONT_CLOSED ) ) {
      page( ch, "\r\nCapacity: %.2f lbs\r\n", float( pIndexData->value[0] ) );
      page( ch, "Contains: %.2f lbs\r\n", contents.weight/100. );
    }
  }
}


const char* before_descr( obj_clss_data* obj )
{
  extra_data* ed;

  if( ( obj = get_obj_index( obj->fakes ) ) == NULL )
    return "Item is bugged - Fakes non-existent object.\r\n"; 

  for( int i = 0; i < obj->extra_descr; i++ ) {
    ed = obj->extra_descr[i]; 
    if( !strcmp( ed->keyword, "either" ) || !strcmp( ed->keyword, "before" ) )
      return ed->text;
  }

  return "Item is bugged - Missing non-identified description.\r\n";
}


const char* after_descr( obj_clss_data* obj )
{
  extra_data*  ed;

  for( int i = 0; i < obj->extra_descr; i++ ) {
    ed = obj->extra_descr[i];
    if( !strcmp( ed->keyword, "either" ) || !strcmp( ed->keyword, "after" ) )
      return ed->text;
  }

  return "Item is bugged - Missing identified description.\r\n";
}



/*
 *   EDITING ROUTINE
 */


void edit_extra( extra_array& array, wizard_data* wizard, int offset, char* argument, char* text )
{
  char            arg  [ MAX_INPUT_LENGTH ];
  extra_data*      ed;
  extra_data**   edit;
  int               i;

  edit = (extra_data**) ( int( wizard )+offset );

  if( *argument == '\0' ) {
    if( is_empty( array ) ) {
      send( wizard, "No extras found.\r\n" );
    } else {
      for( i = 0; i < array; i++ ) {
        page( wizard, "[%2d]  %s\r\n", i+1, array[i]->keyword );
        page( wizard, array[i]->text );
      }
    }
    return;
  }

  if( exact_match( argument, "delete" ) ) {
    if( ( i = atoi( argument )-1 ) < 0 || i >= array ) {
      send( wizard, "Extra not found to remove.\r\n" );
    } else {
      send( wizard, "Extra %d, '%s' removed.\r\n", i+1, array.list[i]->keyword );
      remove( array, i );
      *edit = array.list[i]; 
      extract( wizard, offset, text );
    }
    return;
  }

  if( exact_match( argument, "new" ) ) {
    ed          = new extra_data;
    ed->keyword = alloc_string( argument, MEM_EXTRA );
    ed->text    = alloc_string( "No description.\r\n", MEM_EXTRA );
    *edit       = ed; 
    array += ed;
    send( wizard, "Extra %s created.\r\n", ed->keyword );
    return;
  }

  argument = one_argument( argument, arg );

  if( ( i = atoi( arg )-1 ) < 0 || i >= array ) {
    for( i = 0; ; i++ ) {
      if( i == array ) {
        send( wizard, "Extra not found.\r\n" );
        return;
      }
      if( is_name( arg, array[i]->keyword ) ) 
        break;
    }
  }

  ed = array[i]; 

  if( *argument != '\0' ) {
    free_string( ed->keyword, MEM_OBJ_CLSS );
    ed->keyword = alloc_string( argument, MEM_OBJ_CLSS );
    send( wizard, "Extra name set to: %s\r\n", ed->keyword );
    return;
  }

  *edit = ed;

  send( wizard, "%s now operates on extra %d, '%s'.\r\n", text, i+1, ed->keyword );
}

