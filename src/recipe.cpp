#include "system.h"

void remove_reagent( obj_data*, char_data* );

obj_data* suitable_object( obj_clss_data* obj_clss, obj_data *obj )
{
  // sanity check, don't like invalid objects
  if( obj == NULL || !obj->Is_Valid( ) )
    return NULL;

  if( obj->pIndexData == obj_clss ) {
    // this is the reagent, yippee
    return obj;

  } else if( obj->pIndexData->item_type == ITEM_CONTAINER ) {
    // search container for reagents
    for( int k = 0; k < obj->contents; k++ ) {
      obj_data* obj2 = object( obj->contents[k] );

      // sanity check, don't like invalid objects
      if( obj2 == NULL || !obj2->Is_Valid( ) )
        continue;

      if( obj2->pIndexData == obj_clss ) {
        // this is the reagent, yippee
        return obj2;
      }
    }
  }

  return NULL;
}

void do_build( char_data* ch, char* argument ) 
{
  int                  i, j;
  int                 skill;
  recipe_data*       recipe;
//  obj_data*             obj;
//  obj_clss_data*     ingred;
  obj_clss_data*     result = NULL;

  if( *argument == '\0' ) {
    send( ch, "What do you wish to build?\r\n" );
    return;
    }

  for( i = 0; i < MAX_ENTRY_BUILD; i++ ) 
    if( ( result = get_obj_index( build_table[i].result[0] ) ) != NULL && is_name( argument, result->Keywords( ) ) )
      break;

  if( i >= MAX_ENTRY_BUILD - 1 || result == NULL) {
    send( "Whatever that is it isn't something you can build.\r\n", ch );
    return;
  } 

  recipe = &build_table[i];
 
  for( j = 0; j < 3; j++ )  
    if( ( skill = recipe->skill[j] ) >= 0 && !ch->get_skill( skill ) ) {
      send( ch, "To build %s you need to know %s.\r\n", result->Name( ), skill_table[skill].name );
      return;
    } 

  /*
  for( i = 0; i < 10; i++ ) {
    ingred = get_obj_index( recipe->ingredient[2*i] );
    if( ingred == NULL )
      continue;
    for( j = 0, obj = ch->contents; obj != NULL; obj = obj->next_content ) 
      if( obj->pIndexData == ingred && obj->wear_loc == -1 )
        j += obj->number;
    if( j == 0 ) {
      send( ch, "Building %s requires %s.\r\n", result->Name( ),
        ingred->Name( ) ); 
      return;
      }
    }
  */

  return;
}

bool has_ingredients( char_data* ch, cast_data* cast )
{
  obj_clss_data*  obj_clss;
  int               potion = cast->spell;
  int                 type = cast->cast_type;
  
  for( int i = 0; i < MAX_CON_INGRED; i++ ) {
    if( ( obj_clss = get_obj_index( abs( type == UPDATE_CONCOCT ? concoct_table[potion].ingredient[i*2] 
      : type == UPDATE_POISON ? poison_data_table[potion].ingredient[i*2] 
      : type == UPDATE_SERVICE ? services_data_table[potion].ingredient[i*2] 
      : type == UPDATE_SCRIBE ? scribe_table[potion].ingredient[i*2]
      : type == UPDATE_WAND ? create_wand_table[potion].ingredient[i*2] 
      : fletchery_table[potion].ingredient[i*2] ) ) ) == NULL )
      continue;
    cast->reagent[i] = NULL;
    //  search through the concocters inventory
    for( int j = 0; j < ch->contents; j++ ) {
      obj_data* obj = object( ch->contents[j] );
      if( obj != NULL && obj->Is_Valid( ) && obj->pIndexData == obj_clss ) {
        cast->reagent[i] = obj;
        break;
      }
    }
    //  search through the concocters worn (including containers)
    if( cast->reagent[i] == NULL ) {
      for( int j = 0; j < ch->wearing; j++ ) {
        obj_data* obj = suitable_object( obj_clss, object( ch->wearing[j] ) );
        if( obj != NULL ) {
          cast->reagent[i] = obj;
          break;
        }
      }
    }
    // search through the concoctoers inventory..again (including containers)
    if( cast->reagent[i] == NULL ) {
      for( int j = 0; j < ch->contents; j++ ) {
        obj_data* obj = suitable_object( obj_clss, object( ch->contents[j] ) );
        if( obj != NULL ) {
          cast->reagent[i] = obj;
          break;
        }
      }
    }

    // this isn't looking good for the enlightened
    if( cast->reagent[i] == NULL ) {
      if( ch->pcdata == NULL || ch->shdata->level >= LEVEL_APPRENTICE ) {
        send( ch, "You create %s.\r\n", obj_clss->Name( ) );
        obj_data* obj = create( obj_clss );
        obj->To( ch );
        cast->reagent[i] = obj;
        continue;
      }
      send( ch, "You need %s to %s %s.\r\n", obj_clss->Name( ), cast->cast_type == UPDATE_CONCOCT ? "concoct" : "craft", cast->cast_type == UPDATE_CONCOCT ? concoct_table[potion].name : poison_data_table[potion].name );
      return FALSE;
    }
  }
  return TRUE;
}

