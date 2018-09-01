#include "system.h"


auction_array  auction_list;

void  transfer_buyer   ( auction_data* ); 
void  transfer_file    ( pfile_data*, content_array*, int );
void  return_seller    ( auction_data* );
bool  no_auction       ( obj_data* );
bool  stolen_auction   ( char_data*, obj_data* );
void  display_auction  ( player_data* );
bool  can_auction      ( char_data*, thing_array* );


/*
const char* undo_msg = "A daemon stamps up to you and hands you %s.  He\
 mutters something about making up your mind while tapping his forehead\
 and storms off.";

const char* corpse_msg = "An auction daemon runs up to you.  You hand him\
 %s and a silver coin.  He looks at the corpse, looks at you, rips the corpse\
 apart, eats it, smiles happily and disappears with your silver coin.";

const char* no_auction_msg = "An auction daemon runs up to you.  You hand\
 him a silver coin and attempt to hand him %s, but he quickly refuses with\
 some mumble about items banned by the gods.  He then disappears in cloud of\
 smoke.  Only afterwards do you realize your silver coin went with him.";

const char* distinct_msg = "Due to idiosyncrasies of the accountant daemons\
 you may only auction lots of items which contain at most 5 distinct types of\
 items and distinct is unfortunately defined by them and not by what\
 you see.";
*/
  
