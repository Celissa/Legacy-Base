// bwahahahaaaaaaaaaaaaaaa

#include "system.h"
#include "dungeon.h"


int VnumBlockOffset = 0;

extern exit_data* add_exit( room_data* room, int dir );


TDungeon :: TDungeon( room_data* start_room, int max_size ) : Area_Data( )
{
  if( max_size >= 250 )
    MaxSize = max_size;
  else
    MaxSize = 250;

  NextVnum = 0;

  name = alloc_string( "A Dungeon", MEM_AREA );
  file = NULL;
  creator = NULL;
  help = NULL;
  status = AREA_OPEN;
  continent = start_room->area->continent;

  room_data* room = CreateRoom( start_room, DIR_DOWN );
  room->x = 0;
  room->y = 0;
  room->z = 0;

  set_bit( &room->room_flags, RFLAG_INDOORS );
  set_bit( &room->room_flags, RFLAG_UNDERGROUND );

  room_first = room;
  area_list.add( this );

  for( int i = number_range( 1, 4 ); i > 0; i-- )
    CreateBranch( room, -1 );
}


TDungeon :: ~TDungeon( )
{
  area_list.remove( this );
}


int TDungeon :: GetNextVnum( )
{
  int vnum;

  if( room_first )
    vnum = room_first->vnum + NextVnum;
  else {
    vnum = ROOM_GENERATED + VnumBlockOffset + NextVnum;
    VnumBlockOffset += MaxSize;
  }

  NextVnum++;

  return vnum;
}


void create_dungeon_rdesc( room_data* room )
{
  if( !room )
    return;

  if( room->description )
    free_string( room->description, MEM_ROOM );

  room->description = alloc_string( "You are in a room of the dungeon...", MEM_ROOM );
}


void add_dungeon_rdesc( room_data* room, const char* text )
{
  if( !room )
    return;
  
  selt_string s;
  s = room->description;
  s += text;
  
  free_string( room->description, MEM_ROOM );
  room->description = alloc_string( s, MEM_ROOM );
}


room_data* TDungeon :: RoomAt( int x, int y, int z )
{
  for( room_data* troom = room_first; troom != NULL; troom = troom->next )
    if( troom->x == x && troom->y == y && troom->z == z )
      return troom;

  return NULL;
}


room_data* TDungeon :: RoomAt( int x, int y, int z, int direction )
{
  switch( direction )
  {
  case DIR_NORTH:
    return RoomAt( x, y + 1, z );

  case DIR_EAST:
    return RoomAt( x + 1, y, z );

  case DIR_SOUTH:
    return RoomAt( x, y - 1, z );

  case DIR_WEST:
    return RoomAt( x - 1, y, z );

  case DIR_UP:
    return RoomAt( x, y, z + 1 );

  case DIR_DOWN:
    return RoomAt( x, y, z - 1 );

  default:
    return NULL;
  }
}


room_data* TDungeon :: RoomAt( room_data* room, int direction )
{
  switch( direction )
  {
  case DIR_NORTH:
    return RoomAt( room->x, room->y + 1, room->z );

  case DIR_EAST:
    return RoomAt( room->x + 1, room->y, room->z );

  case DIR_SOUTH:
    return RoomAt( room->x, room->y - 1, room->z );

  case DIR_WEST:
    return RoomAt( room->x - 1, room->y, room->z );

  case DIR_UP:
    return RoomAt( room->x, room->y, room->z + 1 );

  case DIR_DOWN:
    return RoomAt( room->x, room->y, room->z - 1 );

  default:
    return NULL;
  }
}


bool TDungeon :: JoinRooms( room_data* room1, room_data* room2, int direction )
{
  if( exit_direction( room1, direction ) ) // || !RoomAt( room1->x, room1->y, room1->z, direction ) )
    return false;

  exit_data* exit = add_exit( room1, direction );
  exit->to_room   = room2;
  exit            = add_exit( room2, dir_table[ direction ].reverse );
  exit->to_room   = room1;

  return true;
}


