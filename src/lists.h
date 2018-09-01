// use binary search routines - add a key to llist_compare or something
// to make a searchable array of keys for near instant navigation

// check adding a value to an empty list with a navigator open


/* ************************************************************************
*   File: lists.h                       Selt's CircleMUD LinkedList Class *
*  Usage: Linked Lists (no more arrays!)                                  *
*   (c) 1998+ Julian Forsythe (julian@rainchild.com)                      *
************************************************************************ */

#ifndef __SELT__LISTS_H  // Only include it once.
#define __SELT__LISTS_H

// Forward Declerations:
template <class list_type> class llist;
template <class list_type> class llist_member;
template <class list_type> class llist_navigator;


// List Member
template <class list_type>
class llist_member {
public:
  // These should be private, but for speed's sake we'll leave them public too
  list_type *entry;
  llist_member<list_type> *next;
  llist_member<list_type> *prev;

  // This should be the start of the public section
  llist_member() {
    record_new( sizeof( llist_member ), MEM_ARRAY );
    entry = NULL;
    next = NULL;
    prev = NULL;
  }

  ~llist_member() {
    record_delete( sizeof( llist_member ), MEM_ARRAY );
  }
};


///////////////////////  LLIST COMPARING ROUTINES  ///////////////////////
// defined in lists.cpp add your own if you make more types to be sorted!

int llist_compare(const char *string1, const char *string2);
int llist_compare(class noteboard_data *board1, class noteboard_data *board2);
int llist_compare(class note_data *note1, class note_data *note2);
int llist_compare(class xp_data *xp1, class xp_data *xp2);
int llist_compare(area_data *area1, area_data *area2);
int llist_compare(money_data *moni1, money_data* moni2);
int llist_compare(link_data *link1, link_data* link2);
int llist_compare(variable_data *var1, variable_data* var2);

/*
int llist_compare(class room_data *room1, class room_data *room2);
int llist_compare(class si_item *info1, class si_item *info2);
int llist_compare(class descriptor_data *desc1, class descriptor_data *desc2);
int llist_compare(class obj_data *obj1, class obj_data *obj2);
int llist_compare(class ofile_data *of1, class ofile_data *of2);
int llist_compare(class char_data *char1, class char_data *char2);
int llist_compare(class edesc_data *edesc1, class edesc_data *edesc2);
int llist_compare(class help_data *help1, class help_data *help2);
int llist_compare(class social_data *soc1, class social_data *soc2);
int llist_compare(class command_data *cmd1, class command_data *cmd2);
int llist_compare(class race_data *race1, class race_data *race2);
int llist_compare(class terrain_data *terrain1, class terrain_data *terrain2);
int llist_compare(class terrain_msg_data *msg1, class terrain_msg_data *msg2);
int llist_compare(class tell_data *tell1, class tell_data *tell2);
int llist_compare(class auction_data *auct1, class auction_data *auct2);
int llist_compare(class profession_data *profession1, class profession_data *profession2);
int llist_compare(class clan_data *clan1, class clan_data *clan2); // Terek_Clan
int llist_compare(class alias_data *alias1, class alias_data *alias2);
int llist_compare(class thing_data *thing1, class thing_data *thing2);
int llist_compare(class exit_data *exit1, class exit_data *exit2);
int llist_compare(class tell_log *log1, class tell_log *log2);
*/

///////////////////////  LLIST NAVIGATOR  ///////////////////////

template <class list_type>
class llist_navigator {
  llist_member<list_type> *current;
  llist_member<list_type> *nextmem;
  llist_member<list_type> *prevmem;

  llist<list_type> *list;

public:
  //// Constructor
  llist_navigator(llist<list_type> *thelist) {
    record_new( sizeof( llist_navigator ), MEM_ARRAY );
    list = thelist;
    list->subscribe(this);
    current = NULL;
    nextmem = NULL;
    prevmem = NULL;
  };
 
