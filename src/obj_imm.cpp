#include "system.h"


const char* material_name [] = { "paper", "wood",
  "leather", "gemstone", "cloth", "glass", "stone", "bone", "flesh", "organic",
  "metal", "bronze", "iron", "steel", "mithril", "liridium",
  "electrum", "silver", "gold", "copper", "platinum", "krynite", "ele.fire",
  "ele.cold", "ele.electric", "ele.acid" };

const char* oflag_name [ MAX_OFLAG ] = { 
  "glow", "hums", "dark", "lock", "evil",
  "invis", "magic", "no.drop", "sanctified", "flaming",
  "backstab", "no.disarm", "no.remove", "inventory", "two.handed",
  "no.major", "no.show", "no.sacrifice", "water.proof", "appraised",
  "no.sell", "no.junk", "identified", "rust.proof", "uses.skin",
  "is.chair", "no.save", "burning", "additive", "good",
  "use.the", "replicate", "known.liquid", "poison.coated", "no.auction",
  "no.enchant", "copied", "random.metal", "covers", "#approved",
  "no.prefix", "blackjack", "diseased", "weapon.blight"
};

const char* oflag_ident [ MAX_OFLAG ] = {
  "", "", "", "", "This item has been blessed by the evil gods.",
  "", "", "Is cursed (undroppable).", "Has been sanctified.", 
  "This item has a flaming aura.", "", "This item will resist attempts at being disarmed",
  "Is cursed (noremove).", "", "This item requires two hands to use.",
  "This item can not be affected by major enchant ", "", 
  "The gods refuse the sacrifice of this item.", "Is water proof.", "",
  "", "", "", "Is rust proof.", "",
  "", "", "This item has been set on fire.", "", "This item has been blessed by the good gods.",
  "", "This item may be replicated ", "", "This has been coated by a poison.", "",
  "This item can not be affected by minor enchant ", "Is a copy.", "", "", "",
  "", "", "This item has been infected with a disease.", "This item has been blighted."
};

const char* anti_flags [ MAX_ANTI ] = {
  "anti-human", "anti-elf", "anti-gnome", "anti-dwarf", "anti-halfling",
  "anti-ent", "anti-duergar", "anti-lizardman", "anti-ogre", "anti-troll",
  "anti-orc", "anti-goblin", "anti-vyan", "anti-half-elf", "anti-mage",
  "anti-cleric", "anti-thief", "anti-warrior", "anti-paladin", "anti-ranger",
  "anti-dancer", "anti-monk", "anti-sorcerer", "anti-druid", "unused",
  "unused", "anti-good", "anti-neutral", "anti-evil", "anti-lawful",
  "anti-n2??", "anti-chaotic"
};
  
const char* size_flags [ MAX_SFLAG ] = { "custom.fit", "size.specific",
  "race.specific", "random.size", "tiny", "small", "medium", "large", "giant",
  "human", "elf", "gnome", "dwarf", "halfling", "ent", "duergar",
  "lizardman", "ogre", "troll", "orc", "goblin", "vyan", "sp.religion" };

const char* restriction_flags [ MAX_RESTRICTION ] = { "bladed",
  "no.hide", "no.sneak", "dishonorable" };

static const char* no_permission =
  "You don't have permission to alter that object.\r\n";


int   select         ( obj_clss_data*, char_data*, char* );
void  display        ( obj_clss_data*, char_data*, char*, int& );
void  oset_affect    ( char_data*, obj_clss_data*, char* );
void  oedit_replace  ( char_data*, char* );
void  oedit_renumber ( char_data*, int, int );

/*
 *   PERMISSION ROUTINE
 */

bool char_data :: can_edit( obj_clss_data* obj_clss )
{
  if( is_set( obj_clss->extra_flags, OFLAG_APPROVED ) && !has_permission( this, PERM_OBJ_APPROVAL ) ) {
    send( this, "This object is approved and may not be altered.\r\n" );
    return FALSE;
  }

  if( has_permission( this, PERM_ALL_OBJECTS ) || is_name( descr->name, obj_clss->creator ) )
    return TRUE;

  send( this, "You can not alter this object.\r\n" );
  return FALSE;
}


/*
 *   OFIND ROUTINES
 */


