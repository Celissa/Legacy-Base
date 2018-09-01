#include "system.h"


shop_data*  shop_list     = NULL;
const char* shop_flags [] = { "buys.stolen", "no.melt", "vegeterian",
  "butcher" };

const int ingot_vnum[] = { 638, 639, 640, 641, 643, 642 };


/*
 *   LOCAL FUNCTIONS
 */


bool        will_trade       ( char_data*, obj_data*, bool = FALSE );
int         get_cost         ( char_data*, char_data*, obj_data*, bool );
int         melt_cost        ( char_data*, obj_data* );
int         repair_cost      ( char_data*, obj_data*, char_data* = NULL, bool = FALSE );
int         get_current_cost ( char_data*, obj_data* );

/*
 *   SHOP OBJET PROPERTIES
 */


bool Obj_Data :: Damaged( )
{
  return( condition < 9*repair_condition( this )/10 );
};


/*
 *   UPDATE ROUTINE
 */


void shop_update( void )
{
  mob_data*    keeper;
  obj_data*       obj;

  for( int i = 0; i < mob_list; i++ ) {
    keeper = mob_list[i];

    if( !keeper->Is_Valid() || keeper->pShop == 0 || keeper->reset == 0 || player_in_room( keeper->in_room ) )
      continue;

    reset_shop( keeper );

    for( int j = keeper->contents-1; j >= 0; j-- ) {
      obj = (obj_data*) keeper->contents[j];
      for( int k = obj->number; k > 0; k-- ) 
        if( number_range( 0, 10 ) == 0 && --obj->number == 0 )
          obj->Extract( );
    } 
  }
}


/*
 *   COST ROUTINES
 */


bool will_trade( char_data* keeper, obj_data* obj, bool repair )
{
  if( obj->pIndexData->materials != 0 && keeper->pShop->materials && obj->pIndexData->materials == 0 )
    return FALSE;
  
  if( is_set( obj->extra_flags, OFLAG_NO_SELL ) && !repair)
    return FALSE;

  return is_set( keeper->pShop->buy_type, obj->pIndexData->item_type );
}

int get_current_cost( char_data* ch, obj_data* obj )
{
  int        total  = 0;
  int         cost  = 0;

  if( obj == NULL || is_set( obj->extra_flags, OFLAG_NO_SELL ) || ch->get_skill( SKILL_APPRAISE ) < MAX_SKILL_LEVEL/2 )
    return 0;

  cost = obj->Cost( );
  
  if( obj->pIndexData->item_type == ITEM_ARMOR || obj->pIndexData->item_type == ITEM_SHIELD || obj->pIndexData->item_type == ITEM_WEAPON )
    cost /= 12;
  else
    cost /= 4;
  
  cost = cost*obj->condition/obj->pIndexData->durability*sqr(4-obj->rust)/16;
  
  switch( obj->pIndexData->item_type ) {
  case ITEM_STAFF :
  case ITEM_WAND :
    if( obj->value[0] != 0 )
      cost = cost*(1+obj->value[3]/obj->value[0])/2;
    break;

  case ITEM_DRINK_CON :
    cost += obj->value[1]*liquid_table[ obj->value[2] ].cost/100;
    break;
  }    

  total = cost*4*number_range( 10*ch->get_skill( SKILL_APPRAISE )/MAX_SKILL_LEVEL, 11 )/11;

  return total;
}

int get_cost( char_data* keeper, char_data* ch, obj_data* obj, bool fBuy )
{
  obj_data*    inv;
  int        total  = 0;
  int         cost  = 0;
  int       number  = 0;

  if( obj == NULL || is_set( obj->extra_flags, OFLAG_NO_SELL ) || !will_trade( keeper, obj ) )
    return 0;

  for( int i = 0; i < keeper->contents; i++ ) {
    inv = object( keeper->contents[i] );
    if( inv->pIndexData == obj->pIndexData )
      number += inv->number;
  }

  cost = obj->Cost( );

  if( ch->species == NULL )
    cost += (1-2*fBuy)*ch->get_skill( SKILL_HAGGLE )*cost/(10*MAX_SKILL_LEVEL);

  ch->improve_skill( SKILL_HAGGLE );

  if( !fBuy ) {
    if( obj->pIndexData->item_type == ITEM_ARMOR || obj->pIndexData->item_type == ITEM_SHIELD || obj->pIndexData->item_type == ITEM_WEAPON )
      cost /= 12;
    else
      cost /= 4;
  }

  cost = cost*obj->condition/obj->pIndexData->durability*sqr(4-obj->rust)/16;

  switch( obj->pIndexData->item_type ) {
  case ITEM_STAFF :
  case ITEM_WAND :
    if( obj->value[0] != 0 )
      cost = cost*(1+obj->value[3]/obj->value[0])/2;
    break;

  case ITEM_DRINK_CON :
    if( fBuy )
      cost += obj->value[1]*liquid_table[ obj->value[2] ].cost/100;
    break;
  }    

  for( int i = 0; i < obj->selected; i++ )
    total += cost*12/(3+number+( fBuy ? -i : i ) );

  return total;
}


int repair_cost( char_data* keeper, obj_data* obj, char_data* ch, bool repair )
{
  obj_clss_data*   obj_clss = obj->pIndexData;
  int                  cost;

  if( obj->age < 0 )
    obj->age = 0;

  if( !will_trade( keeper, obj, repair ) || obj->pIndexData->repair > keeper->pShop->repair )
    return 0;

  cost = 50*sqr( obj_clss->repair )+obj_clss->cost*( obj_clss->durability-obj->condition )/( 2*obj_clss->durability );
  cost = cost*(20+obj->age)/20;  

  if( obj->pIndexData->item_type == ITEM_WEAPON && ch != NULL && ch->species == NULL ) {
    cost -= cost*ch->get_skill( SKILL_WEAPONSMITH )/(10*MAX_SKILL_LEVEL);
    cost = max( 1, cost );
  }

  if( ( obj->pIndexData->item_type == ITEM_ARMOR || obj_clss->item_type == ITEM_SHIELD )
    && ch != NULL && ch->species == NULL ) {
    cost -= cost*ch->get_skill( SKILL_ARMORER )/(10*MAX_SKILL_LEVEL);
    cost = max( 1, cost );
  }

  return cost;
}

