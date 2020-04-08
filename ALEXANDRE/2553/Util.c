/**
* int to string
*/

void dec_to_str (char* str, int val, int digits)
{
  int i=1u;

  for(; i<=digits; i++)
  {
    str[digits-i] = (char)((val % 10u) + '0');
    val/=10u;
  }

  str[i-1u] = '\0';
}
