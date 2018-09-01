#include "system.h"


/*
 *   LOCAL FUNCTIONS
 */


void display_last   ( pfile_data* pfile, char_data* ch );


/*
 *   LAST ROUTINES 
 */


void display_last( pfile_data* pfile, char_data* ch )
{
  player_data *victim = find_player( pfile );
  wizard_data* imm = wizard( victim );
  if( imm != NULL && is_set( victim->pcdata->pfile->flags, PLR_WIZINVIS ) && ch->shdata->level < imm->wizinvis )
    victim = NULL;

  if( victim ) {
    char *tmp1 = static_string( );
    char *tmp2 = static_string( );
    sprintf_minutes( tmp1, time( 0 ) - victim->logon );
    sprintf_minutes( tmp2, time( 0 ) - victim->timer );

    if( victim->link || ( victim->switched && victim->switched->link ) ||
      ( victim->shape_changed && victim->shape_changed->link ) ) {
      if( is_angel( ch ) ) {
        page( ch, "%s [%s] has been on for %s and idle for %s.\r\n", pfile->name, pfile->last_host, tmp1, tmp2 );
      } else {
        page( ch, "%s has been on for %s and idle for %s.\r\n", pfile->name, tmp1, tmp2 );
      }
    } else {
      if( is_angel( ch ) ) {
        page( ch, "%s [%s] has been on for %s and link dead for %s.\r\n", pfile->name, pfile->last_host, tmp1, tmp2 );
      } else {
        page( ch, "%s has been on for %s and link dead for %s.\r\n", pfile->name, tmp1, tmp2 );
      }
    }
  } else {
    char *tmp1 = static_string( );
    sprintf_minutes( tmp1, time( 0 ) - pfile->last_on );

    if( is_angel( ch ) ) {
      page( ch, "%s [%s] was last on at %s (%s ago).\r\n", pfile->name, pfile->last_host, ltime( pfile->last_on ), tmp1 );
    } else {
      page( ch, "%s was last on at %s (%s ago).\r\n", pfile->name, ltime( pfile->last_on ), tmp1 );
    }
  }
}


void do_last( char_data *ch, char *argument ) 
{
  int          found = FALSE;
  int            pos;
  int         length;

  if( is_mob( ch ) )
    return;

  if( *argument == '\0' ) {
    send( "Specify first few letters of name to search for.\r\n", ch );
    return;
  }

  if( fmatches( argument, "all", 2 ) ) {
    found = TRUE;
    for( pos = 0; pos < max_pfile; pos++ ) 
      display_last( pfile_list[ pos ], ch );
  
  } else if( fmatches( argument, "avatar", 2 ) ) {
    for( pos = 0; pos < max_pfile; pos++ )
      if( pfile_list[ pos ]->level < LEVEL_AVATAR && pfile_list[ pos ]->trust == LEVEL_AVATAR ) {
        found = TRUE;
        display_last( pfile_list[ pos ], ch );
      }  
  
  } else if( fmatches( argument, "clan", 2 ) ) {
    clan_data *clan = ch->pcdata->pfile->clan2;
    if( clan == NULL ) {
      send( ch, "You aren't in a clan.\r\n" );
      return;
    }

    for( pos = 0; pos < max_pfile; pos++ ) 
      if( pfile_list[ pos ]->clan2 == clan ) {
        found = TRUE;
        display_last( pfile_list[ pos ], ch );
      }  

  } else if( fmatches( argument, "guild", 2 ) ) {
    clan_data *clan = ch->pcdata->pfile->guild;
    if( clan == NULL ) {
      send( ch, "You aren't in a guild.\r\n" );
      return;
    }

    for( pos = 0; pos < max_pfile; pos++ ) 
      if( pfile_list[ pos ]->guild == clan ) {
        found = TRUE;
        display_last( pfile_list[ pos ], ch );
      }  

  } else if( fmatches( argument, "befriended", 2 ) || fmatches( argument, "friends", 2 ) ) {
    recognize_data*  recognize  = ch->pcdata->recognize;
    pfile_data*          pfile;

    if( recognize == NULL ) {
      send( "You have befriended no one.\r\n", ch );
      return;
    }

    for( pos = 0; pos < recognize->size; pos++ ) {
      if( ( pfile = get_pfile( recognize->list[ pos ] & 0xffff ) ) == NULL || !is_set( &recognize->list[ pos ], RECOG_BEFRIEND ) )
        continue;

      found = TRUE;
      display_last( pfile, ch );
    }  

  } else if( fmatches( argument, "immortal", 2 ) ) {
    for( pos = 0; pos < max_pfile; pos++ ) 
      if( pfile_list[ pos ]->level >= LEVEL_APPRENTICE ) {
        found = TRUE;
        display_last( pfile_list[ pos ], ch );
      }  
  
  } else if( fmatches( argument, "recent", 2 ) ) {
    int cutoff_time = time( 0 ) - 60*60*6; // 6 hours
    for( pos = 0; pos < max_pfile; pos++ )
      if( pfile_list[ pos ]->last_on > cutoff_time ) {
        found = TRUE;
        display_last( pfile_list[ pos ], ch );
      }  

  } else {
    if( ( pos = pntr_search( pfile_list, max_pfile, argument ) ) < 0 )
      pos = -pos - 1;
    length = strlen( argument );
    for( ; pos < max_pfile; pos++ ) {
      if( strncasecmp( pfile_list[ pos ]->name, argument, length ) )
        break;
      found = TRUE;
      display_last( pfile_list[ pos ], ch );
    }
  }
  
  if( !found ) 
    send( ch, "No matches found.\r\n" );
}


