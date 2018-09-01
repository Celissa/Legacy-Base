/* ************************************************************************
*   File: selt_string.h                    Selt's Sweet CircleMUD Strings *
*  Usage: MagiStrings(TM)                                                 *
*   (c) 1998+ Julian Forsythe (julian@rainchild.com )                     *
************************************************************************ */

#ifndef __SELT_STRINGS_H
#define __SELT_STRINGS_H

// extra buffer space allocated for quick memory concats up to this amount
// wastes a little memory, increases performance, at least.. that's the idea ;)
#define BUFSIZE 256

class selt_string {
  friend selt_string operator+(const selt_string &str1, const selt_string &str2);
  friend selt_string operator+(const selt_string &str1, const char *str2);
  friend selt_string operator+(const char *str1, const selt_string &str2);

  int string_key;
  
  char *text;              // the pointer to the text
  int bufsize;             // for storing the size of allocated memory for this string

  void bufcat(const char *str2);
  void set(const char *str2);
  void init();

  void _catf(const char *format, ...);

public:
  void _printf(const char *format, ...);

  selt_string();
  selt_string(selt_string &str2);
  selt_string(const char *str2);
  selt_string(const char *str1, const char *str2);
  selt_string(const int value);
  ~selt_string();
  unsigned int length();
  const selt_string &operator=(const char *str2);
  const selt_string &operator=(const selt_string &str2);
  const selt_string &operator=(const selt_string *str2);
  const selt_string &operator=(const int value);
  const selt_string &operator+=(const char *str2);
  const selt_string &operator+=(const selt_string &str2);
  const selt_string &operator+=(const int value);
  const selt_string& operator+=(const char value);
  const selt_string& operator--();
  const selt_string& operator--(int);
  bool operator==(const char *string);
  bool operator!=(const char *string);
  bool operator!();
  operator const char * ();

  // public because thing_data needs access, but should be private
  void _vprintf(const char *format, va_list arglist);

  // catf + overloads
  void catf(const char *format, const char *arg1 = "", const char *arg2 = "", const char *arg3 = "", const char *arg4 = "", const char *arg5 = "", const char *arg6 = "", const char *arg7 = "", const char *arg8 = "", const char *arg9 = "", const char *arg10 = "");
  void catf(const char *format, const char *arg1, const char *arg2, const char *arg3, int arg4, const char *arg5 = "", const char *arg6 = "", const char *arg7 = "", const char *arg8 = "", const char *arg9 = "", const char *arg10 = "");
  void catf(const char *format, const char *arg1, const char *arg2, int arg3, const char *arg4 = "", const char *arg5 = "", const char *arg6 = "", const char *arg7 = "", const char *arg8 = "", const char *arg9 = "", const char *arg10 = "");
  void catf(const char *format, const char *arg1, int arg2, const char *arg3 = "", const char *arg4 = "", const char *arg5 = "", const char *arg6 = "", const char *arg7 = "", const char *arg8 = "", const char *arg9 = "", const char *arg10 = "");
  void catf(const char *format, int arg1, const char *arg2 = "", const char *arg3 = "", const char *arg4 = "", const char *arg5 = "", const char *arg6 = "", const char *arg7 = "", const char *arg8 = "", const char *arg9 = "", const char *arg10 = "");

  void catf(const char *format, const char *arg1, void *arg2);
  void catf(const char *format, int arg1, int arg2);

  // printf + overloads
  void printf(const char *format, const char *arg1 = "", const char *arg2 = "", const char *arg3 = "", const char *arg4 = "", const char *arg5 = "", const char *arg6 = "", const char *arg7 = "", const char *arg8 = "", const char *arg9 = "", const char *arg10 = "");
  void printf(const char *format, const char *arg1, const char *arg2, const char *arg3, int arg4, const char *arg5 = "", const char *arg6 = "", const char *arg7 = "", const char *arg8 = "", const char *arg9 = "", const char *arg10 = "");
  void printf(const char *format, const char *arg1, const char *arg2, int arg3, const char *arg4 = "", const char *arg5 = "", const char *arg6 = "", const char *arg7 = "", const char *arg8 = "", const char *arg9 = "", const char *arg10 = "");
  void printf(const char *format, const char *arg1, int arg2, const char *arg3 = "", const char *arg4 = "", const char *arg5 = "", const char *arg6 = "", const char *arg7 = "", const char *arg8 = "", const char *arg9 = "", const char *arg10 = "");
  void printf(const char *format, int arg1, const char *arg2 = "", const char *arg3 = "", const char *arg4 = "", const char *arg5 = "", const char *arg6 = "", const char *arg7 = "", const char *arg8 = "", const char *arg9 = "", const char *arg10 = "");

  void printf(const char *format, const char *arg1, void *arg2);
  void printf(const char *format, int arg1, int arg2);
  void printf(const char *format, const char *arg1, int arg2, const char *arg3, int arg4, const char *arg5 = "", const char *arg6 = "");
  void printf(const char *format, const char *arg1, int arg2, int arg3, const char *arg4 = "", const char *arg5 = "", const char *arg6 = "");

  bool contains(const char value, bool ignore_double);
  bool contains(const char *value);
  void chop(selt_string *to, const char separator, bool ignore_double);
  int subst(const char *token, const char *substitute);

  void capitalize();
  void make_lowercase();
  void make_uppercase();
  void truncate( int, bool = false );

  const char *get_text()
  {
    return text;
  }

  char operator[](const int value) {
    if (text)
      return text[value];
    else
      return '\0';
  }
};

selt_string operator+(const selt_string &str1, const selt_string &str2);
selt_string operator+(const selt_string &str1, const char *str2);
selt_string operator+(const char *str1, const selt_string &str2);

extern bool isnumber(const char *str);
extern bool isnumber(selt_string *str);
extern int atoi(selt_string *str);

extern bool isdelimeter(const char value, const char *delimeters);
extern void half_chop(const char *source, selt_string *arg1, selt_string *arg2, const char *delimeters = " ");
extern void half_chop(selt_string *source, selt_string *arg1, selt_string *arg2, const char* delimeters = " ");
extern void half_chop(const char* src, CBString *arg1, CBString *arg2, const char *delimeters = " ");
extern void half_chop(CBString *source, CBString *arg1, CBString *arg2, const char* delimeters = " ");

#endif
