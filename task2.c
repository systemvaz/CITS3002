#include <stdio.h>
#include <stdlib.h>
#include "checksum.h"

int main (int argc, char* argv[])
{

  char mystring[] = "Hello World";
  //unsigned char* p = mystring;

  //unsigned short checkval = checksum_ccitt(p, sizeof(mystring)/sizeof(char));
  //printf("%i\n", checkval);

  FILE* fp = fopen(argv[1], "r");
  char buffer[50];
  int tocheck[1000];
  int count = 0;

  while (fgets(buffer, sizeof buffer, fp) != NULL)
  {

    unsigned char* p = buffer;
    tocheck[count] = checksum_ccitt(p, sizeof(buffer)/sizeof(char));

    count++;
    //unsigned short checkval = checksum_ccitt(p, sizeof(buffer)/sizeof(char));
    //printf("%i\n", checkval);
  }

  fclose(fp);
  fopen("soln.out", "r");
  count = 0;

  while (fgets(buffer, sizeof buffer, fp) != NULL)
  {
    int check = atoi(buffer);
    printf("input:%i  output:%i\n", tocheck[count], check);
    if (check != tocheck[count])
    {
      printf("checksum fail\n");
    }
    count++;
  }

  fclose(fp);
  return 0;
}