int select_ofind( obj_clss_data* obj, char_data* ch, char* argument )
{
  char               tmp  [ MAX_INPUT_LENGTH ];
  oprog_data*      oprog;
  char            letter;
  char            hyphen;
  const char*     string;
  bool          negative;
  int               i, j;
  int             length;
  int             an_int = 0;

#define types 8

  char flag [types+1] = "sfowirml";

  int  max [types] = { MAX_SFLAG, MAX_ENTRY_AFF_CHAR, MAX_OFLAG,
    MAX_ITEM_WEAR, MAX_ANTI, MAX_RESTRICTION, MAX_MATERIAL, MAX_LAYER };

  const char** name1 [types] = { &size_flags[0], &aff_char_table[0].name,
    &oflag_name[0], &wear_name[0], &anti_flags[0], &restriction_flags[0],
    &material_name[0], &layer_name[0] };
  const char** name2 [types] = { &size_flags[1], &aff_char_table[1].name,
    &oflag_name[1], &wear_name[1], &anti_flags[1], &restriction_flags[1],
    &material_name[1], &layer_name[1] };

  int* flag_value [types] = { &obj->size_flags, obj->affect_flags,
    obj->extra_flags, &obj->wear_flags, &obj->anti_flags,
    &obj->restrictions, &obj->materials, &obj->layer_flags };

  for( ; ; ) {
    if( ( hyphen = *argument ) == '\0' )
      return 1;

    if( hyphen != '-' ) {
      letter = 'n';
    } else {
      argument++;
      if( !isalpha( letter = *argument++ ) ) {
        send( ch, "Illegal character for flag - See help ofind.\r\n" );
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
      send( ch, "All flags require an argument - See help ofind.\r\n" );
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
    length = i;

    switch( letter ) {
      case 't' :  string = item_type_name[ obj->item_type ];     break;
      case 'c' :  string = obj->creator;                         break;
      }

    if( string != NULL ) {
      if( !strncasecmp( tmp, string, length ) == negative )
        return 0;
      continue;
    }

    if ( letter == 'C' ) { // filter by cost
      an_int = atoi(tmp);
      if( an_int != obj->cost )
        return 0;
      continue;
    }

    if ( letter == 'L' ) { // filter by level
      an_int = atoi(tmp);
      if ( an_int != obj->level )
        return 0;
      continue;
    }

    if( letter == 'a' || letter == 'n' ) {
      if( !is_name( tmp, obj->Name( ) ) )
        return 0;
      continue;
    }

    if( letter == 'b' ) {
      if( !is_name( tmp, name_before( obj ) ) ) 
        return 0;
      continue;
    }

    if( letter == 'T' ) {
      for( i = 0; !fmatches( tmp, oprog_trigger[i] ); i++ ) 
        if( i == MAX_OPROG_TRIGGER-1 ) {
          send( ch, "Unknown trigger type, see help ofind.\r\n" );
          return -1;
        } 
      for( oprog = obj->oprog; oprog != NULL && oprog->trigger != i;
        oprog = oprog->next );
      if( ( oprog != NULL ) != negative )
        continue;
      return 0;
    }

    if( letter == 'W' ) {
      if( obj->item_type != ITEM_WEAPON ) 
        return 0;
      for( i = 0; i < WEAPON_COUNT; i++ ) 
        if( fmatches( tmp, skill_table[ WEAPON_FIRST+i ].name ) ) {
          if( ( obj->value[3] == i ) == negative )
            return 0;
          break;
        }
      if( i == WEAPON_COUNT ) {
        send( ch, "Unknown weapon class - See help ofind.\r\n" );
        return -1;
      }
      continue;
    }

    for( i = 0; i < types; i++ ) 
      if( letter == flag[i] )
        break;

    if( i != types ) {
      for( j = 0; j < max[i]; j++ ) 
        if( fmatches( tmp, *(name1[i]+j*(name2[i]-name1[i])) ) ) {
          if( is_set( flag_value[i], j ) == negative )
            return 0;
          break;
        }
      if( j != max[i] )
        continue;
    }

    send( ch, "Unknown flag - See help ofind.\r\n" );
    return -1;
  }

#undef types
}


void display_ofind( obj_clss_data* obj, char_data* ch, char* buf, int& length )
{

  char *temp = str_dup(obj->Name());
  sprintf( &buf[length], "[%4d] %-33s %s %s %s%4d%4d %s ", obj->vnum, truncate( temp, 33 ), int4( obj->cost ),
    int4( obj->weight ), int4( obj->count ), obj->blocks, obj->level, int4( obj->durability ) ); 
  length += strlen( &buf[length] );

  switch( obj->item_type ) {
  case ITEM_SHIELD :
  case ITEM_ARMOR :
    sprintf( &buf[length], "AC: %d", obj->value[1] );
    break;
  case ITEM_WEAPON :
    sprintf( &buf[length], "Dm: %dd%d", obj->value[1], obj->value[2] );
    break;
  }

  strcat( buf, "\r\n" );
  length += strlen( &buf[length] );

  if( length > MAX_STRING_LENGTH-100 ) {
    page( ch, buf );
    length = 0;
    *buf = '\0';
  } 
}


void do_ofind( char_data* ch, char* argument )
{
  char                buf  [ MAX_STRING_LENGTH ];
  obj_clss_data*      obj;
  int                   i;
  int              length  = 0;
  bool              found  = FALSE;

  for( i = 0; i < MAX_OBJ_INDEX; i++ ) {
    if( ( obj = obj_index_list[i] ) != NULL ) { 
      switch( select_ofind( obj, ch, argument ) ) {
      case -1 : return;
      case  1 :
        if( !found ) {
          found = TRUE;
          page_underlined( ch, "Vnum   Name                              Cost  Wgt  Num Ing Lvl  Dur\r\n" );
        }
        display_ofind( obj, ch, buf, length );
      }
    }
  }

  if( !found ) 
    send( ch, "No object class matching search was found.\r\n" );
  else
    page( ch, buf );
}


void do_identify( char_data* ch, char* argument )
{
  obj_data* obj;

  if( *argument == '\0' ) {
    send( ch, "Identify what?\r\n" );
    return;
  }

  if( ( obj = one_object( ch, argument, "identity", &ch->contents, &ch->wearing ) ) == NULL ) 
    return;

  spell_identify( ch, NULL, obj, 10, -1 );
}


/*
 *   OBJECT ONLINE COMMANDS
 */


void do_oedit( char_data* ch, char* argument )
{
  char                 buf  [ MAX_INPUT_LENGTH ];
  obj_data*            obj;
  obj_clss_data*  obj_clss;
  obj_clss_data*  obj_copy; 
  wizard_data*      wizard;
  int                 i, j;

  wizard    = (wizard_data*) ch;
  obj_clss  = wizard->obj_edit;

  if( *argument == '\0' ) {
    send( ch, "Which object do you want to edit?\r\n" );
    return;
  }

  if( matches( argument, "new" ) ) {
    if( *argument == '\0' ) {
      send( ch, "What do you want to name the new object?\r\n" );
      return;
    }

    if( number_arg( argument, i ) ) {
      if( ( obj_copy = get_obj_index( i ) ) == NULL ) {
        send( ch, "The vnum %d cooresponds to no existing object.\r\n", i );
        return;
      }
    } else
      obj_copy = NULL;
     
    for( i = 1; ; i++ ) {
      if( i >= MAX_OBJ_INDEX ) {
        send( ch, "Mud is out of object vnums.\r\n" );
        return;
      }
      if( obj_index_list[i] == NULL )
        break;
    }

    if( obj_copy == NULL )
      obj_clss = new obj_clss_data( argument, i, ch->descr->name );
    else
      obj_clss = new obj_clss_data( obj_copy, argument, i, ch->descr->name );

    obj_index_list[ i ] = obj_clss;

    obj = create( obj_clss );
    obj->source = alloc_string( ch->real_name( ), MEM_OBJECT );
    set_owner( obj, ch, NULL );

    obj->To( ch );

    wizard->obj_edit     = obj_clss;
    wizard->oprog_edit   = NULL;
    wizard->oextra_edit  = NULL;

    send( ch, "Object created: %s (vnum %d)\r\n", obj_clss->Name( ), obj_clss->vnum );
    if( obj_clss->item_type == ITEM_MONEY )
      update_coinage();
    return;
  }

/*
  if( exact_match( argument, "replace" ) ) {
    oedit_replace( ch, argument );
    return;
  }
*/

  if( is_spirit( ch ) && exact_match( argument, "renumber" ) ) {
    if( !number_arg( argument, i ) || !number_arg( argument, j ) ) {
      send( ch, "Syntax: oedit renumber <vnum_old> <vnum_new>.\r\n" );
      return;
    }

    obj_clss_data* obj1 = get_obj_index( i );
    obj_clss_data* obj2 = get_obj_index( j );

    if( !obj1 ) {
      send( ch, "Object %d does not exist.\r\n", i );
      return;
    } else if( obj2 ) {
      send( ch, "Object %d exists, delete it first.\r\n", j );
      return;
    } else if( j >= MAX_OBJ_INDEX ) {
      send( ch, "Pick a number between 1 and %d.\r\n", MAX_OBJ_INDEX );
      return;
    }

    if( !ch->can_edit( obj1 ) ) {
      send( ch, no_permission );
      return;
    }

    oedit_renumber( ch, i, j );
    return;
  }

  if( matches( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      if( ( obj_clss = wizard->obj_edit ) == NULL ) {
        send( "You aren't editing any object.\r\n", ch );
        return;
      }
    } else {
      if( ( obj_clss = get_obj_index( atoi( argument ) ) ) == NULL ) {
        send( ch, "There is no object by that number.\r\n" );
        return;
      }
    } 

    if( !ch->can_edit( obj_clss ) ) {
      send( ch, no_permission );
      return;
    }

    if( !can_extract( obj_clss, ch ) )
      return;

    send( ch, "You delete %s.\r\n", obj_clss->Name( ) );
    sprintf( buf, "Object deleted: %s (%s)", obj_clss->Name( ), ch->descr->name );
    info( "", LEVEL_APPRENTICE, buf, IFLAG_WRITES ); 

    wizard->obj_edit = obj_clss;
    extract( wizard, offset( &wizard->obj_edit, wizard ), "object" );
    obj_index_list[obj_clss->vnum] = NULL;
    delete obj_clss;
    wizard->obj_edit = NULL;
    return;
  }

  if( number_arg( argument, i ) ) {
    if( ( obj_clss = get_obj_index( i ) ) == NULL ) {
      send( ch, "No object has that vnum.\r\n" );
      return;
    }
    wizard->obj_edit = obj_clss;
  } else {
    if( ( obj = one_object( ch, argument, "oedit", &ch->contents, &ch->wearing, ch->array ) ) == NULL )
      return;
    wizard->obj_edit = obj->pIndexData;
  }

  wizard->oextra_edit = NULL;
  wizard->oprog_edit  = NULL;

  send( ch, "Odesc and oset now operate on %s.\r\n", wizard->obj_edit->Name( ) );
}


void oedit_renumber( char_data* ch, int old_vnum, int new_vnum )
{
  reset_data*         reset;
  species_data*     species;
  mprog_data*         mprog;
  obj_clss_data*   obj_clss;
  oprog_data*         oprog;
  room_data*           room;
  action_data*       action;
  shop_data*           shop;
  custom_data*       custom;
  selt_string temp;
  int i, j;

  int found = 0;
  obj_clss_data* tobj = get_obj_index( old_vnum );

  // search mobs
  for( i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[ i ] ) == NULL ) 
      continue;

    for( reset = species->reset; reset != NULL; reset = reset->next ) {
      if( reset->target == tobj )
        found++;
    }

    if( species->corpse == old_vnum ) {
      species->corpse = new_vnum;
      found++;
    }

    if( search_progs( species->attack->binary, old_vnum, OBJECT ) ) {
      temp._printf( "// object %d needs references changed to %d\r\n", old_vnum, new_vnum );
      temp += species->attack->code;
      free_string( species->attack->code, MEM_MPROG );
      species->attack->code = alloc_string( temp.get_text( ), MEM_MPROG );

      page( ch, "  Used in attack prog on mob #%d.\r\n", i );
      found++;
    }

    for( j = 1, mprog = species->mprog; mprog != NULL; j++, mprog = mprog->next ) {
      if( search_progs( mprog->binary, old_vnum, OBJECT ) ) {
        temp._printf( "// object %d needs references changed to %d\r\n", old_vnum, new_vnum );
        temp += mprog->code;
        free_string( mprog->code, MEM_MPROG );
        mprog->code = alloc_string( temp.get_text( ), MEM_MPROG );

        page( ch, "  Used in mprog #%d on mob #%d.\r\n", j, i );
        found++;
      }

      if( ( mprog->trigger == MPROG_TRIGGER_GIVE || mprog->trigger == MPROG_TRIGGER_GIVE_TYPE ) && mprog->value == old_vnum ) {
        mprog->value = new_vnum;
        found++;
      }
    }
  }

  // search objs
  for( i = 0; i < MAX_OBJ_INDEX; i++ ) {
    if( ( obj_clss = obj_index_list[ i ] ) == NULL )
      continue;

    if( obj_clss->item_type == ITEM_CONTAINER && obj_clss->value[ 2 ] == old_vnum ) {
      obj_clss->value[ 2 ] = new_vnum;
      found++;
    }

    if( obj_clss->item_type == ITEM_FIRE && obj_clss->value[ 1 ] == old_vnum ) {
      obj_clss->value[ 1 ] = new_vnum;
      found++;
    }

    if( obj_clss->fakes == old_vnum ) {
      obj_clss->fakes = new_vnum;
      found++;
    }

    for( j = 1, oprog = obj_clss->oprog; oprog != NULL; j++, oprog = oprog->next ) {
      if( search_progs( oprog->binary, old_vnum, OBJECT ) ) {
        temp._printf( "// object %d needs references changed to %d\r\n", old_vnum, new_vnum );
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
        if( reset->target == tobj )
          found++;
      }

      if( tobj->item_type == ITEM_KEY ) {
        for( int i = 0; i < room->exits; i++ ) {
          if( room->exits[ i ]->key == old_vnum ) {
            room->exits[ i ]->key = new_vnum;
            found++;
          }
        }
      }

      for( j = 1, action = room->action; action != NULL; j++, action = action->next ) {
        if( action->binary != NULL && search_progs( action->binary, old_vnum, OBJECT ) ) {
          temp._printf( "// object %d needs references changed to %d\r\n", old_vnum, new_vnum );
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
      if( reset->target == tobj )
        found++;
    }
  }

  // search shops
  for( shop = shop_list; shop != NULL; shop = shop->next ) {
    for( custom = shop->custom; custom != NULL; custom = custom->next ) {
      if( custom->item == tobj )
        found++;
      
      for( i = 0; i < MAX_INGRED; i++ ) {
        if( custom->ingred[ i ] == tobj )
          found++;
      }
    }
  }
  
  // search spells
  for( i = 0; i < SPELL_COUNT; i++ ) {
    for( j = 0; j < MAX_SPELL_WAIT; j++ ) {
      if( abs( spell_table[ i ].reagent[ j ] ) == old_vnum ) {
        if( spell_table[ i ].reagent[ j ] < 0 )
          spell_table[ i ].reagent[ j ] = -new_vnum;
        else
          spell_table[ i ].reagent[ j ] = new_vnum;
        found++;
      }
    }
  }

  // search lists
  for( i = 0; i < MAX_LIST; i++ ) {
    if( !str_cmp( list_entry[ i ][ 2 ], "O" ) ) {
      for( j = 0; j < atoi( list_entry[ i ][ 0 ] ); j++ ) {
        if( list_value[ i ][ j ] == old_vnum ) {
          list_value[ i ][ j ] = new_vnum;
          found++;
        }
      }
    }
  }

  // search starting tables
  for( i = 0; i < MAX_STARTING; i++ ) {
    for( j = 0; j < 5; j++ ) {
      if( starting_table[ i ].object[ 2 * j ] == old_vnum ) {
        starting_table[ i ].object[ 2 * j ] = new_vnum;
        found++;
      }
    }
  }

  // search build tables
  for( i = 0; i < MAX_ENTRY_BUILD; i++ ) {
    if( build_table[ i ].result[ 0 ] == old_vnum ) {
      build_table[ i ].result[ 0 ] = new_vnum;
      found++;
    }

    for( j = 0; j < 10; j++ ) {
      if( build_table[ i ].ingredient[ 2 * j ] == old_vnum ) {
        build_table[ i ].ingredient[ 2 * j ] = new_vnum;
        found++;
      }
    }
  }


  tobj->vnum = new_vnum;
  obj_index_list[ new_vnum ] = tobj;
  obj_index_list[ old_vnum ] = NULL;

  send( ch, "Object #%d renumbered to #%d in %d places.\r\nPlease 'write -f all' and fix all scripts as listed above.\r\n", old_vnum, new_vnum, found );
}


void oedit_replace( char_data* ch, char* argument )
{
  reset_data*        reset;
  room_data*          room;
  obj_clss_data*      obj1  = NULL;
  obj_clss_data*      obj2  = NULL;
  int                count  = 0;
  int                 i, j;

  if( !number_arg( argument, i ) || !number_arg( argument, j ) ) {
    send( ch, "Syntax: oedit replace <vnum_old> <vnum_new>.\r\n" );
    return;
  }

  if( ( obj1 = get_obj_index( i ) ) == NULL || ( obj2 = get_obj_index( j ) ) == NULL ) {
    send( ch, "Vnum %d doesn't coorespond to an existing object.\r\n", obj1 == NULL ? i : j );
    return;
  }

  // search areas
  area_nav alist( &area_list );
  for( area_data *area = alist.first( ); area; area = alist.next( ) )
    for( room = area->room_first; room != NULL; room = room->next ) 
      for( reset = room->reset; reset != NULL; reset = reset->next ) 
        if( reset->target == obj1 && is_set( &reset->flags, RSFLAG_OBJECT ) ) {
          reset->target = obj2;
          count++;
          area->modified = TRUE;
        }

/*
  // search mobs
  for( int k = 0; k < MAX_SPECIES; k++ ) 
    if( ( species = species_list[k] ) != NULL )
      for( reset = species->reset; reset != NULL; reset = reset->next )
        if( reset->target == obj1 && is_set( &reset->flags, RSFLAG_OBJECT ) ) {
          reset->target = obj2;
          count++;
        }
*/

//#pragma message ( "oedit_replace requires script search for reset change" ) 

  page( ch, "Vnum %d replaced by %d in %d reset%s.\r\n", i, j, count, count != 1 ? "s" : "" );
  
}


/*
 *   OBJECT DESCRIPTIONS
 */


void do_odesc( char_data* ch, char* argument )
{
  wizard_data*  wizard;

  wizard = (wizard_data*) ch;

  if( wizard->obj_edit == NULL ) {
    send( ch, "You aren't editing any object.\r\n" );
    return;
  }

  if( wizard->oextra_edit == NULL ) {
    send( "You aren't editing any extra.\r\n", ch );
    return;
  } else {
    wizard->oextra_edit->text = edit_string( ch, argument, wizard->oextra_edit->text, MEM_OBJ_CLSS );

    if( *argument != '\0' ) {
      char tmp  [ MAX_STRING_LENGTH ];
      sprintf( tmp, "ode %s: %s", wizard->oextra_edit->keyword, argument );
      obj_log( ch, wizard->obj_edit->vnum, tmp );
    }
  }
}


void do_oextra( char_data* ch, char* argument )
{
  obj_clss_data*  obj_clss;
  wizard_data*      wizard;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    send( "You aren't editing any object.\r\n", ch );
    return;
  }

  if( ch->can_edit( obj_clss ) ) 
    edit_extra( obj_clss->extra_descr, wizard, offset( &wizard->oextra_edit, wizard ), argument, "odesc" );
}


void do_oflag( char_data* ch, char* argument )
{
  obj_clss_data*   obj_clss;
  wizard_data*       wizard;
  const char*        string;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    page( ch, "You aren't editing any object.\r\n" );
    return;
  }

  #define types 11

  const char* title [types] = { "Size", "Affect", "Object", "Wear", "Layer",
    "Anti", "Restriction", "Material", "Container", "Trap", "Religion" }; 

  int  max [types] = { MAX_SFLAG, MAX_ENTRY_AFF_CHAR, MAX_OFLAG,
    MAX_ITEM_WEAR, MAX_LAYER, MAX_ANTI,
    MAX_RESTRICTION, MAX_MATERIAL, MAX_CONT, MAX_TRAP, MAX_ENTRY_RELIGION };

  const char** name1 [types] = { &size_flags[0], &aff_char_table[0].name,
    &oflag_name[0], &wear_name[0], &layer_name[0],
    &anti_flags[0], &restriction_flags[0],
    &material_name[0], &cont_flag_name[0], &trap_flags[0], 
    &religion_table[0].name };
  const char** name2 [types] = { &size_flags[1], &aff_char_table[1].name,
    &oflag_name[1], &wear_name[1], &layer_name[1],
    &anti_flags[1], &restriction_flags[1], &material_name[1], 
    &cont_flag_name[1], &trap_flags[1], &religion_table[1].name };

  int *flag_value [types] = { &obj_clss->size_flags, obj_clss->affect_flags,
    obj_clss->extra_flags, &obj_clss->wear_flags, &obj_clss->layer_flags,
    &obj_clss->anti_flags,
    &obj_clss->restrictions, &obj_clss->materials, &obj_clss->value[1],
    &obj_clss->value[0], & obj_clss->religion_flags };

  int uses_flag [types] = { 1, 1, 1, 1, 1, 1, 1, 1, 
    obj_clss->item_type == ITEM_CONTAINER,
    obj_clss->item_type == ITEM_TRAP, 1 };

  string = flag_handler( title, name1, name2, flag_value, max, uses_flag,
    ch->can_edit( obj_clss ) ? NULL : no_permission, ch, argument, types ); 

  if( string != NULL )
    obj_log( ch, obj_clss->vnum, string );

  #undef types
}


