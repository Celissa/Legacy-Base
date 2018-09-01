#include "system.h"

//////// BELOW CODE IS SELT-OLC-DATABASE FUNCTIONS ////////

//// SDB TEXT/TYPE Lookup Code ////
lookup_data sdb_types[] = {
  /* SDB_TYPE       TEXT VALUE */
  lookup_data(SDB_END       , "end")         ,

  lookup_data(SDB_INT       , "int")         ,
  lookup_data(SDB_STRING    , "string")      ,
  lookup_data(SDB_TEXTBLOCK , "text")        ,
  lookup_data(SDB_LINE      , "line")        ,
  lookup_data(SDB_EDESC     , "edesc")       ,
  lookup_data(SDB_AREA      , "area")        ,
  lookup_data(SDB_ROOM      , "room")        ,
  lookup_data(SDB_EXIT      , "exit")        ,
  lookup_data(SDB_HELP      , "help")        ,
  lookup_data(SDB_HELPFILE  , "helpfile")    ,
  lookup_data(SDB_SOCIAL    , "action")      ,
  lookup_data(SDB_SOCIALLIST, "sociallist")  ,
  lookup_data(SDB_PLAYER    , "player")      ,
  lookup_data(SDB_ALIAS     , "alias")       ,
  lookup_data(SDB_OBJECT    , "object")      ,
  lookup_data(SDB_OFILE     , "ofile")       ,
  lookup_data(SDB_NOTEBOARD , "noteboard")   ,
  lookup_data(SDB_NOTE      , "note")        ,
  lookup_data(SDB_RACE      , "race")        ,
  lookup_data(SDB_TERRAIN   , "terrain")     ,
  lookup_data(SDB_TMSG      , "tmsg")        ,
  lookup_data(SDB_PROFESSION, "profession")  ,
  lookup_data(SDB_MOB       , "mob")         ,
  lookup_data(SDB_CHANNEL   , "channel")     ,
  lookup_data(SDB_CLAN      , "clan")        ,
  lookup_data(SDB_TITLE     , "title")       ,
  lookup_data(SDB_XPFILE    , "xpfile")      ,
  lookup_data(SDB_XP        , "xp")          ,

  lookup_data(SDB_UNDEFINED , "undefined")   
};


// Re-Allocates memory of STR1 in order to concatenate STR2
char *strallocat(char *str1, const char *str2)
{
  char *newstr;

  if (!str2 || !*str2)
    return str1;

  if (!str1 || !*str1) {
    newstr = new char [strlen(str2) + 1];
    strcpy(newstr, str2);
  } else {
    newstr = new char [strlen(str1) + strlen(str2) + 1];
    strcpy(newstr, str1);
    strcat(newstr, str2);
  }

  if (str1)
    delete [] str1;

  return newstr;
}

//// Generic Load Routines ////
// Reads a line of text from a file, allocates memory for it, and stuff.
char *read_line(FILE *file)
{
  char *line = NULL, temp, buf[260];
  int i = 0;

  if (!file)
    return NULL;

  // if we're on a blank line, skip ahead to the first one with data
  do {
    temp = fgetc(file);
    if (feof(file) || ferror(file))
      return NULL;

  } while (temp == '\n' || temp == '\r');

  // if we get this far, we're ready to start reading for real
  do {
    buf[i++] = temp;

    if (i >= 255) {
      buf[i] = '\0';
      line = strallocat(line, buf);
      i = 0;
    }

    temp = fgetc(file);
    if (feof(file) || ferror(file))
      return NULL;

  } while (temp != '\n' && temp != '\r');

  // plus don't forget to add the last bit from the buffer
  buf[i] = '\0';
  line = strallocat(line, buf);

  return line;
}


//// SDB OUTPUT FUNCTIONS ////
// constructor
sdb_output::sdb_output(const char *name)
{
  nest = 1;

  if (name && *name) {
    filename = str_dup(name);
    for (char *temp = filename; *temp; temp++)
      *temp = tolower(*temp);
  } else
    filename = NULL;

  if (filename) {
    selt_string tfn;
    tfn.printf("%s.new", filename);
    file = fopen(tfn, "wb");
  } else
    file = NULL;

  if (!file) {
    log("SYSERR: SDB_OUTPUT was unable to save to '%s'", filename);
  } else {
    write("sdb");
  }
}

