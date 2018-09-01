#include "system.h"


const char*   seperate   ( const char*, bool );
void          append_liquid   ( char*, obj_data* );


char    static_storage  [ 10*THREE_LINES ]; 
int         store_pntr  = 0;


bool include_liquid = TRUE;
bool include_empty  = TRUE;
bool include_closed = TRUE;


/*
 *   SUPPORT ROUTINES
 */


const char* color_word [] = {
  "grey",   // 1
  "red",    // 2
  "orange", // 3
  "yellow", // 4
  "green",  // 5
  "cyan",   // 6
  "blue",   // 7
  "indigo", // 8
  "violet", // 9
  "purple", // 10
  "white"   // magic
};

const char* corpse_light_color_word [] = {
  "dying,",        // 1
  "whispering,",   // 2
  "sobbing,",      // 3
  "whimpering,",   // 4
  "moaning,",      // 5
  "howling,",      // 6
  "screaming,"     // magic
};

const char* firefly_word [] = {
  "few luminous",           // 1
  "several luminous",       // 2
  "pack of luminous",       // 3
  "covey of luminous",      // 4
  "crowd of luminous",      // 5
  "horde of luminous",      // 6
  "collection of luminous", // 7
  "host of luminous",       // 8
  "multitude of luminous",  // 9
  "swarm of luminous",      // 10
  "myriad of luminous",     // magic
};
 
const char* gem_quality [] = { "uncut", "flawless", "blemished", "fractured" };


void get_obj_adj( char* tmp, char_data* ch, obj_data* obj )
{
  int metal;

  *tmp = '\0';

  for( metal = MAT_BRONZE; metal <= MAT_LIRIDIUM; metal++ )
    if( is_set( &obj->materials, metal ) )
      break;

  /*
  if( is_set( obj->pIndexData->size_flags, SFLAG_RANDOM ) ) {
    for( i = SFLAG_TINY; i < SFLAG_GIANT; i++ ) 
      if( is_set( &obj_clss->size_flags, i ) )
        break;
  */

  if( ch != NULL ) {
    if( is_apprentice( ch ) && obj->source != NULL && obj->source != empty_string )
      sprintf( tmp+strlen( tmp ), "%s[oload:%s]", ( tmp[0] == '\0' ? "" : ", " ), obj->source );

    if( is_set( obj->extra_flags, OFLAG_EVIL ) && IS_AFFECTED( ch, AFF_DETECT_EVIL ) )
      sprintf( tmp+strlen( tmp ), "%s+evil+", tmp[0] == '\0' ? "" : ", " ); 

    if( is_set( obj->extra_flags, OFLAG_GOOD ) && IS_AFFECTED( ch, AFF_DETECT_GOOD ) )
      sprintf( tmp+strlen( tmp ), "%s+good+", tmp[0] == '\0' ? "" : ", " ); 
  }

  if( obj->rust != 0 && metal <= MAT_LIRIDIUM )
    strcpy( tmp, material_table[ metal ].rust[ obj->rust-1 ] );

  if( is_set( obj->extra_flags, OFLAG_IS_INVIS ) )
    sprintf( tmp+strlen( tmp ), "%sinvisible", tmp[0] == '\0' ? "" : ", " ); 

  if( ch != NULL && IS_AFFECTED( ch, AFF_DETECT_MAGIC )
    && is_set( obj->extra_flags, OFLAG_MAGIC ) )
    sprintf( tmp+strlen( tmp ), "%senchanted", tmp[0] == '\0' ? "" : ", " ); 

  if( is_set( obj->extra_flags, OFLAG_BURNING ) )
    sprintf( tmp+strlen( tmp ), "%sburning",
      tmp[0] == '\0' ? "" : ", " ); 

  if( is_set( obj->extra_flags, OFLAG_FLAMING ) )
    sprintf( tmp+strlen( tmp ), "%sflaming",
      tmp[0] == '\0' ? "" : ", " ); 

  if( is_set( obj->extra_flags, OFLAG_POISON_COATED ) )
    sprintf( tmp+strlen( tmp ), "%spoison coated",
      tmp[0] == '\0' ? "" : ", " ); 

  if( is_set( obj->extra_flags, OFLAG_DISEASED ) )
    sprintf( tmp+strlen( tmp ), "%sdiseased",
      tmp[0] == '\0' ? "" : ", " ); 

  switch( obj->pIndexData->item_type ) {
    case ITEM_DRINK_CON : 
      if( obj->value[1] == 0 && include_empty ) 
        sprintf( tmp+strlen( tmp ), "%sempty", *tmp == '\0' ? "" : ", " );
      break;

    case ITEM_FOOD :
      if( obj->value[1] >= 0 && obj->value[1] < 3 ) 
        sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : ", ",
          cook_word[ obj->value[1] ] );
      break;

    case ITEM_CONTAINER :
      if( include_closed ) {
        if( is_set( &obj->value[1], CONT_CLOSED ) )
          sprintf( tmp+strlen( tmp ), "%sclosed", *tmp == '\0' ? "" : ", " );
        else if( obj->contents == 0 ) 
          sprintf( tmp+strlen( tmp ), "%sempty", *tmp == '\0' ? "" : ", " );
        }

      /*
    case ITEM_GEM :
      sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : ", ",
        gem_quality[ obj->value[1] ] );
      break;        
      */
    }

  if( obj->pIndexData->vnum == OBJ_BALL_OF_LIGHT ) 
    sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : ", ", color_word[ range( 0, obj->value[2] / 5, 10 ) ] );
  if( obj->pIndexData->vnum == OBJ_CORPSE_LIGHT )
    sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : ", ", corpse_light_color_word[ obj->value[3] > 0 ? range( 0, 6 * obj->value[2] / obj->value[3], 6 ) : 6 ] );
  if( obj->pIndexData->vnum == OBJ_FIREFLY )
    sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : ", ", firefly_word[ range( 0, obj->value[2] /5, 10 ) ] );

