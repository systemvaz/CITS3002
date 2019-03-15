#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/param.h>

#include "checksum_ccitt.c"
#include "checksum_crc16.c"

/*  This is a simple function designed to test checksum algorithms.
    The function randomly corrupts one or more bits of a given frame.

    As corruption is random, you'll need to call srand() from somewhere
    earlier in your program to ensure different random sequences.

    You may like to devise other forms of corruption and run tests on them.
 */

#define FRAMESIZE 200
typedef unsigned char byte;
byte frame[FRAMESIZE];
byte frameog[FRAMESIZE];
byte save;

int countccitt = 0;
int countcrc = 0;
int countcompfails = 0;

void randomframes()
{
  srand(time(NULL));
  for (int i = 0; i < FRAMESIZE; i++)
  {
    frame[i] = rand() % 256;
    frameog[i] = frame[i];
  }
}


void corrupt_frame(byte frame[], int length, char type)
{
  srand(time(NULL));
//  this is a severe/unrealistic function, swapping adjacent, different chars
  if (type == 's')
  {
    while(true)
    {
        int byte                = (rand() % (length-1));
        if(frame[byte] != frame[byte+1])
        {
          // ensure chars are different
          save = frame[byte];
          frame[byte]         = frame[byte+1];
          frame[byte+1]       = save;
          break;
        }
    }
  }

//  corrupt one character by setting it to the complement of its own value
  else if (type == 'c')
  {
    int byte    = (rand() % length);
    frame[byte] = ~(frame[byte]) ;
  }

//  corrupt one bit of a byte by toggling its value
  else if (type == 't')
  {
    int byte    = (rand() % length);
    int bit     = (rand() % NBBY);
    frame[byte] = (frame[byte] ^ (1UL << bit));
  }

//  corrupt a frame with a burst error
  else if (type == 'b')
  {
    #define MIN_BURSTLENGTH         10
    #define MAX_BURSTLENGTH         100
    int nbits           = (length * NBBY);
    while(true)
    {
      int     b0      = rand() % nbits;
      int     b1      = rand() % nbits;
    	int	burst	= b1 - b0;

      if(burst >= MIN_BURSTLENGTH && burst <= MAX_BURSTLENGTH)
      {
        for(int b=b0 ; b<b1 ; ++b)
        {
          int     byte    = b / NBBY;
          int     bit     = b % NBBY;
          frame[byte]     = (frame[byte] | (1UL << bit));
        }
        break;
      }
    }
  }
  else

  {
    printf("Incorrect error type argument provided\n");
  }

}


void dochecks()
{
  unsigned short checkogframeccitt, checkerrframeccitt,
                 checkogframecrc, checkerrframecrc;

  for (int i = 0; i < FRAMESIZE; i++)
  {
    checkogframeccitt = checksum_ccitt(&frameog[i], sizeof(frameog[i]));
    checkerrframeccitt = checksum_ccitt(&frame[i], sizeof(frame[i]));
    checkogframecrc = checksum_crc16(&frameog[i], sizeof(frameog[i]));
    checkerrframecrc = checksum_crc16(&frame[i], sizeof(frame[i]));

    if (frameog[i] != frame[i])
    {
      countcompfails++;
    }
    if (checkogframeccitt != checkerrframeccitt)
    {
      countccitt++;
      printf("(CCITT) checksum og: %u  | checksum err: %u  | Frame: %i\n", checkogframeccitt, checkerrframeccitt, i);
    }
    if (checkogframecrc != checkerrframecrc)
    {
      countcrc++;
      printf("(CRC16) checksum og: %u  | checksum err: %u  | Frame: %i\n", checkogframecrc, checkerrframecrc, i);
    }
  }

  printf("------------------------------------\n");
  printf("CCITT Checksum errors: %i\n", countccitt);
  printf("CRC Checksum errors: %i\n", countcrc);
  printf("Direct compare fails: %i\n", countcompfails);
  printf("------------------------------------\n");
}


int main(int argc, char** argv)
{
  if (argc != 2)
  {
    printf("Please include error type as program argument.\n");
    printf("s = swap , c = complement , t = toggle , b = burst\n");
    return -1;
  }

  char type = *argv[1];

  for (int i = 0; i <= 100000; i++)
  {
    randomframes();
    corrupt_frame(frame, sizeof(frame)/sizeof(char), type);
    dochecks();
  }

  return 0;
}
