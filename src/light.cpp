#include "system.h"


/*
 *   SUNLIGHT
 */


const char* light_name( int i )
{
  const char* adj [] = { "Pitch Dark", "Extremely Dark", "Dark",
    "Dimly Lit", "Lit", "Well Lit", "Brightly Lit", "Blinding" };

  i = (int) log( (float) 2+i );
  i = min( i, 7 );

  return adj[i];
}


int sunlight( int time )
{
  int sunrise = 5*60;
  int sunset  = 20*60;

  if( time < sunrise || time > sunset ) 
    return 10;

  double x = 10+500*sin(M_PI*(time-sunrise)/(sunset-sunrise)); 

  return (int) x;
}


/*
 *   ILLUMINATION
 */


int Thing_Data  :: Light( int )  { return 0; }
int Player_Data :: Light( int )  { return wearing.light; }
int Mob_Data    :: Light( int )  { return wearing.light+species->light; }


int Obj_Data :: Light( int n )
{
  int i  = pIndexData->light;

  i *= ( n == -1 ? number : n );

  return i; 
}   


int Room_Data :: Light( int ) 
{
  int i = contents.light;

  if( is_outside( ) ) 
    i += weather.sunlight;

  return i;
} 


/*
 *   SPELLS
 */


bool spell_create_light( char_data* ch, char_data*, void*, int level, int )
{
  if( null_caster( ch, SPELL_CREATE_LIGHT ) )
    return TRUE;

  obj_data *light = create( get_obj_index( OBJ_BALL_OF_LIGHT ) );

  if( light == NULL ) {
    bug( "Ball of Light Object is NULL, should be vnum %i", OBJ_BALL_OF_LIGHT );
    return TRUE;
  }

  if( ch->get_skill( SPELL_CREATE_LIGHT ) != UNLEARNT )
    light->value[ 2 ] = ch->get_skill( SPELL_CREATE_LIGHT ) * 5 - 1;
  else 
    light->value[ 2 ] = level;

  if( number_range( 1, 20 ) == 5 )
    light->value[ 2 ] += 5;

  send( ch, "%s appears in your hands.\r\n", light );
  send( *ch->array, "%s appears in %s's hands.\r\n", light, ch );
  
  light->owner = ch->pcdata->pfile;
  light->To( ch );
  consolidate( light );

  return TRUE;    
}


bool spell_continual_light( char_data *ch, char_data *victim, void*, int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_CONTINUAL_LIGHT, AFF_CONTINUAL_LIGHT );

  return TRUE;
}

bool spell_firefly( char_data* ch, char_data*, void*, int level, int )
{
  if( null_caster( ch, SPELL_FIREFLY ) )
    return TRUE;

  obj_data *light = create( get_obj_index( OBJ_FIREFLY ) );

  if( light == NULL ) {
    bug( "Firefly object is NULL, vnum should be %i", OBJ_FIREFLY );
    return TRUE;
  }

  int slot;

  for( slot = 0; slot <= MAX_LAYER; slot++ ) { 
    if( ch->Wearing( WEAR_FLOATING, slot ) == NULL && slot != MAX_LAYER )
      break;

    if( slot == MAX_LAYER ) { 
      send( ch, "You are already controlling as many fireflies as you can.\r\n" );
      light->Extract( );
      return TRUE;
    }
  }

  light->value[ 2 ] = ch->get_skill( SPELL_FIREFLY ) * 5 - 1;

  if( number_range( 1, 20 ) == 5 )
    light->value[ 2 ] += 5;

  send( ch, "%s appears near you lighting the way.\r\n", light );
  send( *ch->array, "%s appears near you lighting the way.\r\n", light, ch );
  
  light->owner    = ch->pcdata->pfile;
  light->position = WEAR_FLOATING;
  light->layer    = slot;
  light->To( &ch->wearing );

  return TRUE;    
}

