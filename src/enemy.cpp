#include "system.h"


/*
 *   ENEMY_DATA CLASS
 */

// NOTE pfile can be set to NULL, and will be whenever a pet is the enemy
// petId is unique each reboot
class Enemy_Data
{
 public:
  enemy_data*   next;
  pfile_data*   pfile;
  int           damage;
  bool          attacker;
  int           petId;

  Enemy_Data( );
  ~Enemy_Data( );
}; 


Enemy_Data :: Enemy_Data( )
{
  record_new( sizeof( enemy_data ), MEM_ENEMY );

  damage = 0;
  petId = -1;
  return;
};


Enemy_Data :: ~Enemy_Data( )
{
  record_delete( sizeof( enemy_data ), MEM_ENEMY );

  return;
};


/*
 *   GENERIC ROUTINES
 */


bool is_enemy( char_data* ch, char_data* victim )
{
  enemy_data* enemy;

  for( enemy = ch->enemy; enemy != NULL; enemy = enemy->next )
    if( enemy->pfile == victim->pcdata->pfile )
      return TRUE;

  return FALSE;
}


int damage_done( char_data* ch, char_data* victim )
{
//  if (!ch->pcdata)
//    return -1;

  for( enemy_data *enemy = victim->enemy; enemy != NULL; enemy = enemy->next ) {
    if( ch->pcdata != NULL ) {
      if( enemy->pfile == ch->pcdata->pfile )//&& 
//    enemy->petId == ch->Get_Integer( "/Squeed/Code/PetId" ) )
        return enemy->damage;
    }
    else if( is_pet( ch ) ) {
      if( enemy->petId == ch->Get_Integer( "/Squeed/Code/PetId" ) )
        return enemy->damage;
    }
  }
  return -1;
}


/*  
 *   DELETION ROUTINES
 */


void clear_enemies( char_data* victim )
{
  mob_data*       ch;
  enemy_data*  enemy;

  if( victim->pcdata == NULL )
    return;

  for( int i = 0; i < mob_list; i++ ) {
    ch = mob_list[i];
    if (!ch->Is_Valid())
      continue;

    for( enemy = ch->enemy; enemy != NULL; enemy = enemy->next ) 
      if( enemy->pfile == victim->pcdata->pfile ) {
   //&& enemy->petId == victim->Get_Integer( "/Squeed/Code/PetId" ) ) {
        remove( ch->enemy, enemy );
        break;
      }
    }

  return;
}


void extract( enemy_data*& list )
{
  enemy_data* enemy;

  for( ; list != NULL; ) {
    enemy = list;
    list  = list->next;

    delete enemy;
    }

  return;
}


/*
 *   CREATION ROUTINES
 */


void add_enemy( char_data* ch, char_data* victim )
{
  enemy_data* enemy;

  if( victim->species == NULL || ( ch->pcdata == NULL && !is_pet( ch ) ) )
    return;

  for( enemy = victim->enemy; enemy != NULL; enemy = enemy->next ) {
    if( ch->pcdata != NULL ) {
      if( enemy->pfile == ch->pcdata->pfile )//&& 
          //enemy->petId == ch->Get_Integer( "/Squeed/Code/PetId" ) )
        return;
    }
    else if( is_pet( ch ) ) {
      if( enemy->petId == ch->Get_Integer( "/Squeed/Code/PetId" ) )
       return;
    }
  }

  enemy          = new enemy_data;
  if( ch->pcdata != NULL )
    enemy->pfile   = ch->pcdata->pfile;
  else {
    enemy->pfile = NULL;
    enemy->petId = ch->Get_Integer( "/Squeed/Code/PetId" );
  }
  enemy->next    = victim->enemy;
  victim->enemy  = enemy;

  return;
}  


void share_enemies( char_data* ch1, char_data* ch2 )
{
  enemy_data*  e1;
  enemy_data*  e2;

  if( !is_set( ch2->species->act_flags, ACT_ASSIST_GROUP ) )
    return; 

  for( e1 = ch1->enemy; e1 != NULL; e1 = e1->next ) {
    for( e2 = ch2->enemy; ; e2 = e2->next ) {
      if( e2 == NULL ) {
        e2          = new enemy_data;
        e2->pfile   = e1->pfile;
  e2->petId   = e1->petId;
        e2->next    = ch2->enemy;
        ch2->enemy  = e2;
        break;
        }
      if( e2->pfile == e1->pfile && e2->petId == e2->petId)
        break;
      } 
    }

  return;
}


void record_damage( char_data* victim, char_data* ch, int damage )
{
  enemy_data* enemy;

  if( victim->species == NULL || ch == NULL || 
    ( ch->pcdata == NULL && !is_pet( ch ) ) )
    return;

  for( enemy = victim->enemy; enemy != NULL; enemy = enemy->next ) {
    if( ch->pcdata != NULL ) {
      if( enemy->pfile == ch->pcdata->pfile ) {
        enemy->damage += damage;
        return;
      }
    }
    else {
      if( enemy->petId == ch->Get_Integer( "/Squeed/Code/PetId" ) ) {
        enemy->damage += damage;
        return; 
      }
    }
  }
  
  enemy          = new enemy_data;
  if( ch->pcdata != NULL )
    enemy->pfile   = ch->pcdata->pfile;
  else {
    enemy->pfile = NULL;
    enemy->petId = ch->Get_Integer( "/Squeed/Code/PetId" );
  }
  enemy->damage  = damage;
  enemy->next    = victim->enemy;
  victim->enemy  = enemy;

  return;
}  









