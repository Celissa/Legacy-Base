#include "system.h"

int    current_condition             ( char_data* );

const char* plr_name [MAX_PLR] = {
  "Autoassist",
  "Autoexit",
  "Autobackstab",
  "Autoskin",
  "Affect.Mod",
  "Autosplit",
  "Brief",
  "Chant",
  "Chat",
  "Ctell",
  "Email.Public",
  "Gossip",
  "Group.Incog",
  "Numeric",
  "Idle",
  "Info.Repeat",
  "Info",
  "Lang.Id.",
  "Spamback",
  "No.Summon",
  "Ooc",
  "Parry",
  "Pet.Assist",
  "Prompt",
  "Reverse",
  "Guild.Chat",
  "Say.Repeat",
  "Say.RepColor",
  "Searching",
  "Sneak",
  "Status.Bar",
  "Track",
  "Portal",
  "Atalk",
  "Pet.Inventory",
  "No.Title",
  "No.Emote",
  "No.Shout",
  "No.Tell",
  "Deny",
  "Freeze",
  "Edit_Help",
  "Familiar",
  "Leader",
  "Enrolled",
  "#approved",
  "Donated",
  "Valid.Email",
  "Crash.Quit",
  "Avatar.Chnnl",
  "Build.Chnnl",
  "Wizinvis",
  "Holylight",
  "Logged",
  "Imm.Chnnl",
  "No.Communication"
};

const char* act_name [] = { "Is.Npc", "Sentinel", "Scavenger", "Can.Tame",
  "Aggressive", "Stay.Area", "Wimpy", "Can.Trip", "Warm.blooded",
  "Summonable", "Assist.Group", "Can.Fly", "Can.Swim", "Mount",
  "Can.Climb", "Can.Carry", "Has.Eyes", "Has.Skeleton", "Ghost",
  "Zero.Regen", "Slithers", "Oozes", "No.Bash", "Mimic", "No.Block",
  "Shatters", "Aggr.Evil", "Aggr.Good", "Carnivore", "Elemental",
  "Use.The", "#approved", "No.Stun", "No.Crit", "No.Disarm", "No.Trip",
  "Fire.Aura", "Cold.Aura", "Shock.Aura", "Acid.Aura", "Aggr.Neut",
  "Shift.Cast"
};

const char* no_permission =
  "You do not have permission to alter that species\r\n";
/*
const char* stats_name [] = { "Str", "Int", "Wis", "Dex", "Con",
  "Level", "Piety", "Class", "Alignment", "Magic", "Fire", "Cold", 
  "Electric", "Mind", "Age", "Mana", "Hit_Points", "Move", "AC", 
  "Hitroll", "Damroll", "Mana_Regen", "Hit_Regen", "Move_Regen", 
  "Acid", "Poison", "none" };
*/
bool mobs_modified  = TRUE;


int   select         ( species_data*, char_data*, char* );
void  display        ( species_data*, char_data*, char*, int& );
void  medit_replace  ( char_data*, char* );
void  medit_renumber ( char_data* ch, int old_vnum, int new_vnum );

/*
 *   MLOAD FUNCTION
 */


void do_mload( char_data* ch, char* argument )
{
  species_data*  species;
  mob_data*          mob;
    
  if( *argument == '\0' || !is_number( argument ) ) {
    send( ch, "Usage: mload <vnum>.\r\n" );
    return;
  }

  if( ( species = get_species( atoi( argument ) ) ) == NULL ) {
    send( ch, "No mob has that vnum.\r\n" );
    return;
  }

  mob = create_mobile( species );
  mreset_mob( mob );

  mob->To( ch->array );
  send( ch, "You create %s.\r\n", mob );
  send( *ch->array, "%s has created %s!\r\n", ch, mob );
}


/*
 *   MFIND ROUTINES
 */


int select( species_data* species, char_data* ch, char* argument )
{
  char               tmp  [ MAX_INPUT_LENGTH ];
  char            letter;
  char            hyphen;
  const char*     string;
  bool          negative;
  int                  i;
  int             length;
  int           min, max;
  mprog_data*      mprog;

  for( ; ; ) {
    if( ( hyphen = *argument ) == '\0' )
      return 1;

    if( hyphen != '-' ) {
      letter = 'n';
    } else {
      argument++;
      if( !isalpha( letter = *argument++ ) ) {
        send( ch, "Illegal character for flag - See help mfind.\r\n" );
        return -1;
      }
    }

    negative = FALSE;
    skip_spaces( argument );

    if( *argument == '!' ) {
      negative = TRUE;
      argument++;
    }

    if( *argument == '-' || isspace( *argument ) || *argument == '\0' ) {
      send( ch, "All flags require an argument - See help mfind.\r\n" );
      return -1;
    }
  
    for( i = 0; strncmp( argument-1, " -", 2 ) && *argument != '\0'; ) {
      if( i > ONE_LINE-2 ) {
        send( ch, "Flag arguments must be less than one line.\r\n" );
        return -1;
      } 
      tmp[i++] = *argument++;
    }

    for( ; isspace( tmp[i-1] ); i-- );

    tmp[i] = '\0';
    string = NULL;
    length = strlen( tmp );

    switch( letter ) {
      case 'g' :  string = group_table[species->group].name;                   break;
      case 'c' :  string = species->creator;                                   break;
      case 'r' :  string = race_table[species->shdata->race].name;             break; 
      case 'N' :  string = nation_table[species->nation].name;                 break;
      case 'A' :  string = alignment_table[species->shdata->alignment].abbrev; break;
    }

    if( string != NULL ) {
      if( !strncasecmp( tmp, string, length ) == negative )
        return 0;
      
      continue;
    }

    if( letter == 'n' ) {
      if( !is_name( tmp, species->MfindName( ) ) )
        return 0;
      
      continue;
    }

    if( letter == 'l' ) {
      atorange( tmp, min, max );
      if( species->shdata->level < min || species->shdata->level > max )
        return 0;

      continue;
    }

    if( letter == 'w' ) {
      for( i = 0; i < MAX_WEAR; i++ ) {
        if( !strncasecmp( tmp, wear_part_name[i], length ) ) {
          if( is_set( &species->wear_part, i ) == negative )
            return 0;
          break;
        }
      }
      
      continue;
    }

    if( letter == 'f' ) {
      for( i = 0; i < MAX_ACT; i++ ) {
        if( !strncasecmp( tmp, act_name[i], length ) ) {
          if( is_set( species->act_flags, i ) == negative )
            return 0;
          break;
        }
      }
      
      continue;
    }

    if( letter == 'a' ) {
      for( i = 0; i < MAX_ENTRY_AFF_CHAR; i++ ) {
        if( !strncasecmp( tmp, aff_char_table[i].name, length ) ) {
          if( is_set( species->affected_by, i ) == negative )
            return 0;
          break;
        }
      }

      continue;
    }

    if( letter == 'T' ) {
      for( i = 0; !fmatches( tmp, mprog_trigger[i] ); i++ ) {
        if( i == MAX_MPROG_TRIGGER-1 ) {
          send( ch, "Unknown trigger type, see help mfind.\r\n" );
          return -1;
        }
      }

      for( mprog = species->mprog; mprog != NULL && mprog->trigger != i; mprog = mprog->next );
      if( ( mprog != NULL ) != negative )
        continue;
      
      return 0;
    }

    send( ch, "Unknown flag - See help mfind.\r\n" );
    return -1;
  }
}


