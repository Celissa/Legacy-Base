#include "system.h"

void old_save_quests( );  // old quest decl, delete later
void old_load_quests( );  // old quest decl, delete later


/*
 *    New Quest System - class decl, move back to quest.h later
 */

// ok the mentality behind doing things this way is each "TClassOfWhatever" should know
// how to run itself with no external access to variables required... in theory they
// shouldn't even have to 'friend' each other, but I've left that in for now

// you will see how they have their own handlers for editing, loading, saving, etc... so
// when the mud wants to do something with the quest, it just goes "here's what the person
// typed, you figure out what to do with it" and lets the quest classes sort it out

// for that to work, there has to be stub functions (eg the do_qnedit which calls Quests.HandleEdit)
// but it makes things more modular and easier to modify without breaking everything else
// ideally, all of TFE would be converted to this mentality over time, but new content
// should be as modular as possible... anyway, here goes

const char* quest_flag_name [ MAX_QUEST_FLAGS ] = { "sub-step", "concurrent", "random" };
const char* quest_type_name [ MAX_QUEST_TYPES ] = { "kill", "collect", "pop", "location",
  "speak", "sacrifice", "examine", "finish-quest" };

class TQuestStep
{
  // this class encompasses the step/substep whatever you want to call it...
  // it's not very implimented at the moment
  friend class TQuest;

  int                        flags;
  char*                 full_descr;
  char*                 comp_descr;
  int                         type;
  int                     quantity;
  char*                       what;

public:
  TQuestStep( );
  ~TQuestStep( );

  // load save
  bool Save( FILE* fp );
  bool Load( FILE* fp );

  // edit handlers
  void HandleEdit( wizard_data* ch, char* argument );
  void HandleStat( wizard_data* ch );
  void HandleSet( wizard_data* ch, char* argument );
};

class TQuestReward
{
  // this class encompasses a reward given, ideally we would have a second class
  // 'TQuestRewards' which would contain the array instead of TQuest holding it,
  // but I was feeling lazy
  friend class TQuest;

  int        obj_vnum;
  int        quantity;

public:
  TQuestReward( );
  ~TQuestReward( );

  // load save
  bool Save( FILE* fp );
  bool Load( FILE* fp );

  // edit handlers
  void HandleEdit( wizard_data* ch, char* argument );
  void HandleStat( wizard_data* ch );
  void HandleSet( wizard_data* ch, char* argument );
};

class TQuest
{
  // the main class for quests... trialling a SUID as vnum to see how that goes
  // we can always backtrack that later if it sucks... also, I decided that we
  // wouldn't need vnums for the reward and step after all, that would just be
  // based on the position in the array similar to the way rtables work... this
  // means we'll need a move-position function but simplifies the interface
  friend class TQuests;

  suid                          vnum;
  char*                  short_descr;
  char*                        level;
  char*                         type;
  char*                   exp_reward;
  char*                  coin_reward;
  char*                   full_descr;
  char*                   comp_descr;
  Array< TQuestReward* >     rewards;
  Array< TQuestStep* >         steps;

public:
  TQuest( );
  ~TQuest( );

  // load save
  bool Save( FILE* fp );
  bool Load( FILE* fp );

  // edit handlers
  void HandleEdit( wizard_data* ch, char* argument );
  void HandleSet( wizard_data* ch, char* argument );
  void HandleStat( wizard_data* ch );
  void HandleDesc( wizard_data* ch, char* argument );

  void HandleRewardEdit( wizard_data* ch, char* argument );
  void HandleRewardSet( wizard_data* ch, char* argument );
  void HandleRewardStat( wizard_data* ch, char* argument );
  
  void HandleStepEdit( wizard_data* ch, char* argument );
  void HandleStepSet( wizard_data* ch, char* argument );
  void HandleStepStat( wizard_data* ch, char* argument );

  void DisplayRewardList( wizard_data* ch );
  void DisplayStepList( wizard_data* ch );
};

class TQuests
{
  // this class is the 'base interface' so to speak... the mud talks through
  // here to the entire quest system and never has to interface with any of
  // the sub-classes, keeps things nice and tidy and contained, also because
  // it has only one interface that means that you don't get 100 instances of
  // copy-paste code in 40 different files which you have to change every one
  // of if you change your mind later
  Array< TQuest* > quests;

public:
  TQuests( );
  ~TQuests( );

  // load save
  bool Save( );
  bool Load( );

  // edit handlers
  void HandleEdit( wizard_data* ch, char* argument );
  void HandleSet( wizard_data* ch, char* argument );
  void HandleStat( wizard_data* ch );
  void HandleDesc( wizard_data* ch, char* argument );

  void HandleRewardEdit( wizard_data* ch, char* argument );
  void HandleRewardSet( wizard_data* ch, char* argument );
  void HandleRewardStat( wizard_data* ch, char* argument );

  void HandleStepEdit( wizard_data* ch, char* argument );
  void HandleStepSet( wizard_data* ch, char* argument );
  void HandleStepStat( wizard_data* ch, char* argument );
};


/*
 *    New Quest System -- helper functions
 */

TQuests Quests;  // encompassing class for the new quest system
                 // the whole system lives inside this class
                 // but it needs some helper functions to connect it to the MUD

void do_qnedit( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  Quests.HandleEdit( imm, argument );
}

void do_qnset( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  Quests.HandleSet( imm, argument );
}

void do_qnstat( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  Quests.HandleStat( imm );
}

void do_qndesc( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  Quests.HandleDesc( imm, argument );
}

void do_qnreward( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  Quests.HandleRewardEdit( imm, argument );
}

void do_qnrset( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  Quests.HandleRewardSet( imm, argument );
}

void do_qnrstat( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  Quests.HandleRewardStat( imm, argument );
}

void do_qnstep( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  Quests.HandleStepEdit( imm, argument );
}

void do_qnsset( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  Quests.HandleStepSet( imm, argument );
}

void do_qnsstat( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  Quests.HandleStepStat( imm, argument );
}

void load_quests( )
{
  // load new quests
  Quests.Load( );

  old_load_quests( );
}

