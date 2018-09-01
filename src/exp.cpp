#include "system.h"


int death_exp( int level )
{
  // level 1-9 shoud be protected by their god in can_die () but just in case...
  if( level < 10 )
    return 0;

  // reduce the XP for death for under level 18's
  if( level < 18 )
    return (int) ( ( exp_for_level( level ) / 5.0 ) * ( ( level - 9 ) / 9.0 ) );

  // otherwise yer shit outta luck ;)
  return (int) ( exp_for_level( level ) / 5.0 );
}


int death_exp( char_data* victim, char_data* )
{
  return death_exp( victim->shdata->level );
}


int exp_for_level( int level )
{
  int exp;

  exp  = level*(110*level*level+400*level-411);
  exp += int( pow( level*level*level*level, 1.11 ) ); 

  return exp;
}


/*
 *   EXP CALCULATION SUB-ROUTINES
 */


int base_exp( char_data* mob )
{
  species_data*  species  = mob->species;
  double              xp;
  
  if( species->shdata->deaths == 0 || species->rounds == 0 )
    return 0;

  xp = species->damage * ( mob->max_hit + species->damage_taken / species->shdata->deaths ) / ( 2. * species->rounds);
  xp = int( pow( xp, 1.07 ) )+5.; 

  return (int) xp;
}


int base_exp( species_data* species )
{
  double       xp;
  dice_data  dice;

  dice = species->hitdice;
  
  if( species->shdata->deaths == 0 || species->rounds == 0 )
    return 0;

  xp = species->damage * ( average( dice ) + species->damage_taken / species->shdata->deaths ) / (2. * species->rounds);
  xp = pow( double( xp ), double( 1.07 ) )+5.;

  return (int) xp;
}


int death_exp( species_data* species, int base )
{
  double xp = species->shdata->kills / double(2 + species->shdata->deaths);

  if( xp > 1.5 )
    xp = 1.5 * base / 2;
  else
    xp = xp * base / 2;

  return (int) xp;
}


int special_exp( species_data* species, int base )
{
  if( species->rounds == 0 )
    return 0;

  return base*species->special/species->rounds/2;
}


int level_exp( species_data* species, int base )
{
  return base*species->shdata->level/100;
}


int modify_exp( species_data* species, int base )
{
  if( is_set( species->act_flags, ACT_AGGR_ALL ) )
    return base/10;

  return 0;
}


int modify_exp( char_data* mob, int base )
{
  if( is_set( &mob->status, STAT_AGGR_ALL ) ) 
    return base/10;

  return 0;
}


/*
 *   EXP DISPLAY FUNCTION
 */


void do_exp( char_data* ch, char* argument )
{
  dice_data         dice;
  int               base;
  int              death;
  int               spec;
  int              level;
  int              modif;
  int              total;
  species_data*  species;

  if( ( species = get_species( atoi( argument ) ) ) == NULL ) {
    send( ch, "No mob has that vnum.\r\n" );
    return;
  }

  base  = base_exp( species );
  death = death_exp( species, base );
  spec  = special_exp( species, base );
  modif = modify_exp( species, base );
  total = base+death+spec+modif;

  level  = level_exp( species, total );
  total += level;

  page_title( ch, "Exp for %s", species->Name( ) );

  if( species->shdata->deaths > 0 ) 
    page( ch, "       Ave Exp: %d\r\n", species->exp/species->shdata->deaths );

  dice   = species->hitdice;
  page( ch, "        Damage: %-10d Rounds: %-11d Dam/Rnd: %.2f\r\n ", species->damage, species->rounds, float( species->damage )/float( species->rounds ) ); 
  page( ch, "      Avg. Hp: %-7d Dmg_Taken: %-9d Avg_Taken: ", average( dice ), species->damage_taken );

  if( species->shdata->deaths == 0 )
    page( ch, "??\r\n" );
  else
    page( ch, "%d\r\n", species->damage_taken/species->shdata->deaths );

  page( ch, "        Deaths: %-10d  Kills: %-13d Level: %d\r\n", species->shdata->deaths, species->shdata->kills, species->shdata->level );
  page( ch, "       Special: %-8d Spec/Rnd: %.2f\r\n\r\n", species->special, float( species->special )/species->rounds );
  page( ch, "   Base Exp: %7d\r\n", base  );
  page( ch, "  Death Exp: %7d\r\n", death );
  page( ch, "    Special: %7d\r\n", spec  );
  page( ch, "  Modifiers: %7d\r\n", modif );
  page( ch, "      Level: %7d\r\n", level );
  page( ch, "             -------\r\n" );
  page( ch, "      Total: %7d\r\n", total );
}


