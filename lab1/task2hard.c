#include <stdio.h>
#include <stdlib.h>
#include "checksum.h"
#include "checksum_ccitt.c"

int main (int argc, char* argv[])
{

  FILE* fp = fopen("test.in", "r");
  unsigned char buffer[500];
  unsigned short tocheck[34];
  int check[34];
  int count = 0;

  while (fgets(buffer, sizeof buffer, fp) != NULL)
  {
    //unsigned char data = buffer;
    tocheck[count] = checksum_ccitt(buffer, sizeof(buffer));
    count++;
  }

  fclose(fp);
  fp = fopen("soln.out", "r");
  count = 0;

  while (fgets(buffer, sizeof buffer, fp) != NULL)
  {
    check[count] = atoi(buffer);
    count++;
  }

  for (int i = 0; i < count; i++)
  {
    if (check[i] != tocheck[i])
    {
      printf("Checksum Fail! input:%i | output:%i | Count:%i\n", tocheck[i], check[i], i);
    }
  }

  fclose(fp);
  return 0;
}
