/* ************************************************************************
*   File: selt_notes.h                  Selt's Sweet CircleMUD Noteboards *
*  Usage: Nice global noteboards                                          *
*   (c) 1998+ Julian Forsythe (julian@rainchild.com)                      *
************************************************************************ */

#ifndef __SELT__NOTE_H  // Only include it once.
#define __SELT__NOTE_H

/* note board structures */
class note_data
{
  friend class sdb_input;
  friend class sdb_output;
protected:
  VARIABLE( time_t, modified );
  VARIABLE_MEM_CHAR( author, MEM_NOTE );
  VARIABLE_MEM_CHAR( title, MEM_NOTE );
  VARIABLE_POINTER( note_data, original );
  VARIABLE_POINTER( noteboard_data, board );
  textblock text;
public:
  note_data( );
  virtual ~note_data( );

  textblock *get_text( )              { return &text; }
  void set_text( textblock *value )   { text.set_text( value ); }
  void set_text( const char *value )  { text.set_text( value ); }

  note_data *duplicate( );
};

class noteboard_data
{
  friend class sdb_input;
  friend class sdb_output;
protected:
  VARIABLE_MEM_CHAR( name, MEM_NOTE );
  VARIABLE_MEM_CHAR( filename, MEM_NOTE );

  llist<note_data> notes;

  VARIABLE_INT( lvl_read );
  VARIABLE_INT( lvl_write );
  VARIABLE_INT( lvl_remove );

  VARIABLE_INT( iflag_level );
  VARIABLE_INT( note_size );
public:
  noteboard_data( const char *tname, const char *tfile, int tread, int twrite, int tremove, int tiflag, int tbuf_size );
  noteboard_data( );
  virtual ~noteboard_data( );
  void rename( const char*, const char* );

  bool add_note( note_data *note )
  {
    note->set_board( this );
    return notes.add( note );
  }

  bool remove_note( note_data *note )
  {
    if( notes.remove( note ) ) {
      note->set_board( NULL );
      return true;
    }

    return false;
  }

  note_list *get_notes( )
  {
    return &notes;
  }
};

// Noteboard Routines
extern noteboard_data *new_noteboard(const char *name, const char *file, int read, int write, int remove, int post, int modify, int buf_size);
extern noteboard_data *return_board(player_data *ch, const char *arg);
extern note_data *note_dup(note_data *note);
extern void boot_noteboards(void);
extern bool save_noteboard(noteboard_data *board);
extern void save_all_noteboards(void);
extern int new_notes(char_data *ch);
extern noteboard_data *load_noteboard(const char *filename);
extern bool verify(noteboard_data *board);
extern bool verify(note_data *note);

// Global Noteboards
extern llist<noteboard_data> noteboards;


#endif

