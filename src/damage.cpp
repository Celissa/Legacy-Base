#include "system.h"

const char*   condition_prep  ( char_data* );

void          msg_attack      ( char_data*, char_data*, const char*,
                                const char*, char );
void          msg_other       ( char_data*, char_data*, const char*,
                                const char*, char );

/*
 *   GENERAL MESSAGE ROUTINES
 */

void mod_element_damage( char_data* ch, int& damage, int type )
{
  float modifier = 0;

  if( type < 0 || type > MAX_ATTACK || ch == NULL )
    return;

  switch( type ) {
    case ATT_PHYSICAL:
      modifier = (float) ch->Modify_Physical( )/10000;
      break;

    case ATT_FIRE:
      modifier = (float) ch->Modify_Fire( )/10000;
      break;
    
    case ATT_COLD:
      modifier = (float) ch->Modify_Cold( )/10000;
      break;

    case ATT_ACID:
      modifier = (float) ch->Modify_Acid( )/10000;
      break;

    case ATT_SHOCK:
      modifier = (float) ch->Modify_Shock( )/10000;
      break;

    case ATT_MIND:
      modifier = (float) ch->Modify_Mind( )/10000;
      break;

    case ATT_MAGIC:
      modifier = (float) ch->Modify_Magic( )/10000;
      break;

    case ATT_POISON:
      modifier = (float) ch->Modify_Poison( )/10000;
      break;

    case ATT_HOLY:
      modifier = (float) ch->Modify_Unholy( )/10000;
      break;

    case ATT_UNHOLY:
      modifier = (float) ch->Modify_Holy( )/10000;
      break;

    default:
      return;
  }

  modifier = (float) damage*modifier;
  damage   += (int) modifier;
  return;
}



void dam_local( char_data* victim, char_data* ch, int damage, const char* dt, const char* loc_name, bool plural )
{
  char               tmp  [ TWO_LINES ];
  char             punct;
  const char*   dam_name;
  char_data*         rch;
  int               flag;

  if( *loc_name == '\0' ) {
    dam_message( victim, ch, damage, dt, lookup( physical_index, damage, plural ) );
    return;
  }

  punct        = ( damage <= 24 ? '.' : '!' );
  dam_name     = lookup( physical_index, damage, plural );
  victim->hit -= damage;

  sprintf( tmp, "%s's %s %s%s%s your %s%c\r\n", ch->Name( victim,1, TRUE ), dt, to_self( victim ), dam_name, normal( victim ), loc_name, punct );
  corrupt( tmp, TWO_LINES, "Dam_Local" );
  *tmp = toupper( *tmp );
  send( victim, tmp );
 
  if( victim->Seen( ch ) )
    sprintf( tmp, "Your %s %s%s%s %s's %s%c", dt, by_self( ch ), dam_name, normal( ch ), victim->Name( ch, 1, TRUE ), loc_name, punct );
  else
    sprintf( tmp, "Your %s %s%s%s someone%c", dt, by_self( ch ), dam_name, normal( ch ), punct );

  corrupt( tmp, TWO_LINES, "Dam_Local" );
  strcat_cond( tmp, ch, victim );

  flag = ( victim->pcdata == NULL && !is_set( &victim->status, STAT_PET ) ? MSG_DAMAGE_MOBS : MSG_DAMAGE_PLAYERS );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL || rch == ch || rch == victim || rch->link == NULL || !is_set( &rch->pcdata->message, flag ) || !rch->Can_See( ) )
      continue;
    if( victim->Seen( rch ) ) 
      sprintf( tmp, "%s's %s %s%s%s %s's %s%c", ch->Name( rch, 1, TRUE ), dt, damage_color( rch, ch, victim ), dam_name, normal( rch ), victim->Seen_Name( rch ), loc_name, punct );
    else
      sprintf( tmp, "%s's %s %s%s%s someone%c", ch->Name( rch ), dt, damage_color( rch, ch, victim ), dam_name, normal( rch ), punct );
    corrupt( tmp, TWO_LINES, "Dam_Local" );
    *tmp = toupper( *tmp );
    strcat_cond( tmp, rch, victim );
  }

  victim->hit += damage;

  return;
}