//    color_word[ range( 0, ( obj->value[2]-10 )/10, 10 ) ] );  // old
//    color_word[ range( 0, obj->value[2]/3, 10 ) ] ); // stock

  if( is_set( obj->pIndexData->extra_flags, OFLAG_RANDOM_METAL )
    && metal <= MAT_LIRIDIUM ) 
    sprintf( tmp+strlen( tmp ), "%s%s", tmp[0] == '\0' ? "" : ", ",
      material_name[metal] ); 

  return;
}


void append_liquid( char* tmp, obj_data* obj )
{
  if( !include_liquid )
    return;

  if( obj->pIndexData->item_type != ITEM_DRINK_CON
    || ( obj->value[1] != -1 && obj->value[1] <= 0 )
    || obj->value[2] < 0 || obj->value[2] >= MAX_ENTRY_LIQUID )
    return;

  if( is_set( obj->extra_flags, OFLAG_KNOWN_LIQUID ) )
    sprintf( tmp+strlen( tmp ), " of %s",
      liquid_table[obj->value[2]].name );
  else {
    sprintf( tmp+strlen( tmp ), " containing %s",
      liquid_table[obj->value[2]].color );
    }

  return;
}


/*
 *   LOOK_SAME?
 */


bool char_data :: look_same( obj_data* obj1, obj_data* obj2 )
{
  bool id1;
  bool id2;

  if( !obj1 || !obj1->Is_Valid() || !obj2 || !obj2->Is_Valid() )
    return FALSE;

  if( obj1->pIndexData->fakes != obj2->pIndexData->fakes
    || ( obj1->contents == 0 ) != ( obj2->contents == 0 ) 
    || obj1->rust != obj2->rust )
    return FALSE;

  id1 = is_set( obj1->extra_flags, OFLAG_IDENTIFIED ); 
  id2 = is_set( obj2->extra_flags, OFLAG_IDENTIFIED );

  if( id2 ) {
    m2swap( obj1, obj2 );
    m2swap( id1, id2 );
  }

  if( id1 ) {
    if( !id2 ) {
      if( *obj1->singular == '{' || strcasecmp( obj1->after, obj2->before ) )
        return FALSE;
      if( ( obj1->pIndexData->item_type == ITEM_WEAPON
        || obj1->pIndexData->item_type == ITEM_SHIELD
        || obj1->pIndexData->item_type == ITEM_ARMOR )
        && obj1->value[0] != 0 )
        return FALSE;
    }   
    if( obj1->pIndexData != obj2->pIndexData )
      return FALSE;
    if( ( obj1->pIndexData->item_type == ITEM_WEAPON
      || obj1->pIndexData->item_type == ITEM_SHIELD
      || obj1->pIndexData->item_type == ITEM_ARMOR )
      && obj1->value[0] != obj2->value[0] )
      return FALSE;
  }

  if( obj1->pIndexData->vnum == OBJ_BALL_OF_LIGHT && range( 0, obj1->value[2] / 5, 10 ) != range( 0, obj2->value[2] / 5, 10 ) )
    return FALSE;
  if( obj1->pIndexData->vnum == OBJ_CORPSE_LIGHT && ( obj1->value[3] > 0 ? range( 0, 6 * obj1->value[2] / obj1->value[3], 6 ) : 6 ) != ( obj2->value[3] > 0 ? range( 0, 6 * obj2->value[2] / obj2->value[3], 6 ) : 6 ) )
    return FALSE;

  if( obj1->pIndexData->item_type == ITEM_CORPSE
    && strcasecmp( obj1->singular, obj2->singular ) ) 
    return FALSE;

  if( is_set( obj1->extra_flags, OFLAG_IS_INVIS )
    != is_set( obj2->extra_flags, OFLAG_IS_INVIS ) )
    return FALSE;

  if( is_set( obj1->extra_flags, OFLAG_BURNING )
    != is_set( obj2->extra_flags, OFLAG_BURNING ) )
    return FALSE;

  if( IS_AFFECTED( this, AFF_DETECT_MAGIC )
    && is_set( obj1->extra_flags, OFLAG_MAGIC )  
    != is_set( obj2->extra_flags, OFLAG_MAGIC ) )
    return FALSE;

  if( is_set( obj1->extra_flags, OFLAG_FLAMING )  
    != is_set( obj2->extra_flags, OFLAG_FLAMING ) )
    return FALSE;

  if( is_set( obj1->extra_flags, OFLAG_POISON_COATED )  
    != is_set( obj2->extra_flags, OFLAG_POISON_COATED ) )
    return FALSE;

  if( is_set( obj1->extra_flags, OFLAG_DISEASED )  
    != is_set( obj2->extra_flags, OFLAG_DISEASED ) )
    return FALSE;

  if( is_set( obj1->pIndexData->extra_flags, OFLAG_RANDOM_METAL )
    && obj1->materials != obj2->materials )
    return FALSE;

  if( obj1->pIndexData->item_type == ITEM_DRINK_CON ) {
    if( obj1->value[1] == 0 ) {
      if( obj2->value[1] != 0 )
        return FALSE;
    } else {
      if( obj2->value[1] == 0 || obj2->value[2] != obj1->value[2] ) 
        return FALSE;
    }
  } else if( obj1->pIndexData->item_type == ITEM_FOOD ) {
    if( obj1->value[1] != obj2->value[1] )
      return FALSE;
  } else if( obj1->pIndexData->item_type == ITEM_CONTAINER ) {
    if( !is_set( &obj1->value[1], CONT_CLOSED ) != !is_set( &obj2->value[1], CONT_CLOSED ) )
      return FALSE;
  }

  if( strcmp( obj1->label, obj2->label ) )
    return FALSE;

  return TRUE;
}


