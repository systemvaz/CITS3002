#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>

// ---------------------------------------------------------------------------

int64_t timing(bool start)
{
    static      struct timeval startw, endw;
    int64_t     usecs   = 0;

    if(start) {
        gettimeofday(&startw, NULL);
    }
    else {
        gettimeofday(&endw, NULL);
        usecs   =
               (endw.tv_sec - startw.tv_sec)*1000000 +
               (endw.tv_usec - startw.tv_usec);
    }
    return usecs;
}

int main(void)
{
//  START OUR TIMING
    timing(true);

//  DO SOMETHING THAT WILL OCCUPY THE CPU
    for(int i=0 ; i<1000000 ; ++i) {
        do_something();
    }

//  PRINT THE TIME TAKEN
    printf("%lldusecs\n", timing(false));

    exit(EXIT_SUCCESS);
}