void dam_message( char_data* victim, char_data* ch, int damage, const char* dt, const char* word )
{
  if( !victim )
    return;

  if( dt == '\0' ) {
    code_bug( "Dam_Message: NULL string." );
    return;
  }

  char      tmp  [ ONE_LINE ];
  int         i;
  char    punct;

  victim->hit -= damage;

  strcpy( tmp, dt );
  if( ( i = strlen( tmp ) ) > 2 && tmp[i-2] == '\r' )
    tmp[i-2] = '\0';

  punct = ( damage <= 24 ? '.' : '!' );

  if( ch == NULL || *dt == '*' ) 
    msg_other( victim, ch, *tmp == '*' ? &tmp[1] : tmp, word, punct );
  else 
    msg_attack( victim, ch, tmp, word, punct );

  victim->hit += damage;

  return;
}  
 

void msg_attack( char_data* victim, char_data* ch, const char* dt, const char* word, char punct )
{
  char           tmp  [ MAX_INPUT_LENGTH ];
  char_data*     rch;
  int           flag;

  sprintf( tmp, "Your %s %s%s%s %s%c", dt, by_self( ch ), word, normal( ch ), victim->Name( ch ), punct );
  strcat_cond( tmp, ch, victim );

  sprintf( tmp, "%s's %s %s%s%s you%c\r\n", ch->Name( victim ), dt, to_self( victim ), word, normal( victim ), punct );
  *tmp = toupper( *tmp );
  send( tmp, victim );

  flag = ( victim->pcdata == NULL && !is_set( &victim->status, STAT_PET ) ? MSG_DAMAGE_MOBS : MSG_DAMAGE_PLAYERS );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL || rch == ch || rch == victim || rch->link == NULL || !is_set( &rch->pcdata->message, flag ) || !rch->Can_See( ) )
      continue;
    sprintf( tmp, "%s's %s %s%s%s %s%c", ch->Name( rch ), dt, damage_color( rch, ch, victim ), word, normal( rch ), victim->Name( rch ), punct );
    *tmp = toupper( *tmp );
    strcat_cond( tmp, rch, victim );
  }

  return;
}


void msg_other( char_data* victim, char_data* ch, const char* dt, const char* word, char punct )
{
  char                tmp  [ TWO_LINES ];
  char_data*          rch;

  sprintf( tmp, "%s %s%s%s you%c\r\n", dt, to_self( victim ), word, normal( victim ), punct );
  corrupt( tmp, TWO_LINES, "Msg_Other" );
  *tmp = toupper( *tmp );
  send( tmp, victim );

  for( int i = 0; i < *victim->array; i++ ) {
    if( ( rch = character( victim->array->list[i] ) ) == NULL || rch == victim || !rch->Can_See( ) )
      continue;
    sprintf( tmp, "%s %s%s%s %s%c", dt, damage_color( rch, ch, victim ), word, normal( rch ), victim->Name( rch ), punct );
    corrupt( tmp, TWO_LINES, "Msg_Other" );
    *tmp = toupper( *tmp );
    strcat_cond( tmp, rch, victim );
  }

  return;
}


/*
 *   CONDITION ROUTINES
 */


void strcat_cond( char* tmp, char_data* ch, char_data* victim )
{
  const char*  cond_word  = condition_word( victim ); 
  bool           visible  = TRUE;
  int             length  = 0;
 
  for( int i = 0; tmp[i] != '\0'; i++ ) {
    if( visible ) {
      if( tmp[i] == '\x1B' )
        visible = FALSE;
      else
        length++;
    } else if( tmp[i] == 'm' )
      visible = TRUE;
  }

  send( ch, "%s%s[ %s ]\r\n", tmp, length + strlen( cond_word ) > 74 ? "\r\n" : " ", is_set( ch->affected_by, AFF_BLIND ) ? "unknown" : cond_word );

  return;
}
  

