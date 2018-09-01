/*
 *
 */

void  fsend         ( char_data*, const char* );  
void  fpage         ( char_data*, const char* );  

extern const char*   scroll_line     [ 3 ];

void convert_to_ansi( char_data*, const char*, char*, int );

/*
 *   ACT ROUTINES
 */


#define cc const char
#define cd char_data
#define od obj_data
#define vd visible_data

void  act_print     ( char*, cc*, vd*, vd*, vd*, vd*, cc*, cc*, cd* );

void  act           ( cd*, cc*, vd*, vd* = NULL, vd* = NULL, vd* = NULL, int = VIS_SIGHT );
void  act           ( cd*, cc*, vd*, vd*, cc*, cc* = NULL ); 
void  act_notchar   ( cc*, cd*, vd* = NULL, vd* = NULL, vd* = NULL, int = VIS_SIGHT );
void  act_notchar   ( cc*, cd*, cc*, cc* );
void  act_notchar   ( cc*, cd*, vd*, cc*, cc* );
void  act_room      ( room_data*, cc*, cc* = NULL, cc* = NULL );
void  act_room      ( room_data*, cc*, vd*, vd* = NULL, int = VIS_SIGHT );
void  act_area      ( cc*, cd*, cd*, vd*, int = VIS_SIGHT );
void  act_neither   ( cc*, cd*, cd*, vd*, vd* = NULL );

#undef cc
#undef cd
#undef od
#undef vd

/*
 *   GENERIC DISPLAY ROUTINES
 */


void  display_array  ( char_data*, const char*, const char**, const char**, int );

 
/*
 *   STRING CONVERSION ROUTINES
 */

// New stuff here to see if we can test things for validity before printing
inline const char* tostring( thing_data* thing, char_data* ch )
{
  // Test thing for validity
  if ( !thing || thing->valid < THING_DATA || thing->valid > WIZARD_DATA ) 
  {
     return "[BUG]";
  }
  return thing->Name( ch, thing->shown );
}

inline const char* tostring( exit_data* exit, char_data* ch )
{
  // Need this because an exit is not a thing :(
  return (exit ? exit->Name( ch, exit->shown ) : "[BUG]");
}

/*
**inline const char* tostring( visible_data* visible, char_data* ch )
**{
**  // Crashes mud if visible is not valid.
**  return visible->Name( ch, visible->shown );
**}
*/

inline const char* tostring( species_data* species, char_data* )
{
  return (species ? species->Name( ) : "[BUG]");
}

inline const char* tostring( obj_clss_data* obj_clss, char_data* )
{
  return (obj_clss ? obj_clss->Name( ) : "[BUG]");
}

inline const char* tostring( thing_array* array, char_data* ch )
{  
  return list_name( ch, array );
}

inline const char* tostring( suid a, ... )
{
  static selt_string x;
#ifdef MYSTERIA_WINDOWS
  x._printf( "%016I64X", a );
#else
  x._printf( "%016llX", a );
#endif
  return x;
} 

inline int         tostring( int a, ... )          { return a; } 
inline float       tostring( float a, ... )        { return a; } 
inline double      tostring( double a, ... )       { return a; } 
inline const char* tostring( const char* a, ... )  { return a; } 


/*
 *   ECHO
 */


void   echo                 ( const char* );


template < class T >
void echo( const char* text, T item )
{
  selt_string tmp;
  tmp._printf(text, item);
  tmp.capitalize();
  tmp.truncate(MAX_STRING_LENGTH);
  echo(tmp.get_text());

  return;
}


/*
 *   PAGE
 */


void    page              ( char_data*, const char* );
void    page_centered     ( char_data*, const char* );
void    page_underlined   ( char_data*, const char* );
void    next_page         ( link_data* );


template < class T >
void page( char_data* ch, const char* text, T item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item, ch ) );
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page(ch, tmp.get_text());
  }
}


template < class S, class T >
void page( char_data* ch, const char* text, S item1, T item2 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ) );
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page(ch, tmp.get_text());
  }
}


template < class S, class T, class U >
void page( char_data* ch, const char* text, S item1, T item2, U item3 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page(ch, tmp.get_text());
  }
}


template < class S, class T, class U, class V >
void page( char_data* ch, const char* text, S item1, T item2, U item3, V item4 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page(ch, tmp.get_text());
  }

  return;
}


template < class S, class T, class U, class V, class X >
void page( char_data* ch, const char* text, S item1, T item2, U item3, V item4, X item5 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ), tostring( item5, ch ) );
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page(ch, tmp.get_text());
  }

  return;
}


