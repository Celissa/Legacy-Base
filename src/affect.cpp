#include "system.h"


/*
*   CONST TABLES
*/


const char* affect_location[ MAX_AFF_LOCATION ] = {
  "None", 
  "Strength", 
  "Dexterity", 
  "Intelligence", 
  "Wisdom",
  "Constitution", 
  "Magic", 
  "Fire", 
  "Cold", 
  "Electricity",
  "Mind", 
  "Age", 
  "Mana", 
  "Hit Points", 
  "Move", 
  "Undefined-1",
  "Undefined-2",  
  "Armor", 
  "Hitroll", 
  "Damroll", 
  "Mana_regen",
  "Hit_regen", 
  "Move_regen", 
  "Acid", 
  "Poison", 
  "Holy",
  "Physical_Damage",
  "Fire_Damage",
  "Cold_Damage",
  "Acid_Damage",
  "Shock_Damage",
  "Mind_Damage",
  "Magic_Damage",
  "Poison_Damage",
  "Holy_Damage",
  "Unholy_Damage"
};

int affect_loc [ MAX_AFF_LOCATION ] = { 
  APPLY_NONE, 
  APPLY_STR, 
  APPLY_DEX, 
  APPLY_INT, 
  APPLY_WIS,
  APPLY_CON, 
  APPLY_MAGIC, 
  APPLY_FIRE, 
  APPLY_COLD, 
  APPLY_ELECTRIC,
  APPLY_MIND, 
  -1 /*AGE*/, 
  APPLY_MANA, 
  APPLY_HIT, 
  APPLY_MOVE, 
  -1 /*UNUSED*/,
  -1 /*UNUSED*/,
  APPLY_AC, 
  APPLY_HITROLL, 
  APPLY_DAMROLL, 
  APPLY_MANA_REGEN,
  APPLY_HIT_REGEN, 
  APPLY_MOVE_REGEN, 
  APPLY_ACID, 
  APPLY_POISON, 
  APPLY_HOLY,
  APPLY_PHYSICAL_DAM,
  APPLY_FIRE_DAM,
  APPLY_COLD_DAM,
  APPLY_SHOCK_DAM,
  APPLY_MIND_DAM,
  APPLY_MAGIC_DAM,
  APPLY_POISON_DAM,
  APPLY_HOLY_DAM,
  APPLY_UNHOLY_DAM
};


/*
 *   AFFECT CLASS
 */


Affect_Data :: Affect_Data( )
{
  record_new( sizeof( affect_data ), MEM_AFFECT );
  
  leech       = NULL;
  victim      = NULL;
  type        = AFF_NONE;
  duration    = -1;
  level       = 0;
  leech_regen = 0;
  leech_max   = 0;
  
  for( int i = 0; i < MAX_AFF_MODS; i++ ) {
    mlocation[ i ] = APPLY_NONE;
    mmodifier[ i ] = 0;
  }
};

Affect_Data :: ~Affect_Data( )
{
  record_delete( sizeof( affect_data ), MEM_AFFECT );
};


/*
*   DISK ROUTINES
*/


void read_affects( FILE* fp, char_data* ch, bool old_format )
{
  affect_data*  affect;
  int          version;
  int        modifiers;
  int             i, j;
  
  if( old_format )
    version = 0;
  else
    fread( &version, sizeof( int ), 1, fp );

  fread( &i, sizeof( int ), 1, fp );
  
  for( ; i > 0; i-- ) {
    affect = new affect_data;
    fread( &affect->type,     sizeof( int ), 1, fp );
    fread( &affect->duration, sizeof( int ), 1, fp );
    fread( &affect->level,    sizeof( int ), 1, fp );
    if( version > 0 ) {
      // new-style modifier/location information ... first integer the count of the # of modifiers
      fread( &modifiers, sizeof( int ), 1, fp );

      if( modifiers > MAX_AFF_MODS )
        panic( "Too many affect modifiers for the allocated memory!" );

      for( j = 0; j < modifiers; j++ ) {
        // load the number of listed modifiers
        fread( &affect->mmodifier[ j ], sizeof( int ), 1, fp );
        fread( &affect->mlocation[ j ], sizeof( int ), 1, fp ); 
      }
    } else if( version == 0 && ( affect->type == AFF_NONE || affect->type == AFF_HARDINESS || affect->type == AFF_POISON ) ) {
      // old-style modifier/location information
      fread( &affect->mmodifier, sizeof( int ), 1, fp );
      fread( &affect->mlocation, sizeof( int ), 1, fp ); 
    } 

    fread( &j, sizeof( int ), 1, fp );
    if( j != -1 ) {
      affect->leech_regen = j;
      affect->leech       = ch;
      ch->leech_list     += affect;
      fread( &affect->leech_max, sizeof( int ), 1, fp );
    }

    affect->victim = ch;
    modify_affect( ch, affect, TRUE );
    ch->affected += affect;
  }
}