/*
 *   OBJECT CLASS NAME ROUTINES
 */


const char* obj_clss_data :: Keywords( )
{
  return Name( );
} 


const char* obj_clss_data :: Name( )
{
  char*             tmp;
  const char*  singular;

  tmp = static_string( );
  strcpy( tmp, is_set( extra_flags, OFLAG_THE ) ? "the " : ( is_set( extra_flags, OFLAG_NO_PREFIX ) ? "" : "an " ) );
  if( prefix_singular != empty_string )
    sprintf( tmp+strlen( tmp ), "%s ", prefix_singular );

  singular = seperate( this->singular, TRUE );

  switch( *after ) {
  case '\0' :
    strcat( tmp, singular );
    break;

  case '+' :
    sprintf( tmp+strlen( tmp ), "%s %s", singular, &after[1] );
    break;

  default :
    sprintf( tmp+strlen( tmp ), "%s%s%s", after, *after == '\0' ? "" : " ", singular );
    break;
  }

  // retarded 'a/an' code!
  if( *tmp == 'a' && !isvowel( tmp[3] ) ) {
    tmp[1] = 'a';
    return &tmp[1];
  }
  
  return tmp;
} 


const char* obj_clss_data :: Name( int number )
{
  char*           tmp;
  const char*  plural;
  
  if( number == 1 )
    return Name( );

  tmp = static_string( );

  strcpy( tmp, number_word( number ) );

  if( prefix_plural != empty_string )
    sprintf( tmp+strlen( tmp ), " %s ", prefix_plural );

  plural = seperate( this->plural, TRUE );

  switch( *after ) {
  case '\0' :
    sprintf( tmp+strlen( tmp ), " %s", plural );
    break;

  case '+' :
    sprintf( tmp+strlen( tmp ), " %s %s", plural, &after[1] );
    break;

  default :
    sprintf( tmp+strlen( tmp ), " %s %s", after, plural );
    break;
  }

  return tmp;
} 