void save_quests( )
{
  // save new quests
  Quests.Save( );

  old_save_quests( );
}


/*
 *    Initializing New Quest Stuff
 */

TQuests :: TQuests( )
{
  record_new( sizeof( TQuests ), MEM_QUEST );
}

TQuests :: ~TQuests( )
{
  record_delete( sizeof( TQuests ), MEM_QUEST );

  while( quests.size > 0 ) {
    TQuest* quest = quests[ 0 ];
    quests -= quest;
    delete quest;
  }
}

TQuest :: TQuest( )
{
  record_new( sizeof( TQuest ), MEM_QUEST );

  vnum         = 0;
  short_descr  = empty_string;
  level        = empty_string;
  type         = empty_string;
  exp_reward   = empty_string;
  coin_reward  = empty_string;
  full_descr   = empty_string;
  comp_descr   = empty_string;
}

TQuest :: ~TQuest( )
{
  record_delete( sizeof( TQuest ), MEM_QUEST );

  free_string( short_descr, MEM_QUEST );
  free_string( level, MEM_QUEST );
  free_string( type, MEM_QUEST );
  free_string( exp_reward, MEM_QUEST );
  free_string( coin_reward, MEM_QUEST );
  free_string( full_descr, MEM_QUEST );
  free_string( comp_descr, MEM_QUEST );

  while( rewards.size > 0 ) {
    TQuestReward* reward = rewards[ 0 ];
    rewards -= reward;
    delete reward;
  }

  while( steps.size > 0 ) {
    TQuestStep* step = steps[ 0 ];
    steps -= step;
    delete step;
  }
}

TQuestStep :: TQuestStep( )
{
  record_new( sizeof( TQuestStep ), MEM_QUEST );

  flags           = 0;
  type            = 0;
  full_descr      = empty_string;
  comp_descr      = empty_string;
  quantity        = 0;
  what            = empty_string;
}

TQuestStep :: ~TQuestStep( )
{
  record_delete( sizeof( TQuestStep ), MEM_QUEST );

  free_string( full_descr, MEM_QUEST );
  free_string( comp_descr, MEM_QUEST );
}

TQuestReward :: TQuestReward( )
{
  record_new( sizeof( TQuestReward ), MEM_QUEST );

  obj_vnum = 0;
  quantity = 0;
}

TQuestReward :: ~TQuestReward( )
{
  record_delete( sizeof( TQuestReward ), MEM_QUEST );
}


/*
 *    Load / Save routines ..
 */

bool TQuests :: Save( )
{
  FILE*                fp;

  if( ( fp = open_file( QUESTS_FILE, "wb" ) ) == NULL ) 
    return false;

  fprintf( fp, "#M2_QUESTS\n\n" );
  fprintf( fp, "%d\n", 1 ); // version number

  fprintf( fp, "\n%d\n", quests.size );
  for( int i = 0; i < quests.size; i++ )
    quests[ i ]->Save( fp );

  fclose( fp );

  return true;
}

bool TQuests :: Load( )
{
  FILE*                fp;

  log( "Loading Quests...\r\n" );

  if( ( fp = fopen( QUESTS_FILE, "rb" ) ) == NULL ) {
    log( "... Quests File Not Found." );
    return false;
  }

  char *word = fread_word( fp );
  int version = 0;
    
  if( !strcmp( word, "#M2_QUESTS" ) ) {
    version = fread_number( fp );
    log( "  * %-20s : v%d :", QUESTS_FILE, version );
  } else {
    panic( "TQuests.Load( ): header not found" );
    return false; // not necessary, because panic calls exit( ) but it shuts up the compiler warnings
  }

  int count = fread_number( fp ); // number of quests we have to load
  for( int i = 0; i < count; i++ ) {
    TQuest* quest = new TQuest( );
    if( quest && quest->Load( fp ) )
      quests += quest;
  }

  fclose( fp );

  return true;
}

bool TQuest :: Save( FILE* fp )
{
  fprintf( fp, "%d\n", 1 ); // version number

  fprintf( fp, "#%s~\n", tostring( vnum ) );
  fprintf( fp, "%s~\n", short_descr );
  fprintf( fp, "%s~\n", level );
  fprintf( fp, "%s~\n", type );
  fprintf( fp, "%s~\n", full_descr );
  fprintf( fp, "%s~\n", comp_descr );
  fprintf( fp, "%s~\n", exp_reward );
  fprintf( fp, "%s~\n", coin_reward );

  fprintf( fp, "\n%d\n", rewards.size );
  for( int i = 0; i < rewards.size; i++ )
    rewards[ i ]->Save( fp );
    
  fprintf( fp, "\n%d\n", steps.size );
  for( int i = 0; i < steps.size; i++ )
    steps[ i ]->Save( fp );

  fprintf( fp, "\n" );
  return true;
}

bool TQuest :: Load( FILE* fp )
{
  int version = fread_number( fp );

  char* temp = fread_string( fp, MEM_QUEST );
  vnum = atosuid( temp );
  free_string( temp, MEM_QUEST );

  short_descr = fread_string( fp, MEM_QUEST );
  level       = fread_string( fp, MEM_QUEST );
  type        = fread_string( fp, MEM_QUEST );
  full_descr  = fread_string( fp, MEM_QUEST );
  comp_descr  = fread_string( fp, MEM_QUEST );
  exp_reward  = fread_string( fp, MEM_QUEST );
  coin_reward = fread_string( fp, MEM_QUEST );

  int num_rewards = fread_number( fp );
  for( int i = 0; i < num_rewards; i++ ) {
    TQuestReward* reward = new TQuestReward( );
    if( reward && reward->Load( fp ) )
      rewards += reward;
  }

  int num_steps = fread_number( fp );
  for( int i = 0; i < num_steps; i++ ) {
    TQuestStep* step = new TQuestStep( );
    if( step && step->Load( fp ) )
      steps += step;
  }

  return true;
}

bool TQuestReward :: Save( FILE* fp )
{
  fprintf( fp, "%d\n", 1 );  // version number

  fprintf( fp, "%d\n", obj_vnum );
  fprintf( fp, "%d\n", quantity );

  return true;
}

