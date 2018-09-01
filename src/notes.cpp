/* ************************************************************************
*   File: selt_notes.cpp                Selt's Sweet CircleMUD Noteboards *
*  Usage: Nice global noteboards                                          *
*   (c) 1998+ Julian Forsythe (julian@rainchild.com)                      *
************************************************************************ */

#include "system.h"

bool note_to_help( char_data *ch, note_data *note );

// global noteboard structure
llist<noteboard_data> noteboards;

// permissions stuff

bool can_remove( player_data* ch, noteboard_data *noteboard, note_data* note )
{
  if( noteboard->get_lvl_remove( ) == 0 )
    return TRUE;

  if( !strcmp( ch->descr->name, note->get_author( ) ) || has_permission( ch, PERM_NOTEBOARD ) || noteboard->get_lvl_remove( ) <= ch->pcdata->trust )
    return TRUE;

  if( fmatches( "clan_", noteboard->get_name( ) ) && ch->pcdata->pfile->clan2 && ch->pcdata->pfile->clan2->noteboard == noteboard ) {
    title_data *title = get_title( ch->pcdata->pfile->clan2, ch->pcdata->pfile );
    if( title && is_set( title->flags, TITLE_REMOVE_NOTES ) )
      return TRUE;
  }

  if( fmatches( "guild_", noteboard->get_name( ) ) && ch->pcdata->pfile->guild && ch->pcdata->pfile->guild->noteboard == noteboard ) {
    title_data *title = get_title( ch->pcdata->pfile->guild, ch->pcdata->pfile );
    if( title && is_set( title->flags, TITLE_REMOVE_NOTES ) )
      return TRUE;
  }

  return FALSE;
}


bool can_write( player_data* ch, noteboard_data *noteboard )
{
  if( ch->gossip_pts < 0 )
    return FALSE;

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_COMMUNICATION ) )
    return FALSE;

  if( noteboard->get_lvl_write( ) == 0 )
    return TRUE;

  if( noteboard->get_lvl_write( ) <= ch->pcdata->trust )
    return TRUE;

  if( fmatches( "clan_", noteboard->get_name( ) ) && ch->pcdata->pfile->clan2 && ch->pcdata->pfile->clan2->noteboard == noteboard )
    return TRUE;

  if( fmatches( "guild_", noteboard->get_name( ) ) && ch->pcdata->pfile->guild && ch->pcdata->pfile->guild->noteboard == noteboard )
    return TRUE;

  return FALSE;
}


bool can_read( player_data *ch, noteboard_data *noteboard )
{ 
  if( noteboard->get_lvl_read( ) == 0 )
    return TRUE;

  if( noteboard->get_lvl_read( ) <= ch->pcdata->trust )
    return TRUE;

  if( fmatches( "clan_", noteboard->get_name( ) ) && ch->pcdata->pfile->clan2 && ch->pcdata->pfile->clan2->noteboard == noteboard )
    return TRUE;

  if( fmatches( "guild_", noteboard->get_name( ) ) && ch->pcdata->pfile->guild && ch->pcdata->pfile->guild->noteboard == noteboard )
    return TRUE;

  return FALSE;
}


void extract( player_data * ch, note_data* note )
{
  if( !ch || !note )
    return;

  for( int i = 0; i < player_list; i++ ) {
    player_data *victim = player_list[i];
    if( !victim || !victim->Is_Valid( ) || !victim->get_note_buffer( ) )
      continue;

    if( note == victim->get_note_buffer()->get_original( ) ) {
      victim->get_note_buffer()->set_original(NULL);
      if( victim != ch )
        send( victim, "%s just deleted the note you were editing.  Buffer kept, use 'note post' to post this new copy.\r\n", ch );
    }
  } 

  if( note->get_board( ) )
    note->get_board( )->remove_note( note );

  if( ch->get_note_buffer() == note )
    ch->set_note_buffer( NULL );

  delete note;
}        


