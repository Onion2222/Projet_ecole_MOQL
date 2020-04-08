#include <Util.h>

/**
* int to string
*/

void dec_to_str (CHAR_8* str, INT_32 val, INT_32 digits)
{
  INT_32 i=1u;

  for(; i<=digits; i++)
  {
    str[digits-i] = (CHAR_8)((val % 10u) + '0');
    val/=10u;
  }

  str[i-1u] = '\0';
}