void do_oload( char_data* ch, char *argument )
{  
  obj_clss_data*  obj_clss;
  obj_data*            obj;
  int                 vnum;

  if( *argument == '\0' || !number_arg( argument, vnum ) ) {
    send( ch, "Syntax: oload <vnum>.\r\n" );
    return;
  }

  if( ( obj_clss = get_obj_index( vnum ) ) == NULL ) {
    send( ch, "No object has that vnum.\r\n" );
    return;
  }

  obj         = create( obj_clss );
  obj->source = alloc_string( ch->real_name( ), MEM_OBJECT );

//  enchant_object( obj );
  set_alloy( obj, 100 );

//  obj->owner = ch->pcdata->pfile;
  set_owner( obj, ch, NULL );

  send( ch, "You have created %s!\r\n", obj );
  send( *ch->array, "%s has created %s!\r\n", ch, obj );

  obj->To( can_wear( obj, ITEM_TAKE ) ? ch : ch->array->where );
  consolidate( obj );
}


/*
 *   OSET
 */


bool oset_drink_container( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "capacity",        -1,    10000,   &obj->value[0]       },
    { "current.volume",   0,    10000,   &obj->value[1]       },
    { "",                 0,        0,   NULL                 },
  };

  return process( int_list, ch, obj->Name( ), argument, obj );
}