/*
 *   EXP COMPUTATION FUNCTION
 */

 
int xp_compute( char_data* victim )
{
  obj_data*        wield;
  species_data*  species  = victim->species;
  int                 xp;
  int             damage;

  if( victim->species != NULL ) {
    xp  = base_exp( species );
    xp += death_exp( species, xp );
    xp += special_exp( species, xp );
    xp += level_exp( species, xp );
    xp += modify_exp( victim, xp );
  } else {
    if( ( wield = get_weapon( victim, WEAR_HELD_R ) ) != NULL ) 
      damage = wield->value[1]*(wield->value[2]+1)/2;
    else
      damage = 2;  
    damage += (int) victim->Damroll( wield );
    xp = damage*victim->max_hit/2;
    xp = min( xp, victim->exp/2 );
  }

  xp = max( 0, xp );
 
  return xp;
}


/* MODIFY REPUTATION */
void adjust_reputation( reput_data& reputation, int nation_index )
{
  int* relations = &nation_table[nation_index].nation[1];
  for ( int i = 1; i < MAX_NATION; i++, relations++ ) {
    if ( *relations != 0 ) {                 // nation is indifferent
      int roll = number_range(0, 99);        // toss a d100
      if ( *relations > 0 ) {                // nation #i is a friend
        if ( roll < *relations ) {
          reputation.nation[i]++;
        }
      } else if ( -roll > *relations ) {      // nation #i is an enemy
        reputation.nation[i]--;
      }
    }
  }
}

/*
 *   GROUP EXP FUNCTION
 */

void gain_exp( char_data* ch, char_data* victim, int gain, const char* msg, int size, bool no_gain )
{
  int           xp = 0;

  if( ch->species != NULL && !is_pet( ch ) && ch->link == NULL ) //&& !is_set( &ch->status, STAT_FAMILIAR ))
    return;

  int level = exp_for_level( ch );

  //bonux xp for being in a group, size is the number of people in the group
  int percent = size <= 1 ? 0 : size == 2 ? 2 : size == 3 ? 4 :
    size == 4 ? 6 : size == 5 ? 8 : size == 6 ? 10 : size == 7 ? 8 :
    size == 8 ? 6 : size == 9 ? 4 : 2;

  xp += percent*xp/100;

  if( gain > 0 ) 
    gain = int( gain / sqr( 1.0 + sqrt( 1.0 * gain / level ) ) );

  const char*     bonus_entry = ch->in_room->area->bonus;
  int  bonus = -1;
  int global = 0;

  global = find_table_entry( "#global", TABLE_BONUS );

  if( global == MAX_ENTRY_BONUS ) {
    bug( "#global bonus entry is missing" );
    global = 0;
  }

  if( bonus_entry != empty_string )
    bonus = find_table_entry( bonus_entry, TABLE_BONUS );

  if( bonus != -1 ) {
    if( bonus_table[global].on && bonus_table[bonus].on ) {
      if( bonus_table[global].bonus_xp > bonus_table[bonus].bonus_xp )
        bonus = global;
    }
    if( !bonus_table[bonus].on )
      bonus = global;
  }
  else
    bonus = global;

  if( bonus != -1 && bonus_table[bonus].on && gain > 0 ) {
    gain += gain*bonus_table[bonus].bonus_xp/10000;
    xp += xp*bonus_table[bonus].bonus_xp/10000;
  }

  // penalty for being shifted during combat (increased battle stats)
  // penalty is based on the #druid table entry in bonus tables
  // penalty is then shifted further based on the level of the druid
  // as the higher level forms continue to do more damage than lower
  // level ones.  level penalty is L/250 4% at level 5 and 36% at level 90.
  if( ch->shifted != NULL && gain > 0 ) {
    bonus = find_table_entry( "#druid", TABLE_BONUS );
    if( bonus != MAX_ENTRY_BONUS && bonus != -1 ) {
      gain += gain*bonus_table[bonus].bonus_xp/10000;
      gain -= gain*ch->shdata->level/250;
      xp += xp*bonus_table[bonus].bonus_xp/10000;
      xp -= xp*ch->shdata->level/250;
    }
  }
  
  add_exp( ch, gain, msg, xp, no_gain );

  if( victim == NULL )
    return;

  char    file  [ ONE_LINE ];
  FILE*     fp;

  if( victim->species != NULL ) {
    sprintf( file, "%s%i.log", MOB_EXP_LOG_DIR, victim->species->vnum );

    if( ( fp = open_file( file, "ab" ) ) == NULL )
      return;

    fprintf( fp, "%s gained %i experience.\r\n", ch->Name( ), gain );
    fclose( fp );
  }

  player_data* pc  = player( ch );
  if( ch->species == NULL ) {
    if ( pc != NULL && victim->shdata != NULL ) {
      pc->reputation.alignment[ victim->shdata->alignment ] += gain;
      if( victim->species != NULL && victim->species->nation != NATION_NONE )
        adjust_reputation( pc->reputation, victim->species->nation );
    }
  }
}


