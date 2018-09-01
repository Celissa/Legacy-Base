#include "system.h"
 

/*
 *   LOCAL FUNCTIONS
 */


void   clear_variables        ( void );
void   remove_queue  ( queue_data* );  
void*  proc_arg      ( arg_type* );

bool end_prog;
bool cont_prog;
bool queue_prog;

queue_data*  queue_list  = NULL;
stack_data*  stack_list  = NULL;


/*
 *   VARIABLES/CONSTANTS
 */


char_data*   var_mob;  
char_data*   var_ch;
char_data*   var_rch;
char_data*   var_victim;
char*        var_arg;
room_data*   var_room;
obj_data*    var_obj;
obj_data*    var_robj;
obj_data*    var_container;
int          var_i;
int          var_j;


/*
 *   MEMORY ROUTINES
 */


char* code_alloc( const char* argument )
{
  mem_block*  block;
  int          size  = strlen( argument );

  if( size == 0 )
    return empty_string;

  block       = new mem_block( size+1 );
  block->next = block_list;
  block_list  = block;

  memcpy( block->pntr, argument, size+1 );

  return (char*) block->pntr;
}


/*
 *   BUG ROUTINE
 */


void code_bug( char *text )
{
  char tmp [ MAX_INPUT_LENGTH ];

  bug( text );

  sprintf( tmp, "-- Char = %s  Mob = %s  Victim = %s  Rch = %s",
    var_ch  == NULL    ? "NULL" : *var_ch->descr->name == '\0' ? var_ch->descr->singular : var_ch->descr->name,
    var_mob == NULL    ? "NULL" : *var_mob->descr->name == '\0' ? var_mob->descr->singular : var_mob->descr->name,
    var_victim == NULL ? "NULL" : *var_victim->descr->name == '\0' ? var_victim->descr->singular : var_victim->descr->name,
    var_rch == NULL ? "NULL" : *var_rch->descr->name == '\0' ? var_rch->descr->singular : var_rch->descr->name );
  bug( tmp );

  sprintf( tmp, "-- Obj = %s  Robj = %s  Container = %s",
    var_obj == NULL ? "NULL" : var_obj->singular,
    var_robj == NULL ? "NULL" : var_robj->singular,
    var_container == NULL ? "NULL" : var_container->singular );
  bug( tmp );

  sprintf( tmp, "-- Room #%d, i = %d", var_room == NULL ? -1 : var_room->vnum, var_i );  
  bug( tmp );
}


/*
 *   STACK ROUTINES
 */


void push( )
{
  stack_data* stack = new stack_data;
  
  stack->mob        = var_mob;
  stack->ch         = var_ch;
  stack->rch        = var_rch;
  stack->victim     = var_victim;
  stack->arg        = var_arg;
  stack->room       = var_room;
  stack->obj        = var_obj;
  stack->robj       = var_robj;
  stack->container  = var_container;
  stack->i          = var_i;
  stack->j          = var_j;

  stack->next       = stack_list;
  stack_list        = stack;
}


void pop( )
{
  if( !stack_list )
    return;

  stack_data* stack = stack_list;
  stack_list        = stack->next;

  var_mob           = stack->mob;
  var_ch            = stack->ch;
  var_rch           = stack->rch;
  var_victim        = stack->victim;
  var_arg           = stack->arg;
  var_room          = stack->room;
  var_obj           = stack->obj;
  var_robj          = stack->robj;
  var_container     = stack->container;
  var_i             = stack->i;
  var_j             = stack->j;

  delete stack;
}


/*
 *   QUEUE ROUTINES
 */


void do_ps( char_data* ch, char* )
{
  queue_data* queue;

  if( queue_list == NULL ) {
    send( ch, "The queue is empty.\r\n" );
    return;
  }

  page_underlined( ch, "Vnum     Type        Location\r\n" );   

  for( queue = queue_list; queue != NULL; queue = queue->next )
    queue->program->display( ch );
}  