bool oset_potion( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "spell",            0, SPELL_COUNT,  &obj->value[0]       },
    { "level",            0,          99,  &obj->value[1]       },
    { "",                 0,           0,  NULL                 },
  };

  return process( int_list, ch, obj->Name( ), argument, obj );
}

bool oset_wand( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "spell",            0, SPELL_COUNT,  &obj->value[0]       },
    { "level",            0,          99,  &obj->value[1]       },
    { "duration",         0,          99,  &obj->value[2]       },
    { "charges",          0,         100,  &obj->value[3]       },
    { "",                 0,           0,  NULL                 },
  };

  return process( int_list, ch, obj->Name( ), argument, obj );
}

bool oset_container( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "capacity",     0,         10000,   &obj->value[0]       },
    { "key",          0, MAX_OBJ_INDEX,   &obj->value[2]       },
    { "",             0,             0,   NULL                 },
  };

  return process( int_list, ch, obj->Name( ), argument, obj );
}


bool oset_reagent( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "charges",          0,      100,   &obj->value[0]       },
    { "",                 0,        0,   NULL                 },
  };

  return process( int_list, ch, obj->Name( ), argument, obj );
}


bool oset_food( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "nourishment",      0,      100,   &obj->value[0]       },
    { "cooked",           0,        4,   &obj->value[1]       },
    { "poisoned",        -1,        0,   &obj->value[3]       },
    { "",                 0,        0,   NULL                 },
  };

  return process( int_list, ch, obj->Name( ), argument, obj );
}


bool oset_armor( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "enchantment",     -5,        5,   &obj->value[0]       },
    { "ac",               0,       50,   &obj->value[1]       },
    { "global ac",        0,       50,   &obj->value[2]       },
    { "",                 0,        0,   NULL                 },
  };

  return process( int_list, ch, obj->Name( ), argument, obj );
}


bool oset_corpse( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "halflife",   0,  QUEUE_LENGTH-1,   &obj->value[0]       },
    { "species",    0,     MAX_SPECIES,   &obj->value[1]       },
    { "",           0,               0,   NULL                 },
  };

  return process( int_list, ch, obj->Name( ), argument, obj );
}


bool oset_weapon( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "enchantment",     -5,        5,   &obj->value[0]       },
    { "damdice",          0,       50,   &obj->value[1]       },
    { "damside",          0,       50,   &obj->value[2]       },
    { "",                 0,        0,   NULL                 },
  };

  if( process( int_list, ch, obj->Name( ), argument, obj ) )
    return TRUE;

  #define wn(i) skill_table[ WEAPON_FIRST+i ].name

  class type_field type_list[] = {
    { "class", WEAPON_COUNT, &wn(0),  &wn(1),  &obj->value[3] },
    { "",      0,            NULL,    NULL,    NULL        }
  };

  #undef wn

  return process( type_list, ch, obj->Name( ), argument, obj );
}

bool oset_arrow( char_data* ch, obj_clss_data* obj, char* argument )
{
  class int_field int_list[] = {
    { "damdice",          0,       50,   &obj->value[1]        },
    { "damside",          0,       50,   &obj->value[2]        },
    { "shots",            0,      200,   &obj->value[3]        },
    { "",                 0,        0,   NULL                  },
  };

  return process( int_list, ch, obj->Name( ), argument, obj );
}

