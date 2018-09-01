#include "system.h"

void read_string( FILE*, char**, char, int );

/*
 *   LOCAL_CONSTANTS
 */

const char* item_type_name [] = { "other", "light", "scroll", "wand",
  "staff", "weapon", "gem", "spellbook", "treasure", "armor", "potion",
  "reagent", "furniture", "trash", "cross", "container", "lock.pick", 
  "drink.container", "key", "food", "money", "key.ring", "boat",
  "corpse", "bait", "fountain", "whistle", "trap",
  "light.perm", "bandage", "bounty", "gate", "arrow", "skin",
  "body.part", "chair", "table", "book", "pipe", "tobacco", "deck.cards",
  "fire", "garrote", "shield", "herb", "flower", "jewelry", "ore",
  "tree", "plant", "charm", "charm.bracelet", "toy", "apparel",
  "dragon.scale", "shot", "bolt", "s.corpse", "musical", "poison",
  "ancient.oak.tree", "statue" };

const char* item_values [] = { "unused",                            //other
  "unused | unused | life time | unused",                           //light
  "spell | level | duration | unused",                              //scroll
  "spell | level | duration | charges",                             //wand
  "spell | level | duration | charges",                             //staff
  "enchantment | damdice | damside | weapon class",                 //weapon
  "hardness | condition",                                           //gem
  "unused",                                                         //spell book
  "unused",                                                         //treasure
  "enchantment | armor value | bracer ac | unused",                 //armor
  "spell | level | duration | unused",                              //potion
  "charges",                                                        //reagent
  "unused",                                                         //furniture
  "unused",                                                         //trash
  "unused",                                                         //cross
  "capacity | container flags | key vnum | unused",                 //container
  "pick modifier | unused",                                         //pick modifier?
  "capacity | contains | liquid | poisoned == -1",                  //drink container
  "unused",                                                         //key
  "food value | cooked? | unused | poisoned == -1",                 //food
  "unused",                                                         //money
  "unused",                                                         //key ring
  "unused",                                                         //boat object?!?
  "half-life| species | level of PC corpse | identity of PC",       //corpse
  "unused",                                                         //bait
  "required to be -1 | unused | liquid | poisoned == -1",           //fountain
  "range | unused",                                                 //whistle
  "trap flags | damdice | damside | unused",                        //trap, values unused?
  "unused | unused | lifetime | unused",                            //permanent light, duration?
  "unused",                                                         //bandage
  "unused",                                                         //bounty?
  "lifetime | unused",                                              //gate
  "unused | damdice | damside | shots",                             //arrow
  "unused",                                                         //skin
  "unused",                                                         //body part
  "seats | unused",                                                 //chair
  "unused",                                                         //table
  "unused",                                                         //book
  "unused",                                                         //pipe
  "unused",                                                         //tobacco
  "unused",                                                         //deck of cards
  "timer | next item | unused",                                     //fire
  "unused",                                                         //garrote
  "enchantment | armor value | bracer ac | unused",                 //shield
  "unused",                                                         //herb
  "unused",                                                         //flower
  "unused",                                                         //jewelry
  "unused",                                                         //ore
  "unused",                                                         //tree
  "unused",                                                         //plant
  "unused",                                                         //charm
  "unused",                                                         //charm bracelet
  "unused",                                                         //toy
  "unused",                                                         //apparel
  "unused",                                                         //dragon.scale
  "unused | damdice | damside | shots",                             //shot
  "unused | damdice | damside | shots",                             //bolt
  "half-life| species",                                             //s.corpse
  "unused",                                                         //musical
  "unused",                                                         //poison
  "unused",                                                         //ancient tree
  "unused",                                                         //statues
 };

const char *cont_flag_name [] = { "closeable", "pickproof", "closed",
  "locked", "holding" };


/*
 *   OBJ_CLSS_DATA CLASS
 */

Synergy :: Synergy( )
{
  record_new( sizeof( synergy ), MEM_OBJ_CLSS );

  skill  = empty_string;
  clss   = 0;
  amount = 0;
}

Synergy :: ~Synergy( )
{
  record_delete( sizeof( synergy ), MEM_OBJ_CLSS );
}


Obj_Clss_Data :: Obj_Clss_Data( )
{
  record_new( sizeof( obj_clss_data ), MEM_OBJ_CLSS );

  oprog           = NULL;
  date            = -1;
  count           = 0;
}


Obj_Clss_Data :: ~Obj_Clss_Data( )
{
  record_delete( sizeof( obj_clss_data ), MEM_OBJ_CLSS );
  if( item_type == ITEM_MONEY )
    update_coinage();

  // memory leaks galore
}