/*
 *  SERVICE UPDATE FUNCTION
 */

void service_update( char_data* ch )
{
  cast_data*          cast = ch->concoct;
  int                 wait = cast->wait;
  int               potion = cast->spell;
  obj_data*        reagent = cast->reagent[wait];
  obj_data*           tool = cast->tool[wait];
  char_data*        victim = cast->customer;
  char_data*         bully = NULL;
  obj_data*            obj = NULL;
  int                spell = services_data_table[potion].spell;
  int                skill = services_data_table[potion].skill;

  if( spell_table[spell-SPELL_FIRST].type == STYPE_OBJECT || 
    spell_table[spell-SPELL_FIRST].type == STYPE_WEAPON ||
    spell_table[spell-SPELL_FIRST].type == STYPE_WEAPON_ARMOR ||
    spell_table[spell-SPELL_FIRST].type == STYPE_CORPSE ||
    spell_table[spell-SPELL_FIRST].type == STYPE_REPLICATE )
    obj = (obj_data*) cast->target;
  else
    bully = (char_data*) cast->target;

  if( victim->array != ch->array ) {
    if( victim->link != NULL ) {
      send( victim, "You hear %s shouting at you, that %s wasn't done yet.\r\n", ch, ch->He_She( ) );
      ch->concoct = NULL;
      return;
    }
  }

  if( wait < services_data_table[potion].wait ) {
    reagent  = cast->reagent[wait];
    tool     = cast->tool[wait];
    if( reagent != NULL )
      reagent->shown = 1;
    if( tool != NULL )
      tool->shown = 1;
    if( wait == 0 && *services_data_table[potion].rmsg1 != '\0' )
      act_room( ch->in_room, services_data_table[potion].rmsg1, ch, reagent, VIS_SIGHT );
    else if( wait == 1 && *services_data_table[potion].rmsg2 != '\0' )
      act_room( ch->in_room, services_data_table[potion].rmsg2, ch, reagent, VIS_SIGHT );
    else if( wait == 2 && *services_data_table[potion].rmsg3 != '\0' ) 
      act_room( ch->in_room, services_data_table[potion].rmsg3, ch, reagent, VIS_SIGHT );
    else if( wait == 3 && *services_data_table[potion].rmsg4 != '\0' ) 
      act_room( ch->in_room, services_data_table[potion].rmsg4, ch, reagent, VIS_SIGHT );
    else if( wait == 4 && *services_data_table[potion].rmsg5 != '\0' ) 
      act_room( ch->in_room, services_data_table[potion].rmsg5, ch, reagent, VIS_SIGHT );

    cast->wait++;

    if( reagent != NULL ) {
      reagent->value[0]--;
      if( reagent->value[0] <= 0 )
        reagent->Extract( 1 );
      if( services_data_table[potion].consume_obj[wait] )
        reagent->Extract( 1 );
    }

    if( tool != NULL ) {
      tool->value[0]--;
      if( tool->value[0] <= 0 )
        reagent->Extract( 1 );
    }

    set_delay( ch, 32 );
    return;
  }

  if( bully == NULL )
    bully = cast->customer;

  if( spell >= SPELL_FIRST && spell < SPELL_MAX )
    ( spell_table[ spell - SPELL_FIRST ].function )( ch, bully, obj, skill, -1 );

  ch->concoct = NULL;
  return;
}


/*
 *   BUY FUNCTION 
 */

int find_table_entry( const char* name, int table )
{
  const char*       arg = name;
  int                 j = 0;

  if( table == TABLE_TAX ) {
    for( j = 0; j < MAX_ENTRY_TAX_DATA; j++ ) {
      const char* tax_name = tax_table[j].name;
      if( exact_match( arg, tax_name ) )
        break;
    }
  }
  else if( table == TABLE_BONUS ) {
    for( j = 0; j < MAX_ENTRY_BONUS; j++ ) {
      const char* bonus_name = bonus_table[j].entry_name;
      if( exact_match( arg, bonus_name ) )
        break;
    }
  }
  else
    bug( "Illegal call to find table entry( )" );

  return j;
}

void tax_money_delivery( int where, int amt )
{
  coin_array moni;
  room_data* room = get_room_index( where );

  if( amt <= 0 || where <= 0 )
    return;

  if( room == NULL ) {
    bug( "Invalid Room for Tax Cash" );
    return;
  }

  money_nav coins(&coinage);
  for( money_data* money = coins.first(); money; money = coins.next() ) {
    if( money->value == 1 ) {
      obj_data *obj = create( get_obj_index( money->vnum ), amt );
      money_data *temp = new money_data( obj );
      moni.add( temp );
      obj->To( room );
      consolidate( obj );
    }
  }

  while( money_data* temp = moni.headitem() ) {
    moni.remove( temp );
    delete temp;
  }

}

int add_tax( room_data* room, int cost, bool buy )
{
  const char*   tax_entry = room->area->tax_rate;
  int                   i = -1;
  int                 tax = 0;
  int             coffers = 0;

  if( tax_entry != empty_string )
    i = find_table_entry( tax_entry, TABLE_TAX );

  if( i == -1 ) {
    tax_entry = room->area->continent == OLD_CONTINENT ? "#old_continent" : "#new_continent";
    i = find_table_entry( tax_entry, TABLE_TAX );
  }

  if( i == MAX_ENTRY_TAX_DATA )
    i = find_table_entry( "#global", TABLE_TAX );

  if( i == -1 ) {
    bug( "No matching data in table Tax.Data" );;
    return 0;
  }

  tax     = cost*tax_table[i].global_tax/10000;
  coffers = tax*tax_table[i].local_tax/10000;
  
  if( buy ) {
    tax_money_delivery( tax_table[i].global_vault, tax-coffers );
    tax_money_delivery( tax_table[i].local_vault, coffers );
  }

  return tax;
}

bool buyable_pet( char_data* pet )
{
  if( pet->species == 0 || pet->leader != 0 || !is_set( pet->species->act_flags, ACT_CAN_TAME ) )
    return FALSE;

  return TRUE;
}


