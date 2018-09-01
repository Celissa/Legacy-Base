#include "system.h"


arg_type*     curr_arg;
mem_block*  block_list  = NULL;

char         error_buf  [ MAX_INPUT_LENGTH ];
char*             code = NULL;
extra_array*      data;


/*
 *   COMPILER ROUTINES
 */


void compile( program_data* program )
{
  arg_type*         arg;
  arg_type*    arg_list  = NULL;
  arg_type*    arg_last  = NULL;
  mem_block*       list;
  mem_block*   tmp_list;
  int              line;
  char*          letter;

  clear_queue( program );
  delete_list( program->memory );  

  if( program->binary != NULL ) {
    delete program->binary;
    program->binary = NULL;
  }
 
  program->memory = NULL;

  tmp_list    = block_list;
  data        = &program->data;
  block_list  = NULL;

  *error_buf  = '\0';

  if( program->code == empty_string )
    return;

  char* pcode = alloc_string( program->code, MEM_PROGRAM );
  char* read  = pcode;
  char* write = pcode;

  while( *read ) {
    if( !strncmp( read, "//", 2 ) ) {
      while( *read && *read != '\n' )
        read++;
      if( *read == '\n' )
        read++;
      *write++ = '\r';
      *write++ = '\n';
    } else if( !strncmp( read, "/*", 2 ) ) {
      read++;
      while( *read && strncmp( read, "*/", 2 ) )
        read++;
      if( *read == '*' )
        read++;
      if( *read == '/' )
        read++;
    } else
      *write++ = *read++;
  }

  *write++ = '\0';

  code = pcode;

  for( line = 1; *error_buf == '\0'; ) {
    if( ( arg = read_op( read_arg( ) ) ) == NULL )
      break; 

    line += 2; // was commented
   
    if( *code == ';' )
      code++;

    if( arg_list == NULL )
      arg_list = arg;
    else
      arg_last->next = arg;

    arg_last = arg;
  }

  list       = block_list;
  block_list = tmp_list;

  if( *error_buf != '\0' ) { 
    for( line = 1, letter = code; letter != code; letter++ )
      if( /* *letter == '\r' || */ *letter == '\n' )
        line += 2;
    program->corrupt = TRUE;
    page_header( var_ch, "*** %s\r\n\r\n", error_buf );
    page_header( var_ch, "*** Error on line %d\r\n", line );
    page_header( var_ch, "*** FAILS TO COMPILE\r\n" );
    delete_list( list );
    if( arg_list != NULL )
      delete arg_list;
    free_string( pcode, MEM_PROGRAM );
    return;
  }

  program->binary  = arg_list;
  program->memory  = list;
  program->active  = 0;
  program->corrupt = FALSE;

  free_string( pcode, MEM_PROGRAM );
}


/*
 *   READ_ARG
 */


arg_type* read_arg( arg_enum expected )
{
  arg_type*    arg;
  arg_type*   arg1;
  arg_type*   arg2;
  bool         neg;

  if( *error_buf != '\0' )
    return NULL;

  skip_spaces( code );

  if( *code == '\0' ) 
    return NULL;

  if( *code == '{' ) {
    code++;
    if( ( arg1 = read_op( read_arg( expected ) ) ) == NULL )
      return NULL;
    for( arg2 = arg1; ; arg2 = arg2->next ) {
      skip_spaces( code );
      if( *code == '}' ) {
        code++;
        return arg1;
      }

      if( ( arg2->next = read_op( read_arg( expected ) ) ) == NULL ) {
        if( *error_buf == '\0' )
          strcpy( error_buf, "End of statement block without }." );
        delete arg1;
        return NULL;
      }
    } 
  }       

  if( ( arg = read_const( expected, code ) ) != NULL )
    return arg;
        
  if( exact_match( code, "end" ) ) {
    arg         = new arg_type;
    arg->family = end;
    return arg;
  }

  if( exact_match( code, "continue" ) ) {
    arg         = new arg_type;
    arg->family = cont;
    return arg;
  }

  if( exact_match( code, "loop(" ) )
    return read_loop( code );

  if( exact_match( code, "if(" ) )
    return read_if( code );

  if( neg = ( *code == '!' ) )
    code++;

  if( ( arg = read_function( code, neg ) ) != NULL || *error_buf != '\0' )
    return arg;

  if( ( arg = read_variable( code, neg ) ) != NULL || *error_buf != '\0' )
    return arg;

  if( neg )
    code--;

  if( isdigit( *code ) || ( *code == '-' && isdigit( *(code+1) ) ) ) 
    return read_digit( code );

  if( ( arg = read_string( code, *data ) ) != NULL || *error_buf != '\0' )
    return arg;
  
  if( *error_buf == '\0' ) 
    strcpy( error_buf, "Error of unknown type." );

  return NULL;
}