Obj_Clss_Data :: Obj_Clss_Data( obj_clss_data* obj_copy, const char* name, int vnumber, const char* acreator )
{
  char buf [ MAX_INPUT_LENGTH ];

  record_new( sizeof( obj_clss_data ), MEM_OBJ_CLSS );

  vnum            = vnumber;
  fakes           = vnumber;  // obj_copy->vnum ??

  sprintf( buf, "%ss", name );

  singular        = alloc_string( name, MEM_OBJ_CLSS );
  plural          = alloc_string( buf, MEM_OBJ_CLSS );
  before          = alloc_string( obj_copy->before, MEM_OBJ_CLSS );
  after           = alloc_string( obj_copy->after, MEM_OBJ_CLSS );
  long_s          = alloc_string( obj_copy->long_s, MEM_OBJ_CLSS );
  long_p          = alloc_string( obj_copy->long_p, MEM_OBJ_CLSS );
  prefix_singular = alloc_string( obj_copy->prefix_singular, MEM_OBJ_CLSS );
  prefix_plural   = alloc_string( obj_copy->prefix_plural, MEM_OBJ_CLSS );
  creator         = alloc_string( acreator, MEM_OBJ_CLSS );
  last_mod        = alloc_string( acreator, MEM_OBJ_CLSS );

  item_type       = obj_copy->item_type;
  memcpy( extra_flags, obj_copy->extra_flags, sizeof( extra_flags ) );
  wear_flags      = obj_copy->wear_flags;
  anti_flags      = obj_copy->anti_flags;
  restrictions    = obj_copy->restrictions;
  size_flags      = obj_copy->size_flags;
  materials       = obj_copy->materials;
  memcpy( affect_flags, obj_copy->affect_flags, sizeof( affect_flags ) );
  layer_flags     = obj_copy->layer_flags;
  memcpy( value, obj_copy->value, sizeof( value ) );
  weight          = obj_copy->weight;
  cost            = obj_copy->cost;
  level           = obj_copy->level;
  limit           = obj_copy->limit;
  repair          = obj_copy->repair;
  durability      = obj_copy->durability;
  blocks          = obj_copy->blocks;
  light           = obj_copy->light;
  memcpy( clss_synergy, obj_copy->clss_synergy, sizeof( clss_synergy ) );

  religion_flags  = obj_copy->religion_flags;
  color           = obj_copy->color;

  count           = 0;
  date            = time(0);

  copy_extras( extra_descr, obj_copy->extra_descr );
  oprog           = NULL;  // one day dupe oprogs
}


Obj_Clss_Data :: Obj_Clss_Data( const char* name, int vnumber, const char* acreator )
{
  char buf [ MAX_INPUT_LENGTH ];

  record_new( sizeof( obj_clss_data ), MEM_OBJ_CLSS );

  vnum            = vnumber;
  fakes           = vnumber;

  sprintf( buf, "%ss", name );

  singular        = alloc_string( name, MEM_OBJ_CLSS );
  plural          = alloc_string( buf, MEM_OBJ_CLSS );
  before          = empty_string;
  after           = empty_string;
  long_s          = empty_string;
  long_p          = empty_string;
  prefix_singular = empty_string;
  prefix_plural   = empty_string;
  creator         = alloc_string( acreator, MEM_OBJ_CLSS );
  last_mod        = alloc_string( acreator, MEM_OBJ_CLSS );

  item_type       = ITEM_TRASH;
  vzero( extra_flags, 2 );
  wear_flags      = 0;
  set_bit( &wear_flags, ITEM_TAKE );
  anti_flags      = 0;
  restrictions    = 0;
  size_flags      = 0;
  materials       = 0;
  vzero( affect_flags, AFFECT_INTS );
  layer_flags     = 0;
  vzero( value, 4 );
  weight          = 0;
  cost            = 0;
  level           = 1;
  limit           = -1;
  repair          = 10;
  durability      = 1000;
  blocks          = 0;
  light           = 0;
  vzero( clss_synergy, MAX_NEW_CLSS );

  religion_flags  = 0;
  color           = ANSI_NORMAL;

  count           = 0;
  date            = time(0);
  oprog           = NULL;
}

/*
 *   Obj_Clss_Data Synergy Stuff
 */

void Obj_Clss_Data :: HandleSynergyEdit( wizard_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    // display the synergies
    if( is_empty( synergy_array ) ) {
      page( ch, "There are no synergies on %s.\r\n", this->Name( ) );
      return;
    }

    DisplaySynergyList( ch );
    return;
  } else if( exact_match( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      page( ch, "Which synergy do you wish to delte?\r\n" );
      page( ch, "Syntax: <unsure yet>\r\n" );
      return;
    }

    if( is_empty( synergy_array ) ) {
      page( ch, "There are no synergies to delete.\r\n" );
      return;
    }

    int i = atoi( argument );
    if( i < 1 || i > synergy_array ) {
      page( ch, "Selected synergy '%s' out of range [1-%d].\r\n", argument, synergy_array.size );
      return;
    }

    Synergy*  syn = synergy_array[i-1];
    if( syn != NULL ) {
      synergy_array -= syn;
      page( ch, "Synergy #%i deleted.\r\n", i );
      delete syn;
    } else {
      send( ch, "[BUG] Synergy #%i not found.\r\n", i );
    }
    return;
  } else if( exact_match( argument, "new" ) ) {
    Synergy* syn = new Synergy;

    // add the synergy to the array for that object class
    // then make the immortal edit it
    ch->SynergyEdit  = syn;
    synergy_array   += syn;

    page( ch, "Synergy created and you are editting it.\r\n" );
    return;
  } else if( isdigit( *argument ) ) {

    int i = atoi( argument );
    if( i < 1 || i > synergy_array ) {
      page( ch, "Selected synergy '%s' is out of range [1-%d].\r\n", argument, synergy_array.size );
      return;
    }

    synergy* syn = synergy_array[i-1];
    if( syn != NULL ) {
      ch->SynergyEdit = syn;
      page( ch, "You are now editting synergy #%i.\r\n", i );
    } else {
      send( ch, "[BUG] Synergy #%i not found.\r\n", i );
    }
    return;
  } else {
    send( ch, "Syntax: <unknown>\r\n" );
  }
}