void display( species_data* species, char_data* ch, char* buf, int& length )
{
  dice_data  dice;
  int          hp;
 
  dice = species->hitdice;
  hp   = average( dice );

  char *temp = str_dup(species->Name());
  sprintf( &buf[length], "[%5d] %-30s %5d %5s %5d %5d\r\n", species->vnum, truncate(temp, 30), species->shdata->level,
    species->shdata->deaths == 0 ? "??" : int5( species->exp/species->shdata->deaths ), hp, species->shdata->deaths );
  length += strlen( &buf[length] );

  if( length > MAX_STRING_LENGTH-100 ) {
    page( ch, buf );
    length  = 0;
    *buf    = '\0';
  }        
}

int current_condition( char_data* ch )
{
  float      percentage  = ch->hit/ch->max_hit;
  int        number = (int) ( ch->original_hit*percentage );
  
  return number;
}

void do_mfind( char_data* ch, char* argument )
{
  const char* title_msg =
    "Vnum    Name                           Level   Exp    Hp  Dths\r\n";

  char                buf  [ MAX_STRING_LENGTH ];
  species_data*   species;
  int                   i;
  int              length  = 0;
  bool              found  = FALSE;

  for( i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) != NULL ) {
      switch( select( species, ch, argument ) ) {
       case -1 : return;
       case  1 :
        if( !found ) {
          found = TRUE;
          page_underlined( ch, title_msg );
        }
        display( species, ch, buf, length );
      }
    }
  }

  if( !found ) 
    send( ch, "No creature matching search was found.\r\n" );
  else
    page( ch, buf );
}
        

/*
 *   MWHERE ROUTINE
 */


char* status( mob_data* mob )
{
  char* tmp;

  if( mob->leader != NULL
    && is_set( &mob->status, STAT_PET ) ) {
    tmp = static_string( );
    sprintf( tmp, "Pet of %s", mob->leader->Name( ) );
    return tmp;
    }

  if( mob->reset != NULL )
    return "Reset";

  return "Mloaded?";
}


void do_mwhere( char_data* ch, char* argument )
{
  char          tmp1  [ TWO_LINES ];
  char          tmp2  [ TWO_LINES ];
  mob_data*   victim;
  int           vnum;
  bool         found;
  const char*   name;

  if( *argument == '\0' ) {
    send( ch, "Usage: mwhere <mob>\r\n" );
    return;
  }

  vnum = atoi( argument );

  sprintf( tmp1, "%26s  %6s  %-15s  %s\r\n", "Room", "Vnum", "Status", "Area" );
  page_underlined( ch, tmp1 );

  for( int i = 0; i < MAX_SPECIES; i++ ) {
    if( species_list[i] == NULL )
      continue;
    
    name = species_list[i]->Name( );
    
    if( vnum != i && !is_name( argument, name ) )
      continue;

    page_divider( ch, name, i );
    found = FALSE;

    for( int j = 0; j < mob_list; j++ ) {
      victim = mob_list[j];
      if( victim->Is_Valid() && victim->species->vnum == i ) {
        strcpy( tmp1, victim->in_room->name );
        strcpy( tmp2, victim->in_room->area->name ); 
        truncate( tmp1, 25 );
        truncate( tmp2, 20 );
        page( ch, "%26s  %6d  %-15s  %s\r\n", tmp1, victim->in_room->vnum, status( victim ), tmp2 );
        found = TRUE;
      }
    }

    if( !found )
      page_centered( ch, "None found" );
  }

  return;
}


/*
 *   MEDIT
 */


bool char_data :: can_edit( species_data* species )
{
/*
  if( species == NULL || has_permission( this, PERM_ALL_MOBS ) 
    || !strcmp( species->creator, descr->name ) ) 
    return TRUE;
*/
  if( species == NULL )
    return TRUE;

  if( is_set( species->act_flags, ACT_APPROVED ) && !has_permission( this, PERM_MOB_APPROVAL ) ) {
    send( this, "This mob is approved and may not be altered.\r\n" );
    return FALSE;
  }

  if( has_permission( this, PERM_ALL_MOBS ) || !strcmp( species->creator, descr->name ) )
    return TRUE;

  send( this, "You may not alter this mob.\r\n" );
  return FALSE;
}


void do_medit( char_data* ch, char* argument )
{
  char                     arg  [ MAX_INPUT_LENGTH ];
  char_data*            victim;
  descr_data*            descr;
  share_data*           shdata;
  species_data*        species;
  species_data*   species_copy  = NULL;
  wizard_data*          imm;
  int i, j;

  imm = (wizard_data*) ch;

  if( exact_match( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      if( ( species = imm->mob_edit ) == NULL ) {
        send( ch, "Which mob do you want to delete?\r\n" );
        return;
      }
      
      if( !ch->can_edit( species ) )
        return;
    } else if( ( species = get_species( atoi( argument ) ) ) == NULL ) {
      send( ch, "There is no species with that number.\r\n" );
      return;
    }

    if( !can_extract( species, ch ) )
      return;

    imm->mob_edit = species;
    send( ch, "You genocide mob %d, %s.\r\n", species->vnum, species );
    extract( imm, offset( &imm->mob_edit, imm ), "species" );
    species_list[ species->vnum ] = NULL;
    delete species;
    return;
  }
  
  if( exact_match( argument, "new" ) ) {
    if( isdigit( *argument ) ) {
      argument = one_argument( argument, arg );
      species_copy = get_species( atoi( arg ) );
      if( species_copy == NULL ) {
        send( ch, "No mob has given vnum to copy.\r\n" );
        return;
      }
    } else
      species_copy = get_species( MOB_BLANK );

    if( *argument == '\0' ) {
      send( ch, "You need to give the mob a name.\r\n" );
      return;
    }

    species = new species_data;
    shdata  = new share_data;
    descr   = new descr_data;
   
    for( i = 1; get_species( i ) != NULL; i++ );

    memcpy( species, species_copy, sizeof( *species ) );
    memcpy( shdata, species_copy->shdata, sizeof( *species->shdata ) );
    
    species->shdata = shdata;
    species->descr  = descr;
    species->vnum   = i;

    descr->keywords      = alloc_string( argument, MEM_DESCR );
    descr->singular      = alloc_string( argument, MEM_DESCR );
    descr->plural        = alloc_string( argument, MEM_DESCR );
    descr->complete      = alloc_string( species_copy->descr->complete,
                           MEM_DESCR );
    descr->long_s = alloc_string( "is here", MEM_DESCR );
    descr->long_p   = alloc_string( "are here", MEM_DESCR );

    species->creator = alloc_string( ch->descr->name, MEM_DESCR );     

    species->attack       = new program_data( species );
    species->attack->code = alloc_string( species_copy->attack->code, MEM_MPROG );

    species->mprog = NULL;
    species->reset = NULL;
 
    zero_exp( species );

    for( i = 0; i < MAX_ARMOR; i++ )
      species->part_name[i] = alloc_string( species_copy->part_name[i], MEM_SPECIES );
    species->wear_part = 0;

    for( i = 0; i < AFFECT_INTS; i++ )
      species->affected_by[i] = 0;

    species_list[ species->vnum ] = species;

    create_mobile( species )->To( ch->array );
    send( ch, "New mob created, assigned vnum %d.\r\n", species->vnum );

    imm->mob_edit    = species;
    imm->mpdata_edit = NULL;
    imm->player_edit = NULL;

    return;
  }

  if( is_spirit( ch ) && exact_match( argument, "renumber" ) ) {
    if( !number_arg( argument, i ) || !number_arg( argument, j ) ) {
      send( ch, "Syntax: medit renumber <vnum_old> <vnum_new>.\r\n" );
      return;
    }

    species_data* mob1 = get_species( i );
    species_data* mob2 = get_species( j );

    if( !mob1 ) {
      send( ch, "Mob %d does not exist.\r\n", i );
      return;
    } else if( mob2 ) {
      send( ch, "Mob %d exists, delete it first.\r\n", j );
      return;
    } else if( j >= MAX_SPECIES ) {
      send( ch, "Pick a number between 1 and %d.\r\n", MAX_SPECIES );
      return;
    }

    if( !ch->can_edit( mob1 ) ) {
      send( ch, no_permission );
      return;
    }

    medit_renumber( ch, i, j );
    return;
  }

  if( is_demigod(ch) && exact_match( argument, "reset" ) ) {
    if( *argument == '\0' ) {
      if( ( species = imm->mob_edit ) == NULL ) {
        send( ch, "Which mob do you want to reset the XP on?\r\n" );
        return;
      }
      
      if( !ch->can_edit( species ) )
        return;
    } else if( ( species = get_species( atoi( argument ) ) ) == NULL ) {
      send( ch, "There is no mob with that vnum.\r\n" );
      return;
    }

    send( ch, "You reset the exp for mob %d, %s.\r\n", species->vnum, species );
    mob_log( ch, species->vnum, "Exp reset." );
    zero_exp( species );
    
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "Who do you want to edit?\r\n" );
    return;
  }

  if( isdigit( *argument ) ) {
    if( ( species = get_species( atoi( argument ) ) ) == NULL ) {
      send( ch, "No mob has that vnum.\r\n" );
      return;
    }
  } else {
    if( ( victim = one_character( ch, argument, "medit", ch->array, (thing_array*) &player_list ) ) == NULL )
      return;
    
    if( victim->species == NULL ) {
      if( ch != victim ) {
        if( is_apprentice( victim ) && !is_demigod( ch ) ) {
          send( ch, "You can't edit immortals.\r\n" );
          return;
        }
        if( get_trust( victim ) >= get_trust( ch ) ) {
          send( ch, "You can't edit them.\r\n" );
          return;
        }
        if( !has_permission( ch, PERM_PLAYERS ) ) {
          send( ch, "You can't edit players.\r\n" );
          return;
        }
      }

      imm->player_edit = (player_data*) victim;
      imm->mob_edit    = NULL;
      imm->mprog_edit  = NULL;
      imm->mpdata_edit = NULL;

      send( ch, "Mset and mflag now operate on %s.\r\n", victim );
      return;
    }
    species = victim->species;    
  }

  imm->player_edit  = NULL;
  imm->mob_edit     = species;
  imm->mprog_edit   = NULL;
  imm->mpdata_edit  = NULL;

  send( ch, "Mset and mflag now operate on %s.\r\n", species );
}