void write_affects( FILE* fp, char_data* ch )
{
  affect_data*  affect;
  int             i, j;
  int        modifiers;
  
  if( is_empty( ch->affected ) )
    return;
  
  fwrite( "Aff2", 4, 1, fp );

  // write file format version info
  int file_version = 2;
  fwrite( &file_version, sizeof( int ), 1, fp );

  // the number of affects to load
  fwrite( &ch->affected.size, sizeof( int ), 1, fp );
  
  for( i = 0; i < ch->affected; i++ ) {
    affect = ch->affected[ i ];

    fwrite( &affect->type,     sizeof( int ), 1, fp );
    fwrite( &affect->duration, sizeof( int ), 1, fp );
    fwrite( &affect->level,    sizeof( int ), 1, fp );

    if( affect->type == AFF_NONE || affect->type == AFF_HARDINESS || affect->type == AFF_POISON || affect->type == AFF_COMPANIONS_STRENGTH ) {  
      // save the affect mods (this probably should be limited to the actual number of locations != APPLY_NONE but this will do for now)
      modifiers = MAX_AFF_MODS;
      fwrite( &modifiers, sizeof( int ), 1, fp );

      for( j = 0; j < modifiers; j++ ) {
        fwrite( &affect->mmodifier[ j ], sizeof( int ), 1, fp );
        fwrite( &affect->mlocation[ j ], sizeof( int ), 1, fp );
      }
    } else {
      // write 0 modifier count
      modifiers = 0;
      fwrite( &modifiers, sizeof( int ), 1, fp );
    }

    // write leech info, or -1 if there is no leech info
    if( affect->leech == ch ) {
      fwrite( &affect->leech_regen, sizeof( int ), 1, fp );
      fwrite( &affect->leech_max,   sizeof( int ), 1, fp );        
    } else {
      j = -1;
      fwrite( &j, sizeof( int ), 1, fp );
    }
  } 
}


void read_affects( FILE* fp, obj_clss_data* obj_clss, bool old_format )
{
  affect_data*  affect;
  char          letter;
  int version, modifiers = 0;
  
  for( ; ; ) {
    if( ( letter = fread_letter( fp ) ) != 'A' )
      break;
    
    affect            = new affect_data;    
    affect->type      = AFF_NONE;
    affect->duration  = -1;

    // determine what version of format we're loading from
    if( !old_format )
      version = fread_number( fp );
    else
      version = 0;

    // if we're loading from a version > 0 then the first entry in the file is the count of modifiers saved
    if( version > 0 )
      modifiers = fread_number( fp );
    else
      modifiers = 1;

    if( modifiers > MAX_AFF_MODS )
      panic( "Too many affect modifiers for the allocated memory!" );

    // then we load the modifiers
    for( int j = 0; j < modifiers; j++ ) {
      affect->mlocation[ j ] = fread_number( fp );
      affect->mmodifier[ j ] = fread_number( fp );
    }
    
    obj_clss->affected += affect;
  }
  
  ungetc( letter, fp );
};