/*
 *   OPERATORS
 */


bool can_assign( arg_enum t1, arg_enum t2 )
{
  if( t1 == INTEGER && t2 != CHARACTER && t2 != OBJECT && t2 != ROOM && t2 != THING )
    return TRUE;

  if( t1 == THING )
    return( t2 == CHARACTER || t2 == OBJECT || t2 == ROOM || t2 == THING );

  return( t1 == t2 );  
}


arg_type* read_op( arg_type* arg )
{
  afunc_type*  afunc;
  arg_type*     targ;
  int              i;

  if( arg == NULL || arg->family == if_clause || arg->family == loop )
    return arg; 

  skip_spaces( code );

  if( *code == ')' || *code == ',' )
    return arg;

  if( *code == ';' ) {
    code++;
    return arg;
  }

  afunc         = new afunc_type;     
  afunc->type   = INTEGER;
  afunc->args  += arg;

  for( i = 0; ; i++ ) {
    if( *op_list[i].name == '\0' ) {
      strcpy( error_buf, "Unknown operator or missing semi-colon." );
      delete afunc;
      return NULL;
    }
    if( exact_match( code, op_list[i].name ) ) 
      break;
  }

  afunc->func = &op_list[i];

  skip_spaces( code );

  if( ( targ = read_arg( ( ( i >= 5 && i <= 6 ) ? arg->type : NONE ) ) ) == NULL ) {
    if( *error_buf == '\0' )
      strcpy( error_buf, "Operator missing second argument." );
    delete afunc;
    return NULL;
  }

  afunc->args += targ;

  // 3 and 4 is && and ||
  if( i >= 3 && i <= 4 ) 
    return read_op( afunc );

  // 5 and 6 is == and !=
  if( i >= 5 && i <= 6 ) {
    if( targ->type != arg->type ) {
      sprintf( error_buf, "Both sides of operator '%s' must be of identical type.", op_list[i].name );
      delete afunc;
      return NULL;
    }
    return afunc;
  }

  // checking 1, 2 += and -= for integer
  if( i != 0 ) {
    if( targ->type != INTEGER || arg->type != INTEGER ) {
      sprintf( error_buf, "Operator '%s' requires integer arguments.", op_list[i].name );
      delete afunc;
      return NULL; 
    }
  }

  // 0, 1, 2 is =, += and -=
  if( i < 3 ) {
    if( arg->family != variable ) {
      strcpy( error_buf, "Assigning a value to a non-variable?" );
      delete afunc;
      return NULL;
    }

    if( !can_assign( arg->type, targ->type ) ) {
      sprintf( error_buf, "Assignment to %s from %s.", arg_type_name[ arg->type ], arg_type_name[ targ->type ] );
      delete afunc;
      return NULL;
    }

    if( *code != ';' ) {
      strcpy( error_buf, "Assignment missing trailing semi-colon." );
      delete afunc;
      return NULL;
    }
    code++;
  }

  return afunc;
}  


/*
 *   CONSTANTS
 */


class Const_Data
{
 public:
  const char**   entry1;
  const char**   entry2;
  int*             size;
  arg_enum         type;

  const char* entry( int j ) const {
    return *(entry1+j*(entry2-entry1));
  };
};


