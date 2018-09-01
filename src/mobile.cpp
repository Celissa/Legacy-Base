#include "system.h"


/*
 *   CAN_EXTRACT ROUTINE
 */
 

bool can_extract( species_data* species, char_data* ch )
{
  if( species->vnum == MOB_BLANK ) {
    send( ch, "You may not delete the template mob.\r\n" );
    return FALSE;
  }

  for( int i = 0; i < mob_list; i++ )
    if( mob_list[i]->Is_Valid() && mob_list[i]->species == species ) {
      send( ch, "There are still some creatures of that species alive.\r\n" );
      return FALSE;
    }
 
  if( generic_search( ch, species, species->vnum, CHARACTER, true ) ) {
    send( ch, "You must remove all references of that species first.\r\n" );
    return FALSE;
  }

  return TRUE;
}


/*
 *   CREATION ROUTINES
 */


mob_data* create_mobile( species_data* species, room_data* room )
{
  mob_data*       mob;
  obj_data*       obj;
  int           coins;

  if( species == NULL ) 
    panic( "Create_mobile: NULL species." );

  mob          = new mob_data;
  mob->shdata  = species->shdata;
  mob->descr   = species->descr;
  mob->species = species;

  {
    char temp[256];
    sprintf(temp, "MOB:%d", species->vnum);
    mob->active.owner_name = str_dup(temp);
  }

  mob->affected_by[0] = species->affected_by[0];
  mob->affected_by[1] = species->affected_by[1];
  mob->affected_by[2] = species->affected_by[2];
  mob->affected_by[3] = species->affected_by[3];
  mob->affected_by[4] = species->affected_by[4];
  mob->affected_by[5] = species->affected_by[5];
  mob->affected_by[6] = species->affected_by[6];
  mob->affected_by[7] = species->affected_by[7];
  mob->affected_by[8] = species->affected_by[8];
  mob->affected_by[9] = species->affected_by[9];
  mob->affected_by[10] = species->affected_by[10];

  mob->base_hit  = max( 1, roll_dice( species->hitdice ) );
  mob->original_hit = mob->base_hit;
  mob->base_move = max( 0, roll_dice( species->movedice ) );
  mob->base_mana = 100;

  mob->damage       = 0;
  mob->damage_taken = 0;
  mob->rounds       = 1;
  mob->special      = 0;

  // ---------------------------------------------------------------------------------
  // <halo>

  // I tweaked the base hit here with a formula to boost it's hps at higher levels ...
  // this one gives the mobs level in percent as a boost (eg level 50 mob gets 50%
  // more hps) ... this works out that a standard frostie will have 1809 hps instead
  // of 1131 hps... the frostie lord 2558 instead of 1505... a rabbit or deer have
  // the same as normal, since a 1-2% boost doesn't equate to any extra hps at level
  // 1-2... a durf honor guard gets 538 instead of 431 and of course the benchmark
  // jarihei gets 13k hps instead of 7.5k

  // to sneak this in over time, I've added a formula with a gradual increase over
  // 30 days as follows ... there are 86400 seconds in a day so we want maximum
  // effectiveness in 30 days (2592000) ... if we started today (1080032074) it would
  // be 1082624074... but I chose around the end of april (1083488074) meaning it won't
  // start adding hps for about a week yet, so there's time for it to be reversed
  // and/or tweaked if necessary...

  int END_DATE  =  1083488074; // end of april 2004
  int FADE_DAYS =     2592000; // 30 days

  time_t now = time( 0 );
  if( now > END_DATE - FADE_DAYS ) {
    // within fade_days prior to end date, begin slowly increasing mob hps
    if( now < END_DATE )
      mob->base_hit += (int) ( mob->base_hit * mob->shdata->level / 100 * ( 1.0 - ( END_DATE - ( now ) ) / (double) FADE_DAYS ) );
    else
      mob->base_hit += mob->base_hit * mob->shdata->level / 100;
  }

  // </halo>
  // ---------------------------------------------------------------------------------

  if( ( mob->sex = species->sex ) == SEX_RANDOM ) 
    mob->sex = ( number_range( 0, 1 ) == 0 ? SEX_MALE : SEX_FEMALE ); 

  update_max_hit( mob );
  update_max_mana( mob );

  mob->hit   = mob->max_hit;
  mob->mana  = mob->max_mana;

  update_max_move( mob );

  mob->move     = mob->max_move;
  mob->position = POS_STANDING;

  coins = number_range( 0, species->gold );

  // apply bonus tables, over-ride local with global if bonus is higher
  // only if its a reset mob this also controls the creation of the difficulty slot
  if( room != NULL ) {
    const char*     bonus_entry = room->area->bonus;
    int  bonus = -1;
    int global = 0;

    if( mob->species->low_challenge != mob->species->high_challenge )
      mob->challenge = number_range( mob->species->low_challenge, mob->species->high_challenge );
    else
      mob->challenge = mob->species->low_challenge;

    global = find_table_entry( "#global", TABLE_BONUS );

    if( global == MAX_ENTRY_BONUS ) {
      bug( "#global bonus entry is missing" );
      global = 0;
    }

    if( bonus_entry != empty_string )
      bonus = find_table_entry( bonus_entry, TABLE_BONUS );

    if( bonus != -1 ) {
      if( bonus_table[global].on && bonus_table[bonus].on ) {
        if( bonus_table[global].bonus_gp > bonus_table[bonus].bonus_gp )
          bonus = global;
      }
      if( !bonus_table[bonus].on )
        bonus = global;
    }
    else
      bonus = global;

    if( bonus_table[bonus].on )
      coins += coins*bonus_table[bonus].bonus_gp/10000;
  }

  money_nav c(&coinage);
  for( money_data* money = c.first(); money; money = c.next() ) {
    int i = number_range( 0, coins / money->value );
    if( i > 0 ) {
      obj = create( get_obj_index( money->vnum ), i );
      obj->To( mob );
      coins -= i * money->value;
    }
  }

  remove_bit( &mob->status, STAT_PET );

  assign_bit( &mob->status, STAT_SENTINEL, is_set( species->act_flags, ACT_SENTINEL ) );
  assign_bit( &mob->status, STAT_AGGR_ALL, is_set( species->act_flags, ACT_AGGR_ALL ) );
  assign_bit( &mob->status, STAT_AGGR_GOOD, is_set( species->act_flags, ACT_AGGR_GOOD ) );
  assign_bit( &mob->status, STAT_AGGR_EVIL, is_set( species->act_flags, ACT_AGGR_EVIL ) );
  assign_bit( &mob->status, STAT_AGGR_NEUT, is_set( species->act_flags, ACT_AGGR_NEUTRAL ) );

  mob->reset  = NULL;  
  mob->number = 1;

  return mob;
}