  //// List Management
  void update_removed(llist_member<list_type> *affected) {
    if (affected == current) {
      current = NULL;
    } else if (affected == nextmem) {
      nextmem = nextmem->next;
    } else if (affected == prevmem) {
      prevmem = prevmem->prev;
    }
  };

  void update_insertbefore(llist_member<list_type> *point) {
    if (point == current) {
      prevmem = current->prev; // inserted before current member, update with new previous
    } else if (point == nextmem) {
      nextmem = current->next; // inserted before old next member, update with new next
    }
  };

  void update_inserttail(llist_member<list_type> *oldtail) {
    if (current == oldtail) {
      nextmem = oldtail->next; // now there's one new member after the old tail
    } else if (prevmem == oldtail) {
      current = oldtail->next; // current would previously have been NULL, since we've stepped to the last member
    }
  };

  void update_revoked(void) {
    list = NULL;
    current = NULL;
    nextmem = NULL;
    prevmem = NULL;
  };

  //// List Navigation
  list_type *first(void) {
    llist_member<list_type> *head = list->gethead();

    if (head) {
      current = head;
      nextmem = current->next;
      prevmem = NULL;
    } else {
      current = NULL;
      nextmem = NULL;
      prevmem = NULL;
    }

    if (current)
      return current->entry;
    else
      return NULL;
  };

  list_type *last(void) {
    llist_member<list_type> *tail = list->gettail();

    if (tail) {
      current = tail;
      prevmem = current->prev;
      nextmem = NULL;
    } else {
      current = NULL;
      nextmem = NULL;
      prevmem = NULL;
    }

    if (current)
      return current->entry;
    else
      return NULL;
  };

  list_type *next(void) {
    if (nextmem) {
      if (current)
        prevmem = current;  // So remove -> next -> prev works
      current = nextmem;
      nextmem = nextmem->next;
    } else {
      prevmem = current;
      current = NULL;
    }

    if (current)
      return current->entry;
    else
      return NULL;
  };

  list_type *prev(void) {
    if (prevmem) {
      if (current)
        nextmem = current;  // So remove -> prev -> next works
      current = prevmem;
      prevmem = prevmem->prev;
    } else {
      nextmem = current;
      current = NULL;
    }

    if (current)
      return current->entry;
    else
      return NULL;
  };

  //// Destructor
  ~llist_navigator() {
    record_delete( sizeof( llist_navigator ), MEM_ARRAY );
    if (list)
      list->unsubscribe(this);
  };
};


///////////////////////  MAIN LLIST CLASS  ///////////////////////

template <class list_type>
class llist {
  // Head/Tail Pointers
  llist_member<list_type> *head;
  llist_member<list_type> *tail;

  // cast the void to llist_navigator<list_type> .. didn't like nested templates
  llist_member<void> *navigators;

public:
  bool empty;
  int size;

  //// Constructor
  llist() {
    record_new( sizeof( llist ), MEM_ARRAY );
    head = NULL;
    tail = NULL;
    empty = TRUE;
    size = 0;
    navigators = NULL;
  };

