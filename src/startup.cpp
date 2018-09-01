#include "system.h"


obj_clss_data*   obj_index_list  [ MAX_OBJ_INDEX ];
quest_data*          quest_list  [ MAX_QUEST ];
species_data*      species_list  [ MAX_SPECIES ];
char*                  cmd_help  [ MAX_COMMAND ];

mob_array        mob_list;
player_array  player_list;
obj_array        obj_list;
obj_array     corpse_list;

int            boot_stage;
int         player_number  = 0;
int             NUM_PETS = 1;

/*
 *   LOCAL ROUTINES
 */


void  load_helps     ( );
void  load_mobiles   ( );
void  load_players   ( );
void  load_trainers  ( );
void  load_quests    ( );
void  load_polls     ( ); // in vote.cpp

void  check_player   ( char_data* );
void  set_relations  ( );
void  fix_species    ( );

void load_misc();
void save_misc();

/* 
 *   EXTERNAL ROUTINES
 */    


void  read_mail      ( pfile_data* ); 

void boot_db( )
{
  start_mysql( );

  boot_stage = BOOT_WORLD;

  weather.hour     = 0;
  weather.minute   = 0;
  weather.sunlight = sunlight( 0 );
  weather.month    = 0;
  weather.day      = 1;
  weather.year     = 0;

  init_time( );
  load_misc( );

  load_rtables( );
  load_tables( );
  load_objects( );
  update_coinage( );
  load_mobiles( );
  fix_rtables( );
  load_lists( );
 
  load_areas( );
  load_helps( );
  load_notes( );
  load_shops( );
  load_trainers( );
  load_quests( );
  load_exp( );
  load_banned( );
  load_badname( );
  load_room_items( );

  fix_species( );

  boot_stage = BOOT_PLAYERS;

  load_accounts( );
  load_players( );
  load_clans( );
  load_polls( );

  boot_stage = BOOT_COMPILE;

#ifndef _DEBUG
  echo( "Compiling Scripts ...\r\n" );
  compile_areas( );
  compile_mobs( );
  compile_objs( );
#endif

  boot_stage = BOOT_UPDATE;

  echo( "Area Update ...\r\n" );
  area_update( );

  boot_stage = BOOT_COMPLETE;
}



/*
 *   PLAYERS
 */


void load_players( )
{
  player_data*      ch;
  link_data*      link;
  filelist        list;

  echo( "Loading Players ...\r\n" );
  
  char *filespec = new char[strlen(PLAYER_DIR) + 2];
  sprintf(filespec, "%s*", PLAYER_DIR);
  dirlist(&list, filespec);
  delete [] filespec;

  link            = new link_data;
  link->connected = CON_PLAYING;

  while (!list.empty) {
    char *filename = list.headitem();
    list.remove(filename);

    if( !load_char( link, filename, PLAYER_DIR ) ) {
      bug( "Load_players: error reading player file. (%s)", filename );
      delete [] filename;
      continue;
    }

    ch = link->player;

    /*
    if( ch->shdata->level < 10 && time(0) > ch->pcdata->pfile->last_on + 2 * weeks( ch->shdata->level ) ) {
      log( "  * Deleting %s.\n", filename );
      player_log( ch, "Deleted by file sweeper." );
      purge( ch );
      continue;
    }
    */

    player_number++;

    read_mail( ch->pcdata->pfile );
    check_player( ch );

    ch->Extract( );
    delete_list( extracted );

    delete [] filename;
  }

  delete link;
}


void check_player( char_data* ch )
{
  int       j;
  int  points;

  if( ch->shdata->level >= LEVEL_APPRENTICE )
    return; 

  if( ch->shdata->race >= MAX_PLYR_RACE ) {
    log( "  * %s is a non-player race.\r\n", ch->descr->name );
    return;
  }

  int* stat [] = { &ch->shdata->strength, &ch->shdata->intelligence,
    &ch->shdata->wisdom, &ch->shdata->dexterity, &ch->shdata->constitution };
  int* bonus = plyr_race_table[ch->shdata->race].stat_bonus;

  for( j = 0, points = 500; j < 5; j++ )
    points -= (*stat[j]-bonus[j])*(*stat[j]-bonus[j]+1)/2;

  if( points < 0 ) 
    log( "  * %s has impossible stats.\r\n", ch->descr->name );

  return;
}


extern unsigned char reboot_number;

void load_misc()
{
  FILE*                fp;

  log( "Loading Misc Data...\r\n" );

  if( ( fp = fopen( MISC_FILE, "rb" ) ) == NULL ) {
    bug( "... Misc File Not Found." );
    return;
  }

  int version = 0;
  char *word = fread_word( fp );

  if( !strcmp( word, "#M2_MISC" ) ) {
    version = fread_number( fp );
    log( "  * %-20s : v%d", MISC_FILE, version );
  } else {
    log( "... old version misc file" );
    fseek( fp, 0, SEEK_SET ); // rewind to start of file
  }

  record_players   = fread_number( fp );

  weather.year     = fread_number( fp );
  weather.month    = fread_number( fp );
  weather.day      = fread_number( fp );
  weather.hour     = fread_number( fp );
  weather.minute   = fread_number( fp );

  if( version > 0 )
    reboot_number  = fread_number( fp );
  else
    reboot_number  = 0;

  weather.sunlight = sunlight( 60 * weather.hour + weather.minute );
  fclose( fp );

  reboot_number++;
  save_misc( );
}


void save_misc( )
{
  FILE*                fp;

  if( ( fp = open_file( MISC_FILE, "wb" ) ) == NULL ) 
    return;

  fprintf( fp, "#M2_MISC\n" );
  fprintf( fp, "%d\n\n", 1 ); // version

  fprintf( fp, "%d\n\n", record_players );

  fprintf( fp, "%d\n", weather.year );
  fprintf( fp, "%d\n", weather.month );
  fprintf( fp, "%d\n", weather.day );
  fprintf( fp, "%d\n", weather.hour );
  fprintf( fp, "%d\n\n", weather.minute );

  fprintf( fp, "%d\n", reboot_number );

  fclose( fp );
}


