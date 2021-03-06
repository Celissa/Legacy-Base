#include "system.h"


const char* oprog_trigger[ MAX_OPROG_TRIGGER ] =
{
  "put", "get", "timer", "hit", "none", "to_room", "entering", "n/a",
  "after_use", "sit", "random", "untrap", "use", "unlock", "lock", "n/a"
};


void display_oprog   ( char_data*, oprog_data* );


const char** oprog_msgs [ MAX_OPROG_TRIGGER ] =
{
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  unlock_msg,
  lock_msg,
  NULL
};


/*
 *   DISPLAY ROUTINE
 */


void oprog_data :: display( char_data* ch )
{
  send( ch, "%10s ??    %s\r\n", "Oprog", "Null Object" );
}     


/*
 *   EDITING ROUTINES
 */


void do_opedit( char_data* ch, char *argument )
{
  char                 arg  [ MAX_STRING_LENGTH ];
  obj_clss_data*  obj_clss;
  oprog_data*        oprog;
  wizard_data*      wizard;
  int                    i;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    send( "You aren't editing any object - use oedit <obj>.\r\n", ch );
    return;
  }

  if( *argument == '\0' ) {
    display_oprog( ch, obj_clss->oprog );
    return;
  }

  argument = one_argument( argument, arg );

  if( is_number( arg ) ) {
    i = atoi( arg );
    for( oprog = obj_clss->oprog; oprog != NULL && i != 1;
      oprog = oprog->next, i-- );
    if( oprog == NULL ) {
      send( ch, "No oprog by that number.\r\n" );
      return;
    }
    wizard->oprog_edit  = oprog;
    wizard->opdata_edit = NULL;
    send( ch, "You now edit that oprog.\r\n" );
    return;
  }

  if( !ch->can_edit( obj_clss ) )
    return;

  if( !strcasecmp( arg, "new" ) ) {
    oprog               = new oprog_data( obj_clss );
    wizard->oprog_edit  = oprog;
    wizard->opdata_edit = NULL;
    append( obj_clss->oprog, oprog );
    send( ch, "Oprog added.\r\n" );
    return;
  }

  if( !strcasecmp( arg, "delete" ) ) {
    if( ( oprog = wizard->oprog_edit ) == NULL ) {
      send( ch, "You aren't editing any oprog.\r\n" );
      return;
    }
    remove( obj_clss->oprog, oprog );
    extract( wizard, offset( &wizard->oprog_edit, wizard ), "oprog" );
    delete oprog;
    send( ch, "Oprog deleted.\r\n" );
    return;
  }

  send( ch, "Illegal syntax.\n" );
}


void display_oprog( char_data* ch, oprog_data *oprog )
{
  char tmp  [ MAX_INPUT_LENGTH ];
  int    i;

  if( oprog == NULL ) {
    send( ch, "This object has no programs.\r\n" );
    return;
    }

  for( i = 0; oprog != NULL; oprog = oprog->next ) {
    switch( oprog->trigger ) {
    case OPROG_TRIGGER_NONE :
      sprintf( tmp, "%20s %s", oprog->command, oprog->target );
      break;
    default :
      sprintf( tmp, "%20s", oprog_trigger[oprog->trigger] );
      break;
    }
    send( ch, "[%2d]  %s\r\n", ++i, tmp );
  }
}


void do_opcode( char_data* ch, char* argument )
{
  obj_clss_data*    obj_clss;
  oprog_data*          oprog;
  wizard_data*        wizard;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    send( ch, "You aren't editing any object.\r\n" );
    return;
  }

  if( ( oprog = wizard->oprog_edit ) == NULL ) {
    send( ch, "You aren't editing any oprog.\r\n" );
    return;
  }

  if( *argument != '\0' && !ch->can_edit( obj_clss ) )
    return;

  oprog->code = edit_string( ch, argument, oprog->code, MEM_OPROG );

  var_ch = ch;
  compile( oprog );
  var_ch = NULL;
}


void do_opdata( char_data* ch, char* argument )
{
  obj_clss_data*  obj_clss;
  oprog_data*        oprog;
  wizard_data*      wizard;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    send( ch, "You aren't editing any object.\r\n" );
    return;
  }

  if( ( oprog = wizard->oprog_edit ) == NULL ) {
    send( ch, "You aren't editing any oprog.\r\n" );
    return;
  }

  if( *argument != '\0' && !ch->can_edit( obj_clss ) )
    return;
 
  if( wizard->opdata_edit != NULL ) {
    if( !strcasecmp( argument, "exit" ) || !strcmp( argument, ".." ) ) {
      wizard->opdata_edit = NULL;
      send( ch, "Opdata now operates on the data list.\r\n" );
      return;
    }
    wizard->opdata_edit->text = edit_string( ch, argument, wizard->opdata_edit->text, MEM_EXTRA );
  } else {
    if( *argument == '\0' )
      show_defaults( ch, oprog->trigger, oprog_msgs );
    
    edit_extra( oprog->data, wizard, offset( &wizard->opdata_edit, wizard ), argument, "opdata" );
  }

  var_ch = ch;
  compile( oprog );
  var_ch = NULL;
}