bool TQuestReward :: Load( FILE* fp )
{
  int version = fread_number( fp );

  obj_vnum = fread_number( fp );
  quantity = fread_number( fp );

  return true;
}

bool TQuestStep :: Save( FILE* fp )
{
  fprintf( fp, "%d\n", 3 );  // version number

  fprintf( fp, "%d\n", flags );
  fprintf( fp, "%d\n", type );
  fprintf( fp, "%d\n", quantity );

  fprintf( fp, "%s~\n", what );
  fprintf( fp, "%s~\n", full_descr );
  fprintf( fp, "%s~\n", comp_descr );

  return true;
}

bool TQuestStep :: Load( FILE* fp )
{
  int version = fread_number( fp );

  flags    = fread_number( fp );
  type     = fread_number( fp );

  if( version < 2 )
    quantity = 0;
  else
    quantity = fread_number( fp );

  if( version < 3 )
    what = empty_string;
  else
    what = fread_string( fp, MEM_QUEST );

  full_descr = fread_string( fp, MEM_QUEST );
  comp_descr = fread_string( fp, MEM_QUEST );

  return true;
}


/*
 *    Editting/Creation of New Quests
 */

// quest edit
void TQuests :: HandleEdit( wizard_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    // show list of quests availabile for edition
    if( is_empty( quests ) ) {
      send( ch, "There have been no quests created.\r\n" );
      return;
    }
  
    bool found = false;
    for( int i = 0; i < quests; i++ ) {
      TQuest* quest = quests[ i ];
      if( quest != NULL ) {
        if( !found ) {
          page_underlined( ch, "Quest              Name\r\n" );
          found = true;
        }
        page( ch, "#%-16s  %s\r\n", tostring( quest->vnum ), quest->short_descr );
      }
    }

    return;

  } else if( exact_match( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      send( ch, "Which quest do you wish to delete?\r\n" );
      send( ch, "Syntax: qnedit delete [id].\r\n" );
      return;
    }

    if( is_empty( quests ) ) {
      send( ch, "There are no quests to delete.\r\n" );
      return;
    }

    suid id = atosuid( argument );
    if( id == 0 ) {
      send( ch, "Invalid ID '%s'.\r\n", argument );
      return;
    }

    for( int i = 0; i < quests; i++ ) {
      TQuest* quest = quests[ i ];
      if( quest != NULL  ) {
        if( quest->vnum == id ) {
          send( ch, "Quest #%s deleted.\r\n", tostring( quest->vnum ) );
          quests -= quest;
          delete quest;
          return;
        }
      }
    }

    send( ch, "There is no quest matching id %s.\r\n", tostring( id ) );
    return;

  } else if( exact_match( argument, "new" ) ) {
    TQuest* quest = new TQuest;

    // assign a unique id to the quest
    quest->vnum = get_suid( );

    // add the quest to the array of quests
    // then make the immortal be editting that quest
    quests += quest;
    ch->QuestEdit = quest;

    // would like to have it set the 'name' as an optional parameter, eg qned new Helping Snoopy

    send( ch, "Quest #%s created and you are editing it.\r\n", tostring( quest->vnum ) );
    return;

  } else if( *argument == '#' || isxdigit( *argument ) ) {
    // not sure if linux has isxdigit... basically checks for 0-9, a-f (hexidecimal) can write a function if necessary
    suid id = atosuid( argument );
    if( id == 0 ) {
      send( ch, "Invalid ID '%s'.\r\n", argument );
      return;
    }

    for( int i = 0; i < quests; i++ ) {
      if( quests[ i ]->vnum == id ) {
        TQuest* quest = quests[ i ];
        send( ch, "You start editting quest #%s.\r\n", tostring( quest->vnum ) );
        ch->QuestEdit = quest;
        return;
      }
    }
    send( ch, "That quest does not exist.\r\n" );
    return;
  
  } else {
    send( ch, "Syntax: qnedit [new|#|delete <#>]\r\n" );
    return;
  }
}

// quest set
void TQuests :: HandleSet( wizard_data* ch, char* argument )
{
  TQuest* quest = ch->QuestEdit;
  if( quest == NULL ) {
    send( ch, "You are not editing a quest.\r\n" );
    return;
  }

  quest->HandleSet( ch, argument );
}

void TQuest :: HandleSet( wizard_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    HandleStat( ch );
    return;
  }

  char arg[ MAX_INPUT_LENGTH ];
  char* check = argument;

  check = one_argument( check, arg );
  if( fmatches( arg, "name" ) ) {
    if( strlen( check ) > 30 ) {
      send( ch, "Quest names must be less than 30 characters in length.r\n" );
      return;
    }
  }

  class string_field string_list[] = {
    { "name",       MEM_QUEST,      &short_descr,    NULL },
    { "coin",       MEM_QUEST,      &coin_reward,    NULL },
    { "experience", MEM_QUEST,      &exp_reward,     NULL },
    { "type",       MEM_QUEST,      &type,           NULL },
    { "level",      MEM_QUEST,      &level,          NULL },
    { "",                   0,      NULL,            NULL },
  };

  if( process( string_list, ch, "quest", argument ) )
    return;

  send( ch, "Unknown parameters.\r\n" );
  return;
}

// quest stat
void TQuests :: HandleStat( wizard_data* ch )
{
  TQuest* quest = ch->QuestEdit;
  if( quest == NULL ) {
    send( ch, "You are not editing a quest.\r\n" );
    return;
  }

  quest->HandleStat( ch );
}

void TQuest ::HandleStat( wizard_data* ch )
{
  page_title( ch, "Quest #%s", tostring( vnum ) );

  page( ch, "\r\n" );

  page( ch, "        Name: %s\r\n", short_descr );
  page( ch, "        Type: %-15s       Level: %-15s\r\n", type, level );

  page( ch, "\r\n" );
  
  page_title( ch, "Rewards" );

  page( ch, "        Coin: %-15s  Experience: %-15s\r\n", coin_reward, exp_reward );
  DisplayRewardList( ch );

  page( ch, "\r\n" );

  page_title( ch, "Full Description" );
  page( ch, "%s\r\n", full_descr );
  page_title( ch, "Complete Description" );
  page( ch, "%s\r\n", comp_descr );

  return;
}