void update_queue( void )
{
  queue_data *queue, *queue_next;
//  mem_block *tmp_list = block_list;

  for( queue = queue_list; queue != NULL; queue = queue_next ) {
    queue_next = queue->next;

    if( queue->time-- > 1 )
      continue;

    var_room = queue->room;
    var_ch   = queue->ch;
    var_mob  = queue->mob;
    var_obj  = queue->obj;
    var_i    = queue->i;

    error_buf[0] = '\0';
    end_prog     = FALSE;
    cont_prog    = FALSE;
    queue_prog   = FALSE;

    proc_arg( queue->arg->next );

    if( queue_prog ) 
      queue_list->program = queue->program;
    else 
      if( queue->program != NULL )
        queue->program->active--;
         
    remove( queue_list, queue );
    delete queue;
  }

  clear_variables( );
}


/*
 *   CLEAR QUEUE OF A VARIABLE
 */


void clear_queue( char_data* ch )
{
  queue_data* queue;
  stack_data* stack;

  for( queue = queue_list; queue != NULL; queue = queue->next ) {
    if( queue->mob == ch )
      queue->mob = NULL;
    if( queue->ch == ch )
      queue->ch = NULL;
  }

  if( var_mob == ch )
    var_mob = NULL;
  
  if( var_ch == ch )
    var_ch = NULL;
  
  if( var_victim == ch )
    var_victim = NULL;

  if( var_rch == ch )
    var_rch = NULL;

  for( stack = stack_list; stack != NULL; stack = stack->next ) {
    if( stack->ch == ch )
      stack->ch = NULL;
    if( stack->victim == ch )
      stack->victim = NULL;
    if( stack->rch == ch )
      stack->rch = NULL;
    if( stack->mob == ch )
      stack->mob = NULL;
  } 
}


void clear_queue( obj_data* obj )
{
  queue_data* queue;
  stack_data* stack;

  for( queue = queue_list; queue != NULL; queue = queue->next ) {
    if( queue->obj == obj )
      queue->obj = NULL;
  }

  if( var_obj == obj )
    var_obj = NULL;
  if( var_robj == obj )
    var_robj = NULL;
  if( var_container == obj )
    var_container = NULL;

  for( stack = stack_list; stack != NULL; stack = stack->next ) {
    if( stack->obj == obj )
      stack->obj = NULL;
    if( stack->robj == obj )
      stack->robj = NULL;
    if( stack->container == obj )
      stack->container = NULL;
  }
}


void clear_queue( program_data* program )
{
  queue_data*  queue;
  queue_data*   next;

  for( queue = queue_list; queue != NULL; queue = next ) {
    next = queue->next; 
    if( queue->program == program )  
      remove( queue_list, queue );
  }

  program->active = 0;
}


/*
 *   RUN-TIME FUNCTIONS 
 */


bool execute( program_data* program )
{
  bool flag;

  if( program->binary == NULL ) {
    compile( program );
    if( program->binary == NULL ) {
      delete_list( program->memory );
      return FALSE;
    }
  }

  if( program->active > 50 ) {
    code_bug( "Execute: infinite recursive loop." );
    return FALSE;
  }

  end_prog   = FALSE;
  cont_prog  = FALSE;
  queue_prog = FALSE;

  program->active++;
  proc_arg( program->binary );

  if( queue_prog ) 
    queue_list->program = program;
  else 
    program->active--;

  flag = cont_prog;

  end_prog   = FALSE;
  cont_prog  = FALSE;
  queue_prog = FALSE;

  clear_variables( );

  return flag;
}