void do_oset( char_data* ch, char* argument )
{
  char                 buf  [ MAX_INPUT_LENGTH ];
  obj_clss_data*  obj_clss;
  obj_data*            obj;
  wizard_data*      wizard;
  char*             string;

  wizard = (wizard_data*) ch;

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    send( ch, "You aren't editing any object.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    do_ostat( ch, "" );
    return;
  }

  if( !ch->can_edit( obj_clss ) ) 
    return;

  free_string( obj_clss->last_mod, MEM_OBJ_CLSS );
  obj_clss->last_mod = alloc_string( ch->descr->name, MEM_OBJ_CLSS );

  class int_field int_list[] = {
    { "fakes",            0,    10000,   &obj_clss->fakes                },
    { "level",            0,       99,   &obj_clss->level                },
    { "value0",     -100000,   100000,   &obj_clss->value[0]             },
    { "value1",     -100000,   100000,   &obj_clss->value[1]             },
    { "value2",     -100000,   100000,   &obj_clss->value[2]             },
    { "value3",     -100000,   100000,   &obj_clss->value[3]             },
    { "limit",           -1,   100000,   &obj_clss->limit                },
    { "repair",           0,       10,   &obj_clss->repair               },
    { "durability",       1,    10000,   &obj_clss->durability           },
    { "ingots",           0,      200,   &obj_clss->blocks               },
    { "cost",             0,  1000000,   &obj_clss->cost                 },
    { "light",            0,      100,   &obj_clss->light                },
    { "defensive.mod", -200,       50,   &obj_clss->clss_synergy[0]      },
    { "mage.mod",      -200,       50,   &obj_clss->clss_synergy[1]      },
    { "cleric.mod",    -200,       50,   &obj_clss->clss_synergy[2]      },
    { "thief.mod",     -200,       50,   &obj_clss->clss_synergy[3]      },
    { "warrior.mod",   -200,       50,   &obj_clss->clss_synergy[4]      },
    { "paladin.mod",   -200,       50,   &obj_clss->clss_synergy[5]      },
    { "ranger.mod",    -200,       50,   &obj_clss->clss_synergy[6]      },
    { "blade.mod",     -200,       50,   &obj_clss->clss_synergy[7]      },
    { "monk.mod",      -200,       50,   &obj_clss->clss_synergy[8]      },
    { "sorcerer.mod",  -200,       50,   &obj_clss->clss_synergy[9]      },
    { "druid.mod",     -200,       50,   &obj_clss->clss_synergy[10]     },
    { "cavalier.mod",  -200,       50,   &obj_clss->clss_synergy[11]     },
    { "barbarian.mod", -200,       50,   &obj_clss->clss_synergy[12]     },
    { "rogue.mod",     -200,       50,   &obj_clss->clss_synergy[13]     },
    { "assassin.mod",  -200,       50,   &obj_clss->clss_synergy[14]     },
    { "",                 0,        0,   NULL                   },
  };

  if( process( int_list, ch, obj_clss->Name( ), argument, obj_clss ) )
    return;

  class cent_field cent_list[] = {
    { "weight",           0,  1000000,   &obj_clss->weight      },
    { "",                 0,        0,   NULL                   },
  };

  if( process( cent_list, ch, obj_clss->Name( ), argument, obj_clss ) )
    return;

  char *word[] = { "singular", "plural", "after", "before", "long_s", "long_p", "creator", "prefix_singular", "prefix_plural" };

  char **pChar[] = { &obj_clss->singular, &obj_clss->plural,
    &obj_clss->after, &obj_clss->before, &obj_clss->long_s,
    &obj_clss->long_p, &obj_clss->creator,
    &obj_clss->prefix_singular, &obj_clss->prefix_plural };

  for( int i = 0; i < 9; i++ ) {
    if( matches( argument, word[i] ) ) {
      sprintf( buf, "%s set to %s.", word[i], argument );
      obj_log( ch, obj_clss->vnum, buf );
      send( ch, "The %s of %s is now:\r\n%s\r\n", word[i], obj_clss->Name( ), argument );

      string = alloc_string( argument, MEM_OBJ_CLSS );

      if( i < 4 ) {
        for( int j = 0; j < obj_list; j++ ) {
          obj = obj_list[j];
          if (!obj || !obj->Is_Valid())
            continue;

          if( obj->pIndexData == obj_clss ) {
            char **pChar2[] = { &obj->singular, &obj->plural,
              &obj->after, &obj->before };
            if( *pChar2[i] == *pChar[i] )
              *pChar2[i] = string;
          }
        }
      }
      free_string( *pChar[i], MEM_OBJ_CLSS );
      *pChar[i] = string;
      return;
    }          
  }

#define itn         item_type_name
#define cn( i )     color_fields[i]

  class type_field type_list [] = {
    { "type",   MAX_ITEM,    &itn[0], &itn[1],  &obj_clss->item_type },
    { "color",  MAX_COLOR,   &cn(0),  &cn(1),   &obj_clss->color    },
    { "",       0,           NULL,    NULL,     NULL                 },
  };

  if( process( type_list, ch, obj_clss->Name( ), argument, obj_clss ) ) {
    if( obj_clss->item_type == ITEM_MONEY )
      update_coinage();
    return;
  }

#undef itn
#undef cn

  switch( obj_clss->item_type ) {
  case ITEM_WEAPON :
    if( oset_weapon( ch, obj_clss, argument ) )
      return;
    break;
  case ITEM_SHIELD :
  case ITEM_ARMOR :
    if( oset_armor( ch, obj_clss, argument ) )
      return;
    break;
  case ITEM_POTION :
  case ITEM_SCROLL :
    if( oset_potion( ch, obj_clss, argument ) )
      return;
    break;
  case ITEM_WAND :
  case ITEM_STAFF :
    if( oset_wand( ch, obj_clss, argument ) )
      return;
    break;
  case ITEM_CONTAINER :
    if( oset_container( ch, obj_clss, argument ) )
      return;
    break;
  case ITEM_REAGENT :
    if( oset_reagent( ch, obj_clss, argument ) )
      return;
    break;
  case ITEM_FOOD :
    if( oset_food( ch, obj_clss, argument ) )
      return;
    break;
  case ITEM_DRINK_CON :
    if( oset_drink_container( ch, obj_clss, argument ) )
      return;
    break;
  case ITEM_CORPSE :
    if( oset_corpse( ch, obj_clss, argument ) )
      return;
    break;
  case ITEM_BOLT :
  case ITEM_STONE :
  case ITEM_ARROW :
    if( oset_arrow( ch, obj_clss, argument ) )
      return;
    break;
  }

  if( matches( argument, "affect" ) ) {
    oset_affect( ch, obj_clss, argument );
    return;
  }
      
  send( ch, "Unknown field.\r\n" );
}


void oset_affect( char_data* ch, obj_clss_data* obj_clss, char* argument )
{
  int                  i;
  int                col;
  int                num;
  affect_data*       paf  = NULL;
      
  if( *argument == '\0' ) {
    send( ch, "Affect Types:\r\n" );
    for( i = 1, col = 0; i < MAX_AFF_LOCATION; i++ ) {
      send( ch, "%15s", affect_location[i] );
      if( ++col%3 == 0 )
        send( ch, "\r\n" );
    }
    if( col%3 != 0 )
      send( ch, "\r\n" );
    return;
  }
      
  for( i = 1; i < MAX_AFF_LOCATION; i++ ) {
    if( matches( argument, affect_location[i] ) ) {
      for( int j = 0; j < obj_clss->affected; j++ ) 
        for( int k = 0; k < MAX_AFF_MODS; k++ ) {
        if( obj_clss->affected[j]->mlocation[k] == i ) {
          paf = obj_clss->affected[j];
          break;
        }
        }
      if( ( num = atoi( argument ) ) == 0 ) {
        if( paf != NULL ) 
          obj_clss->affected -= paf;
      } else {
        if( paf == NULL ) {
          paf                 = new affect_data;
          paf->type           = AFF_NONE;
          paf->duration       = -1;
          paf->mlocation[0]   = i;
          obj_clss->affected += paf;
        }
        paf->mmodifier[0]     = num;
      }
      send( ch, "%s modifier of %d set on %s.\r\n", affect_location[i], num, obj_clss->Name( ) );
      return;
    }
  }
       
  send( ch, "Unknown affect location.\r\n" );
}