// quest description editor
void TQuests :: HandleDesc( wizard_data* ch, char* argument )
{
  TQuest* quest = ch->QuestEdit;
  if( quest == NULL ) {
    send( ch, "You are not editing a quest.\r\n" );
    return;
  }

  quest->HandleDesc( ch, argument );
}

void TQuest :: HandleDesc( wizard_data* ch, char* argument )
{
  int flags = 0;

  if( !get_flags( ch, argument, &flags, "fc", "Qndesc" ) )
    return;

  if( flags == 0 ) {
    send( ch, "Which description do you wish to edit? [ flags: fc ]\r\n" );
    return;
  }

  if( is_set( &flags, 0 ) )
    full_descr = edit_string( ch, argument, full_descr, MEM_QUEST );
  else if( is_set( &flags, 1 ) )
    comp_descr = edit_string( ch, argument, comp_descr, MEM_QUEST );
  else
    send( ch, "Illegal flag for descriptions.\r\n" );

  return;
}


//////////////////////////////////////////////////////////////////////////////////////////////
// Quest Reward Editors
//////////////////////////////////////////////////////////////////////////////////////////////

void TQuest :: DisplayRewardList( wizard_data* ch )
{
  page( ch, "   Object(s):" );

  if( is_empty( rewards ) ) {
    page( ch, " -- none --\r\n" );
    return;
  }

  for( int i = 0; i < rewards; i++ ) {
    if( rewards[ i ] != NULL ) {
      obj_clss_data* obj_clss = get_obj_index( rewards[ i ]->obj_vnum );
      page( ch, "%s #%d - %s\r\n", i == 0 ? "" : "             ", i + 1, obj_clss == NULL ? "<undefined>" : obj_clss->Name( rewards[ i ]->quantity ) );
    }
  }
}

// reward edit handlers
void TQuests :: HandleRewardEdit( wizard_data* ch, char* argument )
{
  TQuest* quest = ch->QuestEdit;
  if( quest == NULL ) {
    send( ch, "You are not editing a quest.\r\n" );
    return;
  }

  quest->HandleRewardEdit( ch, argument );  
}

void TQuest :: HandleRewardEdit( wizard_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    // display reward table

    if( is_empty( rewards ) ) {
      page( ch, "There are no object rewards for this quest.\r\n" );
      return;
    }

    DisplayRewardList( ch );
    return;

  } else if( exact_match( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      send( ch, "Which reward do you wish to delete?\r\n" );
      send( ch, "Syntax: qnreward delete [number].\r\n" );
      return;
    }

    if( is_empty( rewards ) ) {
      send( ch, "There are no rewards to delete.\r\n" );
      return;
    }

    int i = atoi( argument );
    if( i < 1 || i > rewards ) {
      send( ch, "Selected reward '%s' out of range [1-%d].\r\n", argument, rewards.size );
      return;
    }

    TQuestReward* reward = rewards[ i-1 ];
    if( reward != NULL ) {
      rewards -= reward;
      send( ch, "Reward #%i deleted.\r\n", i );
      delete reward;
    } else {
      send( ch, "[BUG] Reward #%i not found.\r\n", i );
    }

    return;
  
  } else if( exact_match( argument, "new" ) ) {
    TQuestReward* reward = new TQuestReward;

    // add the reward to the array of rewards for that quest
    // then make the immortal be editting that reward
    ch->QuestRewardEdit = reward;
    rewards            += reward;

    send( ch, "Reward created and you are editing it.\r\n" );
    return;

  } else if( isdigit( *argument ) ) {

    int i = atoi( argument );

    if( i < 1 || i > rewards ) {
      send( ch, "Selected reward '%s' out of range [1-%d].\r\n", argument, rewards.size );
      return;
    }

    TQuestReward* reward = rewards[ i-1 ];
    if( reward != NULL ) {
      ch->QuestRewardEdit = reward;
      send( ch, "You are now editing reward #%i.\r\n", i );
    } else {
      send( ch, "[BUG] Reward #%i not found.\r\n", i );
    }

    return;

  } else {
    send( ch, "Syntax: qnreward [new|#|delete <#>]\r\n" );

  }
}

// reward set handlers
void TQuests :: HandleRewardSet( wizard_data* ch, char* argument )
{
  TQuest* quest = ch->QuestEdit;
  if( quest == NULL ) {
    send( ch, "You are not editing a quest.\r\n" );
    return;
  }

  quest->HandleRewardSet( ch, argument );  
}

void TQuest :: HandleRewardSet( wizard_data* ch, char* argument )
{
  TQuestReward* reward = ch->QuestRewardEdit;
  if( reward == NULL ) {
    send( ch, "You are not editing a quest reward.\r\n" );
    return;
  }

  reward->HandleSet( ch, argument );  
}

void TQuestReward :: HandleSet( wizard_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    HandleStat( ch );
    return;
  }

  class int_field int_list [] = {
    { "object",           0,     MAX_OBJ_INDEX,  &obj_vnum      },
    { "quantity",         1,             20000,  &quantity      },
    { "",                 0,                 0,  NULL           }, 
  };
               
  if( process( int_list, ch, "reward", argument ) )
    return;

  send( ch, "Unknown parameters.\r\n" );
  return;
}

// reward stat handlers
void TQuests :: HandleRewardStat( wizard_data* ch, char* argument )
{
  TQuest* quest = ch->QuestEdit;
  if( quest == NULL ) {
    send( ch, "You are not editing a quest.\r\n" );
    return;
  }

  quest->HandleRewardStat( ch, argument );  
}

void TQuest :: HandleRewardStat( wizard_data* ch, char* argument )
{
  TQuestReward* reward = ch->QuestRewardEdit;
  if( reward == NULL ) {
    send( ch, "You are not editing a quest reward.\r\n" );
    return;
  }

  reward->HandleStat( ch );  
}
    