void* proc_arg( arg_type* arg )  
{
  afunc_type*    afunc;
  arg_type*   arg_next;
  void**          pntr;
  void*         result;
  int                i;

  if( end_prog )
    return NULL;  

  if( arg == NULL )
    return NULL;

  arg_next = arg->next;

  if( arg_next != NULL ) {
    arg->next = NULL;
    proc_arg( arg );
    arg->next = arg_next;
    if( end_prog )
      return NULL;
    proc_arg( arg_next );
    return NULL;
  }

  if( arg->family == cont ) {
    cont_prog = TRUE;
    end_prog  = TRUE;
    return NULL;
  }

  if( arg->family == end ) {
    end_prog = TRUE;
    return NULL;
  }

  if( arg->family == constant ) 
    return arg->value;

  if( arg->family == variable ) {
    pntr = (void**) arg->value;
    if( arg->neg )
      return *pntr == NULL ? (void*) true : NULL;
    else
      return *pntr;

    /*DME> arg2 of the conditional was originally "*pntr == NULL" which evaluates to an integer. */
    /*DME> Changed to return NULL, since it is supposed to return a pointer.                     */
    // changed back
  }

  if( arg->family == loop ) {
    loop_type*      aloop  = (loop_type*) arg;
   
    if( aloop->fruit == loop_unknown ) {
      for( i = 0; i < 100; i++ ) {
        if( ( proc_arg( aloop->condition ) == NULL ) != aloop->neg )
          break;
        proc_arg( aloop->aloop );
      }
      if( i == 100 )
        bug( "Proc_arg: Infinite script loop." );
      return NULL;
    }    

    if( var_room == NULL ) {
      code_bug( "Proc_arg: NULL room in loop." );
      return NULL;
    }

    if( aloop->fruit == loop_objs ) {
      // loop thru objects
      obj_data* robj;

      content_array& content_ref = var_room->contents;
      for( i = content_ref.size - 1; i >= 0; i-- ) {
        if( i >= content_ref.size )
          continue;
        robj = object( content_ref.list[i] );
        if( robj == NULL || !robj->Is_Valid( ) )
          continue;
        var_robj = robj;
        proc_arg( aloop->aloop ); // note that this can reset the var_ variables
      }
    } 

/*    
    else if( aloop->fruit == loop_enemies ) {
      char_data* rch;
      char_data*  ch = var_ch;

      content_array& content_ref = var_room->contents;
      for( i = content_ref.size-1; i >= 0; i-- ) {
        if( i >= content_ref.size )
          continue;
        rch = character( content_ref.list[i] );
        if( rch == NULL || !rch->Is_Valid( ) || !are_fighting( ch, rch ) || !var_ch->Seen( rch ) )
          continue;
        var_rch = rch;
        proc_arg( aloop->aloop );
      }
    }
*/

    else {
      // must be loop followers of some kind
      char_data*        rch;
      char_data*     leader  = var_ch;

      content_array& content_ref = var_room->contents;
      for ( i = content_ref.size - 1; i >= 0; i-- ) {
        if ( i >= content_ref.size )
          continue;
        rch = character( content_ref.list[i] );
        if( rch == NULL || !rch->Is_Valid( ) || ( rch->species == NULL && is_set( rch->pcdata->pfile->flags, PLR_WIZINVIS ) ) || ( aloop->fruit == loop_followers && rch->leader != leader ) || ( aloop->fruit == loop_pets && rch->species != NULL && !is_set( &rch->status, STAT_PET ) ) )
          continue; 
        var_rch = rch;
        proc_arg( aloop->aloop ); // note that this can reset the var_ variables
      }
    }

    return NULL;
  }

  if( arg->family == if_clause ) {
    aif_type*   aif  = (aif_type*) arg; 
    void*      flag  = proc_arg( aif->condition );

    if( flag == NULL )
      proc_arg( aif->no );
    else
      proc_arg( aif->yes );

    return NULL;
  }

  afunc = (afunc_type*) arg;

  void** farg = new void*[ afunc->func->args.size ];
  for( i = 0; i < afunc->func->args.size; i++ )
    farg[ i ] = NULL; 

  // special case for =, += and -=
  if( afunc->func->func_call == code_set_equal || afunc->func->func_call == code_plus_equal || afunc->func->func_call == code_minus_equal ) {
    farg[ 0 ] = afunc->args[ 0 ];
    farg[ 1 ] = proc_arg( afunc->args[ 1 ] );
    ( afunc->func->func_call )( farg );
    delete [] farg;
    return NULL;
  }

  for( i = 0; i < afunc->args; i++ )
    farg[ i ] = proc_arg( afunc->args[ i ] );

  curr_arg = arg;
  result   = ( afunc->func->func_call )( farg );

  delete [] farg;

  if( !arg->neg )
    return result;

  return (void*) ( result == NULL );
}


/*
 *   MISC SUPPORT ROUTINES
 */


char* get_string( const char* name, extra_array& list )
{
  for( int i = 0; i < list; i++ )
    if( !strcasecmp( name, list[i]->keyword ) )
      return list[i]->text;

  return NULL;
}


void clear_variables( )
{
  var_mob       = NULL;
  var_ch        = NULL;
  var_rch       = NULL;
  var_victim    = NULL;
  var_arg       = NULL;
  var_room      = NULL;
  var_obj       = NULL;
  var_robj      = NULL;
  var_container = NULL;
  var_i         = -1;
  var_j         = -1;
} 



  