bool has_tools( char_data* ch, cast_data* cast )
{
  obj_clss_data*  obj_clss;
  int               potion = cast->spell;
  int                 type = cast->cast_type;

  for( int i = 0; i < MAX_CON_INGRED; i++ ) {
    if( ( obj_clss = get_obj_index( abs( type == UPDATE_CONCOCT ? concoct_table[potion].tool[i*2] 
      : type == UPDATE_POISON ? poison_data_table[potion].tool[i*2] 
      : type == UPDATE_SERVICE ? services_data_table[potion].tool[i*2]
      : type == UPDATE_SCRIBE ? scribe_table[potion].ingredient[i*2]
      : type == UPDATE_WAND ? create_wand_table[potion].ingredient[i*2] 
      : fletchery_table[potion].ingredient[i*2] ) ) ) == NULL )
      continue;
    cast->tool[i] = NULL;
    //  search through the concocters inventory
    for( int j = 0; j < ch->contents; j++ ) {
      obj_data* obj = object( ch->contents[j] );
      if( obj != NULL && obj->Is_Valid( ) && obj->pIndexData == obj_clss ) {
        cast->tool[i] = obj;
        break;
      }
    }
    //  search through the concocters worn (including containers)
    if( cast->reagent[i] == NULL ) {
      for( int j = 0; j < ch->wearing; j++ ) {
        obj_data* obj = suitable_object( obj_clss, object( ch->wearing[j] ) );
        if( obj != NULL ) {
          cast->tool[i] = obj;
          break;
        }
      }
    }
    // search through the concoctoers inventory..again (including containers)
    if( cast->reagent[i] == NULL ) {
      for( int j = 0; j < ch->contents; j++ ) {
        obj_data* obj = suitable_object( obj_clss, object( ch->contents[j] ) );
        if( obj != NULL ) {
          cast->tool[i] = obj;
          break;
        }
      }
    }

    // this isn't looking good for the enlightened
    if( cast->tool[i] == NULL ) {
      if( ch->pcdata == NULL || ch->shdata->level >= LEVEL_APPRENTICE ) {
        send( ch, "You create %s.\r\n", obj_clss->Name( ) );
        obj_data* obj = create( obj_clss );
        obj->To( ch );
        cast->tool[i] = obj;
        continue;
      }
      send( ch, "You need %s to %s %s.\r\n", obj_clss->Name( ), cast->cast_type == UPDATE_CONCOCT ? "concoct" : "craft", cast->cast_type == UPDATE_CONCOCT ? concoct_table[potion].name : poison_data_table[potion].name );
      return FALSE;
    }
  }
  return TRUE;
}