void medit_replace( char_data* ch, char* argument )
{
  reset_data*        reset;
  room_data*          room;
  species_data*   species1  = NULL;
  species_data*   species2  = NULL;
  int                count  = 0;
  int                 i, j;

  if( !number_arg( argument, i ) || !number_arg( argument, j ) ) {
    send( ch, "Syntax: medit replace <vnum_old> <vnum_new>.\r\n" );
    return;
  }

  if( ( species1 = get_species( i ) ) == NULL || ( species2 = get_species( j ) ) == NULL ) {
    send( ch, "Vnum %d doesn't coorespond to an existing species.\r\n", species1 == NULL ? i : j );
    return;
  }

  area_nav alist(&area_list);
  for( area_data *area = alist.first( ); area; area = alist.next( ) )
    for( room = area->room_first; room != NULL; room = room->next ) 
      for( reset = room->reset; reset != NULL; reset = reset->next ) 
        if( reset->target == species1 && is_set( &reset->flags, RSFLAG_MOB ) ) {
          reset->target = species2;
          count++;
          area->modified = TRUE;
        }

//#pragma message ( "medit_replace requires script search for reset change" ) 

  send( ch, "Species %d replaced by %d in %d reset%s.\r\n", i, j, count, count != 1 ? "s" : "" );

  return;
}


/*
 *   MARMOR
 */


void do_marmor( char_data *ch, char* argument )
{
  char               arg  [ MAX_STRING_LENGTH ];
  char               buf  [ MAX_STRING_LENGTH ];
  species_data*  species;
  wizard_data*    imm;
  int             chance;
  int              armor; 
  int                  i;

  imm = (wizard_data*) ch;

  if( ( species = imm->mob_edit ) == NULL ) {
    send( "You aren't editing any mob.\r\n", ch );
    return;
    }
/*
  if( IS_SET( species->act_flags, ACT_HUMANOID ) ) {
    sprintf( buf, "%s is a humanoid so marmor has no affect.\r\n",
      species->Name() );
    buf[0] = toupper( buf[0] );
    send( buf, ch );
    return;
    }
*/
  if( argument[0] == '\0' ) {
    for( i = 0; i < MAX_ARMOR; i++ ) {
      sprintf( buf, "[%2d] %5d %5d %s\r\n", i+1,
        species->chance[i], species->armor[i], species->part_name[i] );
      send( buf, ch );
      } 
    return;
    }

  if( !ch->can_edit( species ) )
    return;

  argument = one_argument( argument, arg );
  i = atoi( arg )-1;

  if( i < 0 || i >= MAX_ARMOR ) {
    send( "Part number out of range.\r\n", ch );
    return;
    }

  argument = one_argument( argument, arg );
  chance =  atoi( arg );
  
  if( chance < 0 || chance > 1000 ) {
    send( "Chance out of range.\r\n", ch );
    return;
    }

  argument = one_argument( argument, arg );
  armor =  atoi( arg );
  
  if( armor < -100 || armor > 10000 ) {
    send( "Armor out of range.\r\n", ch );
    return;
    }

//  zero_exp( species );

  species->armor[i]     = armor;
  species->chance[i]    = chance;
  species->part_name[i] = alloc_string( argument, MEM_SPECIES );

  send( ch, "Ok.\r\n" );
}


/*
 *   MDESC
 */


void do_mdesc( char_data* ch, char* argument )
{
  species_data*  species;
  wizard_data*    imm;

  imm = (wizard_data*) ch;

  if( ( species = imm->mob_edit ) == NULL ) {
    send( ch, "You aren't editing any mob - use medit <mob>.\r\n" );
    return;
    }

  if( *argument != '\0' && !ch->can_edit( species ) )
    return;

  species->descr->complete = edit_string( ch, argument, species->descr->complete, MEM_DESCR );

  if( *argument != '\0' ) {
    char tmp  [ MAX_STRING_LENGTH ];
    sprintf( tmp, "mde: %s", argument );
    mob_log( ch, species->vnum, tmp );
  }

  return;
}


/*
 *   MFLAG
 */