// note command handling routine
void do_notes( char_data *chd, char *argumentx )
{
  selt_string arg, arg2, line, buf, temp, argument;
  noteboard_data *board;
  note_data *note, *prev;
  int num = 0, displayed = FALSE, post;
  bool opt_reverse = false;

  player_data *ch;
  if( ( ch = player( chd ) ) == NULL )
    return;

  if( is_set( ch->pcdata->pfile->flags, PLR_REVERSE ) )
    opt_reverse = true;

  argument = argumentx;
  half_chop( &argument, &arg, &arg2 );

  // noteboard admin routines
  if( is_demigod( ch ) ) {
    if( !str_cmp( arg, "createboard" ) ) {
      // noteboard creator
      line = arg2;
      half_chop( &line, &arg, &arg2 );

      if( arg == "" ) {
        page( ch, "Syntax: note createboard <name>\r\n" );
        return;
      }

      temp = arg;
      temp += ".brd";

      // search through all the boards to make sure this is a unique name
      board_nav blist( &noteboards );
      for( board = blist.first( ); board; board = blist.next( ) ) {
        if( fmatches( board->get_name( ), arg ) || !strcmp( board->get_filename( ), temp ) ) {
          page( ch, "A board already exists with that name.\r\n" );
          return;
        }
      }

      if( arg2 == "" ) {
        board = new noteboard_data( arg, temp, LEVEL_APPRENTICE, LEVEL_APPRENTICE, LEVEL_BUILDER, 0, 4096 );
        // noteboards.add(board);
        ch->set_current_board( board );
        save_noteboard( board );

        // new_noteboard(name,lename, read level, write level, modify others level,
        //                 cost to post <unsupported>, cost to modify <unsupported>,
        //                 buffer size <unsupported>);

        ch->sendf( "Created '%s' noteboard with read/write level %d, modify level %d.\r\n", arg.get_text( ), LEVEL_APPRENTICE, LEVEL_BUILDER );
        line._printf( "%s just created the '%s' noteboard with read/write level %d, modify level %d.", ch->descr->name, arg.get_text(), LEVEL_APPRENTICE, LEVEL_BUILDER );
        info( empty_string, LEVEL_APPRENTICE, line, IFLAG_NOTES, board->get_iflag_level( ), ch );

      } else {
        page( ch, "Syntax: note createboard <name>.\r\n" );
        return;
      }
      return;
    } else if( !str_cmp( arg, "boardlevel" ) ) {
      // set noteboards levels
      half_chop( &arg2, &arg, &arg2 );

      if( arg == "" ) {
        ch->sendf( "Noteboard levels: Read %d, Write %d, Remove %d, Iflag %d.\r\n", ch->get_current_board( )->get_lvl_read( ), ch->get_current_board( )->get_lvl_write( ), ch->get_current_board( )->get_lvl_remove( ), ch->get_current_board( )->get_iflag_level( ) );
        return;
      } else if( !isnumber( arg2 ) ) {
        page( ch, "Syntax: note boardlevel <read|write|modify|iflag> <level>\r\n" );
        return;
      } else if( !ch->get_current_board( ) ) {
        page( ch, "Select a board to modify first, using note <boardname>\r\n" );
        return;
      }

      int lvl = atoi( arg2 );
      board = ch->get_current_board( );

      if( !str_cmp( arg, "read" ) ) {
        // set read level
        board->set_lvl_read( max( 0, min( ch->pcdata->trust, lvl ) ) );
        ch->sendf( "Noteboard read level set to %d.\r\n", board->get_lvl_read( ) );
        line._printf( "%s just changed the read level on the '%s' noteboard to %d", ch->descr->name, board->get_name( ), board->get_lvl_read( ) );
        info( empty_string, LEVEL_APPRENTICE, line, IFLAG_NOTES, board->get_iflag_level( ), ch );
      } else if( !str_cmp( arg, "write" ) ) {
        // set write level
        board->set_lvl_write( max( 0, min( ch->pcdata->trust, lvl ) ) );
        ch->sendf( "Noteboard write level set to %d.\r\n", board->get_lvl_write( ) );
        line._printf( "%s just changed the write level on the '%s' noteboard to %d", ch->descr->name, board->get_name( ), board->get_lvl_write( ) );
        info( empty_string, LEVEL_APPRENTICE, line, IFLAG_NOTES, board->get_iflag_level( ), ch );
      } else if( !str_cmp( arg, "modify" ) ) {
        // set modify level
        board->set_lvl_remove( max( 0, min( ch->pcdata->trust, lvl ) ) );
        ch->sendf( "Noteboard modify level set to %d.\r\n", board->get_lvl_remove( ) );
        line._printf( "%s just changed the modify level on the '%s' noteboard to %d", ch->descr->name, board->get_name( ), board->get_lvl_remove( ) );
        info( empty_string, LEVEL_APPRENTICE, line, IFLAG_NOTES, board->get_iflag_level( ), ch );
      } else if( !str_cmp( arg, "iflag" ) ) {
        // set iflag level
        board->set_iflag_level( max( 0, min( 3, lvl ) ) );
        ch->sendf( "Noteboard iflag level set to %d.\r\n", board->get_iflag_level( ) );
        line._printf( "%s just changed the iflag level on the '%s' noteboard to %d", ch->descr->name, board->get_name( ), board->get_iflag_level( ) );
        info( empty_string, LEVEL_APPRENTICE, line, IFLAG_NOTES, board->get_iflag_level( ), ch );
      } else {
        // invalid syntax, return
        page( ch, "Syntax: note boardlevel <read|write|modify|iflag> <level>\r\n" );
        return;
      }
      
      save_noteboard( board );
      return;
    }
  } // end noteboard admin routines

  if (noteboards.empty) {
    page(ch, "No noteboards found! Please contact an immortal.\r\n");
    if (is_demigod(ch))
      page(ch, "Or create a new board with 'note createboard <name>' and 'note boardlevel <read|write|modify|iflag> <level>\r\n");
    return;
  }

  if (!ch->get_current_board()) {
    board_nav blist(&noteboards);
    for (noteboard_data *temp = blist.first(); temp; temp = blist.next())
      if (can_read(ch, temp)) {
        ch->set_current_board(temp);
        break;
      }

    if (!ch->get_current_board()) {
      page(ch, "No noteboards found! Please contact an immortal.\r\n");
      return;
    }
  }

  if (!ch->get_note_buffer()) { // check reserved words while browsing
    if (arg == "") { // list notes
      selt_string name;
      name.printf("%s Board:   (iflag level %d)\r\n", ch->get_current_board()->get_name(), ch->get_current_board()->get_iflag_level());
      page(ch, name);
      if (ch->get_current_board()->get_notes()->empty) {
        page(ch, "... is empty\r\n");
      } else {
        note_nav nlist(ch->get_current_board()->get_notes());
        for (note = (opt_reverse ? nlist.last() : nlist.first()), num = 1; note; note = (opt_reverse ? nlist.prev() : nlist.next()), num++) {
          time_t modified = note->get_modified();
          selt_string temp;
          temp.printf("[%3d] %-35s %-12s ", num, note->get_title(), note->get_author());
          temp += ctime(&modified);
          temp--; // kill the \n
          temp += "\r\n";
          page(ch, temp);
        }
      }
    } else if (isnumber(arg)) {
      displayed = FALSE;
      note_nav nlist(ch->get_current_board()->get_notes());
      for (note = (opt_reverse ? nlist.last() : nlist.first()), num = 1; note; note = (opt_reverse ? nlist.prev() : nlist.next()), num++) {
        if (num == atoi(arg)) {
          time_t modified = note->get_modified();
          char tmp1  [ 3*MAX_STRING_LENGTH ];
          convert_to_ansi( ch, note->get_text()->get_string(), tmp1, sizeof(tmp1) );
          selt_string buf;
          buf.printf(" Title: %s\r\n"
                     "Author: %s\r\n"
                     "  Date: %s\r\n%s\r\n", note->get_title(), note->get_author(), ctime(&modified), tmp1);
          page(ch, buf);
          displayed = TRUE;
          break;
        }
      }
      if (!displayed)
        page(ch, "No such note exists.\r\n");
    } else if (is_demigod(ch) && !str_cmp(arg, "convert")) { // convert to help
      half_chop(&arg2, &arg, &arg2);
      if (!isnumber(arg)) {
        page(ch, "Syntax: note convert <number> [remove]. See 'help note' for more information.\r\n");
        return;
      }

      displayed = FALSE;
      prev = NULL;
      post = atoi(arg);
      note_nav nlist(ch->get_current_board()->get_notes());
      for (note = (opt_reverse ? nlist.last() : nlist.first()), num = 1; note; note = (opt_reverse ? nlist.prev() : nlist.next()), num++) {
        if (num == post) {
          displayed = TRUE;
          if (note_to_help(ch, note)) {
            if (arg2 == "remove") {
              send(ch, "**** Note %s Successfully Converted and Removed ****\r\n", note->get_title());
              extract(ch, note);
              save_noteboard(ch->get_current_board());
            } else {
              send(ch, "**** Note %s Successfully Converted ****\r\n", note->get_title());
            }
          } else {
            send(ch, "**** Note Convertsion Failed ****\r\n");
          }
          break;
        }
      }
      if (!displayed)
        page(ch, "No such note exists.\r\n");
    } else if (!str_cmp(arg, "delete") || !str_cmp(arg, "remove")) { // delete a post
      if (!isnumber(arg2)) {
        page(ch, "Syntax: note delete <number>. See 'help note' for more information.\r\n");
        return;
      }

      displayed = FALSE;
      prev = NULL;
      post = atoi(arg2);
      note_nav nlist(ch->get_current_board()->get_notes());
      for (note = (opt_reverse ? nlist.last() : nlist.first()), num = 1; note; note = (opt_reverse ? nlist.prev() : nlist.next()), num++) {
        if (num == post) {
          displayed = TRUE;

          if (!can_remove(ch, ch->get_current_board(), note)) {
            page(ch, "You do not have permission to modify that note.\r\n");
            return;
          }

          clan_data* clan = NULL;
          board = ch->get_current_board();
      
          if( fmatches( "clan_", board->get_name( ) ) && ch->pcdata->pfile->clan2 && ch->pcdata->pfile->clan2->noteboard == board )
            clan = ch->pcdata->pfile->clan2; 

          if( fmatches( "guild_", board->get_name( ) ) && ch->pcdata->pfile->guild && ch->pcdata->pfile->guild->noteboard == board )
            clan = ch->pcdata->pfile->guild; 

          line.printf("Note '%s' removed by %s from the %s Board.", note->get_title(), ch->descr->name, board->get_name());
          info( empty_string, board->get_lvl_read(), line, IFLAG_NOTES, board->get_iflag_level(), ch, clan );

          extract(ch, note);
          save_noteboard(board);

          page(ch, "Note removed.\r\n");
          break;
        }
      }
      if (!displayed)
        page(ch, "No such note exists.\r\n");
    } else if (!str_cmp(arg, "edit")) { // edit a post
      if (!isnumber(arg2)) {
        page(ch, "Syntax: note edit <number>. See 'help note' for more information.\r\n");
        return;
      }

      displayed = FALSE;
      prev = NULL;
      post = atoi(arg2);

      note_nav nlist(ch->get_current_board()->get_notes());
      for (note = (opt_reverse ? nlist.last() : nlist.first()), num = 1; note; note = (opt_reverse ? nlist.prev() : nlist.next()), num++) {
        if (num == post) {
          displayed = TRUE;

          if (!can_remove(ch, ch->get_current_board(), note)) {
            page(ch, "You do not have permission to modify that note.\r\n");
            return;
          }

          ch->set_note_buffer(note->duplicate());
          ch->get_note_buffer()->set_original(note);

          page(ch, "You begin editing that note.\r\n");
          break;
        }
      }
      if (!displayed)
        page(ch, "No such note exists.\r\n");
      
    } else if (!str_cmp(arg, "move")) { // move a note from one board to the next
      line = arg2;
      half_chop(&line, &arg, &arg2);
      if (!isnumber(arg) || arg2 == "") {
        page(ch, "Syntax: note move <number> <board>. See 'help note' for more information.\r\n");
        return;
      }

      if (!(board = return_board(ch, arg2))) {
        page(ch, "That board does not exist.\r\n");
        return;
      }

      displayed = FALSE;
      prev = NULL;
      post = atoi(arg);

      note_nav nlist(ch->get_current_board()->get_notes());
      for (note = (opt_reverse ? nlist.last() : nlist.first()), num = 1; note; note = (opt_reverse ? nlist.prev() : nlist.next()), num++) {
        if (num == post) {
          displayed = TRUE;

          if (!can_remove(ch, ch->get_current_board(), note)) {
            page(ch, "You do not have permission to modify that note.\r\n");
            return;
          }

          ch->get_current_board()->remove_note(note);
          note->set_modified(time(0));
          board->add_note(note);

          save_noteboard(board);
          save_noteboard(ch->get_current_board());

          page(ch, "Note moved.\r\n");
          line.printf("Note '%s' moved by %s to the %s Board from the %s Board.", note->get_title(), ch->descr->name, board->get_name(), ch->get_current_board()->get_name());
          info(empty_string, board->get_lvl_read(), line, IFLAG_NOTES, min(board->get_iflag_level(), ch->get_current_board()->get_iflag_level()), ch );
          break;
        }
      }
      if (!displayed)
        page(ch, "No such note exists.\r\n");
      
    } else if (fmatches(arg, "summary")) { // give a summary of notes since last login
      if( arg2 == "update" ) {
        ch->pcdata->pfile->last_on = time( 0 );
        return;
      }
      else if (arg2 == "" || !str_cmp(arg2, "last")) {
        displayed = FALSE;
        board_nav blist(&noteboards);
        for (board = blist.first(); board; board = blist.next()) {
          if (!can_read(ch, board))
            continue;
          num = 0;
          note_nav nlist(board->get_notes());
          for (note = nlist.first(); note; note = nlist.next()) {
            if (note->get_modified() >= ch->pcdata->pfile->last_on)
              num++;
          }
          if (num) {
            if (!displayed) {
              page(ch, "Note Summary (since last login):\r\n");
              displayed = TRUE;
            }
            ch->sendf("%15s: %d\r\n", board->get_name(), num);
          }
        }
        if (!displayed)
          page(ch, "No new notes since last login.\r\n");

      } else if (!str_cmp(arg2, "list")) {
        page(ch, "Note Summary (all notes):\r\n");
        board_nav blist(&noteboards);
        for (board = blist.first(); board; board = blist.next()) {
          if (!can_read(ch, board))
            continue;
          num = 0;
          note_nav nlist(board->get_notes());
          for (note = nlist.first(); note; note = nlist.next())
            num++;
          ch->sendf("%15s: %d\r\n", board->get_name(), num);
        }
      } else {
        page(ch, "Syntax: note summary <last/list/etc>. See 'help note' for more information.\r\n");
      }
    } else if (!str_cmp(arg, "new")) { // start writing a new note
      if (!can_write(ch, ch->get_current_board())) {
        page(ch, "You do not have permission to post on that board.\r\n");
        return;
      }

      if (arg2 == "") {
        page(ch, "You must specify a title.\r\n");
        return;
      } else if (strlen(arg2) > 35) {
        page(ch, "Maximum title length is 35 characters.\r\n");
        return;
      }

      // allocate the memory
      ch->set_note_buffer(new note_data);
      ch->get_note_buffer()->set_modified(time(0));
      ch->get_note_buffer()->set_author(ch->descr->name);
      ch->get_note_buffer()->set_title(arg2);

      // say ok
      page(ch, "Note created: ");
      page(ch, ch->get_note_buffer()->get_title());
      page(ch, "\r\n");
    } else { // either change noteboards, or give a board list
      if (arg2 == "" && (board = return_board(ch, arg))) { // change current noteboard
        ch->set_current_board(board);
        ch->sendf("Reading %s Board (%d notes).\r\n", ch->get_current_board()->get_name(), ch->get_current_board()->get_notes()->size);
      } else { // list boards
        line = "";
        board_nav blist(&noteboards);
        for (board = blist.first(); board; board = blist.next()) {
          if (!can_read(ch, board))
            continue;
          if (line)
            line += ", ";
          line += board->get_name();
        }
        line.printf("Available Noteboards (use 'note new <title>' for a new note): @c%s@n.\r\n", line.get_text());
        char tmp1  [ 3*MAX_STRING_LENGTH ];
        convert_to_ansi( ch, line.get_text(), tmp1, sizeof(tmp1) );
        ch->sendf(tmp1);
      }
    }
  } else { // check reserved words while editing
    if (arg == "") { // echo the edit buffer to the character
      page(ch, "Note Title: %s\r\n", ch->get_note_buffer()->get_title());
      ch->get_note_buffer()->get_text()->list(ch);
    } else if (!str_cmp(arg, "adopt") && arg2 == "") { // adopt note
      ch->get_note_buffer()->set_author(ch->descr->name);
      page(ch, "Note adopted.\r\n");
    } else if (!str_cmp(arg, "delete") && arg2 == "") { // free note
      extract(ch, ch->get_note_buffer());
      page(ch, "Note aborted.\r\n");
    } else if (!str_cmp(arg, "save") && arg2 == "") {
      displayed = FALSE;
      board = ch->get_current_board();

      if (ch->get_note_buffer()->get_original()) {
        // free old copy
        board = ch->get_note_buffer()->get_original()->get_board();
        if (!board)
          board = ch->get_current_board();

        extract(ch, ch->get_note_buffer()->get_original());
        displayed = TRUE;
      }

      ch->get_note_buffer()->set_modified(time(0));

      // dupe it so you can continue editing
      ch->get_note_buffer()->set_original(ch->get_note_buffer()->duplicate());
      board->add_note(ch->get_note_buffer()->get_original());
      save_noteboard(board);

      clan_data* clan = NULL;
      
      if( fmatches( "clan_", board->get_name( ) ) && ch->pcdata->pfile->clan2 && ch->pcdata->pfile->clan2->noteboard == board )
        clan = ch->pcdata->pfile->clan2; 

      if( fmatches( "guild_", board->get_name( ) ) && ch->pcdata->pfile->guild && ch->pcdata->pfile->guild->noteboard == board )
        clan = ch->pcdata->pfile->guild; 

      if (!displayed) {
        ch->sendf("Note saved on the %s Board.\r\n", board->get_name());
        line.printf("Note '%s' saved by %s on the %s Board.", ch->get_note_buffer()->get_original()->get_title(), ch->descr->name, board->get_name());
        info(empty_string, board->get_lvl_read(), line, IFLAG_NOTES, board->get_iflag_level(), ch );
      } else {
        ch->sendf("Note updated on the %s Board.\r\n", board->get_name());
        line.printf("Note '%s' updated by %s on the %s Board.", ch->get_note_buffer()->get_original()->get_title(), ch->descr->name, board->get_name());
        info(empty_string, board->get_lvl_read(), line, IFLAG_NOTES, board->get_iflag_level(), ch );
      }
    } else if (!str_cmp(arg, "post") && arg2 == "") {
      displayed = FALSE;
      board = ch->get_current_board();

      if (ch->get_note_buffer()->get_original()) {
        // free old copy
        board = ch->get_note_buffer()->get_original()->get_board();
        if (!board)
          board = ch->get_current_board();

        extract(ch, ch->get_note_buffer()->get_original());
        displayed = TRUE;
      }

      note = ch->get_note_buffer();
      ch->set_note_buffer(NULL);
      note->set_modified(time(0));
      board->add_note(note);
      save_noteboard(board);

      clan_data* clan = NULL;
      
      if( fmatches( "clan_", board->get_name( ) ) && ch->pcdata->pfile->clan2 && ch->pcdata->pfile->clan2->noteboard == board )
        clan = ch->pcdata->pfile->clan2; 

      if( fmatches( "guild_", board->get_name( ) ) && ch->pcdata->pfile->guild && ch->pcdata->pfile->guild->noteboard == board )
        clan = ch->pcdata->pfile->guild; 

      if( !displayed ) {
        ch->sendf( "Note posted to the %s Board.\r\n", board->get_name( ) );
        line.printf( "Note '%s' posted by %s on the %s Board.", note->get_title( ), ch->descr->name, board->get_name( ) );
        info( empty_string, board->get_lvl_read( ), line, IFLAG_NOTES, board->get_iflag_level( ), ch, clan );
      } else {
        ch->sendf( "Note updated on the %s Board.\r\n", board->get_name( ) );
        line.printf( "Note '%s' updated by %s on the %s Board.", note->get_title( ), ch->descr->name, board->get_name( ) );
        info( empty_string, board->get_lvl_read( ), line, IFLAG_NOTES, board->get_iflag_level( ), ch, clan );
      }

    } else if (!str_cmp(arg, "preview") && arg2 == "") { // preview note
      char tmp1  [ 3*MAX_STRING_LENGTH ];
      note_data *note = ch->get_note_buffer();
      time_t modified = note->get_modified();

      convert_to_ansi( ch, note->get_text()->get_string(), tmp1, sizeof(tmp1) );

      selt_string buf;
      buf.printf(" Title: %s\r\n"
                 "Author: %s\r\n"
                 "  Date: %s\r\n%s\r\n", note->get_title(), note->get_author(), ctime(&modified), tmp1);
      page(ch, buf);
    } else if (!str_cmp(arg, "title")) { // change notes title
      if (arg2 == "") {
        page(ch, "You must specify a title.\r\n");
        return;
      } else if (strlen(arg2) > 35) {
        page(ch, "Maximum title length is 35 characters.\r\n");
        return;
      }
      ch->get_note_buffer()->set_title(arg2);
      page(ch, "Note renamed: ");
      page(ch, ch->get_note_buffer()->get_title());
      page(ch, "\r\n");
    } else { // add a new line of text
      ch->get_note_buffer()->get_text()->edit(argument, ch);
    }
  }
}