void do_whois( char_data* ch, char* argument )
{
  pfile_data*  pfile;
  bool         email;
  wizard_data*   imm  = wizard( ch );

  if( not_player( ch ) )
    return;

  if( *argument == '\0' ) {
    send( ch, "Specify full name of character.\r\nTo search for a name use last <first letter of name>.\r\n" );
    return;
  }

  if( ( pfile = find_pfile_exact( argument ) ) == NULL || pfile->level == 0 ) {
    send( ch, "No character by that name exists.\r\n" );
    return;
  }

  bool see_private = ( is_demigod( ch ) || ch->pcdata->pfile == pfile );
  bool see_account = ( imm != NULL && imm->See_Account( pfile ) );

  send( ch, scroll_line[1] );
  send( ch, "\r\n" );
 
  send( ch, "        Name: %s\r\n", pfile->name );
  send( ch, "        Race: %s\r\n", race_table[ pfile->race ].name );

  if( !is_incognito( pfile, ch ) ) {
    send( ch, "       Class: %s\r\n", clss_table[ pfile->clss ].name );
    send( ch, "         Sex: %s\r\n", sex_name[ pfile->sex] );

    if( pfile->level < LEVEL_APPRENTICE )
      send( ch, "       Level: %d  [ Rank %d%s ]\r\n", pfile->level, pfile->rank+1, number_suffix( pfile->rank+1 ) );
    else
      send( ch, "       Level: %s\r\n", imm_title[ pfile->level-LEVEL_AVATAR ] );
  }

  if( is_demigod( ch ) && pfile->trust > pfile->level )
    send( ch, "       Trust: %d\r\n", pfile->trust );

  if( pfile->level < LEVEL_APPRENTICE )
    send( ch, "      Bounty: %d\r\n", pfile->bounty );

  send( ch, "        Clan: %s\r\n", ( pfile->clan2 == NULL || !knows_members( player( ch ), pfile->clan2 ) ) ? "none" : pfile->clan2->name );
  send( ch, "       Guild: %s\r\n", ( pfile->guild == NULL || !knows_members( player( ch ), pfile->guild ) ) ? "none" : pfile->guild->name );

  send( ch, "\r\n" );
  send( ch, scroll_line[1] );
  send( ch, "\r\n" );

  send( ch, "  Last Login: %s\r\n", ltime( pfile->last_on ) );
  send( ch, "     Created: %s\r\n", ltime( pfile->created ) );

  if( is_demigod( ch ) ) 
    send( ch, "    Password: %s\r\n", pfile->pwd );

  if( see_account || see_private ) {
    send( ch, "        Site: %s\r\n", pfile->last_host );
    send( ch, "     Account: %s\r\n", pfile->account == NULL ? "none" : pfile->account->name );
    if( is_demigod( ch ) && pfile->account != NULL )
      send( ch, "  Acnt. Pswd: %s\r\n", pfile->account->pwd );
  }

  if( pfile->account != NULL && ((email = is_set(pfile->flags, PLR_EMAIL_PUBLIC)) || see_private)) 
    send( ch, "       Email: %s%s\r\n", pfile->account->email, email ? "" : "  (Hidden)");

  send( ch, "    Homepage: %s\r\n", pfile->homepage == empty_string ? "none" : pfile->homepage );

  if( see_private ) {
    send( ch, "\r\n" );
    send( ch, scroll_line[1] );
    send( ch, "\r\n" );
    send( ch, "     Balance: $%.2f\r\n", pfile->account == NULL ? 0. : (float)pfile->account->balance/100 );
  }
 
  send( ch, "\r\n" );
  send( scroll_line[1], ch );
}


