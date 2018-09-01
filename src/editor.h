// Selt Editor

extern unsigned int cstrlen(const char *text);
extern void format(const char *text, selt_string *buf, unsigned int cols, const int type);

// not entirely happy about this, would be good if we could use the llist class
// individual lines for the textblock class
class textblock_line {
  char *text;
  VARIABLE_POINTER(textblock_line, next);
  VARIABLE_INT(length);
public:
  textblock_line(const char *string)
  {
    text = NULL;
    next = NULL;
    length = 0;
    if (string && *string) {
      text = str_dup(string);
      length = strlen(text);
    }
  }

  void set_text(const char *string)
  {
    if (text)
      delete [] text;
    text = NULL;
    length = 0;
    if (string && *string) {
      text = str_dup(string);
      length = strlen(text);
    }
  }

  char *get_text()
  {
    return text;
  }

  virtual ~textblock_line()
  {
    if (text)
      delete [] text;
  }
};

class textblock {
  friend class sdb_input;
  friend class sdb_output;
protected:
  VARIABLE_POINTER(textblock_line, text);
  char *tstring;  // temp variable for returning 'static' strings and such

  char *empty_str()
  {
    char *temp = new char[1];
    *temp = '\0';
    return temp;
  }

public:
  void init();
  textblock();
  textblock(const char *string);
  textblock(textblock *block);
  virtual ~textblock();

  void edit(const char *line_to_add, char_data *ch);

  bool format(const int start_line, const int end_line, const int cols);
  void clear();
  
  void add_text(const char *string);
  void add_text(const textblock *block);
  void set_text(const char *string);
  void set_text(const textblock *block);
  char *get_string(bool strip = false);

  int get_size()
  {
    int size = 0;
    for (textblock_line *temp = text; temp; temp = temp->get_next())
      size += temp->get_length() + 2;

    return size;
  }

  bool has_text()
  {
    if (!text)
      return false;

    return true;
  }

  // extracts the first line from the block
  bool get_line(selt_string *buffer)
  {
    if (!text)
      return false;

    textblock_line *temp = text;
    text = text->get_next();
    *buffer = temp->get_text();
    delete temp;

    return true;
  }

  textblock *duplicate();

  void list(char_data *ch);
  void show(char_data *ch);

  int IndexOfName( const char* name );
  CBString ValueOfName( const char* name );
  void SetValue( const char* name, const char* value );
};

/* used in char *format( ... ) */
#define CHAR_NORMAL      0
#define CHAR_PERIOD      1
#define CHAR_DOTDOTDOT   2
#define CHAR_OTHER       3
#define CHAR_SPACE       4

/* Format Types */
#define FORMAT_TEXTBLOCK 0
#define FORMAT_COMMS     1
#define FORMAT_ACTION    2
#define FORMAT_NOCAP     3