/*
 *   LOAD SPECIES
 */


void fix_species( )
{
  species_data*  species;

  echo( "Fixing Species ...\r\n" );

  for( int i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL )
      continue;

    if( species->compan_str > 26 || species->compan_str < 0 )
      species->compan_str = 26;
    if( species->compan_amt > 1000 || species->compan_amt < 0 )
      species->compan_amt = 0;

    if( species->group < 0 || species->group >= MAX_ENTRY_GROUP ) {
      roach( "Fix_Species: Non-existent group." );
      roach( "-- Species = %s", species->Name( ) );
      species->group = 0;
    } 
    if( species->nation < 0 || species->nation >= MAX_ENTRY_NATION ) {
      roach( "Fix_Species: Non-existent nation." );
      roach( "-- Species = %s", species->Name( ) );
      species->nation = 0;
    }
    if( species->shdata->race < 0 || species->shdata->race >= MAX_ENTRY_RACE ) {
      roach( "Fix_Species: Non-existant race." );
      roach( "-- Species = %s", species->Name( ) );
      species->shdata->race = 0;
    }
    if( species->size < 0 || species->size >= MAX_SIZE ) {
      roach( "Fix_Species: Invalid size." );
      roach( "-- Species = %s", species->Name( ) );
      species->size = 0;
    }
    if( species->shdata->alignment < 0 || species->shdata->alignment > MAX_ALIGNMENT ) {
      roach( "Fix_Species: Non-existent alignment." );
      roach( "-- Species = %s", species->Name( ) );
      species->shdata->alignment = 4;
    }
/*
  if( species->shdata->race < MAX_PLYR_RACE && species->shdata->race > RACE_LIZARD )
    species->shdata->race -= 7; */
  }

  return;
}