void TQuestReward :: HandleStat( wizard_data* ch )
{  
  obj_clss_data* obj_clss = get_obj_index( obj_vnum );
  page( ch, "Reward Object: %s (%d)\r\n", obj_clss == NULL ? "<undefined>" : obj_clss->Name( ), obj_vnum );
  page( ch, "     Quantity: %d\r\n", quantity );
  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Quest Step Editors
//////////////////////////////////////////////////////////////////////////////////////////////

void TQuest :: DisplayStepList( wizard_data* ch )
{
  if( is_empty( steps ) )
    return;

  for( int i = 0; i < steps; i++ ) {
    if( steps[ i ] != NULL ) {
      if( i > 0 )
        page( ch, "\r\n" );

      char     what_name [ TWO_LINES ];

      switch( steps[i]->type ) {
        case QTYPE_KILL:
        case QTYPE_SPEAK: {
          int j = abs( atoi( steps[i]->what ) );
          species_data*      mob = get_species( j );;
          sprintf( what_name, "%s", mob == NULL ? "<undefined>" : mob->Name( ) );
          break;
        }

        case QTYPE_POP:
        case QTYPE_COLLECT:
        case QTYPE_SACRIFICE:
        case QTYPE_EXAMINE: {
          int j = abs( atoi( steps[i]->what ) );
          obj_clss_data*     obj = get_obj_index( j );
          sprintf( what_name, "%s", obj == NULL ? "<undefined>" : obj->Name( ) );
          break;
        }

        case QTYPE_LOCATION: {
          int j = abs( atoi( steps[i]->what ) );
          room_data*        room = get_room_index( j );
          sprintf( what_name, "%s [%d]", room == NULL ? "<undefined>" : room->name, room == NULL ? 0 : room->vnum );
          break;
        }

        case QTYPE_QUEST: {
          sprintf( what_name, "%s", steps[i]->what );
          break;
        }

        default:
          sprintf( what_name, "Unused in this type." );
          break;
      }


      page_title( ch, "Step #%d", i + 1 );
      page( ch, "\r\n" );
      page( ch, "Type: %s / What: %s / Quantity: %d\r\n", quest_type_name[ steps[ i ]->type ], what_name, steps[i]->quantity, steps[ i ]->flags );  // need some lookup for type name and sprint_flags?
      display_flags( "Quest", &quest_flag_name[0], &quest_flag_name[1], &steps[ i ]->flags, MAX_QUEST_FLAGS, ch );
      page( ch, "\r\n\r\n" );
      page( ch, "Description:\r\n %s\r\n", steps[i]->full_descr );  // be nice if the entire description could be indented in case of multi-lines?
      page( ch, "Completed:\r\n %s\r\n", steps[i]->comp_descr );  // be nice if the entire description could be indented in case of multi-lines?
    }
  }
}

// reward edit handlers
void TQuests :: HandleStepEdit( wizard_data* ch, char* argument )
{
  TQuest* quest = ch->QuestEdit;
  if( quest == NULL ) {
    send( ch, "You are not editing a quest.\r\n" );
    return;
  }

  quest->HandleStepEdit( ch, argument );  
}

void TQuest :: HandleStepEdit( wizard_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    // display step table

    if( is_empty( steps ) ) {
      page( ch, "There are no steps for this quest.\r\n" );
      return;
    }

    DisplayStepList( ch );
    return;

  } else if( exact_match( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      send( ch, "Which step do you wish to delete?\r\n" );
      send( ch, "Syntax: qnstep delete [number].\r\n" );
      return;
    }

    if( is_empty( steps ) ) {
      send( ch, "There are no steps to delete.\r\n" );
      return;
    }

    int i = atoi( argument );
    if( i < 1 || i > steps ) {
      send( ch, "Selected steps '%s' out of range [1-%d].\r\n", argument, steps.size );
      return;
    }

    TQuestStep* step = steps[ i-1 ];
    if( step != NULL ) {
      steps -= step;
      send( ch, "Step #%i deleted.\r\n", i );
      delete step;
    } else {
      send( ch, "[BUG] Step #%i not found.\r\n", i );
    }

    return;
  
  } else if( exact_match( argument, "new" ) ) {
    TQuestStep* step = new TQuestStep;

    // add the reward to the array of rewards for that quest
    // then make the immortal be editting that reward
    ch->QuestStepEdit = step;
    steps            += step;

    send( ch, "Step created and you are editing it.\r\n" );
    return;

  } else if( isdigit( *argument ) ) {
    int i = atoi( argument );

    if( i < 1 || i > steps ) {
      send( ch, "Selected step '%s' out of range [1-%d].\r\n", argument, steps.size );
      return;
    }

    TQuestStep* step = steps[ i-1 ];
    if( step != NULL ) {
      ch->QuestStepEdit = step;
      send( ch, "You are now editing step #%i.\r\n", i );
    } else {
      send( ch, "[BUG] Step #%i not found.\r\n", i );
    }

    return;

  } else {
    send( ch, "Syntax: qnstep [new|#|delete <#>]\r\n" );

  }
}

// reward set handlers
void TQuests :: HandleStepSet( wizard_data* ch, char* argument )
{
  TQuest* quest = ch->QuestEdit;
  if( quest == NULL ) {
    send( ch, "You are not editing a quest.\r\n" );
    return;
  }

  quest->HandleStepSet( ch, argument );  
}

void TQuest :: HandleStepSet( wizard_data* ch, char* argument )
{
  TQuestStep* step = ch->QuestStepEdit;
  if( step == NULL ) {
    send( ch, "You are not editing a quest step.\r\n" );
    return;
  }

  step->HandleSet( ch, argument );
}

