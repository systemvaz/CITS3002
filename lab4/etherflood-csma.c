#include <cnet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//  THIS IS A SIMPLE TEST TO FLOOD A SINGLE ETHERNET SEGMENT

typedef struct {
    CnetNICaddr		dest;			// 6 bytes
    CnetNICaddr		src;			// 6 bytes
    int16_t		type;			// 2 bytes
    char		payload[LAN_MAXDATA];	// 1500 bytes
} IEEE802_3_FRAME;

#define	LEN_ETHERHEADER		(sizeof(IEEE802_3_FRAME) - LAN_MAXDATA)
#define	DEFAULT_FREQ		100000
#define BACKOFF_SLOT        52
#define BACKOFF_LIMIT       1024

//  freq STORES THE AVERAGE NUMBER OF microseconds BETWEEN TRANSMISSIONS
int64_t	freq	= DEFAULT_FREQ;
int slot = 0;
int curr_max_backoff = BACKOFF_SLOT;


//  ATTEMPT TO TRANSMIT AN ETHERNET FRAME - POSSIBLE COLLISIONS ARE IGNORED
EVENT_HANDLER(transmitting)
{
    IEEE802_3_FRAME	frame;
    int			link	= 1;
    size_t	        length	= LAN_MINPACKET + CNET_rand() % 1000;

    if(CNET_carrier_sense(link) == 1)
    {
        // Link is currently transmitting/receiving
        //reschedule transmission
    }

    memset(&frame, 'a', sizeof(frame));				// fill frame (!)
    CHECK(CNET_write_physical(link, &frame, &length));

    CNET_start_timer(EV_TIMER1, CNET_rand()%freq + 1, 0);	// reschedule
}


//  RECEIVE, BUT JUST IGNORE, AN ARRIVING ETHERNET FRAME
EVENT_HANDLER(receiving)
{
    IEEE802_3_FRAME	frame;
    int			link;
    size_t		length	= sizeof(frame);

    CHECK(CNET_read_physical(&link, &frame, &length));
}


EVENT_HANDLER(reboot_node)
{
    char	**argv  = (char **)data;

//  OBTAIN DESIRED FRAME GENERATION FREQUENCY FROM COMMAND-LINE
    if(argv[1] != NULL)
	freq	= atoi(argv[1]);

    CNET_srand(getpid() + nodeinfo.nodenumber);

//  DECLARE EVENT HANDLERS TO RECEIVE AND TRANSMIT FRAMES
    CNET_set_handler(EV_PHYSICALREADY, receiving, 0);
    CNET_set_handler(EV_TIMER1, transmitting, 0);

//  SCHEDULE GENERATION OF FRAMES
    CNET_start_timer(EV_TIMER1, 1000000 + CNET_rand()%freq, 0);
}