  //// List Management
  bool add(list_type *item) {
    llist_member<list_type> *temp, *member;

    if (item == NULL)
      return FALSE;  // Screw that, we're not adding a null entry

    //-----------------------------------------------------------------------
    // Create the new container for this list item
    member = new llist_member<list_type>;
    if (!member)
      return FALSE; // Out of memory!!
    member->entry = item; // Assign the given object to the container

    //-----------------------------------------------------------------------
    // Add member to list
    if (!head) {
      // First item to be added to the list, no need to do fancy scan-thru things
      head = member;
      tail = member;
      empty = FALSE;
    } else {
      // Scan thru the list till we find insertion point
      // llist_compare returns:
      //     0 when item 1 = item 2
      //   < 0 when item 1 < item 2
      //   > 0 when item 1 > item 2
      // we want to add the entry in as soon as the item gets > the tested member in the list
      // so in practice newest orange we pick up will be the first in the orange section
      // change to >= 0 if you want it to be last in the orange section
      for (temp = head; temp && llist_compare(item, temp->entry) > 0; temp = temp->next);

      if (temp == head) {
        // insert at head
        member->next = temp;        // add link to next
        temp->prev = member;        // update link from next
        head = member;              // set as head

      } else if (temp == NULL) {
        // insert after tail
        member->prev = tail;        // add link to previous
        tail->next = member;        // update link from previous
        tail = member;              // set as tail

      } else {
        // insert before temp
        member->next = temp;        // add link to next
        member->prev = temp->prev;  // add link to previous
        temp->prev->next = member;  // update link from previous
        temp->prev = member;        // update link from next
      }

      // Notify Iterations of List Change
      for (llist_member<void> *sub = navigators; sub; sub = sub->next) {
        if (sub->entry) {
          if (temp) {
            // performed a standard insert
            ((llist_navigator<list_type> *) sub->entry)->update_insertbefore(temp);
          } else {
            // inserted at the tail
            ((llist_navigator<list_type> *) sub->entry)->update_inserttail(tail->prev);
          }
        }
      }
    }

    size++;

    //-----------------------------------------------------------------------
    // Done
    return TRUE;
  }

  bool remove(list_type *item) {
    llist_member<list_type> *temp;
    bool updated = FALSE;

    if (item == NULL)
      return FALSE;  // Screw that, we're not looking for a non-existant entry

    // Scan thru the list till we find it
    for (temp = head; temp && temp->entry != item; temp = temp->next);

    if (temp == NULL)
      return FALSE;  // Item not found

    // Notify Iterations of List Change
    for (llist_member<void> *sub = navigators; sub; sub = sub->next)
      if (sub->entry)
        ((llist_navigator<list_type> *) sub->entry)->update_removed(temp);

    if (temp == head) {
      head = temp->next;    // Item is head
      if (head)
        head->prev = NULL;  // therefore there's nothing before it
      updated = TRUE;
    }

    if (temp == tail) {
      if (updated)          // We've already removed it as the head, meaning it was the last item on the list
        empty = TRUE;       // So update the empty list value

      tail = temp->prev;    // Item is tail
      if (tail)
        tail->next = NULL;  // therefore there's nothing after it
      updated = TRUE;
    }

    if (!updated) {  // because we had to use 2 if blocks for head + tail (because an item can be both) we had to make a boolean to tell us if we'd alrady done the job or not
      temp->prev->next = temp->next;  // update next and previous pointers
      temp->next->prev = temp->prev;  // so we don't break the list
    }

    delete temp;     // Delete the list container (not the actual item)

    size--;

    return TRUE;
  };

  void destroy_list(void) {
    while (head)
      remove (head->entry);
  };

  bool subscribe(llist_navigator<list_type> *nav) {
    // add to the head of the list
    llist_member<void> *temp = new llist_member<void>;
    temp->entry = (void *) nav;
    temp->next = navigators;
    navigators = temp;
    return TRUE;
  };

  bool unsubscribe(llist_navigator<list_type> *nav) {
    llist_member<void> *temp, *prev;
    // scan to find the nav
    for (prev = NULL, temp = navigators; temp && temp->entry != (void *) nav; prev = temp, temp = temp->next);

    if (!temp) // wasn't subscribed
      return FALSE;
    else if (temp == navigators)
      navigators = temp->next;
    else
      prev->next = temp->next;

    delete temp;

    return TRUE;
  };

  void kill_subscriptions(llist_member<void> *sub) {
    if (sub->next)
      kill_subscriptions(sub->next);

    ((llist_navigator<list_type> *) sub->entry)->update_revoked();

    delete sub;
  };

  int merge(llist<list_type> *source) {
    list_type *temp;
    int i = 0;

    while (temp = source->headitem()) {
      source->remove(temp);  // remove from the source list
      this->add(temp);       // and add to this list
      i++;                   // count the number of items merged
    }

    return i;
  };

  //// List Navigation
  llist_member<list_type> *gethead(void) {
    return head;
  };

