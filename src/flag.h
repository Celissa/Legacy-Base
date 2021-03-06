class flag_data
{
 public:
  const char*     title;
  const char**    name1;
  const char**    name2;
  int*            max;

  void   sprint       ( char*, int* );
  void   display      ( char_data*, int* );
  const char *set     ( char_data*, char*, int* );
};


extern flag_data affect_flags;
extern flag_data permission_flags;
extern flag_data material_flags;
extern flag_data location_flags;
extern flag_data align_flags;
extern flag_data abv_align_flags;
extern flag_data lang_flags;
extern flag_data race_flags;
extern flag_data clss_flags;
extern flag_data terrain_flags;
extern flag_data quest_flags;

extern int max_language;

/*
 *   FUNCTION DECLARATIONS
 */


const char*  flag_handler    ( const char**, const char***, const char***,
                               int**, int*, int*, const char*, char_data*,
                               char*, int );
void         display_flags   ( const char*, const char**, const char**,
                               int*, int, char_data* );
const char*  set_flags       ( const char**, const char**, int*, int,
                               const char*, char_data*, char*,
                               bool, bool );
void         set_flags       ( char_data*, char*&, int*, const char* );
bool         toggle          ( char_data*, char*, const char*, int*, int );
bool         get_flags       ( char_data*, char*&, int*, const char*,
                               const char* );
void         alter_flags     ( int*, int*, int*, int );
void         set_bool        ( char_data*, char*, const char*, int& );


/*
 *   LEVEL SETTING ROUTINES
 */


void   display_levels  ( const char*, const char**, const char**,
                          int*, int, char_data* );
bool   set_levels      ( const char**, const char**, int*,
                         int, char_data*, char*, bool );