// destructor
sdb_output::~sdb_output()
{
  // close the outfile
  if (file) {
    fclose(file);
    file = NULL;
  }

  // And replace the stuff
  if (filename) {
    selt_string tfn;
    tfn.printf("%s.new", filename);

    // delete old file, rename new to old
    remove(filename);
    if (rename(tfn, filename) < 0) {
      log("SYSERR: SDB_OUTPUT was unable to save to '%s'", filename);
    }

    delete [] filename;
  }
}

//////////////////// Main Output Routines ////////////////////
bool sdb_output::write(const char *text)
{
  if (!file)
    return FALSE;

  // make nested hashes
  char *output = new char [nest + strlen(text) + 1];
  char *write = output;

  for (int i = 0; i < nest; i++)
    *write++ = '#';

  for (const char *read = text; *read; read++)
    if (*read == '\r' || *read == '\n')
      *write++ = '*';
    else
      *write++ = *read;

  *write = '\0';

  // output to file
  bool success = (fprintf(file, "%s\n", output) < 1) ? (FALSE) : (TRUE);

  // free and return
  delete [] output;
  return success;
}

bool sdb_output::write(const char *str1, const char *str2)
{
  // merge strings
  char *text = new char [strlen(str1) + strlen(str2) + 2];
  sprintf(text, "%s %s", str1, str2);

  // write to file
  bool success = write(text);

  // free and return
  delete [] text;
  return success;
}
 
bool sdb_output::write(const char *str1, const char *str2, const char *str3)
{
  // merge strings
  selt_string text;
  text.printf("%s %s %s", str1, str2, str3);

  // write to file
  bool success = write(text);
  return success;
}

//////////////////// Nesters ////////////////////
// start of record
bool sdb_output::begin(const char *type)
{
  bool success = write(type);
  nest++;
  return success;
}

// optional record start for  tagged type
bool sdb_output::begin(const char *type, const char *tag)
{
  bool success = write(type, tag);
  nest++;
  return success;
}

// optional record start for integers
bool sdb_output::begin(const char *type, int number)
{
  // stringize integer
  char textnum[256];
  sprintf(textnum, "%d", number);

  // save it
  return begin(type, textnum);
}

// end of record
bool sdb_output::end()
{
  nest--;
  return write(lookup(SDB_END, &sdb_types[0]));
}

// optional record end for string comments
bool sdb_output::end(const char *type)
{
  nest--;
  return write(lookup(SDB_END, &sdb_types[0]), type);
}

// optional record end for dual string comments
bool sdb_output::end(const char *str1, const char *str2)
{
  nest--;
  return write(lookup(SDB_END, &sdb_types[0]), str1, str2);
}

// optional record end for integer comments
bool sdb_output::end(const int number)
{
  // stringize integer
  char textnum[256];
  sprintf(textnum, "%d", number);

  // save it
  return end(textnum);
}

// optional record end for string + integer comment
bool sdb_output::end(const char *string, const int number)
{
  // stringize integer
  char textnum[256];
  sprintf(textnum, "%d", number);

  // save it
  return end(string, textnum);
}

//////////////////// Output Extensions Below ////////////////////
// for strings
bool sdb_output::save(const char *tag, const char *text)
{
  for (const char *temp = text; *temp; temp++) {
    if (*temp == '\n') {
      // Multi-line text, save as a textblock
      textblock *temp = new textblock(text);
      bool success = save(tag, temp);
      delete temp;
      return success;
    }
  }
  
  if (text && *text) {
    selt_string buf;
    buf = "\\";
    buf += text;
    return write(lookup(SDB_STRING, &sdb_types[0]), tag, buf);
  } else {
    // there was nothing to write, so we'll say it was successful
    return TRUE;
  }
}

// for integers
bool sdb_output::save(const char *tag, const int number)
{
  // stringize integer
  selt_string textnum;
  textnum = number;

  // save it
  return write(lookup(SDB_INT, &sdb_types[0]), tag, textnum);
}

// for multi-line strings (textblocks)
bool sdb_output::save(const char *tag, textblock *block)
{
  bool success = TRUE;

  if (!block->get_text())
    return TRUE;

  // mark new record
  if (begin(lookup(SDB_TEXTBLOCK, &sdb_types[0]), tag) == FALSE)
    success = FALSE;

  // output data
  for (textblock_line *temp = block->get_text(); temp; temp = temp->get_next()) {
    selt_string buf;
    buf = "\\";
    buf += temp->get_text();
    if (write(lookup(SDB_LINE, &sdb_types[0]), buf) == FALSE) // maybe add in line counter
      success = FALSE; // if we fail writing even one line, we fail writing the whole thing
  }

  // end record
  end();

  return success;
}