thing_data* cantafford( thing_data* t1, char_data* ch, thing_data* t2 )
{
  obj_data*    obj  = (obj_data*) t1;
  mob_data* keeper  = (mob_data*) t2;

  obj->temp = get_cost( keeper, ch, obj, TRUE );

  int price = obj->temp;
  int tax = add_tax( ch->in_room, price, false );
  price += tax;

  obj = remove_coins( ch, price, NULL ) ? obj : NULL;
  
  if( obj != NULL )
    add_tax( ch->in_room, price-tax, true );

  return obj;
}


thing_data* buy( thing_data* t1, char_data* ch, thing_data* )
{
  obj_data* obj = (obj_data*) t1;
 
  obj = (obj_data*) obj->From( obj->selected );   
  set_owner( obj, ch, NULL );
  obj->To( ch );

  return obj;
}


void do_buy( char_data *ch, char *argument )
{
  char             buf  [ MAX_INPUT_LENGTH ];
  char_data*    keeper;
  char_data*       pet = NULL;
  obj_data*        obj = NULL;
  room_data*      room;
  thing_array*   array;
  shop_data*      shop;
  char            arg  [ MAX_STRING_LENGTH ];
  cast_data*     cast;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  /* SERVICES SHOP */

  if( is_set( &ch->in_room->room_flags, RFLAG_SERVICE_SHOP ) ) {
    if( ch->species != NULL ) {
      send( ch, "Monsters may not buy services.\r\n" );
      return;
    }

    if( !two_argument( argument, "on", arg ) ) {
      send( ch, "Syntax: buy <service> [on] <item>.\r\n" );
      return;
    }

    for( shop = shop_list; shop != NULL; shop = shop->next ) {
      if( ch->in_room == shop->room )
        break;
    }

    int i;
    const char* arg2 = arg;
    for( i = 0; i < MAX_ENTRY_SERVICES; i++ ) {
      const char* table = services_data_table[i].display_name;
      if( exact_match( arg2, table ) )
        break;
    }

    if( i == MAX_ENTRY_SERVICES ) {
      process_ic_tell( keeper, ch, "That is not one of the services I offer.", "say" );
      return;
    }

    if( !is_set( &shop->services, i ) ) {
      process_ic_tell( keeper, ch, "That is not one of the services I offer.", "say" );
      return;
    }

    if( services_data_table[i].spell < SPELL_FIRST || services_data_table[i].spell > SPELL_MAX ) {
      send( ch, "Due to current limitations although this skill has been setup, The code is unable to support it at this time.\r\n" );
      return;
    }

    int spell = services_data_table[i].spell-SPELL_FIRST;

    cast            = new cast_data;
    cast->spell     = i;
    cast->prepare   = FALSE;
    cast->wait      = 0;
    cast->mana      = 0;                    // who cares, the mob is doing it
    cast->cast_type = UPDATE_SERVICE;
    cast->customer  = ch;   

    if( spell_table[spell].type == STYPE_OBJECT || spell_table[spell].type == STYPE_WEAPON ||
      spell_table[spell].type == STYPE_CORPSE || spell_table[spell].type == STYPE_WEAPON_ARMOR ||
      spell_table[spell].type == STYPE_REPLICATE ) {
      if( ( obj = one_object( ch, argument, services_data_table[i].display_name, &ch->contents ) ) == NULL ) {
        delete cast;
        return;
      }
      if( spell_table[spell].type == STYPE_WEAPON && obj->pIndexData->item_type != ITEM_WEAPON ) {
        send( ch, "I'm sorry but this service may only be perfomed on a weapon.\r\n" );
        delete cast;
        return;
      }
      if( spell_table[spell].type == STYPE_WEAPON_ARMOR && obj->pIndexData->item_type != ITEM_WEAPON
        && obj->pIndexData->item_type != ITEM_ARMOR ) {
        send( ch, "I'm sorry but this service can only be perfomed on a weapon or armor.\r\n" );
        delete cast;
        return;
      }
      if( spell_table[spell].type == STYPE_CORPSE && obj->pIndexData->item_type != ITEM_CORPSE ) {
        send( ch, "This service may only be performed on corpses.\r\n" );
        delete cast;
        return;
      }
      cast->target = (thing_data*) obj;
    }
    else if( spell_table[spell].type == STYPE_PEACEFUL || spell_table[spell].type == STYPE_PEACEFUL_OTHER ) {
      if( ( pet = one_character( ch, argument, "buy a service for", ch->array ) ) == NULL ) {
        delete cast;
        return;
      }
      cast->target = (thing_data*) pet;
    }

    if( pet == NULL && obj == NULL ) {
      delete cast;
      return;
    }

    if( !has_ingredients( ch, cast ) ) {
      delete cast;
      return;
    }

    if( !has_tools( ch, cast ) ) {
      delete cast;
      return;
    }
    
    int cost = services_data_table[i].cost;
    int tax = add_tax( ch->in_room, cost, false );
    cost += tax;

    sprintf( buf, "You hand %s", keeper->descr->name == NULL ? keeper->descr->name : keeper->descr->singular ); 
    if( !remove_coins( ch, cost, buf ) ) {
      send( ch, "You can't afford it.\r\n" );
      delete cast;
      return;
    }

    add_tax( ch->in_room, cost-tax, true );
    keeper->concoct = cast;

    set_delay( keeper, 32 );
    return;
  }

  /* PET SHOP */

  if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) ) {
    if( ch->species != NULL  ) {
      send( ch, "Monsters can't buy pets.\r\n" );
      return;
    }

    for( shop = shop_list; shop != NULL; shop = shop->next )
      if( ch->in_room == shop->room ) 
        break;  

    if( ( room = get_room_index( shop->PetShop ) ) == NULL ) {
      send( ch, "The pet shop is still under construction.\r\n" );
      return;
    }

    thing_array list;

    for( int i = 0; i < room->contents; i++ ) 
      if( ( pet = character( room->contents[i] ) ) != NULL && buyable_pet( pet ) )
        list += pet;

    if( ( pet = one_character( ch, argument, "buy", &list ) ) == NULL )
      return;

    if( pet->shdata->level > ch->shdata->level ) {
      send( ch, "%s is too high a level for you.\r\n", pet );
      return;
    }

    if( pet->species->price == 0 ) {
      send( ch, "That pet is not for sale until a god sets a price for it.\r\n" );
      return;
    }

    if( is_set( pet->species->act_flags, ACT_MOUNT ) ) {
      if( has_mount( ch ) )
        return;
    } else {
      if( number_of_pets( ch ) >= 2 ) {
        send( ch, "You already have two pets.\r\n" );
        return;
      }
    }
    
    int price = pet->species->price;
    int tax = add_tax( ch->in_room, price, false );
    price += tax;

    sprintf( buf, "You hand %s", keeper->descr->name == NULL ? keeper->descr->name : keeper->descr->singular ); 
    if( !remove_coins( ch, price, buf ) ) {
      if( ch->shdata->level < LEVEL_APPRENTICE ) {
        send( ch, "You can't afford it.\r\n" );
        return;
      }
      send( ch, "You don't have enough gold, but it doesn't seem to matter.\r\n" );
    }

    tax = add_tax( ch->in_room, price-tax, true );

    pet->From( );

    set_bit( &pet->status, STAT_PET );
    remove_bit( &pet->status, STAT_AGGR_ALL );
    remove_bit( &pet->status, STAT_AGGR_GOOD );
    remove_bit( &pet->status, STAT_AGGR_EVIL );

    pet->To( ch->array );
    add_follower( pet, ch );

    send( ch, "Enjoy your pet.\r\n" );
    fsend_seen( ch, "%s bought %s as a pet.\r\n", ch, pet );

    if( pet->reset != NULL ) {
      pet->reset->count--;
      pet->reset = NULL;
    }
    return;
  }

  /* OBJECT SHOP */

  thing_array list;

  for( int i = 0; i < keeper->contents; i++ ) {
    obj = (obj_data*) keeper->contents[i];
    if( will_trade( keeper, obj ) )
      list += obj;
  }
    
  if( ( array = several_things( OBJ_DATA, ch, argument, "buy", &list ) ) == NULL )
    return;

  thing_array   subset  [ 4 ];
  thing_func*     func  [ 4 ]  = { heavy, many, cantafford, buy };

  sort_objects( ch, *array, keeper, 4, subset, func );
  
  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't lift" );
  page_priv( ch, &subset[1], "can't handle" );
  page_priv( ch, &subset[2], "can't afford" );
  page_publ( ch, &subset[3], "buy", keeper, "from", "for" );

  delete array;
}

