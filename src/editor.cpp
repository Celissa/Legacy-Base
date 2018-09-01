/*
 * NOTE: Buffer Limit Unimplemented.
 * Means you can overflow the MAX_STRING_LENGTH if you're not careful
 */

// made undo function
// save the reverse of every edit, eg note 23 sdf sdf - save 23 first
// note 22 sdf sdf, delete line 23
// note->html
// help->html





#include "system.h"

// -------------------- code below -------------------- 
textblock::textblock()
{
  init();
}

textblock::textblock(const char *string)
{
  init();
  set_text(string);
}

textblock::textblock(textblock *block)
{
  init();
  set_text(block);
}

textblock::~textblock()
{
  clear();
  if (tstring)
    delete [] tstring;
}

void textblock::init()
{
  text = NULL;
  tstring = NULL;
}

void textblock::clear() {
  textblock_line *freeme;

  while (text) {
    freeme = text;
    text = text->get_next();
    delete freeme;
  }
}


// the all-important editor
void textblock::edit(const char *line_to_add, char_data *ch)
{
  textblock_line *line, *prev;
  char temp[MAX_INPUT_LENGTH], *tp;
  int num, num2, i;

  if (!line_to_add || !*line_to_add) {
    list(ch);
    return;
  }
  
  if (!text) { // create a new text buffer 
    while (isdigit(*line_to_add))
      line_to_add++;

    if (!*line_to_add) {
      list(ch);
      return;
    }

    if (*line_to_add == '\\')
      line_to_add++;

    if (get_size() + strlen(line_to_add) < MAX_STRING_LENGTH) {
      text = new textblock_line(line_to_add);
      send(ch, "Line added.\r\n");
      if( is_set( ch->pcdata->pfile->flags, PLR_SPAMBACK ) ) 
        list(ch);
    } else {
      send(ch, "Processing this request would crash the mud.  Operation cancelled.\r\n");
      return;
    }
  } else if (isdigit(*line_to_add)) {   // if a numeric argument in the line to add, then replace/delete/insert/format 
    // figure out what kinda line we have to process 
    tp = temp;
    *tp = '\0';
    while (isdigit(*line_to_add)) // get number 1 
      *tp++ = *line_to_add++;
    *tp = '\0';
    num = atoi(temp);

    // now skip spaces to find out what the next string is 
    while (isspace(*line_to_add))
      line_to_add++;

    if (*line_to_add == '-') { // formatting 
      line_to_add++;
      // now skip spaces to find out what the next string is 
      while (isspace(*line_to_add))
        line_to_add++;
      if (*line_to_add && !isdigit(*line_to_add)) {
        send(ch, "Invalid format parameter (use '\\-' if you would like a '-').\r\n");
        return;
      } else {
        tp = temp;
        *tp = '\0';
        while (isdigit(*line_to_add)) // get number 2 
          *tp++ = *line_to_add++;
        *tp = '\0';
        if (*temp)
          num2 = atoi(temp);
        else
          num2 = -1; // -1 is defined as format every line after start_line 
      }

      if (num == num2)
        num2++;

      if (format(num, num2, 75)) {
        send(ch, "Text formatted.\r\n");
        if( is_set( ch->pcdata->pfile->flags, PLR_SPAMBACK ) ) 
          list(ch);
      } else
        send(ch, "Processing this format would crash the mud.  Format cancelled.\r\n");
      return;
    }

    // if it's not formatting, then it must be an insert/delete/replace 
    if (*line_to_add == '\\')
      line_to_add++;

    if (num % 2) { // if num / 2 has a remainder then it's an odd number, meaning a replace or delete 
      line = text;
      prev = NULL;
      for (i = 1; i < num; i += 2) {
        if (line->get_next()) {   
          prev = line;
          line = line->get_next();
        } else if (*line_to_add) { // not that many lines - add a new one 
          if (get_size() + strlen(line_to_add) >= MAX_STRING_LENGTH) {
            send(ch, "Processing this request would crash the mud.  Operation cancelled.\r\n");
            return;
          }

          line->set_next(new textblock_line(line_to_add));
          send(ch, "Line added.\r\n");
          if( is_set( ch->pcdata->pfile->flags, PLR_SPAMBACK ) ) 
            list(ch);
          return;
        } else { // ignore the call, trying to replace/delete a non-existant line 
          return;
        }
      }

      // if we get this far then do the actual replace or delete 
      if (*line_to_add) { // just change the text 
        if (get_size() - line->get_length() + strlen(line_to_add) >= MAX_STRING_LENGTH) {
          send(ch, "Processing this request would crash the mud.  Operation cancelled.\r\n");
          return;
        }

        if (!prev) {
          text = new textblock_line(line_to_add);
          text->set_next(line->get_next());
        } else {
          prev->set_next(new textblock_line(line_to_add));
          prev = prev->get_next();
          prev->set_next(line->get_next());
        }

        delete line;
        send(ch, "Line replaced.\r\n");
        if( is_set( ch->pcdata->pfile->flags, PLR_SPAMBACK ) ) 
          list(ch);

      } else { // delete that member, and change pointers to reflect 
        if (prev == NULL) { // deleting first entry 
          text = line->get_next();
        } else {
          prev->set_next(line->get_next());
        }

        delete line;
        send(ch, "Line deleted.\r\n");
        if( is_set( ch->pcdata->pfile->flags, PLR_SPAMBACK ) ) 
          list(ch);
      }
  
    } else { // it's an insert (even number) 
      if (get_size() + strlen(line_to_add) >= MAX_STRING_LENGTH) {
        send(ch, "Processing this request would crash the mud.  Operation cancelled.\r\n");
        return;
      }
      
      line = text;
      prev = NULL;
      for (i = 0; i < num; i += 2) {
        if (line->get_next()) {
          prev = line;
          line = line->get_next();
        } else { // not that many lines - add a new one 
          line->set_next(new textblock_line(line_to_add));
          send(ch, "Line added.\r\n");
          if( is_set( ch->pcdata->pfile->flags, PLR_SPAMBACK ) ) 
            list(ch);
          return;
        }
      }
      // do the insert 
      if (prev == NULL) { // insert at front of block 
        prev = new textblock_line(line_to_add);
        prev->set_next(text);
        text = prev;
      } else { // insert in middle of block 
        line = prev->get_next();
        prev->set_next(new textblock_line(line_to_add));
        prev = prev->get_next();
        prev->set_next(line);
      }
      send(ch, "Line inserted.\r\n");
      if( is_set( ch->pcdata->pfile->flags, PLR_SPAMBACK ) ) 
        list(ch);
    }

  } else { // just add the line as per usual method 
    if (*line_to_add == '\\')
      line_to_add++;

    if (get_size() + strlen(line_to_add) >= MAX_STRING_LENGTH) {
      send(ch, "Processing this request would crash the mud.  Operation cancelled.\r\n");
      return;
    }

    for (line = text; line->get_next(); line = line->get_next());
    line->set_next(new textblock_line(line_to_add));
    send(ch, "Line added.\r\n");
    if( is_set( ch->pcdata->pfile->flags, PLR_SPAMBACK ) ) 
      list(ch);
  }

  return;
}

