#include <stdio.h>
#include <stdlib.h>
#include "checksum.h"
#include "checksum_ccitt.c"
#include "checksum_crc16.c"
#include "checksum_internet.c"

int main (int argc, char* argv[])
{

  char mystring[] = "CITS3002: Networks";
  printf("size: %li\n", sizeof(mystring));

  // FILE* fp = fopen("test.in", "r");
  // char buffer[100];

  unsigned short ccitt = checksum_ccitt(mystring, sizeof(mystring)/sizeof(char));
  unsigned short crc = checksum_crc16(mystring, sizeof(mystring)/sizeof(char));
  unsigned short internet = checksum_internet((unsigned short*)mystring, sizeof(mystring)/sizeof(char));

  // unsigned short sumofchars = 0;
  //
  // for (int i = 0; i < sizeof(mystring); i++)
  // {
  //   sumofchars += mystring[i];
  // }
  //
  // printf("sumofchars: %u\n", sumofchars);

  printf("ccitt: %u\n", ccitt);
  printf("crc16: %u\n", crc);
  printf("internet: %u\n", internet);

  // unsigned short checkval = checksum_ccitt(mystring, sizeof(mystring));
  // printf("%i\n", checkval);
  // printf("-----------------\n");

  // while (fgets(buffer, sizeof buffer, fp) != NULL)
  // {
  //   unsigned short checkvalfile = checksum_ccitt(buffer, sizeof(buffer));
  //   printf("%u\n", checkvalfile);
  // }
  // fclose(fp);


  return 0;
}