void Obj_Clss_Data :: DisplaySynergyList( wizard_data* ch )
{
  bool  found    = false;

  if( is_empty( synergy_array ) ) {
    page( ch, "%s has no synergies.\r\n", Name( ) );
    return;
  }

  for( int i = 0; i < synergy_array; i++ ) {
    synergy* syn = synergy_array[i];
    if( !found ) {
      page( ch, "Skill specific synergies on %s.\r\n", Name( ) );
      page_underlined( ch, "   #    Skill                     Amount   Class\r\n" );
      found = true;
    }
    page( ch, "[%3i]   %-26s%6i   %s\r\n", i+1, syn->skill, syn->amount, clss_table[ syn->clss ].name );
  }
  return;
}


void Obj_Clss_Data :: HandleSynergySet( wizard_data* ch, char* argument )
{
  synergy* syn = ch->SynergyEdit;

  if( *argument == '\0'  ) {
    HandleSynergyStat( ch );
    return;
  }

  if( syn == NULL ) {
    send( ch, "You are not currently editting a synergy.\r\n" );
    return;
  }

  class int_field int_list [] = {
    { "amount",     -20000,          100,  &syn->amount },
    { "",                0,            0,  NULL         },
  };

  if( process( int_list, ch, "synergy", argument ) )
    return;

  if( exact_match( argument, "skill" ) ) {
    int skill;
    if( ( skill = skill_index( argument ) ) != -1 ) {
      syn->skill = skill_table[skill].name;
      send( ch, "%s now synergizes %s.\r\n", Name( ), skill_table[skill].name );
      return;
    }
    else {
      send( ch, "%s is an invalid skill.\r\n", argument );
      return;
    }
  }

  if( exact_match( argument, "class" ) ) {
    for( int i = 0; i < MAX_CLSS; i++ ) {
      if( fmatches( argument, clss_table[i].name ) ) {
        syn->clss = i;
        send( ch, "%s now synergizes with %s class skill set.\r\n", Name( ), clss_table[i].name );
        return;
      }
    }
  }

  send( ch, "Unknown parameters\r\n" );
  return;
}

void Obj_Clss_Data :: HandleSynergyStat( wizard_data* ch )
{
  synergy* syn = ch->SynergyEdit;

  if( syn == NULL ) {
    send( ch, "You are not editting any synergy right now.\r\n" );
    return;
  }

  page_title( ch, "Object %s - Synergy Being Editted", Name( ) );
  page( ch, "\r\n" );
  page( ch, "   Skill: %s\r\n", syn->skill );
  page( ch, "  Amount: %i\r\n", syn->amount );
  page( ch, "   Class: %s\r\n", clss_table[ syn->clss ].name );
  return;
}

bool Synergy :: Save( FILE* fp )
{
  fprintf( fp, "%d\n", 1 );  // version

  fprintf( fp, "%s~\n", skill );
  fprintf( fp, "%d\n", amount );
  fprintf( fp, "%d\n", clss );

  return true;
}

bool Synergy :: Load( FILE* fp )
{
  int version = fread_number( fp );

  skill  = fread_string( fp, MEM_OBJ_CLSS );
  amount = fread_number( fp );
  clss   = fread_number( fp );

  return true;
}

void do_synedit( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  obj_clss_data* obj = imm->obj_edit;
  if( obj == NULL ) {
    send( ch, "You need to be editting an object to work on synergies.\r\n" );
    return;
  }

  obj->HandleSynergyEdit( imm, argument );
}

void do_synset( char_data* ch, char* argument )
{
  wizard_data* imm = wizard( ch );
  if( imm == NULL )
    return;

  obj_clss_data* obj = imm->obj_edit;
  if( obj == NULL ) {
    send( ch, "You need to be editting an object to work on synergies.\r\n" );
    return;
  }

  if( imm->SynergyEdit == NULL ) {
    send( ch, "You are not editting a synergy.\r\n" );
    return;
  }

  obj->HandleSynergySet( imm, argument );
}


/*
 *   OBJ_DATA
 */


int compare_vnum( obj_data* obj1, obj_data* obj2 )
{
  int a = obj1->pIndexData->vnum;
  int b = obj2->pIndexData->vnum;

  return( a < b ? -1 : ( a > b ? 1 : 0 ) );
}


Obj_Data :: Obj_Data( obj_clss_data* obj_clss )
{
  record_new( sizeof( obj_data ), MEM_OBJECT );

  valid         = OBJ_DATA;
  extra_descr   = NULL;
  array         = NULL;
  save          = NULL;
  owner         = NULL;
  source        = empty_string;
  label         = empty_string;
  reset_mob_vnum = 0;
  reset_room_vnum = 0;
  position      = WEAR_NONE;
  layer         = 0;
  unjunked      = 0;
  pIndexData    = obj_clss;

  insert( obj_list, this, binary_search( obj_list, this, compare_vnum ) );
}


Obj_Data :: ~Obj_Data( )
{
  record_delete( sizeof( obj_data ), MEM_OBJECT );
  obj_list -= this;
}


/*
 *   SUPPORT FUNCTIONS
 */


bool can_extract( obj_clss_data* obj_clss, char_data* ch )
{
  /*
  if( obj_clss->vnum == 1 ) {
    send( ch, "You may not delete the template object.\r\n" );
    return FALSE;
  }
  */

  for( int i = 0; i < obj_list; i++ ) {
    if( obj_list[i]->Is_Valid() && obj_list[i]->pIndexData == obj_clss ) {
      send( ch, "You must destroy all examples of %s first.\r\n", obj_clss );
      return FALSE;
    }
  }

  if( generic_search( ch, obj_clss, obj_clss->vnum, OBJECT, true ) ) {
    send( ch, "You must remove all references of that object first.\r\n" );
    return FALSE;
  }

  return TRUE;
}        
  

/*
 *   LOW LEVEL OBJECT ROUTINES
 */

