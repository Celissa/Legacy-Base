#include "system.h"

bool   can_study   ( char_data*, int );


const char* skill_cat_name [ MAX_SKILL_CAT ] = { "weapon", "language", "spell", "physical", "trade" };

/*
 *   CURRENTLY UNUSED
 */


char* abil_name( int i )
{
  if( i <  0 ) return " (unknown) ";
  if( i <  1 ) return "(appalling)";
  if( i <  5 ) return "(dreadful) ";
  if( i < 10 ) return "  (awful)  ";
  if( i < 20 ) return "   (bad)   ";
  if( i < 30 ) return "  (poor)   ";
  if( i < 50 ) return "  (fair)   ";
  if( i < 60 ) return " (capable) ";
  if( i < 70 ) return "  (good)   ";
  if( i < 80 ) return "(very good)";
  if( i < 90 ) return " (superb)  ";
               return " (wicked)  ";
}


/*
 *   ABILITY ROUTINE
 */

void do_skills( char_data* ch, char* )
{
  send( ch, "For information on how to get a skill list type help abilities.\r\n" );

  return;
}


void do_abilities( char_data* ch, char* argument )
{
  char        buf  [ MAX_INPUT_LENGTH ];
  char        arg  [ MAX_INPUT_LENGTH ];
  int        i, j;
  int    ps1, ps2;
  int        cost;
  int      length;
  int        clss;
  int       flags;
  bool      found  = FALSE;
  bool    by_level = FALSE;
  bool    show_all = FALSE;

  if( not_player( ch ) )
    return;

  // handle switched
  player_data* pc = player( ch );
  if( pc == NULL ) {
    if( ch->link != NULL && ch->link->player != ch )
      pc = ch->link->player;
    else {
      is_confused_pet( ch );
      return;
    }
  }

  if( !get_flags( ch, argument, &flags, "la", "Abilities" ) )
    return;

  if( is_set( &flags, 0 ) )
    by_level = TRUE;
  if( is_set( &flags, 1 ) )
    show_all = TRUE;

  if( *argument == '\0' ) {
    send( ch, "Syntax: abil <cat> (optional class)\r\n" );
    send( ch, "Where <cat> is one of Physical, Weapon, Spell, Language, or Trade.\r\n" );
    send( ch, "See help abilities for more detail.\r\n" );
    return;
  }

  clss = pc->pcdata->clss;
  argument = one_argument( argument, arg );
  if( *argument != '\0' ) {
    // scan for class
    length = strlen( argument );
    for( clss = 0; clss < MAX_CLSS; clss++ ) 
      if( ( clss_table[clss].open || ( !clss_table[clss].open && pc->pcdata->clss == clss ) || is_apprentice( pc ) ) 
        && !strncasecmp( argument, clss_table[clss].name, length ) ) 
        break;

    if( clss == MAX_CLSS ) {
      send( ch, "Unknown class.\r\n" );
      return;
    }

    show_all = TRUE;
  }

  // now print out skeelz
  length = strlen( arg );

  for( j = 0; j < MAX_SKILL_CAT; j++ ) {
    if( !strncasecmp( skill_cat_name[j], arg, length ) ) {
      skill_array temp;
      for( i = 0; i < MAX_SKILL; i++ ) {
        if( skill_table[i].category != j || ( cost = skill_table[i].prac_cost[clss] ) == -1 || ( !show_all && skill_table[i].level[clss] > pc->shdata->level ) )
          continue;

        // insert skill into temp table
        bool inserted = false;
        if( by_level ) {
          for( int k = 0; k < temp; k++ ) {
            if( temp[ k ]->level[ clss ] > skill_table[ i ].level[ clss ] ) {
              insert( temp, &skill_table[ i ], k );
              inserted = true;
              break;
            }
          }
        }

        if( !inserted )
          temp.add( &skill_table[ i ] );

        if( !found ) {
          if( show_all )
            page_underlined( ch, "Skill                   Pracs  Level     Prerequisites\r\n" );
          else
            page_underlined( ch, "Skill                 Level       Cost   Pracs    Prerequisites\r\n" );

          found = TRUE;
        }
      }
    
      if( !found ) {
        if( show_all )
          send( ch, "The %s class has no %s skills.\r\n", clss_table[ clss ].name, skill_cat_name[ j ] ); 
        else
          send( ch, "You have no %s skills.\r\n", skill_cat_name[ j ] );
      } else {
        for( int k = 0; k < temp; k++ ) {
          // loop thru skeelz and print it up
          cost = temp[ k ]->prac_cost[ clss ];
          if( show_all ) {
            if( temp[ k ]->level[ clss ] >= LEVEL_APPRENTICE ) {
              sprintf( buf, "%-24s<Unfinished>%5s", temp[ k ]->name, "" );
            } else {
              sprintf( buf, "%-22s%5d%7d%7s", temp[ k ]->name, cost, temp[ k ]->level[ clss ], "" );
            }
          } else {
            // find the original position of the skeel in the table for purposes of the pc->pcdata->skill[i]
            for( i = 0; i < MAX_SKILL; i++ )
              if( &skill_table[i] == temp[k] )
                break;
            
            if( pc->real_skill( i ) != pc->get_skill( i ) ) {
              if( pc->real_skill( i ) == UNLEARNT )
                sprintf( arg, "%2d (unk)", pc->get_skill( i ) );
              else
                sprintf( arg, "%2d (%d)", pc->get_skill( i ), pc->real_skill( i ) );
            } else {
              if( pc->real_skill( i ) == UNLEARNT )
                sprintf( arg, "unk" );
              else
                sprintf( arg, "%2d", pc->real_skill( i ) );
            }

            sprintf( buf, "%-22s%-8s", temp[k]->name, arg );

            if( pc->real_skill( i ) < 10 )
              sprintf( buf+strlen(buf), "%8d%6d%6s", pc->shdata->level*cost*cost+add_tax( ch->in_room, pc->shdata->level*cost*cost, false ), cost, "" );
            else
              sprintf( buf+strlen(buf), "%20s", "" );
          }
          
          ps1 = temp[ k ]->pre_skill[ 0 ];
          ps2 = temp[ k ]->pre_skill[ 1 ];
        
          if( ps1 != SKILL_NONE )
            sprintf( buf + strlen( buf ), "%s [%d]", skill_table[ ps1 ].name, temp[ k ]->pre_level[ 0 ] );
        
          if( ps2 != SKILL_NONE ) 
            sprintf( buf + strlen( buf ), " & %s [%d]", skill_table[ ps2 ].name, temp[ k ]->pre_level[ 1 ] );
        
          sprintf( buf + strlen( buf ), "\r\n" );
          page( ch, buf );
        }
      }
      return;
    }
  }

  send( ch, "Unknown skill category.\r\n" );
  return;
}