void add_exp( char_data* ch, int gain, const char* msg, int xp, bool no_gain )
{
  //removed ch->species != NULL check here
  if(  ( ch->shdata->level == LEVEL_HERO && gain >= 0 ) )
    return;

  if( msg != NULL )
    send( ch, msg, abs( gain ), abs( gain ) == 1 ? "" : "s" );

  int level = exp_for_level( ch );

  if( !no_gain )
    return;

  ch->exp += gain;

  //bug( "%s has gained\r\n", ch );
  //bug( "%d exp\r\n", gain );
  //bug( "taking to %d\r\n", ch->exp );
  
  if( !is_pet( ch ) ) //&& !is_set( &ch->status, STAT_FAMILIAR ))  
  for( ; ch->shdata->level > 1 && ch->shdata->level <= LEVEL_HERO && ch->exp < 0; ) {
    lose_level( ch );
    ch->exp += exp_for_level( ch ); 
  }

  ch->exp = max( 0, ch->exp );
   
  if( !is_pet( ch ) ) { 
    if( ch->exp >= level && ch->shdata->level < LEVEL_HERO && ch->pcdata->quest_pts >= 0 && gain > 0 ) {
      if( !is_set( ch->pcdata->pfile->flags, PLR_APPROVED ) && ch->shdata->level >= 5 ) {
        send( ch, "To gain levels past 5th you must have an approved appearance.\r\n" );
      } else {
        ch->exp -= level;
        advance_level( ch, TRUE );
      }
    }
  } else {
    if( ch->exp >= level && ch->shdata->level < LEVEL_HERO && gain > 0 && can_level( ch ) ) {
      //pet has leveled
      ch->exp -= level;
      advance_level(ch, TRUE );
    }

    ch->Set_Integer( "/Squeed/Code/Exp", ch->exp );
  }

//  bug( "end of function for %s\r\n", ch );
//  bug( " code = %d\r\n" , ch->Get_Integer( "/Squeed/Code/Exp" ) );
//  bug( " xp = %d\r\n", ch->exp );

  update_score( ch ); 
}