int  coin_value_in_condition( obj_data* obj )
{
  int cost = 0;

  if( obj == NULL )
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

  cost += cost*12/(3+obj->number);

  return cost;
}

bool is_same( obj_data* obj1, obj_data* obj2 )
{
  obj_array*  array;

  if( obj1->pIndexData != obj2->pIndexData )
    return FALSE;

  if( obj1->pIndexData->item_type == ITEM_MONEY )
    return TRUE; // perhaps money-specific checks later if you want 'dented gold coin' etc

  if( !is_empty( obj1->contents )  || !is_empty( obj2->contents )
    || !is_empty( obj1->affected ) || !is_empty( obj2->affected )
    || !is_empty( obj1->events )   || !is_empty( obj2->events )
    || obj1->extra_flags[0] != obj2->extra_flags[0] 
    || obj1->extra_flags[1] != obj2->extra_flags[1] 
    || obj1->timer != obj2->timer
    || obj1->condition != obj2->condition 
    || obj1->materials != obj2->materials
    || obj1->rust != obj2->rust
    || obj1->owner != obj2->owner
    || obj1->weight != obj2->weight
    || obj1->unjunked != obj2->unjunked
    || obj1->position != obj2->position )
    return FALSE;

  for( int i = 0; i < 4; i++ )
    if( obj1->value[i] != obj2->value[i] )
      return FALSE;

  if( obj2->save != NULL ) {
    if( obj1->save == NULL ) {
      obj1->save = obj2->save;
      obj2->save = NULL;
      array      = &obj1->save->save_list;
      for( int i = 0; ; i++ ) {
        if( i >= array->size ) 
          panic( "Is_Same: Object not in save array." );       
        if( obj2 == array->list[i] ) {
          array->list[i] = obj1;
          break;
        }
      }
    } else if( obj1->save != obj2->save )
      return FALSE;
  }

  if( obj1->pIndexData->vnum == OBJ_BALL_OF_LIGHT && range( 0, obj1->value[2] / 5, 10 ) != range( 0, obj2->value[2] / 5, 10 ) )
    return FALSE;
  if( obj1->pIndexData->vnum == OBJ_CORPSE_LIGHT && ( obj1->value[3] > 0 ? range( 0, 6 * obj1->value[2] / obj1->value[3], 6 ) : 6 ) != ( obj2->value[3] > 0 ? range( 0, 6 * obj2->value[2] / obj2->value[3], 6 ) : 6 ) )
    return FALSE;

  if( obj1->pIndexData->item_type == ITEM_CORPSE )
    return FALSE;

  if( obj1->source != obj2->source && strcmp( obj1->source, obj2->source ) != 0 )
    return FALSE;

  return( !strcmp( obj1->label, obj2->label ) );
}


/*
 *   OWNERSHIP
 */


bool Obj_Data :: Belongs( char_data* ch )
{
  return( owner == NULL || ( ch != NULL && ch->pcdata != NULL
    && ch->pcdata->pfile == owner ) );
}


void set_owner( pfile_data* pfile, thing_array& array, bool force_transfer )
{
  obj_data* obj;

  for( int i = 0; i < array; i++ ) {
     if( ( obj = object( array[i] ) ) != NULL ) {
        if ( obj->pIndexData->item_type != ITEM_MONEY ) {
           if( obj->owner == NULL || force_transfer )
              obj->owner = pfile; 
        }
        set_owner( pfile, obj->contents, force_transfer );
     }
  }
}


void set_owner( obj_data* obj, pfile_data* buyer )
{
  obj_data* content;

  if( obj->pIndexData->item_type != ITEM_MONEY )
    obj->owner = buyer;

  for( int i = 0; i < obj->contents; i++ )
    if( ( content = object( obj->contents[i] ) ) != NULL )
      set_owner( content, buyer );
}

  
void set_owner( obj_data* obj, char_data* buyer, char_data* seller )
{
  obj_data* content;

  if( obj->Belongs( seller ) && obj->pIndexData->item_type != ITEM_MONEY )
    obj->owner = ( ( buyer != NULL && buyer->pcdata != NULL )
      ? buyer->pcdata->pfile : NULL );

  for( int i = 0; i < obj->contents; i++ )
    if( ( content = object( obj->contents[i] ) ) != NULL ) 
      set_owner( content, buyer, seller );
}


/*
 *   MISC ROUTINES
 */


void condition_abbrev( char* tmp, obj_data* obj, char_data* ch )
{
  const char* abbrev [] = { "wls", "dmg", "vwn", "wrn", "vsc", "scr",
    "rea", "goo", "vgo", "exc" };

  int i;

  i = 1000*obj->condition/obj->pIndexData->durability; 
  i = range( 0, i/100, 9 );

  sprintf( tmp, "%s%s%s", i > 4 ? ( i > 7 ? blue( ch ) : green( ch ) ) 
    : ( i > 2 ? yellow( ch ) : red( ch ) ), abbrev[i], normal( ch ) );

  return;
}


void age_abbrev( char* tmp, obj_data*, char_data* )
{
  sprintf( tmp, "   " );

  return;
}

const char* obj_data :: repair_condition_name( char_data* ch, bool ansi )
{
  static char  tmp  [ ONE_LINE ];
  int            i;
  const char*  txt;

  i = 1000 * repair_condition(this) / pIndexData->durability; 
  i = range( 0, i/100, 9 );

       if( i == 0 ) txt = "worthless";
  else if( i == 1 ) txt = "damaged";
  else if( i == 2 ) txt = "very worn";
  else if( i == 3 ) txt = "worn";
  else if( i == 4 ) txt = "very scratched";
  else if( i == 5 ) txt = "scratched";
  else if( i == 6 ) txt = "reasonable";
  else if( i == 7 ) txt = "good";
  else if( i == 8 ) txt = "very good";
  else              txt = "excellent";

  if( !ansi || ch->pcdata == NULL || ch->pcdata->terminal != TERM_ANSI )
    return txt;

  sprintf( tmp, "%s%s%s", i > 4 ? ( i > 7 ? blue( ch ) : green( ch ) ) 
    : ( i > 2 ? yellow( ch ) : red( ch ) ), txt, normal( ch ) );

  return tmp;
}

