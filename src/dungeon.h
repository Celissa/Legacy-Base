// bwahahahaaaaaaaaaaaaaaa

#define room_data Room_Data


class TDungeon : public Area_Data
{
protected:
  int MaxSize, NextVnum;

public:
  TDungeon( room_data* start_room, int max_size );
  virtual ~TDungeon( );

  int GetNextVnum( );

  room_data* CreateRoom( room_data*, int );
  bool CreateBranch( room_data*, int );
  
  bool JoinRooms( room_data* room1, room_data* room2, int direction );
  room_data* RoomAt( int x, int y, int z );
  room_data* RoomAt( int x, int y, int z, int direction );
  room_data* RoomAt( room_data* room, int direction );
  int RandomAvailableExit( room_data* );

  void Show( room_data*, char_data*, bool, bool );
};

#undef room_data

extern void show_dungeon_rdesc( char_data* ch, room_data* room );