void TQuestStep :: HandleSet( wizard_data* ch, char* argument )
{
  int option;

  if( *argument == '\0' ) {
    HandleStat( ch );
    return;
  }

  if( !get_flags( ch, argument, &option, "dcf", "Quests" ) )
    return;

  class type_field type_list [] = {
    { "type", MAX_QUEST_TYPES, &quest_type_name[0],  &quest_type_name[1],  &type },
    { "",     0,               NULL,                 NULL,                 NULL  },
  };

  if( process( type_list, ch, "Quest Types", argument ) )
    return;

  class int_field int_list [] = {
    { "quantity",        1,           10000,           &quantity },
    { "",                0,               0,           NULL      },
  };

  if( process( int_list, ch, "Quest Quantity", argument ) )
    return;

  class string_field string_list [] = {
    { "mob",          MEM_QUEST,        &what,        NULL },
    { "object",       MEM_QUEST,        &what,        NULL },
    { "room",         MEM_QUEST,        &what,        NULL },
    { "quest",        MEM_QUEST,        &what,        NULL },
    { "",             0,                NULL,         NULL },
  };

  if( process( string_list, ch, "Quest", argument ) )
    return;

  if( is_set( &option, 0 ) )
    full_descr = edit_string( ch, argument, full_descr, MEM_QUEST );
  else if( is_set( &option, 1 ) )
    comp_descr = edit_string( ch, argument, comp_descr, MEM_QUEST );
  else if( is_set( &option, 2 ) ) 
    quest_flags.set( ch, argument, &flags );
  else
    send( ch, "Unknown flag: Syntax Qnsstep <flags: dcf | object | room | mob | quantity | type> <argument>\r\n" );
  
  return;
}

// reward stat handlers
void TQuests :: HandleStepStat( wizard_data* ch, char* argument )
{
  TQuest* quest = ch->QuestEdit;
  if( quest == NULL ) {
    send( ch, "You are not editing a quest.\r\n" );
    return;
  }

  quest->HandleStepStat( ch, argument );  
}

void TQuest :: HandleStepStat( wizard_data* ch, char* argument )
{
  TQuestStep* step = ch->QuestStepEdit;
  if( step == NULL ) {
    send( ch, "You are not editing a quest step.\r\n" );
    return;
  }

  step->HandleStat( ch );  
}
    
void TQuestStep :: HandleStat( wizard_data* ch )
{
  char     what_name [ TWO_LINES ];
  ;
  switch( type ) {
    case QTYPE_KILL:
    case QTYPE_SPEAK: {
      int j = abs( atoi( what ) );
      species_data*      mob = get_species( j );;
      sprintf( what_name, "%s", mob == NULL ? "<undefined>" : mob->Name( ) );
      break;
    }

    case QTYPE_POP:
    case QTYPE_COLLECT:
    case QTYPE_SACRIFICE:
    case QTYPE_EXAMINE: {
      int j = abs( atoi( what ) );
      obj_clss_data*     obj = get_obj_index( j );
      sprintf( what_name, "%s", obj == NULL ? "<undefined>" : obj->Name( ) );
      break;
    }

    case QTYPE_LOCATION: {
      int j = abs( atoi( what ) );
      room_data*        room = get_room_index( j );
      sprintf( what_name, "%s [%d]", room == NULL ? "<undefined>" : room->name, room == NULL ? 0 : room->vnum );
      break;
    }

    case QTYPE_QUEST: {
      sprintf( what_name, "%s", what );
      break;
    }

    default:
      sprintf( what_name, "Unused in this type." );
      break;
  }


  page_title( ch, "Step Definitions" );
  page( ch, "\r\n" );
  page( ch, "Type: %-10s    What: %-15s Quantity: %-5i\r\n", quest_type_name[ type ], what_name, quantity );
  display_flags( "Quest", &quest_flag_name[0], &quest_flag_name[1], &flags, MAX_QUEST_FLAGS, ch );
  page( ch, "\r\n\r\n" );
  page( ch, "Description:\r\n%s\r\n", full_descr );
  page( ch, "Completed:\r\n%s\r\n", comp_descr );

  return;
}


// END of new quest stuff, just a search reference

















// crap below here should be DELETED when old quest system isn't needed anymore


quest_data* get_quest_index( int i )
{
  if( i < 0 || i >= MAX_QUEST ) 
    return NULL;

  return quest_list[i];
}

void do_qwhere( char_data* ch, char *argument )
{
  action_data*     action;
  mprog_data*       mprog;
  obj_clss_data*      obj;
  oprog_data*       oprog;
  room_data*         room;
  species_data*   species;
  int                i, j;
  int               index;
  bool              found  = FALSE;

  index = atoi( argument );

  if( get_quest_index( index ) == NULL ) {
    send( ch, "No quest found with that index.\r\n" );
    return;
  }

  area_nav alist(&area_list);
  for( area_data *area = alist.first( ); area; area = alist.next( ) )
    for( room = area->room_first; room != NULL; room = room->next )
      for( j = 1, action = room->action; action != NULL; j++, action = action->next )
        if( search_quest( action->binary, index ) ) {
          found = TRUE;
          page( ch, "  Used in action #%d in room #%d\r\n", j, room->vnum );
        }

  for( i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL ) 
      continue;

    if( search_quest( species->attack->binary, index ) ) {
      page( ch, "  Used in attack prog on mob #%d.\r\n", i );
      found = TRUE;
    }

    for( j = 1, mprog = species->mprog; mprog != NULL; j++, mprog = mprog->next )
      if( search_quest( mprog->binary, index ) ) {
        found = TRUE;
        page( ch, "  Used in mprog #%d on mob #%d\r\n", j, i );
      }
  }

  for( i = 0; i < MAX_OBJ_INDEX; i++ ) {
    if( ( obj = obj_index_list[i] ) == NULL ) 
      continue; 
    for( j = 1, oprog = obj->oprog; oprog != NULL; j++, oprog = oprog->next )
      if( search_quest( oprog->binary, index ) ) {
        found = TRUE;
        page( ch, "  Used in oprog #%d on obj #%d\r\n", j, i );
      }
  }

  if( !found )
    send( ch, "No references to that quest were found.\r\n" );

  return;
}