void load_mobiles( void )
{
  descr_data*      descr;
  FILE*               fp;
  mprog_data*      mprog  = NULL;
  species_data*  species;
  share_data*     shdata;
  bool boot_old = false, boot_tfh = false;
  int version = 0;

  echo( "Loading Mobs ...\r\n" );
 
  fp = open_file( AREA_DIR, MOB_FILE, "rb", TRUE ); 

  char *word = fread_word(fp);
  if( !strcmp( word, "#M2_MOBILES" ) ) {
    version = fread_number( fp );
  } else if( !strcmp( word, "#OLD_MOBILES" ) ) {
    log( "... old style mob file" );
    boot_old = true;
  } else if( !strcmp( word, "#TFH_MOBILES" ) ) {
    log( "... tfh style mob file" );
    boot_tfh = true;
  } else if( !strcmp( word, "#MOBILES" ) ) {
    // version 0 mob file
  } else {
    panic( "Load_mobiles: header not found" );
  }

  log( "  * %-20s : v%d :", MOB_FILE, version );

  for( ; ; ) {
    int vnum;
    char letter;

    if( ( letter = fread_letter( fp ) ) != '#' ) 
      panic( "Load_mobiles: # not found." );

    if( ( vnum = fread_number( fp ) ) == 0 )
       break;

    if( ( species = get_species( vnum ) ) != NULL ) {
      bug( "Load_mobiles: vnum %d duplicated.", vnum );
      bug( fread_string( fp, MEM_UNKNOWN ) );
      bug( species->Name() );
      throw( 1 );
    }

    species         = new species_data;
    species->shdata = new share_data;
    species->descr  = new descr_data;

    shdata = species->shdata;
    descr  = species->descr;

    species->vnum         = vnum;

    descr->name           = fread_string( fp, MEM_DESCR );
    descr->keywords       = fread_string( fp, MEM_DESCR );
    descr->singular       = fread_string( fp, MEM_DESCR );
    descr->prefix_s       = fread_string( fp, MEM_DESCR );
    descr->adj_s          = fread_string( fp, MEM_DESCR );
    descr->long_s         = fread_string( fp, MEM_DESCR );
    descr->plural         = fread_string( fp, MEM_DESCR );
    descr->prefix_p       = fread_string( fp, MEM_DESCR );
    descr->adj_p          = fread_string( fp, MEM_DESCR );
    descr->long_p         = fread_string( fp, MEM_DESCR );
    descr->complete       = fread_string( fp, MEM_DESCR );
    species->creator      = fread_string( fp, MEM_SPECIES ); 

    if( version < 9 ) {
      species->tame_msg    = empty_string;
      species->rtame_msg   = empty_string;
      species->no_bash_msg = empty_string;
    }
    else {
      species->tame_msg    = fread_string( fp, MEM_DESCR );
      species->rtame_msg   = fread_string( fp, MEM_DESCR );
      species->no_bash_msg = fread_string( fp, MEM_DESCR );
    }

    species->attack       = new program_data( species );
    species->attack->code = fread_string( fp, MEM_MPROG );

    read_extra( fp, species->attack->data );

    species->nation       = fread_number( fp );
    species->group        = fread_number( fp );
    shdata->race          = fread_number( fp );
    species->adult        = fread_number( fp );
    species->maturity     = fread_number( fp );
    species->skeleton     = fread_number( fp );
    species->zombie       = fread_number( fp );
    species->corpse       = fread_number( fp );

    if( boot_tfh )
      fread_number( fp ); // tfh subgroup

    species->price        = fread_number( fp );
    shdata->kills         = 0; fread_number( fp );
    shdata->deaths        = 0; fread_number( fp );
    species->wander       = fread_number( fp );
    species->date         = fread_number( fp );
    species->light        = fread_number( fp );
    species->color        = fread_number( fp );
    species->compan_str   = fread_number( fp );
    species->compan_amt   = fread_number( fp );

    if( version < 3 ) {
      species->celestial  = 0;
      species->dire       = 0;
      species->fiendish   = 0;
    } else {
      species->celestial  = fread_number( fp );
      species->dire       = fread_number( fp );
      species->fiendish   = fread_number( fp );
      if( version < 4 ) {
        species->celestial  = 0;
        species->dire       = 0;
        species->fiendish   = 0;
      }
    }

    species->act_flags[0] = fread_number( fp ) | ( 1 << ACT_IS_NPC );
    species->act_flags[1] = 0;

    if( version >= 1 )
      species->act_flags[1] = fread_number( fp );

    species->affected_by[0] = fread_number( fp );
    species->affected_by[1] = fread_number( fp );
    species->affected_by[2] = fread_number( fp );

    if( version > 1 ) {
      species->affected_by[3] = fread_number( fp );
      species->affected_by[4] = fread_number( fp );
      species->affected_by[5] = fread_number( fp );
    } else {
      species->affected_by[3] = 0;
      species->affected_by[4] = 0;
      species->affected_by[5] = 0;
    }

    if( version > 5 ) {
      species->affected_by[6] = fread_number( fp );
      species->affected_by[7] = fread_number( fp );
      species->affected_by[8] = fread_number( fp );
      species->affected_by[9] = fread_number( fp );
      species->affected_by[10] = fread_number( fp );
    } else {
      species->affected_by[6] = 0;
      species->affected_by[7] = 0;
      species->affected_by[8] = 0;
      species->affected_by[9] = 0;
      species->affected_by[10] = 0;
    }

    if( boot_tfh )
      species->act_flags[1] = fread_number( fp ); // tfh act_flags[1]

    shdata->alignment     = fread_number( fp );
    shdata->level         = fread_number( fp );
    shdata->strength      = fread_number( fp );
    shdata->intelligence  = fread_number( fp );
    shdata->wisdom        = fread_number( fp );
    shdata->dexterity     = fread_number( fp );
    shdata->constitution  = fread_number( fp );

    shdata->resist[RES_MAGIC]    = fread_number( fp ); 
    shdata->resist[RES_FIRE]     = fread_number( fp ); 
    shdata->resist[RES_COLD]     = fread_number( fp ); 
    shdata->resist[RES_SHOCK]    = fread_number( fp ); 
    shdata->resist[RES_MIND]     = fread_number( fp ); 
    shdata->resist[RES_ACID]     = fread_number( fp );
    shdata->resist[RES_POISON]   = fread_number( fp );

    if( version > 4 )
      shdata->resist[RES_HOLY]   = fread_number( fp );
    else
      shdata->resist[RES_HOLY]   = 0;

    for( int i = 0; i < MAX_ATTACK; i++ ) {
      if( version < 7 ) 
        shdata->modify_damage[i] = 0;
      else
        shdata->modify_damage[i] = fread_number( fp );
    }

    if( version < 8 ) {
      species->high_challenge = 6;  // set them to normal- difficutly
      species->low_challenge  = 6;  // set them to normal- difficulty
    }
    else {
      species->low_challenge  = fread_number( fp );
      species->high_challenge = fread_number( fp );
    }

    for( int i = 0; i < MAX_ARMOR; i++ ) {
      species->chance[i] = fread_number( fp );
      species->armor[i] = fread_number( fp );
      species->part_name[i] = fread_string( fp, MEM_SPECIES );
    }

    species->wear_part = fread_number( fp );
    species->hitdice   = fread_number( fp );
    species->movedice  = fread_number( fp );

    if( version == 0 ) {
      species->damage       = 0; fread_number( fp );
      species->rounds       = 1; fread_number( fp );
      species->special      = 0; fread_number( fp );
      species->damage_taken = 0; fread_number( fp );
      species->exp          = 0; fread_number( fp );
    }

    switch( fread_letter( fp ) ) {
      case 'M' : species->sex = SEX_MALE;       break;
      case 'F' : species->sex = SEX_FEMALE;     break;
      case 'R' : species->sex = SEX_RANDOM;     break;
      default  : species->sex = SEX_NEUTRAL;    break;
    }

    species->gold   = fread_number( fp );
    species->size   = fread_number( fp );
    species->weight = fread_number( fp );

    load( fp, species->reset, boot_old );
  
    for( ; ; ) {
      int number = fread_number( fp );

      if( number == -1 )
        break;

      mprog = new mprog_data( species );
      append( species->mprog, mprog );

      mprog->next    = NULL;
      mprog->trigger = number;
      mprog->value   = fread_number( fp );
      mprog->string  = fread_string( fp, MEM_MPROG );
      mprog->code    = fread_string( fp, MEM_MPROG );
      mprog->species = species;

      read_extra( fp, mprog->data );
    }
    
    species_list[ vnum ] = species;
  }

  fclose( fp );
}
