void display_services( char_data* ch, char_data* keeper )
{
  shop_data*          shop  = keeper->pShop;
  bool                flag = false;
  obj_clss_data*  obj_clss  = NULL;

  if( shop->services == 0 ) {
    process_ic_tell( keeper, ch, "I can provide no services as of yet.", "say" );
    return;
  }

  send( ch, "Copper Pieces: %d\r\n\r\n", get_money( ch ) );
  send_underlined( ch, "Service                                   Cost  Ingredients\r\n" );
  
  for( int j = 0; j < MAX_ENTRY_SERVICES; j++ ) {
    flag = false;
    if( services_data_table[j].open ) {
      int cost = abs( services_data_table[j].cost );
      cost += add_tax( ch->in_room, cost, false );
//      send( ch, "%-40s%10i ", *services_data_table[j].display_name != '\0' ? services_data_table[j].display_name : services_data_table[j].entry_name, cost );
      for( int i = 0; i < MAX_CON_INGRED; i++ ) {
        obj_clss = get_obj_index( abs( services_data_table[j].ingredient[i*2] ) );
        if( obj_clss != NULL ) {
          if( !flag ) {
            send( ch, "%-40s%6i%2s%s\r\n", *services_data_table[j].display_name != '\0' ? services_data_table[j].display_name : services_data_table[j].entry_name, cost,"", name_brief( obj_clss, services_data_table[j].ingredient[i*2+1] ) );
            flag = true;
          }
          else
            send( ch, "%48s%s\r\n", "", name_brief( obj_clss, services_data_table[j].ingredient[i*2+1] ) );
        }
      }
      for( int i = 0; i < MAX_CON_INGRED; i++ ) {
        obj_clss = get_obj_index( abs( services_data_table[j].tool[i*2] ) );
        if( obj_clss != NULL ) {
          if( !flag ) {
            send( ch, "%-40s%6i%2s%6s%s\r\n", *services_data_table[j].display_name != '\0' ? services_data_table[j].display_name : services_data_table[j].entry_name, cost, "", "Tool - ", name_brief( obj_clss, services_data_table[j].ingredient[i*2+1] ) );
            flag = true;
          }
          else
            send( ch, "%48s%6s%s\r\n", "", "Tool - ", name_brief( obj_clss, services_data_table[j].ingredient[i*2+1] ) );
        }
      }
    }
  }
  return;
}

