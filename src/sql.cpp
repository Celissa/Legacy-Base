/////////////////////////////////////////////////////////////////////////////
// The Mysteria 3 Project
// Copyright 2004 Mysteria Development Team
// www.seltha.net/mysteria
/////////////////////////////////////////////////////////////////////////////
// sql.cpp ... mysteria sql api for mysql
/////////////////////////////////////////////////////////////////////////////

#include "system.h"

TMySQL MySQL;
char*  MyHost   = NULL;
char*  MyUser   = NULL;
char*  MyPasswd = NULL;
int    MyPort   = 0;
char*  MyDBName = NULL;


class TTableDef {
public:
  int           version;
  const char*      name;
  const char*       sql;
  TTableDef( int vv, const char* nn, const char* ss ) { version = vv; name = nn; sql = ss; }
};


TTableDef MyTables[] =
{
  // version table ----------------------------------------------------------------------------------------
  TTableDef(  0, "version",           "CREATE TABLE {tablename} ("
                                      "  tablename       {unique_varchar:width={max_vchar}},"
                                      "  version         {required_integer}"
                                      ")"
  ),

  // constant table ---------------------------------------------------------------------------------------
  TTableDef(  0, "constant",          "CREATE TABLE {tablename} ("
                                      "  constantid      {primary_guid},"
                                      "  variable        {required_varchar:width={max_vchar}},"
                                      "  family          {varchar:width={max_vchar}},"
                                      "  value           {varchar:width={max_vchar}}"
                                      ")"
  ),

  // version 1 added blobvalue ----------------------------------------------------------------------------
  TTableDef(  1, "constant",          "ALTER TABLE {tablename}"
                                      "  ADD blobvalue     {blob}"
  ),

  // must be last entry -----------------------------------------------------------------------------------
  TTableDef( -1, NULL, NULL )
};


void start_mysql( )
{
  FILE* fp;

  log( "Loading MySQL Settings...\r\n" );

  if( ( fp = fopen( MYSQL_FILE, "rb" ) ) == NULL ) {
    bug( "... MySQL File Not Found." );
    return;
  }

  char *word = fread_word( fp );
  if( strcmp( word, "#M2_MYSQL" ) != 0 ) {
    bug( "... not a valid mysql file" );
    return;
  }

  int version = fread_number( fp );
  log( "  * %-20s : v%d :", MYSQL_FILE, version );

  MyHost   = fread_string( fp, MEM_TABLE );
  MyUser   = fread_string( fp, MEM_TABLE );
  MyPasswd = fread_string( fp, MEM_TABLE );
  MyPort   = fread_number( fp );

  MyDBName = fread_string( fp, MEM_TABLE );

  fclose( fp );

  if( MySQL.Connect( MyHost, MyPort, MyUser, MyPasswd ) ) {
    // connected ok, try and open database
    log( "... connected to mysql on %s", MyHost );
    if( MySQL.SelectDB( MyDBName ) == false ) {
      log( "... failed to select database '%s' initializing...", MyDBName );

      // create the database and open it
      MySQL.ExecuteSQL( "CREATE DATABASE %s", MyDBName );
      if( MySQL.SelectDB( MyDBName ) == false ) {
        log( "... panic - failed to create database" );
        return;
      }
    }

    // run table upgrade / creation script on the database
    for( int i = 0; MyTables[ i ].version != -1; i++ ) {
      MySQL.ExecuteSQL( "SELECT version FROM version WHERE tablename='%s'", MyTables[ i ].name );

    }
  }
}

TMySQL :: TMySQL( )
{
  Connected = false;
}

TMySQL :: ~TMySQL( )
{
  if( Connected )
    Disconnect( );
}

bool TMySQL :: Connect( const char* host, const unsigned int port, const char* user, const char* password )
{
  if( mysql_init( &mysql ) == NULL ) {
    beetle( mysql_error( &mysql ) );
    return false;
  }

  if( mysql_real_connect( &mysql, host, user, password, NULL, port, NULL, CLIENT_MULTI_STATEMENTS ) == NULL ) {
    beetle( mysql_error( &mysql ) );
    return false;
  }

  Connected = true;

  return true;
}