// scans for noteboard in the global structure then return it
noteboard_data *return_board(player_data *ch, const char *arg)
{
  noteboard_data *board = NULL;

  if( fmatches( arg, "clan" ) )
    return ch->pcdata->pfile->clan2 ? ch->pcdata->pfile->clan2->noteboard : NULL;

  if( fmatches( arg, "guild" ) )
    return ch->pcdata->pfile->guild ? ch->pcdata->pfile->guild->noteboard : NULL;

  if( noteboards.empty )
    return NULL;

  board_nav blist( &noteboards );
  for( board = blist.first( ); board; board = blist.next( ) )
    if( fmatches( arg, board->get_name( ) ) )
      if( can_read( ch, board ) )
        break;

  return board;
}

noteboard_data::noteboard_data(const char *tname, const char *tfile, int tread, int twrite, int tremove, int tiflag, int tbuf_size)
{
  record_new( sizeof( noteboard_data), MEM_NOTE );
  name = alloc_string( tname, MEM_NOTE );
  filename = alloc_string( tfile, MEM_NOTE );

  lvl_read       = max(0, min(LEVEL_GOD, tread));
  lvl_write      = max(0, min(LEVEL_GOD, twrite));
  lvl_remove     = max(0, min(LEVEL_GOD, tremove));
  iflag_level    = max(0, min(3, tiflag));
  note_size      = max(1024, tbuf_size);

  noteboards.add(this);
}