void do_cfwhere( char_data* ch, char *argument )
{
  action_data*     action;
  mprog_data*       mprog;
  obj_clss_data*      obj;
  oprog_data*       oprog;
  room_data*         room;
  species_data*   species;
  int                i, j;
  int               index;
  bool              found  = FALSE;

  index = atoi( argument );

  if( index > 32*MAX_CFLAG ) {
    send( ch, "Cflag out of range.\r\n" );
    return;
  }

  area_nav alist(&area_list);
  for( area_data *area = alist.first( ); area; area = alist.next( ) )
    for( room = area->room_first; room != NULL; room = room->next )
      for( j = 1, action = room->action; action != NULL; j++, action = action->next )
        if( search_cflag( action->binary, index ) ) {
          found = TRUE;
          page( ch, "  Used in action #%d in room #%d\r\n", j, room->vnum );
        }

  for( i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL ) 
      continue;

    if( search_cflag( species->attack->binary, index ) ) {
      page( ch, "  Used in attack prog on mob #%d.\r\n", i );
      found = TRUE;
    }

    for( j = 1, mprog = species->mprog; mprog != NULL; j++, mprog = mprog->next )
      if( search_cflag( mprog->binary, index ) ) {
        found = TRUE;
        page( ch, "  Used in mprog #%d on mob #%d\r\n", j, i );
      }
  } 

  for( i = 0; i < MAX_OBJ_INDEX; i++ ) {
    if( ( obj = obj_index_list[i] ) == NULL ) 
      continue; 
    for( j = 1, oprog = obj->oprog; oprog != NULL; j++, oprog = oprog->next )
      if( search_cflag( oprog->binary, index ) ) {
        found = TRUE;
        page( ch, "  Used in oprog #%d on obj #%d\r\n", j, i );
      }
  }

  if( !found )
    send( ch, "No references to that cflag was found.\r\n" );

  return;
}


/*
 *   EDITTING OF QUESTS
 */


void do_qedit( char_data* ch, char *argument )
{
  quest_data*    quest;
  wizard_data*     imm = wizard( ch );
  bool           found = FALSE;  
  int                i;

  if( *argument == '\0' ) {
    for( i = 0; i < MAX_QUEST; i++ ) {
      if( ( quest = quest_list[i] ) == NULL )
        continue;
      found = TRUE;
      page( ch, "[%3d] %-50s\n", i, quest->message );
      }
    if( !found ) 
      send( ch, "No quests found.\r\n" );
    return;
    }
 
  if( matches( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      send( ch, "What quest do you want to delete?\r\n" );
      return;
    }
   
    if( ( i = atoi( argument ) ) < 0 || i >= MAX_QUEST || ( quest = quest_list[i] ) == NULL ) {
      send( ch, "Quest not found to remove.\r\n" );
      return;
    }

    extract( imm, offset( &imm->quest_edit, imm ), "quest" );
    quest_list[i] = NULL;
    free_string( quest->message, MEM_QUEST );
    send( ch, "Quest removed.\r\n" );
    delete quest;

    return; 
  }

  if( matches( argument, "new" ) ) {
    for( i = 0; quest_list[i] != NULL; i++ )
      if( i == MAX_QUEST ) {
        send( ch, "Quest space is full.\r\n" );
        return;
      }

    quest              = new quest_data;
    quest->message     = empty_string;
    quest->vnum        = i;  
    quest->points      = 0;
    quest_list[i]      = quest;
    imm->quest_edit    = quest;

    send( ch, "Quest created and assigned #%d.\r\n", i );
    return;
  }

  if( ( i = atoi( argument ) ) < 0 || i >= MAX_QUEST ) {
    send( ch, "Quest number is out of range.\r\n" );
    return;
  }

  if( quest_list[i] == NULL ) {
    send( ch, "There is no quest by that number.\r\n" );
    return;
  }

  imm->quest_edit = quest_list[i];
  send( ch, "Qedit now operates on specified quest.\r\n" );

  return;
}  

void do_qset( char_data* ch, char *argument )
{
  quest_data*  quest;
  wizard_data* imm = wizard( ch );
 
  if( *argument == '\0' ) {
    do_qstat( ch, "" );
    return;
  }

  if( ( quest = imm->quest_edit ) == NULL ) {
    send( ch, "You aren't editing any quest.\r\n" );
    return;
    }

  class int_field int_list[] = {
    { "points",        0,  25,  &quest->points },
    { "",              0,   0,  NULL },   
    };

  if( process( int_list, ch, "quest", argument ) )
    return;
  
  class string_field string_list[] = {
    { "message",    MEM_QUEST,  &quest->message,  NULL },
    { "",           0,          NULL,             NULL }   
    };

  if( process( string_list, ch, "quest", argument ) )
    return;

  send( ch, "Unknown parameter.\r\n" );
  return;
}


void do_qstat( char_data* ch, char* )
{
  wizard_data*  imm = wizard( ch );
  quest_data*   quest;

  if( ( quest = imm->quest_edit ) == NULL ) {
    send( ch, "You aren't editing any quest.\r\n" );
    return;
    }

  send( ch, "Message: %s\r\n", quest->message );
  send( ch, "Points: %d\r\n", quest->points );

  return;
}


/*
 *   PLAYER COMMANDS
 */


void do_quests( char_data* ch, char* argument )
{
  quest_data*       quest;
  bool              found  = FALSE; 
  int                   i;
  int               flags;
  int               value;
  const char*   title_msg;
  const char*    none_msg; 

  if( not_player( ch ) ) 
    return;

  if( !get_flags( ch, argument, &flags, "d", "Quests" ) )
    return;

  if( is_set( &flags, 0 ) ) {
    title_msg = "Completed Quests";
    none_msg  = "not completed any";
    value     = -1;
    }
  else {
    title_msg = "Assigned Quests";
    none_msg  = "no unfinished";
    value     = 1;
    } 

  for( i = 0; i < MAX_QUEST; i++ ) 
    if( ( quest = quest_list[i] ) != NULL 
      && ch->pcdata->quest_flags[i] == value ) {
      if( !found ) {
        page_title( ch, title_msg );
        found = TRUE;
        }
      page_centered( ch, quest->message );   
      }

  if( !found ) 
    send( ch, "You have %s quests.\r\n", none_msg );

  return;
}


/*
 *   EDITTING QUESTS ON PLAYERS
 */