/*
 *   WHO ROUTINES
 */

void do_qwho( char_data* ch, char* argument )
{
  char          tmp  [ ONE_LINE ];
  int             i  = 0;
  int         count  = 0;
  int         flags;

  if( not_player( ch ) )
    return;

  if( !get_flags( ch, argument, &flags, "ib", "Qwho" ) )
    return;

  page_centered( ch, "-- PLAYERS --" );
  page( ch, "\r\n" );

  for( int pi = 0; pi < player_list; pi++ ) {
    player_data *wch = player_list[pi]; 
    if( !wch->In_Game( ) )
      continue;
  
    if( !can_see_who( ch, wch )
      || ( flags == 1 && !ch->Recognizes( wch ) )
      || ( flags == 2 && !ch->Befriended( wch ) )
      || !fmatches( argument, ch->descr->name, -1 ) ) 
      continue;      
 
    if( wch->shdata->level >= LEVEL_APPRENTICE && !wch->link && wch->timer < time(0) - 600 )
      continue;

    count++; // don't count players you can't see

    if( ch->pcdata->terminal != TERM_ANSI ) {
      page( ch, "%17s%s%s", wch->descr->name, ++i%4 ? "" : "\r\n" );
    } else {
      sprintf( tmp, "%s%17s%s%s", !wch->link && ( !wch->switched || !wch->switched->link || !wch->shape_changed || !wch->shape_changed->link ) ? red( ch ) : ( same_clan( player( ch ), wch ) ? cyan( ch ) : ( ch->Befriended( wch ) ? green( ch )
        : ( ch->Recognizes( wch ) ? yellow( ch ) : "" ) ) ), wch->descr->name, normal( ch ), ++i%4 ? "" : "\r\n" );
      page( ch, tmp );
    }
  }
 
  if( i%4 != 0 )
    page( ch, "\r\n" );

  if( count > max_players )
    update_high();

  page( ch, "\r\n" );
  sprintf( tmp, "%d players | %d high | %d record", count, max_players, record_players );
  page_centered( ch, tmp );
}