template < class S, class T, class U, class V, class X, class W >
void page( char_data* ch, const char* text, S item1, T item2, U item3, V item4, X item5, W item6 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ), tostring( item5, ch ), tostring( item6, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page(ch, tmp.get_text());
  }

  return;
}


inline void page_header( char_data* ch, const char* text )
{
  link_data*   link;
  text_data*  paged;

  if( ch != NULL && ( link = ch->link ) != NULL ) {
    paged       = link->paged;
    link->paged = NULL; 
    page( ch, text );
    cat( link->paged, paged );
  }

  return;
}


template < class T >
void page_header( char_data* ch, const char* text, T item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring(item, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page_header(ch, tmp.get_text());
  }

  return;
}


/*
 *   PAGE_UNDERLINED
 */


template < class T >
void page_underlined( char_data* ch, const char* text, T item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring(item, ch));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page_underlined(ch, tmp.get_text());
  }

  return;
}


template < class S, class T >
void page_underlined( char_data* ch, const char* text, S item1, T item2 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page_underlined(ch, tmp.get_text());
  }

  return;
}


/*
 *   PAGE_CENTERED
 */


template < class S >
void page_centered( char_data* ch, const char* text, S item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page_centered(ch, tmp.get_text());
  }

  return;
}


/*
 *   PAGE_DIVIDER
 */


inline void page_divider( char_data* ch, const char* text, int i )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    page(ch, "\r\n");
    tmp._printf("-- %s (%d) --",  text, i);
//    tmp[3] = toupper( tmp[3] );
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page_centered(ch, tmp.get_text());
    page(ch, "\r\n");
  }

  return;
}


/*
 *   FORMATTED PAGE
 */


template < class T >
void fpage( char_data* ch, const char* text, T item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fpage(ch, tmp.get_text());
  }

  return;
};


template < class S, class T >
void fpage( char_data* ch, const char* text, S item1, T item2 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fpage(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U >
void fpage( char_data* ch, const char* text, S item1, T item2, U item3 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fpage(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U, class V >
void fpage( char_data* ch, const char* text, S item1, T item2, U item3, V item4 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fpage(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U, class V, class X >
void fpage( char_data* ch, const char* text, S item1, T item2, U item3, V item4, X item5 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ), tostring( item5, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fpage(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U, class V, class X, class Y >
void fpage( char_data* ch, const char* text, S item1, T item2, U item3, V item4, X item5, Y item6 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ), tostring( item5, ch ), tostring( item6, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fpage(ch, tmp.get_text());
  }

  return;
};


/*
 *   SEND
 */


void    send              ( link_data*, const char* );


inline void write_to_buffer( link_data* link, const char* message )
{
  if( link != NULL )
    send( link, message );

  return;
}


inline void send( char_data* ch, const char* text )
{
  if( ch != NULL && ch->link != NULL )
    send( ch->link, text );

  return;
}


inline void send( const char* text, char_data* ch )
{
  send( ch, text );
  return;
}


template < class T >
inline void send( link_data* link, const char* text, T item )
{
  if( link != NULL ) {
    selt_string tmp;
    tmp._printf(text, item);
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send(link, tmp.get_text());
  }

  return;
}


template < class T >
void send( char_data* ch, const char* text, T item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send(ch, tmp.get_text());
  }

  return;
};


template < class S, class T >
void send( char_data* ch, const char* text, S item1, T item2 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U >
inline void send( link_data* link, const char* text, S item1, T item2, U item3 )
{
  if( link != NULL ) {
    selt_string tmp;
    tmp._printf(text, item1, item2, item3);
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send(link, tmp.get_text());
  }

  return;
}


template < class S, class T, class U >
void send( char_data* ch, const char* text, S item1, T item2, U item3 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U, class V >
void send( link_data* link, const char* text, S item1, T item2, U item3, V item4 )
{
  if( link != NULL && link->player != NULL )
    send( link->player, text, item1, item2, item3, item4 );

  return;
}


template < class S, class T, class U, class V >
void send( char_data* ch, const char* text, S item1, T item2, U item3, V item4 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U, class V, class W >
void send( char_data* ch, const char* text, S item1, T item2, U item3, V item4, W item5 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ), tostring( item5, ch ) );
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U, class V, class W, class X >
void send( char_data* ch, const char* text, S item1, T item2, U item3, V item4, W item5, X item6 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ), tostring( item5, ch ), tostring( item6, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send(ch, tmp.get_text());
  }

  return;
};


/*
 *   SEND_SEEN
 */


template < class T >
void send_seen( char_data* ch, const char* text, T item )
{
  char_data*  rch;

  if( ch->array != NULL ) 
    for( int i = 0; i < *ch->array; i++ ) 
      if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) && rch != (char_data*) item )
        send( rch, text, item );

  return;
};


template < class S, class T >
void send_seen( char_data* ch, const char* text, S item1, T item2 )
{
  char_data*  rch;

  if( ch->array != NULL ) 
    for( int i = 0; i < *ch->array; i++ ) 
      if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) && rch != (char_data*) item1 && rch != (char_data*) item2 )
        send( rch, text, item1, item2 );

  return;
};


template < class S, class T, class U >
void send_seen( char_data* ch, const char* text, S item1, T item2, U item3 )
{
  char_data*  rch;

  if( ch->array != NULL ) 
    for( int i = 0; i < *ch->array; i++ ) 
      if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 )
        send( rch, text, item1, item2, item3 );

  return;
};


template < class S, class T, class U, class V >
void send_seen( char_data* ch, const char* text, S item1, T item2, U item3, V item4 )
{
  char_data*  rch;

  if( ch->array != NULL ) 
    for( int i = 0; i < *ch->array; i++ ) 
      if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 && rch != (char_data*) item4 )
        send( rch, text, item1, item2, item3, item4 );

  return;
};


/*
 *   SEND_ROOM
 */


inline void send( thing_array& array, const char* text )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) 
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && !is_set( rch->affected_by, AFF_BLIND ) )
      send( rch, text );

  return;
};


