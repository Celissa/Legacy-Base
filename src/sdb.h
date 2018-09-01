// Selt's Database Routines

//////// SDB FUNCTIONS AND EXTENSIONS ////////
extern char *read_line(FILE *file);
extern char *strallocat(char *str1, const char *str2);

//// SDB TEXT/TYPE Lookup Code ////
extern lookup_data sdb_types[];

#define SDB_UNDEFINED    LOOKUP_UNDEFINED

#define SDB_END          LOOKUP_UNDEFINED + 1
#define SDB_INT          LOOKUP_UNDEFINED + 2
#define SDB_STRING       LOOKUP_UNDEFINED + 3
#define SDB_TEXTBLOCK    LOOKUP_UNDEFINED + 4
#define SDB_LINE         LOOKUP_UNDEFINED + 5
#define SDB_EDESC        LOOKUP_UNDEFINED + 6
#define SDB_AREA         LOOKUP_UNDEFINED + 7
#define SDB_ROOM         LOOKUP_UNDEFINED + 8
#define SDB_EXIT         LOOKUP_UNDEFINED + 9
#define SDB_HELP         LOOKUP_UNDEFINED + 10
#define SDB_HELPFILE     LOOKUP_UNDEFINED + 11
#define SDB_SOCIAL       LOOKUP_UNDEFINED + 12
#define SDB_SOCIALLIST   LOOKUP_UNDEFINED + 13
#define SDB_PLAYER       LOOKUP_UNDEFINED + 14
#define SDB_ALIAS        LOOKUP_UNDEFINED + 15
#define SDB_OBJECT       LOOKUP_UNDEFINED + 16
#define SDB_OFILE        LOOKUP_UNDEFINED + 17
#define SDB_NOTEBOARD    LOOKUP_UNDEFINED + 18
#define SDB_NOTE         LOOKUP_UNDEFINED + 19
#define SDB_RACE         LOOKUP_UNDEFINED + 20
#define SDB_TERRAIN      LOOKUP_UNDEFINED + 21
#define SDB_TMSG         LOOKUP_UNDEFINED + 22
#define SDB_PROFESSION   LOOKUP_UNDEFINED + 23
#define SDB_MOB          LOOKUP_UNDEFINED + 24
#define SDB_CHANNEL      LOOKUP_UNDEFINED + 25
#define SDB_CLAN         LOOKUP_UNDEFINED + 26
#define SDB_TITLE        LOOKUP_UNDEFINED + 27
#define SDB_XPFILE       LOOKUP_UNDEFINED + 28
#define SDB_XP           LOOKUP_UNDEFINED + 29

//// SDB OUTPUT ROUTINES ////
class sdb_output {
  char *filename;
  FILE *file;
  int nest;

  // write-to-disk routines
  bool write(const char *text);                                     // write single string
  bool write(const char *str1, const char *str2);                   // write double string
  bool write(const char *str1, const char *str2, const char *str3); // write triple string

public:
  // constructor and destructor
  sdb_output(const char *name);
  ~sdb_output();

  // various nester routines
  bool begin(const char *type);                   // begin with just string type
  bool begin(const char *type, const char *tag);  // begin with string type and tag
  bool begin(const char *type, int number);       // begin with string type and int tag
  bool end();                                     // end with no comment
  bool end(const char *type);                     // end with string type
  bool end(const char *str1, const char *str2);   // end with string type and comment
  bool end(const int number);                     // end with int type
  bool end(const char *string, const int number); // end with string type and int tag

  // output extensions
  bool save(const char *tag, const char *text);   // for strings
  bool save(const char *tag, const int number);   // for integers
  bool save(const char *tag, textblock *block);   // for multi-line strings (textblocks)

  // output extensions in notes.cpp
  bool save(noteboard_data *noteboard);
  bool save(note_data *note);
  bool save(const char *tag, note_data *note);    // for the odd occasions we need to tag the note

  // output extensions in clan.cpp
  bool save(clan_data *clan);
  bool save(title_data *title);
};


/// AND THE SDB_LOAD STUFF ///
class sdb_input
{
  char *filename;
  FILE *file;
  void *loaded_data;
  int loaded_type;

  int process_line(selt_string *line); // chops and tags a loaded line to appropriate sdb-types
  bool tag_line(selt_string *line, selt_string *tag);  // takes the tag from the line

  // memory allocater/freer
  void *sdb_create(int mode);
  bool sdb_free(void *data, int type);

  // The central SDB-LOAD routine, all sdb-input requests go through here
  void *load(int mode);

  // standard parsers
  bool parse(textblock *block, int datatype, const char *tag, void *data);
  
  // parser extensions in notes.cpp
  bool parse(noteboard_data *noteboard, int datatype, const char *tag, void *data);
  bool parse(note_data *note, int datatype, const char *tag, void *data);

  // parser extensions in clan.cpp
  bool parse(clan_data *clan, int datatype, const char *tag, void *data);
  bool parse(title_data *title, int datatype, const char *tag, void *data);

  // parser extensions in exp.cpp
  bool parse(llist<xp_data> *xpfile, int datatype, const char *tag, void *data);
  bool parse(xp_data *xp, int datatype, const char *tag, void *data);

public:
  // constructor and destructor
  sdb_input(const char *name);
  ~sdb_input();

  // data-getters
  int get_type()   { return loaded_type; }
  void *get_data() { return loaded_data; }
};
