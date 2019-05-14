void initialise_clientfd()
{
  for(int i = 0; i < MAX_CLIENTS; i++)
  {
    players.fd[i] = 0;
  }
}

void listen_connections(int server_fd, struct sockaddr_in server, struct sockaddr client, struct timeval mytimeout)
{
  printf("listen_conntections()\n");
  socklen_t client_len = sizeof(client);
  max_sd = server_fd;

  for(int i = 0; i < MAX_CLIENTS; i++)
  {
    printf("%d\n", i);
    if(players.fd[i] > 0)
    {
      printf("FD_SET\n");
      FD_SET(players.fd[i], &readfds);
    }
    if(players.fd[i] > max_sd)
    {
      printf("max_sd\n");
      max_sd = players.fd[i];
    }
  }

  printf("before: activity select statement\n");
  activity = select(max_sd + 1, &readfds, NULL, NULL, &mytimeout);
  printf("finished: listen_connections()\n");

  if(FD_ISSET(server_fd, &readfds))
  {
    int new_socket = accept(server_fd, (struct sockaddr *) &client, &client_len);
    printf("New connection, socket fd is %d, port: %d\n",
           new_socket, ntohs(server.sin_port));

    for(int i = 0; i < MAX_CLIENTS; i++)
    {
      if(players.fd[i] == 0)
      {
        printf("Adding client to list of sockets\n");
        players.fd[i] = new_socket;
        break;
      }
    }
  }
}

void kill_user(int i)
{
  printf("Killing user %d\n", players.fd[i]);
  close(players.fd[i]);
  players.fd[i] = 0;
  num_joined--;
}