noteboard_data::~noteboard_data()
{
  noteboards.remove( this );

  note_data *freeme;
  if( name )
    free_string( name, MEM_NOTE );
  if( filename )
    free_string( name, MEM_NOTE );
  while( ( freeme = notes.headitem( ) ) ) {
    notes.remove( freeme );
    delete freeme;
  }

  record_delete( sizeof( noteboard_data ), MEM_NOTE );
}

note_data *note_data::duplicate()
{
  note_data *temp = new note_data;

  temp->set_author(author);
  temp->set_title(title);
  temp->set_modified(modified);
  temp->set_text(&text);

  return temp;
}

note_data::~note_data()
{
  if( board )
    board->remove_note( this );
  if( author )
    free_string( author, MEM_NOTE );
  if( title )
    free_string( title, MEM_NOTE );

  record_delete( sizeof( note_data ), MEM_NOTE );
}

int new_notes(char_data *chd)
{
  int count = 0;
  noteboard_data *board;
  note_data *note;

  player_data *ch;
  if ((ch = player(chd)) == NULL)
    return 0;

/*
  clan_data*    clan;

  if( ( clan = ch->pcdata->pfile->clan ) != NULL ) {
    Note_List[ NOTE_CLAN ] = clan->Note_List;
    max_note[ NOTE_CLAN ]  = clan->max_note;
    }
*/

  board_nav blist(&noteboards);
  for (board = blist.first(); board; board = blist.next()) {
    if (!can_read(ch, board))
      continue;
    note_nav nlist(board->get_notes());
    for (note = nlist.first(); note; note = nlist.next()) {
      if (note->get_modified() >= ch->pcdata->pfile->last_on)
        count++;
    }
  }

  if(count > 0) {
    char tmp[256];
    sprintf(tmp, "%s note%s been posted since last login.", number_word(count), count == 1 ? " has" : "s have" );
    *tmp = toupper(*tmp);
    send_centered(ch, tmp);
  }

  return count;
}

