bool   is_set     ( char_data*, char* );

class Tell_Data
{
 public:
  tell_data*    next;
  char*         name;
  char*         message;
  int           language;
  time_t        tm;

  Tell_Data     ( const char*, const char*, int );
  ~Tell_Data    ( );
};


/*
 *   COMMUNICATION ROUTINES
 */


bool  can_talk    ( char_data*, const char*  = NULL );


/*
 *   LANGUAGE ROUTINES
 */


inline int skill_language( char_data* ch, int language )
{
  if( ch->species != NULL )
    return 0;

  if( is_set( ch->affected_by, AFF_TONGUES ) ) 
    return 10;

  return ch->pcdata->skill[language];
}