int TDungeon :: RandomAvailableExit( room_data* start_room )
{
  int directions[ 5 ] = { -1, -1, -1, -1, -1 };
  int count = 0;

  for( int i = 0; i <= 3; i++ ) {
    if( exit_direction( start_room, i ) == NULL ) {
      if( RoomAt( start_room, i ) == NULL )
        directions[ count++ ] = i;
    }
  }

  if( count == 0 )
    return -1;

  return directions[ number_range( 0, count - 1 ) ];
}


room_data* TDungeon :: CreateRoom( room_data* start_room, int direction )
{
  if( !start_room )
    return NULL;

  room_data* room;

  if( direction == -1 ) {
    // determine random available exit
    direction = RandomAvailableExit( start_room );
    if( direction == -1 )
      return NULL;
  }

  // join to an already created room
  room = RoomAt( start_room, direction ) ;
  if( room ) {
    JoinRooms( start_room, room, direction );
    return room;
  }

  int vnum = GetNextVnum( );
  if( vnum == -1 ) {
    roach( "This dungeon has run out of vnums!" );
    return NULL;
  }

  room = new room_data;

  room->area        = this;
  room->vnum        = vnum;
  room->name        = alloc_string( "Deep Within The Dungeon", MEM_ROOM );
  room->description = NULL;
  room->comments    = empty_string;
  room->room_flags  = start_room->room_flags;
  room->sector_type = start_room->sector_type;

  room->x = start_room->x;
  room->y = start_room->y;
  room->z = start_room->z;

  switch( direction )
  {
  case DIR_NORTH:
    room->y++;
    break;

  case DIR_EAST:
    room->x++;
    break;

  case DIR_SOUTH:
    room->y--;
    break;

  case DIR_WEST:
    room->x--;
    break;

  case DIR_UP:
    room->z++;
    break;

  case DIR_DOWN:
    room->z--;
    break;
  }

  create_dungeon_rdesc( room );

  JoinRooms( start_room, room, direction );

  append( room_first, room );

  return room;
}


int RotateDirection( int direction )
{
  direction--;
  if( direction < 0 )
    direction = 3;

  return direction;
}


bool TDungeon :: CreateBranch( room_data* start_room, int direction )
{
  if( !start_room )
    return false;

  room_data* room = CreateRoom( start_room, direction );
  if( room == NULL )
    return false;

  switch( number_range( 1, 6 ) ) {
  case 0:
    // create a cavern
    {
      direction = RandomAvailableExit( start_room );

      int length = number_range( 4, 12 );
      int width = number_range( 2, 5 );
      room_data* length_room = start_room;
      for( int x = 0; x < length; x++ ) {
        room_data* width_room = length_room;
        for( int y = 0; y < width; y++ ) {
          if( width_room ) {
            width_room = CreateRoom( width_room, RotateDirection( direction ) );
            room_data* temp = RoomAt( width_room, dir_table[ direction ].reverse );
            if( temp )
              JoinRooms( width_room, temp, dir_table[ direction ].reverse );
          }
        }
        if( length_room )
          length_room = CreateRoom( length_room, direction );
      }
    }
    break;

  case 1:
    // go down a level
    CreateBranch( room, DIR_DOWN );
    break;

  default:
    // create a corridor which may have branches at the end
    for( int i = 3 + number_range( 1, 12 ); i > 0; i-- )
      room = CreateRoom( room, -1 );

    if( room && number_range( 0, 2 ) > 0 ) {
      CreateBranch( room, -1 );
      CreateBranch( room, -1 );
    }

    break;
  }

  return true;
}


void TDungeon :: Show( room_data* room, char_data* ch, bool brief, bool scan )
{
  if( ch == NULL || ch->pcdata == NULL || room == NULL )
    return;

  send( ch, "Using TDungeon::Show( )\r\n" );

}