void do_who( char_data* ch, char* )
{
  char               buf  [ THREE_LINES ];
  char               tmp  [ ONE_LINE ];
  wizard_data*       imm;
  int               type;
  int                  i  = 0;
  int              count  = 0;
  int             length;
  bool             found;
  char*           abbrev;
  const char*  lvl_title;
  clan_data*        clan;
  title_data*      title;

  if( not_player( ch ) )
    return;

  page( ch, scroll_line[0] );

  for( type = 0; type < 4; type++ ) {
    found = FALSE;
    *buf = '\0';

    for( int pi = 0; pi < player_list; pi++ ) {
      player_data *wch = player_list[pi]; 
      if( !wch->In_Game( ) )
        continue;

      if( type == 0 ) {
        if( wch->shdata->level < LEVEL_APPRENTICE || !wch->link && wch->timer < time(0) - 600 )
          continue;
      } else if( wch->shdata->level >= LEVEL_APPRENTICE ) {
        continue;
      } else if( ch->Recognizes( wch ) ) {
        bool is_befriended = ch->Befriended( wch );
        
        if ( ch == wch )
          is_befriended = true; // put self on befriended list

        if ( type == 3 || (type == 1 && !is_befriended) || (type == 2 && is_befriended) )
          continue; 
      } else if( type != 3 ) {
        continue;
      }

      if( !can_see_who( ch, wch ) )
        continue;

      count++; // don't count players you can't see

      if( !found ) {
        if( i++ != 0 )
          page( ch, "\r\n" );
        page_title( ch, type > 1 ? ( type == 2 ? "Known" : "Unknown" ) : ( type == 0 ? "Immortals" : "Befriended" ) );
        found = TRUE;
      }  

      clan   = wch->pcdata->pfile->clan2;
      imm    = wizard( wch );
      abbrev = "  ";

      if( wch->shdata->level >= LEVEL_APPRENTICE ) {
        lvl_title = ( imm != NULL && imm->level_title != empty_string ) ? imm->level_title : imm_title[ wch->shdata->level-LEVEL_AVATAR ];
        length = strlen( lvl_title );
        strcpy( buf, "[               ]" );
        memcpy( buf+8-length/2, lvl_title, length );
      } else {
        if( !wch->link && ( ( !wch->switched || !wch->switched->link ) ) ) {
          abbrev = "LD";
        } else if( clan != NULL && ( title = get_title( clan, wch->pcdata->pfile ) ) != NULL && knows_members( player( ch ), clan ) ) {
          abbrev = "CL";
        } else if( wch->pcdata->trust >= LEVEL_AVATAR ) {
          if( has_permission( wch, PERM_APPROVE ) )
            abbrev = "AV";
          else if( ch->pcdata->trust >= LEVEL_AVATAR )
            abbrev = "  ";  // I prefer "IP" myself
        }

        if( is_incognito( wch, ch ) ) {
          sprintf( buf, "[   ??   %s %s ]", race_table[wch->shdata->race].abbrev, abbrev );
        } else {
          sprintf( buf, "[ %2d %s %s %s ]", wch->shdata->level,
            clss_table[wch->pcdata->clss].abbrev,
            race_table[wch->shdata->race].abbrev, abbrev );
        }
      }

      if( type > 0 ) {
        sprintf( tmp, " %%s%%s%%s%%-%ds  %%s\r\n", 53-strlen( wch->descr->name ) );
        sprintf( buf+17, tmp, bold_cyan_v( ch ), wch->descr->name, normal( ch ), wch->pcdata->title, clan != NULL && knows_members( player( ch ), clan ) ? clan->abbrev : " -- " );
      } else {
        if( imm != NULL && imm->wizinvis > 0 && is_set( wch->pcdata->pfile->flags, PLR_WIZINVIS ) )
          sprintf( tmp, " %%s%%s%%s%%-%ds   %d\r\n", 53-strlen( wch->descr->name ), imm->wizinvis );
        else
          sprintf( tmp, " %%s%%s%%s%%-%ds\r\n", 53-strlen( wch->descr->name ) );

        sprintf( buf+17, tmp, bold_cyan_v( ch ), wch->descr->name, normal( ch ), wch->pcdata->title );
      }

      page( ch, buf );
    }
  }
  
  if( count > max_players )
    update_high();

  page( ch, "\r\n" );
  sprintf( tmp, "[ %d players | %d high | %d record ]", count, max_players, record_players );
  page_centered( ch, tmp );

  page( ch, scroll_line[0] );
}


