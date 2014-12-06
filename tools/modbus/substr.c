#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char* substring(const char* str, size_t begin, size_t len)
{
  if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len))
    return 0;

  return strndup(str + begin, len);
}


int main()
{
  size_t      begin  = 5;
  size_t      end    = 10;

  const char* str    = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char*       substr = substring(str, begin, end);

  printf("str    = %s\n", str);
  printf("substr starting from pos %u, and %u characters in length...\n", begin, end);
  printf("substr = %s which has a strlen of %u\n", substr, strlen(substr));

  free(substr);

  return 0;
} 
  