void do_mflag( char_data* ch, char* argument )
{
  char_data*            pc;
  species_data*        mob;
  wizard_data*         imm;
  const char*     response;

  #define types 3
  
  imm = (wizard_data*) ch;  
  pc = imm->player_edit;
  mob    = imm->mob_edit;

  if( pc == NULL && mob == NULL ) {
    send( ch, "You aren't editing a mob or player.\r\n" );
    return;
  }

  if( pc == NULL ) {    
    const char* title [types] = { "Act", "Affect", "Wear" };
    int max [types] = { MAX_ACT, MAX_ENTRY_AFF_CHAR, MAX_WEAR };

    const char** name1 [types] = { &act_name[0], &aff_char_table[0].name,
      &wear_part_name[0] }; 
    const char** name2 [types] = { &act_name[1], &aff_char_table[1].name,
      &wear_part_name[1] };

    int* flag_value [types] = { mob->act_flags, mob->affected_by, &mob->wear_part };
    int uses_flag [types] = { 1, 1, 1 };

    if( ( response = flag_handler( title, name1, name2, flag_value, max, uses_flag, ch->can_edit( mob ) ? (const char*) NULL : no_permission, ch, argument, types ) ) != NULL ) 
      mob_log( ch, mob->vnum, response );
    return;
  }

  imm = wizard( pc );

  const char* title [types] = { "Act", "Affect", "Permission" };
  int max [types] = { is_demigod( ch ) ? MAX_PLR : MAX_PLR_OPTION, MAX_ENTRY_AFF_CHAR, MAX_PERMISSION };

  const char** name1 [types] = { &plr_name[0], &aff_char_table[0].name, &permission_name[1] }; 
  const char** name2 [types] = { &plr_name[1], &aff_char_table[1].name, &permission_name[2] };

  int* flag_value [types] = { pc->pcdata->pfile->flags, pc->affected_by, imm == NULL ? NULL : imm->permission };
  int uses_flag [types] = { -1, 1, is_demigod( ch ) ? -1 : 1 };

  flag_handler( title, name1, name2, flag_value, max, uses_flag, "That flag isn't setable or you don't have permission.\r\n", ch, argument, imm == NULL ? 2 : types );

  #undef types

  return;
}


/*
 *   MSET
 */