// find each line ending and send them one at a time to the character with a [number] appended 
void textblock::list(char_data *ch)
{
  if (!text)
    return;

  int count = 1;
  char buf[MAX_STRING_LENGTH];

  for (textblock_line *line = text; line; line = line->get_next()) {
    sprintf(buf, "[%3d] %s\r\n", count, line->get_text() ? line->get_text() : "");
    page( ch, buf );
    count += 2;
  }
}


// find each line ending and send them one at a time to the character 
void textblock::show(char_data *ch)
{
  if (!text)
    return;

  for (textblock_line *line = text; line; line = line->get_next())
    page( ch, line->get_text() ? line->get_text() : "" );
}


// formats a textblock using the enhanced sact formatting routine 
bool textblock::format(const int start_line, const int end_line, const int cols)
{
  int i, format_all = FALSE;
  textblock_line *line, *line_before, *line_after;
  selt_string buf, temp;

  if (end_line == -1)
    format_all = TRUE;
  else if (start_line >= end_line)
    return true;

  line_before = NULL;
  line_after = NULL;
  
  int bytes = 0;
  // find what lines to format
  for (line = text, i = 1; line; line = line->get_next(), i += 2) {
    if (i < start_line)
      line_before = line;
    else if (!format_all && (i >= end_line - 1)) {
      line_after = line->get_next();
      temp += line->get_text();
      temp += "\r\n";
      bytes += line->get_length() + 2;
      break;
    } else {
      temp += line->get_text();
      temp += "\r\n";
      bytes += line->get_length() + 2;
    }
  }

  if (temp == "")
    return true;

  // format the string using the string formatting routine in selt_misc.cpp
  if (line_before)
    ::format(temp, &buf, cols, FORMAT_NOCAP);
  else
    ::format(temp, &buf, cols, FORMAT_TEXTBLOCK);

  if (get_size() - bytes + buf.length() > MAX_STRING_LENGTH)
    return false;
  
  if (line)
    line->set_next(NULL); // disconnect old segment and delete
  for (line = (line_before ? line_before->get_next() : text); line; ) {
    textblock_line *prev = line;
    line = line->get_next();
    delete prev;
  }
  
  textblock temp_block;
  temp_block.set_text(buf);

  // update all the linked list bits so we can get this finished
  if (line_before)
    line_before->set_next(temp_block.get_text());
  else
    text = temp_block.get_text();

  if (line_after) {
    for (line = text; line && line->get_next(); line = line->get_next());
      line->set_next(line_after);
  }
  
  temp_block.set_text((textblock_line *) NULL); // set to NULL so it doesn't take out our formatted text when we delete it
  return true;
}


