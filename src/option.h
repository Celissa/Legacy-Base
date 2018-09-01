#define MSG_AUTOSAVE                0
#define MSG_BLEEDING                1  
#define MSG_DAMAGE_MOBS             2
#define MSG_DAMAGE_PLAYERS          3
#define MSG_GROUP_MOVE              4
#define MSG_HUNGER                  5 
#define MSG_MAX_HIT                 6 
#define MSG_MAX_ENERGY              7
#define MSG_MAX_MOVE                8
#define MSG_MISSES                  9
#define MSG_QUEUE                  10
#define MSG_SPELL_COUNTER          11
#define MSG_THIRST                 12
#define MSG_WEATHER                13
#define MSG_MULTIPLE_ITEMS         14
#define MSG_LONG_NAMES             15
#define MSG_DOOR_DIRECTION         16
#define MAX_MESSAGE                17


#define MSG_STANDARD               -1
#define MSG_PLYR_MOVE               0
#define MSG_SHOP                    1
#define MSG_BANK                    2
#define MSG_PREPARE                 3
#define MSG_EQUIP                   4
#define MSG_INVENTORY               5
#define MSG_CONSUME                 6
#define MAX_MESS_SETTING            7


#define IFLAG_AUCTION               0
#define IFLAG_CLANS                 1
#define IFLAG_DEATHS                2
#define IFLAG_LEVELS                3 
#define IFLAG_LOGINS                4
#define IFLAG_BUGS                  5
#define IFLAG_REQUESTS              6
#define IFLAG_WRITES                7
#define IFLAG_NOTES                 8
#define MAX_IFLAG                   9


#define PLR_AUTO_ASSIST             0
#define PLR_AUTO_EXIT               1
#define PLR_AUTO_BACKSTAB           2
#define PLR_AUTO_SKIN               3
#define PLR_AFFECT_DISPLAY          4
#define PLR_AUTO_SPLIT              5
#define PLR_BRIEF                   6
#define PLR_CHANT                   7
#define PLR_CHAT                    8
#define PLR_CTELL                   9
#define PLR_EMAIL_PUBLIC           10
#define PLR_GOSSIP                 11
#define PLR_GROUP_INCOG            12
#define PLR_NUMERIC                13
#define PLR_IDLE                   14
#define PLR_INFO_REPEAT            15
#define PLR_INFO                   16
#define PLR_LANG_ID                17
#define PLR_SPAMBACK               18
#define PLR_NO_SUMMON              19 
#define PLR_OOC                    20
#define PLR_PARRY                  21
#define PLR_PET_ASSIST             22
#define PLR_PROMPT                 23
#define PLR_REVERSE                24
#define PLR_GCHAT                  25
#define PLR_SAY_REPEAT             26
#define PLR_SAY_REP_COLOR          27
#define PLR_SEARCHING              28
#define PLR_SNEAK                  29 
#define PLR_STATUS_BAR             30
#define PLR_TRACK                  31
#define PLR_PORTAL                 32
#define PLR_ATALK                  33
#define PLR_PET_INVENTORY          34

#define MAX_PLR_OPTION             35

#define PLR_NO_TITLE               35
#define PLR_NO_EMOTE               36
#define PLR_NO_SHOUT               37
#define PLR_NO_TELL                38 
#define PLR_DENY                   39
#define PLR_FREEZE                 40
#define PLR_EDIT_HELP              41
#define PLR_FAMILIAR               42
#define PLR_LEADER                 43
#define PLR_ENROLLED               44
#define PLR_APPROVED               45 
#define PLR_DONATED                46
#define PLR_EMAIL_CONFIRM          47
#define PLR_CRASH_QUIT             48
#define PLR_AVATAR                 49
#define PLR_BUILDCHAN              50
#define PLR_WIZINVIS               51
#define PLR_HOLYLIGHT              52
#define PLR_LOGFILE                53
#define PLR_IMMCHAN                54
#define PLR_NO_COMMUNICATION       55
#define MAX_PLR                    56


#define SET_AUTOLOOT                0
#define SET_AUTOSCAN                1
#define SET_IGNORE                  2
#define SET_INCOGNITO               3
#define SET_ROOM_INFO               4
#define MAX_SETTING                 5

#define RECOG_INTRODUCE       16
#define RECOG_RECOGNIZE       17
#define RECOG_BEFRIEND        18
#define RECOG_FILTER          19
#define RECOG_CONSENT         20

extern info_data*  info_history  [ MAX_IFLAG+1 ];
extern int             msg_type;   

class Info_Data
{
public:
  info_data*     next;
  char*       string1;
  char*       string2;
  int           level;
  int            type;
  clan_data*     clan;
  pfile_data*   pfile;

  Info_Data( const char* s1, const char* s2, int l, int t, clan_data* c, pfile_data* p );
  ~Info_Data();
};

