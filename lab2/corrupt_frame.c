#include <stdlib.h>
#include <stdbool.h>
#include <sys/param.h>

/*  This is a simple function designed to test checksum algorithms.
    The function randomly corrupts one or more bits of a given frame.

    As corruption is random, you'll need to call srand() from somewhere
    earlier in your program to ensure different random sequences.

    You may like to devise other forms of corruption and run tests on them.
 */

typedef unsigned char    byte;

void corrupt_frame(byte frame[], int length)
{
//  this is a severe/unrealistic function, swapping adjacent, different chars
    while(true) {
        int byte                = (rand() % (length-1));

        if(frame[byte] != frame[byte+1]) {      // ensure chars are different
            byte save  = frame[byte];

            frame[byte]         = frame[byte+1];
            frame[byte+1]       = save;
            break;
        }
    }
}


#if   OTHER_POSSIBLE_FORMS_OF_FRAME_CORRUPTION

//  corrupt one character by setting it to the complement of its own value
    int byte    = (rand() % length);
    frame[byte] = ~(frame[byte]) ;



//  corrupt one bit of a byte by toggling its value
    int byte    = (rand() % length);
    int bit     = (rand() % NBBY);
    frame[byte] = (frame[byte] ^ (1UL << bit));



//  corrupt a frame with a burst error
#define MIN_BURSTLENGTH         10
#define MAX_BURSTLENGTH         100
    int nbits           = (length * NBBY);
    while(true) {
        int     b0      = rand() % nbits;
        int     b1      = rand() % nbits;
	int	burst	= b1 - b0;

        if(burst >= MIN_BURSTLENGTH && burst <= MAX_BURSTLENGTH) {
            for(int b=b0 ; b<b1 ; ++b) {
                int     byte    = b / NBBY;
                int     bit     = b % NBBY;

                frame[byte]     = (frame[byte] | (1UL << bit));
            }
            break;
        }
    }

#endif