const char* obj_data :: condition_name( char_data* ch, bool ansi )
{
  static char  tmp  [ ONE_LINE ];
  int            i;
  const char*  txt;

  i = 1000*condition/pIndexData->durability; 
  i = range( 0, i/100, 9 );

       if( i == 0 ) txt = "worthless";
  else if( i == 1 ) txt = "damaged";
  else if( i == 2 ) txt = "very worn";
  else if( i == 3 ) txt = "worn";
  else if( i == 4 ) txt = "very scratched";
  else if( i == 5 ) txt = "scratched";
  else if( i == 6 ) txt = "reasonable";
  else if( i == 7 ) txt = "good";
  else if( i == 8 ) txt = "very good";
  else              txt = "excellent";

  if( !ansi || ch->pcdata == NULL || ( ch->pcdata->terminal != TERM_ANSI 
     && ch->pcdata->terminal != TERM_MXP ) )
    return txt;

  sprintf( tmp, "%s%s%s", i > 4 ? ( i > 7 ? blue( ch ) : green( ch ) ) 
    : ( i > 2 ? yellow( ch ) : red( ch ) ), txt, normal( ch ) );

  return tmp;
}


/*
 *   WEIGHT/NUMBER ROUTINES
 */


int Obj_Data :: Cost( )
{
  int   cost  = pIndexData->cost;
  int      i;

  if( ( pIndexData->item_type == ITEM_WEAPON || pIndexData->item_type == ITEM_ARMOR || pIndexData->item_type == ITEM_SHIELD ) && is_set( extra_flags, OFLAG_IDENTIFIED ) )
    cost += cost*sqr( URANGE( 0, value[0], 5 ) )/2;

  if( ( pIndexData->item_type != ITEM_WEAPON && pIndexData->item_type != ITEM_ARMOR && pIndexData->item_type != ITEM_SHIELD ) || !is_set( pIndexData->extra_flags, OFLAG_RANDOM_METAL ) )
    return cost;

  for( i = MAT_BRONZE; i <= MAT_LIRIDIUM; i++ )
    if( is_set( &pIndexData->materials, i ) ) 
      return cost*material_table[i].cost;

  return cost;
}


/*
 *   OBJECT UTILITY ROUTINES
 */


void enchant_object( obj_data* obj )
{
  int i;

  if( obj->pIndexData->item_type == ITEM_WAND
    || obj->pIndexData->item_type == ITEM_STAFF ) 
    obj->value[3] = number_range( 0, obj->pIndexData->value[3] );

  if( ( obj->pIndexData->item_type == ITEM_WEAPON
    || obj->pIndexData->item_type == ITEM_SHIELD
    || obj->pIndexData->item_type == ITEM_ARMOR )
    && !is_set( obj->extra_flags, OFLAG_NO_ENCHANT ) ) {
    if( ( i = number_range( 0, 1000 ) ) >= 900 ) {
      obj->value[0] = ( i > 950 ? ( i > 990 ? ( i == 1000 ? 3 : 2 ) : 1 )
        : ( i < 910 ? ( i == 900 ? -3 : -2 ) : -1 ) );
      set_bit( obj->extra_flags, OFLAG_MAGIC );
      if( obj->value[0] < 0 )
        set_bit( obj->extra_flags, OFLAG_NOREMOVE );
      }
    }
}


void rust_object( obj_data* obj, int chance )
{
  int i;

  if( obj->metal( ) && !is_set( obj->extra_flags, OFLAG_RUST_PROOF ) && number_range( 0, 100 ) < chance ) {
    i = number_range( 0, 1000 );
    obj->rust = ( i > 700 ? 1 : ( i > 400 ? 2 : 3 ) );   
  }

  if( obj->pIndexData->item_type == ITEM_WEAPON || obj->pIndexData->item_type == ITEM_SHIELD || obj->pIndexData->item_type == ITEM_ARMOR ) {
    obj->age = number_range( 0, obj->pIndexData->durability/25-1 );
    obj->condition = number_range( 1, repair_condition( obj ) );
  }
}


void set_alloy( obj_data* obj, int level )
{
  int metal;

  if( !is_set( obj->pIndexData->extra_flags, OFLAG_RANDOM_METAL ) )
    return;

  for( metal = MAT_BRONZE; metal <= MAT_LIRIDIUM; metal++ )
    if( is_set( &obj->materials, metal ) )
      break;

  if( metal > MAT_LIRIDIUM ) {
    metal = MAT_BRONZE;
    for( ; ; ) {
      if( metal == MAT_LIRIDIUM || number_range( 0, level+75 ) > level-10*(metal-MAT_BRONZE) )
        break;
      metal++;
    }
    set_bit( &obj->materials, metal );
  }

  if( obj->pIndexData->item_type == ITEM_ARMOR || obj->pIndexData->item_type == ITEM_SHIELD )
    obj->value[1] = obj->pIndexData->value[1]-MAT_BRONZE+metal;
}