bool TMySQL :: Disconnect( )
{
  if( !Connected )
    return true;

  mysql_close( &mysql );

  Connected = false;

  return true;
}

bool TMySQL :: SelectDB( const char* dbname )
{
  if( !Connected )
    return false;

  if( mysql_select_db( &mysql, dbname ) != 0 ) {
    beetle( mysql_error( &mysql ) );
    return false;
  }

  return true;
}

bool TMySQL :: ExecuteSQL( const char* format, ... )
{
  if( !Connected )
    return false;

  va_list arglist;
  selt_string statement;

  va_start( arglist, format );
  statement._vprintf( format, arglist );

  beetle( statement );

  if( mysql_query( &mysql, statement ) != 0 ) {
    beetle( mysql_error( &mysql ) );
    return false;
  }

  do {
    if( mysql_field_count( &mysql ) != 0 ) {
      beetle( "Execute SQL returned rows." );
    
      MYSQL_RES* result = mysql_store_result( &mysql );

      MYSQL_ROW row;
      int num_fields = mysql_num_fields( result );
      char buf[ 2048 ];

      while( ( row = mysql_fetch_row( result ) ) ) {
        buf[ 0 ] = '\0';
        unsigned long *lengths = mysql_fetch_lengths( result );
        for( int i = 0; i < num_fields; i++ )
          sprintf( buf + strlen( buf ), "%.*s ", (int) lengths[i], row[i] ? row[i] : "NULL");
        log( buf );
      }

      mysql_free_result( result );

    } else {
      log( "Num rows affected: %d", mysql_affected_rows( &mysql ) );
    }
  
  } while( !mysql_next_result( &mysql ) );

  return true;
}




#define		DEFALT_SQL_STMT	"SELECT * FROM db"
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