/*
 *  OSTAT ROUTINE
 */
/*
void display_oskill( char_data* ch, obj_clss_data* obj_clss )
{
  bool found = false;
  
  for( int i = 0; i < table_max[ TABLE_SKILL ]; i++ ) {
    if( obj_clss->skill_modifier[i] != 0 ) {
      if( !found ) {
        page_title( ch, obj_clss->Name( ) );
        page( ch, "\r\n" );
        page_underlined( ch, "     Skill Modified                        Modifier\r\n" );
        found = true;
      }
      page( ch, "     %-40s %3d\r\n", skill_table[i].name, obj_clss->skill_modifier[i] );
    }
  }

  if( !found )
    send( ch, "%s has no skill synergies.\r\n", obj_clss );

  return;
}

void do_oskill( char_data* ch, char* argument )
{
  obj_clss_data*     obj_clss;
  wizard_data*         wizard = (wizard_data*) ch;
  char                    arg  [ MAX_STRING_LENGTH ];
  int        n, length, level = 0;
  char                    buf  [ MAX_INPUT_LENGTH ];

  if( ( obj_clss = wizard->obj_edit ) == NULL ) {
    send( ch, "You must be editing an object to view or edit a skill synergy on it.\r\n" );
    return;
  }

  if( *argument == '\0' ) {
    display_oskill( ch, obj_clss );
    return;
  }

  argument = one_argument( argument, arg );

  if( !is_number( argument ) ) {
    send( ch, "Syntax: oskill <skill> <level>.\r\n" );
    return;
  }

  if( ( level = atoi( argument ) ) < -500 || level > 50 ) {
    send( "Level must be 0 (reset) or -500 to 50.\r\n", ch );
    return;
  }

  length = strlen( arg );
  for( n = 0; n < MAX_SKILL; n++ ) 
    if( !strncasecmp( arg, skill_table[n].name, length ) )
      break;
  if( n == MAX_SKILL ) {
    if( strncasecmp( arg, "none", length ) ) {
      send( "Unknown skill.\r\n", ch );
      return;
    } 
  }
  
  obj_clss->skill_modifier[n] = level;
  send( ch, "%s skill synergy set to %d.\r\n", skill_table[n].name, level );
  sprintf( buf, "%s synergy set to %d.", skill_table[n].name, level );
  obj_log( ch, obj_clss->vnum, buf );

  return;
}
*/
void do_ostat( char_data* ch, char* argument )
{
  char                   tmp  [ 3*MAX_STRING_LENGTH ];
  affect_data*           paf;
  obj_data*              obj  = NULL;
  obj_clss_data*         key;
  obj_clss_data*    obj_clss;
  wizard_data*        wizard  = (wizard_data*) ch;
  species_data*      species;

  if( *argument == '\0' ) {
    if( ( obj_clss = wizard->obj_edit ) == NULL ) {
      send( ch, "Specify object or select one with oedit.\r\n" );
      return;
    }
  } else if( is_number( argument ) ) {
    if( ( obj_clss = get_obj_index( atoi( argument ) ) ) == NULL ) {
      send( ch, "No object has that vnum.\r\n" );
      return;
    }
  } else {
    if( ( obj = one_object( ch, argument, "ostat", &ch->contents, &ch->wearing, ch->array ) ) == NULL )
      return;
    obj_clss = obj->pIndexData;
  }

  page_title( ch, obj_clss->Name( ) );

  page( ch, "          Vnum: %-10d Creator: %s%s%s\r\n", obj_clss->vnum,  color_code( ch, COLOR_BOLD_YELLOW ), obj_clss->creator, normal( ch ) );
  page( ch, "         Fakes: %-10s", obj_clss->fakes == obj_clss->vnum ? "--" : atos( obj_clss->fakes ) );
  if( is_angel( ch ) )
    page( ch, "Last Mod: %s", obj_clss->last_mod ); 
  page( ch, "\r\n" );

  page( ch, "          Type: %s\r\n", item_type_name[ obj_clss->item_type ] );
  page( ch, "        Weight: %-12.2f Light: %-8d Base Cost: %d\r\n", (float) obj_clss->weight/100, obj_clss->light, obj_clss->cost );
  page( ch, "         Level: %-11d Number: %-12d Limit: %d\r\n", obj_clss->level, obj_clss->count, obj_clss->limit );
  page( ch, "        Repair: %-10d Durabil: %-11d Ingots: %d\r\n\r\n", obj_clss->repair, obj_clss->durability, obj_clss->blocks );
  page( ch, "         Color: %-10s Affect.Levels: %-4i\r\n\r\n", color_fields[ obj_clss->color ], obj_clss->affect_levels( ) );

  page( ch, " Defensive.Mod: %-8d      Mage.Mod: %-7d     Cleric.Mod: %d\r\n", obj_clss->clss_synergy[0], obj_clss->clss_synergy[1], obj_clss->clss_synergy[2] );
  page( ch, "     Thief.Mod: %-8d   Warrior.Mod: %-7d    Paladin.Mod: %d\r\n", obj_clss->clss_synergy[3], obj_clss->clss_synergy[4], obj_clss->clss_synergy[5] );
  page( ch, "    Ranger.Mod: %-8d     Blade.Mod: %-7d       Monk.Mod: %d\r\n", obj_clss->clss_synergy[6], obj_clss->clss_synergy[7], obj_clss->clss_synergy[8] );
  page( ch, "  Sorceror.Mod: %-8d     Druid.Mod: %-7d   Cavalier.Mod: %d\r\n", obj_clss->clss_synergy[9], obj_clss->clss_synergy[10], obj_clss->clss_synergy[11] );
  page( ch, " Barbarian.Mod: %-8d     Rogue.Mod: %-7d   Assassin.Mod: %d\r\n\r\n", obj_clss->clss_synergy[12], obj_clss->clss_synergy[13], obj_clss->clss_synergy[14] );

  switch( obj_clss->item_type ) { // lots of common code with spell_identify
  case ITEM_LIGHT :
    page( ch, "Default Duration: %d (when loaded)\r\n", obj_clss->value[2] );
    page( ch, "  Spell Duration: %d (when created light spell at level 10)\r\n\r\n", obj_clss->value[3] );
    break;

  case ITEM_SCROLL :
  case ITEM_POTION :
    page( ch, "         Spell: %d (%s)\r\n", obj_clss->value[0], ( obj_clss->value[0] >= 0 && obj_clss->value[0] < SPELL_COUNT ) ? spell_table[ obj_clss->value[0] ].name : "none" );
    page( ch, "    Base Level: %-4d Base Duration: %d\r\n\r\n", obj_clss->value[1], obj_clss->value[2] );
    break;

  case ITEM_WAND:
  case ITEM_STAFF:
    page( ch, "         Spell: %d (%s)\r\n", obj_clss->value[0], ( obj_clss->value[0] >= 0 && obj_clss->value[0] < SPELL_COUNT ) ? spell_table[ obj_clss->value[0] ].name : "none" );
    page( ch, "  Base Charges: %-12d Level: %-9d Duration: %d\r\n\r\n", obj_clss->value[3], obj_clss->value[1], obj_clss->value[2] );
    break;

  case ITEM_WEAPON :
    page( ch, "         Class: %-11s Attack: %s\r\n", weapon_class( obj_clss ), weapon_attack( obj_clss ) );
    page( ch, "       Damdice: %-10d Damside: %-6d Enchantment: %d\r\n\r\n", obj_clss->value[1], obj_clss->value[2], obj_clss->value[0] );
    break;

  case ITEM_STONE :
  case ITEM_BOLT :
  case ITEM_ARROW:
    page( ch, "       Damdice: %-10d Damside: %-6d\r\n\r\n", obj_clss->value[1], obj_clss->value[2] );
    page( ch, "       Number of Shots: %d\r\n", obj_clss->value[3] );
    break;
    
  case ITEM_SHIELD :
  case ITEM_ARMOR :
    page( ch, "            AC: %-8d Global AC: %d\r\n", obj_clss->value[1], obj_clss->value[2] );
    page( ch, "   Enchantment: %d\r\n\r\n", obj_clss->value[0] );
    break;

  case ITEM_REAGENT :
    page( ch, "       Charges: %d\r\n\r\n", obj_clss->value[0] );
    break;

  case ITEM_CONTAINER :   
    page( ch, "      Capacity: %-14d Key: %d (%s)\r\n\r\n", obj_clss->value[0], obj_clss->value[2], ( key = get_obj_index( obj_clss->value[2] ) ) == NULL ? "none" : key->Name( ) );
    break;

  case ITEM_DRINK_CON :
    page( ch, "      Capacity: %d\r\n\r\n", obj_clss->value[0] );
    break;

  case ITEM_FOOD :
    page( ch, "   Nourishment: %-14d\r\n\r\n", obj_clss->value[0] );
    break;

  case ITEM_CORPSE:
    species  = get_species( obj_clss->value[1] );
    page( ch, "      Halflife: %d\r\n", obj_clss->value[0] );
    page( ch, "       Species: %-5d (%s)\r\n\r\n", obj_clss->value[1], species == NULL ? "none" : species->Name( ) ); 
    break;
  
  case ITEM_WHISTLE:
    page( ch, "         Range: %d\r\n", obj_clss->value[0] );
    break;

  case ITEM_TRAP:
    page( ch, "        Damage: %dd%d\r\n", obj_clss->value[1], obj_clss->value[2] );
    break;
  }

  if( obj != NULL ) {
    page( ch, "          Acid: %3d%%          Fire: %3d%%          Cold: %3d%%\r\n", obj->vs_acid(), obj->vs_fire(), obj->vs_cold() );
    page( ch, "     Condition: %-14d Age: %-12d Value: %d\r\n", obj->condition, obj->age, obj->Cost( ) );
    page( ch, "        Weight: %.2f lbs\r\n", (float) obj->Weight( 1 )/100 );
    page( ch, "         Owner: %s\r\n\r\n", obj->owner == NULL ? "no one" : obj->owner->name );
    page( ch, "           Age: %i\r\n", obj->age );
    page( ch, "        Value0: %i    Value1: %i    Value2: %i   Value3: %i\r\n", obj->value[0],obj->value[1], obj->value[2], obj->value[3] );
    page( ch, "Number In Pile: %10i  Timer: %i\r\n", obj->number, obj->timer );
    if( obj->source != empty_string )
      page( ch, "    Reset Crap: Oloaded by %s\r\n", obj->source );
    else if( obj->reset_mob_vnum == 0 && obj->reset_room_vnum == 0 )
      page( ch, "    Reset Crap: Old object\r\n" );
    else
      page( ch, "    Reset Crap: %s%i at one of these rates 1ch: %i 2ch: %i 3ch: %i\r\n",
      obj->reset_mob_vnum != 0 ? "Mob #" : "Room #", obj->reset_mob_vnum != 0 ?
      obj->reset_mob_vnum : obj->reset_room_vnum, obj->reset_chances[0],
      obj->reset_chances[1], obj->reset_chances[2] );
    page( ch, "\r\n" );
    page_title( ch, "Variables of %s", obj_clss->Name( ));
    
    if( obj->variables.size > 0 ) {
      variable_nav list( &obj->variables );
      for( variable_data* var = list.first( ); var; var = list.next( ) )
        page( ch, "%39s : %-5d\r\n", var->get_name( ), var->get_value( ) );
    } else
      page( ch, "<none defined>\r\n" );

    switch ( obj_clss->item_type ) {
    case ITEM_CORPSE :
      page( ch, "RemainHalflife: %d\r\n", obj->value[0] );
      if( obj->pIndexData->vnum == OBJ_CORPSE_PC ) {
        pfile_data* pfile = get_pfile( obj->value[1] );
        page( ch, "    Belongs To: %d (%s)\r\n", obj->value[1], pfile == NULL ? "unknown" : pfile->name );
      } else {
        species = get_species( obj->value[1] );
        page( ch, "       Species: %-5d (%s)\r\n", obj->value[1], species == NULL ? "none" : species->Name( ) );
      }
      break;

    case ITEM_LIGHT :
      page( ch, "        Duration: %d\r\n", obj->value[2] );
      break;

    case ITEM_SCROLL :
    case ITEM_POTION :
      page( ch, "         Level: %-9d Duration: %d\r\n", obj->value[1], obj->value[2] );
      break;

    case ITEM_WAND:
    case ITEM_STAFF:
      page( ch, "       Charges: %-12d Level: %-9d Duration: %d\r\n", obj->value[3], obj->value[1], obj->value[2] );
      break;

    case ITEM_WEAPON :
      page( ch, "   Enchantment: %-9d\r\n\r\n", obj->value[0] );
      break;

    case ITEM_SHIELD :
    case ITEM_ARMOR :
      page( ch, "   Enchantment: %-8d Global AC: %d\r\n\r\n", obj->value[0], obj->value[2] );
      break;

    case ITEM_REAGENT :
      page( ch, "       Charges: %d\r\n\r\n", obj->value[0] );
      break;

    case ITEM_DRINK_CON:
      page( ch, "      Fullness: %-10d  Liquid: %d (%s)\r\n", obj->value[1], obj->value[2], (obj->value[2] < 0 || obj->value[2] >= MAX_ENTRY_LIQUID) ? "none" : liquid_table[ obj->value[2] ].name );
      break;

    case ITEM_FOUNTAIN:
      page( ch, "        Liquid: %d (%s)\r\n", obj->value[2], (obj->value[2] < 0 || obj->value[2] >= MAX_ENTRY_LIQUID) ? "none" : liquid_table[ obj->value[2] ].name );
      break;

    case ITEM_FOOD:
      page( ch, "    Cookedness: %d\r\n", obj->value[1] );
      break;

    case ITEM_GATE:
      page( ch, "  Gate to Vnum: %d\r\n", obj->value[1] );
      break;

    /*
    case ITEM_ARROW:
        page( ch, "        Damage: %dd%d\r\n", obj->value[0], obj->value[1] );
        break;
    */
 
    case ITEM_FIRE:
      key = get_obj_index( obj->value[1] );
      page( ch, "   Fire Object: %d (%s)\r\n", obj->value[1], (key == NULL ? "none" : key->Name()) );
      break;
    }
    page( ch, "\r\n" );
  }

  page( ch, "Prefix_S: %s\r\n", obj_clss->prefix_singular );
  page( ch, "Singular: %s\r\n", obj_clss->singular );
  page( ch, "  Long_S: %s\r\n", obj_clss->long_s );
  page( ch, "Prefix_P: %s\r\n", obj_clss->prefix_plural );
  page( ch, "  Plural: %s\r\n", obj_clss->plural );
  page( ch, "  Long_P: %s\r\n", obj_clss->long_p );
  page( ch, "  Before: %s\r\n", obj_clss->before );
  page( ch, "   After: %s\r\n", obj_clss->after );

  show_extras( ch, obj_clss->extra_descr );

  if( obj_clss->fakes != obj_clss->vnum ) {
    sprintf( tmp, "[ before ][ from Obj %d ]\r\n%s", obj_clss->fakes, before_descr( obj_clss ) );
    page( ch, tmp );
  }   

  for( int i = 0; i < obj_clss->affected; i++ ) {
    paf = obj_clss->affected[i];
    if( paf->type == AFF_NONE ) {
      for( int j = 0; j < MAX_AFF_MODS; j++ ) {
        if( paf->mlocation[ j ] > 0 )
          page( ch, "Affects %s by %d.\r\n", affect_location[ paf->mlocation[ j ] ], paf->mmodifier[ j ] );
      }
    }
  }

  page( ch, "\r\n" );
  obj_clss->DisplaySynergyList( wizard );
/*
  page( ch, "\r\n" );
  page_title( ch, "Variables of %s", obj_clss->Name( ));

  if( obj->variables.size > 0 ) {
    variable_nav list( &obj->variables );
    for( variable_data* var = list.first( ); var; var = list.next( ))
      page( ch, "%39s : %-5d\r\n", var->get_name( ), var->get_value( ) );
  }
  else
    page( ch, "<none defined>\r\n" );
*/
}