void write_affects( FILE* fp, obj_clss_data* obj_clss )
{
  affect_data* affect;
  
  for( int i = 0; i < obj_clss->affected; i++ ) {
    affect = obj_clss->affected[i]; 

    // write the format version number and number of modifiers saved
    fprintf( fp, "A %d %d", 1 /* version */, MAX_AFF_MODS );

    // write all the modifiers
    for( int i = 0; i < MAX_AFF_MODS; i++ )
      fprintf( fp, " %d %d", affect->mlocation[ i ], affect->mmodifier[ i ] );

    // terminate the entry
    fprintf( fp, "\n" );
  }
  
  return;
};


/*
 *   CHARACTER SUBROUTINES
 */

int prep_max( char_data* ch )
{
  cast_data* cast;
  int           i;
  
  for( i = 0, cast = ch->prepare; cast != NULL; cast = cast->next )
    i += cast->mana*cast->times;
  
  return i;
}


int leech_max( char_data* ch )
{
  int  i, j;
  
  for( i = j = 0; i < ch->leech_list; i++ )
    j += ch->leech_list[i]->leech_max;
  
  return j;
}


int leech_regen( char_data* ch )
{
  int  i, j;
  
  for( i = j = 0; i < ch->leech_list; i++ )
    j += ch->leech_list[i]->leech_regen;
  
  return j;
}


/*
*   DO_AFFECT ROUTINE
*/


const char* source( char_data* ch, int sn )
{
  obj_data*        obj;
  affect_data*  affect;
  
  if( ch->shdata->race < MAX_PLYR_RACE && is_set( plyr_race_table[ch->shdata->race].affect, sn ) )
    return "innate";  
  
  for( int i = 0; i < ch->wearing; i++ )
    if( ( obj = object( ch->wearing[i] ) ) != NULL && is_set( obj->pIndexData->affect_flags, sn ) )
      return obj->Seen_Name( ch, 1, TRUE );
    
  for( int i = 0; i < ch->affected; i++ ) {
    affect = ch->affected[i];
    if( sn == affect->type ) {
      if( affect->leech == NULL ) 
        return "temporary";
      char* tmp = static_string( );
      sprintf( tmp, "leech: %s", affect->leech == ch ? "self" : affect->leech->Seen_Name( ch ) );
      return tmp;
    }
  }
  
  return "unknown";
}


void do_affects( char_data* ch, char* argument )
{
  affect_data*    affect;
  char_data*      victim = ch;
  bool             found = FALSE;
  int              flags;
  
  if( is_confused_pet( ch ) )
    return;
  
  if( get_trust( ch ) >= LEVEL_IMMORTAL ) {
    if( !get_flags( ch, argument, &flags, "lc", "Affect" ) )
      return;

    if( ( victim = one_player( ch, argument, "view affects on", (thing_array*) &player_list ) ) == NULL )
      victim = ch;

    if( is_set( &flags, 0 ) ) {
      if( victim->affected == 0 ) {
        send( ch, "%s has no non-default affects.\r\n", victim );
        return;
      }

      page_underlined( ch, "Dur.   Level   Affect\r\n" );
      for( int i = 0; i < victim->affected; i++ ) {
        affect = victim->affected[i];
        page( ch, "%3d%8d    %s\r\n", affect->duration, affect->level, affect->type == AFF_NONE ? "Unknown?" : aff_char_table[ affect->type ].name );
      }
      return;

    } else if( is_set( &flags, 1 ) ) {
      remove_affect( victim );
      send( ch, "Affects on %s have been cleared.\r\n", victim );
      return;
    }
  }   
  
  for( int i = 0; i < MAX_ENTRY_AFF_CHAR; i++ ) {
    if( is_set( victim->affected_by, i ) && *aff_char_table[ i ].score_name != '\0' ) {
      if( !found ) {
        found = TRUE;
        page_underlined( ch, "%-50s%s\r\n", "Affects", "Source" );
      }
      
      char *temp = str_dup( source( victim, i ) );
      if( aff_char_table[ i ].score_name ) 
        page( ch, "%-50s%s\r\n", aff_char_table[ i ].score_name, truncate( temp, 30 ) );
      else
        bug( "Affect with no score.name: %s", aff_char_table[ i ].name );
      
      delete [] temp;
    }
  }

  if( victim->affected != 0 ) {
    for( int i = 0; i < victim->affected; i++ ) {
      if( !found ) {
        found = TRUE;
        page_underlined( ch, "%-50s%s\r\n", "Affects", "Source" );
      }
      
      affect = victim->affected[ i ];

      for( int j = 0; j < MAX_AFF_MODS; j++ ) {
        if( affect->type == AFF_NONE && affect->mlocation[ j ] >= APPLY_STR && affect->mlocation[ j ] <= APPLY_CON && affect->mmodifier[ j ] != 0 ) {
          if( affect->mlocation[ j ] != APPLY_NONE ) {
            page( ch, "%-50stemporary\r\n",
              affect->mlocation[ j ] == APPLY_STR ? affect->mmodifier[ j ] >= 0 ? "You feel stronger." : "You feel weaker." :
              affect->mlocation[ j ] == APPLY_INT ? affect->mmodifier[ j ] >= 0 ? "You feel more intelligent." : "You feel less intelligent." :
              affect->mlocation[ j ] == APPLY_WIS ? affect->mmodifier[ j ] >= 0 ? "You feel more wise." : "You feel less wise." :
              affect->mlocation[ j ] == APPLY_DEX ? affect->mmodifier[ j ] >= 0 ? "You feel more dextrous." : "You feel less dextrous." :
              affect->mlocation[ j ] == APPLY_CON ? affect->mmodifier[ j ] >= 0 ? "You feel healthier." : "You feel less healthy." : "Unknown"
            );
          }
        }
      }
    }
  }

  if( !found )
    send( ch, "You have not noticed any affects to %s.\r\n", victim == ch ? "yourself" : victim->Name( ) ); 
}