int max_clss      = MAX_CLSS;
int max_dir       = 6;
int max_relig     = MAX_RELIGION;
int max_rflag     = MAX_RFLAG;
int max_skill     = MAX_SKILL;
//int max_stat    = 9;
int max_stat      = 28;
int max_element   = MAX_ATTACK;
int max_wear      = MAX_ITEM_WEAR;
int max_layer     = MAX_LAYER;
int max_visible   = MAX_VISIBLE;
int max_range     = MAX_RANGE;
int max_validity  = 2;
int max_obj_value = 18;
int max_otype     = MAX_ITEM;
int max_dflag     = MAX_DFLAG;
int max_aff_loc   = MAX_AFF_LOCATION;
int max_position  = MAX_POSITION;

//const char* stat_name[] = { "str", "int", "wis", "dex", "con", "level", "piety", "class", "align", "none" };
const char* stat_name[] = { "str", "int", "wis", "dex", "con", "level", "piety", "class", "align", "magic", "fire", 
  "cold", "elec", "mind", "age", "mana", "hit", "move", "ac", "hitroll", "damroll", "maregen", "hpregen", "mvregen",
  "acid", "poison", "holy", "none" };

const char* element_name [] = { "physical", "fire", "cold", "acid", "shock", "mind", "magic", "poison", "holy", "unholy", "none" }; 
const char* visible [] = { "all", "sight", "sound", "touch", "sight_sound" };
const char* range_type_name [] = { "physical", "ranged" };
const char* validity [] = { "false", "true" };

const char* obj_value_name [] = { "lvl", "cond", "dur", "cost","val0", "val1",
  "val2", "val3", "rust", "repr", "ligt", "age", "vnum", "cval", "cont", "numb",
  "sour", "value" };

#define max_nation   MAX_ENTRY_NATION
#define max_race     MAX_ENTRY_RACE
#define max_affect   MAX_ENTRY_AFF_CHAR
#define max_dance    MAX_ENTRY_DANCE

const Const_Data const_list [] = 
{
  {  &nation_table[0].name,       &nation_table[1].name,       &max_nation,   NATION            },
  {  &clss_table[0].name,         &clss_table[1].name,         &max_clss,     CLASS             },
  {  &dir_table[0].name,          &dir_table[1].name,          &max_dir,      DIRECTION         },
  {  &rflag_name[0],              &rflag_name[1],              &max_rflag,    RFLAG             },
  {  &skill_table[0].name,        &skill_table[1].name,        &max_skill,    SKILL             },
  {  &stat_name[0],               &stat_name[1],               &max_stat,     STAT              },
  {  &race_table[0].name,         &race_table[1].name,         &max_race,     RACE              },
  {  &element_name[0],            &element_name[1],            &max_element,  ELEMENT           },
  {  &wear_part_name[0],          &wear_part_name[1],          &max_wear,     WEARPOS           },
  {  &layer_name[0],              &layer_name[1],              &max_layer,    LAYER             },
  {  &aff_char_table[0].name,     &aff_char_table[1].name,     &max_affect,   AFFECT            },
  {  &visible[0],                 &visible[1],                 &max_visible,  VISIBLE           },
  {  &range_type_name[0],         &range_type_name[1],         &max_range,    RANGE             },
  {  &validity[0],                &validity[1],                &max_validity, VALIDITY          },
  {  &religion_table[0].name,     &religion_table[1].name,     &max_relig,    RELIGION          },
  {  &obj_value_name[0],          &obj_value_name[1],          &max_obj_value,OBJ_VALUE         },
  {  &item_type_name[0],          &item_type_name[1],          &max_otype,    OBJ_TYPE          },
  {  &dflag_name[0],              &dflag_name[1],              &max_dflag,    DFLAG_TYPE        },
  {  &dance_table[0].dance,       &dance_table[1].dance,       &max_dance,    DANCE             },
  {  &affect_location[0],         &affect_location[1],         &max_aff_loc,  AFF_LOCATION      },
  {  &position_name[0],           &position_name[1],           &max_position, POSITION          },
  {  NULL,                  NULL,                  NULL,          NONE       },
};

#undef max_aff_loc
#undef max_affect
#undef max_nation
#undef max_race
#undef max_dance

arg_type* read_const( arg_enum expected, char*& code )
{
  arg_type*   arg;

  // need to read a const based on expected type
  for( int i = 0; const_list[i].entry1 != NULL; i++ ) {
    if( expected != const_list[i].type )
      continue;

    for( int j = 0; j < *const_list[i].size; j++ ) {
      if( exact_match( code, const_list[i].entry( j ) ) ) {
        arg         = new arg_type;
        arg->type   = const_list[i].type;
        arg->family = constant;
        arg->value  = (void*) j;
        return arg;
      }
    }
  }

  return NULL;
}