// converts a textblock to a string and allocates the memory for it 
char *textblock::get_string( bool strip )
{
  char *buf = NULL;

  textblock_line *line = text;

  if( strip )
    while( line && !line->get_text( ) )
      line = line->get_next( );

  for( ; line; line = line->get_next( ) ) {
    buf = strallocat( buf, line->get_text( ) );
    if( line->get_next( ) )
      buf = strallocat( buf, "\r\n" );
  }

  if( strip && buf )
    for( char *temp = buf + strlen( buf ) - 1; temp > buf && ( *temp == '\r' || *temp == '\n' ); temp-- )
      *temp = '\0';

  return buf;
}


// converts a string to a textblock and allocates the memory for it
// may not preserve blank lines
void textblock::add_text(const char *string)
{
  const char *read;
  char *write, *temp;
  
  // skip to end of block
  textblock_line *line;
  for (line = text; line && line->get_next(); line = line->get_next());

  if (!string || !*string) {
    if (!text) {
      text = new textblock_line(NULL);
    } else {
      line->set_next(new textblock_line(NULL));
    }
    return;
  }

  temp = new char [strlen(string) + 1];

  write = temp;
  *write = '\0';

  for (read = string; ; read++) {
    if (*read == '\r')
      continue;
    else if (*read == '\n') {
      *write++ = '\0';
      if (!text) {
        text = new textblock_line(temp);
        line = text;
      } else {
        line->set_next(new textblock_line(temp));
        line = line->get_next();
      }
      write = temp;
      *write = '\0';
      continue;
    } else if (*read == '\0') {
      *write++ = '\0';
      if (*temp) {
        if (!text) {
          text = new textblock_line(temp);
          line = text;
        } else {
          line->set_next(new textblock_line(temp));
          line = line->get_next();
        }
      }
      break;
    } else
      *write++ = *read;
  }

  delete [] temp;
}

void textblock::add_text(const textblock *block)
{
  textblock_line *line;

  // skip to end of block
  for (line = text; line && line->get_next(); line = line->get_next());

  // and append new block (discarded const on block since it was bitchen)
  for (textblock_line *temp = ((textblock*) block)->get_text(); temp; temp = temp->get_next())
  {
    if (!text) {
      text = new textblock_line(temp->get_text());
      line = text;
    } else {
      line->set_next(new textblock_line(temp->get_text()));
      line = line->get_next();
    }
  }
}

void textblock::set_text(const char *string)
{
  if (text)
    clear();

  add_text(string);
}

void textblock::set_text(const textblock *block)
{
  if (text)
    clear();

  add_text(block);
}

// duplicates a textblock 
textblock *textblock::duplicate()
{
  if (!text)
    return NULL;
  else
    return new textblock(this);
}


int textblock :: IndexOfName( const char* name )
{
  textblock_line* line = text;
  int i = 0;

  while( line ) {
    selt_string temp, temp2;
    half_chop( line->get_text( ), &temp, &temp2, "=" );
    if( temp == name )
      return i;
    line = line->get_next( );
    i++;
  }

  return -1;
}

CBString textblock :: ValueOfName( const char* name )
{
  textblock_line* line = text;
  int i = 0;

  while( line ) {
    CBStringList list;
    list.split( line->get_text( ), "=" );
    if( list[ 0 ] == name )
      return list[ 1 ];
    line = line->get_next( );
    i++;
  }

  return "";
}

void textblock :: SetValue( const char* name, const char* value )
{
  selt_string temp;
  temp = name;
  temp += "=";
  temp += value;

  int index = IndexOfName( name );
  if( index < 0 )
    add_text( temp );
  else {

  }
}