const char* condition_prep( char_data *ch )
{
  int percent = (100*ch->hit)/ch->max_hit;

  if( ch->hit <= -10 )
    return "is";

       if( percent >= 100 ) return "is in";
  else if( percent >=  90 ) return "is";
  else if( percent >=  80 ) return "has a";
  else if( percent >=  70 ) return "has";
  else if( percent >=  60 ) return "has";
  else if( percent >=  50 ) return "is";
  else if( percent >=  40 ) return "has";
  else if( percent >=  30 ) return "is";
  else if( percent >=  20 ) return "is";
  else if( percent >=  10 ) return "is";
  else if( percent >=   0 ) return "is";
                            return "is";
}


const char* condition_word( char_data* ch )
{
  int percent = (100*ch->hit)/ch->max_hit;

  if( ch->hit <= -11 )
    return "DEAD!";

       if( percent >= 100 ) return "perfect health";
  else if( percent >=  90 ) return "slightly scratched";
  else if( percent >=  80 ) return "few bruises";
  else if( percent >=  70 ) return "some cuts";
  else if( percent >=  60 ) return "several wounds";
  else if( percent >=  50 ) return "badly wounded";
  else if( percent >=  40 ) return "many nasty wounds";
  else if( percent >=  30 ) return "bleeding freely";
  else if( percent >=  20 ) return "covered in blood";
  else if( percent >=  10 ) return "leaking guts";
  else if( percent >=   0 ) return "mostly dead";
                            return "dying";

}


const char* condition_short( char_data* ch, char_data* victim )
{
  if( victim == NULL )
    return "-";

  if( victim->array != ch->array || !victim->Seen( ch ) )
    return "??";

  return condition_short( victim );
}


const char* condition_short( char_data* ch )
{
  int percent;

  if( ch->hit <= -11 )
    return "DEAD!";

  percent = (100*ch->hit)/ch->max_hit;

       if( percent >= 100 ) return "perfect";
  else if( percent >=  90 ) return "scratched";
  else if( percent >=  80 ) return "bruised";
  else if( percent >=  70 ) return "cut";
  else if( percent >=  60 ) return "wounded";
  else if( percent >=  50 ) return "badly wounded";
  else if( percent >=  40 ) return "nastily wounded";
  else if( percent >=  30 ) return "bleeding freely";
  else if( percent >=  20 ) return "covered in blood";
  else if( percent >=  10 ) return "leaking guts";
  else if( percent >=   0 ) return "mostly dead";
                            return "dying";
}


/*
 *   GENERAL INFLICT ROUTINE
 */


