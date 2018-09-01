/*
 *   CHARACTER CAPABILITIES
 */

bool can_carry(char_data*, obj_data*, bool = TRUE);
bool can_carry(char_data*, thing_array*, bool = TRUE);


inline int char_data :: can_carry_n( )
{
  return MAX_WEAR+2*Dexterity( )+20;
}


inline int char_data :: get_burden( )
{
  return range( 0, (6*contents.weight+3*wearing.weight)/Empty_Capacity( ), 6 );
}