/*
 *   PRACTICE ROUTINES
 */


void do_practice( char_data* ch, char* argument )
{ 
  char                 buf  [ MAX_STRING_LENGTH ];
  char                 arg  [ MAX_STRING_LENGTH ];
  mob_data*        trainer  = NULL;
  trainer_data*   pTrainer  = NULL;
  int                    i;
  int                pracs;
  mprog_data*     mprog;
  bool           worked = true;

  if( ch->species != NULL ) {
    send( ch, "Only players can practice skills.\r\n" );
    return;
  }
  
  for( i = 0; ; i++ ) {
    if( i == *ch->array ) {
      send( ch, "There is no one here who can train you.\r\n" );
      return;
    }
    if( ( trainer = mob( ch->array->list[i] ) ) != NULL
      && ( pTrainer = trainer->pTrainer ) != NULL
      && trainer->Seen( ch ) )
      break;
    }

  if( is_set( trainer->affected_by, AFF_SLEEP ) ) {
    send( ch, "%s is sleeping currently and unable to help you.\r\n", trainer->Name( ) );
    return;
  }

  if( trainer->fighting == ch ) {
    send( ch, "You expect me to train you, fool, I shall destroy you.\r\n" );
    return;
  }

  *buf = '\0';

  if( *argument == '\0' ) {
    for( i = 0; i < MAX_SKILL; i++ ) {
      if( !is_set( pTrainer->skill, i ) || skill_table[i].prac_cost[ch->pcdata->clss] == -1 ) 
        continue;

      if( *buf == '\0' ) {
        sprintf( buf, "Practice Pnts: %d    Copper Pieces: %d\r\n\r\n", ch->pcdata->practice, get_money( ch ) );
        sprintf( buf+strlen( buf ), "Skill                    Level       Cost       Prac\r\n" );
        sprintf( buf+strlen( buf ), "-----                    -----       ----       ----\r\n" );
      }

      pracs = skill_table[i].prac_cost[ch->pcdata->clss];
      if( ch->real_skill( i ) == UNLEARNT )
        sprintf( arg, "%s", "unk" );
      else
        sprintf( arg, "%d ", ch->real_skill( i ) );
   
      sprintf( buf+strlen( buf ), "%-22s%7s", skill_table[i].name, arg );

      if( ch->real_skill( i ) >= MAX_SKILL_LEVEL ) 
        strcat( buf, "\r\n" ); 
      else {
        int cost = ch->shdata->level*pracs*pracs;
        int tax  = add_tax( ch->in_room, cost, false );
        cost += tax;
        sprintf( buf+strlen( buf ), "%12d%10d\r\n", cost, pracs );
      }
    }

    if( *buf == '\0' ) 
      process_ic_tell( trainer, ch, "Sorry, there is nothing I can teach you.", "say" );
    else
      send( ch, buf );
    return;
  }
  
  for( i = 0; i < MAX_SKILL; i++ ) {
    if( !is_set( pTrainer->skill, i ) || skill_table[i].prac_cost[ch->pcdata->clss] == -1 || !fmatches( argument, skill_table[i].name ) )
      continue;

    if( skill_table[ i ].alignment != 0 ) {
      if( !is_set( &skill_table[i].alignment, ch->shdata->alignment ) ) {
        fsend( ch, "You thought you could deceive me, but I can see right through you.  Your intentions I disagree with and will not teach what you will use wrongly.\r\n" );
        return;
      }
    }
      
    if( ch->real_skill( i ) >= 7 ) {
      fsend( ch, "You know %s as well as you can be taught it.  It can only be improved by practice now.\r\n", skill_table[i].name );
      return;
    }   

    pracs = skill_table[i].prac_cost[ch->pcdata->clss];

    for( mprog = trainer->species->mprog; mprog != NULL; mprog = mprog->next ) {
      if( mprog->trigger == MPROG_TRIGGER_PRACTICE && ( is_name( argument, mprog->string ) || *mprog->string == '\0' ) ) {
        var_ch   = ch;
        var_mob  = trainer;
        var_room = trainer->in_room;
        worked = execute( mprog );
        if( !worked )
          return;
      }
    }

    if( !can_study( ch, i ) )
      return;

    if( ch->pcdata->practice < pracs ) {
      fsend( ch, "%s tries to explain %s to you, but after a short time tells you, you need to go out and practice your current skills more before you can learn new ones.\r\n", trainer, skill_table[i].name ); 
      return;
    }

    int cost = ch->shdata->level*pracs*pracs;
    int tax  = add_tax( ch->in_room, cost, false );

    sprintf( buf, "You hand %s", trainer->Name( ch ) );
    if( !remove_coins( ch, cost, buf ) ) {
      process_ic_tell( trainer, ch, "You are unable to afford my services.", "say" );
      return;
    }

    add_tax( ch->in_room, cost-tax, true );

    ch->pcdata->practice -= pracs;
    send( ch, "%s teaches you %s%s.\r\n", trainer, skill_table[i].name, ch->real_skill( i ) == UNLEARNT ? "" : ", improving your ability at it" );
    ch->pcdata->skill[i]++;
    send_seen( ch, "%s practices %s.\r\n", ch, skill_table[i].name );
    return;
  }

  process_ic_tell( trainer, ch, "I know of no such skill.", "say" );
}


