/*
 *   SHOPS
 */


class Shop_Data
{
 public:
  shop_data*        next;        
  room_data*        room;
  custom_data*    custom; 
  int             keeper;
  int          open_hour;       
  int         close_hour;      
  int             repair; 
  int              flags;
  int           buy_type  [ 2 ];
  int          materials;
  int            PetShop;
  int           services;

  Shop_Data( ) {
    record_new( sizeof( shop_data ), MEM_SHOP );
    next   = NULL;
    custom = NULL;
    }

  ~Shop_Data( ) {
    record_delete( sizeof( shop_data ), MEM_SHOP );
    }
};


extern shop_data*  shop_list;


void          load_shops          ( void );
void          save_shops          ( void );
char_data*    active_shop         ( char_data* );
int           add_tax             ( room_data*, int, bool = false );
int           find_table_entry    ( const char*, int );
void          tax_money_delivery  ( int, int );
int           coin_value_in_condition( obj_data* );

#define SHOP_STOLEN     0
#define SHOP_NO_MELT    1
#define SHOP_VEGETERIAN 2
#define SHOP_BUTCHER    3
#define MAX_SHOP        4


/*
 *   CUSTOMS
 */


class Custom_Data
{
 public:
  custom_data*      next;
  int               cost;
  obj_clss_data*    item;
  obj_clss_data*  ingred  [ MAX_INGRED ];
  int             number  [ MAX_INGRED ];

  Custom_Data( ) {
    record_new( sizeof( custom_data ), MEM_CUSTOM );
    item  = NULL;
    next  = NULL;
    cost  = 100;
    vzero( ingred, MAX_INGRED );
    vzero( number, MAX_INGRED );
    }

  ~Custom_Data( ) {
    record_delete( sizeof( custom_data ), MEM_CUSTOM );
    }
};