/*
*   AFFECT ROUTINES
*/


void add_affect( char_data* ch, affect_data* paf, bool msg )
{
  affect_data* paf_new  = NULL;
  
  if( paf->type != AFF_NONE ) 
    for( int i = 0; i < ch->affected; i++ ) 
      if( paf->type == ch->affected[i]->type ) {
        paf_new = ch->affected[i];
        paf_new->duration = max( paf_new->duration, paf->duration );
        break;
      }
      
  if( paf_new == NULL ) {
    paf_new = new affect_data;
    
    memcpy( paf_new, paf, sizeof( *paf_new ) );
    modify_affect( ch, paf_new, TRUE, msg );
    
    ch->affected   += paf_new;
    paf_new->leech  = NULL;
    paf_new->victim = ch;
  }
  
  if( paf_new->leech == NULL && paf->leech != NULL ) {
    paf->leech->leech_list   += paf_new;
    paf->leech->max_mana     -= paf->leech_max;
    paf_new->leech            = paf->leech;
    paf_new->leech_regen      = paf->leech_regen;
    paf_new->leech_max        = paf->leech_max;
  }  
}


void remove_affect( char_data* ch )
{
  for( int i = ch->affected.size-1; i >= 0; i-- )
    remove_affect( ch, ch->affected[i] );
}


void remove_affect( char_data* ch, affect_data* affect, bool msg )
{
  ch->affected -= affect;
  
  modify_affect( ch, affect, FALSE, msg );
  remove_leech( affect );

  if( affect->type == AFF_SLEEP && ch->pcdata == NULL && !is_set( &ch->status, STAT_PET ) && ch->reset != NULL && ch->reset->value != RSPOS_SLEEPING )
    do_stand( ch, "" );
  
  delete affect;
}


void strip_affect( char_data* ch, int sn )
{
  for( int i = ch->affected.size - 1; i >= 0; i-- ) {
    if( ch->affected[ i ]->type == sn ) {
      if( ch->affected[ i ]->type == AFF_POISON ) {
        for( int j = 0; j < MAX_AFF_MODS; j++ ) {
          if( ch->affected[ i ]->mmodifier[ j ] > 0 && ch->affected[ i ]->mlocation[ j ] >= 0 )
            strip_affect( ch, ch->affected[ i ]->mlocation[ j ] );
        }
      }
            
      remove_affect( ch, ch->affected[ i ] );
    }
  }
}