bool can_study( char_data* ch, int i )
{
  char           tmp  [ TWO_LINES ];
  skill_type*  skill  = &skill_table[i];
  int             n1  = skill->pre_skill[0];
  int             n2  = skill->pre_skill[1];
  skill_type*    ps1;
  skill_type*    ps2;

  ps1 = ( n1 == SKILL_NONE ? NULL : &skill_table[n1] );
  ps2 = ( n2 == SKILL_NONE ? NULL : &skill_table[n2] );

  if( ( ps1 != NULL && ch->real_skill( n1 ) < skill->pre_level[0] ) 
    || ( ps2 != NULL && ch->real_skill( n2 ) < skill->pre_level[1] ) ) {
    sprintf( tmp, "Before you are prepared to study %s you need to know ", skill->name );
    if( ps2 == NULL )
      sprintf( tmp+strlen( tmp ), "%s at level %d.", ps1->name, skill->pre_level[0] );
    else
      sprintf( tmp+strlen( tmp ), "%s at level %d and %s at level %d.", ps1->name, skill->pre_level[0], ps2->name, skill->pre_level[1] );
    fsend( ch, tmp );
    return FALSE;
  }

  if( ch->shdata->level < skill->level[ ch->pcdata->clss ] ) {
    send( ch, "To study %s you must be at least level %d.\r\n", skill->name, skill->level[ ch->pcdata->clss ] );
    return FALSE;
  }  

  return TRUE;
}