void do_auction( char_data* ch, char* argument )
{
  char                 arg  [ MAX_INPUT_LENGTH ];
  char                 tmp  [ SIX_LINES ];
  auction_data*    auction;
  player_data*          pc;
  thing_array*       array;
  obj_data*            obj;
  int                  bid;
  int                 slot;
  int                count; 
  int                flags  = 0;

  if( is_mob( ch ) || (pc = player( ch )) == NULL)
    return;

  if( has_permission( ch, PERM_PLAYERS ) && !get_flags( ch, argument, &flags, "c", "Auction" ) )
    return;
 
  if( !strcasecmp( argument, "on" ) || !strcasecmp( argument, "off" ) ) {
    send( ch, "See help iflag for turning on and off auction.\r\n" );
    return;
  }
  
  if( is_set( &flags, 0 ) ) {
    if( auction_list == 0 ) {
      send( ch, "The auction block is empty.\r\n" );
      return;
    }

    sprintf( tmp, "++ Auction Block cleared by %s. ++", ch->real_name( ) );
    info( tmp, 0, tmp, IFLAG_AUCTION, 1, ch );
    send( ch, "-- Auction block cleared --\r\n" );

    for( int i = 0; i < auction_list; i++ ) 
      return_seller( auction_list[i] );
    delete_list( auction_list );

    return;
  }

  if( *argument == '\0' ) {
    display_auction( pc );
    return;
  }

  if( matches( argument, "undo" ) ) {
    for ( int i = auction_list.size - 1; i >= 0; i-- ) {
      auction = auction_list[i];
      if( auction->seller == ch->pcdata->pfile && ( *argument == '\0' || atoi( argument ) == auction->slot ) ) {
        if( auction->time < 45 && ( auction->buyer != NULL || auction->deleted ) ) {
          send( ch, "That lot has been on the auction block too long to remove it.\r\n" );
          return;
        }
        in_character = false;

        fsend( ch, "You remove %s from the auction block.", list_name( ch, &auction->contents ) );
        sprintf( tmp, "%s has removed Lot #%d, %s from the auction block.", ch->real_name(), auction->slot, list_name(ch, &auction->contents) );
        info( tmp, 0, tmp, IFLAG_AUCTION, 2, ch );
        auction_list -= auction;
        in_character = true;
/*        
        if( ch->in_room != NULL && !is_set( &ch->in_room->room_flags, RFLAG_AUCTION_HOUSE ) ) {
          // return to bank
          page_priv(ch, &auction->contents, NULL, NULL, "returned to bank", "returned to bank");
          for( int i = auction->contents.size - 1; i >= 0; i-- ) {
            obj = object(auction->contents.list[i]);
            if (!obj || !obj->Is_Valid())
              continue;

            obj->From(obj->selected);
            obj->To(&pc->locker);
            consolidate(obj);
          }
        } else {*/
          // return to bank
          page_priv(ch, &auction->contents, NULL, NULL, "returned to you", "returned to you");
          for( int i = auction->contents.size - 1; i >= 0; i-- ) {
            obj = object(auction->contents.list[i]);
            if (!obj || !obj->Is_Valid())
              continue;

            obj->From(obj->selected);
            obj->To( &ch->contents );
            consolidate(obj);
          }
  //      }

        delete auction;
        return;
      }
    }
    
    send( ch, "You have no items on the auction block.\r\n" );
    return;
  }
/*
  if( ch->in_room != NULL && !is_set( &ch->in_room->room_flags, RFLAG_AUCTION_HOUSE ) ) {
    send( ch, "You must be in an auction house to place an item on the block.\r\n" );
    return;
  }
*/
  count = 0;
  for( int i = 0; i < auction_list; i++ )
    if( auction_list[i]->seller == ch->pcdata->pfile && ++count == 3 ) {
      send( ch, "You may only have three lots on the auction block at once.\r\n" );
      return;
    }

  argument = one_argument( argument, arg );

  if( ( array = several_things( OBJ_DATA, ch, arg, "auction", &ch->contents ) ) == NULL )
    return;

  if( *argument == '\0' ) {
    send( ch, "What do you want the minimum bid to be?\r\n" );
    return;
  }

  if( ( bid = atoi( argument ) ) < 1 ) {
    send( ch, "Minimum bid must be at least 1 cp.\r\n" );
    return;
  }

  if( bid > 100000 ) {
    send( ch, "The minimum bid cannot be greater than 100 pp.\r\n" );
    return;
  }

  int tax_entry;

  tax_entry = find_table_entry( "#auction", TABLE_TAX );
  bid += bid*tax_table[tax_entry].global_tax/10000;

  if( !can_auction( ch, array ) )
    return;

  /* FIND FIRST OPEN SLOT */

  slot = 1;
  for( int i = 0; ; ) {
    if( i == auction_list )
      break;
    if( auction_list[i]->slot == slot ) {
      slot++;
      i = 0;
    } else
      i++;
  }

  /* AUCTION ITEM */

  in_character = FALSE;

  if( ch->in_room != NULL && is_set( &ch->in_room->room_flags, RFLAG_AUCTION_HOUSE ) )
    fsend( ch, "You hand an auctioneer %s to sell.", list_name( ch, array ) );
  else
    fsend( ch, "An auction gremlin appears in a flash of smoke and grabs %s.  He then sprints off towards the nearest auctioneer.", list_name( ch, array ) );

  in_character = TRUE;

  auction            = new auction_data;
  auction->seller    = ch->pcdata->pfile;
  auction->buyer     = NULL;
  auction->bid       = bid;
  auction->slot      = slot;
  auction->tax_entry = tax_entry;

#ifdef _DEBUG
  auction->time    = 5;
#else
  auction->time    = 50;
#endif

  for( int i = 0; i < *array; i++ ) {
    obj = object(array->list[i]);
    if (!obj || !obj->Is_Valid())
      continue;
    obj = (obj_data *) obj->From( obj->selected );
    obj->To( auction );
  }

  in_character = FALSE;

  sprintf( tmp, "%s has placed %s on the auction block.", ch->real_name(), list_name( ch, &auction->contents ) );
  info( tmp, 0, tmp, IFLAG_AUCTION, 1, ch );

  in_character = TRUE;

  auction_list += auction;
}


/*
 *   CAN AUCTION
 */


bool can_auction( char_data* ch, thing_array* array )
{
  obj_data* obj;

  if( *array > 5 ) {
    fsend(ch, "The auctioneer refuses to auction %s, telling you that the lot contains too many items for the block.", list_name( ch, array ));

//    fsend( ch, distinct_msg );
    return FALSE;
  }

  for( int i = 0; i < *array; i++ ) {
    obj = object( array->list[i] );
 
    if( no_auction( obj ) ) {
      fsend( ch, "The gods forbid the sale of %s.", obj );
      return FALSE;
    }
  }

  return TRUE;
}


bool no_auction( obj_data* obj )
{
  obj_data* content;

  if( is_set( obj->pIndexData->extra_flags, OFLAG_NO_AUCTION ) ) 
    return TRUE;

  if( is_set( obj->extra_flags, OFLAG_NODROP ) ) 
    return TRUE;

  if( obj->pIndexData->item_type == ITEM_MONEY )
    return TRUE;
 
  if(  obj->pIndexData->item_type == ITEM_CORPSE ) 
    return TRUE;

  for( int i = 0; i < obj->contents; i++ )
    if( ( content = object( obj->contents[i] ) ) == NULL || no_auction( content ) )
      return TRUE;

  return FALSE;
}