bool has_affect( char_data* ch, int sn )
{
  obj_data* obj;
  
  if( sn == AFF_NONE ) 
    return FALSE;
  
  if( ch->shdata->race < MAX_PLYR_RACE && is_set( plyr_race_table[ ch->shdata->race ].affect, sn ) )
    return TRUE;  
  
  for( int i = 0; i < ch->affected; i++ )
    if( ch->affected[i]->type == sn )
      return TRUE;
    
  for( int i = 0; i < ch->wearing; i++ )
    if( ( obj = object( ch->wearing[i] ) ) != NULL && is_set( obj->pIndexData->affect_flags, sn ) )
      return TRUE;
      
  return FALSE;
}


void modify_affect( char_data* ch, affect_data* paf, bool fAdd, bool msg )
{
  char *modify_amount[] = { "slightly", "somewhat", "quite a bit", "much", "**MUCH**" };
  char* affect_name [ 2*MAX_AFF_LOCATION ] = {
    "", "",                                                   //  0  no location
    "stronger", "weaker",                                     //  1  apply strength
    "more agile", "less agile",                               //  2  apply dex
    "more intelligent", "less intelligent",                   //  3  apply int
    "wiser", "less wise",                                     //  4  apply wis
    "tougher", "less tough",                                  //  5  apply con
    "more resistant to magic", "weaker to magic",             //  6  apply magic
    "more resistant to fire", "weaker to fire",               //  7  apply fire
    "more resistant to cold", "weaker to cold",               //  8  apply cold
    "more resistant to electricity", "weaker to electricity", //  9  apply electric
    "more resistant to mind", "weaker to mind",               // 10  apply mind
    "older", "younger",                                       // 11  apply age
    "", "",                                                   // 12  apply mana
    "", "",                                                   // 13  apply hitpoints
    "", "",                                                   // 14  apply movement
    "", "",                                                   // 15  unknown
    "", "",                                                   // 16  unknown
    "more armored", "less armored",                           // 17  apply armor
    "", "",                                                   // 18  apply hitroll
    "", "",                                                   // 19  apply damroll
    "", "",                                                   // 20  apply mana regen
    "", "",                                                   // 21  apply hit regen
    "", "",                                                   // 22  apply move regen
    "more resistant to acid", "weaker to acid",               // 23  apply acid  
    "more resistant to poison", "weaker to poison",           // 24  apply poison
    "more resistant to (un)holy", "weaker to (un)holy",       // 25  apply holy
    "", "",                                                   // 26  apply physical damage
    "", "",                                                   // 26  apply fire damage
    "", "",                                                   // 27  apply cold damage
    "", "",                                                   // 28  apply acid damage
    "", "",                                                   // 29  apply shock damage
    "", "",                                                   // 30  apply mind damage
    "", "",                                                   // 31  apply magic damage
    "", "",                                                   // 32  apply poison damage
    "", "",                                                   // 33  apply holy damage
    "", "",                                                   // 34  apply unholy damage
  };
    
//  int      mod;
//  int        i;
  bool   error;

  if( paf->type != AFF_NONE ) {
    if( fAdd ? is_set( ch->affected_by, paf->type ) : has_affect( ch, paf->type ) )
      return;

    if( paf->type != AFF_POISON && paf->type != AFF_COMPANIONS_STRENGTH ) { 
      for( int j = 0; j < MAX_AFF_MODS; j++ ) {
        paf->mlocation[ j ] = aff_char_table[ paf->type ].mlocation[ j ];

        if( aff_char_table[ paf->type ].mmodifier[ j ]  )
          paf->mmodifier[ j ] = evaluate( aff_char_table[ paf->type ].mmodifier[ j ], error, paf->level, 0 );
        else 
          bug( "Affect with no modify: %s", aff_char_table[ paf->type ].name );
        
        if( error ) {
          bug( "Modify_Affect: Failed evaluate." );
          bug( "-- Aff = %s", aff_char_table[ paf->type ].name ); 
        }
      }
    }

    if( fAdd ) {
      if( ch->array != NULL ) {
        if( msg ) {
          if( aff_char_table[ paf->type ].msg_on ) {
            act( ch, aff_char_table[ paf->type ].msg_on, ch );
          } else {
            bug( "Affect with no msg.on: %s", aff_char_table[ paf->type ].name );
          }

          if( aff_char_table[ paf->type ].msg_on_room ) {
            act_notchar( aff_char_table[ paf->type ].msg_on_room, ch );
          } else {
            bug( "Affect with no msg.on_room: %s", aff_char_table[ paf->type ].name );
          }
        }
      }
      set_bit( ch->affected_by, paf->type );  
    } else {
      remove_bit( ch->affected_by, paf->type );
      if( ch->array != NULL ) {
        if( msg ) {
          if( aff_char_table[ paf->type ].msg_off ) {
            act( ch, aff_char_table[ paf->type ].msg_off, ch, NULL, NULL, NULL, VIS_ALL );
          } else {
            bug( "Affect with no msg.off: %s", aff_char_table[ paf->type ].name );
          }

          if( aff_char_table[ paf->type ].msg_off_room ) {
            act_notchar( aff_char_table[ paf->type ].msg_off_room, ch );
          } else {
            bug( "Affect with no msg.off_room: %s", aff_char_table[ paf->type ].name );
          }
        }
      }
    }
  }

  for( int j = 0; j < MAX_AFF_MODS; j++ ) {
    if( paf->type == AFF_POISON && paf->mmodifier[j] > 0 )
      continue;
    
    int mod = paf->mmodifier[j]*( fAdd ? 1 : -1 );
    
    if( mod == 0 )
      continue;
    
    switch( paf->mlocation[j] ) {
    case APPLY_AGE:
      if( ch->species == NULL && ch->pcdata != NULL )
        ch->pcdata->mod_age += mod;
      break;
    
    case APPLY_NONE:
      break;

    case APPLY_INT:
      ch->mod_int                           += mod;
      break;

    case APPLY_STR:
      ch->mod_str                           += mod;   
      break;

    case APPLY_DEX:
      ch->mod_dex                           += mod;   
      break;

    case APPLY_WIS:
      ch->mod_wis                           += mod;   
      break;

    case APPLY_CON:
      ch->mod_con                           += mod;   
      break;

    case APPLY_MOVE:
      ch->mod_move                          += mod;   
      break;

    case APPLY_MAGIC:
      ch->mod_resist[ RES_MAGIC ]           += mod;   
      break;

    case APPLY_FIRE:
      ch->mod_resist[ RES_FIRE ]            += mod;   
      break;

    case APPLY_COLD:
      ch->mod_resist[ RES_COLD ]            += mod;   
      break;

    case APPLY_ELECTRIC:
      ch->mod_resist[ RES_SHOCK ]           += mod;   
      break;

    case APPLY_MIND:
      ch->mod_resist[ RES_MIND ]            += mod;   
      break;

    case APPLY_ACID:
      ch->mod_resist[ RES_ACID ]            += mod;   
      break;

    case APPLY_POISON:
      ch->mod_resist[ RES_POISON ]          += mod;   
      break;

    case APPLY_HOLY:
      ch->mod_resist[ RES_HOLY ]            += mod;   
      break;

    case APPLY_MANA:
      ch->mod_mana                          += mod;   
      break;

    case APPLY_HIT:
      ch->mod_hit                           += mod;   
      break;

    case APPLY_AC:
      ch->mod_armor                         += mod;   
      break;

    case APPLY_HITROLL:
      ch->hitroll                           += mod;   
      break;

    case APPLY_DAMROLL:
      ch->damroll                           += mod;   
      break;

    case APPLY_MANA_REGEN:
      ch->mana_regen                        += mod;   
      break;

    case APPLY_HIT_REGEN:
      ch->hit_regen                         += mod;   
      break;

    case APPLY_MOVE_REGEN:
      ch->move_regen                        += mod;   
      break;

    case APPLY_PHYSICAL_DAM:
      ch->mod_damage[ ATT_PHYSICAL ]        += mod;
      break;

    case APPLY_FIRE_DAM:
      ch->mod_damage[ ATT_FIRE ]            += mod;
      break;

    case APPLY_COLD_DAM:
      ch->mod_damage[ ATT_COLD ]            += mod;
      break;

    case APPLY_ACID_DAM:
      ch->mod_damage[ ATT_ACID ]            += mod;
      break;

    case APPLY_SHOCK_DAM:
      ch->mod_damage[ ATT_SHOCK ]           += mod;
      break;

    case APPLY_MIND_DAM:
      ch->mod_damage[ ATT_MIND ]            += mod;
      break;

    case APPLY_MAGIC_DAM:
      ch->mod_damage[ ATT_MAGIC ]           += mod;
      break;

    case APPLY_POISON_DAM:
      ch->mod_damage[ ATT_POISON ]          += mod;
      break;

    case APPLY_HOLY_DAM:
      ch->mod_damage[ ATT_HOLY ]            += mod;
      break;

    case APPLY_UNHOLY_DAM:
      ch->mod_damage[ ATT_UNHOLY ]          += mod;
      break;
      
    default:
      bug( "Modify_affect: unknown location %d.", paf->mlocation[j] );
      continue;;
    }
    
    int i = 2*( paf->mlocation[j] )+( mod < 0 );
    
    if( msg && ch->link != NULL && ch->link->connected == CON_PLAYING && affect_name[i][0] != '\0' && is_set( ch->pcdata->pfile->flags, PLR_AFFECT_DISPLAY ) )
      send( ch, "[You feel %s %s.]\r\n", modify_amount[ min( max( mod, -mod ), 5 ) - 1 ], affect_name[ i ] );  
    
    if( ch->array != NULL )
      update_maxes( ch );
  }
}


