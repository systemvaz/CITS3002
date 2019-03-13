#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define FRAMESIZE 200

void main (int argc, char* argv)
{
  typedef unsigned char byte;
  byte frame[FRAMESIZE];

  //SEED THE RANDOM NUMBER GENERATOR
  srand(getpid());
  //POPULATE THE FRAME WITH RANDOM BYTES
  for (int i = 0; i < FRAMESIZE; i++)
  {
    frame[i] = rand() % 256;
    printf("%c\n", frame[i]);
  }

}
