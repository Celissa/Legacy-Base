/*
 *   TYPE DECLARATIONS
 */


typedef class Loop_Type   loop_type;
typedef class Aif_Type    aif_type;
typedef class Afunc_Type  afunc_type;
typedef class Cfunc_Type  cfunc_type;


enum arg_enum {  NONE, ANY, STRING, INTEGER, CHARACTER,
  OBJECT, ROOM, DIRECTION, NATION, SKILL, RFLAG, STAT, CLASS,
  RELIGION, RACE, THING, ELEMENT, WEARPOS, LAYER,  AFFECT, VISIBLE, RANGE,
  VALIDITY, CLAN_ABB, OBJ_VALUE, OBJ_TYPE, DFLAG_TYPE, DANCE, AFF_LOCATION,
  POSITION };
enum fam_enum  { variable, constant, function, if_clause, end, cont, loop };
enum op_enum   { greater_than, less_than, is_equal, set_equal, not_equal };
enum loop_enum { loop_all_in_room, loop_followers, loop_pets, loop_objs, /* loop_enemies,*/ loop_unknown }; 


typedef void* cfunc     ( void** );

typedef class Array<arg_enum>       arg_enum_array;
typedef class Array<arg_type*>      arg_type_array;

/*
 *   QUEUE CLASS
 */


class Queue_Data
{
public:
  queue_data*        next;
  arg_type*           arg;
  char_data*           ch;
  char_data*          mob;
  obj_data*           obj;
  room_data*         room;
  int                   i;
  program_data*   program;
  int                time;

  Queue_Data( ) {
    record_new( sizeof( queue_data ), MEM_QUEUE );
    return;
  }

  ~Queue_Data( ) {
    record_delete( sizeof( queue_data ), MEM_QUEUE );
    return;
  }
};


/*
 *   ARGUMENT TYPES
 */


class Arg_Type
{
public:
  arg_enum      type;
  int         family;
  void*        value;
  arg_type*     next;
  bool           neg;

  Arg_Type( ) {
    record_new( sizeof( arg_type ), MEM_PROGRAM );
    next  = NULL;
    neg   = FALSE;
    value = NULL;
    type  = NONE;
  };

  ~Arg_Type( ) {
    record_delete( sizeof( arg_type ), MEM_PROGRAM );
    if( next != NULL )
      delete next;
  };
};


class Loop_Type : public arg_type
{
public:
  loop_enum       fruit;
  arg_type*       aloop;
  arg_type*   condition;

  Loop_Type( ) {
    record_new( sizeof( loop_type ), MEM_PROGRAM );
    family    = loop;
    aloop     = NULL;
    condition = NULL;
  }

  ~Loop_Type( ) {
    record_delete( sizeof( loop_type ), MEM_PROGRAM );
    if( aloop != NULL ) 
      delete aloop;
    if( condition != NULL )
      delete condition;
  }
};


class Aif_Type : public arg_type
{
public:
  arg_type*  condition;
  arg_type*        yes;
  arg_type*         no;        

  Aif_Type( ) {
    record_new( sizeof( aif_type ), MEM_PROGRAM );
    family    = if_clause;
    condition = NULL;
    yes       = NULL;
    no        = NULL;
  }

  ~Aif_Type( ) {
    record_delete( sizeof( aif_type ), MEM_PROGRAM );
    if( condition != NULL )
      delete condition;
    if( yes != NULL )
      delete yes;
    if( no != NULL )
      delete no;
  }
};


class Afunc_Type : public arg_type
{
public:
  const cfunc_type*  func;
  arg_type_array     args;

  Afunc_Type( ) {
    record_new( sizeof( afunc_type ), MEM_PROGRAM );
    family = function;
    args.allow_multiple = true;
  }

  ~Afunc_Type( ) {
    record_delete( sizeof( afunc_type ), MEM_PROGRAM );
    while( args.size > 0 ) {
      arg_type* arg = args[ 0 ];
      args -= arg;
      delete arg;
    }
  }
};


/*
 *   ???
 */

class Cfunc_Type
{
public:
  Cfunc_Type( const char*, cfunc*, arg_enum, ... );
  ~Cfunc_Type( );

  const char*     name;
  cfunc*     func_call;
  arg_enum        type;
  arg_enum_array  args;
};


class Stack_Data
{
public:
  stack_data*    next;

  char_data*   mob;  
  char_data*   ch;
  char_data*   rch;
  char_data*   victim;
  char*        arg;
  room_data*   room;
  obj_data*    obj;
  obj_data*    robj;
  obj_data*    container;
  int          i;
  int          j;
};


/*
 *   CODE RELATED FUNCTIONS
 */


void         compile_areas   ( char_data* = NULL );
void         compile_mobs    ( char_data* = NULL );
void         compile_objs    ( char_data* = NULL );


void*        code_alloc      ( int );
char*        code_alloc      ( const char* );
void         code_bug        ( char* );
void         pop             ( void );
void         push            ( void );
void         compile         ( program_data* );
char*        get_string      ( const char*, extra_array& );
void         clear_queue     ( program_data* );


arg_type*    read_arg        ( arg_enum = NONE );
arg_type*    read_op         ( arg_type* );
arg_type*    read_string     ( char*&, extra_array& );
arg_type*    read_variable   ( char*&, bool );
aif_type*    read_if         ( char*& );
loop_type*   read_loop       ( char*& );
arg_type*    read_function   ( char*&, bool );
arg_type*    read_const      ( arg_enum, char*& );
arg_type*    read_digit      ( char*& );


extern bool               end_prog;
extern bool              cont_prog;
extern bool             queue_prog;
extern queue_data*      queue_list;
extern arg_type*          curr_arg;
extern char              error_buf  [ MAX_INPUT_LENGTH ];
extern mem_block*       block_list;
extern const char*   arg_type_name  [];

extern cfunc_type cfunc_list [];
extern cfunc_type op_list [];

