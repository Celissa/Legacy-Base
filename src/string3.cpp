/* ************************************************************************
*   File: selt_string.cpp                  Selt's Sweet CircleMUD Strings *
*  Usage: MagiStrings(TM)                                                 *
*   (c) 1998+ Julian Forsythe (julian@rainchild.com )                     *
************************************************************************ */


#include "system.h"

// concatenate a string to the exisiting buffer
void selt_string::bufcat(const char *str2)
{
  if (!str2 || !*str2)
    return;

  if (!text) {
    // no buffer to concat to, so make a new one and put str2 into it
    bufsize = strlen(str2) + BUFSIZE;
    record_new( bufsize + 1, -MEM_STRING );
    text = new char[bufsize + 1];
    strcpy(text, str2);
  } else {
    // got a buffer, test the size of it
    char *newstr = NULL;
    int newstringsize = strlen(text) + strlen(str2);
    if (bufsize <= newstringsize) {
      // too big for buffer, create new one
      int oldbufsize = bufsize;
      bufsize = newstringsize + BUFSIZE;     // calculate required buffer size
      record_new( bufsize + 1, -MEM_STRING );
      newstr = new char[bufsize + 1];        // allocate memory for new string
      strcpy(newstr, text);                  // copy original text into new buffer
      record_delete( oldbufsize + 1, -MEM_STRING );
      delete [] text;                        // free old buffer
    } else {
      // buffer can handle the extra string
      newstr = text;
    }
    strcat(newstr, str2);
    text = newstr;
  }
}

// toss out old string and put in new one
void selt_string::set(const char *str2)
{
  // copy to temp variable so you can do string = itself without crashing
  char *temp = text;

  int oldbufsize = bufsize;

  // set buffer to NULL so we're starting afresh
  text = NULL;
  bufsize = 0;

  // copy the text now
  bufcat(str2);

  // free temp memory after the set, if necessary
  if (temp) {
    record_delete( oldbufsize + 1, -MEM_STRING );
    delete [] temp;
  }
}

// printf overloads.......................................................
void selt_string::_printf(const char *format, ...)
{
  // get arglist out for the printf
  va_list arglist;
  va_start(arglist, format);

  _vprintf(format, arglist);
}