void do_opstat( char_data* ch, char* )
{
  char              buf  [ 3*MAX_STRING_LENGTH ];
  oprog_data*     oprog;
  wizard_data*   wizard;

  wizard = (wizard_data*) ch;

  if( wizard->obj_edit == NULL ) {
    send( ch, "You aren't editing any object.\r\n" );
    return;
  }

  if( ( oprog = wizard->oprog_edit ) == NULL ) {
    send( ch, "You aren't editing any oprog.\r\n" );
    return;
  }

  page( ch, "%10s : %s\r\n", "Trigger", oprog_trigger[ oprog->trigger ] );
  page( ch, "%10s : %s\r\n", "Obj_Act", oprog->obj_act == NULL ? "none" : oprog->obj_act->Name( ) );

  page( ch, "%10s : %s\r\n", "Command", oprog->command );
  page( ch, "%10s : %s\r\n", "Target", oprog->target );
  page( ch, "%10s : %d\r\n\r\n", "Rand_Value", oprog->value );

  sprintf( buf, "[Code]\r\n%s\r\n", oprog->code );
  page( ch, buf );

  show_extras( ch, oprog->data );
}


void do_opset( char_data* ch, char *argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  obj_data*         obj;
  oprog_data*     oprog;
  wizard_data*   wizard;

  wizard = (wizard_data*) ch;  

  if( ( oprog = wizard->oprog_edit ) == NULL ) {
    send( ch, "You aren't editing any oprog.\r\n" );
    return;
    }

  if( *argument == '\0' ) {
    do_opstat( ch, "" );
    return;
    }

#define ot( i )  oprog_trigger[i]

  class type_field type_list[] = {
    { "trigger",   MAX_OPROG_TRIGGER,  &ot(0),  &ot(1),  &oprog->trigger },
    { "",          0,                  NULL,    NULL,    NULL            }
    };

#undef ot

  if( process( type_list, ch, "oprog", argument ) )
    return;

  if( matches( argument, "obj_act" ) ) {
    if( ( obj = one_object( ch, argument, "oprog", &ch->contents ) ) != NULL )
      oprog->obj_act = obj->pIndexData;
    else
      oprog->obj_act = NULL;
    send( ch, "Obj_act set to %s.\r\n", oprog->obj_act == NULL ? "none" : oprog->obj_act->Name( ) );
    return;
  }

  if( matches( argument, "command" ) ) {
    free_string( oprog->command, MEM_OPROG );
    oprog->command = alloc_string( argument, MEM_OPROG );
    sprintf( buf, "Command set to %s.\r\n", argument );
    send( buf, ch );
    return;
  }

  if( matches( argument, "target" ) ) {
    free_string( oprog->target, MEM_OPROG );
    oprog->target = alloc_string( argument, MEM_OPROG );
    sprintf( buf, "Target set to %s.\r\n", argument );
    send( buf, ch );
    return;
  }

  int new_value = 0;
  if( matches( argument, "rand_value" ) ) {
    new_value = atoi( argument );
    sprintf( buf, "Rand_Value set to %d [was %d].\r\n", new_value, oprog->value );
    oprog->value = new_value;
    send( buf, ch );
    return;
  } 

  send( ch, "Syntax: opset <field> <value>\r\n" );
}


void oset_obj( obj_data *obj, char *argument )
{
  char *word[] = { "value0", "value1", "value2", "value3" };
  int *pInt[] = { &obj->value[0], &obj->value[1], &obj->value[2], &obj->value[3] };

  char arg [ MAX_INPUT_LENGTH ];
  int i, j, length;
 
  argument = one_argument( argument, arg );
  length = strlen( arg );

  for( i = 0; i < 4; i++ )
    if( !strncasecmp( word[i], arg, length ) ) {
      j = atoi( argument );
      *pInt[i] = j;
      return;
    }

  bug( "Oset_obj: unknown field." ); 
}


// returns 'true' if handled (eg 'end' is called), false if not (eg 'continue' is called)
bool execute_use_trigger( char_data *ch, obj_data *obj, int type )
{
  if( !ch || !obj || type < 0 || type >= MAX_OPROG_TRIGGER )
    return false;

  for( oprog_data* oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) {
    if( oprog->trigger == type ) {
      var_ch   = ch;
      var_obj  = obj; 
      var_room = ch->in_room;
      int vnum = obj->pIndexData->vnum;
      
      if( execute( oprog ) ) {
        // execute returns 'true' on a 'continue' so we want to say 'not handled' unless the object is invalid
        if( !obj || !obj->Is_Valid() ) {
          log( "Oprog %s trigger 'continues' with NULL object %d -%s-\r\n", oprog_trigger[ type ], vnum, ch->Name( ) );
          return true;
        }
        return false;
      } else {
        // execute returns 'false' on an 'end' so we want to say 'handled'
        return true;
      }
    }
  }
  
  return false;
}