const char* name_before( obj_clss_data* obj )
{ 
  char*             tmp;
  const char*  singular;

  if( obj == NULL ) 
    return "## Null Pointer?? ##";

  tmp = static_string( );

  strcpy( tmp, is_set( obj->extra_flags, OFLAG_THE ) ? "the " : ( is_set( obj->extra_flags, OFLAG_NO_PREFIX ) ? "" : "an " ) );
  if( obj->prefix_singular != empty_string )
    sprintf( tmp+strlen( tmp ), "%s ", obj->prefix_singular );

  singular = seperate( obj->singular, FALSE );

  switch( *obj->before ) {
  case '\0' :
    strcat( tmp, singular );
    break;

  case '+' :
    sprintf( tmp+strlen( tmp ), "%s %s", singular, &obj->before[1] );
    break;

  default :
    sprintf( tmp+strlen( tmp ), "%s%s%s", obj->before, *obj->before == '\0' ? "" : " ", singular );
    break;
  }

  // retarded 'a/an' code!
  if( *tmp == 'a' && !isvowel( tmp[3] ) ) {
    tmp[1] = 'a';
    return &tmp[1];
  }

  return tmp;
} 


const char* name_brief( obj_clss_data* obj, int number )
{
  char*             tmp;
  const char*  singular;

  if( obj == NULL ) 
    return "## Null Pointer?? ##";

  if( number != 1 )
    return obj->Name( number );

  store_pntr = ( store_pntr+1 )%5;
  tmp        = &static_storage[store_pntr*THREE_LINES];

  singular = seperate( obj->singular, TRUE );

  switch( *obj->after ) {
  case '\0' :
    strcpy( tmp, singular );
    break;

  case '+' :
    sprintf( tmp, "%s %s", singular, &obj->after[1] );
    break;

  default :
    sprintf( tmp, "%s%s%s", obj->after, *obj->after == '\0' ? "" : " ", singular );
    break;
  }

  return tmp;
} 


/*
 *   OBJECT NAME ROUTINES
 */


const char* obj_data :: Name( char_data* ch, int number, bool brief )
{
  if( ch != NULL && is_set( extra_flags, OFLAG_IS_INVIS ) && !ch->Sees_Invis( ) )
    return( number == 1 ? "something invisible" : "several invisible items" );

  return Seen_Name( ch, number, brief );
}


