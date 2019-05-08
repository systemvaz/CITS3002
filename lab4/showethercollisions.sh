#!/bin/sh

# location of the cnet simulator
SIMULATION_SECONDS="100"

run_etherflood () {
    ncollisions=`cnet -WTqz -S 19 -e ${SIMULATION_SECONDS}sec ETHERFLOOD $1 | grep collision | cut '-d:' -f2 | sed 's/ //g'`
    per=`expr $ncollisions / $SIMULATION_SECONDS`
    printf "avg. interframe interval = %8iusecs, %8i collisions/sec\\n" $1 $per
}

for freq in 100 1000 10000 100000
do
    run_etherflood $freq
done