void w3_who( )
{
  player_data*  pc;
  FILE*         fp;
  char         buf  [ THREE_LINES ];
  bool       found = false;

  if( ( fp = open_file( W3_DIR, "who.html", "wb" ) ) == NULL )
    return;

  fprintf( fp, "<html>\n" );
  fprintf( fp, "<body bgcolor=\"#ffffff\">\n" );
  fprintf( fp, "<header>" );

  fprintf( fp, "<STYLE TYPE=\"text/css\">" );
  fprintf( fp, "<!--" );
  fprintf( fp, "T2" );
  fprintf( fp, "{" );
  fprintf( fp, "font-size:30pt;" );
  fprintf( fp, "font-style:sans-serif;" );
  fprintf( fp, "color:#736AFF;" );
  fprintf( fp, "}" );
  fprintf( fp, "-->" );
  fprintf( fp, "</STYLE>" );

  fprintf( fp, "<STYLE TYPE=\"text/css\">" );
  fprintf( fp, "<!--" );
  fprintf( fp, "T3" );
  fprintf( fp, "{" );
  fprintf( fp, "font-size:45pt;" );
  fprintf( fp, "font-style:sans-serif;" );
  fprintf( fp, "color:#4E9258;" );
  fprintf( fp, "}" );
  fprintf( fp, "-->" );
  fprintf( fp, "</STYLE>" );
  
  fprintf( fp, "<STYLE TYPE=\"text/css\">" );
  fprintf( fp, "<!--" );
  fprintf( fp, "A2" );
  fprintf( fp, "{" );
  fprintf( fp, "font-size:15pt;" );
  fprintf( fp, "font-style:sans-serif;" );
  fprintf( fp, "}" );
  fprintf( fp, "-->" );
  fprintf( fp, "</STYLE>" );

  fprintf( fp, "<div align=center><T3> Legacy's Dawning </div align=center></T3><br>" );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( is_apprentice( pc ) && !is_set( pc->pcdata->pfile->flags, PLR_WIZINVIS ) ) {
      if( !found ) {
        fprintf( fp, "<div align=center>\n" );
        fprintf( fp, "<u><T2> Immortals </T2></u><br><br>" );
        fprintf( fp, "</div><pre><A2>" );
        found = true;
      }
      wizard_data* imm = wizard( pc );
      int length = 0;
      const char*  lvl_title;

      lvl_title = ( imm != NULL && imm->level_title != empty_string ) ? imm->level_title : imm_title[ pc->shdata->level-LEVEL_AVATAR ];
      length = strlen( lvl_title );
      strcpy( buf, "[               ]" );
      memcpy( buf+8-length/2, lvl_title, length );
      sprintf( buf, "%s %s %s<br>", buf, pc->descr->name, pc->pcdata->title );

      fprintf( fp, buf );
    }
  }

  fprintf( fp, "</pre></A2>" );

  found = false;
  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i]; 
    if( pc->In_Game( )  && !is_apprentice( pc ) ) {
      const char* abbrev = "  ";
      title_data* title;
      if( !found ) {
        fprintf( fp, "<div align=center>\n" );
        fprintf( fp, "<u><T2> Players </T2></u>\n" );
        fprintf( fp, "</div><pre><A2><br><br>\n" );
        found = true;
      }
      if( !pc->link && ( ( !pc->switched || !pc->switched->link ) ) ) {
          abbrev = "LD";
        } else if( pc->pcdata->pfile->clan2 != NULL && ( title = get_title( pc->pcdata->pfile->clan2, pc->pcdata->pfile ) ) != NULL && is_set( pc->pcdata->pfile->clan2->flags, CLAN_KNOWN ) && is_set( pc->pcdata->pfile->clan2->flags, CLAN_PUBLIC ) ) {
          abbrev = "CL";
        } else if( pc->pcdata->trust >= LEVEL_AVATAR ) {
          if( has_permission( pc, PERM_APPROVE ) )
            abbrev = "AV";
        }
        if( level_setting( &pc->pcdata->pfile->settings, SET_INCOGNITO ) != 0 )
          fprintf( fp, "[  ??    %s %s ] %s %s       %s<br>", race_table[ pc->shdata->race ].abbrev, abbrev,  pc->descr->name, pc->pcdata->title, pc->pcdata->pfile->clan2 != NULL && is_set( pc->pcdata->pfile->clan2->flags, CLAN_KNOWN ) && is_set( pc->pcdata->pfile->clan2->flags, CLAN_PUBLIC ) ? pc->pcdata->pfile->clan2->abbrev : "----" );
        else
          fprintf( fp, "[ %2i %s %s %s ] %s %s       %s<br>", pc->shdata->level, clss_table[pc->pcdata->clss].abbrev, race_table[pc->shdata->race].abbrev, abbrev, pc->descr->name, pc->pcdata->title, pc->pcdata->pfile->clan2 != NULL && is_set( pc->pcdata->pfile->clan2->flags, CLAN_KNOWN ) && is_set( pc->pcdata->pfile->clan2->flags, CLAN_PUBLIC ) ? pc->pcdata->pfile->clan2->abbrev : "----" );
    }
  }
  fprintf( fp, "<pre></A2>" );
  fprintf( fp, "<hr><br>\n" );
  fprintf( fp, "<a href=\"./index.html\"> Return to Main </a></p>\n" );

  fclose( fp );
}  


/*
 *   USERS ROUTINE
 */


bool Wizard_Data :: See_Account( pfile_data* pfile )
{
  if( is_demigod( this ) || pcdata->pfile == pfile )
    return TRUE;

  if( pfile->level >= LEVEL_APPRENTICE )
    return FALSE;

  return has_permission( this, PERM_SITE_NAMES ); 
}