/*
 *   DISPLAY
 */


const char* Auction_Data :: Seen_Name( char_data* ch, int, bool brief)
{
  char *tmp;
  obj_data* obj;

  tmp = static_string();

  bool first_obj = TRUE;
  for ( int i = contents.size - 1; i >= 0; i-- ) {
   obj = object(contents.list[i]);
   if ( obj == NULL ) continue;
   if ( !first_obj ) {
     strcat( tmp, ", " );  
     if ( i == 0 && contents.size > 1 ) {
       strcat( tmp, " and " );  
     }
     first_obj = FALSE;
   }
   strcat( tmp, obj->Seen_Name( ch, obj->shown, brief ) );
  }
  return tmp;
}

void display_auction( player_data* pc )
{
  char               tmp  [ TWO_LINES ];
  char         condition  [ 50 ];
  char             buyer  [ 20 ];
  char               age  [ 20 ];
  auction_data*  auction;
  obj_data*          obj;
  bool             first;

  if( is_empty( auction_list ) ) {
    send( pc, "There are no items being auctioned.\r\n" );
    return;
  }

  page( pc, "Bank Account: %d cp\r\n\r\n", pc->bank );
  page_centered( pc, "+++ The Auction Block +++" );
  page( pc, "\r\n" );
  sprintf( tmp, "%4s %3s %-42s %4s %4s %s %s %7s\r\n", "Slot", "Tme", "Item", "Buyr", "Use?", "Age", "Cnd", "Min Bid" );
  page_underlined( pc, tmp );
 
  for( int i = 0; i < auction_list; i++ ) {
    auction = auction_list[i];
    rehash( pc, auction->contents );
    first = TRUE;
    for( int j = 0; j < auction->contents; j++ ) {
      obj = object( auction->contents[j] );
      if( obj->shown > 0 ) {
        condition_abbrev( condition, obj, pc );
        age_abbrev( age, obj, pc );

        if( first ) {
          first = FALSE;
          memcpy( buyer, auction->buyer == NULL ? " -- " : auction->buyer->name, 4 );
          buyer[4] = '\0';

          char *temp = str_dup(obj->Seen_Name(NULL, obj->shown, TRUE )); // truncate can affect its 1st argument string, so dupe it
          sprintf( tmp, "%-4d %-3d %-42s %4s %4s %s %s %7d\r\n",
            auction->slot, auction->time,
            truncate(temp, 42), buyer,
            can_use( pc, obj->pIndexData, obj ) ? "yes" : "no",
            age, condition, auction->minimum_bid( ) );
          delete [] temp;
        } else {
          char *temp = str_dup(obj->Seen_Name(NULL, obj->shown, TRUE));
          sprintf( tmp, "         %-42s      %4s %s %s\r\n",
            truncate(temp, 42),
            can_use( pc, obj->pIndexData, obj ) ? "yes" : "no",
            age, condition );
          delete [] temp;
        }
        
        page( pc, tmp );
      }
    }
  }
}


/*
 *   BID
 */