/*
 *   VARIABLES
 */


class Var_Data
{
 public:
  char*        name;
  void*        pointer;
  arg_enum     type;  
};


const class Var_Data variable_list [] =
{
  { "mob",           &var_mob,            CHARACTER   }, 
  { "ch",            &var_ch,             CHARACTER   },
  { "rch",           &var_rch,            CHARACTER   }, 
  { "victim",        &var_victim,         CHARACTER   }, 
  { "arg",           &var_arg,            STRING     },
  { "room",          &var_room,           ROOM },
  { "obj",           &var_obj,            OBJECT     },
  { "robj",          &var_robj,           OBJECT     },
  { "container",     &var_container,      OBJECT     },
  { "i",             &var_i,              INTEGER   },
  { "j",             &var_j,              INTEGER   },
  { "",              NULL,                NONE    }
};


arg_type* read_variable( char*& code, bool neg )
{
  arg_type*  arg;
  int          i;

  for( i = 0; ; i++ ) {
    if( *variable_list[i].name == '\0' ) 
      return NULL;
    if( exact_match( code, variable_list[i].name ) )
      break;
    }

  arg         = new arg_type;
  arg->type   = ( neg ? INTEGER : variable_list[i].type );
  arg->family = variable;
  arg->value  = (void*) variable_list[i].pointer;
  arg->neg    = neg;

  return arg;
}


/*
 *   LOOPS
 */


const char* loop_name[] = { "all_in_room", "followers", "pets", "objects"/*, "enemies" */};


loop_type* read_loop( char*& code )
{
  loop_type* aloop  = new loop_type;
  loop_enum i;

  for( i = loop_all_in_room; i < loop_unknown; i = (loop_enum) ( i + 1 ) ) {
    if( exact_match( code, loop_name[i] ) ) {
      if( *code != ')' ) {
        strcpy( error_buf, "Missing ')' after loop." ); 
        delete aloop;
        return NULL;
      }
      code++; 
      aloop->fruit = (loop_enum) i;
      if( ( aloop->aloop = read_arg( ) ) != NULL )
        return aloop;
      if( *error_buf == '\0' ) 
        strcpy( error_buf, "Error in loop." );
      delete aloop;
      return NULL;
    }
  }

  aloop->fruit = loop_unknown;
  aloop->neg   = ( *code == '!' );
  code        += aloop->neg;

  if( ( aloop->condition = read_op( read_arg( ) ) ) == NULL ) {
    if( *error_buf == '\0' ) 
      strcpy( error_buf, "Loop statement with null condition??" );
    delete aloop;
    return NULL;  
  }   

  skip_spaces( code );

  if( *code != ')' ) {
    strcpy( error_buf, "Loop statement missing closing )." );
    delete aloop;
    return NULL;
  }

  code++;

  if( ( aloop->aloop = read_arg( ) ) == NULL ) {
    if( *error_buf == '\0' ) 
      strcpy( error_buf, "Loop statement with null loop." );
    delete aloop;
    return NULL;  
  }   

  return aloop;
}  


/*
 *   IF STATEMENTS
 */