/*
*   OBJECT AFFECT ROUTINES
*/


void update_affect( event_data* event )
{
  obj_data*       obj  = (obj_data*)    event->owner;
  affect_data* affect  = (affect_data*) event->pointer;
  
  if( affect->type == AFF_BURNING ) {
    if( ( obj->condition -= 10 ) > 0 ) 
      add_queue( event, 100 );
    else {
      remove_affect( obj, affect );
      obj->Extract( );
    }
    return;
  }
  
  if( --affect->duration > 0 ) {
    add_queue( event, 100 );
    return;
  }
  
  obj->events -= event;
  remove_affect( obj, affect );
  
  delete event;
}


void write_affects( FILE* fp, obj_data* obj )
{
  affect_data*  affect;
  
  if( is_empty( obj->affected ) )
    return;
  
  fwrite( "Aff2", 4, 1, fp );

  // write file format version info
  int file_version = 0; // starting at version 0 since I haven't modified the file format yet
  fwrite( &file_version, sizeof( int ), 1, fp );

  fwrite( &obj->affected.size, sizeof( int ), 1, fp );
  
  for( int i = 0; i < obj->affected; i++ ) {
    affect = obj->affected[i];
    fwrite( &affect->type,     sizeof( int ), 1, fp );
    fwrite( &affect->duration, sizeof( int ), 1, fp );
    fwrite( &affect->level,    sizeof( int ), 1, fp );
  } 
}


