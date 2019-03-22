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
    printf("line %i || swaped: %d | og: %d | og hex: %.2X\n", i, bswap_32(buffer), buffer, buffer);
    i++;
  }
  printf("\n");

}