// standard printf overloads
void selt_string::printf(const char *format, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, const char *arg10)
{
  _printf(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

void selt_string::printf(const char *format, const char *arg1, const char *arg2, const char *arg3, int arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, const char *arg10)
{
  _printf(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

void selt_string::printf(const char *format, const char *arg1, const char *arg2, int arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, const char *arg10)
{
  _printf(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

void selt_string::printf(const char *format, const char *arg1, int arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, const char *arg10)
{
  _printf(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

void selt_string::printf(const char *format, int arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, const char *arg10)
{
  _printf(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

// misc printf overloads
void selt_string::printf(const char *format, const char *arg1, void *arg2)
{
  _printf(format, arg1, arg2);
}

void selt_string::printf(const char *format, int arg1, int arg2)
{
  _printf(format, arg1, arg2);
}

void selt_string::printf(const char *format, const char *arg1, int arg2, const char *arg3, int arg4, const char *arg5, const char *arg6)
{
  _printf(format, arg1, arg2, arg3, arg4, arg5, arg6);
}

void selt_string::printf(const char *format, const char *arg1, int arg2, int arg3, const char *arg4, const char *arg5, const char *arg6)
{
  _printf(format, arg1, arg2, arg3, arg4, arg5, arg6);
}


// catf overloads.......................................................
void selt_string::_catf(const char *format, ...)
{
  // get arglist
  va_list arglist;
  va_start(arglist, format);

  selt_string temp;
  temp._vprintf(format, arglist);

  bufcat(temp);
}

// standard catf overloads
void selt_string::catf(const char *format, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, const char *arg10)
{
  _catf(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

void selt_string::catf(const char *format, const char *arg1, const char *arg2, const char *arg3, int arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, const char *arg10)
{
  _catf(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

void selt_string::catf(const char *format, const char *arg1, const char *arg2, int arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, const char *arg10)
{
  _catf(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

void selt_string::catf(const char *format, const char *arg1, int arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, const char *arg10)
{
  _catf(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

void selt_string::catf(const char *format, int arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6, const char *arg7, const char *arg8, const char *arg9, const char *arg10)
{
  _catf(format, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}

// misc catf overloads
void selt_string::catf(const char *format, int arg1, int arg2)
{
  _catf(format, arg1, arg2);
}

#ifdef MYSTERIA_UNIX
#define _vsnprintf vsnprintf
#endif

// the selt printf routine >:)
void selt_string::_vprintf(const char *format, va_list arglist)
{
  int oldbufsize = bufsize;

  // create a temp backup of the string so you can pass itself
  char *temp = text;
  bufsize = 0;
  text = NULL;

  // if there's no text to handle, might as well quit
  if (!format || !*format) {
    if (temp) {
      record_delete( oldbufsize + 1, -MEM_STRING );
      delete [] temp;
    }
    return;
  }

  // set buffer to NULL so we're starting afresh
  bufsize = BUFSIZE; // reserve memory for this allocation
  record_new( bufsize + 1, -MEM_STRING );
  text = new char[bufsize + 1];

  while (1) {
    int bytes = _vsnprintf(text, bufsize, format, arglist);
    if (bytes > -1 && bytes < bufsize)
      return;
    record_delete( bufsize + 1, -MEM_STRING );
    delete [] text;
    bufsize *= (bufsize < 10000 ? 4 : 2);  // increase the buffer (steps: 256, 1k, 4k, 16k, 32k, 64k, 128k, etc)
    record_new( bufsize + 1, -MEM_STRING );
    text = new char[bufsize + 1];
  }

  /*
//  va_start(arglist, format);

  char read;
  while ((read = *format++) != '\0') {

enum CHARTYPE {
    CH_OTHER,           // character with no special meaning
    CH_PERCENT,         // '%'
    CH_DOT,             // '.'
    CH_STAR,            // '*'
    CH_ZERO,            // '0'
    CH_DIGIT,           // '1'..'9'
    CH_FLAG,            // ' ', '+', '-', '#'
    CH_SIZE,            // 'h', 'l', 'L', 'N', 'F', 'w'
    CH_TYPE             // type specifying character
};
    
    switch (read) {
    case '%':
      break;
    }

    if (read == '%') {
      // nasty, we have to do work ;)
      switch (*(++read)) {
      case 'd':
        // digit
        *this += va_arg(arglist, int);
        break;

      case 'c':
        // single character
        *this += va_arg(arglist, char);
        break;

      case 's': {
        // string
        char *pointer = va_arg(arglist, char *);
        if (pointer == NULL) {
          *this += "(null)";
        } else if (pointer == (char *) 0xC0FFEE) {
          pointer = va_arg(arglist, char *);
          *this += pointer;
          pointer = va_arg(arglist, char *);
        } else if (*(int *) pointer == 0xC0FFEE) {
          string *s = (string *) pointer;
          *this += *s;
        } else {
          *this += pointer;
        }
      } break;

      case 'p':
        // pointer
        break;

      case '%':
        // real percent
        break;

      default:
        *this += "%<unhandled>";
        *this += *read;
        va_arg(arglist, int);
      }

    } else {
      *this += *read;
    }
  }

  va_end(arglist);
*/

  // free temp memory after the set, if necessary
  if (temp) {
    record_delete( oldbufsize + 1, -MEM_STRING );
    delete [] temp;
  }
}

// construct blank string
void selt_string::init()
{
  record_new( sizeof( selt_string ), MEM_STRING );
  string_key = 0xC0FFEE;
  text = NULL;
  bufsize = 0;
}

selt_string :: selt_string()
{
  init();
}

// construct string with text provided
selt_string :: selt_string( selt_string &str2 )
{
  init();
  set(str2);
}

// construct string with text provided
selt_string :: selt_string(const char *str2)
{
  init();
  set(str2);
}

// construct string with number provided
selt_string :: selt_string(const int value)
{
  init();

  char temp[128];
  sprintf(temp, "%d", value);
  set(temp);
}


// construct string with two strings provided
selt_string :: selt_string(const char *str1, const char *str2)
{
  init();
  set(str1);
  bufcat(str2);
}

// free strin
selt_string ::~ selt_string()
{
  if (text) {
    record_delete( bufsize + 1, -MEM_STRING );
    delete [] text;
  }
  record_delete( sizeof( selt_string ), MEM_STRING );
}

unsigned int selt_string::length()
{
  if (text)
    return strlen(text);

  return 0;
}

const selt_string &selt_string::operator=(const char *str2)
{
  set(str2);
  return *this;
}

const selt_string &selt_string::operator=(const selt_string &str2)
{
  set(str2.text);
  return *this;
}

const selt_string &selt_string::operator=(const selt_string *str2)
{
  set(str2->text);
  return *this;
}

const selt_string &selt_string::operator=(int value)
{
  char temp[128];
  sprintf(temp, "%d", value);

  set(temp);
  return *this;
}

const selt_string &selt_string::operator+=(const char *str2)
{
  bufcat(str2);
  return *this;
}

const selt_string &selt_string::operator+=(const selt_string &str2)
{
  bufcat(str2.text);
  return *this;
}

const selt_string &selt_string::operator+=(int value)
{
  char temp[128];
  sprintf(temp, "%d", value);

  bufcat(temp);
  return *this;
}

const selt_string &selt_string::operator+=(char value)
{
  char temp[2];
  temp[0] = value;
  temp[1] = '\0';
  bufcat(temp);

  return *this;
}

const selt_string &selt_string::operator--()
{
  if (text) {
    unsigned int len = strlen(text);
    if (len > 0)
      text[strlen(text) - 1] = '\0';
    else {
      record_delete( bufsize + 1, -MEM_STRING );
      delete [] text;
      text = NULL;
      bufsize = 0;
    }
  }

  return *this;
}

const selt_string &selt_string::operator--(int)
{
  if (text) {
    unsigned int len = strlen(text);
    if (len > 0)
      text[strlen(text) - 1] = '\0';
    else {
      record_delete( bufsize + 1, -MEM_STRING );
      delete [] text;
      text = NULL;
      bufsize = 0;
    }
  }

  return *this;
}

bool selt_string::operator==(const char *string)
{
  if ((!string || !*string) && !text)
    return true;
  else if ((!string || !*string) || !text)
    return false;
  else if (!str_cmp(string, text))
    return true;

  return false;
}

bool selt_string::operator!()
{
  return (text && *text ? true : false);
}

bool selt_string::operator!=(const char *string)
{
  return !operator==(string);
}

selt_string::operator const char * ()
{
  return text;
}

bool selt_string::contains(const char value, bool ignore_double)
{
  char *temp;

  if (!text || !(temp = strchr(text, value)))
    return false;

  while (ignore_double && temp[1] == value)
    temp = strchr(&temp[1], value);

  if (temp)
    return true;
  return false;
}

bool selt_string::contains(const char *value)
{
  if (!text || !strstr(text, value))
    return false;

  return true;
}


void selt_string::chop(selt_string *to, const char separator, bool ignore_double)
{
  if (!to)
    return;

  char *temp;
  if (!text || !(temp = strchr(text, separator))) {
    *to = text;
    set(NULL);
    return;
  }

  while (temp && ignore_double && temp[1] == separator)
    temp = strchr(&temp[2], separator);

  if (!temp) {
    *to = text;
    set(NULL);
    return;
  }
  
  *temp = '\0';
  *to = text;
  set(&temp[1]);
}


int selt_string::subst(const char *token, const char *substitute)
{
  if (!token || !substitute)
    return 0;

  int num = 0, len = strlen(token);
  char *prev = text, *pos = NULL;
  selt_string temp;
  
  // find all occurances of token, using strstr
  while (prev && (pos = strstr(prev, token)) != NULL) {
    *pos = '\0';
    temp += prev;
    temp += substitute;
    prev = pos + len;
    num++;
  }

  if (prev)
    temp += prev;

  if (num)
    set(temp);

  return num;
}


void selt_string::truncate( int len, bool pad )
{
  if( !text || !*text )
    return;

  int tlen = length();

  if( pad && tlen < len ) {
    _printf( "%-*s", len, text );
  } else if( tlen > len ) {
    strcpy( text + len - 3, "..." );
  }
}


void selt_string::capitalize()
{
  if( !text || !*text )
    return;

  int pos = 0;
  while( text[ pos ] == '\r' || text[ pos ] == '\n' )
    pos++;

  if( text[ pos ] == '\x1B' ) {
    // special case -- ansi escape sequence
    for( int i = pos; text[ i ] != '\0'; i++ ) {
      if( text[ i ] == 'm' || text[ i ] == 'H' || text[ i ] == 'J' || text[ i ] == 'r' || text[ i ] == 'z' ) {
        text[ i + 1 ] = toupper( text[ i + 1 ] );
        return;
      }
    }

    bug( "Capitalize: Missing end of escape code?" );
    bug( "-- arg = %s", text );
    return;
  }

  text[ pos ] = toupper( text[ pos ] );

  return;
}


void selt_string::make_lowercase()
{
  if( !text || !*text )
    return;

  for( char* temp = text; *temp; temp++ )
    *temp = tolower( *temp );
}

void selt_string::make_uppercase()
{
  if( !text || !*text )
    return;

  for( char* temp = text; *temp; temp++ )
    *temp = toupper( *temp );
}

// friend operators below ----------------------------------------
selt_string operator+(selt_string &str1, selt_string &str2)
{
  return selt_string(str1, str2);
}

selt_string operator+(selt_string &str1, const char *str2)
{
  return selt_string(str1, str2);
}

selt_string operator+(const char *str1, selt_string &str2)
{
  return selt_string(str1, str2);
}


// utils ---------------------------------------------------------
bool isnumber(const char *str)
{
  if (!str || !*str)
    return false;

  while (*str)
    if (!isdigit(*(str++)))
      return false;

  return true;
}

bool isnumber(selt_string *str)
{
  return isnumber(*str);
}

int atoi(selt_string *str)
{
  return atoi(*str);
}

// returns true if the value is a delimeter
bool isdelimeter(const char value, const char *delimeters)
{
  for (; *delimeters; delimeters++) {
    if (*delimeters == ' ' && isspace(value)) {
      return true;
    } else if (*delimeters == value) {
      return true;
    }
  }

  return false;
}

void half_chop( const char* src, selt_string *arg1, selt_string *arg2, const char *delimeters )
{
  if (!src || !arg1 || !arg2 || !delimeters)
    return;

  bool got = false;

  *arg1 = "";
  const char *read;

  for (read = src; read && *read; read++)
  {
    if (isdelimeter(*read, delimeters)) {
      if (!got && arg1[0] != '\0')
        got = true;
      continue;
    } else {
      if (got)
        break;
      else 
        *arg1 += *read;
    }
  }

  *arg2 = read;
}

void half_chop(selt_string *source, selt_string *arg1, selt_string *arg2, const char* delimeters)
{
  selt_string src = *source;
  half_chop(src, arg1, arg2, delimeters);
}

void half_chop( const char* src, CBString *arg1, CBString *arg2, const char *delimeters )
{
  if (!src || !arg1 || !arg2 || !delimeters)
    return;

  bool got = false;

  *arg1 = "";
  const char *read;

  for (read = src; read && *read; read++)
  {
    if (isdelimeter(*read, delimeters)) {
      if (!got && arg1->character(0) != '\0')
        got = true;
      continue;
    } else {
      if (got)
        break;
      else 
        *arg1 += *read;
    }
  }

  *arg2 = read;
}

void half_chop(CBString *source, CBString *arg1, CBString *arg2, const char* delimeters)
{
  CBString src = *source;
  half_chop(src, arg1, arg2, delimeters);
}