void read_affects( FILE* fp, obj_data* obj, bool old_format )
{
  affect_data*  affect;
  event_data*    event;
  int                i;
  int          version;

  if( old_format )
    version = 0;
  else
    fread( &version, sizeof( int ), 1, fp );

  fread( &i, sizeof( int ), 1, fp );
  
  for( ; i > 0; i-- ) {
    affect = new affect_data;
    fread( &affect->type,     sizeof( int ), 1, fp );
    fread( &affect->duration, sizeof( int ), 1, fp );
    fread( &affect->level,    sizeof( int ), 1, fp );
    obj->affected += affect;
    
    event          = new event_data( update_affect, obj );
    event->pointer = (void*) affect;
    add_queue( event, 100 );
  }
}


void add_affect( obj_data* obj, affect_data* paf )
{
  affect_data*   paf_new;
  char_data*          ch;
  event_data*      event;
  
  for( int i = 0; i < obj->affected; i++ )
    if( paf->type == obj->affected[i]->type ) {
      obj->affected[i]->duration = max( obj->affected[i]->duration, paf->duration );
      return;
    }
    
  if( obj->array != NULL ) {
    if( aff_obj_table[ paf->type ].msg_on ) {
      if( (ch = character( obj->array->where )) != NULL )
        act( ch, aff_obj_table[ paf->type ].msg_on, ch, NULL, obj );
    } else {
      bug( "Affect with no msg.on: %s", aff_obj_table[ paf->type ].name );
    }
  }
  
  paf_new = new affect_data;
  
  memcpy( paf_new, paf, sizeof( *paf_new ) );
  
  obj->affected  += paf_new;
  paf_new->leech  = NULL;
  paf_new->victim = NULL;
  
  event          = new event_data( update_affect, obj );
  event->pointer = (thing_data*) paf_new;
  add_queue( event, 100 );
  for( int i = 0; i < MAX_AFF_MODS; i++ ) {
  if( aff_obj_table[ paf->type ].mlocation[i] != 0 )
  set_bit( obj->extra_flags, aff_obj_table[ paf->type ].mlocation[i] );
  }
}