int sql_test( char_data* ch )
{

  char		szSQL[ 200 ], aszFlds[ 25 ][ 25 ], szDB[ 50 ] ;
  const  char   *pszT;
  int			i, j, k, l, x ;
  MYSQL		* myData ;
  MYSQL_RES	* res ;
  MYSQL_FIELD	* fd ;
  MYSQL_ROW	row ;

  //....just curious....
  page( ch, "sizeof( MYSQL ) == %d\n", (int) sizeof( MYSQL ) );
  strcpy( szSQL, DEFALT_SQL_STMT ) ;
  strcpy( szDB, "mysql" ) ;
  page( ch, "Some mysql struct information (size and offset):\n");
  page( ch, "net:\t%3d %3d\n",(int) sizeof(myData->net), (int) offsetof(MYSQL,net));
  page( ch, "host:\t%3d %3d\n",(int) sizeof(myData->host), (int) offsetof(MYSQL,host));
  page( ch, "port:\t%3d %3d\n", (int) sizeof(myData->port), (int) offsetof(MYSQL,port));
  page( ch, "protocol_version:\t%3d %3d\n", (int) sizeof(myData->protocol_version), (int) offsetof(MYSQL,protocol_version));
  page( ch, "thread_id:\t%3d %3d\n",(int) sizeof(myData->thread_id), (int) offsetof(MYSQL,thread_id));
  page( ch, "affected_rows:\t%3d %3d\n",(int) sizeof(myData->affected_rows), (int) offsetof(MYSQL,affected_rows));
  page( ch, "packet_length:\t%3d %3d\n",(int) sizeof(myData->packet_length), (int) offsetof(MYSQL,packet_length));
  page( ch, "status:\t%3d %3d\n",(int) sizeof(myData->status), (int) offsetof(MYSQL,status));
  page( ch, "fields:\t%3d %3d\n",(int) sizeof(myData->fields), (int) offsetof(MYSQL,fields));
  page( ch, "field_alloc:\t%3d %3d\n",(int) sizeof(myData->field_alloc), (int) offsetof(MYSQL,field_alloc));
  page( ch, "free_me:\t%3d %3d\n",(int) sizeof(myData->free_me), (int) offsetof(MYSQL,free_me));
  page( ch, "options:\t%3d %3d\n",(int) sizeof(myData->options), (int) offsetof(MYSQL,options));

  if ( (myData = mysql_init((MYSQL*) 0)) && mysql_real_connect( myData, NULL, NULL, NULL, NULL, MYSQL_PORT, NULL, 0 ) ) {
    if ( mysql_select_db( myData, szDB ) < 0 ) {
      page( ch, "Can't select the %s database !\n", szDB ) ;
      mysql_close( myData ) ;
      return 2 ;
    }
  } else {
    page( ch, "Can't connect to the mysql server on port %d !\n", MYSQL_PORT ) ;
    mysql_close( myData ) ;
    return 1 ;
  }
  //....
  if ( ! mysql_query( myData, szSQL ) ) {
    res = mysql_store_result( myData ) ;
    i = (int) mysql_num_rows( res ) ; l = 1 ;
    page( ch, "Query:  %s\nNumber of records found:  %ld\n", szSQL, i ) ;
    //....we can get the field-specific characteristics here....
    for ( x = 0 ; fd = mysql_fetch_field( res ) ; x++ )
      strcpy( aszFlds[ x ], fd->name ) ;
    //....
    while ( row = mysql_fetch_row( res ) ) {
      j = mysql_num_fields( res ) ;
      page( ch, "Record #%ld:-\n", l++ ) ;
      for ( k = 0 ; k < j ; k++ )
        page( ch, "  Fld #%d (%s): %s\n", k + 1, aszFlds[ k ], (((row[k]==NULL)||(!strlen(row[k])))?"NULL":row[k])) ;
      page( ch, "==============================\n" ) ;
    }
    mysql_free_result( res ) ;
  }
  else
    page( ch, "Couldn't execute %s on the server !\n", szSQL ) ;
  //....
  page( ch, "====  Diagnostic info  ====" ) ;
  pszT = mysql_get_client_info() ;
  page( ch, "Client info: %s\n", pszT ) ;
  //....
  pszT = mysql_get_host_info( myData ) ;
  page( ch, "Host info: %s\n", pszT ) ;
  //....
  pszT = mysql_get_server_info( myData ) ;
  page( ch, "Server info: %s\n", pszT ) ;
  //....
  res = mysql_list_processes( myData ) ; l = 1 ;
  if (res) {
    for ( x = 0 ; fd = mysql_fetch_field( res ) ; x++ )
      strcpy( aszFlds[ x ], fd->name ) ;
    while ( row = mysql_fetch_row( res ) ) {
      j = mysql_num_fields( res ) ;
      page( ch, "Process #%ld:-\n", l++ ) ;
      for ( k = 0 ; k < j ; k++ )
        page( ch, "  Fld #%d (%s): %s\n", k + 1, aszFlds[ k ], (((row[k]==NULL)||(!strlen(row[k])))?"NULL":row[k])) ;
      page( ch, "==============================\n" ) ;
    }
  } else {
    page( ch, "Got error %s when retreiving processlist\n",mysql_error(myData));
  }
  //....
  res = mysql_list_tables( myData, "%" ) ; l = 1 ;
  for ( x = 0 ; fd = mysql_fetch_field( res ) ; x++ )
    strcpy( aszFlds[ x ], fd->name ) ;
  while ( row = mysql_fetch_row( res ) ) {
    j = mysql_num_fields( res ) ;
    page( ch,  "Table #%ld:-\n", l++ ) ;
    for ( k = 0 ; k < j ; k++ )
      page( ch,  "  Fld #%d (%s): %s\n", k + 1, aszFlds[ k ], (((row[k]==NULL)||(!strlen(row[k])))?"NULL":row[k])) ;
    page( ch,  "==============================\n" ) ;
  }
  //....
  pszT = mysql_stat( myData ) ;
  puts( pszT ) ;
  //....
  mysql_close( myData ) ;
  return 0 ;

}