bool inflict( char_data* victim, char_data* ch, int dam, const char* dt )
{
/*
  if( ch && victim && victim->in_room != ch->in_room ) {
    bug( "Inflict: char (%s) in different room to victim (%s)", ch->Name( ), victim->Name( ) );
    return TRUE;
  }
*/
  if( victim == NULL && ch == NULL ) {
    code_bug( "Both ch, and victim are NULL in a damage case." );
    return TRUE;
  }

  if( ch != NULL && victim != NULL && victim->in_room != ch->in_room ) {
    code_bug( "Ch and Victim are in different rooms, queue related?" );
    return TRUE;
  }

  if( !victim || !victim->Is_Valid() ) {
    bug( "Inflict: char (%s) hitting invalid victim", ch && ch->Is_Valid( ) ? ch->Name( ) : "acode" );
    return TRUE;
  }

  if( victim->position == POS_DEAD ) {
    bug( "Inflict: char (%s) hitting already dead victim (%s)", ch && ch->Is_Valid( ) ? ch->Name( ) : "acode", victim->Name( ) );
    return TRUE;
  }

  if( dam > 1000 ) {
    if( ch != NULL )
      bug( "Attack: %d points of damage! (%s)", dam, ch->Name( )  );
    else
      bug( "Attack: %d points of damage! (acode)", dam );
    dam = 1000;
  }

  record_damage( victim, ch, dam );

  victim->hit -= dam;

  update_pos( victim );
  update_max_move( victim );

  set_bit( &victim->status, STAT_WIMPY );

  if( victim->position == POS_SLEEPING || victim->position == POS_MEDITATING ) {
    strip_affect( victim, AFF_SLEEP );
    send( victim, "You are suddenly %s by the feeling of pain.\r\n",
    victim->position == POS_MEDITATING ? "disturbed" : "awakened" );
    send_seen( victim, "%s %s.\r\n", victim, 
    victim->position == POS_MEDITATING ? "meditation is disturbed" : "wakes up" );
    victim->position = POS_RESTING;
    renter_combat( victim );
  }

  /*
  if( IS_AFFECTED( victim, AFF_CONFUSED ) && dam > number_range( 0, 100 ) ) {
    strip_affect( victim, AFF_CONFUSED );
    send( "You are knocked to your senses.\r\n", victim );
    act( "$n is knocked to $s senses.", victim, NULL, NULL, TO_ROOM );
    victim->fighting = NULL;
    victim->aggressive = NULL;
    for( rch = victim->in_room->people; rch != NULL; rch = rch->next_in_room )
      if( IS_NPC( rch ) && !IS_SET( rch->status, STAT_PET ) ) {
        if( rch->fighting == ch )
          rch->fighting = NULL;
        if( rch->aggressive == ch )
          rch->aggressive = NULL;
        }
    }
    */

  switch( victim->position ) {
  case POS_MORTAL:
    send( victim, "You are mortally wounded, and will die soon, if not aided.\r\n" );
    send_seen( victim, "%s is mortally wounded, and will die soon, if not aided.\r\n", victim );
    break;
 
  case POS_INCAP:
    send( victim, "You are incapacitated and will slowly die, if not aided.\r\n" );
    send_seen( victim, "%s is incapacitated and will slowly die, if not aided.\r\n", victim );
    break;
  
  case POS_STUNNED:
    send( victim, "You are stunned, but will probably recover.\r\n" );
    send_seen( victim, "%s is stunned, but will probably recover.\r\n", victim );
    break;

  case POS_DEAD:
    death_message( victim );
    break;
  
  default:
    if( dam > victim->max_hit/4 )
      send( "That really did HURT!\r\n", victim );
    
    if( victim->pcdata != NULL && victim->hit < victim->max_hit/4 && is_set( &victim->pcdata->message, MSG_BLEEDING ) )
      send( victim, "You sure are BLEEDING!\r\n" );
    break;
  }

  if( victim->rider != NULL && victim->position < POS_FIGHTING ) {
    send( victim->rider, "You are thrown to the ground as your mount is %s.\r\n", victim->position == POS_DEAD ? "killed" : "incapacitated" );
    victim->rider->position = POS_RESTING;
    victim->rider->mount    = NULL;
    victim->rider           = NULL;
  }

  if( ch != NULL && ch->species != NULL && victim != NULL && victim->species == NULL ) {
    ch->damage          += dam;
    ch->species->damage += dam;
    if( ( victim->pcdata != NULL && is_set( victim->pcdata->pfile->flags, PLR_PARRY ) ) || 
      victim->in_room != ch->in_room ) 
      ch->damage        -= dam;
  }

  if( victim->position == POS_DEAD ) {
    death( victim, ch, dt );
    return TRUE;
  }

  return check_wimpy( victim, ch );

  // return FALSE;
} 


/*
 *   GLANCE ROUTINES
 */


void do_glance( char_data* ch, char* argument )
{
  char_data*  victim;

  if( *argument == '\0' ) {
    send( ch, "Who do you want to glance at?\r\n" );
    return;
  }

  if( ( victim = one_character( ch, argument, "glance", ch->array ) ) == NULL )
    return;

  glance( ch, victim );

  return;
}


void glance( char_data* ch, char_data* victim )
{
  if( victim == ch ) {
    page( ch, "Your condition is %s.\r\n", condition_word( ch ) );
    return;
  }

  page( ch, "%s %s %s%s%s%c\r\n",  victim->Seen_Name( ch ), condition_prep( victim ), color_scale( ch, 6-6*victim->hit/victim->max_hit ), condition_word( victim ),  normal( ch ), victim->hit > 0 ? '.' : '!' );

  return;
}


/*
 *   SPAM ROUTINES
 */


void spam_char( char_data* ch, const char* text )
{
  if( ch->link == NULL || !is_set( &ch->pcdata->message, MSG_MISSES ) )
    return;

  send( ch, text );

  return;  
}


