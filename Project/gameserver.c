#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>

#include "structs.h"
#include "init_server.h"
#include "init_sessions.h"
#include "messaging.h"
#include "game_logic.h"

int main(int argc, char *argv[])
{
  int true = 1;
  int server_fd;
  int game_started, waiting_moves;
  char *buffer;
  struct sockaddr_in server;
  struct sockaddr client;
  struct timeval mytimeout;

  mytimeout.tv_usec = 200;

  //Setup server - init_server.h
  server_fd = create_socket(PORT);
  server = setops_bind(PORT, server_fd);
  listen_port(PORT, server_fd);

  //Initialise all client sockets to 0 - init_sessions.h;
  initialise_clientfd();
  num_joined = 0;
  num_clients = 0;
  to_lobby = 0;
  game_started = 0;
  players_ready = 0;

  while(true)
  {
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);
    buffer = calloc(BUFFER_SIZE, sizeof(char));

    //Check for new connections - init_sessions.h
    listen_connections(server_fd, server, client, mytimeout);
    /*Check active connections still connected.
    * If not, Kill session: init_sessions.h */
    check_alives();

    //Read and parse messages from connected clients: messaging.h
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
      if(FD_ISSET(players.fd[i], &readfds))
      {
        read(players.fd[i], buffer, BUFFER_SIZE);
        check_message(i, buffer);
      }
    }

    /*If enough players joined, setup game and send START packets.
    * Set lobby flag to 1 so any new connections are placed to wait in the lobby.
    * game_logic.h */
    if(num_joined == (NUM_PLAYERS - num_elim) && game_started == 0)
    {
      players_ready = 0;
      to_lobby = 1;
      game_started = setup_game();
    }

    /*If game started, we are awaiting moves. Check for 30sec timeouts
    * for each player: game_logic.h */
    if(game_started == 1)
    {
      check_timeouts();
    }

    /* If all moves made, play the game, tally the results, check if Victory
    *  or if all players eliminated. If no more players check the lobby to
    *  initialise new players for a new round of games: game.logic.h */
    if(players_ready == (NUM_PLAYERS - num_elim) && game_started == 1)
    {
      play_round();
      tally_results();
      check_victory();

      if(to_lobby == 0)
      {
        check_lobby();
      }

      game_started = 0;
      players_ready = 0;
    }

    free(buffer);
  }

}
