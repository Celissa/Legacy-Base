#include "system.h"

class TPollCategory;
class TPoll;


class TVote
{
  friend class TPoll;

  int Player;
  int Category;

public:
  TVote( const int CategoryUID, const int PlayerUID );
  TVote( FILE* fp );
  bool Save( FILE* fp );
};


class TPollCategory
{
  friend class TPoll;

  int UID;
  char* Name;
public:
  TPollCategory( const int ID, const char* CategoryName );
  TPollCategory( FILE* fp );
  ~TPollCategory( );
  bool Save( FILE* fp );
};


class TPoll
{
  friend class TPolls;

  char* Name;

  int CatUID;
  Array< TPollCategory* > Categories;
  Array< TVote* > Votes;
  time_t Open, Duration;

  TPollCategory* FindCategory( const char* CategoryName );
  TVote* FindVote( const int PlayerUID );
  bool CastVote( TPollCategory* Category, const int PlayerUID );
public:
  TPoll( const char* PollName );
  TPoll( FILE* fp );
  ~TPoll( );

  TPollCategory* AddCategory( const char* CategoryName );
  bool RemoveCategory( TPollCategory* Category );
  bool VoteHandler( char_data* ch, char*& argument );
  int Tally( TPollCategory* Category );

  bool IsOpen( );
  bool IsFinished( );
  int NumVotes( );
  const char* Winner( );

  bool Save( FILE* fp );
};


class TPolls
{
  Array< TPoll* > Polls;

  TPoll* FindPoll( const char* PollName );
public:
  TPolls( );
  ~TPolls( );

  void VoteHandler( char_data* ch, char*& argument );
  bool SavePolls( );
  bool LoadPolls( );
};


TVote :: TVote( const int CategoryUID, const int PlayerUID )
{
  Category = CategoryUID;
  Player = PlayerUID;
}


TPollCategory :: TPollCategory( const int ID, const char* CategoryName )
{
  UID = ID;
  Name = strdup( CategoryName );
}


TPollCategory :: ~TPollCategory( )
{
  if( Name )
    delete [] Name;
}


TPolls :: TPolls( )
{

}


TPolls :: ~TPolls( )
{
  while( Polls.size > 0 ) {
    TPoll* Poll = Polls[ 0 ];
    Polls -= Poll;
    delete Poll;
  }
}


TPoll :: TPoll( const char* PollName )
{
  Name = strdup( PollName );
  Open = 0;
  Duration = 0;
  CatUID = 0;
}


TPoll :: ~TPoll( )
{
  while( Categories.size > 0 ) {
    TPollCategory* Category = Categories[ 0 ];
    Categories -= Category;
    delete Category;
  }

  while( Votes.size > 0 ) {
    TVote* Vote = Votes[ 0 ];
    Votes -= Vote;
    delete Vote;
  }

  if( Name )
    delete [] Name;
}


int TPoll :: NumVotes( )
{
  return Votes.size;
}


int TPoll :: Tally( TPollCategory* Category )
{
  int total = 0;

  for( int i = 0; i < Votes.size; i++ )
    if( Votes[ i ]->Category == Category->UID )
      total++;

  return total;
}


const char* TPoll :: Winner( )
{
  if( Categories.size == 0 || NumVotes( ) == 0 )
    return "nothing";

  TPollCategory* Winner = NULL;
  int WinTally = -1;
  for( int i = 0; i < Categories.size; i++ ) {
    int CatTally = Tally( Categories[ i ] );
    if( CatTally > WinTally ) {
      WinTally = CatTally;
      Winner = Categories[ i ];
    }
  }

  if( Winner )
    return Winner->Name;
  
  return "nothing";
}


TPollCategory* TPoll :: AddCategory( const char* CategoryName )
{
  TPollCategory* Category = new TPollCategory( CatUID++, CategoryName );
  Categories += Category;
  return Category;
}


bool TPoll :: RemoveCategory( TPollCategory* Category )
{
  Categories -= Category;

  for( int i = Votes.size - 1; i >= 0; i-- ) {
    TVote* Vote = Votes[ i ];
    if( Vote->Category == Category->UID ) {
      Votes -= Vote;
      delete Vote;
    }
  }

  return true;
}