void spam_room( const char* text, char_data* ch, char_data* victim )
{
  char         tmp  [ TWO_LINES ];
  char_data*   rch;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL
      || rch->link == NULL || rch == ch || rch == victim
      || rch->position <= POS_SLEEPING
      || !is_set( &rch->pcdata->message, MSG_MISSES ) )
      continue;
    sprintf( tmp, text, ch->Name( rch ), victim == NULL ? "[BUG]" : victim->Name( rch ) );
    corrupt( tmp, TWO_LINES, "Spam_Room" );
    *tmp = toupper( *tmp );
    send( tmp, rch );
  }

  return;
}

 
/*
 *   PHYSICAL DAMAGE ROUTINES
 */


index_data physical_index [] = 
{
  { "misses",                 "miss",                  0 },
  { "scratches",              "scratch",               1 },
  { "grazes",                 "graze",                 2 },
  { "hits",                   "hit",                   4 },
  { "injures",                "injure",                6 },
  { "wounds",                 "wound",                10 },
  { "mauls",                  "maul",                 15 },
  { "decimates",              "decimate",             21 },
  { "devastates",             "devastate",            28 },
  { "maims",                  "maim",                 35 },
  { "SAVAGES",                "SAVAGE",               45 },
  { "CRIPPLES",               "CRIPPLE",              55 },
  { "MUTILATES",              "MUTILATE",             70 },
  { "DISEMBOWELS",            "DISEMBOWEL",           85 },
  { "* DISMEMBERS *",         "* DISMEMBER *",       100 },
  { "* EVISCERATES *",        "* EVISCERATE *",      120 },
  { "* MASSACRES *",          "* MASSACRE *",        145 },
  { "* PULVERIZES *",         "* PULVERIZE *",       170 },
  { "** DEMOLISHES **",       "** DEMOLISH **",      200 },
  { "** EXTIRPATES **",       "** EXTIRPATE **",     240 },
  { "*** OBLITERATES ***",    "*** OBLITERATE ***",  290 },
  { "*** ERADICATES ***",     "*** ERADICATE ***",   350 },
  { "*** ANNIHILATES ***",    "*** ANNIHILATE ***",   -1 }
};


index_data fire_index [] = 
{
  { "singes",                 "singe",                 3 },
  { "scorches",               "scorch",                7 },
  { "toasts",                 "toast",                15 },
  { "cooks",                  "cook",                 30 },
  { "fries",                  "fry",                  45 },
  { "TORCHES",                "TORCH",                60 },
  { "SEARS",                  "SEAR",                 90 },
  { "* CHARS *",              "* CHAR *",            120 },
  { "* IMMOLATES *",          "* IMMOLATE *",        150 },
  { "* CAUTERIZES *",         "* CAUTERIZE *",       190 },
  { "** VAPORIZES **",        "** VAPORIZE **",      240 },
  { "** INCINERATES **",      "** INCINERATE **",    290 },  
  { "*** CREMATES ***",       "*** CREMATE ***",     350 },
  { "*** DISINTEGRATES ***",  "*** DISINTEGRATE ***", -1 }
};


index_data electric_index [] = 
{
  { "startles",               "startle",               3 },
  { "tingles",                "tingle",                7 },
  { "sparks",                 "spark",                15 },
  { "zaps",                   "zap",                  30 },
  { "charges",                "charge",               45 },
  { "JOLTS",                  "JOLT",                 60 },
  { "SHOCKS",                 "SHOCK",                90 },
  { "* STRIKES *",            "* STRIKE *",          120 },
  { "* DIFFUSES *",           "* DIFFUSE *",         150 },
  { "* ELECTROCUTES *",       "* ELECTROCUTE *",     190 },
  { "** ELECTRIFIES **",      "** ELECTRIFY **",     240 },
  { "** IONIZES **",          "** IONIZE **",        290 },
  { "*** ATOMIZES ***",       "*** ATOMIZE ***",     350 },
  { "*** IRRADIATES ***",     "*** IRRADIATE ***",    -1 }
};