void poison_concoct_update( char_data* ch )
{
  cast_data*          cast = ch->concoct;
  int                 wait = cast->wait;
  int               potion = cast->spell;
  obj_data*        reagent = cast->reagent[wait];
  obj_data*           tool = cast->tool[wait];
  obj_data*            obj;
  obj_clss_data*  obj_clss;

  if( wait < poison_data_table[potion].wait ) {
    reagent  = cast->reagent[wait];
    tool     = cast->tool[wait];
    if( reagent != NULL )
      reagent->shown = 1;
    if( tool != NULL )
      tool->shown = 1;
    if( wait == 0 && poison_data_table[potion].first_message != '\0' ) {
      act( ch, poison_data_table[potion].first_message, ch, reagent );
      if( *poison_data_table[potion].first_rmessage != '\0' )
        act_notchar( poison_data_table[potion].first_rmessage, ch, NULL, reagent );
    }
    else if( wait == 1 && *poison_data_table[potion].second_message != '\0' ) {
      act( ch, poison_data_table[potion].second_message, ch, reagent );
      if( *poison_data_table[potion].second_rmessage != '\0' )
        act_notchar( poison_data_table[potion].second_rmessage, ch, NULL, reagent );
    }
    else if( wait == 2 && *poison_data_table[potion].third_message != '\0' ) {
      act( ch, poison_data_table[potion].third_message, ch, reagent );
      if( *poison_data_table[potion].third_rmessage != '\0' )
        act_notchar( poison_data_table[potion].third_rmessage, ch, NULL, reagent );
    }
    else if( wait == 3 && *poison_data_table[potion].fourth_message != '\0' ) {
      act( ch, poison_data_table[potion].fourth_message, ch, reagent );
      if( *poison_data_table[potion].fourth_rmessage != '\0' )
        act_notchar( poison_data_table[potion].fourth_rmessage, ch, NULL, reagent );
    }
    else if( wait == 4 && *poison_data_table[potion].fifth_message != '\0' ) {
      act( ch, poison_data_table[potion].fifth_message, ch, reagent );
      if( *poison_data_table[potion].fifth_rmessage != '\0' )
        act_notchar( poison_data_table[potion].fifth_rmessage, ch, NULL, reagent );
    }

    cast->wait++;

    if( reagent != NULL )
      reagent->Extract( 1 );

    int delay = 70-10*ch->get_skill( TRADE_POISON )/MAX_SKILL_LEVEL;
    set_delay( ch, delay );
    return;
  }

  if( ( obj_clss = get_obj_index( poison_data_table[potion].poison[0] ) ) == NULL ) {
    bug( "Poison item was NULL" );
    send( ch, "Poison item was NULL.\r\n" );
    return;
  }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Poison  NULL object" );
    return;
  }

  set_bit( obj->extra_flags, OFLAG_IDENTIFIED );

  send( ch, "You have created %s.\r\n", obj );
  obj->To( ch );
  
  ch->improve_skill( TRADE_POISON );

  ch->concoct = NULL;

  set_delay( ch, 2 );
}

void concoct_update( char_data* ch )
{
  cast_data*          cast = ch->concoct;
  int                 wait = cast->wait;
  int               potion = cast->spell;
  obj_data*        reagent = cast->reagent[wait];
  obj_data*            obj;
  obj_clss_data*  obj_clss;

  if( wait < concoct_table[potion].wait ) {
    reagent  = cast->reagent[wait];
    if( reagent != NULL )
      reagent->shown = 1;
    if( wait == 0 && *concoct_table[potion].first_message != '\0' ) {
      act( ch, concoct_table[potion].first_message, ch, reagent );
      if( *concoct_table[potion].first_rmessage != '\0' )
        act_notchar( concoct_table[potion].first_rmessage, ch, NULL, reagent );
    }
    else if( wait == 1 && *concoct_table[potion].second_message != '\0' ) {
      act( ch, concoct_table[potion].second_message, ch, reagent );
      if( *concoct_table[potion].second_rmessage != '\0' )
        act_notchar( concoct_table[potion].second_rmessage, ch, NULL, reagent );
    }
    else if( wait == 2 && *concoct_table[potion].third_message != '\0' ) {
      act( ch, concoct_table[potion].third_message, ch, reagent );
      if( *concoct_table[potion].third_rmessage != '\0' )
        act_notchar( concoct_table[potion].third_rmessage, ch, NULL, reagent );
    }
    else if( wait == 3 && *concoct_table[potion].fourth_message != '\0' ) {
      act( ch, concoct_table[potion].fourth_message, ch, reagent );
      if( *concoct_table[potion].fourth_rmessage != '\0' )
        act_notchar( concoct_table[potion].fourth_rmessage, ch, NULL, reagent );
    }
    else if( wait == 4 && *concoct_table[potion].fifth_message != '\0' ) {
      act( ch, concoct_table[potion].fifth_message, ch, reagent );
      if( *concoct_table[potion].fifth_rmessage != '\0' )
        act_notchar( concoct_table[potion].fifth_rmessage, ch, NULL, reagent );
    }

    cast->wait++;

    if( reagent != NULL )
      reagent->Extract( 1 );

    int delay = 70-10*ch->get_skill( TRADE_CONCOCT )/MAX_SKILL_LEVEL;
    set_delay( ch, delay );
    return;
  }

  if( ( obj_clss = get_obj_index( concoct_table[potion].poison[0] ) ) == NULL ) {
    bug( "Concoct item was NULL" );
    send( ch, "Concoct item was NULL.\r\n" );
    return;
  }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Concoct  NULL object" );
    return;
  }

  if( obj_clss->item_type == ITEM_DRINK_CON ) {
    if( ( obj->value[2] = concoct_table[potion].liquid ) == -1 )
      obj->value[2] = LIQ_WATER;
//    float contains = obj->value[0]*ch->get_skill( TRADE_CONCOCT )/MAX_SKILL_LEVEL;
    obj->value[1] = obj->value[0]*ch->get_skill( TRADE_CONCOCT )/MAX_SKILL_LEVEL;
  }

  set_bit( obj->extra_flags, OFLAG_IDENTIFIED );
  set_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );

  send( ch, "You have created %s.\r\n", obj );
  obj->To( ch );
  
  ch->improve_skill( TRADE_CONCOCT );

  ch->concoct = NULL;

  set_delay( ch, 2 );
  return;
}

