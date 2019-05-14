int create_socket(int port)
{
  int server_fd;
  //Create socket....
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(server_fd < 0)
  {
    fprintf(stderr, "Could not create socket\n");
    exit(EXIT_FAILURE);
  }
  return server_fd;
}

struct sockaddr_in setops_bind(int port, int server_fd)
{
  int opt_val, err;
  struct sockaddr_in server;

  //Set socket options....
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  opt_val = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

  //Bind socket to port number....
  err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
  if(err < 0)
  {
    fprintf(stderr, "Could not bind on socket");
    exit(EXIT_FAILURE);
  }
  return server;
}

void listen_port(int port, int server_fd)
{
  //Listen on port number....
  int err = listen(server_fd, 128);
  if(err < 0)
  {
    fprintf(stderr, "Could not listen on socket");
    exit(EXIT_FAILURE);
  }
  printf("Server is listening on %d\n", port);
}