template < class T >
void send( thing_array& array, const char* text, T item )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) 
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && !is_set( rch->affected_by, AFF_BLIND )&& rch != (char_data*) item )
      send( rch, text, item );

  return;
};


template < class S, class T >
void send( thing_array& array, const char* text, S item1, T item2 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ )
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && !is_set( rch->affected_by, AFF_BLIND ) && rch != (char_data*) item1 && rch != (char_data*) item2 )
      send( rch, text, item1, item2 );

  return;
};


template < class S, class T, class U >
void send( thing_array& array, const char* text, S item1, T item2, U item3 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ )
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && !is_set( rch->affected_by, AFF_BLIND ) && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 )
      send( rch, text, item1, item2, item3 );

  return;
};


template < class S, class T, class U, class V  >
void send( thing_array& array, const char* text, S item1, T item2, U item3, V item4 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ )
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && !is_set( rch->affected_by, AFF_BLIND ) && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 && rch != (char_data*) item4 )
      send( rch, text, item1, item2, item3, item4  );

  return;
};


template < class S, class T, class U, class V, class W >
void send( thing_array& array, const char* text, S item1, T item2, U item3, V item4, W item5 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ )
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && !is_set( rch->affected_by, AFF_BLIND ) && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 && rch != (char_data*) item4 && rch != (char_data*) item5 )
      send( rch, text, item1, item2, item3, item4, item5 );

  return;
};


template < class S, class T, class U, class V, class W, class X >
void send( thing_array& array, const char* text, S item1, T item2, U item3, V item4, W item5, X item6 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ )
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && !is_set( rch->affected_by, AFF_BLIND ) && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 && rch != (char_data*) item4 && rch != (char_data*) item5 && rch != (char_data*) item6 )
      send( rch, text, item1, item2, item3, item4, item5, item6 );

  return;
};


/*
 *   SEND_UNDERLINED
 */


void send_underlined( char_data*, const char* );


template < class S >
void send_underlined( char_data* ch, const char* text, S item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send_underlined(ch, tmp.get_text());
  }

  return;
}


/*
 *   SEND_CENTERED
 */


void send_centered( char_data*, const char* );


template < class S >
void send_centered( char_data* ch, const char* text, S item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring(item, ch));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send_centered(ch, tmp.get_text());
  }

  return;
}


/*
 *   FSEND_ALL
 */


template < class T >
void fsend_all( room_data* room, const char* text, T item )
{
  char_data* rch;

  for( int i = 0; i < room->contents; i++ ) 
    if( ( rch = character( room->contents[i] ) ) != NULL && rch->position > POS_SLEEPING )
      fsend( rch, text, item );

  return;
}


/*
 *   FORMATTED SEND 
 */


template < class T >
void fsend( char_data* ch, const char* text, T item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring(item, ch));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fsend(ch, tmp.get_text());
  }

  return;
} 