//// SDB INPUT FUNCTIONS ////
void half_chop(const char *source, char *arg, char *dest)
{
  char *pos = one_argument(arg, dest);
  strcpy(dest, pos);
}

// chops and tags a loaded line to appropriate sdb-types
int sdb_input::process_line( selt_string *line )
{
  if (line == NULL || *line == "")
    return SDB_UNDEFINED;

  // first skip the hashes
  const char *read = *line;
  while (*read && *read == '#')
    read++;

  selt_string temp;

  // then extract the first word (the datatype);
  *line = read;
  half_chop(line, &temp, line);

  // figure out the SDB type
  int type = lookup(temp, &sdb_types[0]);

  // and return
  return type;
}


// takes the tag from the line
bool sdb_input::tag_line( selt_string *line, selt_string *tag )
{
  if (!line || !tag)
    return false;

  half_chop(line, tag, line);

  if (*tag == "")
    return false;

  return true;
}


// textblock parser
bool sdb_input::parse(textblock *block, int datatype, const char *tag, void *data)
{
  switch (datatype) {
  case SDB_LINE:
    {
      block->add_text((char *) data);
      delete [] (char *) data;
    }
    break;

  case SDB_END:
    // what can go wrong with textblocks that need verifying?
    break;

  default:
    log("SDB_INPUT: parse_textblock doesn't know what to do with the %s datatype.", lookup(datatype, &sdb_types[0]));
    return FALSE;
  }

  return TRUE;
}

// sdb memory allocater
void *sdb_input::sdb_create(int mode)
{
  void *item = NULL;

  // we're informed what kind of data type we are (mode)
  // so let's allocate some memory for the loading process
  // don't forget to initialize with default values!
  switch (mode) {
  case SDB_TEXTBLOCK:
    item = (void *) new textblock;
    break;

  case SDB_NOTEBOARD:
    item = (void *) new noteboard_data;
    break;

  case SDB_NOTE:
    item = (void *) new note_data;
    break;

  case SDB_CLAN:
    item = (void *) new clan_data;
    break;

  case SDB_TITLE:
    item = (void *) new title_data;
    break;

  case SDB_XPFILE:
    item = (void *) new llist<xp_data>;
    break;

  case SDB_XP:
    item = (void *) new xp_data;
    break;

    /*
  case SDB_AREA:
    item = (void *) new area_data;
    break;

  case SDB_EDESC:
    item = (void *) new edesc_data;
    break;

  case SDB_ROOM:
    item = (void *) new room_data;
    break;

  case SDB_EXIT:
    item = (void *) new exit_data;
    break;

  case SDB_HELP:
    item = (void *) new help_data;
    break;

  case SDB_HELPFILE:
    item = (void *) new llist<help_data>;
    break;

  case SDB_RACE:
    item = (void *) new race_data;
    break;

  case SDB_PROFESSION:
    item = (void *) new profession_data;
    break;

  case SDB_CLAN: //Terek_clan
    item = (void *) new clan_data;
    break;

  case SDB_SOCIAL:
    item = (void *) new social_data;
    break;

  case SDB_SOCIALLIST:
    item = (void *) new llist<social_data>;
    break;

  case SDB_PLAYER:
    item = (void *) new player_data;
    break;

  case SDB_MOB:
    item = (void *) new mob_data;
    break;

  case SDB_ALIAS:
    item = (void *) new alias_data;
    break;

  case SDB_OBJECT:
    item = (void *) new obj_data;
    break;

  case SDB_TERRAIN:
    item = (void *) new terrain_data;
    break;

  case SDB_TMSG:
    item = (void *) new terrain_msg_data;
    break;

  case SDB_OFILE:
    item = (void *) new ofile_data;
    break;

  case SDB_CHANNEL:
    item = (void *) new channel_data;
    break;
*/
  // ------------------------------------------------------------------
  // place other case statements here as required for new sdb-datatypes
  // ------------------------------------------------------------------

  // building block functions shouldn't need memory allocated (shouldn't be called)
  case SDB_INT:
  case SDB_STRING:
  case SDB_END:
  case SDB_LINE:
    break;

  case SDB_UNDEFINED:
    break;

  // we're lacking a parse call for the specified mode
  default:
    log("SDB_INPUT: Warning, no memory allocater for data type %s!", lookup(mode, &sdb_types[0]));
    break;
  }

  return item;
}