void do_users( char_data* ch, char* argument )
{
  char            tmp  [ THREE_LINES ];
  player_data* victim;
  link_data*     link;
  int           count  = 0;
  int           flags;
  int          length;

  if( !get_flags( ch, argument, &flags, "aiw", "Users" ) )
    return;

  if( !is_apprentice( ch ) )
    return;

  length = strlen( argument );

  if( is_set( &flags, 0 ) ) {
    sprintf( tmp, "%-15s   %s\r\n", "Name", "Appearance" );
  } else if( is_set( &flags, 1 ) ) {
    sprintf( tmp, "%-18s%3s %3s %3s %3s   %3s %3s   %4s %4s %4s   %-7s\r\n",
      "Name", "Cls", "Rce", "Ali", "Lvl", "Trs", "Idl",
      "Hits", "Enrg", "Move", "Bank" );
  } else if( is_set( &flags, 2 ) ) {
    sprintf( tmp, "%-15s  %4s  %s\r\n", "Name", "Idle", "What?" );
  } else {
    sprintf( tmp, "%-15s   %-30s   %s\r\n", "Name", "Site", "Location" );
  }

  page_underlined( ch, tmp );

  link_nav links( &link_list );
  for( link = links.first(); link != NULL; link = links.next() ) {
    victim = link->player;
    if( ( is_god( ch ) || ( victim != NULL && can_see_who( ch, victim ) ) )
      && !strncasecmp( argument, victim == NULL ? "" : victim->descr->name, length ) ) {
      if( flags == 0 ) {
        sprintf( tmp, "%-15s   %-30s   %s",
          victim == NULL ? "(Logging In)" : victim->descr->name,
          is_angel( ch ) || victim == ch
          || ( victim != NULL && victim->shdata->level < LEVEL_APPRENTICE ) 
          ? &link->host[ max( 0, strlen( link->host )-30 ) ] : "(protected)",
          ( victim == NULL || victim->array == NULL )
          ? "(nowhere)" : victim->array->where->Location( ) );
        truncate( tmp, 78 );
        strcat( tmp, "\r\n" );
      } else {
        if( link->connected != CON_PLAYING ) {
          sprintf( tmp, "-- Logging In --\r\n" );
        } else if( is_set( &flags, 0 ) ) {
          sprintf( tmp, "%-15s   %s\r\n",
            victim->descr->name, victim->descr->singular );          
        } else if( is_set( &flags, 2 ) ) {
          sprintf( tmp, "%-15s  %4d  %s\r\n", victim->descr->name, (int) time(0) - victim->timer, "??" );          
        } else {
          sprintf( tmp,
            "%-18s%3s %3s  %2s %3d   %3d %3d   %4d %4d %4d   %-7d\r\n",
            victim->descr->name,
            clss_table[victim->pcdata->clss].abbrev,
            race_table[victim->shdata->race].abbrev,
            alignment_table[ victim->shdata->alignment ].abbrev,
            victim->shdata->level,
            victim->pcdata->trust, (int) time(0) - victim->timer,
            victim->max_hit, victim->max_mana, victim->max_move,
            victim->bank );
        }
      }
      page( ch, tmp );
      count++;
    }
  }
}


/*
 *   HOMEPAGE
 */


void do_homepage( char_data* ch, char* argument )
{
  if( is_mob( ch ) ) 
    return;

  if( *argument == '\0' ) {
    send( ch, "What is your homepage address?\r\n" );
    return;
  } 

  if( strlen( argument ) > 60 ) {
    send( ch, "You homepage address must be less than 60 characters.\r\n" );
    return;
  } 

  free_string( ch->pcdata->pfile->homepage, MEM_PFILE );
  ch->pcdata->pfile->homepage = alloc_string( argument, MEM_PFILE );

  send( ch, "Your homepage is set to %s.\r\n", argument );
}  


void update_high()
{
  int count = 0;

  for( int i = 0; i < player_list; i++ ) {
    player_data* player = player_list[ i ];
    if( !player || !player->In_Game( ) )
      continue;

    count++;
  }

  if (count > max_players) {
    max_players = count;
    
    if (max_players > record_players) {
      record_players = max_players;
      save_misc();
    }
  }
}