void do_list( char_data* ch, char* argument )
{
  char_data*   keeper;
  char_data*      pet;
  thing_array*  array;
  obj_data*       obj;
  room_data*     room;
  shop_data*     shop;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  if( *argument == '\0' )
    argument = "all";

  shop = keeper->pShop;

  /* SERVICES SHOP */

  if( is_set( &ch->in_room->room_flags, RFLAG_SERVICE_SHOP ) ) {
    display_services( ch, keeper );
    return;
  }   /* PET SHOP */
  else if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) ) {
    if( ( room = get_room_index( shop->PetShop ) ) == NULL ) {
      send( ch, "The pet shop is still under construction.\r\n" );
      return;
    }

    thing_array list;

    for( int i = 0; i < room->contents; i++ ) 
      if( ( pet = character( room->contents[i] ) ) != NULL && buyable_pet( pet ) )
        list += pet;

    if( is_empty( list ) ) {
      process_ic_tell( keeper, ch, "Sorry, I'm out of pets right now.", "say" );
      return;
    }

    if( ( array = several_things( CHAR_DATA, ch, argument, "list", &list ) ) == NULL )
      return;

    send( ch, "Copper Pieces: %d\r\n\r\n", get_money( ch ) );
    send_underlined( ch, "Pet                          Cost    Level\r\n" );

    for( int i = 0; i < *array; i++ ) {
      pet = (char_data*) array->list[i];
      int price = pet->species->price;
      price += add_tax( ch->in_room, price, false );
      send( ch, "%-25s%8d%8d\r\n", pet->Seen_Name( ch, 1, TRUE ), price, pet->shdata->level );
    }

    delete array;

    return;
  }

  /* OBJECT SHOP */

  thing_array list;
  in_character = FALSE;

  for( int i = 0; i < keeper->contents; i++ ) {
    obj = (obj_data*) keeper->contents[i];
    obj->selected = 1;
    if( ( obj->temp = get_cost( keeper, ch, obj, TRUE ) ) > 0 )
      list += obj;
  }

  if( is_empty( list ) ) {
    process_ic_tell( keeper, ch, "Sorry, I have nothing to sell right now.", "say" );
    return;
  }

  if( ( array = several_things( OBJ_DATA, ch, argument, "list",  &list ) ) == NULL )
    return;

  page( ch, "Copper Pieces: %d\r\n\r\n", get_money( ch ) );
  page_underlined( ch, "Item                                     Cost    Level   Number   Condition\r\n" );

  char level [ 5 ];

  include_closed = FALSE;

  for( int i = 0; i < *array; i++ ) {
    obj = (obj_data*) array->list[i];

    if( !can_use( ch, obj->pIndexData, obj ) )
      sprintf( level, "***" );
    else
      sprintf( level, "%d", obj->pIndexData->level );

    char *temp = str_dup(obj->Seen_Name( ch, 1, TRUE ));
    int price = obj->temp;
    price += add_tax( ch->in_room, price, false );
    page( ch, "%-37s%8d%8s%8d%5s%-s\r\n", truncate( temp, 37 ), price, level, obj->number, "", obj->condition_name( ch, TRUE ) );
    delete [] temp;
  }

  include_closed = TRUE;
  in_character = TRUE;
  delete array;
}


/*
 *   SELL
 */


thing_data* sold( thing_data* t1, char_data*, thing_data* )
{
  return t1;
}


thing_data* uninterested( thing_data* thing, char_data* ch,
  thing_data* keeper )
{
  obj_data* obj = (obj_data*) thing;
  char_data* keeper2 = (char_data*) keeper;

  if( obj->pIndexData->item_type == ITEM_FOOD ) {
    if( is_set( &keeper2->pShop->flags, SHOP_VEGETERIAN ) && 
      is_set( &obj->materials, MAT_FLESH ) )
      return NULL;

    if( is_set( &keeper2->pShop->flags, SHOP_BUTCHER ) && 
      !is_set( &obj->materials, MAT_FLESH ) )
      return NULL;
    }

  if( ( obj->temp = get_cost( (char_data*) keeper, ch, obj, FALSE ) ) <= 0 ) 
    return NULL;

  return obj;
}


thing_data* not_empty( thing_data* obj, char_data*, thing_data* )
{
  return( obj->contents == 0 ? obj : NULL );
}


void do_sell( char_data* ch, char* argument )
{
  char_data*   keeper;
  thing_array*  array;
  obj_data*       obj;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  if( is_set( &ch->in_room->room_flags, RFLAG_SERVICE_SHOP ) ) {
    process_ic_tell( keeper, ch, "I do not buy things, my services can be bought.", "say" );
    return;
  }

  if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) ) {
    process_ic_tell( keeper, ch, "We don't buy pets.", "say" );
    return;
  }

  if( *argument == '\0' ) {
    send( ch, "Sell what?\r\n" );
    return;
  }

  if( ( array = several_things( OBJ_DATA, ch, argument, "sell", &ch->contents ) ) == NULL ) 
    return;

  thing_array   subset  [ 5 ];
  thing_func*     func  [ 5 ]  = { cursed, stolen, uninterested, not_empty, sold };

  sort_objects( ch, *array, keeper, 5, subset, func );

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[1], "don't own" );
  page_priv( ch, &subset[2], "isn't interested in", keeper );
  page_priv( ch, &subset[0], "can't let go of" );
  page_priv( ch, &subset[3], NULL, NULL, "isn't empty", "aren't empty" );
  page_publ( ch, &subset[4], "sell", keeper, "to", "for", false );

  for( int i = 0; i < subset[4]; i++ ) {
    obj = (obj_data*) subset[4][i];
    obj = (obj_data*) obj->From( obj->selected );

    set_bit( obj->extra_flags, OFLAG_IDENTIFIED );
    set_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );

    if( obj->pIndexData->item_type == ITEM_WAND || obj->pIndexData->item_type == ITEM_STAFF ) 
      obj->value[3] = obj->pIndexData->value[3];

    free_string( obj->label, MEM_OBJECT );
    obj->label = empty_string;

    const char*     bonus_entry = ch->in_room->area->bonus;
    int  bonus = -1;
    int global = 0;
    int coins = 0;

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
      coins += obj->temp*bonus_table[bonus].bonus_gp/10000;

    obj->owner = NULL;
    add_coins( ch, obj->temp+coins );
    obj->To( keeper );
    consolidate( obj );
  }

  delete array;
}


void do_value( char_data* ch, char* argument )
{
  char           buf  [ MAX_STRING_LENGTH ];
  char_data*  keeper;
  obj_data*      obj;
  int           cost;
  int          rcost;
  int         blocks;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) ) {
    process_ic_tell( keeper, ch, "We don't buy pets or items.", "say" );
    return;
  }

  if( ( obj = one_object( ch, argument, "value", &ch->contents ) ) == NULL ) 
    return;

  if( !obj->droppable( ) ) {
    send( ch, "You can't let go of %s.\r\n", obj );
    return;
  }

  /*
  if( !obj->Belongs( ch ) ) {
    sprintf( buf, "%s is stolen so I would never buy it.",
      obj->Seen_Name( ch ) );
    process_tell( keeper, ch, buf );
    return;
    } 
  */

  cost  = get_cost( keeper, ch, obj, FALSE );
  rcost = repair_cost( keeper, obj, ch, TRUE );
 
  rcost += add_tax( ch->in_room, rcost, false );

  if( obj->Damaged( ) ) {
    if( rcost > 0 ) 
      sprintf( buf, "I see %s is damaged.  I can repair it to %s condition for %d cp %s ",
        obj->Seen_Name( ch ), obj->repair_condition_name(ch), rcost, cost > 0 ? "or" : "but" ); 
    else 
      sprintf( buf, "I see %s is damaged.  I am unable to repair it %s ",
        obj->Seen_Name( ch ), cost > 0 ? "but" : "and" );
    if( cost > 0 ) 
      sprintf( buf+strlen( buf ), "would give you %d cp for it.", cost );
    else
      strcat( buf, "am uninterested in buying it." );
  } else {
    if( cost > 0 )
      sprintf( buf, "I would pay you %d cp for %s.", cost, obj->Seen_Name( ch ) ); 
    else
      sprintf( buf, "I am uninterested in buying %s.", obj->Seen_Name( ch ) );
  }

  blocks = obj->pIndexData->blocks;
  if( ( cost = melt_cost( keeper, obj ) ) != 0 ) 
    sprintf( buf+strlen( buf ), "  I would melt it down to produce %d ingot%s for %d cp.", blocks, blocks == 1 ? "" : "s", cost );
      
  process_ic_tell( keeper, ch, buf, "say" );
}


