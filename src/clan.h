/*
 *   ALIGNMENT
 */


#define ALGN_LAWFUL_GOOD            0
#define ALGN_LAWFUL_NEUTRAL         1
#define ALGN_LAWFUL_EVIL            2
#define ALGN_NEUTRAL_GOOD           3
#define ALGN_PURE_NEUTRAL           4
#define ALGN_NEUTRAL_EVIL           5
#define ALGN_CHAOTIC_GOOD           6
#define ALGN_CHAOTIC_NEUTRAL        7
#define ALGN_CHAOTIC_EVIL           8
#define MAX_ALIGNMENT               9


#define align   ch->shdata->alignment%3

inline bool is_good    ( char_data* ch ) { return( align == 0 ); }
inline bool is_neutral ( char_data* ch ) { return( align == 1 ); }
inline bool is_evil    ( char_data* ch ) { return( align == 2 ); }

#undef align

#define align   ch->shdata->alignment/3

inline bool is_lawful  ( char_data* ch ) { return( align == 0 ); }
inline bool is_n2      ( char_data* ch ) { return( align == 1 ); }
inline bool is_chaotic ( char_data* ch ) { return( align == 2 ); }

#undef align

inline bool is_int_good( int align )
{
  if( is_set( &align, ALGN_LAWFUL_GOOD ) || is_set( &align, ALGN_NEUTRAL_GOOD )
    || is_set( &align, ALGN_CHAOTIC_GOOD ) )
    return true;
  
  return false;
}

/*
 *   TITLES
 */


#define TITLE_SET_FLAGS       0
#define TITLE_EDIT_CHARTER    1
#define TITLE_RECRUIT         2
#define TITLE_REMOVE_NOTES    3
#define MAX_TITLE             4


class Title_Data
{
public:
  char*          name;
  int           flags  [ 2 ];
  pfile_data*   pfile;

  Title_Data( char* s1, pfile_data* p1 ) {
    name     = alloc_string( s1, MEM_CLAN );
    flags[0] = 0;
    flags[1] = 0;
    pfile    = p1;
  }

  Title_Data( ) {
    name     = empty_string;
    flags[0] = 0;
    flags[1] = 0;
    pfile    = NULL;
  }

  ~Title_Data( ) {
    free_string( name, MEM_CLAN );
  }
};


title_data*    get_title         ( clan_data*, pfile_data* );

/*
class Title_Array
{
 public:
  int           size;
  title_data**  list;

  Title_Array( ) {
    size = 0;
    list = NULL;
  }

  ~Title_Array( ) {
    if( size > 0 ) 
      delete [] list;
  }
};
*/

/*
 *   CLAN CLASS
 */


#define CLAN_APPROVED         0
#define CLAN_KNOWN            1
#define CLAN_PUBLIC           2
#define MAX_CLAN_FLAGS        3


extern clan_array     clan_list;

class Clan_Data
{
 public:
  char*                name;
  char*              abbrev;
  char*             charter;
  bool                guild;
  int                 flags  [ 2 ];
  int             min_level;
  int               classes;
  int                 races;
  int            alignments;
  int                 sexes;
  int                  date;
  bool             modified;
  title_array        titles;
  pfile_array       members;
  noteboard_data *noteboard;
  tell_data*          tells;

  Clan_Data();
  ~Clan_Data();

  friend const char* name( clan_data* clan ) {
    return( clan->name == empty_string ? clan->abbrev : clan->name );
  }
};


inline bool same_clan( player_data* c1, player_data* c2 )
{
  if( !c1 || !c2 )
    return false;

  return( c1->pcdata->pfile->clan2 != NULL && c1->pcdata->pfile->clan2 == c2->pcdata->pfile->clan2 );
}


inline bool knows_members( player_data* ch, clan_data* clan )
{
  if( !ch || !clan )
    return false;

  if( ch->pcdata->pfile->clan2 == clan || ( is_set( clan->flags, CLAN_APPROVED ) && is_set( clan->flags, CLAN_PUBLIC ) ) )
    return true;

  if( ch->pcdata->pfile->guild == clan || ( is_set( clan->flags, CLAN_APPROVED ) && is_set( clan->flags, CLAN_PUBLIC ) ) )
    return true;

  if( has_permission( ch, PERM_CLANS ) || is_apprentice( ch ) )
    return true;

  return false;
}  


inline bool knows( player_data* ch, clan_data* clan )
{
  if( !ch || !clan )
    return false;

  if( ch->pcdata->pfile->clan2 == clan || ( is_set( clan->flags, CLAN_APPROVED ) && is_set( clan->flags, CLAN_KNOWN ) ) )
    return true;

  if( ch->pcdata->pfile->guild == clan || ( is_set( clan->flags, CLAN_APPROVED ) && is_set( clan->flags, CLAN_KNOWN ) ) )
    return true;

  if( has_permission( ch, PERM_CLANS ) || is_apprentice( ch ) )
    return true;

  return false;
}


void        add_member      ( clan_data*, pfile_data* );
void        remove_member   ( clan_data*, pfile_data* );
void        remove_member   ( clan_data*, player_data* );
bool        save_clans      ( clan_data* = NULL );
void        load_clans      ( void );
clan_data*  find_clan       ( player_data*, char* );


