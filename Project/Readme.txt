/--------------------------------------------------
CITS3002 Project 2019
Author: Alexander Varano della Vergiliana
Student ID: 22268701
/--------------------------------------------------

Files:
gameserver.c    (Main C program)
structs.h       (Global variables and structures)
server.h        (Initialises server by creating socket, setting options and binding to port number 4444)
sessions.h      (Handles client connections, disconnections, killing users, setting up player structures)
messaging.h     (Handles packet formation and translation. Performs sanity/security checks on incoming packets)
gamelogic.h     (Provides main game management functions. Setup, run and close games. Check timeouts. Check lobby)
Makefile        (Compiles program using gcc with flags for c99)

Runtime environment:
Program has been tested on Ubuntu Linux 18.04.2 LTS and Red Hat Enterprise Linux Workstation 7.6 (lab computers).

Make usage:
make gameserver

Run program:
./gameserver <number of players>

Example: Start the server suporting games with 4 users.
./gameserver 4

Info:
Gameserver is a Tier 4 system, covering all requirements stipulated from tier 1 through to 4.

Supported number of users:
Gameserver has been designed with flexibility in mind and supports a lobby system which allows for accepting new
connections and INIT requests while a game is in process. There are two main variables to be aware of in structs.h,
NUM_PLAYERS which is set at runtime, is the number of players requried per game & MAX_CLIENTS which is the maximum
number of client connections the server will accept before sending REJECT packets. MAX_CLIENTS is currently set at
12 and has been tested up to this number, however you can increase or decrease this number to your liking.

For example: Running "./gameserver 4", will support rounds of games with 4 users at a time, with an additional 8 users waiting in the lobby (12 total connected clients). Once a game round has a victory or all players are eliminated, gameserver
automatic takes the next 4 users (on a first in, first out basis) from the lobby and runs the next round of games. Etc etc.
Gameserver does not shutdown and this process runs continously with robust controls for handling unexpected behaviour.