const char* obj_data :: Seen_Name( char_data* ch, int num, bool brief )
{
  char               adj  [ TWO_LINES ];
  char              plus  [ 6 ]; 
  char*           string;
  char*              tmp;
  const char*       noun;
  bool        identified  = is_set( extra_flags, OFLAG_IDENTIFIED );
  int             cc_len = 0;

  tmp  = static_string( );
  *tmp = '\0';

  get_obj_adj( adj, ch, this );

  if( num == 1 || brief ) {
    noun = seperate( singular, identified );
  } else {
    strcpy( tmp, number_word( num, ch ) );
    noun = seperate( plural, identified );
  }

  *plus = '\0';

  if( identified ) {
    string = after;
    if( ( pIndexData->item_type == ITEM_WEAPON
      || pIndexData->item_type == ITEM_SHIELD
      || pIndexData->item_type == ITEM_ARMOR ) && value[0] != 0 )
      sprintf( plus, " %+d", value[0] );
  } else 
    string = before;
  if( in_character ) {
    cc_len = strlen( color_code( ch, pIndexData->color ) );
    if( num == 1 || brief ) {
      if( !brief ) {
        strcpy( tmp, is_set( pIndexData->extra_flags, OFLAG_THE ) ? "the " : ( is_set( pIndexData->extra_flags, OFLAG_NO_PREFIX ) ? "" : "an " ) );
        if( pIndexData->prefix_singular != empty_string ) {
          cc_len = 0;
          sprintf( tmp+strlen( tmp ), "%s ", pIndexData->prefix_singular );
        }
      }
      if( *string != '+' )
        sprintf( tmp+strlen( tmp ), "%s%s%s%s%s%s%s%s", adj, adj[0] == '\0' ? "" : " ", string, ( *noun == '\0' || string[0] == '\0' ) ? "" : " ", color_code( ch, pIndexData->color ), noun, normal( ch ), plus );
      else
        sprintf( tmp+strlen( tmp ), "%s%s%s%s%s %s%s", adj,( *noun == '\0' || *adj == '\0' ) ? "" : " ", color_code( ch, pIndexData->color ), noun, normal( ch ), &string[1], plus );
      if( num != 1 ) 
        sprintf( tmp+strlen( tmp ), " (x%d)", num );
    }
    else {
      if( pIndexData->prefix_plural != empty_string )
        sprintf( tmp+strlen( tmp ), " %s ", pIndexData->prefix_plural );
      else 
        strcat( tmp, " " );

      if( *string != '+' )
        sprintf( tmp+strlen( tmp ), "%s%s%s%s%s%s%s%s", adj, adj[0] == '\0' ? "" : " ", string, ( noun[0] == '\0' || string[0] == '\0' ) ? "" : " ", color_code( ch, pIndexData->color ), noun, normal( ch ), plus );
      else
        sprintf( tmp+strlen( tmp ), "%s%s%s%s%s %s%s", adj, ( *noun == '\0' || adj[0] == '\0' ) ? "" : " ", color_code( ch, pIndexData->color ), noun, normal( ch ), &string[1], plus );
    }
  }
  else {
    if( num == 1 || brief ) {
      if( !brief ) {
        strcpy( tmp, is_set( pIndexData->extra_flags, OFLAG_THE ) ? "the " : ( is_set( pIndexData->extra_flags, OFLAG_NO_PREFIX ) ? "" : "an " ) );
        if( pIndexData->prefix_singular != empty_string )
          sprintf( tmp+strlen( tmp ), "%s ", pIndexData->prefix_singular );
      }
      if( *string != '+' )
        sprintf( tmp+strlen( tmp ), "%s%s%s%s%s%s", adj, adj[0] == '\0' ? "" : " ", string, ( *noun == '\0' || string[0] == '\0' ) ? "" : " ", noun, plus );
      else
        sprintf( tmp+strlen( tmp ), "%s%s%s %s%s", adj,( *noun == '\0' || *adj == '\0' ) ? "" : " ", noun, &string[1], plus );
      if( num != 1 ) 
        sprintf( tmp+strlen( tmp ), " (x%d)", num );
    }
    else {
      if( pIndexData->prefix_plural != empty_string )
        sprintf( tmp+strlen( tmp ), " %s ", pIndexData->prefix_plural );
      else 
        strcat( tmp, " " );

      if( *string != '+' )
        sprintf( tmp+strlen( tmp ), "%s%s%s%s%s%s", adj, adj[0] == '\0' ? "" : " ", string, ( noun[0] == '\0' || string[0] == '\0' ) ? "" : " ", noun, plus );
      else
        sprintf( tmp+strlen( tmp ), "%s%s%s %s%s", adj, ( *noun == '\0' || adj[0] == '\0' ) ? "" : " ", noun, &string[1], plus );
    }
  }

  append_liquid( tmp, this );

  if( label != empty_string ) 
    sprintf( tmp+strlen( tmp ), " labeled %s", label );

  if( adj[0] != '\0' || ( pIndexData->prefix_singular != empty_string && num == 1 ) ||
    ( pIndexData->prefix_plural != empty_string && num > 1 ) ||
    string[0] != '\0')
    cc_len = 0;

  if( num == 1 && !brief && *tmp == 'a' && !isvowel( tmp[3+cc_len] ) && ( tmp[3+cc_len] != '+' || !isvowel( tmp[4+cc_len] ) ) ) {
    tmp[1] = 'a';
    return &tmp[1];
  }

  return tmp;
}


