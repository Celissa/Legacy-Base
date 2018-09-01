#include "system.h"


/*
 *   APPEARANCE
 */


bool char_data :: look_same( thing_data* t1, thing_data* t2 )
{
  obj_data*    obj1;
  obj_data*    obj2;
  char_data*    ch1;
  char_data*    ch2;

  if( ( obj1 = object( t1 ) ) != NULL
    && ( obj2 = object( t2 ) ) != NULL )
    return look_same( obj1, obj2 );

  if( ( ch1 = character( t1 ) ) != NULL
    && ( ch2 = character( t2 ) ) != NULL )
    return look_same( ch1, ch2 );
 
  return FALSE;
}


bool none_shown( thing_array& array )
{
  for( int i = 0; i < array; i++ )
    if( array[i]->shown > 0 )
      return FALSE;
  
  return TRUE;
}  


thing_data* one_shown( thing_array& array )
{
  int j = -1;

  for( int i = 0; i < array; i++ )
    if( array[i]->shown > 0 )
      if( j == -1 ) 
        j = i;
      else
        return NULL;

  return( j == -1 ? NULL : array[j] );
}  


const char* list_name( char_data* ch, thing_array* array, bool use_and ) 
{
  static char tmp[THREE_LINES]; 

  thing_data*   last  = NULL;
  thing_data*  first  = NULL;
  thing_data*  thing;

  rehash( ch, *array );

  for( int i = 0; i < *array; i++ )
    if( array->list[i]->shown > 0 ) { 
      first = array->list[i];
      break;
    }

  if( first == NULL )
    return "nothing??";

  for( int i = *array-1; i >= 0; i-- )
    if( array->list[i]->shown > 0 ) { 
      last = array->list[i];
      break;
    }

//  char* tmp = static_string( );
  
  *tmp = '\0'; 

  for( int i = 0; i < *array; i++ ) {
    thing = array->list[i];
    if( thing->shown > 0 ) 
      sprintf( tmp+strlen( tmp ), "%s%s", thing == first ? "" : ( thing == last && use_and ? " and " : ", " ), thing->Name( ch, thing->shown ) );
  }

  return tmp;
}    
         

/*
 *   REMOVAL ROUTINES
 */


const char* Visible_Data :: Keywords( char_data* )
{
  return "** BUG **";
}


const char* Visible_Data :: Name( char_data*, int, bool )
{
  return "** BUG **";
}


const char* thing_data :: Seen_Name( char_data*, int, bool )
{
  return "** BUG **";
}


const char* thing_data :: Long_Name( char_data*, int )
{
  return "** BUG **";
}


const char* thing_data :: Show( char_data*, int )
{
  return "** BUG **";
}


bool Visible_Data :: Seen( char_data* )
{
  return TRUE;
}


void Visible_Data :: Look_At( char_data* ch )
{
  send( ch, "Whatever you are looking at is very bugged.\r\n" );
}


/*
 *   EXTRACTION
 */


void extract( thing_array& array )
{
  for( int i = array-1; i >= 0; i-- )
    array.list[i]->Extract( );
}

/*
 *    VARIABLES
 */

Thing_Data :: ~Thing_Data( )
{
  valid = -2;

  while( variables.headitem( ) ) {
    variable_data* temp = variables.headitem( );
    variables.remove( temp );
    delete temp;
  }
}

Variable_Data :: Variable_Data( const char* nName, const int nValue )
{
  name = NULL;
  value = 0;

  set_name( nName );
  set_value( nValue );
}

Variable_Data :: ~Variable_Data( )
{
  if( name )
    delete [] name;
  name = NULL;
}

variable_data* find_var( variable_array* varray, const char* name )
{
  variable_nav list( varray );
  for( variable_data* var = list.first( ); var; var = list.next( ) )
    if( !str_cmp( name, var->get_name( ) ) )
      return var;

  return NULL;
}

int Thing_Data :: Get_Integer( const char* name )
{
  variable_data* var = find_var( &variables, name );
  
  if( var )
    return var->get_value( );

  return 0;
}

void Thing_Data :: Set_Integer( const char* name, const int value )
{
  variable_data* var = find_var( &variables, name );

  if( var ) {
    if( value == 0 ) {
      variables.remove( var );
      delete var;
    } 
  else {
      var->set_value( value );
    }
  } 
  else {
    var = new variable_data( name, value );
    variables.add( var );
  }
}



