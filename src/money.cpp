#include "system.h"

coin_array coinage;

void update_coinage()
{
  obj_clss_data *obj_clss;

  while( money_data* temp = coinage.headitem() ) {
    coinage.remove( temp );
    delete temp;
  }

  for( int index = 0; index < MAX_OBJ_INDEX; index++ ) {
    if( ( obj_clss = obj_index_list[index] ) == NULL || obj_clss->item_type != ITEM_MONEY )
      continue;

    if( obj_clss->cost == 0 ) {
      bug( "update_coinage: money object found with no value %d.", obj_clss->vnum );
      continue;
    }

    money_data *money = new money_data;
    money->vnum = obj_clss->vnum;
    money->value = obj_clss->cost;
    money->letter = obj_clss->singular[0];
    coinage.add( money );
  }
}

Money_Data :: Money_Data()
{
  init();
}

Money_Data :: Money_Data( obj_data *o )
{
  init();
  if( o->pIndexData->item_type == ITEM_MONEY ) {
    vnum   = o->pIndexData->vnum;
    amount = o->number;
    value  = o->pIndexData->cost;
    letter = o->pIndexData->singular[0];
    obj    = o;
  }
}

coin_array* Char_Data::get_coins( void )
{
  obj_data *obj;
  consolidate( contents );

  while( money_data* temp = coins.headitem() ) {
    coins.remove( temp );
    delete temp;
  }

  for( int i = 0; i < contents; i++ ) {
    if( ( obj = object( contents[i] ) ) != NULL && obj->pIndexData->item_type == ITEM_MONEY ) {
      money_data *money = new money_data( obj );
      coins.add( money );
    }
  }

  return &coins;
}

int monetary_value( obj_data* obj )
{
  if( obj->pIndexData->item_type == ITEM_MONEY ) 
    return obj->selected * obj->pIndexData->cost;

  return 0;
}

int get_money( char_data* ch )
{
  obj_data*  obj;
  int        sum  = 0;

  for( int i = 0; i < ch->contents; i++ )
    if( ( obj = object( ch->contents[i] ) ) != NULL && obj->pIndexData->item_type == ITEM_MONEY )
      sum += obj->number * obj->pIndexData->cost;

  return sum;
}

char* coin_phrase( coin_array *c )
{
  static char   buf  [ ONE_LINE ];
  bool         flag  = FALSE;
  buf[ 0 ] = '\0';

  money_nav coins(c);
  for( money_data* money = coins.first(); money; money = coins.next() ) {
    sprintf( buf + strlen( buf ), "%s %s%d %cp", flag ? "," : "",
      ( money == c->tailitem() && flag ) ? "and " : "", money->amount, money->letter );
    flag = TRUE;
  }

  if( !flag ) 
    sprintf( buf, " none" );

  return &buf[0];
}

char* coin_phrase( char_data* ch )
{
  if (ch)
    return coin_phrase( ch->get_coins() );
  else
    return "[BUG]";
}

void add_coins( char_data* ch, int amount, const char* message )
{
  coin_array moni;

  money_nav coins(&coinage);
  for( money_data* money = coins.first(); money; money = coins.next() ) {
    int num;
    if( ( num = amount/money->value ) > 0 ) {
      amount -= num*money->value;
      obj_data *obj = create( get_obj_index( money->vnum ), num );
      money_data *temp = new money_data( obj );
      moni.add( temp );
      obj->To( ch );
      consolidate( obj );
    }
  }

  if( message != NULL ) {
    if( strstr( message, "%s" ) )
      send( ch, message, coin_phrase( &moni ) );
    else
      send( ch, "%s%s.\r\n", message, coin_phrase( &moni ) );
  }

  while( money_data* temp = moni.headitem() ) {
    moni.remove( temp );
    delete temp;
  }
}


bool remove_coins( char_data* ch, int amount, const char* message )
{
  if (!ch) {
    roach("remove_coins: null character!");
    return FALSE;
  }

  // sanity check.  free?  yeah, ch can afford that.
  if ( amount <= 0 )
    return true;

  // short-cut, don't bother calculating if they don't have the money
  if ( amount > get_money( ch ) )
    return false;

  coin_array spent;

  coin_array *c = ch->get_coins();
  money_nav coins(c);

  int sum = 0;
  for( money_data* money = coins.last(); money && amount > 0; money = coins.prev() ) {
    int num = min( money->amount, int( ceil( ( amount - sum ) / double( money->value ) ) ) );
    sum += num * money->value;

    if( money->obj ) {
      thing_data *thing_taken = money->obj->From( num );
      obj_data *obj = object( thing_taken );

      if( !obj || !obj->Is_Valid( ) )
        continue;

      money->amount -= obj->number;
      if( obj == money->obj ) {
        // given away the last of that type of coin
        money->obj = NULL;
        money->amount = 0;
      }

      spent.add( new money_data( obj ) );
    }
    
    if (sum >= amount)
      break;
  }

  if( message != NULL )
    send( ch, "%s%s.\r\n", message, coin_phrase( &spent ) );

  if( sum > amount ) 
    add_coins( ch, sum - amount, message != NULL ? "You receive%s in change.\r\n" : NULL );

  while( money_data* temp = spent.headitem() ) {
    spent.remove( temp );
    if( temp->obj )
      temp->obj->Extract( );

    delete temp;
  }

  return TRUE;
}