const char* obj_data :: Keywords( char_data* ch )
{
  char* tmp = static_string( );
  char  adj [ TWO_LINES ];
  char  plus [ 6 ];

  get_obj_adj( adj, ch, this );
  *plus = '\0';

  if( !is_set( extra_flags, OFLAG_IDENTIFIED ) )
    sprintf( tmp, "%s %s %s %s %s %s", name_before( pIndexData ), condition_name( ch ), item_type_name[ pIndexData->item_type ], label == empty_string ? "" : label, adj, pIndexData->item_type != ITEM_FOOD ? "" 
    : !is_set( &materials, MAT_FLESH ) ? "vegetable" : "meat" );
  else {
    if( ( pIndexData->item_type == ITEM_WEAPON
      || pIndexData->item_type == ITEM_SHIELD
      || pIndexData->item_type == ITEM_ARMOR ) && value[0] != 0 )
      sprintf( plus, " %s%d", value[0] > 0 ? "+" : "", value[0] );
    sprintf( tmp, "%s %s %s %s %s %s %s", pIndexData->Name( ), condition_name( ch ), item_type_name[ pIndexData->item_type ], label == empty_string ? "" : label, adj, plus, pIndexData->item_type != ITEM_FOOD ? "" 
    : !is_set( &materials, MAT_FLESH ) ? "vegetable" : "meat" );
  }
  if( pIndexData->item_type == ITEM_CORPSE || pIndexData->item_type == ITEM_SEARCHABLE_CORPSE )
    sprintf( tmp, "%s %s", singular, adj );

  append_liquid( tmp, this );

  if( !strncmp( tmp, "a ", 2 ) )
    strcpy( tmp, &tmp[2] );
  else if( !strncmp( tmp, "an ", 3 ) )
    strcpy( tmp, &tmp[3] );
  if( char *pos = strstr( tmp, " labeled " ) )
    strcpy( pos, pos + strlen( " labeled" ) );

  return tmp;
}


const char* obj_data :: Show( char_data* ch, int num )
{ 
  char* tmp = static_string( );

  sprintf( tmp, "%s %s", Seen_Name( ch, num ),
    num == 1 ? ( *pIndexData->long_s == '\0' ?
    "lies here." : pIndexData->long_s )  
    : ( *pIndexData->long_p == '\0' ?
    "lie here." : pIndexData->long_p ) );

  return tmp;
}


/*
 *   LABEL ROUTINE
 */


bool valid_label( char_data* ch, const char* label )
{
  int i;

  for( i = 0; label[i] != '\0'; i++ )
    if( !isalpha( label[i] ) && label[i] != ' ' ) {
      send( ch, "Labels may only contain letters and spaces.\r\n" );
      return FALSE;
    }

  if( i >= 15 ) {
    send( ch, "Labels must be less than 15 characters.\r\n" );
    return FALSE;
  } 

  return TRUE;
}


void do_label( char_data* ch, char* argument )
{
  char        arg  [ MAX_INPUT_LENGTH ];
  obj_data*   obj;
  char*     label;
  bool     remove;

  remove = !contains_word( argument, "as", arg );

  if( *argument == '\0' || ( !remove && *arg == '\0' ) ) {
    send( ch, "Label what item and as what?\r\n" );
    return;
  }

  if( ( obj = one_object( ch, remove ? argument : arg, "label", &ch->contents ) ) == NULL ) 
    return;

  if( remove ) {
    if( obj->label == empty_string ) {
      send( ch, "%s isn't labeled.\r\n", obj );
      return;
    }

    obj = object( obj->From( 1 ) );
    if( !obj || !obj->Is_Valid() ) {
      roach( "do_label: label remove succeeded but object not created for %s", ch );
      return;
    }

    free_string( obj->label, MEM_OBJECT );
    obj->label = empty_string;
    send( ch, "You remove the label from %s.\r\n", obj );

    if( obj->array == NULL ) {
      obj->To( ch );
      consolidate( obj );
    }

    return;
  }

  if( obj->pIndexData->item_type != ITEM_CONTAINER ) {
    send( ch, "You may only label containers.\r\n" );
    return;
  }

  if( ( label = obj->label ) != empty_string ) {
    obj->label = empty_string;
    send( ch, "%s is already labeled as '%s'.\r\n", obj, label );
    obj->label = label;
    return;
  }

  if( !valid_label( ch, argument ) )
    return;

  obj = object( obj->From( 1 ) );
  if( !obj || !obj->Is_Valid() ) {
    roach( "do_label: label succeeded but object not created for %s", ch );
    return;
  }

  if( obj->label == empty_string ) {
    send( ch, "You label %s '%s'.\r\n", obj, argument );
  } else {
    free_string( obj->label, MEM_OBJECT );
    obj->label = empty_string;
    fsend( ch, "You remove the old label from %s and replace it with '%s'.\r\n", obj, argument );
  }

  obj->label = alloc_string( argument, MEM_OBJECT );

  if( obj->array == NULL ) {
    obj->To( ch );
    consolidate( obj );
  }
}