TPollCategory* TPoll :: FindCategory( const char* CategoryName )
{
  for( int i = 0; i < Categories.size; i++ )
    if( fmatches( CategoryName, Categories[ i ]->Name ) )
      return Categories[ i ];

  return NULL;
}


TVote* TPoll :: FindVote( const int PlayerUID )
{
  for( int i = 0; i < Votes.size; i++ )
    if( Votes[ i ]->Player == PlayerUID )
      return Votes[ i ];

  return NULL;
}


bool TPoll :: IsOpen( )
{
  time_t now = time( 0 );

  if( Open > 0 && Open < now && Open + Duration > now )
    return true;

  return false;
}


bool TPoll :: IsFinished( )
{
  time_t now = time( 0 );

  if( Open > 0 && Open + Duration <= now )
    return true;

  return false;
}


bool TPoll :: CastVote( TPollCategory* Category, const int PlayerUID )
{
  if( !IsOpen( ) )
    return false;

  if( !Category ) {
    // no such category
    return false;
  }

  TVote* Vote = FindVote( PlayerUID );
  if( Vote != NULL ) {
    // already voted, remove old vote
    Votes -= Vote;
    delete Vote;
  }

  Votes += new TVote( Category->UID, PlayerUID );

  return true;
}


TPoll* TPolls :: FindPoll( const char* PollName )
{
  for( int i = 0; i < Polls.size; i++ )
    if( fmatches( PollName, Polls[ i ]->Name ) )
      return Polls[ i ];

  return NULL;
}


TVote :: TVote( FILE* fp )
{
  int version = fread_number( fp );

  Category = fread_number( fp );
  Player = fread_number( fp );
}


bool TVote :: Save( FILE* fp )
{
  fprintf( fp, "%d %d %d\n", 1, Category, Player ); // version number, Category, Player
  return true;
}


TPollCategory :: TPollCategory( FILE* fp )
{
  int version = fread_number( fp );
  UID = fread_number( fp );
  Name = fread_string( fp, MEM_UNKNOWN );
}


bool TPollCategory :: Save( FILE* fp )
{
  fprintf( fp, "%d %d %s~\n", 1, UID, Name ); // version number, UID, Name
  return true;
}


TPoll :: TPoll( FILE* fp )
{
  int version = fread_number( fp );

  Name = fread_string( fp, MEM_UNKNOWN );
  CatUID = fread_number( fp );
  Open = fread_number( fp );
  Duration = fread_number( fp );

  int Cats = fread_number( fp );
  for( int i = 0; i < Cats; i++ )
    Categories += new TPollCategory( fp );

  int Vots = fread_number( fp );
  for( int i = 0; i < Vots; i++ )
    Votes += new TVote( fp );
}


bool TPoll :: Save( FILE* fp )
{
  fprintf( fp, "%d\n", 1 ); // version number

  fprintf( fp, "%s~\n", Name );
  fprintf( fp, "%d\n", CatUID );
  fprintf( fp, "%d %d\n", Open, Duration );

  fprintf( fp, "\n%d\n", Categories.size );
  for( int i = 0; i < Categories.size; i++ )
    Categories[ i ]->Save( fp );
    
  fprintf( fp, "\n%d\n", Votes.size );
  for( int i = 0; i < Votes.size; i++ )
    Votes[ i ]->Save( fp );

  fprintf( fp, "\n" );
  return true;
}


bool TPolls :: SavePolls( )
{
  FILE*                fp;

  if( ( fp = open_file( POLLS_FILE, "wb" ) ) == NULL ) 
    return false;

  fprintf( fp, "#M2_POLLS\n\n" );
  fprintf( fp, "%d\n%d\n\n", 1, Polls.size ); // file version number and number of polls

  for( int i = 0; i < Polls.size; i++ )
    Polls[ i ]->Save( fp );

  fclose( fp );

  return true;
}


bool TPolls :: LoadPolls( )
{
  FILE*                fp;

  log( "Loading Polls...\r\n" );

  if( ( fp = fopen( POLLS_FILE, "rb" ) ) == NULL ) {
    log( "... Polls File Not Found." );
    return false;
  }

  char *word = fread_word( fp );
  int version = 0;
    
  if( !strcmp( word, "#M2_POLLS" ) ) {
    version = fread_number( fp );
    log( "  * %-20s : v%d :", POLLS_FILE, version );
  } else {
    panic( "Load_polls: header not found" );
    return false; // not necessary, because panic calls exit( ) but it shuts up the compiler warnings
  }

  int count = fread_number( fp ); // number of polls we have to load
  for( int i = 0; i < count; i++ )
    Polls += new TPoll( fp );

  fclose( fp );

  return true;
}