obj_data* create( obj_clss_data* obj_clss, int number )
{
  obj_data* obj;

  if( obj_clss == NULL ) {
    roach( "Create_object: NULL obj_clss." );
    return NULL;
  }

  obj = new obj_data( obj_clss );

  obj->singular = obj_clss->singular;
  obj->plural   = obj_clss->plural;
  obj->after    = obj_clss->after;
  obj->before   = obj_clss->before;

  obj->extra_flags[0] = obj_clss->extra_flags[0];
  obj->extra_flags[1] = obj_clss->extra_flags[1];
  
  if( is_set( &obj_clss->size_flags, SFLAG_CUSTOM ) )
    obj->size_flags  = -1;
  else
    obj->size_flags = obj_clss->size_flags;

  obj->value[0]   = obj_clss->value[0];
  obj->value[1]   = obj_clss->value[1];
  obj->value[2]   = obj_clss->value[2];
  obj->value[3]   = obj_clss->value[3];
  obj->condition  = obj_clss->durability;
  obj->materials  = obj_clss->materials;
  obj->weight     = obj_clss->weight;
  obj->age        = 0; 
  obj->rust       = 0;
  obj->timer      = 0; 

  if( number > 0 )
    obj_clss->count += number;
  else
    number = -number;

  obj->number     = number;
  obj->selected   = number;
  obj->shown      = number;


  switch( obj_clss->item_type ) {
  case ITEM_MONEY:
    obj->value[0]  = obj->pIndexData->cost;
    break;
  case ITEM_FIRE:
  case ITEM_GATE:
    obj->timer = obj->value[0];
    break;
  }

  if( obj_clss->item_type != ITEM_ARMOR && obj_clss->item_type != ITEM_WEAPON
    && obj_clss->item_type != ITEM_SHIELD
    && !strcmp( obj_clss->before, obj_clss->after )
    && obj_clss->plural[0] != '{' && obj_clss->singular[0] != '{' )
    set_bit( obj->extra_flags, OFLAG_IDENTIFIED ); 

  return obj;
}


obj_data* duplicate( obj_data* copy, int num )
{
  obj_data*            obj;
  obj_clss_data*  obj_clss  = copy->pIndexData;

  obj = new obj_data( obj_clss );

  char* string_copy [] = { copy->singular, copy->plural, copy->before, copy->after }; 
  char** string_obj [] = { &obj->singular, &obj->plural, &obj->before, &obj->after }; 
  char* string_index [] = { obj_clss->singular, obj_clss->plural, obj_clss->before, obj_clss->after }; 

  for( int i = 0; i < 4; i++ ) 
    *string_obj[i] = ( string_copy[i] != string_index[i] ? alloc_string( string_copy[i], MEM_OBJECT ) : string_index[i] );
  
  obj->age            = copy->age;
  obj->extra_flags[0] = copy->extra_flags[0];
  obj->extra_flags[1] = copy->extra_flags[1];
  obj->size_flags     = copy->size_flags;
  obj->value[0]       = copy->value[0];
  obj->value[1]       = copy->value[1];
  obj->value[2]       = copy->value[2];
  obj->value[3]       = copy->value[3];
  obj->weight         = copy->weight;
  obj->condition      = copy->condition;
  obj->rust           = copy->rust;
  obj->timer          = copy->timer; 
  obj->materials      = copy->materials;
  obj->owner          = copy->owner;

  obj->temp     = copy->temp;
  obj->number   = num;   
  obj->selected = num;   
  obj->shown    = num;

  obj_clss->count += num;

  if( copy->save != NULL ) {
    copy->save->save_list += obj;
    obj->save = copy->save;
  }

  return obj;
}


/*
 *   OBJECT TRANSFER FUNCTIONS
 */


int drop_contents( obj_data* obj )
{
  room_data*     room;

  if( obj == NULL || obj->array == NULL || (room = Room(obj->array->where)) == NULL )
    return 0;

  int num = 0;
  for( int i = obj->contents.size - 1; i >= 0; i-- ) {
    obj_data *content = object(obj->contents[i]);
    if (!content || !content->Is_Valid())
      continue;
    content->From(content->number);
    content->To(room);
    num++;
  }

  return num;
}


/*
 *   OBJECT EXTRACTION ROUTINES
 */


void Obj_Data :: Extract( int i )
{
  if( i < number ) {
    remove_weight( this, i );
    number -= i;
    if( boot_stage == BOOT_COMPLETE )
      pIndexData->count -= i;
    return;
  }

  if( i > number ) {
    roach( "Extract( Obj ): number > amount." );
    roach( "-- Obj = %s", this );
    roach( "-- Number = %d", i ); 
    roach( "-- Amount = %d", number );
  }

  Extract( );
}


void Obj_Data :: Extract( )
{
  obj_array*    array;
  int               i;

  if( !Is_Valid( ) ) {
    roach( "Extracting invalid object." );
    roach( "-- Valid = %d", valid );
    roach( "--   Obj = %s", this );
    return;
  }

  if( pIndexData->vnum == OBJ_CORPSE_PC )
    corpse_list -= this;

  if( this->array != NULL ) 
    From( number );

  extract( contents );

  if( boot_stage == BOOT_COMPLETE )
    pIndexData->count -= number;

  clear_queue( this );
  stop_events( this );

  if( save != NULL ) {
    array = &save->save_list;
    for( i = 0; ; i++ ) {
      if( i >= array->size ) 
        panic( "Extract: Object not found in save array." );
      if( this == array->list[i] ) {
        array->list[i] = NULL;
        save = NULL;
        break;
      }
    }
  }

  delete_list( affected );
  free_string( source, MEM_OBJECT );

  if( singular != pIndexData->singular )
    free_string( singular, MEM_OBJECT );
  if( plural != pIndexData->plural )
    free_string( plural,   MEM_OBJECT );
  if( after != pIndexData->after )
    free_string( after,    MEM_OBJECT );
  if( before != pIndexData->before )
    free_string( before,   MEM_OBJECT );

  timer      = -2;
  valid      = -1;

  extracted += this;
}