aif_type* read_if( char*& code )
{
  aif_type*       aif  = new aif_type;
  char*  letter;

  if( ( aif->condition = read_op( read_arg( ) ) ) == NULL ) {
    if( *error_buf == '\0' ) 
      strcpy( error_buf, "If statement with null condition??" );
    delete aif;
    return NULL;  
  }   

  skip_spaces( code );

  if( *code != ')' ) {
    strcpy( error_buf, "If statement missing closing )." );
    delete aif;
    return NULL;
  }

  code++;

  if( ( aif->yes = read_arg( ) ) == NULL || ( ( aif->yes->family == variable || aif->yes->family == constant ) && ( aif->yes = read_op( aif->yes ) ) == NULL ) ) {
    // || ( aif->yes->next == NULL && ( aif->yes = read_op( aif->yes ) ) == NULL ) ) {
    if( *error_buf == '\0' )
      strcpy( error_buf, "If statement with no effect." );
    delete aif;
    return NULL;
  }

  letter = code;
  for( ; isspace( *letter ) || *letter == ';'; letter++ );
  
  if( !strncasecmp( "else", letter, 4 ) ) {
    code = letter+4;
    if( ( aif->no = read_arg( ) ) == NULL || ( ( aif->no->family == variable || aif->no->family == constant ) && ( aif->no = read_op( aif->no ) ) == NULL ) ) {
      // || ( aif->no->next == NULL && ( aif->no = read_op( aif->no ) ) == NULL ) ) {
      if( *error_buf == '\0' )
        strcpy( error_buf, "Else statement with no effect." );
      delete aif;
      return NULL;
    }
  }
 
  for( ; isspace( *code ) || *code == ';'; code++ );

  return aif;
}


/*
 *   FUNCTIONS
 */


arg_type* read_function( char*& code, bool neg )
{
  afunc_type*  afunc;
  arg_type*     targ;
  int           i, j;
  int         length;

  for( i = 0; ; i++ ) {
    if( cfunc_list[i].name[0] == '\0' ) 
      return NULL;
    length = strlen( cfunc_list[i].name );
    if( !strncasecmp( cfunc_list[i].name, code, length ) && code[length] == '(' ) 
      break;
  }

  code       += length + 1; 
  afunc       = new afunc_type;
  afunc->type = ( neg ? INTEGER : cfunc_list[i].type );
  afunc->neg  = neg;
  afunc->func = &cfunc_list[ i ];

  for( j = 0; ; j++ ) {
    skip_spaces( code );
    if( *code == ')' ) 
      break;

    if( j >= cfunc_list[ i ].args.size ) {
      sprintf( error_buf, "Too many arguments for function %s.", cfunc_list[ i ].name );
      delete afunc;
      return NULL;
    }
    
    if( ( targ = read_op( read_arg( cfunc_list[ i ].args[ j ] ) ) ) == NULL ) {
      if( *error_buf == '\0' )
        strcpy( error_buf, "Function missing closing ')'." );
      delete afunc;
      return NULL;
    }

    afunc->args += targ;
    
    if( !can_assign( cfunc_list[ i ].args[ j ], targ->type ) ) {
      sprintf( error_buf, "Passing %s to function %s for argument %d, requires %s.", arg_type_name[ targ->type ], cfunc_list[ i ].name, j + 1, arg_type_name[ cfunc_list[ i ].args[ j ] ] );
      delete afunc;
      return NULL;
    }

    if( *code == ',' ) 
      code++;
  }

  code++;

  return afunc;
}


/*
 *   STRINGS
 */


arg_type* read_string( char*& code, extra_array& data )
{
  arg_type*       arg;
  const char*  string;
  char         letter;

  if( *code != '"' && *code != '#' )
    return NULL;

  arg         = new arg_type;
  arg->type   = STRING;
  arg->family = constant;

  string = code+1;

  if( *code == '"' ) {
    for( code++; *code != '"'; code++ ) {
      if( *code == '\0' ) {
        strcpy( error_buf, "Unexpected end of string." );
        delete arg;
        return NULL;
      }
    }

    *((char*)code) = '\0';
    arg->value     = (void*) code_alloc( string );
    *((char*)code) = '"';
    code++;
    return arg;
  }

  for( code++; *code != ',' && !isspace( *code ); code++ ) 
    if( *code == '\0' ) {
      strcpy( error_buf, "Unexpected end of string." );
      delete arg;
      return NULL;
    }

  letter         = *code;
  *((char*)code) = '\0';
  arg->value     = (void*) get_string( string, data );

  if( arg->value == NULL ) {
    sprintf( error_buf, "String not found. (%s)", string ); 
    delete arg;
    arg = NULL;
  } 

  *((char*)code) = letter;

  return arg;
}


/*
 *   NUMBERS
 */


arg_type* itoarg( int i )
{
  arg_type*   arg  = new arg_type;

  arg->type   = INTEGER;
  arg->family = constant;
  arg->value  = (void*) i;

  return arg;
}