/*
 *   OWHERE
 */


int select_owhere( obj_clss_data* obj_clss, obj_data* obj, char_data* ch, char* argument )
{
  char               tmp  [ MAX_INPUT_LENGTH ];
  char            letter;
  char            hyphen;
  const char*     string;
  bool          negative;
  int             length;

  for( ; ; ) {
    if( ( hyphen = *argument ) == '\0' )
      return 1;

    if( hyphen != '-' ) {
      letter = 'n';
    } 
    else {
      argument++;
      if( !isalpha( letter = *argument++ ) ) {
        send( ch, "Illegal character for flag - See help owhere.\r\n" );
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
      send( ch, "All flags require an argument - See help owhere.\r\n" );
      return -1;
    }
  
    for( int i = 0; ; ) {
      if( !strncmp( argument-1, " -", 2 ) || *argument == '\0' ) {
        for( ; isspace( tmp[i-1] ); i-- );
        tmp[i] = '\0';
        length = i;
        break;
      }
      if( i > ONE_LINE-2 ) {
        send( ch, "Flag arguments must be less than one line.\r\n" );
        return -1;
      } 
      tmp[i++] = *argument++;
    }

    string = NULL;

    if( obj != NULL ) {
      switch( letter ) {
        case 's' :  string = obj->source;     break;
        case 'l' :  string = obj->label;      break;
      }
    }
  
    if( string != NULL ) {
      if( !strncasecmp( tmp, string, length ) == negative )
        return 0;
      continue;
    }

    if( letter == 'n' ) {
      if( !is_name( tmp, obj != NULL ? obj->Seen_Name( ch ) : obj_clss->Name( ) ) )
        return 0;
      continue;
    }

    send( ch, "Unknown flag - See help owhere.\r\n" );
    return -1;
  }
}


inline void display_owhere( obj_data* obj, char_data* ch, char* tmp, int& length )
{
  char                tmp1  [ ONE_LINE ];
  char                tmp2  [ ONE_LINE ];
  char                tmp3  [ ONE_LINE ];
  room_data*          room  = NULL;
  char_data*         owner  = NULL;   
  obj_data*      container  = NULL;
  thing_data*        where;

  if (!obj) {
    roach("display_owhere: NULL object");
    return;
  }

  for( where = obj; ; ) {
    if( where->array == NULL || ( where = where->array->where ) == NULL )
      break; 
    if( Room( where ) != NULL )
      room = (room_data*) where;
    else if( object( where ) != NULL )
      container = (obj_data*) where;
    else if( character( where ) != NULL )
      owner = (char_data*) where;
  } 

  strcpy( tmp1, room == NULL ? "Nowhere??" : room->name );
  strcpy( tmp2, owner == NULL ? "" : owner->Name( ) );
  strcpy( tmp3, container == NULL ? "" : container->Name( ) );

  truncate( tmp1, 25 );
  truncate( tmp2, 20 );
  truncate( tmp3, 20 );

  sprintf( &tmp[length], "%26s%7d%5s  %-20s %s\r\n",
    tmp1, room == NULL ? -1 : room->vnum,
    int4( obj->number ), tmp2, tmp3 );

  length += strlen( &tmp[length] );

  if( length > MAX_STRING_LENGTH-100 ) {
    page( ch, tmp );
    length = 0;
    *tmp = '\0';
  } 
}


void do_owhere( char_data* ch, char* argument )
{
  char                  tmp  [ MAX_STRING_LENGTH ];
  obj_data*             obj;
  obj_clss_data*   obj_clss;
  int                length  = 0;
  bool                found;
  bool                first;
  bool             anything  = FALSE;
  int                  i, j, hits;

  if( *argument == '\0' ) {
    send( ch, "Which objects do you want to find?\r\n" );
    return;
  } 
  else if( number_arg( argument, i ) ) {
    if( ( obj_clss = get_obj_index( i ) ) == NULL ) {
      send( ch, "No object has that vnum.\r\n" );
      return;
    }
    if( ( found = ( select_owhere( obj_clss, NULL, ch, argument ) != 0 ) ) ) 
      page_divider( ch, obj_clss->Name( ), i );
    else {
      page_centered( ch, "None found" );
      return;
    }

    sprintf( tmp, "%26s%7s%5s  %-20s %s\r\n", "Room", "Vnum", "Nmbr", "Carried By", "Container" );
    page_underlined( ch, tmp );

    first = TRUE;

    for(j = 0; j < obj_list; j++ ) {
      obj = obj_list[j];
      if( !obj || !obj->Is_Valid() || obj->pIndexData != obj_clss )
        continue;

      switch( select_owhere( obj_clss, obj, ch, argument ) ) {
      case -1 :
        return;
      case  1 : 
        if( !found ) {
          found = TRUE;
          page_divider( ch, obj_clss->Name( ), i );
        }
        first = FALSE;
        display_owhere( obj, ch, tmp, length );
        break;
      }
    }
 
    if( found ) {
      if( first )
        page_centered( ch, "None found" );
      else {
        length = 0;
        page( ch, tmp );
      } 
    }

    return;
  }

  sprintf( tmp, "%26s%7s%5s  %-20s %s\r\n", "Room", "Vnum", "Nmbr", "Carried By", "Container" );
  page_underlined( ch, tmp );

  for( i = hits = 0; i < MAX_OBJ_INDEX; i++ ) {
    if( ( obj_clss = obj_index_list[i] ) == NULL )
      continue;

    if( ( found = ( select_owhere( obj_clss, NULL, ch, argument ) != 0 ) ) ) 
      page_divider( ch, obj_clss->Name( ), i );
    else
      continue;

    first = TRUE;

    for( j = 0; j < obj_list; j++ ) {
      obj = obj_list[j];
      if( !obj || !obj->Is_Valid() || obj->pIndexData != obj_clss )
        continue;

      switch( select_owhere( obj_clss, obj, ch, argument ) ) {
      case -1 :
        return;
      case  1 : 
        if( !found ) {
          found = TRUE;
          page_divider( ch, obj_clss->Name( ), i );
        }
        first = FALSE;
        display_owhere( obj, ch, tmp, length );
        break;
      }
    }
 
    if( found ) {
      if( first )
        page_centered( ch, "None found" );
      else {
        length = 0;
        page( ch, tmp );
      } 
      anything = TRUE;
    }

    if( hits++ > 10 ) {
      page_centered( ch, "**** Too many hits.  Please refine search. ****" );
      return;
    }
  }

  if( !anything )
    page_centered( ch, "Nothing found" );
}
