/*
 *   DISK ROUTINES
 */


void fix( obj_clss_data* obj_clss )
{
  if( obj_clss->item_type == ITEM_SCROLL ) 
    set_bit( &obj_clss->materials, MAT_PAPER );

  for( int i = 0; i < MAX_ANTI; i++ ) {
    if( !strncasecmp( anti_flags[i], "unused", 6 ) )
      remove_bit( &obj_clss->anti_flags, i );
  }

  if( obj_clss->color < COLOR_DEFAULT || obj_clss->color > MAX_COLOR )
    obj_clss->color = COLOR_DEFAULT;

  return;
}


void load_objects( void )
{
  FILE*                 fp;
  obj_clss_data*  obj_clss;
  oprog_data*        oprog;
  char              letter;
  int                    i;
  int                 vnum;
  int                count = 0;

  bool boot_old = false, boot_tfh = false;
  int version = 0;

  echo( "Loading Objects ...\r\n" );
  vzero( obj_index_list, MAX_OBJ_INDEX );

  fp = open_file( AREA_DIR, OBJECT_FILE, "rb", TRUE );

  char *word = fread_word( fp );
  if( !strcmp( word, "#M2_OBJECTS" ) ) {
    version = fread_number( fp );
  } else if( !strcmp( word, "#OLD_OBJECTS" ) ) {
    log("... old style object file");
    boot_old = true;
  } else if( !str_cmp( word, "#TFH_OBJECTS" ) ) {
    log("... tfh style object file");
    boot_tfh = true;
  } else if( !strcmp( word, "#OBJECTS" ) ) {
    // version 0 object file
  } else {
    panic( "Load_objects: header not found" );
  }

  log( "  * %-20s : v%d :", OBJECT_FILE, version );

  for( ; ; ) {
    letter = fread_letter( fp );

    if( letter != '#' ) 
      panic( "Load_objects: # not found." );

    if( ( vnum = fread_number( fp ) ) == 0 )
      break;
   
    if( vnum < 0 || vnum >= MAX_OBJ_INDEX ) 
      panic( "Load_objects: vnum out of range." );

    if( obj_index_list[vnum] != NULL ) 
      panic( "Load_objects: vnum %d duplicated.", vnum );

    obj_clss = new obj_clss_data;
 
    obj_index_list[vnum]  = obj_clss;
    obj_clss->vnum        = vnum;
    obj_clss->fakes       = vnum;

    obj_clss->singular         = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->plural           = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->before           = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->after            = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->long_s           = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->long_p           = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->prefix_singular  = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->prefix_plural    = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->creator          = fread_string( fp, MEM_OBJ_CLSS );
    obj_clss->last_mod         = fread_string( fp, MEM_OBJ_CLSS );      

    obj_clss->item_type       = fread_number( fp );
    obj_clss->fakes           = fread_number( fp );
    obj_clss->extra_flags[0]  = fread_number( fp );
    obj_clss->extra_flags[1]  = fread_number( fp );
    obj_clss->wear_flags      = fread_number( fp );

    if( boot_old ) {
      // perform object translation
      int old_flags = obj_clss->wear_flags;
      obj_clss->wear_flags = 0;

      for (int i = 0; i < MAX_WEAR; i++)
        if (is_set(&old_flags, i))
          set_bit(&obj_clss->wear_flags, old_wear_locs[i]);          
    }

    obj_clss->anti_flags      = fread_number( fp );
    obj_clss->restrictions    = fread_number( fp );
    obj_clss->size_flags      = fread_number( fp );
    obj_clss->materials       = fread_number( fp );

    obj_clss->affect_flags[0] = fread_number( fp );
    obj_clss->affect_flags[1] = fread_number( fp );
    obj_clss->affect_flags[2] = fread_number( fp );

    if( version < 3 ) {
      obj_clss->affect_flags[3] = 0;
      obj_clss->affect_flags[4] = 0;
    }
    else {
      obj_clss->affect_flags[3] = fread_number( fp );
      obj_clss->affect_flags[4] = fread_number( fp );
    }

    if( version < 10 ) {
      obj_clss->affect_flags[5] = 0;
      obj_clss->affect_flags[6] = 0;
      obj_clss->affect_flags[7] = 0;
      obj_clss->affect_flags[8] = 0;
      obj_clss->affect_flags[9] = 0;
    }
    else {
      obj_clss->affect_flags[5] = fread_number( fp );
      obj_clss->affect_flags[6] = fread_number( fp );
      obj_clss->affect_flags[7] = fread_number( fp );
      obj_clss->affect_flags[8] = fread_number( fp );
      obj_clss->affect_flags[9] = fread_number( fp );
    }

    obj_clss->layer_flags     = fread_number( fp );

    obj_clss->value[0]      = fread_number( fp );
    obj_clss->value[1]      = fread_number( fp );
    obj_clss->value[2]      = fread_number( fp );
    obj_clss->value[3]      = fread_number( fp );

    obj_clss->weight        = fread_number( fp );
    obj_clss->cost          = fread_number( fp );
    obj_clss->level         = fread_number( fp );
    obj_clss->limit         = fread_number( fp );
    obj_clss->repair        = fread_number( fp );
    obj_clss->durability    = fread_number( fp );
    obj_clss->blocks        = fread_number( fp );
    obj_clss->light         = fread_number( fp );

    if( version < 6 )
      obj_clss->color       = 0;
    else
      obj_clss->color       = fread_number( fp );

    if( version < 7 )
      obj_clss->religion_flags       = 0;
    else
      obj_clss->religion_flags       = fread_number( fp );

    if( version < 5 ) {
      obj_clss->clss_synergy[ CLSS_THIEF+1    ] = 0;
      obj_clss->clss_synergy[ CLSS_MAGE+1     ] = 0;
      obj_clss->clss_synergy[ CLSS_SORCERER+1 ] = 0;
      obj_clss->clss_synergy[ CLSS_CLERIC+1   ] = 0;
      obj_clss->clss_synergy[ CLSS_DANCER+1   ] = 0;
    }
    else {
      obj_clss->clss_synergy[ CLSS_THIEF+1    ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_MAGE+1     ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_SORCERER+1 ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_CLERIC+1   ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_DANCER+1   ] = fread_number( fp );
    }

    if( version < 10 )
      obj_clss->clss_synergy[ CLSS_DRUID+1 ]    = 0;
    else
      obj_clss->clss_synergy[ CLSS_DRUID+1 ]    = fread_number( fp );

    if( version < 11 ) {
      obj_clss->clss_synergy[ CLSS_WARRIOR+1   ] = 0;
      obj_clss->clss_synergy[ CLSS_CAVALIER+1  ] = 0;
      obj_clss->clss_synergy[ CLSS_BARBARIAN+1 ] = 0;
      obj_clss->clss_synergy[ CLSS_PALADIN+1   ] = 0;
      obj_clss->clss_synergy[ CLSS_RANGER+1    ] = 0;
      obj_clss->clss_synergy[ CLSS_DEFENSIVE+1 ] = 0;
      obj_clss->clss_synergy[ CLSS_MONK+1      ] = 0;
      obj_clss->clss_synergy[ CLSS_ROGUE+1     ] = 0;
      obj_clss->clss_synergy[ CLSS_ASSASSIN+1  ] = 0;
    }
    else {
      obj_clss->clss_synergy[ CLSS_WARRIOR+1   ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_CAVALIER+1  ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_BARBARIAN+1 ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_PALADIN+1   ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_RANGER+1    ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_DEFENSIVE+1 ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_MONK+1      ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_ROGUE+1     ] = fread_number( fp );
      obj_clss->clss_synergy[ CLSS_ASSASSIN+1  ] = fread_number( fp );
    }
    
    if( version > 8 )
      count = fread_number( fp );

    // Reading skills (not likely, but i am hoping )
/*
    for( int i = 0; i < MAX_SKILL; i++ )
      obj_clss->skill_modifier[i] = 0;

    if( version < 4 ) {
      for( int i = 0; i < MAX_SKILL; i++ )
        obj_clss->skill_modifier[i] = 0;
    }
    else if( version < 8 ) {
      for( int i = 0; i < 600; i++ ) {
        obj_clss->skill_modifier[i] = fread_number( fp );
      }
    }
    else if( version < 9 ) {
      for( int i = 0; i < MAX_SKILL; i++ )
        obj_clss->skill_modifier[i] = fread_number( fp );
    }
    else */if( version < 12 ) {
      if( count > 0 ) {
        for( int i = 0; i < count; i++ ) {
          const char* skill = fread_string( fp, MEM_OBJ_CLSS );
          for( int j = 0; j < MAX_SKILL; j++ ) {
            if( !strcmp( skill, skill_table[j].name ) ) {
//              obj_clss->skill_modifier[j] = fread_number( fp );
              synergy* syn = new Synergy;
              syn->skill = skill_table[j].name;
              syn->amount = fread_number( fp );
              obj_clss->synergy_array += syn;
              break;
            }
            if( j == MAX_SKILL ) {
              fread_number( fp );
            }
          }
        }
      }
    }
    else if( version < 13 ) {
      int size = fread_number( fp );

      for( int i = 0; i < size; i++ ) {
        synergy*  syn = new Synergy;
        syn->Load( fp );
      
        obj_clss->synergy_array += syn;
      }
    }


    if( boot_tfh ) {
      fread_number( fp );
      fread_number( fp );
    }

    obj_clss->date          = fread_number( fp );

    read_affects( fp, obj_clss, version >= 2 ? false : true ); // version 2 object file introduced new affects format
    read_extra( fp, obj_clss->extra_descr );

    fread_letter( fp );

    for( ; ; ) {
      int number = fread_number( fp );

      if( number == -1 )
        break;

      oprog = new oprog_data( obj_clss );
      append( obj_clss->oprog, oprog );

      oprog->trigger  = number;

      if( oprog->trigger == OPROG_TRIGGER_WEAR )
        oprog->trigger = OPROG_TRIGGER_AFTER_USE;

      oprog->obj_vnum = fread_number( fp );
      if( boot_old ) {
        oprog->value = 0;
      } else {
        oprog->value    = fread_number( fp );
      }
      oprog->command  = fread_string( fp, MEM_OPROG );
      oprog->target   = fread_string( fp, MEM_OPROG );
//      oprog->value    = fread_number( fp );
      oprog->code     = fread_string( fp, MEM_OPROG );

      read_extra( fp, oprog->data );
    }       

    fix( obj_clss );
  }

  fclose( fp );

  for( i = 0; i < MAX_OBJ_INDEX; i++ ) 
    if( obj_index_list[i] != NULL )
      for( oprog = obj_index_list[i]->oprog; oprog != NULL; oprog = oprog->next )
        if( oprog->obj_vnum > 0 )
          oprog->obj_act = get_obj_index( oprog->obj_vnum );
 
  return;
}







