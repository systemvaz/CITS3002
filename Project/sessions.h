/*-------------------------------------------------
*  CITS3002 Project 2019.
*  Author: Alexander Varano della Vergiliana
*  Student ID: 22268701
*--------------------------------------------------*/

/*Set all file descriptors to 0 for all possible number of clients.*/
void initialise_clientfd()
{
  for(int i = 0; i <= MAX_CLIENTS; i++)
  {
    players.fd[i] = 0;
  }
}

/*Initialise a new connection with game variables, id, lives, level.
* If a game is already in process, then flag them as waiting in the lobby.*/
void initialise_player(int i)
{
  players.id[i] = i+1;
  players.lives[i] = NUM_LIVES;
  players.level[i] = 1;
  players.move[i] = NONE;
  players.move_var[i] = 0;
  players.pass[i] = 0;
  players.packets[i] = 0;
  players.in_game[i] = 0;
  printf("Player %d initialised\n", players.id[i]);

  if(to_lobby == 1)
  {
    players.in_lobby[i] = 1;
    printf("Game in session, player %d added to lobby\n", players.id[i]);
  }
  else
  {
    num_joined++;
    players.in_lobby[i] = 0;
  }
}

/*Listen for new connections to the server. File descriptor for new socket
* connection is then added to the player structure at the first available array
* possition (eg, where fd[i] = 0). */
void listen_connections(int server_fd, struct sockaddr_in server, struct sockaddr client, struct timeval mytimeout)
{
  socklen_t client_len = sizeof(client);
  max_sd = server_fd;

  for(int i = 0; i <= MAX_CLIENTS; i++)
  {
    if(players.fd[i] > 0)
    {
      FD_SET(players.fd[i], &readfds);
    }
    if(players.fd[i] > max_sd)
    {
      max_sd = players.fd[i];
    }
  }

  activity = select(max_sd + 1, &readfds, NULL, NULL, &mytimeout);

  if(FD_ISSET(server_fd, &readfds))
  {
    int new_socket = accept(server_fd, (struct sockaddr *) &client, &client_len);
    printf("New connection, socket fd is %d, port: %d\n",
           new_socket, ntohs(server.sin_port));

    for(int i = 0; i <= MAX_CLIENTS; i++)
    {
      if(players.fd[i] == 0)
      {
        printf("Adding client to list of sockets\n");
        players.fd[i] = new_socket;
        num_clients++;
        break;
      }
    }
  }
}

/*Close client connection gracefully. If currently an active player not in the
* lobby change num_elim, num_joined, player_ready variable to ensure game play_round
* continues without interuption. */
void kill_user(int i)
{
  printf("Killing user %d\n", players.fd[i]);
  close(players.fd[i]);
  num_clients--;
  players.fd[i] = 0;
  players.packets[i] = 0;

  //If active game player, alter game variables so game logic continues uninterupted
  if(players.id[i] != 0 && players.in_lobby[i] != 1)
  {
    printf("Player %d removed from game.\n", players.id[i]);
    players.id[i] = 0;
    players.level[i] = 0;
    players.move_var[i] = 0;
    players.pass[i] = 0;
    if(players.in_game[i] == 1)
    {
      num_elim++;
    }
    num_joined--;
    /*If player had submitted move before disconnecting,
    * alter player_ready variable so game logic continues uninterupted. */
    if(players.move[i] != NONE && players.in_game[i] == 1)
    {
      players_ready--;
    }
  }

  players.move[i] = NONE;
  players.in_lobby[i] = 0;
  players.in_game[i] = 0;
  players.id[i] = 0;
}

/*Initialise a non-blocking recv on all connected clients. If error 0, client
* has disconnected and kill_user is called on the user to clean up the socket
* and ensure game variables are modified to ensure no disruption to ongoing game. */
void check_alives()
{
  char *tempbuffer;
  for(int i = 0; i < MAX_CLIENTS; i++)
  {
    if(players.fd[i] != 0)
    {
      if(recv(players.fd[i], tempbuffer, sizeof(tempbuffer), MSG_DONTWAIT) == 0)
      {
        printf("Client %d lost connection. Killing\n", players.fd[i]);
        kill_user(i);
      }
    }
  }
}