/*
 *   PRACTICE POINTS
 */


double total_pracs( char_data* ch )
{
  int            clss  = ch->pcdata->clss;
  double        total  = 0;
  double         cost  = 0;
  int           skill;
  register int      i;

  for( i = 0; i < MAX_SKILL; i++ ) 
    if( ( skill = ch->real_skill( i ) ) != 0 && ( cost = skill_table[i].prac_cost[clss] ) != -1 ) 
      total += cost*skill;

  total -= 20*skill_table[LANG_COMMON].prac_cost[clss];
  total += ch->pcdata->practice;

  return total;
}     


void char_data :: improve_skill( int i )
{
  double  total, norm;
  int      difficulty;
  int  j = 0;
  int  k = 0;
  bool          spell = FALSE;

  if( !Is_Valid( ) || species != NULL || i < 0 || i > MAX_SKILL || real_skill( i ) == UNLEARNT )
    return;

  if( pcdata->trust > LEVEL_AVATAR && shdata->level > LEVEL_AVATAR )
    return;
/*
  if( i < SPELL_FIRST || i >= WEAPON_FIRST ) {
    if( pcdata->prac_timer < 0 && ++pcdata->prac_timer != -1 ) 
      return;
    if( pcdata->prac_timer == 0 ) {
      pcdata->prac_timer = number_range( -20, -2 );
      return;
    }
  }
*/
  if( real_skill( i ) != MAX_SKILL_LEVEL &&
    get_skill_usage( i ) < MAX_SKILL_USAGE )
      pcdata->skill_usage[i]++;
  
  if( pcdata->prac_timer > 0 )
    return;

  if( number_range( 0, skill_table[i].improve_difficulty ) > 0 )
    return;

  norm  = expected_pracs( this );
  total = total_pracs( this );

  if( ( shdata->level >= 5 && norm-total <= 0 ) 
    || ( shdata->level < 5 && norm-total+(20-4*shdata->level) < 0 ) )
    return;

//  pcdata->prac_timer = 10;
  if( i >= SPELL_FIRST && i < WEAPON_FIRST )
    spell = TRUE;

  difficulty = ( spell ? real_skill( i )*real_skill( i ) : 2*real_skill( i )*real_skill( i ) )-get_skill_usage( i );
  
  k = number_range( 0, 100+difficulty );
//  if( number_range( 0, 100+difficulty ) < Wisdom( ) && get_skill( i ) != 10 ) {
  if( k < Wisdom( ) && real_skill( i ) != MAX_SKILL_LEVEL ) {
    pcdata->skill[i]++;
    pcdata->skill_usage[i] = 0;
    send( this, "\r\n**  You improve at %s. **\r\n\r\n", skill_table[i].name );
    pcdata->prac_timer = 30;
  }
  else if( pcdata->practice < 40+2*shdata->level ) {
    j = number_range( 0, 100+10*skill_table[i].improve_difficulty );
//    if( number_range( 0, 99+skill_table[i].improve_difficulty ) < ( Intelligence( )+Wisdom( ) )/2 ) {
    if( j < ( Intelligence( )+Wisdom( ) )/2  ) {
      pcdata->practice++;
      send( this, "\r\n** You gain a practice point from %s. **\r\n\r\n", skill_table[i].name );
      pcdata->prac_timer = 15;
    }
  }
/*
  if( pcdata->trust > 0 )
  send( this, "\r\n<Debug> You have used %s %i times, the roll was %i/%i\r\n", skill_table[i].name, get_skill_usage( i ), k, j );

  if( number_range( 0, 160 ) < ( Intelligence( )+Wisdom( ) )
    && number_range( 0, 250 ) > shdata->level
    && ( total-norm < ( 1.5*shdata->level ) ||
    number_range( 0, (shdata->level)+4 ) == 1 ) ) { 
    if( get_skill( i ) == 10 || number_range( 0, 1 ) != 0 ) {
      pcdata->practice++;
      send( this, "\r\n** You gain a practice point from %s. **\r\n\r\n", skill_table[i].name );
    } else {
      pcdata->skill[skill_table[i].skill_location][skill_table[i].category]++;
      send( this, "\r\n** You improve at %s. **\r\n\r\n", skill_table[i].name );
    }
  }
*/
  return;
}


