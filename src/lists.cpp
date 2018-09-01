/* ************************************************************************
*   File: lists.cpp                     Selt's CircleMUD LinkedList Class *
*  Usage: Linked Lists (no more arrays!)                                  *
*   (c) 1998+ Julian Forsythe (julian@rainchild.com)                      *
************************************************************************ */

#include "system.h"



int lookup(const char *type, lookup_data *table)
{
  int i;

  for (i = 0; table[i].get_type() != LOOKUP_UNDEFINED; i++)
    if (!str_cmp(table[i].get_text(), type))
      break;

  return table[i].get_type();
}

int lookup_isname(const char *type, lookup_data *table)
{
  int i;

  for (i = 0; table[i].get_type() != LOOKUP_UNDEFINED; i++)
    if (matches(type, table[i].get_text()))
      break;

  return table[i].get_type();
}

char *lookup(const int type, lookup_data *table)
{
  int i;

  for (i = 0; table[i].get_type() != LOOKUP_UNDEFINED; i++)
    if (table[i].get_type() == type)
      break;

  return table[i].get_text();
}



///////////////////////  LLIST COMPARING ROUTINES  ///////////////////////
// return:
//    0 if item1 = item2
//  > 0 if item1 > item2
//  < 0 if item1 < item2

// for strings
int llist_compare(const char *string1, const char *string2) {
  // standard str_cmp
  return str_cmp(string1, string2);
}

// for noteboards
int llist_compare(noteboard_data *board1, noteboard_data *board2)
{
  // sort by name
  return str_cmp(board1->get_name(), board2->get_name());
}

// for notes
int llist_compare(note_data *note1, note_data *note2)
{
  // sort by date
  return (note1->get_modified() - note2->get_modified());
}

// for xp data
int llist_compare(xp_data *xp1, xp_data *xp2) {
  // sort by xp vnum
  return (xp1->vnum - xp2->vnum);
}

// for areas
int llist_compare(area_data *area1, area_data *area2) {
  // sort by vnum
  return (area1->room_first->vnum - area2->room_first->vnum);
}

// for moni
int llist_compare(money_data *moni1, money_data* moni2) {
  // sort by value of coin
  return 1-(moni1->value - moni2->value);
}

// for links
int llist_compare(link_data *link1, link_data* link2) {
  // sort by connection time
  return (link1->connecttime - link2->connecttime);
}

// for variables
int llist_compare(variable_data* var1, variable_data* var2)
{
  // sort by name
  return str_cmp(var1->get_name(), var2->get_name());
}

/*

// for rooms
int llist_compare(room_data *room1, room_data *room2) {
  // sort by vnum
  return (room1->get_number() - room2->get_number());
}

// for races
int llist_compare(race_data *race1, race_data *race2) {
  // sort by name
  return str_cmp(race1->get_name(), race2->get_name());
}

// for save_info
int llist_compare(si_item *info1, si_item *info2) {
  if (info1->get_type() != info2->get_type()) {
    // sort by type
    return (info1->get_type() - info2->get_type());
  } else {
    // secondary sort by vnum
    return (info1->get_vnum() - info2->get_vnum());
  }
}

// for terrains
int llist_compare(class terrain_data *terrain1, class terrain_data *terrain2) {
  // sort by terrain id
  return str_cmp(terrain1->get_id(), terrain2->get_id());
}

int llist_compare(class terrain_msg_data *msg1, class terrain_msg_data *msg2) {
  // don't sort
  return 0;
}

// for descriptors
int llist_compare(descriptor_data *desc1, descriptor_data *desc2) {
  // sort by descriptor number
  return (desc1->get_desc_num() - desc2->get_desc_num());
}

// for objects
int llist_compare(class obj_data *obj1, class obj_data *obj2) {
  // sort by vnum
  // perhaps add secondary sort-by-name later
  return (obj1->get_vnum() - obj2->get_vnum());
}

int llist_compare(class tell_data *tell1, class tell_data *tell2) {
  // sort by inverse time
  return (0 - (tell1->get_time() - tell2->get_time()));
}

// for characters
int llist_compare(char_data *char1, char_data *char2) {
  if (!IS_NPC(char1) && !IS_NPC(char2)) {
    // if both players use name ..
    return str_cmp(GET_NAME(char1), GET_NAME(char2));

  } else {
    // someone's a NPC, a players 'vnum' is set to -1, so will always come before a mob
    return ((mob_data *) char1)->get_number() - ((mob_data *) char2)->get_number();
  }
}

// for extra descriptions
int llist_compare(edesc_data *edesc1, edesc_data *edesc2) {
  // sort by keywords
  return str_cmp(edesc1->get_keyword(), edesc2->get_keyword());
}

// for helps
int llist_compare(help_data *help1, help_data *help2) {
  // sort by vnum
  // maybe secondary sort by keywords?
  return (help1->get_number() - help2->get_number());
}

// for ofiles
int llist_compare(ofile_data *of1, ofile_data *of2) {
  // sort by vnum
  return (of1->get_base() - of2->get_base());
  // perhaps secondary sort by name
}

// for socials
int llist_compare(social_data *soc1, social_data *soc2) {
  // sort by sort_as, secondary sort by command name
  int result = str_cmp(soc1->get_sort_as(), soc2->get_sort_as());
  if (result == 0)
    return str_cmp(soc1->get_command(), soc2->get_command());
  else 
    return result;
}

int llist_compare(command_data *cmd1, command_data *cmd2) {
  // command precedence: do_move, do_xxx, do_action
  int result = str_cmp(cmd1->get_sort_as(), cmd2->get_sort_as());

  if (result != 0)
    return result;  // sort by sort_as
  else {
//    if (cmd1->get_function() == do_move)  // move takes precedence
//      return -1;
//    else if (cmd2->get_function() == do_move)  // move takes precedence
//      return 1;
//    else
    if (cmd1->get_function() == do_action)  // action takes last
      return 1;
    else if (cmd2->get_function() == do_action)  // action takes last
      return -1;
    else
      return str_cmp(cmd1->get_command(), cmd2->get_command());  // sort by command name
  }
}

// for auctions
int llist_compare(auction_data *auct1, auction_data *auct2)
{
  // sort by auction id 
  return (auct1->get_auction_id() - auct2->get_auction_id());
}

// for classes
int llist_compare(profession_data *profession1, profession_data *profession2) {
  // sort by name
  return str_cmp(profession1->get_name(), profession2->get_name());
}
//Terek_clan
int llist_compare(clan_data *clan1, clan_data *clan2) {
  // sort by name
  return str_cmp(clan1->get_name(), clan2->get_name());
}

// for aliases
int llist_compare(class alias_data *alias1, class alias_data *alias2)
{
  // sort by alias keyword
  return str_cmp(alias1->get_keyword(), alias2->get_keyword());
}

// for tell logs
int llist_compare(class tell_log *log1, class tell_log *log2)
{
  // sort by alias keyword
  return str_cmp(log1->get_type(), log2->get_type());
}

int llist_compare(class thing_data *thing1, class thing_data *thing2)
{
  // sort by type
  return thing1->get_type() - thing2->get_type();
}

int llist_compare(class exit_data *exit1, class exit_data *exit2)
{
  // sort by inverse cardinal (so all teleport rooms come last)
  return -(exit1->get_cardinal() - exit2->get_cardinal());
}
*/
