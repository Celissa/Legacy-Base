#include "system.h"


/*
 *  GENERIC SEARCH ROUTINE
 */
          
// need to upgrade to support rtables
bool generic_search( char_data* ch, void* target, int index, arg_enum type, bool scripts )
{
  reset_data*         reset;
  species_data*     species;
  mprog_data*         mprog;
  obj_clss_data*   obj_clss;
  obj_clss_data*       tobj;
  oprog_data*         oprog;
  room_data*           room;
  action_data*       action;
  shop_data*           shop;
  custom_data*       custom;
  int i, j;
  
  bool found = false;

  // search mobs
  for( i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL ) 
      continue;

    for( reset = species->reset; reset != NULL; reset = reset->next ) {
      if( reset->target == target ) {
        page( ch, "  On %s (%d).\r\n", species->Name( ), species->vnum );
        found = true;
      }
    }

    if( type == OBJECT ) {
      tobj = ( obj_clss_data* ) target;
      if( species->corpse == tobj->vnum ) {
        page( ch, "  Corpse of %s.\r\n", species );
        found = true;
      }

    } else if( type == CHARACTER ) {
      species_data* undead = ( species_data* ) target;

      if( undead->vnum == species->adult ) {
        page( ch, "  Adult form of %s, mob #%d.\r\n", species->Name( ), species->vnum );
        found = true;
      }

      if( undead->vnum == species->zombie ) {
        page( ch, "  Zombie form of %s, mob #%d.\r\n", species->Name( ), species->vnum );
        found = true;
      }
      
      if( undead->vnum == species->skeleton ) {
        page( ch, "  Skeletal form of %s, mob #%d.\r\n", species->Name( ), species->vnum );
        found = true;
      }
    }

    if( scripts ) {
      if( search_progs( species->attack->binary, index, type ) ) {
        page( ch, "  Used in attack prog on mob #%d.\r\n", i );
        found = true;
      }
      for( j = 1, mprog = species->mprog; mprog != NULL; j++, mprog = mprog->next ) {
        if( search_progs( mprog->binary, index, type ) ) {
          page( ch, "  Used in mprog #%d on mob #%d.\r\n", j, i );
          found = true;
        }

        if( type == OBJECT ) {
          tobj = ( obj_clss_data* ) target;
          if( ( mprog->trigger == MPROG_TRIGGER_GIVE || mprog->trigger == MPROG_TRIGGER_GIVE_TYPE ) && mprog->value == tobj->vnum ) {
            page( ch, "  Trigger for mprog #%d on mob #%d.\r\n", j, i );
            found = true;
          }
        }
      }
    }
  }

  // search objs
  for( i = 0; i < MAX_OBJ_INDEX; i++ ) {
    if( ( obj_clss = obj_index_list[i] ) == NULL )
      continue;

    if( type == OBJECT ) {
      tobj = ( obj_clss_data* ) target;

      if( obj_clss->item_type == ITEM_CONTAINER && obj_clss->value[ 2 ] == tobj->vnum ) {
        page( ch, "  Unlocks object %d, %s.\r\n", obj_clss->vnum, obj_clss->Name( ) );
        found = true;
      }

      if( obj_clss->item_type == ITEM_FIRE && obj_clss->value[ 1 ] == tobj->vnum ) {
        page( ch, "  Fire object for %d, %s.\r\n", obj_clss->vnum, obj_clss->Name( ) );
        found = true;
      }

      if( obj_clss->fakes == tobj->vnum ) {
        page( ch, "  Fake for object %d, %s.\r\n", obj_clss->vnum, obj_clss->Name( ) );
        found = true;
      }

    }

    if( scripts ) {
      for( j = 1, oprog = obj_clss->oprog; oprog != NULL; j++, oprog = oprog->next ) {
        if( search_progs( oprog->binary, index, type ) ) {
          page( ch, "  Used in oprog #%d on object #%d.\r\n", j, i );
          found = true;
        }
      }
    }
  }

  // search rooms
  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) ) {
    for( room = area->room_first; room != NULL; room = room->next ) {
      for( reset = room->reset; reset != NULL; reset = reset->next ) {
        if( reset->target == target ) {
          found = true;
          page( ch, "  At %s [%d].\r\n", room->name, room->vnum );
        }
      }

      if( type == OBJECT && ( obj_clss = ( obj_clss_data* ) target )->item_type == ITEM_KEY ) {
        for( int i = 0; i < room->exits; i++ ) {
          if( room->exits[i]->key == obj_clss->vnum ) {
            page( ch, "  Unlocks %s door of room %d.\r\n", dir_table[ room->exits[i]->direction ].name, room->vnum );
            found = true;
          }
        }
      }

      if( scripts ) {
        for( j = 1, action = room->action; action != NULL; j++, action = action->next ) {
          if( action->binary != NULL && search_progs( action->binary, index, type ) ) {
            page( ch, "  Used in action #%d in room #%d.\r\n", j, room->vnum );
            found = true;
          }
        }
      }
    }
  }

  // search rtables
  for( int i = 0; i < rtable_list; i++ ) {
    for( reset = rtable_list[i]->reset; reset != NULL; reset = reset->next ) {
      if( reset->target == target ) {
        page( ch, "  In rtable %d, %s.\r\n", rtable_list[i]->vnum, rtable_list[i]->name );
        found = true;
      }
    }
  }

  if( type == CHARACTER ) {
    species = ( species_data* ) target;

    // search trainers
    for( trainer_data* trainer = trainer_list; trainer != NULL; trainer = trainer->next ) {
      if( trainer->trainer == species->vnum ) {
        if( !trainer->room )
          page( ch, "  Trainer in non-existant room.\r\n" );
        else {
          for( reset = trainer->room->reset; reset != NULL; reset = reset->next )
            if( reset->target == species && is_set( &reset->flags, RSFLAG_MOB ) )
              break;

          if( reset == NULL )
            page( ch, "  Trainer in room %d but no reset??\r\n", trainer->room->vnum ); 
          else
            page( ch, "  Trainer in room %d.\r\n", trainer->room->vnum );
        }

        found = true;
      }
    }

    // search lists
    for( i = 0; i < MAX_LIST; i++ ) {
      if( !str_cmp( list_entry[ i ][ 2 ], "M" ) ) {
        for( j = 0; j < atoi( list_entry[ i ][ 0 ] ); j++ ) {
          if( list_value[ i ][ j ] == species->vnum ) {
            page( ch, "  Included in list %s.\r\n", list_entry[ 0 ][ i ] );
            found = true;
            break;
          }
        }
      }
    }

    // search summon tables
    for( i = 0; i < MAX_ENTRY_ANIMAL_COMP; i++ ) {
      if( animal_compan_table[i].mob == species->vnum ) {
        page( ch, "  Species for animal companion [%s].\r\n", animal_compan_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_WALK_DAMNED; i++ ) {
      if( walk_damned_table[i].mob == species->vnum ) {
        page( ch, "  Species for walk of the damned [%s].\r\n", walk_damned_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_SUM_CADAVER; i++ ) {
      if( summon_cadaver_table[i].mob == species->vnum ) {
        page( ch, "  Species for summon cadaver [%s].\r\n", summon_cadaver_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_SUM_DRAGON; i++ ) {
      if( summon_dragon_table[i].mob == species->vnum ) {
        page( ch, "  Species for summon dragon [%s].\r\n", summon_dragon_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_RETURN_DEAD; i++ ) {
      if( return_dead_table[i].mob == species->vnum ) {
        page( ch, "  Species for return the dead [%s].\r\n", return_dead_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_BANE_FIEND; i++ ) {
      if( bane_table[i].mob == species->vnum ) {
        page( ch, "  Species for bane fiend [%s].r\n", bane_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_SUMMON_UNDEAD; i++ ) {
      if( summon_undead_table[i].mob == species->vnum ) {
        page( ch, "  Species for summon undead [%s].\r\n", summon_undead_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_LSSR_FAMILIAR; i++ ) {
      if( lesser_familiar_table[i].mob == species->vnum ) {
        page( ch, "  Species for lesser familiar [%s].\r\n", lesser_familiar_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_ANIMATE_CLAY; i++ ) { 
      if( animate_clay_table[i].mob == species->vnum ) {
        page( ch, "  Species for animate clay [%s].\r\n", animate_clay_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_FIND_FAMILIAR; i++ ) {
      if( find_familiar_table[i].mob == species->vnum ) {
        page( ch, "  Species for find familiar [%s].\r\n", find_familiar_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_FIND_MOUNT; i++ ) {
      if( find_mount_table[i].mob == species->vnum ) {
        page( ch, "  Species for find mount [%s].\r\n", find_mount_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_LESSER_MOUNT; i++ ) {
      if( lesser_mount_table[i].mob == species->vnum ) {
        page( ch, "  Species for lesser mount [%s].\r\n", lesser_mount_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_CONSTRUCT_GOL; i++ ) {
      if( construct_golem_table[i].mob == species->vnum ) {
        page( ch, "  Species for construct golem [%s].\r\n", construct_golem_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_REQUEST_ALLY; i++ ) {
      if( request_ally_table[i].mob == species->vnum ) {
        page( ch, "  Species for request ally [%s].\r\n", request_ally_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_CONJ_ELEMENT; i++  ) {
      if( conjure_element_table[i].mob == species->vnum ) {
        page( ch, "  Species for conjure elemental [%s].\r\n", conjure_element_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_BLOOD_FIEND; i++ ) {
      if( blood_fiend_table[i].mob == species->vnum ) {
        page( ch, "  Species for blood fiend [%s].\r\n", blood_fiend_table[i].name );
        found = true;
        break;
      }
    }

    // search shape change table

    for( i = 0; i < MAX_ENTRY_SHAPE_CHANGE; i++ ) {
      if( shape_table[i].mob == species->vnum ) {
        page( ch, "  Species for shape change [%s].\r\n", shape_table[i].name );
        found = true;
        break;
      }
    }
  } else if( type == OBJECT ) {
    tobj = ( obj_clss_data* ) target;

    // search shops
    for( shop = shop_list; shop != NULL; shop = shop->next ) {
      for( custom = shop->custom; custom != NULL; custom = custom->next ) {
        if( custom->item == target ) {
          page( ch, "  Custom in room %d.\r\n", shop->room->vnum );
          found = true;
        }
      
        for( i = 0; i < MAX_INGRED; i++ ) {
          if( custom->ingred[i] == target ) {
            page( ch, "  Ingredient for making %s in room %d.\r\n", custom->item->Name( ), shop->room->vnum );
            found = true;
          }
        }
      }
    }
  
    // search spells
    for( i = 0; i < SPELL_COUNT; i++ ) {
      for( j = 0; j < MAX_SPELL_WAIT; j++ ) {
        if( abs( spell_table[ i ].reagent[ j ] ) == tobj->vnum ) {
          page( ch, "  Reagent for spell %s.\r\n", spell_table[ i ].name );
          found = true;
          break;
        }
      }
    }

    // search lists
    for( i = 0; i < MAX_LIST; i++ ) {
      if( !str_cmp( list_entry[ i ][ 2 ], "O" ) ) {
        for( j = 0; j < atoi( list_entry[ i ][ 0 ] ); j++ ) {
          if( list_value[ i ][ j ] == tobj->vnum ) {
            page( ch, "  Included in list %s.\r\n", list_entry[ 0 ][ i ] );
            found = true;
            break;
          }
        }
      }
    }

    // search starting tables
    for( i = 0; i < MAX_ENTRY_STARTING; i++ ) {
      for( j = 0; j < 5; j++ ) {
        if( starting_table[ i ].object[ 2 * j ] == tobj->vnum ) {
          page( ch, "  Included on starting table %s.\r\n", starting_table[ i ].name );
          found = true;
          break;
        }
      }
    }

    // search conccot tables
    for( i = 0; i < MAX_ENTRY_CONCOCT; i++ ) {
      if( concoct_table[i].poison[0] == tobj->vnum ) {
        page( ch, "  Result of concoct table '%s'.\r\n", concoct_table[i].name );
        found = true;
        break;
      }
      for( j = 0; j < 5; j++ ) {
        if( concoct_table[i].ingredient[ 2*j ] == tobj->vnum ) {
          page( ch, "  Ingredient on concoct table '%s'.\r\n", concoct_table[i].name );
          found = true;
          break;
        }
      }
      for( int k = 0; k < 5; k++ ) {
        if( concoct_table[i].tool[ 2*j ] == tobj->vnum ) {
          page( ch, "  Tool on concoct table '%s'.\r\n", concoct_table[i].name );
          found = true;
          break;
        }
      }
    }

    // search poison table
    for( i = 0; i < MAX_ENTRY_POISON; i++ ) {
      if( poison_data_table[i].poison[0] == tobj->vnum ) {
        page( ch, "  Result of poison table '%s'.\r\n", poison_data_table[i].name );
        found = true;
        break;
      }
      for( j = 0; j < 5; j++ ) {
        if( poison_data_table[i].ingredient[ 2*j ] == tobj->vnum ) {
          page( ch, "  Ingredient on poison table '%s'.\r\n", poison_data_table[i].name );
          found = true;
          break;
        }
      }
      for( int j = 0; j < 5; j++ ) {
        if( poison_data_table[i].tool[ 2*j ] == tobj->vnum ) {
          page( ch, "  Tool on poison table '%s'.\r\n", poison_data_table[i].name );
          found = true;
          break;
        }
      }
    }

    // search harvest table
    for( i = 0; i < MAX_ENTRY_HARVEST; i++ ) {
      if( harvest_table[i].object[0] == tobj->vnum ) {
        page( ch, "  Result of harvest table '%s'.\r\n", harvest_table[i].name );
        found = true;
        break;
      }
    }

    //search forage table
    for( i = 0; i < MAX_ENTRY_FORAGE; i++ ) {
      if( forage_table[i].food[0] == tobj->vnum ) {
        page( ch, "  Result of forage table '%s'.\r\n", forage_table[i].name );
        found = true;
        break;
      }
    }

    // search tinder table
    for( i = 0; i < MAX_ENTRY_TINDER; i++ ) {
      if( tinder_table[i].food[0] == tobj->vnum ) {
        page( ch, "  Result of tinder table '%s'.\r\n", tinder_table[i].name );
        found = true;
        break;
      }
    }

    //search create food table
    for( i = 0; i < MAX_ENTRY_CREATE_FOOD; i++ ) {
      if( create_food_table[i].food[0] == tobj->vnum ) {
        page( ch, "  Result of create food table '%s'.\r\n", create_food_table[i].name );
        found = true;
        break;
      }
    }

    //search services table
    for( i = 0; i < MAX_ENTRY_SERVICES; i++ ) {
      for( j = 0; j < 5; j++ ) {
        if( services_data_table[i].ingredient[i*2] == tobj->vnum ) {
          page( ch, "  Ingredient of services table '%s'.\r\n", services_data_table[i].entry_name );
          found = true;
          break;
        }
      }
      for( j = 0; j < 5; j++ ) {
        if( services_data_table[i].tool[i*2] == tobj->vnum ) {
          page( ch, "  Tool of services table '%s'.\r\n", services_data_table[i].entry_name );
          found = true;
          break;
        }
      }
    }

    // search summon tables
    for( i = 0; i < MAX_ENTRY_ANIMAL_COMP; i++ ) {
      if( animal_compan_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for animal companion [%s].\r\n", animal_compan_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_WALK_DAMNED; i++ ) {
      if( walk_damned_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for walk of the damned [%s].\r\n", walk_damned_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_SUM_CADAVER; i++ ) {
      if( summon_cadaver_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for summon cadaver [%s].\r\n", summon_cadaver_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_SUM_DRAGON; i++ ) {
      if( summon_dragon_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for summon dragon [%s].\r\n", summon_dragon_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_RETURN_DEAD; i++ ) {
      if( return_dead_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for return the dead [%s].\r\n", return_dead_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_BANE_FIEND; i++ ) {
      if( bane_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for bane fiend [%s].r\n", bane_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_SUMMON_UNDEAD; i++ ) {
      if( summon_undead_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for summon undead [%s].\r\n", summon_undead_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_LSSR_FAMILIAR; i++ ) {
      if( lesser_familiar_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for lesser familiar [%s].\r\n", lesser_familiar_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_ANIMATE_CLAY; i++ ) { 
      if( animate_clay_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for animate clay [%s].\r\n", animate_clay_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_FIND_FAMILIAR; i++ ) {
      if( find_familiar_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for find familiar [%s].\r\n", find_familiar_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_FIND_MOUNT; i++ ) {
      if( find_mount_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for find mount [%s].\r\n", find_mount_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_LESSER_MOUNT; i++ ) {
      if( lesser_mount_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for lesser mount [%s].\r\n", lesser_mount_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_CONSTRUCT_GOL; i++ ) {
      if( construct_golem_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for construct golem [%s].\r\n", construct_golem_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_REQUEST_ALLY; i++ ) {
      if( request_ally_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for request ally [%s].\r\n", request_ally_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_CONJ_ELEMENT; i++  ) {
      if( conjure_element_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for conjure elemental [%s].\r\n", conjure_element_table[i].name );
        found = true;
        break;
      }
    }

    for( i = 0; i < MAX_ENTRY_BLOOD_FIEND; i++ ) {
      if( blood_fiend_table[i].reagent[0] == tobj->vnum ) {
        page( ch, "  Reagent for blood fiend [%s].\r\n", blood_fiend_table[i].name );
        found = true;
        break;
      }
    }

    // search build tables
    for( i = 0; i < MAX_ENTRY_BUILD; i++ ) {
      if( build_table[ i ].result[ 0 ] == tobj->vnum ) {
        page( ch, "  Result of build table '%s'.\r\n", build_table[ i ].name );
        found = true;
        break;
      }

      for( j = 0; j < 10; j++ ) {
        if( build_table[ i ].ingredient[ 2 * j ] == tobj->vnum ) {
          page( ch, "  Ingredient on build table '%s'.\r\n", build_table[ i ].name );
          found = true;
          break;
        }
      }
    }

  }

  return found;
}


/*
 *   RECURSIVE SEARCH FUNCTIONS
 */

bool cfunc_search( char_data* ch, cfunc* function, bool only_vars )
{
  bool found = false;
  
  for( int i = 0; i < MAX_SPECIES; i++ ) {
    species_data *species;
    if( ( species = species_list[i] ) == NULL )
      continue;

    if( search_progs( species->attack->binary, function, only_vars ) ) {
      page( ch, "  Used in attack prog on mob #%d.\r\n", i );
      found = true;
    }
  
    int j;
    mprog_data* mprog;
    for( j = 1, mprog = species->mprog; mprog != NULL; j++, mprog = mprog->next ) {
      if( search_progs( mprog->binary, function, only_vars ) ) {
        page( ch, "  Used in mprog #%d on mob #%d.\r\n", j, i );
        found = true;
      }
    }
  }
    
  for( int i = 0; i < MAX_OBJ_INDEX; i++ ) {
    obj_clss_data* obj_clss;
    if( ( obj_clss = obj_index_list[i] ) == NULL )
      continue;

    int j;
    oprog_data* oprog;
    for( j = 1, oprog = obj_clss->oprog; oprog != NULL; j++, oprog = oprog->next ) {
      if( search_progs( oprog->binary, function, only_vars ) ) {
        page( ch, "  Used in oprog #%d on object #%d.\r\n", j, i );
        found = true;
      }
    }
  }

  area_nav alist(&area_list);
  for( area_data *area = alist.first(); area; area = alist.next() ) {
    for( room_data *room = area->room_first; room != NULL; room = room->next ) {
      int j;
      action_data* action;
      for( j = 1, action = room->action; action != NULL; j++, action = action->next ) {
        if( action->binary != NULL && search_progs( action->binary, function, only_vars ) ) {
          page( ch, "  Used in action #%d in room #%d.\r\n", j, room->vnum );
          found = true;
        }
      }
    }
  }

  return found;
}


bool search_progs( arg_type* arg, int vnum, arg_enum type )
{
  aif_type*      aif;
  afunc_type*  afunc;
  int                     i;

  if( arg == NULL )
    return FALSE;
   
  if( arg->family == if_clause ) {
    aif = (aif_type*) arg;
    if( search_progs( aif->yes, vnum, type ) || search_progs( aif->no, vnum, type ) 
      || search_progs( aif->condition, vnum, type ) )
      return TRUE;
  }

  if( arg->family == function ) {
    afunc = (afunc_type*) arg;
    if( afunc->func->type == type && afunc->args[0] != NULL ) {
      if( afunc->args[0]->type == INTEGER && int( afunc->args[0]->value ) == vnum )
        return TRUE;
    }

    for( i = 0; i < afunc->args; i++ )
      if( search_progs( afunc->args[i], vnum, type ) )
        return TRUE;
  }     

  return search_progs( arg->next, vnum, type );
}


bool search_progs( arg_type* arg, cfunc* target, bool only_vars )
{
  aif_type*      aif;
  afunc_type*  afunc;
  int              i;

  if( arg == NULL )
    return FALSE;
   
  if( arg->family == if_clause ) {
    aif = (aif_type*) arg;
    if( search_progs( aif->yes, target, only_vars ) || search_progs( aif->no, target, only_vars ) 
      || search_progs( aif->condition, target, only_vars ) )
      return TRUE;
  }

  if( arg->family == function ) {
    afunc = (afunc_type*) arg;
    if( afunc->func->func_call == target && ( !only_vars || afunc->args[ 0 ]->family != constant ) )
      return TRUE;

    for( i = 0; i < afunc->args; i++ )
      if( search_progs( afunc->args[ i ], target, only_vars ) )
        return TRUE;
  }     

  return search_progs( arg->next, target, only_vars );
}


bool search_quest( arg_type* arg, int vnum )
{
  aif_type*      aif;
  afunc_type*  afunc;
  int              i;

  if( arg == NULL )
    return FALSE;

  if( arg->family == if_clause ) {
    aif = (aif_type*) arg;
    if( search_quest( aif->yes, vnum ) || search_quest( aif->no, vnum ) || search_quest( aif->condition, vnum ) )
      return TRUE;
  }

  if( arg->family == function ) {
    afunc = (afunc_type*) arg;
    if( ( afunc->func->func_call == &code_assign_quest || afunc->func->func_call == &code_update_quest 
      || afunc->func->func_call == &code_has_quest || afunc->func->func_call == &code_doing_quest
      || afunc->func->func_call == &code_done_quest ) && afunc->args[ 1 ] != NULL ) {
      if( int( afunc->args[ 1 ]->value ) == vnum )
        return TRUE;
    }
    for( i = 0; i < afunc->args; i++ )
      if( search_quest( afunc->args[ i ], vnum ) )
        return TRUE;
  }     

  return search_quest( arg->next, vnum );
}


bool search_cflag( arg_type* arg, int vnum )
{
  aif_type*      aif;
  afunc_type*  afunc;
  int              i;

  if( arg == NULL )
    return FALSE;

  if( arg->family == if_clause ) {
    aif = (aif_type*) arg;
    if( search_cflag( aif->yes, vnum ) || search_cflag( aif->no, vnum ) || search_cflag( aif->condition, vnum ) )
      return TRUE;
  }

  if( arg->family == function ) {
    afunc = (afunc_type*) arg;
    if( ( afunc->func->func_call == &code_cflag || afunc->func->func_call == &code_remove_cflag 
      || afunc->func->func_call == &code_set_cflag ) && afunc->args[ 0 ] != NULL ) {
      if( int( afunc->args[ 0 ]->value ) == vnum )
        return TRUE;
    }
    for( i = 0; i < afunc->args; i++ )
      if( search_cflag( afunc->args[ i ], vnum ) )
        return TRUE;
  }     

  return search_cflag( arg->next, vnum );
}


/*
bool search_oload( arg_type* arg, int vnum )
{
  aif_type*      aif;
  afunc_type*  afunc;
  int                     i;

  if( arg == NULL )
    return FALSE;
   
  if( arg->family == if_clause ) {
    aif = (aif_type*) arg;
    if( search_oload( aif->yes, vnum ) || search_oload( aif->no, vnum ) 
      || search_oload( aif->condition, vnum ) )
      return TRUE;
  }

  if( arg->family == function ) {
    afunc = (afunc_type*) arg;
    if( afunc->func->func_call == &code_oload && afunc->arg[0] != NULL ) {
      if( int( afunc->arg[0]->value ) == vnum )
        return TRUE;
    }
    for( i = 0; i < 4 && afunc->arg[i] != NULL; i++ )
      if( search_oload( afunc->arg[i], vnum ) )
        return TRUE;
  }     

  return search_oload( arg->next, vnum );
}


bool search_mload( arg_type *arg, int vnum )
{
  aif_type*      aif;
  afunc_type*  afunc;
  int              i;

  if( arg == NULL )
    return FALSE;
   
  if( arg->family == if_clause ) {
    aif = (aif_type*) arg;
    if( search_mload( aif->yes, vnum ) || search_mload( aif->no, vnum ) 
      || search_mload( aif->condition, vnum ) )
      return TRUE;
  }

  if( arg->family == function ) {
    afunc = (afunc_type*) arg;
    if( afunc->func->func_call == &code_mload && afunc->arg[0] != NULL ) {
      if( int( afunc->arg[0]->value ) == vnum )
        return TRUE;
    }
    for( i = 0; i < 4 && afunc->arg[i] != NULL; i++ )
      if( search_mload( afunc->arg[i], vnum ) )
        return TRUE;
  }     

  return search_mload( arg->next, vnum );
}
*/