/* formats a string to specified column ... there are a couple of issues you may not like, such as auto-caps, auto-remove spaces, etc */
void format(const char *text, selt_string *dest, unsigned int cols, const int type) {
  int previous = CHAR_PERIOD, softbreaks, cap_next_char;
  unsigned int num_lines, line_len, wordlen;
  
  if (type != FORMAT_NOCAP)
    cap_next_char = TRUE;
  else
    cap_next_char = FALSE;

  // Get rows and col data from the char...
  if (!cols)
    cols = 75;

  if (!text || !*text)
    return;

  selt_string temp;
  const char *read = text;

  // Strip linefeeds and multiple spaces, and format sentence ends with two spaces.
  while (*read) {
    switch (*read) {
    case '\r':
    case ' ':
      if (previous != CHAR_SPACE) {
        if (previous != CHAR_NORMAL && previous != CHAR_DOTDOTDOT && (type == FORMAT_TEXTBLOCK || type == FORMAT_NOCAP)) {
          temp += ' ';
          cap_next_char = TRUE;
        }
        temp += ' ';
        previous = CHAR_SPACE;
      }
      break;
    case '\n':
      break;
    case '?':
    case '!':
      temp += *read;
      previous = CHAR_OTHER;
      break;
    case '.':
      if (previous == CHAR_PERIOD)
        previous = CHAR_DOTDOTDOT;
      temp += *read;
      if (previous != CHAR_DOTDOTDOT)
        previous = CHAR_PERIOD;
      break;
    case '@': /* ignore colour code */
      temp += *read++;
      switch (*read) {
      case 'n':
      case 'e':
      case 'r':
      case 'R':
      case 'g':
      case 'G':
      case 'b':
      case 'B':
      case 'y':
      case 'Y':
      case 'm':
      case 'M':
      case 'c':
      case 'C':
      case 'w':
      case 'W':
      case 'I':
        temp += *read;
        break;
      default:
        read--;
        break;
      }
      break;
    default:
      if (cap_next_char) {
        temp += (char) toupper(*read);
        cap_next_char = FALSE;
      } else {
        temp += *read;
      }
      previous = CHAR_NORMAL;
    }
    read++;
  }


  line_len = 0;
  num_lines = 0;
  softbreaks = FALSE;
  wordlen = 0;

  read = temp;

  // set softbreak(s) to true if lenth of string is greater than the players screen width
  if (cstrlen(read) > cols)
    softbreaks = TRUE;

  while (*read) {
    // scan ahead and work out word length
    selt_string word;
    for (;;) { // read all pre-spaces
      if (*read == ' ')
        word += *read++;
      else
        break;
    }

    if (!*read)
      break; // only got whitespace at the end left over, discard

    for (;;) { // read the word
      word += *read++;
      if (!*read || *read == ' ') 
        break;
    }

    if (word == "") // nothing to see here... probly an unnecessary check
      break;

    wordlen = cstrlen(word);
    const char *temp = word;
    // check to see if the word we just extracted is too long to fit on the current line
    if (wordlen + line_len > cols) { // we need a line break
      if (wordlen > cols / 2) { // if it's a long word, then force-wrap after a portion
        while (line_len++ < cols)
          *dest += *temp++;
        while (cstrlen(temp) > cols) {
          *dest += "\r\n";
          line_len = 0;
          if (type == FORMAT_COMMS) {
            *dest += "  ";
            line_len += 2;
          }
          num_lines++;
          while (line_len++ < cols)
            *dest += *temp++;
        }
      }

      *dest += "\r\n";
      line_len = 0;
      if (type == FORMAT_COMMS) {
        *dest += "  ";
        line_len += 2;
      }

      num_lines++;
      while (*temp == ' ')
        temp++;

      *dest += temp;
      line_len += cstrlen(temp);

    } else {
      *dest += word;
      line_len += cstrlen(word);
    }

    /* / copy word from temp space into outstring
    for(;;) {
      if ((*write++ = *temp++) == '%') {
        if ((softbreaks && *temp == '$') || *temp == '\\') {
          *write++ = '\\';
          temp++;
          line_len = 0;
          if (type == FORMAT_COMMS) {
            *write++ = ' ';
            *write++ = ' ';
            line_len += 2;
            wordlen = cstrlen(temp);
          }
          num_lines++;
        } else if (*temp == '%' ) {
          *write++ = '%';
          temp++;
        }
      }

      if (!*temp) { // end of word
        line_len += wordlen;
        break;
      }
    }
    */

    // maybe copy correct whitespace?
  }
  
  *dest += "\r\n";
}


/* Calculates the length of a string with colour codes ignored.*/
unsigned int cstrlen(const char *text)
{
  unsigned int length;

  length = 0;

  if (!*text) {
    return 0;
  }

  for (;;) {
    if (*text == '\0') {
      return length;
    } else {
      if (*text == '@') { /* take into account colour codes */
        switch (*(++text)) {
        case 'n':
        case 'e':
        case 'r':
        case 'R':
        case 'g':
        case 'G':
        case 'b':
        case 'B':
        case 'y':
        case 'Y':
        case 'm':
        case 'M':
        case 'c':
        case 'C':
        case 'w':
        case 'W':
        case 'I':
          break;
        default:
          length += 2;
          break;
        }
      }/* else if (*text == '%') {  take into account the force para character 
        switch (*(++text)) {
        case '\\':
          break;
        case '$':
        case '%':
          length++;
          break;
        default:
          length += 2;
          break;
        }
      } */ else {
        length++;
      }
    }
    text++;
  }

  return length;
}