note_data::note_data()
{
  record_new( sizeof( note_data ), MEM_NOTE );

  board = NULL;
  author = NULL;
  modified = 0;
  title = NULL;
  original = NULL;
}

noteboard_data::noteboard_data()
{
  record_new( sizeof( noteboard_data), MEM_NOTE );

  filename = NULL;
  name = NULL;
  iflag_level = 1;
  lvl_read = 0;
  lvl_remove = 0;
  lvl_write = 0;
  note_size = 0;
}

void noteboard_data::rename( const char* new_name, const char* new_filename )
{
  rename_file( BOARD_DIR, filename, BOARD_DIR, new_filename );

  set_name( new_name );
  set_filename( new_filename );

  save_noteboard( this );
}


//////// BELOW CODE IS SELT-OLC-DATABASE FUNCTIONS ////////

noteboard_data *load_help_noteboard(const char *filename, const char *name)
{
  FILE *fp = open_file(filename, "rb");

  if (!fp)
    return NULL;

  if (strcmp(fread_word(fp), "#HELPS")) {
    fclose(fp);
    return NULL;
  }

  noteboard_data *board = new noteboard_data(name, filename, LEVEL_APPRENTICE, LEVEL_APPRENTICE, LEVEL_IMMORTAL, 0, 4096);
  char tmp[MAX_STRING_LENGTH];
  selt_string temp;

  for(;;) {
    int level[2];
    level[0] = fread_number(fp);
    level[1] = fread_number(fp);
    int cat = fread_number(fp);
    char *name = fread_string( fp, MEM_HELP );
    tmp[0] = '\0';
    temp = "";

    if (*name == '$') {
      free_string(name, MEM_HELP);
      break;
    }

    char *text = fread_string(fp, MEM_HELP);
    char *immortal = fread_string(fp, MEM_HELP);

    note_data *note = new note_data();
    note->set_title(name);
    note->set_modified(time(0));
    note->set_author("Helps");

    permission_flags.sprint( tmp, level );
    temp._printf("#level %s\r\n#category %s\r\n#name %s\r\n#text\r\n\r\n", tmp, help_cat_table[cat].name, name);

    note->set_text(temp);
    note->get_text()->add_text(text);

    if (immortal && *immortal) {
      note->get_text()->add_text("\r\n#immortal\r\n\r\n");
      note->get_text()->add_text(immortal);
    }
    
    board->add_note(note);

    free_string(name, MEM_HELP);
    free_string(text, MEM_HELP);
    free_string(immortal, MEM_HELP);
  }

  fclose( fp );
  return board;
}