bool TPoll :: VoteHandler( char_data* ch, char*& argument )
{
  // return true if data has changed and needs saving, else return false

  if( !player( ch ) )
    return false;

  if( !*argument ) {
    if( IsFinished( ) || is_immortal( ch ) ) {
      page( ch, "Poll Results for %s\r\n", Name );
      page( ch, "------------------------------------------\r\n" );
      for( int i = 0; i < Categories.size; i++ ) {
        int t = Tally( Categories[ i ] );
        page( ch, "%-30s %5d (%d%%)\r\n", Categories[ i ]->Name, t, Votes.size > 0 ? t * 100 / Votes.size : 0 );
      }
      page( ch, "------------------------------------------\r\n" );
      page( ch, "%-30s %5d\r\n", "Total", Votes.size );

    } else {
      page( ch, "      Poll: %s\r\n", Name );
      page( ch, "Categories:" );
      for( int i = 0; i < Categories; i++ )
        page( ch, "%s %s", i == 0 ? "" : ",", Categories[ i ]->Name );
      page( ch, "\r\n\r\n" );
      page( ch, "Syntax: vote <poll> <category>\r\n" );
    }

    return false;
  }

  if( is_immortal( ch ) ) {
    char arg[ MAX_INPUT_LENGTH ];
    char *targ = one_argument( argument, arg );

    if( !str_cmp( arg, "addcat" ) ) {
      if( !*targ ) {
        send( ch, "Syntax: vote <poll> addcat <category>\r\n", ch );
        return false;
      } else if( FindCategory( targ ) ) {
        send( ch, "A category already exists with that name.\r\n", ch );
        return false;
      }

      TPollCategory* Category = AddCategory( targ );
      send( ch, "Category '%s' created.\r\n", Category->Name );
      return true;

    } else if( !str_cmp( arg, "delcat" ) ) {
      if( !*targ ) {
        send( ch, "Syntax: vote <poll> delcat <category>\r\n", ch );
        return false;
      }

      TPollCategory* Category = FindCategory( targ );
      if( !Category ) {
        send( ch, "No category exists with that name.\r\n" );
        return false;
      }

      RemoveCategory( Category );
      send( ch, "Category '%s' removed.\r\n", Category->Name );
      delete Category;
      return true;

    } else if( !str_cmp( arg, "setopen" ) ) {
      if( Duration == 0 ) {
        send( ch, "Define a duration first with 'vote <poll> setduration <length>'\r\n" );
        return false;
      }

      int duration = time_arg( targ, ch, false, true );
      if( duration == -1 )
        return false;

      Open = time( 0 ) + duration;
      if( duration != 0 ) {
        char open[20];
        sprintf_datetime( open, Open );
        send( ch, "Poll '%s' set to open at %s.\r\n", Name, open );
      } else {
        send( ch, "Poll '%s' opened.\r\n", Name );
      }
      return true;

    } else if( !str_cmp( arg, "setduration" ) ) {
      if( !*targ ) {
        send( ch, "Syntax: vote <poll> setduration <length>\r\n" );
        return false;
      }

      int duration = time_arg( targ, ch, false, false );
      if( duration == -1 )
        return false;

      Duration = duration;
      char tmpduration[20];
      sprintf_minutes( tmpduration, Duration );

      send( ch, "Poll '%s' duration set to %s.\r\n", Name, tmpduration );
      return true;
    }
  }

  if( ch->shdata->level < 10 ) {
    send( ch, "You need to be level 10 or higher to vote.\r\n" );
    return false;
  }

  if( !IsOpen( ) ) {
    if( IsFinished( ) )
      send( ch, "Voting for poll '%s' has closed.  Vote not cast.\r\n", Name );
    else
      send( ch, "Voting for poll '%s' has not yet opened.  Vote not cast.\r\n", Name );

    return false;
  }

  TPollCategory* Category = FindCategory( argument );
  if( !Category ) {
    send( ch, "No category found matching '%s'\r\n", argument );
    return false;
  }

  if( !CastVote( Category, ch->pcdata->pfile->ident ) ) {
    send( ch, "Could not cast vote at this time.\r\n" );
    return false;
  }

  send( ch, "Vote cast: %s for %s.\r\n", Category->Name, Name );
  return true;
}