void do_qremove( char_data* ch, char* )
{
  int   i;

  for( i = 0; i < MAX_QUEST; i++ ) 
    ch->pcdata->quest_flags[i] = 0;

  ch->pcdata->quest_pts = 0;

  send( ch, "All quest records erased for your character.\r\n" );
  return;   
}

// Syntax: qflag <num> [U|A|D]
// Effect: Sets qflag #num to 0 (unassigned (default)) or 1 (assigned), or
//         -1 (done).
//
// Alternate Syntax: qflag
// Effect: Dump qflags 8 per row.  Display as A for assigned, D for done.
void do_qflag( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if ( imm == NULL ) 
     return;
  player_data* victim = ( imm->player_edit != NULL ? imm->player_edit : 
                          player( ch ) );
  if ( victim == NULL || victim->pcdata == NULL ) 
     return;

  if ( *argument == '\0' ) {
    page_title( ch, "Quest flags of %s", victim->descr->name );
    page_centered( ch, "A = Assigned, D = Done." );
    int j;
    for ( j = 0; j < MAX_QUEST; j++ ) {
      char quest_char = ' ';
      if ( victim->pcdata->quest_flags[j] == 1 )
   quest_char = 'A';
      else if ( victim->pcdata->quest_flags[j] == -1 )
         quest_char = 'D';
      page( ch, "%5d (%1c)", j, quest_char );
      if( j%8 == 7 )
        page( ch, "\r\n" );
    }
    if( j%8 != 0 )
      page( ch, "\r\n" );
    return;
  }

  // Extract the quest flag number (0-128).
  int i;
  if ( !number_arg( argument, i )) {
    send( ch, "Syntax: qflag <num> [A|D]\r\n" );
    return;
  }
  if ( i < 0 || i >= MAX_QUEST ) {
    send( ch, "Qflag number out of range.\r\n" );
    return;
  }

  // Last (optional) argument indicates how to set the quest flag.
  if ( *argument == '\0' || *argument == 'U' ) {
    victim->pcdata->quest_flags[i] = 0;
    send( ch, "Qflag %d on %s set to Unassigned.\r\n", i,
          victim == ch ? "yourself" : victim->descr->name );
  }
  else if ( *argument == 'A' ) {
    victim->pcdata->quest_flags[i] = 1;
    send( ch, "Qflag %d on %s set to Assigned.\r\n", i,
          victim == ch ? "yourself" : victim->descr->name );
  }
  else if ( *argument == 'D' ) {
    victim->pcdata->quest_flags[i] = -1;
    send( ch, "Qflag %d on %s set to Done.\r\n", i,
          victim == ch ? "yourself" : victim->descr->name );
  }
  else {
    send( ch, "Syntax: qflag <num> [U|A|D]\r\n" );
  }
  return;
}

void do_cflag( char_data* ch, char *argument )
{
  player_data* victim;
  wizard_data* imm  = wizard( ch );
  int i;

  victim = ( imm->player_edit == NULL ? (player_data*) ch : imm->player_edit );

  if( *argument == '\0' ) {
    page_title( ch, "Cflags of %s", victim->descr->name );
    for( i = 0; i < 32*MAX_CFLAG; i++ ) {
      page( ch, "%5d (%1c)", i,
        is_set( victim->pcdata->cflags, i ) ? '*' : ' ' );
      if( i%8 == 7 )
        page( ch, "\r\n" );
    }
    if( i%8 != 0 )
      page( ch, "\r\n" );

    page( ch, "\r\n" );
    page_title( ch, "Variables of %s", victim->descr->name );

    if( victim->variables.size > 0 ) {
      variable_nav list( &victim->variables );
      for( variable_data* var = list.first( ); var; var = list.next( ) )
        page( ch, "%39s : %-5d\r\n", var->get_name( ), var->get_value( ) );

    } else
      page( ch, "<none defined>\r\n" );
      
    return;
  }

  i = atoi( argument );

  if( i < 0 || i >= 32*MAX_CFLAG ) {
    send( ch, "Cflag number out of range.\r\n" );
    return;
  }

  if( victim->pcdata != NULL ) {
    switch_bit( victim->pcdata->cflags, i );
    send( ch, "Cflag %d on %s set to %s.\r\n", i, victim == ch ? "yourself" : victim->descr->name, is_set( victim->pcdata->cflags, i ) ? "true" : "false" );
  }
} 


void old_load_quests( )
{
  // load old quests ... delete later
  quest_data*  quest;
  FILE*           fp;
  char        letter;
  int           vnum;

  log( "Loading Quests...\r\n" );

  if( ( fp = fopen( QUEST_FILE, "rb" ) ) == NULL ) 
    panic( "Load_quests: file not found" );

  if( strcmp( fread_word( fp ), "#QUESTS" ) ) 
    panic( "Load_quests: header not found" );
 
  for( ; ; ) {
    if( ( letter = fread_letter( fp ) ) != '#' ) 
      panic( "Load_quests: # not found." );
   
    if( ( vnum = fread_number( fp ) ) == -1 )
      break;

    quest          = new quest_data;
    quest->vnum    = vnum;      
    quest->message = fread_string( fp, MEM_QUEST );
    quest->points  = fread_number( fp );

    fread_number( fp );

    quest_list[vnum] = quest;
  }

  fclose( fp );
}


void old_save_quests( )
{
  // save old quests ... delete later
  quest_data*  quest;
  FILE*           fp;
  int              i;

  rename_file( AREA_DIR, QUEST_FILE, PREV_DIR, QUEST_FILE );

  if( ( fp = fopen( QUEST_FILE, "wb" ) ) == NULL ) {
    bug( "Save_quest: fopen" );
    return;
  }

  fprintf( fp, "#QUESTS\n\n" );
 
  for( i = 0; i < MAX_QUEST; i++ ) 
    if( ( quest = quest_list[i] ) != NULL ) {
      fprintf( fp, "#%d\n", quest->vnum );
      fprintf( fp, "%s~\n", quest->message );
      fprintf( fp, "%d 0\n", quest->points  );
    }

  fprintf( fp, "#-1\n" );
  fclose( fp );
}