template < class S, class T >
void fsend( char_data* ch, const char* text, S item1, T item2 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fsend(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U >
void fsend( char_data* ch, const char* text, S item1, T item2, U item3 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fsend(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U, class V >
void fsend( char_data* ch, const char* text, S item1, T item2, U item3, V item4 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fsend(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U, class V, class W >
void fsend( char_data* ch, const char* text, S item1, T item2, U item3, V item4, W item5 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ), tostring( item5, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fsend(ch, tmp.get_text());
  }

  return;
};


template < class S, class T, class U, class V, class W, class X >
void fsend( char_data* ch, const char* text, S item1, T item2, U item3, V item4, W item5, X item6 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ), tostring( item3, ch ), tostring( item4, ch ), tostring( item5, ch ), tostring( item6, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    fsend(ch, tmp.get_text());
  }

  return;
};


/*
 *   FORMATTED SEND ROOM
 */


template < class T >
void fsend( thing_array& array, const char* text, T item )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ ) 
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && rch != (char_data*) item )
      fsend( rch, text, item );

  return;
} 


template < class S, class T >
void fsend( thing_array& array, const char* text, S item1, T item2 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ )
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && rch != (char_data*) item1 && rch != (char_data*) item2 )
      fsend( rch, text, item1, item2 );

  return;
} 


template < class S, class T, class U >
void fsend( thing_array& array, const char* text, S item1, T item2, U item3 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ )
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 )
      fsend( rch, text, item1, item2, item3 );

  return;
} 


template < class S, class T, class U, class V >
void fsend( thing_data& array, const char* text, S item1, T item2, U item3, V item4 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ )
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 && rch != (char_data*) item4 )
      fsend( rch, text, item1, item2, item3, item4 );

  return;
} 


template < class S, class T, class U, class V, class W >
void fsend( thing_array& array, const char* text, S item1, T item2, U item3, V item4, W item5 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ )
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 && rch != (char_data*) item4 && rch != (char_data*) item5 )
      fsend( rch, text, item1, item2, item3, item4, item5 );

  return;
} 


template < class S, class T, class U, class V, class W, class X >
void fsend( thing_array& array, const char* text, S item1, T item2, U item3, V item4, W item5, X item6 )
{
  char_data*  rch;

  for( int i = 0; i < array; i++ )
    if( ( rch = character( array[i] ) ) != NULL && rch->position > POS_SLEEPING && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 && rch != (char_data*) item4 && rch != (char_data*) item5 && rch != (char_data*) item6 )
      fsend( rch, text, item1, item2, item3, item4, item5, item6 );

  return;
} 


/*
 *   FORMATTED_SEND_SEEN
 */


template < class T >
void fsend_seen( char_data* ch, const char* text, T item )
{
  char_data*  rch;

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) && rch != (char_data*) item )
      fsend( rch, text, item );

  return;
};


template < class S, class T >
void fsend_seen( char_data* ch, const char* text, S item1, T item2 )
{
  char_data*  rch;

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) && rch != (char_data*) item1 && rch != (char_data*) item2 )
      fsend( rch, text, item1, item2 );

  return;
};


template < class S, class T, class U >
void fsend_seen( char_data* ch, const char* text, S item1, T item2, U item3 )
{
  char_data*  rch;

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 )
      fsend( rch, text, item1, item2, item3 );

  return;
};


template < class S, class T, class U, class V >
void fsend_seen( char_data* ch, const char* text, S item1, T item2, U item3, V item4 )
{
  char_data*  rch;

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL && rch != ch && ch->Seen( rch ) && rch != (char_data*) item1 && rch != (char_data*) item2 && rch != (char_data*) item3 && rch != (char_data*) item4 )
      fsend( rch, text, item1, item2, item3, item4 );

  return;
};


/*
 *   SEND_COLOR
 */


template < class T >
inline void send_color( char_data* ch, int color, const char* text, T item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send_color(ch, color, tmp.get_text());
  }

  return;
}


template < class S, class T >
inline void send_color( char_data* ch, int color, const char* text, S item1, T item2 )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item1, ch ), tostring( item2, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send_color(ch, color, tmp.get_text());
  }

  return;
}


/*
 *   TITLE
 */


void  page_title    ( char_data*, const char* );
void  send_title    ( char_data*, const char* );


template < class S >
inline void page_title( char_data* ch, const char* text, S item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    page_title(ch, tmp.get_text());
  }

  return;
}


template < class S >
inline void send_title( char_data* ch, const char* text, S item )
{
  if( ch != NULL && ch->link != NULL ) {
    selt_string tmp;
    tmp._printf(text, tostring( item, ch ));
    tmp.capitalize();
    tmp.truncate(MAX_STRING_LENGTH);
    send_title(ch, tmp.get_text());
  }

  return;
}