void do_mset( char_data* bob, char* argument )
{
  char_data*      victim;
  descr_data*      descr;
  pc_data*        pcdata;
  share_data*     shdata;
  species_data*  species;
  player_data*        pc;
  wizard_data*       imm;
  account_data*  account;
  clan_data*        clan;
  const char*       name;

  player_data*        ch = player( bob );
  if( !ch )
    return;

  imm     = (wizard_data*) ch;
  species = imm->mob_edit;
  victim  = imm->player_edit;

  if( species == NULL && victim == NULL ) {
    send( ch, "You aren't editing any mob - use medit <mob>.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    do_mstat( ch, "" );
    return;
  }

  if( !ch->can_edit( species ) )
    return;

  if( species == NULL ) {
    shdata = victim->shdata;
    pcdata = victim->pcdata;
    descr  = victim->descr;
    pc     = player( victim );
    imm    = wizard( victim );
    name   = descr->name;
  } else {
    shdata = species->shdata;
    pcdata = NULL;
    pc     = NULL;
    descr  = species->descr;
    name   = species->Name( );
  } 

  { 
    class int_field int_list[] = {
      { "strength",          0,    30,  &shdata->strength      },
      { "intelligence",      0,    30,  &shdata->intelligence  },
      { "wisdom",            0,    30,  &shdata->wisdom        },
      { "dexterity",         0,    30,  &shdata->dexterity     },
      { "constitution",      0,    30,  &shdata->constitution  },
      { "fame",              0,  1000,  &shdata->fame          },
      { "",                  0,     0,  NULL                   },
      };

    if( process( int_list, ch, name, argument, species, pc ) ) {
      update_maxes( victim );
      return;
    }

#define an( i )      alignment_table[i].name
#define rn( i )      race_table[i].name
#define mea          MAX_ENTRY_ALIGNMENT
#define mer          MAX_ENTRY_RACE
#define dn( i )      mob_diff_table[i].entry_name
#define max_chal     MAX_ENTRY_MOB_DIFF

    class type_field type_list[] = {
      { "alignment",       mea,      &an(0), &an(1),     &shdata->alignment       },
      { "race",            mer,      &rn(0), &rn(1),     &shdata->race            },
      { "lowerchallenge",  max_chal, &dn(0), &dn(1),     &species->low_challenge  },
      { "upperchallenge",  max_chal, &dn(0), &dn(1),     &species->high_challenge },
      { "",          0,     NULL,   NULL,   NULL                }
    };

#undef max_chal
#undef mea
#undef mer
#undef an
#undef rn

    if( process( type_list, ch, name, argument, species, pc ) )
      return;
  }

  class string_field string_list[] = {
    { "keywords",      MEM_DESCR,  &descr->keywords,    NULL },
    { "singular",      MEM_DESCR,  &descr->singular,    NULL },
    { "",              0,          NULL,                NULL },   
  };

  if( process( string_list, ch, name, argument, species ) )
    return;

  if( species != NULL ) {
    class string_field string_list[] = {
      { "name",         MEM_DESCR,  &descr->name,          NULL },
      { "plural",       MEM_DESCR,  &descr->plural,        NULL },
      { "long_s",       MEM_DESCR,  &descr->long_s,        NULL },
      { "long_p",       MEM_DESCR,  &descr->long_p,        NULL },
      { "prefix_s",     MEM_DESCR,  &descr->prefix_s,      NULL },
      { "prefix_p",     MEM_DESCR,  &descr->prefix_p,      NULL },
      { "adj_s",        MEM_DESCR,  &descr->adj_s,         NULL },
      { "adj_p",        MEM_DESCR,  &descr->adj_p,         NULL },
      { "creator",      MEM_DESCR,  &species->creator,     NULL },
      { "no_bash_msg",  MEM_DESCR,  &species->no_bash_msg, NULL },
      { "tame_msg",     MEM_DESCR,  &species->tame_msg,    NULL },
      { "rtame_msg",    MEM_DESCR,  &species->rtame_msg,   NULL },
      { "",             0,          NULL,                  NULL },   
    };

    if( process( string_list, ch, name, argument, species ) )
      return;

    class int_field int_list[] = {
      { "coins",               0,       100000,  &species->gold        },
      { "level",               0,           90,  &shdata->level        },
      { "magic res.",       -200,          100,  &shdata->resist[0]    },
      { "fire res.",        -200,          100,  &shdata->resist[1]    },
      { "cold res.",        -200,          100,  &shdata->resist[2]    },
      { "electricity res.", -200,          100,  &shdata->resist[3]    },
      { "mind res.",        -200,          100,  &shdata->resist[4]    },
      { "acid res.",        -200,          100,  &shdata->resist[5]    },
      { "poison res.",      -200,          100,  &shdata->resist[6]    },
      { "holy res.",        -200,          100,  &shdata->resist[7]    },
      { "maturity",            1,         1000,  &species->maturity    },
      { "adult",               0,  MAX_SPECIES,  &species->adult       },
      { "skeleton",            0,  MAX_SPECIES,  &species->skeleton    },
      { "zombie",              0,  MAX_SPECIES,  &species->zombie      },
      { "celestial",           0,  MAX_SPECIES,  &species->celestial   },
      { "dire",                0,  MAX_SPECIES,  &species->dire        },
      { "fiendish",            0,  MAX_SPECIES,  &species->fiendish    },
      { "corpse",              0,        10000,  &species->corpse      },
      { "price",               1,     10000000,  &species->price       },
      { "wander delay",      200,         4999,  &species->wander      },  
      { "light",               0,          100,  &species->light       },  
      { "compan_amt",         -1,         1000,  &species->compan_amt  },
      { "",                    0,            0,  NULL                  }, 
    };
               
    if( process( int_list, ch, name, argument, species ) )
      return;

    class cent_field cent_list[] = {
      { "weight",              0,     10000000,  &species->weight                     },  
      { "fire.damage",      -10000,        10000,  &shdata->modify_damage[ATT_FIRE]     },
      { "cold.damage",      -10000,        10000,  &shdata->modify_damage[ATT_COLD]     },
      { "acid.damage",      -10000,        10000,  &shdata->modify_damage[ATT_ACID]     },  
      { "shock.damage",     -10000,        10000,  &shdata->modify_damage[ATT_SHOCK]    },
      { "holy.damage",      -10000,        10000,  &shdata->modify_damage[ATT_HOLY]     },
      { "mind.damage",      -10000,        10000,  &shdata->modify_damage[ATT_MIND]     },
      { "magic.damage",     -10000,        10000,  &shdata->modify_damage[ATT_MAGIC]    },
      { "poison.damage",    -10000,        10000,  &shdata->modify_damage[ATT_POISON]   },
      { "unholy.damage",    -10000,        10000,  &shdata->modify_damage[ATT_UNHOLY]   },
      { "physical.damage",  -10000,        10000,  &shdata->modify_damage[ATT_PHYSICAL] },
      { "",                      0,            0,  NULL                                   }, 
    };
               
    if( process( cent_list, ch, name, argument, species ) )
      return;

#define gtn( i )    group_table[i].name
#define nn( i )     nation_table[i].name
#define sn( i )     size_name[i]
#define sxn( i )    sex_name[i]
#define cn( i )     color_fields[i]
#define stn( i )    affect_location[i]

    type_field type_list[] = {
      { "group",         MAX_ENTRY_GROUP,  &gtn(0), &gtn(1), &species->group    },
      { "nation",        MAX_ENTRY_NATION, &nn(0),  &nn(1),  &species->nation   },
      { "sex",           MAX_SEX,          &sxn(0), &sxn(1), &species->sex      },
      { "size",          MAX_SIZE,         &sn(0),  &sn(1),  &species->size     },
      { "color",         MAX_COLOR,        &cn(0),  &cn(1),  &species->color    },
      { "compan_str",    MAX_AFF_LOCATION, &stn(0), &stn(1), &species->compan_str },
      { "",         0,                NULL,    NULL,    NULL               }
    };

#undef stn
#undef ms
#undef gtn
#undef nn
#undef sn
#undef sxn
#undef cn

    if( process( type_list, ch, name, argument, species ) )
      return;

    class dice_field dice_list[] = {
      { "movedice",    LEVEL_MOB,     &species->movedice },
      { "hitdice",     LEVEL_MOB,     &species->hitdice },    
      { "",            -1,            NULL }
    };

    if( process( dice_list, ch, name, argument, species ) )
      return; 
    }

  if( imm != NULL ) {
    class string_field string_list[] = {
      { "immtitle",     MEM_WIZARD, &imm->level_title,     NULL },
      { "",             0,          NULL,                  NULL },   
    };

    if( process( string_list, ch, name, argument, species ) )
      return;

    class int_field int_list [] = {
      { "office",          1,       500,  &imm->office    },
      { "",                0,         0,  NULL               },
    };

    if( process( int_list, ch, name, argument ) )
      return;
  }

  if( pc != NULL ) {
    class string_field string_list[] = {
      { "title",        MEM_PLAYER, &pcdata->title,        NULL },
      { "",             0,          NULL,                  NULL },   
    };

    if( process( string_list, ch, name, argument, species ) )
      return;

#define ctn( i )    clss_table[i].name
#define rn( i )     religion_table[i].name
#define sxn( i )    sex_name[i]
#define max_relig   MAX_ENTRY_RELIGION

    class type_field type_list[] = {
      { "sex",       MAX_SEX-1,   &sxn(0),  &sxn(1),  &pc->sex           },
      { "class",     MAX_CLSS,    &ctn(0),  &ctn(1),  &pcdata->clss      },
      { "religion",  max_relig,   &rn(0),   &rn(1),   &pcdata->religion  },
      { "",          0,           NULL,     NULL,     NULL               },
    };

#undef max_relig
#undef ctn
#undef rn
#undef sxn

    if( process( type_list, ch, name, argument, NULL, pc ) )
      return;

    class int_field int_list[] = {
      { "deaths",          0,      10000,  &shdata->deaths                   },
      { "exp",             0,  532837879,  &victim->exp                      },
      { "gsp_points",  -1000,       1000,  &pc->gossip_pts                   },
      { "piety",           0,       1000,  &pcdata->piety                    },
      { "prayer",          0,       1000,  &pc->prayer                       },
      { "prac_points",  -100,       1000,  &pcdata->practice                 },
      { "hunger",       -100,        100,  &pcdata->condition[COND_FULL]     },
      { "thirst",       -100,        100,  &pcdata->condition[COND_THIRST]   },
      { "drunk",        -100,        100,  &pcdata->condition[COND_DRUNK]    },
      { "alcohol",      -100,        100,  &pcdata->condition[COND_ALCOHOL]  },
      { "move",            0,       1000,  &pc->move                         },
      { "hits",            0,       1000,  &pc->hit                          },
      { "mana",            0,       1000,  &pc->mana                         },
      { "base_move",       0,       1000,  &pc->base_move                    },
      { "base_hits",       0,       1000,  &pc->base_hit                     },
      { "base_mana",       0,       1000,  &pc->base_mana                    },
      { "quest_pts",       0,      10000,  &pcdata->quest_pts                },
      { "bank",            0,  100000000,  &pc->bank                         },
      { "",                0,          0,  NULL                              },
    };

    if( process( int_list, ch, name, argument, NULL, pc ) )
      return;
    
    if( matches( argument, "clan" ) ) {
      if( *argument == '\0' ) {
        send( ch, "Set %s to which clan?\r\n", pc );
        return;
      } else if( matches( argument, "none" ) ) {
        remove_member( pc->pcdata->pfile->clan2, pc );   
        remove_member( pc->pcdata->pfile->clan2, pc->pcdata->pfile );
      } else if( ( clan = find_clan( ch, argument ) ) != NULL ) {
        if( clan->guild ) {
          send( ch, "%s is a guild, not a clan.\r\n", clan->name );
          return;
        }

        remove_member( pc->pcdata->pfile->clan2, pc );   
        remove_member( pc->pcdata->pfile->clan2, pc->pcdata->pfile );
        add_member( clan, pc->pcdata->pfile );
        save_clans( clan );
        if( ch == pc ) {
          send( ch, "You set yourself into clan %s.\r\n", clan->name );
        } else {
          send( ch, "You set %s into clan %s.\r\n", pc, clan->name );
          send( pc, "%s sets you into clan %s.\r\n", ch, clan->name );
        }
      }

      return;
    }

    if( matches( argument, "guild" ) ) {
      if( *argument == '\0' ) {
        send( ch, "Set %s to which guild?\r\n", pc );
        return;
      } else if ( matches( argument, "none" ) ) {
        remove_member( pc->pcdata->pfile->guild, pc );   
        remove_member( pc->pcdata->pfile->guild, pc->pcdata->pfile );
      } else if( ( clan = find_clan( ch, argument ) ) != NULL ) {
        if( !clan->guild ) {
          send( ch, "%s is a clan, not a guild.\r\n", clan->name );
          return;
        }

        remove_member( pc->pcdata->pfile->guild, pc );   
        remove_member( pc->pcdata->pfile->guild, pc->pcdata->pfile );
        add_member( clan, pc->pcdata->pfile );
        save_clans( clan );
        if( ch == pc ) {
          send( ch, "You set yourself into clan %s.\r\n", clan->name );
        } else {
          send( ch, "You set %s into clan %s.\r\n", pc, clan->name );
          send( pc, "%s sets you into clan %s.\r\n", ch, clan->name );
        }
      }

      return;
    }

    if( matches( argument, "account" ) ) {
      if( *argument == '\0' ) {
        send( ch, "To which account do you wish to switch %s.\r\n", pc );
        return;
      }
      if( ( account = find_account( argument ) ) == NULL ) {
        send( ch, "No such account exists.\r\n" );
        return;
      }
      send( ch, "%s switched to account %s.\r\n", pc, account->name );
      pc->pcdata->pfile->account = account;
      return;
    }

    if( is_god( ch ) && matches( argument, "balance" ) ) {
      if( pcdata->pfile->account == NULL ) {
        send( ch, "Null Account.\r\n" );
        return;
      }
      pcdata->pfile->account->balance = atoi( argument );
      send( ch, "Balance set to $%.2f.\r\n", (float) pcdata->pfile->account->balance/100 );
      save_accounts( );
      return;
    }
  }

  send( ch, "Unknown field - see help mset.\r\n" );
}

void do_pstatus( char_data* ch, char* argument )
{
  char_data*       victim;
  wizard_data*        imm;
  char*          status [] = { "Pet", "In_Group", "Familiar", "Leaping", "Follower",
    "Stunned", "Polymorph", "Alert", "Tamed", "Berserk", "Gained_Exp", "Sentinel",
    "Reply Locked", "Aggr.All", "Aggr.Good", "Aggr.Evil", "Ordered", "Hiding",
    "Sneaking", "Wimpy", "Passive", "Forced", "Camouflaged", "Studying", "Aggr.Neut" };
  int                 col = 0;
  bool              found = false;

  imm = wizard( ch );

  if( imm->player_edit == NULL ) {
    send( ch, "You must edit a player before this command will function.\r\n" );
    return;
  }

  victim = imm->player_edit;

  if( !is_angel( ch ) ) {
    send( ch, "You do not have permission for this command.\r\n" );
    return;
  }
  if( *argument == '\0' ) {
    for( int i = 0; i < MAX_STAT; i++ ) {
      if( is_set( &victim->status, i ) ) {
        if( !found ) {
          send_title( ch, "The status flags set on %s are:", ch == victim ? "you" : victim->Seen_Name( ch ) );
          found = true;
        }
        if( col == 5 ) {
          send( ch, "\r\n" );
          col = 0;
        }
        send( ch, "%16s", status[i] );
        col++;
      }
    }
    send( ch, "\r\n" );
    if( !found )
      send( ch, "%s has no status flags set?!?\r\n", victim );
  }
  if( fmatches( argument, "reset" ) ) {
    for( int i = 0; i < MAX_STAT; i++ )
      remove_bit( &victim->status, i );
    send( ch, "All status flags of %s reset.\r\n", victim );
  }
}


void do_mstat( char_data* ch, char* argument )
{
  char                buf  [ MAX_STRING_LENGTH ];
  char_data*       victim;
  descr_data*       descr;
  pc_data*         pcdata;
  share_data*      shdata;
  species_data*   species;
  wizard_data*        imm;
  dice_data          dice;
  player_data*         pc;
  int                   i;
  int               flags;

  imm = wizard( ch );

  if( !get_flags( ch, argument, &flags, "a", "Mstat" ) )
    return;
  
  if( *argument == '\0' ) {
    if( ( species = imm->mob_edit ) == NULL ) {
      if( imm->player_edit == NULL ) {
        send( ch, "Specify victim or use medit to select one.\r\n" );
        return;
      }
    
      victim = imm->player_edit;
      shdata = victim->shdata;
      pcdata = victim->pcdata;
      descr  = victim->descr;
      pc     = player( imm->player_edit );
    
    } else {
      victim = NULL;
      shdata = species->shdata;
      pcdata = NULL;  
      descr  = species->descr;
      pc = NULL;
    }

  } else {
    if( isdigit( *argument ) ) {
      i = atoi( argument );
      if( ( species = get_species( i ) ) == NULL ) {
        send( ch, "There is no species with that number.\r\n" );
        return;
      }

      victim = NULL;
      shdata = species->shdata;
      pcdata = NULL;  
      descr  = species->descr;
      pc = NULL;

    } else {
      if( ( victim = one_character( ch, argument, "mstat", ch->array, (thing_array*) &player_list ) ) == NULL )
        return;

      shdata  = victim->shifted ? victim->shifted->shdata : victim->shdata;
      pcdata  = victim->pcdata;
      species = victim->species;   
      descr   = victim->shifted ? victim->shifted->descr : victim->descr;
      pc  = player( victim );
    }
  }

  sprintf( buf, "        " );

  if( !is_set( &flags, 0 ) ) {
    page_title( ch, victim == NULL ? species->Name( ) : victim->Seen_Name( ch ) );

    if( pc != NULL ) {
      sprintf( buf+5,  "    Class: %s        ", clss_table[ pcdata->clss ].name );
      sprintf( buf+25, "  Exp.Lvl: %d\r\n", exp_for_level( victim ) - victim->exp );
      page( ch, buf );

    } else {
      page( ch, "          Vnum: %-10d Creator: %s%s%s\r\n", species->vnum, color_code( ch, COLOR_BOLD_YELLOW ), species->creator, normal( ch ) );
    }

    if( pc == NULL ) {
      page( ch, "        Nation: %-12s Group: %s\r\n", nation_table[species->nation].name, group_table[species->group].name );
      page( ch, "UpperChallenge: %-13s LowerChallenge: %-13s\r\n", mob_diff_table[ species->high_challenge ].entry_name, mob_diff_table[ species->low_challenge ].entry_name );
    }

    page( ch, "         Level: %-12d Kills: %-11d Deaths: %d\r\n", shdata->level, shdata->kills, shdata->deaths );
    page( ch, "          Fame: %-5d\r\n", shdata->fame );

    page( ch,  "           Sex: %-13s Race: %-8s Alignment: %s\r\n",
      sex_name[ victim == NULL ? species->sex : victim->sex ],
      race_table[ shdata->race ].name, alignment_table[ shdata->alignment ].name );

    if( pc != NULL ) {
      page( ch, "         Piety: %-11d Prayer: %-11d Trust: %d\r\n", pcdata->piety, pc->prayer, pcdata->trust );
      page( ch, "           Age: %-12d Pracs: %-12d Total: %d (%.1lf)\r\n", pc->Age( ), pcdata->practice, (int)total_pracs( victim ), expected_pracs( victim ) );
      page( ch, "      Qst_Pnts: %-9d Gsp_Pnts: %d\r\n", pcdata->quest_pts, pc->gossip_pts );
      page( ch, "          Bank: %-12d Coins: %s\r\n", pc->bank, coin_phrase( victim ) );

    } else {
      page( ch, "         Coins: %-11d Wander: %-12d Light: %d\r\n", species->gold, species->wander, species->light );
      page( ch, "          Size: %-11s Weight: %.2f lbs\r\n", species->size < MAX_SIZE ? species->size >= 0 ? size_name[ species->size ] : "NULL" : "NULL", (float) species->weight/100 );

      dice = species->hitdice;
      sprintf( buf+5,  "  HitDice: %dd%d+%d            ", dice.number, dice.side, dice.plus );
      dice = species->movedice;
      sprintf( buf+27,   " MvDice: %dd%d+%d\r\n", dice.number, dice.side, dice.plus );
      page( ch, buf );

      page( ch, "      Maturity: %-12d Price: %d\r\n", species->maturity, species->price );

      species_data*     adult  = get_species( species->adult );
      species_data*  skeleton  = get_species( species->skeleton );
      species_data*    zombie  = get_species( species->zombie );
      species_data* celestial  = get_species( species->celestial );
      species_data*      dire  = get_species( species->dire );
      species_data*  fiendish  = get_species( species->fiendish );

      obj_clss_data*   corpse  = get_obj_index( species->corpse );

      page( ch, "         Adult: %-5d (%s)\r\n", species->adult, adult == NULL ? "none" : adult->Name( ) );
      page( ch, "      Skeleton: %-5d (%s)\r\n", species->skeleton, skeleton == NULL ? "none" : skeleton->Name( ) );
      page( ch, "        Zombie: %-5d (%s)\r\n", species->zombie, zombie == NULL ? "none" : zombie->Name( ) );
      page( ch, "     Celestial: %-5d (%s)\r\n", species->celestial, celestial == NULL ? "none" : celestial->Name( ) );
      page( ch, "          Dire: %-5d (%s)\r\n", species->dire, dire == NULL ? "none" : dire->Name( ) );
      page( ch, "      Fiendish: %-5d (%s)\r\n", species->fiendish, fiendish == NULL ? "none" : fiendish->Name( ) );

      page( ch, "        Corpse: %-5d (%s)\r\n", species->corpse, corpse == NULL ? "none" : corpse->Name( ) );
     sprintf( buf+5, "\r\n    Companion Strength: %s\r\n", affect_location[ species->compan_str ] );
     page( ch, buf );
     sprintf( buf+5, " Companion Amount: %2d\r\n", species->compan_amt );
     page( ch, buf ); 
    }

    if( victim != NULL ) {
      page( ch, "\r\n" );
      if( victim->species == NULL || is_pet( victim ) )
      sprintf( buf+5,  "  Hit Pts: %d/%d       ", victim->hit, victim->max_hit );
      else
      sprintf( buf+5,  "  Hit Pts: %d/%d       ", current_condition( victim ), victim->original_hit );  
      sprintf( buf+25, "   Energy: %d/%d       ", victim->mana, victim->max_mana );
      sprintf( buf+45, "     Move: %d/%d\r\n", victim->move, victim->max_move );
      page( ch, buf );

      page( ch, "     Hit Regen: %-8d Ene Regen: %-9d Mv Regen: %d\r\n", victim->Hit_Regen( ), victim->Mana_Regen( ), victim->Move_Regen( ) );
      page( ch, "      Wght Inv: %-8.2f Wght Worn: %.2f\r\n", (float) victim->contents.weight/100, (float) victim->wearing.weight/100 );
      page( ch, "        Leader: %s\r\n", victim->leader == NULL ? "no one" : victim->leader->Name( ch ) );

      if( victim->species != NULL ) {
        page( ch, "     Challenge: %s\r\n", mob_diff_table[ victim->challenge ].entry_name );
        page( ch, "         Reset: %s\r\n", name( victim->reset ) );
      }
    }

    if( pc != NULL ) { 
      sprintf( buf+5,  "   Hunger: %d          ", pcdata->condition[COND_FULL] );
      sprintf( buf+25, "   Thirst: %d          ", pcdata->condition[COND_THIRST] );
      sprintf( buf+45, "    Drunk: %d (%d)\r\n", pcdata->condition[ COND_DRUNK ], pcdata->condition[ COND_ALCOHOL ] );
      page( ch, buf );
    }

    page( ch, "\r\n" );  

    if( victim != NULL ) {
      sprintf( buf+5, 
        "Str: %2d(%2d)  Int: %2d(%2d)  Wis: %2d(%2d)  Dex: %2d(%2d) Con: %2d(%2d).\r\n\r\n",
        victim->Strength( ), shdata->strength,
        victim->Intelligence( ), shdata->intelligence,
        victim->Wisdom( ), shdata->wisdom,
        victim->Dexterity( ), shdata->dexterity,
        victim->Constitution( ), shdata->constitution );
      page( ch, buf );

      sprintf( buf+5,
        "Mag: %4d(%4d)  Fir: %4d(%4d)  Col: %4d(%4d)  Min: %4d(%4d)\r\n",
        victim->Save_Magic( ),    shdata->resist[RES_MAGIC],
        victim->Save_Fire( ),     shdata->resist[RES_FIRE],
        victim->Save_Cold( ),     shdata->resist[RES_COLD],
        victim->Save_Mind( ),     shdata->resist[RES_MIND] );   
      page( ch, buf );

      sprintf( buf+5,
        "Ele: %4d(%4d)  Aci: %4d(%4d)  Poi: %4d(%4d)  Hol: %4d(%4d)\r\n\r\n",
        victim->Save_Shock( ),    shdata->resist[RES_SHOCK],
        victim->Save_Acid( ),     shdata->resist[RES_ACID],
        victim->Save_Poison( ),   shdata->resist[RES_POISON],
        victim->Save_Holy( ),     shdata->resist[RES_HOLY] );
      page( ch, buf );

      page( ch, "     Damage Increases/Decreases\r\n\r\n" );

      sprintf( buf+5,
        "Mag: %7.2f(%7.2f)  Fir: %7.2f(%7.2f)  Col: %7.2f(%7.2f)\r\n",
        (float) victim->Modify_Magic( )/100, (float) shdata->modify_damage[ ATT_MAGIC ]/100,
        (float) victim->Modify_Fire( )/100, (float) shdata->modify_damage[ ATT_FIRE ]/100,
        (float) victim->Modify_Cold( )/100, (float) shdata->modify_damage[ ATT_COLD ]/100 );
      page( ch, buf );

      sprintf( buf+5,
        "Ele: %7.2f(%7.2f)  Aci: %7.2f(%7.2f)  Poi: %7.2f(%7.2f)\r\n",
        (float) victim->Modify_Shock( )/100, (float) shdata->modify_damage[ ATT_SHOCK ]/100,
        (float) victim->Modify_Acid( )/100, (float) shdata->modify_damage[ ATT_ACID ]/100,
        (float) victim->Modify_Poison( )/100, (float) shdata->modify_damage[ ATT_POISON ]/100 );
      page( ch, buf );
      
      sprintf( buf+5, "Min: %7.2f(%7.2f)  Hol: %7.2f(%7.2f)  Unh: %7.2f(%7.2f)\r\n", 
        (float) victim->Modify_Mind( )/100, (float) shdata->modify_damage[ ATT_MIND ]/100,
        (float) victim->Modify_Holy( )/100, (float) shdata->modify_damage[ ATT_HOLY ]/100,
        (float) victim->Modify_Unholy( )/100, (float) shdata->modify_damage[ ATT_UNHOLY ]/100 );
      page( ch, buf );

      sprintf( buf+5, "Phy: %7.2f(%7.2f)\r\n\r\n", 
        (float) victim->Modify_Physical( )/100, (float) shdata->modify_damage[ ATT_PHYSICAL ]/100 );
    } else {
      sprintf( buf+5,
        "Str: %2d  Int: %2d  Wis: %2d  Dex: %2d  Con: %2d\r\n\r\n",
        shdata->strength, shdata->intelligence, shdata->wisdom,
        shdata->dexterity, shdata->constitution );
      page( ch, buf );

      sprintf( buf+5,
        "Mag: %4d  Fir: %4d  Col: %4d  Min: %4d\r\n",
        shdata->resist[RES_MAGIC], shdata->resist[RES_FIRE],
        shdata->resist[RES_COLD],  shdata->resist[RES_MIND] );
      page( ch, buf );

      sprintf( buf+5,
        "Ele: %4d  Aci: %4d  Poi: %4d  Hol: %4d\r\n\r\n",
        shdata->resist[RES_SHOCK], shdata->resist[RES_ACID],
        shdata->resist[RES_POISON], shdata->resist[RES_HOLY] );
      page( ch, buf );

      page( ch, "     Damage Increases/Decreases\r\n\r\n" );
      sprintf( buf+5,
        "Mag: %7.2f  Fir: %7.2f  Col: %7.2f\r\n",
        (float) shdata->modify_damage[ ATT_MAGIC ]/100, (float) shdata->modify_damage[ ATT_FIRE ]/100,
        (float) shdata->modify_damage[ ATT_COLD ]/100 );
      page( ch, buf );

      sprintf( buf+5,
        "Ele: %7.2f  Aci: %7.2f  Poi: %7.2f\r\n",
        (float) shdata->modify_damage[ ATT_SHOCK ]/100, (float) shdata->modify_damage[ ATT_ACID ]/100,
        (float) shdata->modify_damage[ ATT_POISON ]/100 ); 
      page( ch, buf );

      sprintf( buf+5, "Min: %7.2f  Hol: %7.2f  Unh: %7.2f\r\n", 
        (float) shdata->modify_damage[ ATT_MIND ]/100, 
        (float) shdata->modify_damage[ ATT_HOLY ]/100,
        (float) shdata->modify_damage[ ATT_UNHOLY ]/100 );
      page( ch, buf );

      sprintf( buf+5, "Phy: %7.2f\r\n\r\n",
        (float) shdata->modify_damage[ ATT_PHYSICAL ]/100 );
    }
    page( ch, buf );
  }

  page( ch, "       Name: %s\r\n", descr->name );
  page( ch, "   Singular: %s\r\n", descr->singular );

  if( pc != NULL ) {
    if( pcdata->tmp_short != NULL ) 
      page( ch, " Unapproved: %s\r\n", pcdata->tmp_short );
    page( ch, "   Keywords: %s\r\n", descr->keywords );
    if( pcdata->tmp_keywords != NULL )
      page( ch, " Unapproved: %s\r\n", pcdata->tmp_keywords );

  } else {
    page( ch, "   Prefix_S: %s\r\n", descr->prefix_s );
    page( ch, "      Adj_S: %s\r\n", descr->adj_s );
    page( ch, "     Long_S: %s\r\n", descr->long_s );
    page( ch, "     Plural: %s\r\n", descr->plural );
    page( ch, "   Prefix_P: %s\r\n", descr->prefix_p );
    page( ch, "      Adj_P: %s\r\n", descr->adj_p );
    page( ch, "     Long_P: %s\r\n", descr->long_p );
    page( ch, "   Keywords: %s\r\n", descr->keywords );
    page( ch, "      Color: %s\r\n", color_fields[ species->color ] );
    page( ch, "       Tame: %s\r\n", species->tame_msg );
    page( ch, "      RTame: %s\r\n", species->rtame_msg );
    page( ch, "    No_bash: %s\r\n", species->no_bash_msg );
  }

  sprintf( buf, "\r\nDescription: \r\n%s", *descr->complete != '\0' ? descr->complete : "(none).\r\n" );
  page( ch, buf );
}


void medit_renumber( char_data* ch, int old_vnum, int new_vnum )
{
  reset_data*         reset;
  species_data*     species;
  mprog_data*         mprog;
  obj_clss_data*   obj_clss;
  oprog_data*         oprog;
  room_data*           room;
  action_data*       action;
  selt_string temp;
  int i, j;
  
  int found = 0;
  species_data* mob = get_species( old_vnum );

  // search mobs
  for( i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL ) 
      continue;

    for( reset = species->reset; reset != NULL; reset = reset->next ) {
      if( reset->target == mob )
        found++;
    }

    if( species->adult == old_vnum ) {
      species->adult = new_vnum; 
      found++;
    }

    if( species->zombie == old_vnum ) {
      species->zombie = new_vnum;
      found++;
    }
    
    if( species->skeleton == old_vnum ) {
      species->skeleton = new_vnum;
      found++;
    }

    if( search_progs( species->attack->binary, old_vnum, CHARACTER ) ) {
      temp._printf( "// mob %d needs references changed to %d\r\n", old_vnum, new_vnum );
      temp += species->attack->code;
      free_string( species->attack->code, MEM_MPROG );
      species->attack->code = alloc_string( temp.get_text( ), MEM_MPROG );
      
      page( ch, "  Used in attack prog on mob #%d.\r\n", i );
      found++;
    }

    for( j = 1, mprog = species->mprog; mprog != NULL; j++, mprog = mprog->next ) {
      if( search_progs( mprog->binary, old_vnum, CHARACTER ) ) {
        temp._printf( "// mob %d needs references changed to %d\r\n", old_vnum, new_vnum );
        temp += mprog->code;
        free_string( mprog->code, MEM_MPROG );
        mprog->code = alloc_string( temp.get_text( ), MEM_MPROG );

        page( ch, "  Used in mprog #%d on mob #%d.\r\n", j, i );
        found++;
      }
    }
  }

  // search objs
  for( i = 0; i < MAX_OBJ_INDEX; i++ ) {
    if( ( obj_clss = obj_index_list[i] ) == NULL )
      continue;

    for( j = 1, oprog = obj_clss->oprog; oprog != NULL; j++, oprog = oprog->next ) {
      if( search_progs( oprog->binary, old_vnum, CHARACTER ) ) {
        temp._printf( "// mob %d needs references changed to %d\r\n", old_vnum, new_vnum );
        temp += oprog->code;
        free_string( oprog->code, MEM_OPROG );
        oprog->code = alloc_string( temp.get_text( ), MEM_OPROG );
        
        page( ch, "  Used in oprog #%d on object #%d.\r\n", j, i );
        found++;
      }
    }
  }

  // search rooms
  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) ) {
    for( room = area->room_first; room != NULL; room = room->next ) {
      for( reset = room->reset; reset != NULL; reset = reset->next ) {
        if( reset->target == mob )
          found++;
      }

      for( j = 1, action = room->action; action != NULL; j++, action = action->next ) {
        if( action->binary != NULL && search_progs( action->binary, old_vnum, CHARACTER ) ) {
          temp._printf( "// mob %d needs references changed to %d\r\n", old_vnum, new_vnum );
          temp += action->code;
          free_string( action->code, MEM_ACTION );
          action->code = alloc_string( temp.get_text( ), MEM_ACTION );

          page( ch, "  Used in action #%d in room #%d.\r\n", j, room->vnum );
          found++;
        }
      }
    }
  }

  // search rtables
  for( int i = 0; i < rtable_list; i++ ) {
    for( reset = rtable_list[i]->reset; reset != NULL; reset = reset->next ) {
      if( reset->target == mob )
        found++;
    }
  }

  // search trainers
  for( trainer_data* trainer = trainer_list; trainer != NULL; trainer = trainer->next ) {
    if( trainer->trainer == old_vnum ) {
      trainer->trainer = new_vnum;
      found++;
    }
  }

  // search lists
  for( i = 0; i < MAX_LIST; i++ ) {
    if( !str_cmp( list_entry[ i ][ 2 ], "M" ) ) {
      for( j = 0; j < atoi( list_entry[ i ][ 0 ] ); j++ ) {
        if( list_value[ i ][ j ] == old_vnum ) {
          list_value[ i ][ j ] = new_vnum;
          found++;
        }
      }
    }
  }

  mob->vnum = new_vnum;
  species_list[ new_vnum ] = mob;
  species_list[ old_vnum ] = NULL;

  send( ch, "Mob #%d renumbered to #%d in %d places.\r\nPlease 'write -f all' and fix all scripts as listed above.\r\n", old_vnum, new_vnum, found );
}