// sdb memory freer
bool sdb_input::sdb_free(void *data, int type)
{
  // make sure we have something to free
  if (!data)
    return TRUE;

  // we do - free it
  switch (type) {
  case SDB_TEXTBLOCK:
    delete (textblock *) data;
    break;

  case SDB_LINE:
  case SDB_STRING:
    delete [] (char *) data;
    break;

  case SDB_NOTEBOARD:
    delete (noteboard_data *) data;
    break;

  case SDB_NOTE:
    delete (note_data *) data;
    break;

  case SDB_CLAN:
    delete (clan_data *) data;
    break;

  case SDB_TITLE:
    delete (title_data *) data;
    break;

  case SDB_XPFILE:
    {
      llist<xp_data> *xpfile = (llist<xp_data> *) data;
      while (!xpfile->empty) {
        xp_data *temp = xpfile->headitem();
        xpfile->remove(temp);
        sdb_free(temp, SDB_XP);
      }
      delete xpfile;
    }
    break;

  case SDB_XP:
    delete (xp_data *) data;
    break;

    /*
  case SDB_EXIT:
    delete (exit_data *) data;
    break;

  case SDB_AREA:
    delete (area_data *) data;
    break;

  case SDB_EDESC:
    delete (edesc_data *) data;
    break;

  case SDB_ROOM:
    delete (room_data *) data;
    break;

  case SDB_HELP:
    delete (help_data *) data;
    break;

  case SDB_HELPFILE:
    {
      llist<help_data> *helpfile = (llist<help_data> *) data;
      while (!helpfile->empty) {
        help_data *entry = helpfile->headitem();
        helpfile->remove(entry);
        sdb_free(entry, SDB_HELP);
      }
      delete helpfile;
    }
    break;

  case SDB_RACE:
    delete (race_data *) data;
    break;

  case SDB_PROFESSION:
    delete (profession_data *) data;
    break;
  
  case SDB_CLAN: //Terek_clan
  delete (clan_data *) data;
  break;

  case SDB_SOCIAL:
    delete (social_data *) data;
    break;

  case SDB_SOCIALLIST:
    {
      llist<social_data> *socials = (llist<social_data> *) data;
      while (!socials->empty) {
        social_data *temp = socials->headitem();
        socials->remove(temp);
        sdb_free(temp, SDB_SOCIAL);
      }
      delete socials;
    }
    break;

  case SDB_PLAYER:
    delete (player_data *) data;
    break;

  case SDB_MOB:
    delete (mob_data *) data;
    break;

  case SDB_ALIAS:
    delete (alias_data *) data;
    break;

  case SDB_OBJECT:
    delete (obj_data *) data;
    break;

  case SDB_TERRAIN:
    delete (terrain_data *) data;
    break;

  case SDB_TMSG:
    delete (terrain_msg_data *) data;
    break;

  case SDB_OFILE:
    delete (ofile_data *) data;
    break;

  case SDB_CHANNEL:
    delete (channel_data *) data;
    break;
*/

  // ------------------------------------------------------------------
  // place other case statements here as required for new sdb-datatypes
  // ------------------------------------------------------------------

  // these ones are cool, don't have any data allocated
  case SDB_INT:
  case SDB_END:
  case SDB_UNDEFINED:
    break;

  // if this is called then you've probably got a memory leak
  default:
    log("SDB_INPUT: No data freeing routine for the %s type!  Memory Leak!", lookup(type, sdb_types));
    return FALSE;
  }

  return TRUE;
}