void do_split( char_data* ch, char* argument )
{
  int amount;

  if( *argument == '\0' ) {
    send( ch, "What amount do you wish to split?\r\n" );
    return;
  }

  amount = atoi( argument );

  if( amount < 2 ) {
    send( ch, "It is difficult to split anything less than 2 cp.\r\n" );
    return;
  }
 
  if( get_money( ch ) < amount ) {
    send( ch, "You don't have enough coins to split that amount.\r\n" );
    return;
  }

  split_money( ch, amount, TRUE );
}


void split_money( char_data* ch, int amount, bool msg )
{
  if( amount == 0 )
    return;

  char_array* array = followers( group_leader( ch ) );
  if( !array )
    return;

  int members = 1;

  for( int i = 0; i < *array; i++ ) {
    char_data *gch = array->list[i];

    if( gch == ch || !is_set( &gch->status, STAT_IN_GROUP ) || gch->in_room != ch->in_room || gch->species != NULL ) {
      *array -= gch;
      i--;
      continue;
    }
    
    members++;
  }

  if( members < 2 ) {
    if( msg )
      send( ch, "There is no one here to split the coins with.\r\n" );
  } else {
    /*
    ** New splitting algorithm:
    **   Say there are 3 other members in your group, and you type 'split 400'
    **     Try to split plats 3 ways 
    **     Try to split golds 3 ways
    **     Try to split silvers 3 ways
    **     Try to split coppers 3 ways
    **   If people didn't get enough money, then:
    **     Try to split plats 2 ways 
    **     Try to split golds 2 ways
    **     Try to split silvers 2 ways
    **     Try to split coppers 2 ways
    **   Repeat.
    **   
    */
    bool anything = FALSE;
    int split = amount / members;
    for( int n_ways = members - 1; n_ways > 0 && split > 0; n_ways-- ) {
      coin_array *c = ch->get_coins();
      coin_array coins_split;
      money_nav coins(c);
      bool found = FALSE;

      for( money_data* money = coins.first(); money && amount > 0; money = coins.next() ) {
        // Try to split this value of coins N ways
        int num_split = min( money->amount / n_ways, split / money->value );
        if( num_split > 0 ) {
          // Loop through the player list, give 'em all coins_split[i] coins.
          int j;
          char_data* gch;
          bool split_msg = 0;
          for( j = 0, gch = array->list[j]; j < n_ways && gch != NULL; j++, gch = array->list[j] ) {
            if( money->obj ) {
              thing_data *thing_taken = money->obj->From( num_split );
              obj_data *obj = object( thing_taken );

              if( !obj || !obj->Is_Valid( ) )
                continue;

              money->amount -= obj->number;
              if( obj == money->obj ) {
                // given away the last of that type of coin
                money->obj = NULL;
                money->amount = 0;
              }

              if( !split_msg ) {
                coins_split.add( new money_data( obj ) );
                split_msg = true;
              }

              obj->To( gch );
              consolidate( obj );
              found = TRUE;
            }
          }

          split -= num_split * money->value;
        }
      }
    
      if( found ) {
        char* phrase = coin_phrase( &coins_split );
        char_data* gch2;
        int j2;
        for( j2 = 0, gch2 = array->list[j2] ; j2 < n_ways && gch2 != NULL ; j2++, gch2 = array->list[j2] ) {
          send( ch, "You give%s to %s.\r\n", phrase, gch2 );
          send( gch2, "%s gives%s to you.\r\n", ch, phrase );
          send( *ch->array, "%s gives%s to %s.\r\n", ch, phrase, gch2 );
        }
        anything = TRUE;
      }
 
      while( money_data* temp = coins_split.headitem() ) {
        coins_split.remove( temp );
        delete temp;
      }
    }

    if( !anything )
      send( ch, "You lack the correct coins to split that amount.\r\n" );
  }

  if( array != NULL )
    delete array;
}