/*
 *   REPAIR ROUTINES
 */


void do_repair( char_data* ch, char* argument )
{
  char           buf  [ MAX_INPUT_LENGTH ];
  char_data*  keeper;
  obj_data*      obj;
  int           cost;
  int           cond;

  if( *argument == '\0' ) {
    send( ch, "Repair what?\r\n" );
    return;
  }
  
  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;
  
  if( keeper->pShop->repair == 0 ) {
    process_ic_tell( keeper, ch, "Sorry - I do not repair items.", "say" );
    return;
  }
  
  if( ( obj = one_object( ch, argument, "repair", &ch->contents ) ) == NULL ) 
    return;
 
  if( !obj->droppable( ) ) {
    send( ch, "You can't let go of %s.\r\n", obj );
    return;
  }
  
  if( ( cost = repair_cost( keeper, obj, ch, true ) ) <= 0 ) {
    process_ic_tell( keeper, ch, "That isn't something I can repair.", "say" );
    return;
  }

  if( ( cond = repair_condition( obj ) ) < 0 ) {
    process_ic_tell( keeper, ch, "That item is too old to be worth repairing.", "say" );
    return;
  }

  if( !obj->Damaged( ) ) {
    process_ic_tell( keeper, ch, "That isn't damaged enough to be worth repairing.", "say" );
    return;
  }

  int tax = add_tax( ch->in_room, cost, false );
  cost += tax;

  sprintf( buf, "%s repairs. %s for you at a cost of", keeper->Name( ch ), obj->Seen_Name( ch ) );
  *buf = toupper( *buf );
  if( !remove_coins( ch, cost, buf ) ) {
    sprintf( buf, "You can't afford the cost of repairing %s.", obj->Seen_Name( ch ) );
    process_ic_tell( keeper, ch, buf, "say" );
    return;
  }

  add_tax( ch->in_room, cost-tax, true );

  if( obj->pIndexData->item_type == ITEM_WEAPON && ch->get_skill( SKILL_WEAPONSMITH ) != UNLEARNT ) {
    sprintf( buf, "Thanks for your help in the shop, come work with me anytime.\r\n" );
    process_ic_tell( keeper, ch, buf, "say" );
    ch->improve_skill( SKILL_WEAPONSMITH );
  }
  if( ( obj->pIndexData->item_type == ITEM_ARMOR
    || obj->pIndexData->item_type == ITEM_SHIELD ) && ch->get_skill( SKILL_ARMORER ) != UNLEARNT ) {
    sprintf( buf, "Thanks for your help in the shop, come work with me anytime.\r\n" );
    process_ic_tell( keeper, ch, buf, "say" );
    ch->improve_skill( SKILL_ARMORER );
  }
   
  fsend_seen( ch, "%s gets %s repaired.\r\n", ch, obj );

  obj = (obj_data*) obj->From( 1 );
  obj->age         += max( 0, int( 5-obj->condition+obj->rust*pow(cond-obj->condition, 1.5)/100 ) );
//  obj->age       += int( 1+obj->rust*pow(cond-obj->condition,1.5)/100. );
  obj->condition  = cond;

  obj->To( &ch->contents );
  consolidate( obj );
}


/*
 *   MELT FUNCTIONS
 */


int melt_cost( char_data *keeper, obj_data *obj )
{
  int metal;

  for( metal = MAT_BRONZE; metal <= MAT_GOLD; metal++ )
    if( is_set( &obj->materials, metal ) )
      break;

  if( metal > MAT_GOLD ) 
    return 0;

  if( obj->pIndexData->blocks == 0 || (keeper->pShop->materials & ( 1 << metal )) == 0 )
    return 0;

  return 100;
}


