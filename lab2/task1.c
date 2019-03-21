#include <stdio.h>
#include <stdlib.h>
#include <byteswap.h>
#include <string.h>

void main (int argc, char* argv[])
{

  FILE* fp = fopen("Q1.datafile", "rb");
  unsigned int buffer;
  char binary[4];
  int i = 1;

  while (!feof(fp))
  {
    fread(&buffer, 4, 1, fp);
    //fprintf(stdout, "%d", bswap_32(buffer));
  }

  printf("\n");

}
