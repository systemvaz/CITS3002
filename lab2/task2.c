#include <stdio.h>
#include <stdlib.h>
#include "checksum_ccitt.c"
#include "checksum_crc16.c"
#include "checksum_internet.c"

void main (int argc, char* argv[])
{
  char mystring[] = "Hello World";
  int mysize = sizeof(mystring)/sizeof(char);

  unsigned short ccitt = checksum_ccitt(mystring, mysize);
  unsigned short crc = checksum_crc16(mystring, mysize);
  int cinter = checksum_internet(&crc, sizeof(crc));

  printf("ccitt: %u\n", ccitt);
  printf("crc: %u\n", crc);
  printf("cinter: %i\n", cinter);

}