index_data acid_index [] = 
{
  { "blemishes",              "blemish",               3 },
  { "irritates",              "irritate",              7 },
  { "burns",                  "burn",                 15 },
  { "erodes",                 "erode",                30 },
  { "blisters",               "blister",              45 },
  { "MARS",                   "MAR",                  60 },
  { "WITHERS",                "WITHER",               90 },
  { "* CORRODES *",           "* CORRODE *",         120 },
  { "* SCARS *",              "* SCAR *",            150 }, 
  { "* DISFIGURES *",         "* DISFIGURE *",       190 }, 
  { "** MELTS **",            "** MELT **",          240 },
  { "** LIQUIFIES **",        "** LIQUIFY **",       290 },
  { "*** DISSOLVES ***",      "*** DISSOLVE ***",    350 },
  { "*** DELIQUESCES ***",    "*** DELIQUESCE ***",   -1 }
};


index_data cold_index [] = 
{
  { "pinches",               "pinch",                  3 },
  { "nips",                  "nip",                    7 },
  { "bites",                 "bite",                  15 },
  { "cools",                 "cool",                  30 },
  { "numbs",                 "numbs",                 45 },
  { "CHILLS",                "CHILL",                 60 },
  { "FROSTS",                "FROST",                 90 },
  { "* FREEZES *",           "* FREEZE *",           120 },
  { "* STIFFENS *",          "* STIFFEN *",          150 },
  { "* HARDENS *",           "* HARDEN *",           190 },
  { "** ICES **",            "** ICE **",            240 },
  { "** SOLIDIFIES **",      "** SOLIDIFY **",       290 },
  { "*** PETRIFIES ***",     "*** PETRIFY ***",      350 },
  { "*** GLACIATES ***",     "*** GLACIATE ***",      -1 }
};


index_data poison_index [] = 
{
  { "whitens",               "whiten",                 3 },
  { "taints",                "taint",                  7 },
  { "impairs",               "impair",                15 },
  { "stings",                "sting",                 30 },
  { "nauseates",             "nauseate",              45 },
  { "SICKENS",               "SICKEN",                60 },
  { "INFECTS",               "INFECT",                90 },
  { "* DECAYS *",            "* DECAY *",            120 },
  { "* CONSUMES *",          "* CONSUME *",          150 },
  { "* CORRUPTS *",          "* CORRUPT *",          190 },
  { "** DESSICATES **",      "** DESSICATE **",      240 },
  { "** CONTAMINATES **",    "** CONTAMINATE **",    290 },
  { "*** INCAPACITATES ***", "*** INCAPACITATE ***", 350 },
  { "*** PUTRIFIES ***",     "*** PUTRIFY ***",       -1 }
};

index_data unholy_index [] =
{
  { "decays",                "decay",                 7  },
  { "rots",                  "rot",                   15 },
  { "festers",               "fester",                30 },
  { "taints",                "taint",                 45 },
  { "corrupts",              "corrupt",               60 },
  { "CURSES",                "CURSE",                 90 },
  { "MUTATES",               "MUTATE",               120 },
  { "* DEFILES *",           "* DEFILE *",           150 },
  { "* SCOURGES *",          "* SCOURGE *",          190 },
  { "** BANES **",           "** BANE **",           240 },
  { "** DISEASES **",        "** DISEASE **",        290 },
  { "*** JINXES ***",        "*** JINX ***",         350 },
  { "*** DOOMS ***",         "*** DOOM ***",          -1 }
};

index_data holy_index [] =
{
  { "anoints",               "anoint",                 7 },
  { "blesses",               "bless",                 15 },
  { "consecrates",           "consecrate",            30 },
  { "hallows",               "hallow",                45 },
  { "sanctifies",            "sanctify",              60 },
  { "CLEANSES",              "CLEANSE",               90 },
  { "PURIFIES",              "PURIFY",               120 },
  { "* LUSTRATES *",         "* LUSTRATE *",         150 },
  { "* REDEEMS *",           "* REDEEM *",           190 },
  { "** EXTRICATES **",      "** EXTRICATE **",      240 },
  { "** ATONES **",          "** ATONE **",          290 },
  { "*** ABSOLVES ***",      "*** ABSOLVE ***",      350 },
  { "*** EXPIATE ***",       "***EXPIATE ***",        -1 }
};


