/////////////////////////////////////////////////////////////////////////////
// The Mysteria 3 Project
// Copyright 2004 Mysteria Development Team
// www.seltha.net/mysteria
/////////////////////////////////////////////////////////////////////////////
// sql.h ... mysteria sql api for mysql
/////////////////////////////////////////////////////////////////////////////

#ifndef __MYSTERIA3_SQL_H
#define __MYSTERIA3_SQL_H

class TDataSet;

class TMySQL
{
  MYSQL mysql;
  bool Connected;

public:
  TMySQL( );
  ~TMySQL( );

  bool Connect( const char* host = NULL, const unsigned int port = MYSQL_PORT, const char* user = NULL, const char* password = NULL );
  bool Disconnect( );
  bool IsConnected( ) { return Connected; }

  bool ExecuteSQL( const char* format, ... );

  bool SelectDB( const char* dbname );
};

class TField
{
  char* name;
  char* value;
  unsigned int length;

public:

};

class TDataSet
{
  TMySQL* Owner;

  Array<TField*> Fields;

public:

};

extern TMySQL MySQL;
extern void start_mysql( );


#endif // __MYSTERIA3_SQL_H