arg_type* read_digit( char*& code )
{
  afunc_type*  afunc;
  int           dice;
  int           plus;
  int           side;
  int              i;
  bool      negative  = FALSE;

  if( *code == '-' ) {
    negative = TRUE;
    code++;
    }

  for( dice = 0; isdigit( *code ); code++ )
    dice = *code+10*dice-'0';

  if( negative )
    dice = -dice;

  if( *code != 'd' )
    return itoarg( dice );

  for( plus = side = 0, code++; isdigit( *code ); code++ )
     side = *code+10*side-'0';

  if( *code == '+' ) 
    for( code++; isdigit( *code ); code++ )
      plus = *code+10*plus-'0';

  afunc         = new afunc_type;
  afunc->type   = INTEGER;
  afunc->family = function;

  afunc->args  += itoarg( dice );
  afunc->args  += itoarg( side );
  afunc->args  += itoarg( plus );

  for( i = 0; ; i++ ) 
    if( !strcasecmp( cfunc_list[i].name, "dice" ) ) {
      afunc->func = &cfunc_list[i];
      break;
    }

  return afunc;
}


void compile_areas( char_data* ch )
{
  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) ) {
    for( room_data* room = area->room_first; room != NULL; room = room->next ) {
      int j = 1;
      for( action_data* action = room->action; action != NULL; j++, action = action->next ) {
        if( action->binary == NULL )
          compile( action );
        
        if( ch && action->corrupt )
          page( ch, "  Acode #%d in room %d is not compiling.\r\n", j, room->vnum );
      }
    }
  }
}


void compile_mobs( char_data* ch )
{
  species_data *species;
  for( int i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL ) 
      continue;
    if( species->attack->binary == NULL )
      compile( species->attack );

    if( ch && species->attack->corrupt )
      page( ch, "  Attack program on mob #%d (%s) is not compiling.\r\n", species->vnum, species->Name() ); 

    int j  = 1;
    for( mprog_data* mprog = species->mprog; mprog != NULL; j++, mprog = mprog->next ) {
      if( mprog->binary == NULL )
        compile( mprog );

      if( ch && mprog->corrupt )
        page( ch, "  Mprog #%d on mob #%d (%s) is not compiling.\r\n", j, species->vnum, species->Name() );
    }
  }
}


void compile_objs( char_data* ch )
{
  for( int i = 0; i < MAX_OBJ_INDEX; i++ ) {
    obj_clss_data* obj;
    if( ( obj = obj_index_list[i] ) == NULL )
      continue;
    int j  = 1;
    for( oprog_data* oprog = obj->oprog; oprog != NULL; j++, oprog = oprog->next ) {
      if( oprog->binary == NULL )
        compile( oprog );
      if( ch && oprog->corrupt )
        page( ch, "  Oprog #%d on obj #%d (%s) is not compiling.\r\n", j, obj->vnum, obj->Name() );
    }
  }
}


void do_compile( char_data* ch, char* argument )
{
  char buf [ MAX_INPUT_LENGTH ];
  bool compiled = false;

  if( fmatches( argument, "areas" ) || fmatches( argument, "all" ) ) {
    page( ch, "Compiling areas...\r\n" );

    compile_areas( ch );

    page( ch, "All Areas Compiled.\r\n" );
    sprintf( buf, "All Areas Compiled (%s).", ch->real_name() );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    compiled = true;
  }

  if( fmatches( argument, "mobs" ) || fmatches( argument, "all" ) ) {
    page( ch, "Compiling mobs...\r\n" );

    compile_mobs( ch );

    page( ch, "All Mobs Compiled.\r\n" );
    sprintf( buf, "All Mobs Compiled (%s).", ch->real_name() );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    compiled = true;
  }

  if( fmatches( argument, "objects" ) || fmatches( argument, "all" ) ) {
    page( ch, "Compiling objects...\r\n" );

    compile_objs( ch );

    page( ch, "All Objects Compiled.\r\n" );
    sprintf( buf, "All Objects Compiled (%s).", ch->real_name() );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES, 1, ch );
    compiled = true;
  }

  if( !compiled )
    send( ch, "Syntax: compile <objects|mobs|areas|all>\r\n" );
}