bool damage_element( char_data* victim, char_data* ch, int damage, const char* string, int type, const char* death, bool save, bool message, bool plural )
{
  if( !victim || !victim->Is_Valid() ) {
    bug( "Damage_Element: char (%s) hitting invalid victim", ch && ch->Is_Valid( ) ? ch->Name( ) : "acode" );
    return false;
  }

  index_data* table = NULL;
  int   resist   = 0;

  mod_element_damage( ch, damage, type );

  switch( type ) {
  case ATT_PHYSICAL:
    table = physical_index;
    break;

  case ATT_FIRE:
    table = fire_index;
    resist = victim->Save_Fire( );
    //*** move to softcode
    if( is_set( &victim->dance, DANCE_WINTER ) )
      resist -= 10;
    break;

  case ATT_COLD:
    table    = cold_index;
    resist   = victim->Save_Cold( );
    break;

  case ATT_ACID:
    table    = acid_index;
    resist   = victim->Save_Acid( );
    break;

  case ATT_SHOCK:
    table    = electric_index;
    resist   = victim->Save_Shock( );
    break;

  case ATT_MIND:
    table    = physical_index;
    resist   = victim->Save_Mind( );
    break;

  case ATT_MAGIC:
    table    = physical_index;
    resist   = victim->Save_Magic( );
    break;

  case ATT_POISON:
    table    = poison_index;
    resist   = victim->Save_Poison( );
    break;

  case ATT_HOLY:
    table    = holy_index;
    resist   = victim->Save_Holy( );
    break;

  case ATT_UNHOLY:
    table    = unholy_index;
    resist   = victim->Save_Holy( );
    break;

  default:
    type = ATT_PHYSICAL;
    table = physical_index;
    break;
  }

  if( save ) {
    damage *= ( 100 - resist );
    damage /= 100;
  }

  if( message )
    dam_message( victim, ch, damage, string, lookup( table, damage, plural ) );

  return inflict( victim, ch, damage, death ? death : string );
}


bool damage_cold( char_data* victim, char_data* ch, int damage, const char* string, bool plural )
{
  return damage_element( victim, ch, damage, string, ATT_COLD, NULL, true, true, plural );
}

bool damage_holy( char_data* victim, char_data* ch, int damage, const char* string, bool plural )
{
  return damage_element( victim, ch, damage, string, ATT_HOLY, NULL, true, true, plural );
}

bool damage_unholy( char_data* victim, char_data* ch, int damage, const char* string, bool plural )
{
  return damage_element( victim, ch, damage, string, ATT_UNHOLY, NULL, true, true, plural );
}

bool damage_acid( char_data* victim, char_data* ch, int damage, const char* string, bool plural )
{
  return damage_element( victim, ch, damage, string, ATT_ACID, NULL, true, true, plural );
}


bool damage_shock( char_data* victim, char_data* ch, int damage, const char* string, bool plural )
{
  return damage_element( victim, ch, damage, string, ATT_SHOCK, NULL, true, true, plural );
}


bool damage_fire( char_data* victim, char_data* ch, int damage, const char* string, bool plural )
{
  return damage_element( victim, ch, damage, string, ATT_FIRE, NULL, true, true, plural );
}


bool damage_physical( char_data* victim, char_data* ch, int damage, const char* string, bool plural )
{
  return damage_element( victim, ch, damage, string, ATT_PHYSICAL, NULL, true, true, plural );
}


bool damage_magic( char_data* victim, char_data* ch, int damage, const char* string, bool plural )
{
  return damage_element( victim, ch, damage, string, ATT_MAGIC, NULL, true, true, plural );
}


bool damage_mind( char_data* victim, char_data* ch, int damage, const char* string, bool plural )
{
  return damage_element( victim, ch, damage, string, ATT_MIND, NULL, true, true, plural );
}

bool damage_poison( char_data* victim, char_data* ch, int damage, const char* string, bool plural )
{
  return damage_element( victim, ch, damage, string, ATT_POISON, NULL, true, true, plural );
}