void do_melt( char_data *ch, char *argument )
{
  char           buf  [ MAX_INPUT_LENGTH ];
  char_data*  keeper;
  obj_data*      obj;
  int           cost;
  int         blocks;
  int          metal;
  int         length;

  if( ( keeper = find_keeper( ch ) ) == NULL )
    return;

  if( ( obj = one_object( ch, argument, "melt", &ch->contents ) ) == NULL ) 
    return;

  for( metal = MAT_BRONZE; metal <= MAT_PLATINUM; metal++ )
    if( is_set( &obj->materials, metal ) )
      break;

  if( metal > MAT_PLATINUM ) {
    fsend( ch, "%s is not made out of metal.", obj );
    return;
  }

  if( ( blocks = obj->pIndexData->blocks ) == 0 ) {
    fsend( ch, "%s does not contain enough metal to be worth melting.", obj );
    return;
  }

  if( !obj->droppable( ) ) {
    send( ch, "You can't let go of %s.\r\n", obj );
    return;
  }

  if( (keeper->pShop->materials & ( 1 << metal )) == 0 ) {
    process_ic_tell( keeper, ch, "Sorry, that is not made out of a metal I deal with.", "say" );
    return;
  }

  if( is_set( &keeper->pShop->flags, SHOP_NO_MELT ) ) {
    process_ic_tell( keeper, ch, "Does it look like my shop is set up for what you want?", "say" );
    return;
  }

  cost = 100;
  int tax = add_tax( ch->in_room, cost, false );
  cost += tax;

  sprintf( buf, "You hand %s to %s along with", obj->Seen_Name( ch ), keeper->Seen_Name( ch ) );
  if( !remove_coins( ch, cost, buf ) ) { 
    sprintf( buf, "You can't afford my fee of %d cp to melt %s.", cost, obj->Seen_Name( ch ) );
    process_ic_tell( keeper, ch, buf, "say" );
    return;
  }
  
  add_tax( ch->in_room, cost-tax, true );

  fsend_seen( ch, "%s has %s melted down.", ch, obj );
  sprintf( buf, "%s takes %s and places it in the furnace.  ", keeper->Seen_Name( ch ), obj->Seen_Name( ch ) );
  buf[0] = toupper( buf[0] );

  obj->Extract( 1 );

  if( ( obj = create( get_obj_index( ingot_vnum[metal-MAT_BRONZE] ) ) ) == NULL ) {
    bug( "Repair: Ingot for %s does not exist.", material_table[metal].name );
    return;
  }

  obj->number = blocks;
  obj->shown  = blocks;
  set_bit( &obj->materials, metal );

  length = strlen( buf );
  sprintf( &buf[length], "%s then pulls it out and after much hammering and reheating hands you %s.", keeper->He_She( ), obj->Seen_Name( ch, blocks ) );
  buf[length] = toupper( buf[length] );
  fsend( ch, buf );

  obj->To( &ch->contents );
}


/*
 *  ONLINE EDITING OF SHOPS
 */


void do_shedit( char_data* ch, char* argument )
{
  char                  buf  [MAX_STRING_LENGTH ];
  mob_data*          keeper;
  shop_data*           shop;
  char_data*         victim;
  species_data*     species;
  int                number;
  
  for( shop = shop_list; shop != NULL; shop = shop->next )
    if( ch->in_room == shop->room ) 
      break;  

  if( *argument != '\0' && !can_edit( ch, ch->in_room ) )
    return;

  if( exact_match( argument, "new" ) ) {
    if( shop != NULL ) {
      send( ch, "There is already a shop here.\r\n" );
      return;
      }
    shop = new shop_data;
    shop->room            = ch->in_room;
    shop->custom          = NULL;
    shop->keeper          = -1;
    shop->repair          = 0;
    shop->flags           = ( 1 << 1 );
    shop->materials       = 0;
    shop->buy_type[0]     = 0;
    shop->buy_type[1]     = 0;
    shop->PetShop         = 0;
    shop->services        = 0;
    shop->next = shop_list;
    shop_list = shop;
    send( ch, "New shop created here.\r\n" );
    return;
  }

  if( shop == NULL ) {
    send( ch, "There is no shop associated with this room.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    species = get_species( shop->keeper ); 
    sprintf( buf, "Shop Keeper: %s  [ Vnum: %d ]\r\n\r\n", ( species == NULL ? "none" : species->Name() ), shop->keeper );
    sprintf( buf+strlen( buf ), "Repair: %d\r\n\r\n", shop->repair );
    if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) )
      sprintf( buf+strlen( buf ), "Pet Locale Vnum: %i \r\n", shop->PetShop );
    send( buf, ch );
    return;
  }
  
  if( exact_match( argument, "delete" ) ) {
    remove( shop_list, shop );
    for( int i = 0; i < mob_list; i++ )
      if( mob_list[i]->Is_Valid() && mob_list[i]->pShop == shop )
        mob_list[i]->pShop = NULL;
    send( ch, "Shop deleted.\r\n" );
    return;
  }
   
  if( matches( argument, "keeper" ) ) {
    if( ( victim = one_character( ch, argument, "set keepr", ch->array ) ) == NULL )
      return;

    if( ( keeper = mob( victim ) ) == NULL ) {
      send( ch, "Players can not be shop keepers.\r\n" );
      return;
    }

    shop->keeper = keeper->species->vnum;
    keeper->pShop = shop;
    send( ch, "Shop keeper set to %s.\r\n", keeper->descr->name );
    return;
  }

  if( matches( argument, "repair" ) ) {
    if( ( number = atoi( argument ) ) < 0 || number > 10 ) {
      send( ch, "A shop's repair level must be between 0 and 10.\r\n" );
      return;
    }
    shop->repair = number;
    send( ch, "The shop's repair level is set to %d.\r\n", number );
  } 
  
  if( matches( argument, "petshop" ) ) {
    if( ( number = atoi( argument ) ) < 0 || number > ROOM_GENERATED ) {
      send( ch, "A pet shops location of pets must be between 0 and %d.\r\n", ROOM_GENERATED );
      return;
    }
    shop->PetShop = number;
    send( ch, "The pet shops location for the pets has been set to %d.\r\n", number );
  }
}


void do_shflag( char_data* ch, char* argument )
{
  shop_data*       shop;
  const char*  response;

  for( shop = shop_list; shop != NULL; shop = shop->next )
    if( ch->in_room == shop->room ) 
      break;  

  if( shop == NULL ) {
    send( ch, "This room has no shop entry.\r\n" );
    return;
    }

#define types 4

  const char*  title [types] = { "Basic", "Obj_Types", "Materials", "Services" };
  int            max [types] = { MAX_SHOP, MAX_ITEM, MAX_MATERIAL, MAX_ENTRY_SERVICES };

  const char** name1 [types] = { &shop_flags[0], &item_type_name[0],
    &material_name[0], &services_data_table[0].entry_name };
  const char** name2 [types] = { &shop_flags[1], &item_type_name[1],
    &material_name[1], &services_data_table[1].entry_name };

  int*    flag_value [types] = { &shop->flags, shop->buy_type,
    &shop->materials, &shop->services };
  int      uses_flag [types] = { 1, 1, 1, 1 };

  response = flag_handler( title, name1, name2, flag_value, max,
    uses_flag, (const char*) NULL, ch, argument, types );

#undef types

  if( response == NULL ) 
    send( ch, "No such flag.\r\n" );
  else
    room_log( ch, ch->in_room->vnum, response );

  return;
}


void do_shstat( char_data*, char* )
{
  return;
}


