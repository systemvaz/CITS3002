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
  game_started = 0;
  players_ready = 0;

  while(true)
  {
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);
    buffer = calloc(BUFFER_SIZE, sizeof(char));

    //Check for new connections - init_sessions.h
    listen_connections(server_fd, server, client, mytimeout);
    //Check active connections still connected - init_sessions.h
    //check_alives();

    for(int i = 0; i < MAX_CLIENTS; i++)
    {
      if(FD_ISSET(players.fd[i], &readfds))
      {
        // printf("trying to get user input\n");
        read(players.fd[i], buffer, BUFFER_SIZE);
        check_message(i, buffer);
      }
    }

    // printf("check\n");
    if(num_joined == NUM_PLAYERS && game_started == 0)
    {
      players_ready = 0;
      // printf("Setting up game\n");
      game_started = setup_game();
    }

    if(game_started == 1 && players_ready == NUM_PLAYERS)
    {
      play_round();
      tally_results();
      check_victory();
      game_started = 0;
      players_ready = 0;
    }

    // printf("end of while loop\n");
    free(buffer);
  }

}
