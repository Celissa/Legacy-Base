class Template_Data
{
public:
  int vnum;

  Template_Data( )
  {
    vnum = -1;
  }
};


class Visible_Data
{
 public:
  int               number;
  int             selected;
  int                shown;   

  virtual int Type() { return VISIBLE_DATA; }

  Visible_Data( ) {
    number   = 1;
    selected = 1; 
    shown    = 1;
    }

  virtual ~Visible_Data( ) {
    }


  /* TYPE CASTING */

  friend char_data* character( Visible_Data* visible ) {
    return( visible != NULL && visible->Type( ) >= CHAR_DATA
      ? (char_data*) visible : NULL );
    }

  friend player_data* player( Visible_Data* visible ) {
    return( visible != NULL && visible->Type( ) >= PLAYER_DATA
      ? (player_data*) visible : NULL );
    }

  friend mob_data* mob( Visible_Data* visible ) {
    return( visible != NULL && visible->Type( ) == MOB_DATA
      ? (mob_data*) visible : NULL );
    }

  friend wizard_data* wizard( Visible_Data* visible ) {
    return( visible != NULL && visible->Type( ) == WIZARD_DATA
      ? (wizard_data*) visible : NULL );
    }

  friend obj_data* object( Visible_Data* visible ) {
    return( visible != NULL && visible->Type( ) == OBJ_DATA
      ? (obj_data*) visible : NULL ); 
    }

  friend room_data* Room( Visible_Data* visible ) {
    return( visible != NULL && visible->Type( ) == ROOM_DATA
      ? (room_data*) visible : NULL ); 
    }

  friend exit_data* exit( Visible_Data* visible ) {
    return( visible != NULL && visible->Type( ) == EXIT_DATA
      ? (exit_data*) visible : NULL ); 
    }

  friend extra_data* extra( Visible_Data* visible ) {
    return( visible != NULL && visible->Type( ) == EXTRA_DATA
      ? (extra_data*) visible : NULL ); 
    }

  friend thing_data* thing( Visible_Data* visible ) {
    return( visible != NULL && visible->Type( ) != EXIT_DATA
      ? (thing_data*) visible : NULL ); 
    }

  /* NAME */

  virtual const char* Name       ( char_data* = NULL, int = 1, bool = FALSE );
  virtual const char* Keywords   ( char_data* );
  virtual bool        Seen       ( char_data* );
  virtual void        Look_At    ( char_data* );
};

class Variable_Data
{
  VARIABLE_CHAR( name );
  int value;
public:
  Variable_Data( const char*, int );
  virtual ~Variable_Data( );

  void set_value( int nValue )
  {
    value = nValue;
  }

  int get_value( )
  {
    return value;
  }
};

class Thing_Data : public Visible_Data
{
public:
  content_array   contents;
  event_array       events;
  int                 temp;
  int             position;
  content_array*     array;
  int                valid;

  Thing_Data( ) : contents( this ) {
    valid    = 0;
    number   = 1;
    selected = 1;
    shown    = 1;
    array    = NULL;
    position = -1;
  }

  virtual ~Thing_Data( );

  /* BASE */

  virtual int Type() { return THING_DATA; }
  virtual void  Extract ( )  { };

  /* NAME */

  virtual const char* Seen_Name  ( char_data*, int = 1, bool = FALSE );
  virtual const char* Long_Name  ( char_data*, int = 1 );
  virtual const char* Show       ( char_data*, int = 1 );
  virtual const char* Location   ( content_array* = NULL );

  /* PROPERTIES */

  virtual bool         Is_Valid        ( )  { return valid > 0; }
  virtual bool         In_Game         ( )  { return TRUE; }
  virtual int          Number          ( int = -1 ); 
  virtual int          Light           ( int = -1 );
  virtual int          Weight          ( int = -1 );
  virtual int          Empty_Weight    ( int = -1 );
  virtual int          Capacity        ( );
  virtual int          Empty_Capacity  ( );
  virtual void         To              ( content_array* );
  virtual void         To              ( thing_data* );
  virtual thing_data*  From            ( int = 1 );

  /* VARIABLES */

  variable_array variables;

  int                  Get_Integer     ( const char* );
  void                 Set_Integer     ( const char*, int );
};


extern Array<Thing_Data*> extracted;


/*
 *   BASIC FUNCTIONS
 */


void         extract            ( thing_array& );
void         consolidate        ( thing_array& );
void         consolidate        ( obj_data* );
void         select             ( thing_array& );
void         select             ( thing_array&, char_data* );
const char*  list_name          ( char_data*, thing_array*, bool = TRUE );
void         remove_weight      ( thing_data*, int );
bool         none_shown         ( thing_array& );
thing_data*  one_shown          ( thing_array& );


/*
 *   FINDING THINGS
 */


#define va visible_array
#define vd visible_data
#define ta thing_array
#define td thing_data
#define cd char_data
#define od obj_data
#define pd player_data

vd* one_visible        ( int, cd*, const char*, const char*, va*, va*, va* = NULL, va* = NULL, va* = NULL );
va* several_visible    ( int, cd*, const char*, const char*, va*, va*, va* = NULL, va* = NULL );
td* one_thing          ( int, cd*, const char*, const char*, ta*, ta* = NULL, ta* = NULL, ta* = NULL );
ta* several_things     ( int, cd*, const char*, const char*, ta*, ta* = NULL, ta* = NULL );
cd* one_character      ( cd*, const char*, const char*, ta*, ta* = NULL, ta* = NULL );
cd* one_character_ooc  ( cd*, const char*, const char* );
pd* one_player         ( cd*, const char*, const char*, ta*, ta* = NULL, ta* = NULL );
od* one_object         ( cd*, const char*, const char*, ta*, ta* = NULL, ta* = NULL );

#undef va
#undef vd
#undef ta
#undef td
#undef cd
#undef od
#undef pd


/*
 *   PROPERTIES
 */


inline const char* location( content_array* array )
{
  if( array == NULL || array->where == NULL )
    return "Nowhere?";
 
  return array->where->Location( array );
}


inline char_data* carried_by( thing_data* thing )
{
  for( ; thing->array != NULL; ) {
    thing = thing->array->where;
    if( character( thing ) != NULL )
      return (char_data*) thing;
    }    

  return NULL;
}