noteboard_data *old_load_noteboard(const char *filename, const char *name)
{
  FILE *fp = open_file(filename, "rb");

  if (!fp)
    return NULL;

  if (strcmp(fread_word(fp), "#NOTES")) {
    fclose(fp);
    return NULL;
  }

  noteboard_data *board = new noteboard_data(name, filename, LEVEL_APPRENTICE, LEVEL_APPRENTICE, LEVEL_IMMORTAL, 0, 4096);

  for(;;) {
    char *title = fread_string(fp, MEM_NOTE);
    if (*title == '$') {
      free_string(title, MEM_NOTE);
      break;
    }

    char *from = fread_string(fp, MEM_NOTE);
    char *message = fread_string(fp, MEM_NOTE);
    int date = fread_number(fp);

    note_data *note = new note_data();
    note->set_title(title);
    note->set_author(from);
    note->set_text(message);
    note->set_modified(date);
    
    board->add_note(note);

    free_string(title, MEM_NOTE);
    free_string(from, MEM_NOTE);
    free_string(message, MEM_NOTE);
  }

  fclose( fp );
  return board;
}


// load all global noteboards from the board directory
void load_notes( )
{
  selt_string filespec;
  filelist list;

  filespec.printf( "%s*%s", BOARD_DIR, BOARD_SUF );
  dirlist( &list, filespec );

  echo( "Loading Noteboards ...\r\n" );

  while( !list.empty ) {
    char *filename = list.headitem( );
    list.remove( filename );

    filespec = BOARD_DIR;
    filespec += filename;
    selt_string tempname;
    tempname = "old_";
    tempname += filename;

    noteboard_data *board;
    // try to boot old noteboard first
    if( ( board = old_load_noteboard( filespec, tempname ) ) ) {
      log( "... booted old style noteboard, updating." );
      save_noteboard( board );
      // noteboards.add( board );
    } else if( ( board = load_help_noteboard( filespec, filename ) ) ) {
      log( "... booted helpfile as noteboard, updating." );
      save_noteboard( board );
      // noteboards.add( board );
    } else {    
      noteboard_data *board = load_noteboard( filename );
      if( board )
        noteboards.add( board );
      else
        log( "SYSERR: Noteboard '%s' did not boot correctly.", filename );
    }
    delete [] filename;
  }
}

