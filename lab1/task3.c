#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
#include <sys/param.h>

#include "checksum_ccitt.c"
#include "checksum_crc16.c"
#include "checksum_internet.c"
#include "my_checksum.c"

#define FRAMESIZE 200

typedef unsigned char byte;
typedef enum {DLL_DATA, DLL_ACK, DLL_NACK} FRAMETYPE;

typedef struct
{
  FRAMETYPE type;
  int checksum_crc;
  int checksum_ccitt;
  int checksum_internet;
  int mychecksum;
  unsigned char data[FRAMESIZE];
} FRAME;

FRAME frame, og_frame;
byte save;
int countcrc = 0;
int countccitt = 0;
int countcompfails = 0;
int countinternetfails = 0;
int countmychecksum = 0;
bool compfail = false;

void random_frames()
{
  srand(clock());
  for (int i = 0; i < FRAMESIZE; i++)
  {
    frame.data[i] = rand() % 256;
    og_frame.data[i] = frame.data[i];
  }

  frame.type = DLL_DATA;
  frame.checksum_crc = 0;
  frame.checksum_ccitt = 0;
  frame.checksum_internet = 0;
  frame.mychecksum = 0;
  frame.checksum_ccitt = checksum_ccitt(frame.data, FRAMESIZE);
  frame.checksum_crc = checksum_crc16(frame.data, FRAMESIZE);
  frame.checksum_internet = checksum_internet((unsigned short*)frame.data, FRAMESIZE);
  frame.mychecksum = my_checksum(frame.data, FRAMESIZE);
}

//  this is a severe/unrealistic function, swapping adjacent, different chars
void corrupt_frame_swap(byte frame[], int length)
{
  //srand(clock());
  srand(19);
  while(true)
  {
    int byte = (rand() % (length-1));

    if(frame[byte] != frame[byte+1])
    {      // ensure chars are different
      save  = frame[byte];
      frame[byte] = frame[byte+1];
      frame[byte+1] = save;
      break;
    }
  }
}

//  corrupt one character by setting it to the complement of its own value
void corrupt_frame_complement(byte frame[], int length)
{
  //srand(clock());
  srand(19);
  int byte    = (rand() % length);
  frame[byte] = ~(frame[byte]) ;
}

//  corrupt one bit of a byte by toggling its value
void corrupt_frame_toggle(byte frame[], int length)
{
  //srand(clock());
  srand(19);
  int byte    = (rand() % length);
  int bit     = (rand() % NBBY);
  frame[byte] = (frame[byte] ^ (1UL << bit));
}

//  corrupt a frame with a burst error
void corrupt_frame_burst(byte frame[], int length)
{
  #define MIN_BURSTLENGTH         10
  #define MAX_BURSTLENGTH         100
  int nbits           = (length * NBBY);
  //srand(clock());
  srand(19);
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

void do_checks(int framenum)
{
  for (int i = 0; i < FRAMESIZE; i++)
  {
    if (og_frame.data[i] != frame.data[i])
    {
      compfail = true;
    }
  }

  if(compfail)
  {
    countcompfails++;
    printf("FRAME: %i | Compare fail detected\n", framenum);
    compfail = false;
  }

  if (frame.checksum_ccitt != checksum_ccitt(frame.data, FRAMESIZE))
  {
    countccitt++;
    printf("FRAME: %i | CCITT fail detected\n", framenum);
  }
  if (frame.checksum_crc != checksum_crc16(frame.data, FRAMESIZE))
  {
    countcrc++;
    printf("FRAME: %i | CRC16 fail detected\n", framenum);
  }
  if (frame.checksum_internet != checksum_internet((unsigned short*)frame.data, FRAMESIZE))
  {
    countinternetfails++;
    printf("FRAME: %i | INTERNET fail detected\n", framenum);
  }
  if (frame.mychecksum != my_checksum(frame.data, FRAMESIZE))
  {
    countmychecksum++;
    printf("FRAME %i | MYCHECKSUM fail detected\n", framenum);
  }
}

int64_t timing(bool start)
{
  static struct timeval startw, endw;
  int64_t usecs = 0;

  if(start)
  {
    gettimeofday(&startw, NULL);
  }
  else
  {
    gettimeofday(&endw, NULL);
    usecs =
             (endw.tv_sec - startw.tv_sec)*1000000 +
             (endw.tv_usec - startw.tv_usec);
  }
  return usecs;
}

int main (int argc, char* argv[])
{
  if (argc != 3)
  {
    printf("<Usage: <PROGRAM_NAME> [type] [chance]\n");
    printf("Usage example: ./task3 b 2\n");
    printf("\n");
    printf("Please include error type and chance as program argument.\n");
    printf("type: s = swap , c = complement , t = toggle , b = burst\n");
    printf("chance: 2 = 2pct , 10 = 10pct, 50 = 50pct etc...\n");
    return -1;
  }
  char error_type = *argv[1];
  int error_chance = atoi(argv[2]);
  int doing_errors = 0;

  for (int i = 1; i <= 1000000; i++)
  {
    timing(true);
    random_frames();
    srand(clock());
    if ((rand() % 100 + 1) <= error_chance)
    {
      switch(error_type)
      {
        case 's':
          doing_errors++;
          corrupt_frame_swap(frame.data, FRAMESIZE);
          break;
        case 'c':
          doing_errors++;
          corrupt_frame_complement(frame.data, FRAMESIZE);
          break;
        case 't':
          doing_errors++;
          corrupt_frame_toggle(frame.data, FRAMESIZE);
          break;
        case 'b':
          doing_errors++;
          corrupt_frame_burst(frame.data, FRAMESIZE);
        break;
      }
    }

    do_checks(i);
  }

  printf("----------------------------------------------\n");
  printf("TIME ELAPSED: %li usecs\n", timing(false));
  printf("RESULTS:\n");
  printf("REAL ERRORS GENERATED: %i\n", doing_errors);
  printf("CRC checksum errors detected: %i\n", countcrc);
  printf("CCITT checksum errors detected: %i\n", countccitt);
  printf("INTERNET checksum errors detected: %i\n", countinternetfails);
  printf("MYCHECKSUM checksum errors detected: %i\n", countmychecksum);
  printf("Direct data compare fails detected: %i\n", countcompfails);
  return 0;
}