void do_bid( char_data* ch, char* argument )
{
  char               arg  [ MAX_INPUT_LENGTH ];
  char               tmp  [ TWO_LINES ];
  player_data*        pc;
  auction_data*  auction  = NULL;
  int                bid;
  int            min_bid;
  int               slot;
  int              proxy;
  in_character = FALSE;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

/*
  if( ch->in_room != NULL && !is_set( &ch->in_room->room_flags, RFLAG_AUCTION_HOUSE ) ) {
    send( ch, "There is no auction house here.\r\n" );
    return;
  }
*/

  if( *argument == '\0' ) {
    send( "Syntax: bid <slot> <price>\r\n", ch );
    return;
  }

  argument = one_argument( argument, arg );
  slot     = atoi( arg );

  for( int i = 0; i < auction_list; i++ ) 
    if( ( auction = auction_list[i] )->slot == slot )
      break;
    
  if( auction == NULL ) {
    send( ch, "There is no lot with slot %d on the auction block.\r\n", slot );
    return;
  }

  if( auction->seller == ch->pcdata->pfile ) {
    send( ch, "You can't bid on your own item!\r\n" );
    return;
  }

  argument = one_argument( argument, arg );

  bid     = atoi( arg );
  proxy   = atoi( argument );
  min_bid = auction->minimum_bid( );

  bid = max( bid, min( min_bid, proxy ) );

  if( bid < min_bid ) {
    fsend( ch, "The minimum bid for %s is %d.\r\n", list_name( ch, &auction->contents ), min_bid );
    in_character = TRUE;
    return;
  }

  if( bid > 3*min_bid && bid > 500 ) {
    send( ch, "To protect you from yourself you may not bid more than the greater\r\nof 3 times the current minimum bid and 500 cp.\r\n" );
    in_character = TRUE;
    return;
  }

  if( max( bid, proxy ) > free_balance( pc, auction ) ) {
    send( ch, "The bid is not accepted due to insufficent funds in your bank account.\r\n" );
    in_character = TRUE;
    return;
  }

  if( *argument != '\0' && proxy <= bid ) {
    send( ch, "A proxy only makes sense if greater than the bid.\r\n" );
    in_character = TRUE;
    return;
  }

  if( auction->buyer == ch->pcdata->pfile ) {
    bid = max( bid, proxy );
    if( auction->proxy > 0 ) 
      fsend( ch, "You change the proxy on %s from %d to %d cp.", list_name(ch, &auction->contents), auction->proxy, bid );
    else
      fsend( ch, "You add a proxy on %s of %d cp.", list_name(ch, &auction->contents), bid );
    auction->proxy = bid;
    in_character = TRUE;
    return;
  }

  bid = max( bid, min( auction->proxy, proxy ) );

  if( bid < auction->proxy  ) {
    sprintf( tmp, "You bid %d cp for %s, but it is immediately matched by %s.\r\n", bid, list_name(ch, &auction->contents), auction->buyer->name );
    fsend( ch, tmp );
    sprintf( tmp, "%s bids %d cp on Lot #%d, %s.", ch->real_name(), bid, auction->slot, list_name(ch, &auction->contents) );
    info( tmp, 0, tmp, IFLAG_AUCTION, 3, ch );
    sprintf( tmp, "The bid is matched by %s.", auction->buyer->name );
    info( tmp, 0, tmp, IFLAG_AUCTION, 3, ch );
    auction->bid = bid;
    add_time( auction );
    in_character = TRUE;
    return;
  } 

  if( proxy > bid ) 
    fsend( ch, "You bid %d cp for %s and will automatically match bids on it up to %d cp.\r\n", bid, list_name(ch, &auction->contents), proxy );
  else
    send( ch, "You bid %d cp for %s.\r\n", bid, list_name(ch, &auction->contents) );

  sprintf( tmp, "%s bids %d cp on Lot #%d, %s.", ch->real_name(), bid, auction->slot, list_name(ch, &auction->contents) );
  info( tmp, 0, tmp, IFLAG_AUCTION, 3, ch );

  auction->bid     = bid;
  auction->buyer   = ch->pcdata->pfile;
  auction->deleted = FALSE;
  auction->proxy   = proxy;
  
  add_time( auction );
  in_character = TRUE;
}


const char* delivery_msg = "An auction gremlin  runs up and hands you %s.  He mumbles\
 something about %d cp and raiding your bank account and sprints off.";
/*
const char* return_msg = "A daemon runs up and drops %s at your feet.\
 He then marches off without a word.";
*/
const char* floor_msg = "An auction gremlin runs up to you and attempts to hand %s\
 to you but realizes you are unable to carry %s.  He snickers rudely and drops\
 the delivery on the floor and sprints off.";


void auction_update( )
{
  auction_data*  auction;
  
  in_character = FALSE;

  int i;
  for ( i = auction_list.size - 1; i >= 0; i-- ) {
    auction = auction_list[i];
    if( --auction->time == 0 ) {
      auction_list -= auction;
      if( auction->buyer == NULL && !auction->deleted ) 
        return_seller( auction );
      else
        transfer_buyer( auction );
      delete auction;
    }
  }
  in_character = TRUE;
}


int free_balance( player_data* player, auction_data* replace )
{
  int              credit  = player->bank;
  auction_data*   auction;

  for( int i = 0; i < auction_list; i++ ) {
    auction = auction_list[i];
    if( auction->buyer == player->pcdata->pfile && auction != replace ) 
       credit -= max( auction->bid, auction->proxy );
  }

  return credit;
}


void auction_message( char_data* ch )
{
  char   tmp   [ TWO_LINES ];
  int      i = auction_list.size;
  if ( i != 0 ) {
     sprintf( tmp, "There %s %s lot%s on the auction block.",
              i == 1 ? "is" : "are", number_word( i ),
              i == 1 ? "" : "s" ); 
     send_centered( ch, tmp );
  }
}


