/*
 *  CHARACTER FUNCTIONS 
 */

inline const char* char_data :: seen_pers( char_data* victim )
{
  if( is_set( affected_by, AFF_HALLUCINATE ) )
    return fake_mob[ number_range( 0, MAX_FAKE_MOB-1 ) ];

  if( *victim->pet_name != '\0' && victim->leader == this )
    return victim->pet_name; 

  if( victim->species != NULL || Recognizes( victim ) )
    return victim->descr->name;

  return victim->descr->short_descr;
}


inline const char* char_data :: pers( char_data* victim )
{
  return can_see( victim ) ? seen_pers( victim ) : "someone";
}


inline const char* who_pers( char_data* ch, char_data* victim )
{
  return can_see_who( ch, victim ) ? ch->seen_pers( victim ) : "someone";
}