void do_concoct( char_data* ch, char* argument )
{
  cast_data*           cast;
  int               potion = -1;

  if( !ch->get_skill( TRADE_CONCOCT ) ) {
    send( ch, "You don't know how to brew liquids.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    bool found = false;
    page( ch, "You can concoct these:\r\n" );
    int count = 0;
    for( int i = 0; i < MAX_ENTRY_CONCOCT; i++ ) {
      if( concoct_table[i].level <= ch->shdata->level &&
        concoct_table[i].skill <= ch->get_skill( TRADE_CONCOCT ) &&
        concoct_table[i].open ) {
        if( !found )
          found = true;
        page( ch, "%20s%s", concoct_table[i].name, ++count%3 == 0 ? "\r\n" : "" );
      }
    }
    if( count%3 != 0 )
      page( ch, "\r\n" );
    if( !found )
      page( ch, "%20s\r\n", "No potions" );
    return;
  }

  if( ch->shifted != NULL ) {
    send( ch, "You can't concoct things while in another form.\r\n" );
    return;
  }

  const char* arg = argument;

  if( MAX_ENTRY_CONCOCT == 0 ) {
    bug( "Concoct Table is empty?!?" );
    return;
  }

  for( int i = 0; i < MAX_ENTRY_CONCOCT; i++ ) {
    const char* concoction = concoct_table[i].name;
    if( exact_match( arg, concoction )
      && concoct_table[i].skill <= ch->get_skill( TRADE_CONCOCT )
      && concoct_table[i].level <= ch->shdata->level 
      && concoct_table[i].open )
      potion = i;
  }

  if( potion == -1 ) {
    send( ch, "%s can not be concocted.\r\n", argument );
    return;
  }

  cast            = new cast_data;
  cast->spell     = potion;
  cast->prepare   = FALSE;
  cast->wait      = 0;
  cast->mana      = 0;                    // this is a physical skill
  cast->cast_type = UPDATE_CONCOCT;

  if( !has_ingredients( ch, cast ) ) {
    delete cast;
    return;
  }

  if( !has_tools( ch, cast ) ) {
    delete cast;
    return;
  }

  ch->concoct   = cast;

  send( ch, "You begin concocting %s.\r\n", concoct_table[cast->spell].name );

  int delay = 70-10*ch->get_skill( TRADE_CONCOCT )/MAX_SKILL_LEVEL;
  set_delay( ch, delay );

  return;
}

void do_scribe( char_data* ch, char* argument )
{
  cast_data*           cast;
  int               potion = -1;

  if( !ch->get_skill( TRADE_SCRIBE ) ) {
    send( ch, "You don't know how scribe spells.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    bool found = false;
    page( ch, "You can scribe these:\r\n" );
    int count = 0;
    for( int i = 0; i < MAX_ENTRY_CONCOCT; i++ ) {
      if( scribe_table[i].level <= ch->shdata->level &&
        scribe_table[i].skill <= ch->get_skill( TRADE_SCRIBE ) &&
        ch->get_skill( scribe_table[i].skill_name ) >= scribe_table[i].second_skill_level &&
        scribe_table[i].open ) {
        if( !found )
          found = true;
        page( ch, "%20s%s", scribe_table[i].name, ++count%3 == 0 ? "\r\n" : "" );
      }
    }
    if( count%3 != 0 )
      page( ch, "\r\n" );
    if( !found )
      page( ch, "%20s\r\n", "No items found." );
    return;
  }

  if( ch->shifted != NULL ) {
    send( ch, "You can't scribe things while in another form.\r\n" );
    return;
  }

  const char* arg = argument;

  if( MAX_ENTRY_SCRIBE == 0 ) {
    bug( "Scribe Table is empty?!?" );
    return;
  }

  for( int i = 0; i < MAX_ENTRY_CONCOCT; i++ ) {
    const char* concoction = scribe_table[i].name;
    if( exact_match( arg, concoction )
      && scribe_table[i].skill <= ch->get_skill( TRADE_SCRIBE )
      && scribe_table[i].level <= ch->shdata->level 
      && ch->get_skill( scribe_table[i].skill_name ) >= scribe_table[i].second_skill_level
      && scribe_table[i].open )
      potion = i;
  }

  if( potion == -1 ) {
    send( ch, "%s can not be scribed.\r\n", argument );
    return;
  }

  cast            = new cast_data;
  cast->spell     = potion;
  cast->prepare   = FALSE;
  cast->wait      = 0;
  cast->mana      = 0;                    // this is a physical skill
  cast->cast_type = UPDATE_SCRIBE;

  if( !has_ingredients( ch, cast ) ) {
    delete cast;
    return;
  }

  if( !has_tools( ch, cast ) ) {
    delete cast;
    return;
  }

  ch->concoct   = cast;

  send( ch, "You begin scribing %s.\r\n", scribe_table[cast->spell].name );

  int delay = 70-10*ch->get_skill( TRADE_SCRIBE )/MAX_SKILL_LEVEL;
  set_delay( ch, delay );

  return;
}

void scribe_update( char_data* ch )
{
  cast_data*          cast = ch->concoct;
  int                 wait = cast->wait;
  int               potion = cast->spell;
  obj_data*        reagent = cast->reagent[wait];
  obj_data*           tool = cast->tool[wait];
  obj_data*            obj;
  obj_clss_data*  obj_clss;

  if( wait < scribe_table[potion].wait ) {
    reagent  = cast->reagent[wait];
    tool     = cast->tool[wait];
    if( reagent != NULL )
      reagent->shown = 1;
    if( tool != NULL )
      tool->shown = 1;
    if( wait == 0 && scribe_table[potion].first_message != '\0' ) {
      act( ch, scribe_table[potion].first_message, ch, reagent );
      if( *scribe_table[potion].first_rmessage != '\0' )
        act_notchar( scribe_table[potion].first_rmessage, ch, NULL, reagent );
    }
    else if( wait == 1 && *scribe_table[potion].second_message != '\0' ) {
      act( ch, scribe_table[potion].second_message, ch, reagent );
      if( *scribe_table[potion].second_rmessage != '\0' )
        act_notchar( scribe_table[potion].second_rmessage, ch, NULL, reagent );
    }
    else if( wait == 2 && *scribe_table[potion].third_message != '\0' ) {
      act( ch, scribe_table[potion].third_message, ch, reagent );
      if( *scribe_table[potion].third_rmessage != '\0' )
        act_notchar( scribe_table[potion].third_rmessage, ch, NULL, reagent );
    }
    else if( wait == 3 && *scribe_table[potion].fourth_message != '\0' ) {
      act( ch, scribe_table[potion].fourth_message, ch, reagent );
      if( *scribe_table[potion].fourth_rmessage != '\0' )
        act_notchar( scribe_table[potion].fourth_rmessage, ch, NULL, reagent );
    }
    else if( wait == 4 && *scribe_table[potion].fifth_message != '\0' ) {
      act( ch, scribe_table[potion].fifth_message, ch, reagent );
      if( *scribe_table[potion].fifth_rmessage != '\0' )
        act_notchar( scribe_table[potion].fifth_rmessage, ch, NULL, reagent );
    }

    cast->wait++;

    if( reagent != NULL )
      reagent->Extract( 1 );

    int delay = 70-10*ch->get_skill( TRADE_SCRIBE )/MAX_SKILL_LEVEL;
    set_delay( ch, delay );
    return;
  }

  if( ( obj_clss = get_obj_index( scribe_table[potion].poison[0] ) ) == NULL ) {
    bug( "Scribe item was NULL" );
    send( ch, "Scribe item was NULL.\r\n" );
    return;
  }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Scribe  NULL object" );
    return;
  }

  set_bit( obj->extra_flags, OFLAG_IDENTIFIED );

  send( ch, "You have created %s.\r\n", obj );
  obj->To( ch );
  
  ch->improve_skill( TRADE_SCRIBE );

  ch->concoct = NULL;

  set_delay( ch, 2 );
}

void do_empower( char_data* ch, char* argument )
{
  cast_data*           cast;
  int               potion = -1;

  if( !ch->get_skill( TRADE_CREATE_WAND ) ) {
    send( ch, "You don't know how empower objects.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    bool found = false;
    page( ch, "You can create these:\r\n" );
    int count = 0;
    for( int i = 0; i < MAX_ENTRY_CREATE_WAND; i++ ) {
      if( create_wand_table[i].level <= ch->shdata->level &&
        create_wand_table[i].skill <= ch->get_skill( TRADE_CREATE_WAND ) &&
        ch->get_skill( create_wand_table[i].skill_name ) >= create_wand_table[i].second_skill_level &&
        create_wand_table[i].open ) {
        if( !found )
          found = true;
        page( ch, "%20s%s", create_wand_table[i].name, ++count%3 == 0 ? "\r\n" : "" );
      }
    }
    if( count%3 != 0 )
      page( ch, "\r\n" );
    if( !found )
      page( ch, "%20s\r\n", "No items found." );
    return;
  }

  if( ch->shifted != NULL ) {
    send( ch, "You can't empower objects while in another form.\r\n" );
    return;
  }

  const char* arg = argument;

  if( MAX_ENTRY_CREATE_WAND == 0 ) {
    bug( "Wand Table is empty?!?" );
    return;
  }

  for( int i = 0; i < MAX_ENTRY_CREATE_WAND; i++ ) {
    const char* concoction = create_wand_table[i].name;
    if( exact_match( arg, concoction )
      && create_wand_table[i].skill <= ch->get_skill( TRADE_CREATE_WAND )
      && create_wand_table[i].level <= ch->shdata->level 
      && ch->get_skill( create_wand_table[i].skill_name ) >= create_wand_table[i].second_skill_level
      && create_wand_table[i].open )
      potion = i;
  }

  if( potion == -1 ) {
    send( ch, "%s can not be empowered into an object.\r\n", argument );
    return;
  }

  cast            = new cast_data;
  cast->spell     = potion;
  cast->prepare   = FALSE;
  cast->wait      = 0;
  cast->mana      = 0;                    // this is a physical skill
  cast->cast_type = UPDATE_WAND;

  if( !has_ingredients( ch, cast ) ) {
    delete cast;
    return;
  }

  if( !has_tools( ch, cast ) ) {
    delete cast;
    return;
  }

  ch->concoct   = cast;

  send( ch, "You begin empowering %s.\r\n", create_wand_table[cast->spell].name );

  int delay = 70-10*ch->get_skill( TRADE_CREATE_WAND )/MAX_SKILL_LEVEL;
  set_delay( ch, delay );

  return;
}

void empower_update( char_data* ch )
{
  cast_data*          cast = ch->concoct;
  int                 wait = cast->wait;
  int               potion = cast->spell;
  obj_data*        reagent = cast->reagent[wait];
  obj_data*           tool = cast->tool[wait];
  obj_data*            obj;
  obj_clss_data*  obj_clss;

  if( wait < create_wand_table[potion].wait ) {
    reagent  = cast->reagent[wait];
    tool     = cast->tool[wait];
    if( reagent != NULL )
      reagent->shown = 1;
    if( tool != NULL )
      tool->shown = 1;
    if( wait == 0 && create_wand_table[potion].first_message != '\0' ) {
      act( ch, create_wand_table[potion].first_message, ch, reagent );
      if( *create_wand_table[potion].first_rmessage != '\0' )
        act_notchar( create_wand_table[potion].first_rmessage, ch, NULL, reagent );
    }
    else if( wait == 1 && *create_wand_table[potion].second_message != '\0' ) {
      act( ch, create_wand_table[potion].second_message, ch, reagent );
      if( *create_wand_table[potion].second_rmessage != '\0' )
        act_notchar( create_wand_table[potion].second_rmessage, ch, NULL, reagent );
    }
    else if( wait == 2 && *create_wand_table[potion].third_message != '\0' ) {
      act( ch, create_wand_table[potion].third_message, ch, reagent );
      if( *create_wand_table[potion].third_rmessage != '\0' )
        act_notchar( create_wand_table[potion].third_rmessage, ch, NULL, reagent );
    }
    else if( wait == 3 && *create_wand_table[potion].fourth_message != '\0' ) {
      act( ch, create_wand_table[potion].fourth_message, ch, reagent );
      if( *create_wand_table[potion].fourth_rmessage != '\0' )
        act_notchar( create_wand_table[potion].fourth_rmessage, ch, NULL, reagent );
    }
    else if( wait == 4 && *create_wand_table[potion].fifth_message != '\0' ) {
      act( ch, create_wand_table[potion].fifth_message, ch, reagent );
      if( *create_wand_table[potion].fifth_rmessage != '\0' )
        act_notchar( create_wand_table[potion].fifth_rmessage, ch, NULL, reagent );
    }

    cast->wait++;

    if( reagent != NULL )
      reagent->Extract( 1 );

    int delay = 70-10*ch->get_skill( TRADE_CREATE_WAND )/MAX_SKILL_LEVEL;
    set_delay( ch, delay );
    return;
  }

  if( ( obj_clss = get_obj_index( create_wand_table[potion].poison[0] ) ) == NULL ) {
    bug( "Wand item was NULL" );
    send( ch, "Wand item was NULL.\r\n" );
    return;
  }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Wand  NULL object" );
    return;
  }

  set_bit( obj->extra_flags, OFLAG_IDENTIFIED );

  send( ch, "You have created %s.\r\n", obj );
  obj->To( ch );
  
  ch->improve_skill( TRADE_CREATE_WAND );

  ch->concoct = NULL;

  set_delay( ch, 2 );
}

void do_fletch( char_data* ch, char* argument )
{
  cast_data*           cast;
  int               potion = -1;

  if( !ch->get_skill( TRADE_FLETCHERY ) ) {
    send( ch, "You don't know how fletchery is performed.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    bool found = false;
    page( ch, "You can create these:\r\n" );
    int count = 0;
    for( int i = 0; i < MAX_ENTRY_FLETCHERY; i++ ) {
      if( fletchery_table[i].level <= ch->shdata->level &&
        fletchery_table[i].skill <= ch->get_skill( TRADE_FLETCHERY ) &&
        ch->get_skill( fletchery_table[i].skill_name ) >= fletchery_table[i].second_skill_level &&
        fletchery_table[i].open ) {
        if( !found )
          found = true;
        page( ch, "%20s%s", fletchery_table[i].name, ++count%3 == 0 ? "\r\n" : "" );
      }
    }
    if( count%3 != 0 )
      page( ch, "\r\n" );
    if( !found )
      page( ch, "%20s\r\n", "No items found." );
    return;
  }

  if( ch->shifted != NULL ) {
    send( ch, "You can't perform fletchery while in an animal form.\r\n" );
    return;
  }

  const char* arg = argument;

  if( MAX_ENTRY_FLETCHERY == 0 ) {
    bug( "Fletchery Table is empty?!?" );
    return;
  }

  for( int i = 0; i < MAX_ENTRY_FLETCHERY; i++ ) {
    const char* concoction = fletchery_table[i].name;
    if( exact_match( arg, concoction )
      && fletchery_table[i].skill <= ch->get_skill( TRADE_FLETCHERY )
      && fletchery_table[i].level <= ch->shdata->level 
      && ch->get_skill( fletchery_table[i].skill_name ) >= fletchery_table[i].second_skill_level
      && fletchery_table[i].open )
      potion = i;
  }

  if( potion == -1 ) {
    send( ch, "%s can not created.\r\n", argument );
    return;
  }

  cast            = new cast_data;
  cast->spell     = potion;
  cast->prepare   = FALSE;
  cast->wait      = 0;
  cast->mana      = 0;                    // this is a physical skill
  cast->cast_type = UPDATE_FLETCH;

  if( !has_ingredients( ch, cast ) ) {
    delete cast;
    return;
  }

  if( !has_tools( ch, cast ) ) {
    delete cast;
    return;
  }

  ch->concoct   = cast;

  send( ch, "You begin fletching %s.\r\n", fletchery_table[cast->spell].name );

  int delay = 70-10*ch->get_skill( TRADE_FLETCHERY )/MAX_SKILL_LEVEL;
  set_delay( ch, delay );

  return;
}

void fletch_update( char_data* ch )
{
  cast_data*          cast = ch->concoct;
  int                 wait = cast->wait;
  int               potion = cast->spell;
  obj_data*        reagent = cast->reagent[wait];
  obj_data*           tool = cast->tool[wait];
  obj_data*            obj;
  obj_clss_data*  obj_clss;

  if( wait < fletchery_table[potion].wait ) {
    reagent  = cast->reagent[wait];
    tool     = cast->tool[wait];
    if( reagent != NULL )
      reagent->shown = 1;
    if( tool != NULL )
      tool->shown = 1;
    if( wait == 0 && fletchery_table[potion].first_message != '\0' ) {
      act( ch, fletchery_table[potion].first_message, ch, reagent );
      if( *fletchery_table[potion].first_rmessage != '\0' )
        act_notchar( fletchery_table[potion].first_rmessage, ch, NULL, reagent );
    }
    else if( wait == 1 && *fletchery_table[potion].second_message != '\0' ) {
      act( ch, fletchery_table[potion].second_message, ch, reagent );
      if( *fletchery_table[potion].second_rmessage != '\0' )
        act_notchar( fletchery_table[potion].second_rmessage, ch, NULL, reagent );
    }
    else if( wait == 2 && *fletchery_table[potion].third_message != '\0' ) {
      act( ch, fletchery_table[potion].third_message, ch, reagent );
      if( *fletchery_table[potion].third_rmessage != '\0' )
        act_notchar( fletchery_table[potion].third_rmessage, ch, NULL, reagent );
    }
    else if( wait == 3 && *fletchery_table[potion].fourth_message != '\0' ) {
      act( ch, fletchery_table[potion].fourth_message, ch, reagent );
      if( *fletchery_table[potion].fourth_rmessage != '\0' )
        act_notchar( fletchery_table[potion].fourth_rmessage, ch, NULL, reagent );
    }
    else if( wait == 4 && *fletchery_table[potion].fifth_message != '\0' ) {
      act( ch, fletchery_table[potion].fifth_message, ch, reagent );
      if( *fletchery_table[potion].fifth_rmessage != '\0' )
        act_notchar( fletchery_table[potion].fifth_rmessage, ch, NULL, reagent );
    }

    cast->wait++;

    if( reagent != NULL )
      reagent->Extract( 1 );

    int delay = 70-10*ch->get_skill( TRADE_FLETCHERY )/MAX_SKILL_LEVEL;
    set_delay( ch, delay );
    return;
  }

  if( ( obj_clss = get_obj_index( fletchery_table[potion].poison[0] ) ) == NULL ) {
    bug( "Wand item was NULL" );
    send( ch, "Arrow item was NULL.\r\n" );
    return;
  }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Wand  NULL object" );
    return;
  }

  set_bit( obj->extra_flags, OFLAG_IDENTIFIED );

  send( ch, "You have created %s.\r\n", obj );
  obj->To( ch );
  
  ch->improve_skill( TRADE_FLETCHERY );

  ch->concoct = NULL;

  set_delay( ch, 2 );
}