/*
 *   TRANSFERING OF OBJECT/MONEY
 */


void transfer_file(pfile_data* pfile, content_array* goods, int amount)
{
  player_data*   player;
  int           connect;
  obj_data*         obj = NULL;
  int                 i;

  link_nav links( &link_list );
  for( link_data* link = links.first(); link != NULL; link = links.next() ) {
    if( ( player = link->player ) != NULL && player->pcdata->pfile == pfile ) {
      if( goods != NULL ) {
        for( i = goods->size - 1; i >= 0; i-- ) {
          if( ( obj = object( goods->list[i] ) ) != NULL ) {
            obj->From( obj->selected );
            obj->To( &player->contents );
          }
        }
      }
      
      player->bank   += amount;
      connect         = link->connected;
      link->connected = CON_PLAYING;
      write( player );
      link->connected = connect;
      return;
    }
  }

  link_data* link = new link_data;

  if( !load_char( link, pfile->name, PLAYER_DIR ) ) {
    bug( "Transfer_File: Non-existent player file." ); 
    if( goods != NULL )
      extract( *goods );
    delete link;
    return;
  }

  player           = link->player;
  link->connected  = CON_PLAYING;
  player->bank    += amount;

  if( goods != NULL ) {
    for( i = goods->size - 1; i >= 0 ; i-- ) {
      obj = object( goods->list[i] );
      if( obj != NULL && obj->Is_Valid( ) ) {
        obj->From( obj->selected );
        obj->To( &player->contents );
      }
    }
  }

  write( player );
  player->Extract( );

  delete link;
}


void return_seller( auction_data* auction )
{
  char             tmp  [ SIX_LINES ];
  player_data*      pc;
  obj_data*        obj;
  in_character = FALSE;

  if( auction->seller == NULL ) {
    sprintf( tmp, "Lot #%d, %s returned to the estate of a deceased character.", auction->slot, list_name( NULL, &auction->contents ) );
    info( tmp, 0, tmp, IFLAG_AUCTION, 3 );
    extract( auction->contents );
    in_character = TRUE;
    return;
  }

  pc = find_player( auction->seller );
 
  sprintf( tmp, "Lot #%d, %s, returned to %s.", auction->slot, list_name( pc, &auction->contents ), auction->seller->name );
  info( tmp, 0, tmp, IFLAG_AUCTION, 3, pc );

  fsend(pc, "Lot #%d, failed to sell.", auction->slot);
 
  if( pc != NULL ) {
/*    if( pc->in_room != NULL && !is_set( &pc->in_room->room_flags, RFLAG_AUCTION_HOUSE ) ) {
      // return to bank
      page_priv(pc, &auction->contents, NULL, NULL, "has been returned to your bank", "auction items returned to your bank");
      for( int i = auction->contents.size - 1; i >= 0; i-- ) {
        obj = object(auction->contents.list[i]);
        if (!obj || !obj->Is_Valid())
          continue;

        obj->From(obj->selected);
        obj->To(&pc->locker);
        consolidate(obj);
      }
    } else {*/
      // return to bank
      page_priv(pc, &auction->contents, NULL, NULL, "is returned to you by an auctioneer", "returned to you by an auctioneer");
      for( int i = auction->contents.size - 1; i >= 0; i-- ) {
        obj = object(auction->contents.list[i]);
        if (!obj || !obj->Is_Valid())
          continue;

        obj->From(obj->selected);
        obj->To( &pc->contents );
        consolidate(obj);
      }
//    }
  } else {
    transfer_file( auction->seller, &auction->contents, 0 );
  }
  in_character = TRUE;
}