// save all global noteboards
void save_all_noteboards( )
{
  noteboard_data *board = NULL;

  if( noteboards.empty )
    return;

  board_nav blist( &noteboards );
  for( board = blist.first( ); board; board = blist.next( ) )
    save_noteboard( board );
}

// Board Saver  'Front-End'
bool save_noteboard( noteboard_data *board )
{
  if( board == NULL )
    return FALSE;

  selt_string filename;
  filename.printf( "%s%s", BOARD_DIR, board->get_filename( ) );

  sdb_output *ss = new sdb_output( filename );

  bool success = ss->save( board );
  delete ss;  // Killing the sdb_output closes the file and cleans up.
  return success;
}

// Board Loader  'Front End'
noteboard_data *load_noteboard( const char *filename )
{
  noteboard_data *board = NULL;

  if( filename == NULL || *filename == '\0' )
    return NULL;

  selt_string tfilename;
  tfilename.printf( "%s%s", BOARD_DIR, filename );

  sdb_input *ss = new sdb_input( tfilename );

  int type = ss->get_type( );

  if( type == SDB_NOTEBOARD ) {
    board = (noteboard_data *) ss->get_data( );
    if( board )
      board->set_filename( filename );
  } else if( type == SDB_UNDEFINED ) {
    log( "File '%s' probably doesn't exist.  Board not loaded.", tfilename.get_text( ) );

//    log("File '%s' is SDB_UNDEFINED.  Trying to boot old noteboard format.", tfilename);
//    board = old_load_noteboard(filename);
//    if (!board)
//    else
//      save_noteboard(board);
  } else {
    log( "File '%s' is masquerading as a board file (appears to be of SDB type: '%s')", tfilename.get_text( ), lookup( ss->get_type( ), &sdb_types[ 0 ] ) );
  }

  delete ss;  // Killing the sdb_input closes the file and cleans up.
  return board;
}