void TPolls :: VoteHandler( char_data* ch, char*& argument )
{
  if( !player( ch ) )
    return;

  if( !*argument ) {
    int shown = 0;

    for( int i = 0; i < Polls.size; i++ ) {
      TPoll* Poll = Polls[ i ];

      if( Poll->Open == 0 && !is_immortal( ch ) )
        continue;

      if( shown == 0 )
        page_underlined( ch, "Poll                 Status\r\n" );
      shown++;

      if( Poll->IsFinished( ) ) {
        char tmp[ 128 ];
        sprintf_datetime( tmp, Poll->Open + Poll->Duration );

        page( ch, "%-20s closed %s, %d votes, %s won.\r\n", Poll->Name, tmp, Poll->NumVotes( ), Poll->Winner( ) );

      } else if( Poll->IsOpen( ) ) {
        char tmp[ 128 ];
        sprintf_datetime( tmp, Poll->Open + Poll->Duration );

        if( is_immortal( ch ) )
          page( ch, "%-20s taking votes until %s (%d so far) %s winning.\r\n", Poll->Name, tmp, Poll->NumVotes( ), Poll->Winner( ) );
        else
          page( ch, "%-20s taking votes until %s (%d so far).\r\n", Poll->Name, tmp, Poll->NumVotes( ) );

      } else {
        char opens[ 128 ];
        char closes[ 128 ];

        strcpy( opens, "unknown" );
        strcpy( closes, "unknown" );

        if( Poll->Open > 0 ) {
          sprintf_datetime( opens, Poll->Open );
          sprintf_datetime( closes, Poll->Open + Poll->Duration );
        } else if( Poll->Duration > 0 ) {
          strcpy( closes, "after " );
          sprintf_minutes( closes + strlen( closes ), Poll->Duration );
        }

        page( ch, "%-20s opens %s, closes %s\r\n", Poll->Name, opens, closes );
      }
    }

    if( shown == 0 ) {
      send( ch, "There are no polls defined.\r\n" );
      return;
    }

    return;
  }

  char arg[ MAX_INPUT_LENGTH ];
  argument = one_argument( argument, arg );

  if( is_immortal( ch ) ) {
    if( !str_cmp( arg, "createpoll" ) ) {
      argument = one_argument( argument, arg );
      if( !*arg ) {
        send( ch, "Syntax: vote createpoll <name.of.poll>\r\n" );
        return;
      } else if( *argument != '\0' ) {
        send( ch, "A poll may only be one word long, use '.' to separate words.\r\n" );
        return;
      } else if( FindPoll( arg ) ) {
        send( ch, "A poll already exists with that name.\r\n" );
        return;
      }

      TPoll* Poll = new TPoll( arg );
      Polls += Poll;

      send( ch, "Poll '%s' created.\r\n", Poll->Name );
      SavePolls( );
      return;

    } else if( !str_cmp( arg, "deletepoll" ) ) {
      argument = one_argument( argument, arg );
      if( !*arg ) {
        send( ch, "Syntax: vote deletepoll <name.of.poll>\r\n" );
        return;
      } else if( *argument != '\0' ) {
        send( ch, "A poll may only be one word long, use '.' to separate words.\r\n" );
        return;
      }
      
      TPoll* Poll = FindPoll( arg );
      if( !Poll ) {
        send( ch, "No poll exists by that name.\r\n" );
        return;
      }

      Polls -= Poll;

      send( ch, "Poll '%s' deleted.\r\n", Poll->Name );

      delete Poll;
      SavePolls( );
      return;
    }
  }

  TPoll* Poll = FindPoll( arg );
  if( !Poll ) {
    send( ch, "No poll found matching '%s'.\r\n", arg );
    return;
  }

  if( Poll->VoteHandler( ch, argument ) )
    SavePolls( );
}


TPolls Polls;

void do_vote( char_data* ch, char* argument )
{
  try {
    Polls.VoteHandler( ch, argument );
  } catch( ... ) {
    bug( "CRASH in VoteHandler.  Ch = %s", ch->Name( NULL ) );
    send( ch, "Error voting, please contact an immortal!\r\n" );
  }
}  


void load_polls()
{
  Polls.LoadPolls( );
}