void transfer_buyer( auction_data* auction ) 
{
  char              tmp  [ SIX_LINES ];
  player_data*      pc;
  obj_data*        obj;

  if( auction->seller != NULL ) {
    pc = find_player( auction->seller );
    if ( pc != NULL ) {
      pc->bank += auction->bid-auction->bid*tax_table[auction->tax_entry].global_tax/10000;
    } else {
      transfer_file( auction->seller, NULL, auction->bid-auction->bid*tax_table[auction->tax_entry].global_tax/10000 );
    }
  }

  if( auction->buyer == NULL ) {
    sprintf( tmp, "Lot #%d, %s, sold to the estate of a deceased character.", auction->slot, list_name( NULL, &auction->contents ));
    info( tmp, 0, tmp, IFLAG_AUCTION, 2 );
    extract( auction->contents );
    return;
  }

  pc = find_player( auction->buyer );

  set_owner( auction->buyer, auction->contents, TRUE );

  sprintf( tmp, "Lot #%d, %s, sold to %s for %d cp.", auction->slot, list_name( pc, &auction->contents ), auction->buyer->name, auction->bid );
  info( tmp, 0, tmp, IFLAG_AUCTION, 2, pc );

  if( pc == NULL ) {
    transfer_file( auction->buyer, &auction->contents, -auction->bid );
    return;
  }

  pc->bank -= auction->bid;
  int tax      = auction->bid*tax_table[auction->tax_entry].global_tax/10000;
  int coffers  = tax*tax_table[auction->tax_entry].local_tax/10000;
  tax         -= coffers;

  tax_money_delivery( tax_table[auction->tax_entry].global_vault, tax );
  tax_money_delivery( tax_table[auction->tax_entry].local_vault, coffers );

  fsend(pc, "Lot #%d, sold to you.", auction->slot);
/*
  if( pc->in_room != NULL && !is_set( &pc->in_room->room_flags, RFLAG_AUCTION_HOUSE ) ) {
    // put in bank
    page_priv(pc, &auction->contents, NULL, NULL, "has been deposited in your bank", "auction items deposited in your bank");
    for( int i = auction->contents.size - 1; i >= 0; i-- ) {
      obj = object(auction->contents.list[i]);
      if (!obj || !obj->Is_Valid())
        continue;

      obj->From(obj->selected);
      obj->To(&pc->locker);
      consolidate(obj);
    }
  } else {
    // give to char
    page_priv(pc, &auction->contents, NULL, NULL, "is handed to you by an auctioneer", "auction items handed to you by an auctioneer");
    for( int i = auction->contents.size - 1; i >= 0; i-- ) {
      obj = object(auction->contents.list[i]);
      if (!obj || !obj->Is_Valid())
        continue;

      obj->From(obj->selected);
      obj->To( &pc->contents );
      consolidate(obj);
    }
  }
*/  

  if( can_carry( pc, &auction->contents, FALSE ) ) {
    fsend( pc, delivery_msg, list_name( pc, &auction->contents ), auction->bid );
    for (int i = auction->contents.size - 1; i >= 0; i-- ) {
      if ( (obj = object(auction->contents.list[i])) != NULL ) {
        obj->From( obj->selected );
        obj->To( &pc->contents );
      }
    }
  } else {
    fsend( pc, floor_msg, list_name( pc, &auction->contents ), (auction->contents.size > 1 ? "them" : "it") );
    for (int i = auction->contents.size - 1; i >= 0; i-- ) {
      if ( (obj = object(auction->contents.list[i])) != NULL ) {
        obj->From( obj->selected );
        obj->To( &pc->in_room->contents );
      }
    }
  }

}


void clear_auction( pfile_data* pfile )
{
  auction_data* auction;

  int i;
  for ( i = auction_list.size - 1; i >= 0; i-- ) {
    auction = auction_list[i];
    if( auction->seller == pfile )
      auction->seller = NULL;
    if( auction->buyer == pfile ) {
      auction->buyer   = NULL;
      auction->deleted = TRUE;
    }
  }

  return;
}




bool stolen_auction( char_data* ch, content_array* goods )
{
  return TRUE;

  /*
  player_data* player;
  obj_data*    obj;
  bool         status = TRUE;

  for ( int i = goods->size - 1; i >= 0; i-- ) {
    obj = object( goods->list[i] );
  
    if( obj->Belongs( ch ) ) 
      continue;

    status = FALSE;

    fsend( ch, "A mail daemon in disguise runs up to you.  You hand him\
 %s and a silver coin.  He stops and looks at it closely and then declares\
 it stolen property from %s and disappears with a mutter about returning it\
 to the true owner.", obj, obj->owner->name );

    obj = (obj_data*) obj->From( obj->selected );

    if( ( player = find_player( obj->owner ) ) != NULL ) {
      fsend( player, "A mail daemon in disguise runs up to you and drops %s\
 at your feet.  He bows deeply and then blinks out of existence.", obj );
      obj->To( player->array );
    }
    else {
      content_array temp_array;
      temp_array += obj;
      transfer_file( obj->owner, &temp_array, 0 );
    }
  }
  return status;
  */
}