//// SDB OUTPUT FUNCTIONS ////
// noteboard saver
bool sdb_output::save( noteboard_data *board )
{
  bool success = TRUE;

  // mark new record
  if (begin(lookup(SDB_NOTEBOARD, &sdb_types[0])) == FALSE)
    success = FALSE;

  // save data
  if (board->get_name())
    if (save("Name", board->get_name()) == FALSE)
      success = FALSE;

  if (save("IflagLevel", board->iflag_level) == FALSE)
    success = FALSE;
  if (save("LvlRead", board->get_lvl_read()) == FALSE)
    success = FALSE;
  if (save("LvlRemove", board->lvl_remove) == FALSE)
    success = FALSE;
  if (save("LvlWrite", board->lvl_write) == FALSE)
    success = FALSE;
  if (save("Size", board->note_size) == FALSE)
    success = FALSE;

  note_nav list(board->get_notes());
  for (note_data *note = list.first(); note; note = list.next())
    if (save(note) == FALSE)
      success = FALSE;

  // end record
  end();

  return success;
}

// note saver
bool sdb_output::save(const char *tag, note_data *note)
{
  bool success = TRUE;

  if( !note )
    return false;

  // mark new record
  if( !tag ) {
    if( begin( lookup( SDB_NOTE, &sdb_types[ 0 ] ) ) == FALSE )
      success = FALSE;
  } else {
    if( begin( lookup( SDB_NOTE, &sdb_types[ 0 ] ), tag ) == FALSE)
      success = FALSE;
  }

  // save data
  if( note->get_title( ) )
    if( save( "Title", note->get_title( ) ) == FALSE )
      success = FALSE;
  if( note->get_author( ) )
    if( save( "Author", note->get_author( ) ) == FALSE )
      success = FALSE;
  if( save( "Date", note->modified ) == FALSE )
    success = FALSE;
  if( save( "Text", note->get_text( ) ) == FALSE )
    success = FALSE;

  // end record
  end( );

  return success;
}


bool sdb_output::save(note_data *note)
{
  return save(NULL, note);
}

//// SDB INPUT EXTENSIONS ////
// noteboard parser
bool sdb_input::parse(noteboard_data *board, int datatype, const char *tag, void *data)
{
  switch (datatype) {
  case SDB_INT: {
    int value = (int) data;
    if (tag && !str_cmp(tag, "IflagLevel")) {
      board->iflag_level = value;
    } else if (tag && !str_cmp(tag, "LvlRead")) {
      board->lvl_read = value;
    } else if (tag && !str_cmp(tag, "LvlRemove")) {
      board->lvl_remove = value;
    } else if (tag && !str_cmp(tag, "LvlWrite")) {
      board->lvl_write = value;
    } else if (tag && !str_cmp(tag, "Size")) {
      board->note_size = value;
    } else {
      log("SDB_INPUT: parse_noteboard doesn't know what to do with the (%s) tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
                } break;

  case SDB_STRING:
    if (tag && !str_cmp(tag, "Name")) {
      board->set_name( (char*) data );
      delete [] data;
    } else {
      log("SDB_INPUT: parse_noteboard doesn't know what to do with the (%s) tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
    break;

  case SDB_NOTE: {
    note_data *note = (note_data *) data;
    if (note) {
      board->add_note(note);
    } else {
      log("SDB_INPUT: parse_noteboard didn't find the note supposedly passed to it.");
      return FALSE;
    }
                 } break;

  case SDB_END:
    return verify(board);

  default:
    log("SDB_INPUT: parse_noteboard doesn't know what to do with the %s datatype.", lookup(datatype, &sdb_types[0]));
    return FALSE;
  }

  return TRUE;
}

// note parser
bool sdb_input::parse(note_data *note, int datatype, const char *tag, void *data)
{
  switch (datatype) {
  case SDB_INT: {
    int value = (int) data;
    if (tag && !str_cmp(tag, "Date")) {
      note->modified = value;
    } else {
      log("SDB_INPUT: parse_note doesn't know what to do with the (%s) tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
                } break;

  case SDB_STRING:
    if (tag && !str_cmp(tag, "Title")) {
      note->set_title( (char *) data );
      delete [] data;
    } else if (tag && !str_cmp(tag, "Author")) {
      note->set_author( (char *) data );
      delete [] data;
    } else if (tag && !str_cmp(tag, "Text")) {
      note->set_text((char *) data);
      delete [] data;  // free memory because we converted it
    } else {
      log("SDB_INPUT: parse_note doesn't know what to do with the (%s) tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
    break;

  case SDB_TEXTBLOCK:
    if (tag && !str_cmp(tag, "Text")) {
      note->set_text((textblock *) data);
      delete (textblock *) data; // no leaks
    } else {
      log("SDB_INPUT: parse_room doesn't know what to do with the (%s) tag.", (tag ? tag : "<NULL>"));
      return FALSE;
    }
    break;

  case SDB_END:
    return verify(note);

  default:
    log("SDB_INPUT: parse_note doesn't know what to do with the %s datatype.", lookup(datatype, &sdb_types[0]));
    return FALSE;
  }

  return TRUE;
}

bool verify(noteboard_data *board)
{
  if (!board)
    return FALSE;

  if (!board->get_name())
    return FALSE;

  return TRUE;
}

bool verify(note_data *note)
{
  if (!note)
    return FALSE;

  if (!note->get_title())
    return FALSE;

  if (!note->get_author())
    return FALSE;

  return TRUE;
}