/*
 *   MISC SHOP ROUTINES
 */


char_data* active_shop( char_data* ch )
{
  room_data*    room  = ch->in_room;
  char_data*  keeper;

  if( !is_set( &room->room_flags, RFLAG_PET_SHOP ) && !is_set( &room->room_flags, RFLAG_SHOP ) ) 
    return NULL;

  for( int i = 0; i < room->contents; i++ )
    if( ( keeper = mob( room->contents[i] ) ) != NULL && keeper->pShop != NULL && keeper->pShop->room == room && keeper->is_awake( ) && ch->Seen( keeper ) )
      return keeper;

  return NULL;
}


char_data* find_keeper( char_data* ch )
{
  char_data* keeper = NULL;

  for( int i = 0; ; i++ ) {
    if( i >= *ch->array ) {
      if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) || is_set( &ch->in_room->room_flags, RFLAG_SHOP ) ) {
        send( ch, "The shop keeper is not around right now.\r\n" );
        return NULL;
      }
      send( ch, "You are not in a shop.\r\n" );
      return NULL;
    } 
    
    if( ( keeper = mob( ch->array->list[i] ) ) != NULL && keeper->pShop != NULL )
      break;
  }

  if( !keeper->is_awake( ) ) {
    send( ch, "The shopkeeper seems to be asleep.\r\n" );
    return NULL;
  }
   
  spoil_hide( ch );

  if( ch->shifted != NULL ) {
    send( ch, "I don't trade with animals.\r\n" );
    return NULL;
  }

  if( !ch->Seen( keeper ) && ch->shdata->level < LEVEL_APPRENTICE ) {
    do_say( keeper, "I don't trade with folks I can't see." );
    return NULL;
  }

  if( ch->species != NULL  && !is_set( ch->species->act_flags, ACT_CAN_CARRY ) ) {
    send( ch, "You can't carry anything so shopping is rather pointless.\r\n" );
    return NULL;
  }

  return keeper;
}


/*
 *   DISK ROUTINES
 */


void load_shops( void )
{
  shop_data*      shop  = NULL;
  custom_data*  custom;
  FILE*             fp;
  int             i, j;
  bool boot_old = false;
  double          version = 0;

  log( "Loading Shops...\r\n" );

  fp = open_file( SHOP_FILE, "rb" );
  
  char *word = fread_word(fp);
  if( !strcmp( word, "#VER_SHOP" ) ) {
    version = fread_number( fp );
    log( "...version %d shop file", version );
  } else if( !strcmp( word, "#SHOPS" ) ) {
    log( "...old style shops" );
  } else if (!strcmp(word, "#OLD_SHOPS")) {
    log("... old style shop file");
    boot_old = true;
  } else {
    panic( "Load_shops: header not found" );
  }

  for( ; ; ) {
    if( ( i = fread_number( fp ) ) == -1 )
      break;

    if( i == 0 ) {
      custom        = new custom_data;
      custom->item  = get_obj_index( fread_number( fp ) );
      custom->cost  = fread_number( fp );
  
      for( i = 0; i < (boot_old ? 5 : MAX_INGRED); i++ ) {
        if( ( j = fread_number( fp ) ) == 0 ) {
          fread_number( fp );
          continue;
        }
        custom->ingred[i] = get_obj_index( j );
        custom->number[i] = fread_number( fp );
      }
 
      if( custom->item == 0 ) {
        roach( "Load_Shops: Removing null custom item." );
        delete custom;
      } else if( shop == NULL ) {
        roach( "Load_Shops: Custom in null shop?" );
        delete custom;
      } else {
        custom->next = shop->custom;
        shop->custom = custom;
      }
   
      fread_to_eol( fp );
      continue;
    }

    shop = new shop_data;

    shop->keeper  = fread_number( fp );
  
    shop->flags       = fread_number( fp );
    shop->buy_type[0] = fread_number( fp );
    shop->buy_type[1] = fread_number( fp );

    if( version >= 2 )
      shop->services  = fread_number( fp );
    else
      shop->services  = 0;

    shop->repair      = fread_number( fp );
    shop->materials   = fread_number( fp );
    shop->open_hour   = fread_number( fp );
    shop->close_hour  = fread_number( fp );
    if( version > 0 )
      shop->PetShop   = fread_number( fp );
    else if( i < ROOM_GENERATED-1 )
      shop->PetShop   = i+1;
    else
      shop->PetShop   = 0;

    fread_to_eol( fp );
  
    if( ( shop->room = get_room_index( i ) ) == NULL ) {
      roach( "Load_Shops: Deleting shop in non-existent room %d.", i ); 
      delete shop;
      shop = NULL;
    } else {
      shop->next = shop_list;
      shop_list  = shop; 
    }
  }

  fclose( fp );
  return;
}


void save_shops( )
{
  shop_data*      shop;
  custom_data*  custom;
  FILE*             fp;
  int                i;

  rename_file( AREA_DIR, SHOP_FILE, PREV_DIR, SHOP_FILE );

  if( ( fp = open_file( SHOP_FILE, "wb" ) ) == NULL )
    return;

  fprintf( fp, "#VER_SHOP\n" );
  fprintf( fp, "%i\n\n", 2 );

  for( shop = shop_list; shop != NULL; shop = shop->next ) {
    fprintf( fp, "%5d %5d ", shop->room->vnum, shop->keeper );
    fprintf( fp, "%5d %5d %5d %5d ", shop->flags, shop->buy_type[0], shop->buy_type[1], shop->services );
    fprintf( fp, "%2d %5d %5d %5d %5d\n", shop->repair, shop->materials, shop->open_hour, shop->close_hour, shop->PetShop );
    for( custom = shop->custom; custom != NULL; custom = custom->next ) {
      fprintf( fp, "    0 %5d %5d ", custom->item->vnum, custom->cost );
      for( i = 0; i < MAX_INGRED; i++ )
        fprintf( fp, "%5d %2d ", ( custom->ingred[i] == NULL ? 0 : custom->ingred[i]->vnum ), custom->number[i] );
      fprintf( fp, "\n" );
    }
  }

  fprintf( fp, "-1\n\n#$\n\n" );
  fclose( fp );

  return;
}





