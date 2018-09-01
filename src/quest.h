/*
 *    Quest Data (new stuff)
 *    Molested by Selt *halo*
 *    Moved the class definitions to quest.cpp temporarily to stop having to recompile all files
 */

extern const char* quest_flag_name[];
extern const char* quest_type_name[];

#define       QFLAG_SUBSTEP         0
#define       QFLAG_CONCURRENT      1
#define       QFLAG_RANDOM          2
#define       MAX_QUEST_FLAGS       3

#define       QTYPE_KILL            0
#define       QTYPE_COLLECT         1
#define       QTYPE_POP             2
#define       QTYPE_LOCATION        3
#define       QTYPE_SPEAK           4
#define       QTYPE_SACRIFICE       5
#define       QTYPE_EXAMINE         6
#define       QTYPE_QUEST           7
#define       MAX_QUEST_TYPES       8