bool is_affected( obj_data* obj, int type )
{
  for( int i = 0; i < obj->affected; i++ )
    if( obj->affected[i]->type == type )
      return TRUE;
    
    return FALSE;
}


void remove_affect( obj_data* obj, affect_data* affect, bool msg )
{
  char_data*             ch; 
  room_data*          where;
  int                  flag;
  event_data*         event;
  
  for( int i = 0; i < MAX_AFF_MODS; i++ ) {
    flag           = aff_obj_table[ affect->type ].mlocation[i];
    obj->selected  = 1;
    obj->affected -= affect;
  
  if( !is_set( obj->pIndexData->extra_flags, flag ) && !is_affected( obj, affect->type ) )
    remove_bit( obj->extra_flags, flag );
  
  if( msg ) {
    if( aff_obj_table[ affect->type ].msg_off ) {
      if( obj && obj->array && ( where = Room( obj->array->where ) ) != NULL )
        act_room( where, aff_obj_table[ affect->type ].msg_off, obj );
      else if( ( ch = character( obj->array->where ) ) != NULL )
        act( ch, aff_obj_table[ affect->type ].msg_off, ch, NULL, obj );
    } else {
      bug( "Affect with no msg.off: %s", aff_obj_table[ affect->type ].name );
    }
  }
  }
  for( int i = 0; i < obj->events; i++ ) {
    event = obj->events[i];
    if( event->pointer == (thing_data*) affect ) {
      extract( event );
      break;
    }
  }
  
  delete affect;
}


bool strip_affect( obj_data* obj, int sn )
{
  affect_data*  affect;

  for( int i = 0; i < MAX_AFF_MODS; i++ ) {
  int             flag  = aff_obj_table[ sn ].mlocation[i];
  
  if( !is_set( obj->extra_flags, flag ) || is_set( obj->pIndexData->extra_flags, flag ) )
    return FALSE;
  }
  for( int i = 0; i < obj->affected; i++ ) {
    affect = obj->affected[i];
    if( affect->type == sn ) {
      remove_affect( obj, affect, FALSE );
      return TRUE;
    }
  }
  for( int i = 0; i < MAX_AFF_MODS; i++ ) {
    int flag = aff_obj_table[ sn ].mlocation[i];
  remove_bit( obj->extra_flags, flag );
  }
  
  roach( "Strip_Affect: Affect on object not found." );
  roach( "-- Obj = %s", obj );
  
  return TRUE;
}


/*
*
*/


int affect_level( char_data* ch, int bit ) 
{
  if( is_set( ch->affected_by, bit ) ) 
    for( int i = 0; i < ch->affected; i++ )
      if( ch->affected[i]->type == bit ) 
        return ch->affected[i]->level;
      
  return 0;
}


int affect_duration( char_data* ch, int bit ) 
{
  if( is_set( ch->affected_by, bit ) ) 
    for( int i = 0; i < ch->affected; i++ )
      if( ch->affected[i]->type == bit ) 
        return ch->affected[i]->duration;
      
  return 0;
}