  llist_member<list_type> *gettail(void) {
    return tail;
  };

  list_type *headitem(void) {
    // now this is a lil bit of a shortcut, you should really
    // go thru the listnav routines and stuff to get to this
    // data, but for purposes of speed, sometimes you only want
    // to access the head item (especially for act to room)
    // so it's here, but really shouldn't be
    if (head)
      return head->entry;
    else
      return NULL;
  };

  list_type *tailitem(void) {
    if (tail)
      return tail->entry;
    else
      return NULL;
  };

  //// Destructor
  ~llist() {
    record_delete( sizeof( llist ), MEM_ARRAY );
    destroy_list();
    if (navigators)
      kill_subscriptions(navigators);
  };
};


typedef llist<char>                          str_list;
typedef llist<noteboard_data>                board_list;
typedef llist<note_data>                     note_list;

typedef llist_navigator<char>                str_nav;
typedef llist_navigator<noteboard_data>      board_nav;
typedef llist_navigator<note_data>           note_nav;
typedef llist_navigator<area_data>           area_nav;
typedef llist_navigator<money_data>          money_nav;
typedef llist_navigator<link_data>           link_nav;
typedef llist_navigator<variable_data>       variable_nav;

/*
// llist typedefs to save time
typedef llist_navigator<channel_data>        channel_nav;
typedef llist_navigator<tell_data>           tell_nav;
typedef llist_navigator<tell_log>            tell_log_nav;

typedef llist<obj_data>            obj_list;
typedef llist<ofile_data>          ofile_list;
typedef llist<room_data>           room_list;
typedef llist<thing_data>          thing_list;
typedef llist<char_data>           char_list;
typedef llist<player_data>         player_list;
typedef llist<mob_data>            mob_list;
typedef llist<descriptor_data>     desc_list;
typedef llist<help_data>           help_list;
typedef llist<edesc_data>          edesc_list;
typedef llist<race_data>           race_list;
typedef llist<terrain_data>        terrain_list;
typedef llist<tell_data>           tell_list;
typedef llist<profession_data>     profession_list;
typedef llist<clan_data>           clan_list; //Terek_Clan
typedef llist<alias_data>          alias_list;
typedef llist<tell_log>            tell_log_list;
typedef llist<exit_data>           exit_list;


// llist navigator typedefs to save time
typedef llist_navigator<obj_data>            obj_nav;
typedef llist_navigator<ofile_data>          ofile_nav;
typedef llist_navigator<room_data>           room_nav;
typedef llist_navigator<thing_data>          thing_nav;
typedef llist_navigator<char_data>           char_nav;
typedef llist_navigator<player_data>         player_nav;
typedef llist_navigator<mob_data>            mob_nav;
typedef llist_navigator<descriptor_data>     desc_nav;
typedef llist_navigator<help_data>           help_nav;
typedef llist_navigator<edesc_data>          edesc_nav;
typedef llist_navigator<race_data>           race_nav;
typedef llist_navigator<terrain_data>        terrain_nav;
typedef llist_navigator<tell_data>           tell_nav;
typedef llist_navigator<profession_data>     profession_nav;
typedef llist_navigator<clan_data>           clan_nav; //Terek_Clan
typedef llist_navigator<alias_data>          alias_nav;
typedef llist_navigator<channel_data>        channel_nav;
typedef llist_navigator<tell_log>            tell_log_nav;
typedef llist_navigator<exit_data>           exit_nav;
*/


// lookup table structures
class lookup_data
{
protected:
  VARIABLE_INT(type);
  VARIABLE_CHAR(text);
public:
  lookup_data(int ttype, char *ttext)
  {
    type = ttype;
    text = ttext;
  }
};

#define LOOKUP_UNDEFINED    -1
extern char *lookup(const int type, lookup_data *table);
extern int lookup(const char *type, lookup_data *table);
extern int lookup_isname(const char *type, lookup_data *table);

extern llist<area_data> area_list;

#endif