void disburse_exp( char_data* victim )
{
  char_data*         ch;
  char_array       list;
  float             xpc, xpcd;
  float              xp;
  int           members = 0;
  int        tot_damage = 0;
  int            damage;
  
  victim->shdata->deaths++;

  if( victim->pcdata != NULL || victim->species == NULL )
    return;

  xpc = float( xp_compute( victim ) );
  xpcd = float( victim->Experience( ) );

  if( victim->species != NULL )
    victim->species->exp += int( xpc );

  tot_damage = 0;

  // some time work out how to do a group-based xp disbursement without fux0ring up
  // pets which don't assist and stuff like that... might want to write a 'groupies_in_room'
  // assister function

  for( int i = 0; i < *victim->array; i++ ) {
    if( ( ch = character( victim->array->list[i] ) ) != NULL && ch != victim ) {
      damage = damage_done( ch, victim );
      // remove_bit( &ch->status, STAT_GAINED_EXP );

      if( damage > 0 ) { // = 0 || ch->fighting != NULL ) {
        if( !is_pet( ch ) ) {
          tot_damage  += damage;
          members     += cube( ch->shdata->level + 5 );
          list        += ch;
        } else 
          gain_exp( ch, victim, int( xpc * damage / max( damage, victim->max_hit ) ) );
      }
    }
  }

  xpc = xpc / float( max( tot_damage, victim->max_hit ) );
  xpcd = xpcd / float( max( tot_damage, victim->max_hit ) );

  // cap = 1 - pow( range( 0, list.size - 2, 11 ), 3 ) / 1331;

  char    file  [ ONE_LINE ];
  FILE*     fp;

  if( victim->species != NULL ) {
    sprintf( file, "%s%i.log", MOB_EXP_LOG_DIR, victim->species->vnum );

    if( ( fp = open_file( file, "ab" ) ) == NULL )
      return;

    fprintf( fp, "======================================================.\r\n" );
    fclose( fp );
  }

  for( int i = 0; i < list; i++ ) {
    if( ( xp = float( tot_damage ) * cube( list[i]->shdata->level + 5 ) * xpc / members ) > 0 )
      gain_exp( list[i], victim, int( xp ), "You receive %d experience point%s.\r\n", list.size );
    if( ( xp = float( tot_damage )*cube( list[i]->shdata->level+5 )*xpcd/members ) > 0 )
      gain_exp( list[i], victim, int( xp ), "New Formula: %d experience%s.\r\n", list.size, false );
    list[i]->shdata->fame = min( list[i]->shdata->fame + victim->shdata->level, 1000 );
    // set_bit( &list[i]->status, STAT_GAINED_EXP );
  }

  if( victim->species != NULL ) {
    if( ( fp = open_file( file, "ab" ) ) == NULL )
      return;

    fprintf( fp, "======================================================.\r\n" );
    fclose( fp );
  }
}


void zero_exp( species_data* species )
{
  species->damage         = 0;
  species->rounds         = 1;
  species->special        = 0;
  species->damage_taken   = 0;
  species->exp            = 0;

  species->shdata->deaths = 0;
  species->shdata->kills  = 0;
}


bool save_exp()
{
  rename_file( AREA_DIR, XP_FILE, PREV_DIR, XP_FILE );

  sdb_output *ss = new sdb_output(XP_FILE);

  species_data*  species;
  bool success = TRUE;

  // mark new record
  if (ss->begin(lookup(SDB_XPFILE, &sdb_types[0])) == FALSE)
    success = FALSE;

  // save data
  for (int i = 0; i < MAX_SPECIES; i++) {
    if ((species = species_list[i]) == NULL)
      continue;

    // mark new record
    if (ss->begin(lookup(SDB_XP, &sdb_types[0])) == FALSE)
      success = FALSE;

    // save data
    if (ss->save("Vnum", species->vnum) == FALSE)
      success = FALSE;
    if (ss->save("Damage", species->damage) == FALSE)
      success = FALSE;
    if (ss->save("Rounds", species->rounds) == FALSE)
      success = FALSE;
    if (ss->save("Special", species->special) == FALSE)
      success = FALSE;
    if (ss->save("Taken", species->damage_taken) == FALSE)
      success = FALSE;
    if (ss->save("Exp", species->exp) == FALSE)
      success = FALSE;
    if (ss->save("Deaths", species->shdata->deaths) == FALSE)
      success = FALSE;
    if (ss->save("Kills", species->shdata->kills) == FALSE)
      success = FALSE;

    // end record
    ss->end();
  }

  ss->end();

  delete ss;
  return success;
}


