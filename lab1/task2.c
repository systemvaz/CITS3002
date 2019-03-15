#include <stdio.h>
#include <stdlib.h>
#include "checksum.h"
#include "checksum_ccitt.c"

int main (int argc, char* argv[])
{

  char mystring[] = "Hello World";

  FILE* fp = fopen("test.in", "r");
  char buffer[100];

  unsigned short checkval = checksum_ccitt(mystring, sizeof(mystring));
  printf("%i\n", checkval);
  printf("-----------------\n");

  while (fgets(buffer, sizeof buffer, fp) != NULL)
  {
    unsigned short checkvalfile = checksum_ccitt(buffer, sizeof(buffer));
    printf("%i\n", checkvalfile);
  }
  fclose(fp);


  return 0;
}