// The central SDB-LOAD routine, all sdb-input requests go through here
void *sdb_input::load(int mode)
{
  bool successful = FALSE;
  selt_string line, tag;
  void *data = NULL, *item = NULL;
  int type = SDB_UNDEFINED;

  // allocate memory for the item (if necessary)
  // add new memory allocaters in the sdb_create routine
  item = sdb_create(mode);

  // now we can read from file and loop, parsing or recursing
  // until we come to the SDB_END of this section or file
  do {
    // init for a pass (deallocate old memory if necessary)
    type = SDB_UNDEFINED;
    tag = "";
    line = "";

    // then get crackin'
    line = read_line(file);
    if (line != "") {
      // we've read a line, now hand it to the line processor to work out the datatype
      type = process_line(&line);

      // got the datatype, lets process it
      switch (type) {

      // -------------------------------------------------------------------
      // place tagged datastructures here   (ie: labelled as #<type> <tag> )
      case SDB_EXIT:
      case SDB_TEXTBLOCK:
      case SDB_NOTE:
        tag_line(&line, &tag);
        data = load(type);
        break;

      // -------------------------------------------------------------------
      // non-tagged datastructures go here
      case SDB_AREA:
      case SDB_EDESC:
      case SDB_ROOM:
      case SDB_HELP:
      case SDB_HELPFILE:
      case SDB_SOCIAL:
      case SDB_SOCIALLIST:
      case SDB_PLAYER:
      case SDB_MOB:
      case SDB_ALIAS:
      case SDB_OBJECT:
      case SDB_OFILE:
      case SDB_NOTEBOARD:
      case SDB_RACE:
      case SDB_TERRAIN:
      case SDB_TMSG:
      case SDB_PROFESSION:
      case SDB_CLAN:
      case SDB_TITLE:
      case SDB_CHANNEL:
      case SDB_XPFILE:
      case SDB_XP:
        data = load(type);
        break;

      // -------------------------------------------------------------------
      // building-block functions  (no need to tamper with code below)
      case SDB_INT:
        if (tag_line(&line, &tag)) {
          data = (void *) atoi(line);
        } else {
          log("SDB_INPUT: Integer without tag in file (%s)!", filename);
          continue;
        }
        break;

      case SDB_LINE:
        {
          const char *read = line;
          if (*read == '\\')
            read++;
          data = (void *) str_dup(read);
        }
        break;

      case SDB_STRING:
        if (tag_line(&line, &tag)) {
          const char *read = line;
          if (*read == '\\')
            read++;
          data = (void *) str_dup(read);
        } else {
          log("SDB_INPUT: String without tag in file (%s)!", filename);
          continue;
        }
        break;

      // end of this section.. allows processing before breaking the loop
      case SDB_END:
        break;

      case SDB_UNDEFINED:
        break;

      default:
        log("SDB_INPUT: No parsing available for the %s type!", lookup(type, &sdb_types[0]));
        break;
      }

      // -------------------------------------------------------------------

      // init for parse
      successful = FALSE;

      // parse the data
      switch (mode) {
        case SDB_TEXTBLOCK:
          successful = parse((textblock *) item, type, tag, data);
          break;

        case SDB_NOTEBOARD:
          successful = parse((noteboard_data *) item, type, tag, data);
          break;

        case SDB_NOTE:
          successful = parse((note_data *) item, type, tag, data);
          break;

        case SDB_CLAN:
          successful = parse((clan_data *) item, type, tag, data);
          break;
        
        case SDB_TITLE:
          successful = parse((title_data *) item, type, tag, data);
          break;
        
        case SDB_XPFILE:
          successful = parse((llist<xp_data> *) item, type, tag, data);
          break;

        case SDB_XP:
          successful = parse((xp_data *) item, type, tag, data);
          break;

          /*
        case SDB_AREA:
          successful = parse((area_data *) item, type, tag, data);
          break;
          
        case SDB_EDESC:
          successful = parse((edesc_data *) item, type, tag, data);
          break;

        case SDB_ROOM:
          successful = parse((room_data *) item, type, tag, data);
          break;

        case SDB_EXIT:
          successful = parse((exit_data *) item, type, tag, data);
          break;

        case SDB_HELP:
          successful = parse((help_data *) item, type, tag, data);
          break;

        case SDB_HELPFILE:
          successful = parse((llist<help_data> *) item, type, tag, data);
          break;

        case SDB_RACE:
          successful = parse((race_data *) item, type, tag, data);
          break;

        case SDB_PROFESSION:
          successful = parse((profession_data *) item, type, tag, data);
          break;

        case SDB_CLAN:
          successful = parse((clan_data *) item, type, tag, data);
          break;

        case SDB_SOCIAL:
          successful = parse((social_data *) item, type, tag, data);
          break;

        case SDB_SOCIALLIST:
          successful = parse((llist<social_data> *) item, type, tag, data);
          break;

        case SDB_PLAYER:
          successful = parse((player_data *) item, type, tag, data);
          break;

//        case SDB_MOB:
//          successful = parse((mob_data *) item, type, tag, data);
//          break;

        case SDB_ALIAS:
          successful = parse((alias_data *) item, type, tag, data);
          break;

        case SDB_TERRAIN:
          successful = parse((terrain_data *) item, type, tag, data);
          break;

        case SDB_TMSG:
          successful = parse((terrain_msg_data *) item, type, tag, data);
          break;

        case SDB_OBJECT:
          successful = parse((obj_data *) item, type, tag, data);
          break;

        case SDB_OFILE:
          successful = parse((ofile_data *) item, type, tag, data);
          break;

        case SDB_CHANNEL:
          successful = parse((channel_data *) item, type, tag, data);
          break;
*/

        // -------------------------------------------------------------------
        // add other parse calls here as required
        // -------------------------------------------------------------------

        // -------------------------------------------------------------------
        // building block functions shouldn't appear in mode
        case SDB_INT:
        case SDB_STRING:
        case SDB_END:
        case SDB_LINE:
          log("SDB_INPUT: Warning, load routine called with a building block routine! (%s)", lookup(mode, &sdb_types[0]));
          break;

        // -------------------------------------------------------------------
        // if we're parsing as undefined, then we're at the point of returning
        // the data to the mud, having completed the load proceedure
        case SDB_UNDEFINED:
          item = data;        // copy the data to 'item' (item is returned you see)
          loaded_data = item; // also copy to the 'loaded_data' (for calls to sdb->get_data)
          loaded_type = type; // and remember the 'loaded_type' (for calls to sdb->get_type)
          successful = TRUE;  // and say we were successful so we don't go freeing everything we just loaded
          break;

        // -------------------------------------------------------------------
        // we're lacking a parse call for the specified mode
        default:
          log("SDB_INPUT: Warning, no parser available for data type %s!", lookup(type, &sdb_types[0]));
          break;
      }

      // if the parse routines keep the data, they return TRUE, if they do not
      // they return FALSE and expect sdb->load to free the memory allocated
      if (data && successful == FALSE) // so if we have data, and the parse routines returned FALSE then
        sdb_free(data, type);          // free the data, as it wasn't saved (prevent memory leak)

      // set data to NULL ready for another pass
      data = NULL;
    } else {
      // we failed to read a line from the file (perhaps end of file or corrupt datafile)
      if (mode != SDB_UNDEFINED) {
        log("SDB_INPUT: Read line failed, file (%s) probably corrupt.", filename);
        successful = FALSE;  // (return a NULL value)
        // Future expansion: see if we can parse a SDB_END... maybe we can recover despite the error
      } else {
        // error occured while we were in the last phase of loading, someone probably just
        // forgot to place an '#end' in the file, so the data is probably intact, let's
        // cross our fingers and return the data anyway
        successful = TRUE;
      }

      // bail out
      type = SDB_END;
    }
  } while (type != SDB_END);

  // then we return our finished structure, or NULL if the
  // parser said that it wasn't allowable...
  if (!successful) {
    // free the data
    sdb_free(item, type);
    return NULL;
  } else {
    return item;
  }
}


// constructor
sdb_input::sdb_input(const char *name)
{
  loaded_type = SDB_UNDEFINED;
  loaded_data = NULL;

  if (name && *name) {
    filename = str_dup(name);
    for (char *temp = filename; *temp; temp++)
      *temp = tolower(*temp);
  } else
    filename = NULL;

  if (filename) {
    file = fopen(filename, "rb");
  } else
    file = NULL;

  if (!file) {
    log("SYSERR: SDB_INPUT was unable to read from '%s'", filename);
  } else {
    char *line = read_line(file);
    if (line && !str_cmp(line, "#sdb")) {
      log("  * sdb_input: %s", filename);
      load(SDB_UNDEFINED);  // do the actual load honours
    } else {
      log("SYSERR: SDB_INPUT could not find the SDB header in '%s'", filename);
      fclose(file);
      file = NULL;
    }

    if (line)
      delete [] line;
  }
}

// destructor
sdb_input::~sdb_input()
{
  // close the outfile
  if (file) {
    fclose(file);
    file = NULL;
  }

  // And free the stuff
  if (filename)
    delete [] filename;
}