bool load_exp()
{
  bool success = false;

  echo( "Loading XP...\r\n" );

  sdb_input *ss = new sdb_input(XP_FILE);
  int type = ss->get_type();

  if (type == SDB_XPFILE) {
    // successfully loaded XP tables
    llist<xp_data> *xpfile = (llist<xp_data> *) ss->get_data();
    while (!xpfile->empty) {
      xp_data *xp = xpfile->headitem();
      xpfile->remove(xp);

      species_data *species = get_species(xp->vnum);
      if (!species) {
        bug("Load_Exp: XP found for non-existant species %d.", xp->vnum);
      } else {
        // update mob in memory
        species->damage         = xp->damage;
        species->rounds         = xp->rounds;
        species->special        = xp->special;
        species->damage_taken   = xp->damage_taken;
        species->exp            = xp->exp;
        species->shdata->deaths = xp->deaths;
        species->shdata->kills  = xp->kills;
      }

      delete xp;
    }
    delete xpfile;

    success = true;
  } else if (type == SDB_UNDEFINED) {
    log("File '%s' probably doesn't exist.  XP data not loaded.", XP_FILE);
  } else {
    log("File '%s' is masquerading as an XP file (appears to be of SDB type: '%s')", XP_FILE, lookup(ss->get_type(), &sdb_types[0]));
  }

  delete ss;  // Killing the sdb_input closes the file and cleans up.
  return success;
}

  
bool verify(xp_data *xp)
{
  if (!xp)
    return FALSE;

  if (xp->vnum == 0)
    return FALSE;

  return TRUE;
}


bool verify(llist<xp_data> *xpfile)
{
  if (!xpfile)
    return FALSE;

  return TRUE;
}


// xp parser
bool sdb_input::parse(xp_data *xp, int datatype, const char *tag, void *data)
{
  switch (datatype) {
  case SDB_INT:
  {
    int value = (int) data;
    if (tag && !str_cmp(tag, "Vnum")) {
      xp->vnum = value;
    } else if (tag && !str_cmp(tag, "Damage")) {
      xp->damage = value;
    } else if (tag && !str_cmp(tag, "Rounds")) {
      xp->rounds = value;
    } else if (tag && !str_cmp(tag, "Special")) {
      xp->special = value;
    } else if (tag && !str_cmp(tag, "Taken")) {
      xp->damage_taken = value;
    } else if (tag && !str_cmp(tag, "Exp")) {
      xp->exp = value;
    } else if (tag && !str_cmp(tag, "Deaths")) {
      xp->deaths = value;
    } else if (tag && !str_cmp(tag, "Kills")) {
      xp->kills = value;
    } else {
      log("SDB_INPUT: parse_xp doesn't know what to do with the (%s) tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
  } break;

  case SDB_END:
    return verify(xp);

  default:
    log("SDB_INPUT: parse_xp doesn't know what to do with the %s datatype.", lookup(datatype, &sdb_types[0]));
    return FALSE;
  }

  return TRUE;
}


// xpfile parser
bool sdb_input::parse(llist<xp_data> *xpfile, int datatype, const char *tag, void *data)
{
  switch (datatype) {
  case SDB_XP:
  {
    xp_data *xp = (xp_data *) data;
    if (xp) {
      xpfile->add(xp);
    } else {
      log("SDB_INPUT: parse_xpfile doesn't know what to do with the %s tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
  } break;

  case SDB_END:
    return verify(xpfile);

  default:
    log("SDB_INPUT: parse_xpfile doesn't know what to do with the %s datatype.", lookup(datatype, &sdb_types[0]));
    return FALSE;
  }

  return TRUE;
}

