#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define NUM_PLAYERS 1
#define NUM_LIVES 3

int num_joined = 0;
int game_started = 0;
int waiting_moves = 0;
int current_level = 1;
int player_ID = 0;

typedef enum {INIT, MOV} client_messages;
typedef enum {EVEN, ODD, DOUB, CON} client_moves ;
typedef enum {WELCOME, START, PASS, FAIL, ELIM, VICT, REJECT, CANCEL} server_messages;

struct
{
  int fd;
  int id;
  int lives;
  int level;
  client_moves move;
  int move_var;
} player[NUM_PLAYERS];

struct
{
  int first;
  int second;
} dice;

void send_message(int client_fd, int player_id, server_messages to_send)
{
  char packet[15];
  //Construct packet to send to client....
  switch(to_send)
  {
    case WELCOME:
      sprintf(packet, "WELCOME,%d", player[player_id].id);
      printf("WELCOME message sent to client %d\n", client_fd);
      break;
    case REJECT:
      sprintf(packet, "REJECT");
      printf("REJECT message sent to client %d\n", client_fd);
      break;
    case START:
      sprintf(packet, "START,%d,%d", num_joined, player[player_id].lives);
      printf("START message sent to client %d, player %d\n", client_fd, player_id);
      break;
    case CANCEL:
      sprintf(packet, "CANCEL");
      break;
    case PASS:
      sprintf(packet, "%d,PASS", player_id);
      break;
    case FAIL:
      sprintf(packet, "%d,FAIL", player_id);
      break;
    case ELIM:
      sprintf(packet, "%d,ELIM", player_id);
      break;
    case VICT:
      sprintf(packet, "%d,VICT", player_id);
      break;
  }
  //Send packet....
  send(client_fd, packet, strlen(packet), 0);
}

void init_player(int client_fd)
{
  player[player_ID].fd = client_fd;
  player[player_ID].id = player_ID + 100;
  player[player_ID].lives = NUM_LIVES;
  player[player_ID].level = 1;
  printf("Player %d initialised\n", player[player_ID].id);
  send_message(client_fd, player_ID, WELCOME);
  player_ID++;
  num_joined++;
}

void parse_message(int client_fd, char buf[])
{
  if(strcmp("INIT", buf) == 0 && num_joined < NUM_PLAYERS)
  {
    init_player(client_fd);
  }
  else if(strcmp("INIT", buf) == 0 && num_joined >= NUM_PLAYERS)
  {
    printf("Rejecting connection %d, too many players\n", client_fd);
    send_message(client_fd, 0, REJECT);
  }

  if(strstr(buf, "MOV") && waiting_moves == 1)
  {

  }
}

void play_game_round()
{

}

void setup_game()
{
  game_started = 1;
  waiting_moves = 1;
  for(int i = 0; i < NUM_PLAYERS; i++)
  {
    send_message(player[i].fd, i, START);
  }
  time_t t;
  srand((unsigned) time(&t));
  dice.first = rand() % 7 + 1;
  dice.second = rand() % 7 + 1;
  printf("Dice1: %d, Dice2: %d\n", dice.first, dice.second);
}

void kill_game()
{

}

 int main(int argc, char *argv[])
 {
   int port = 4444;
   int true = 1;
   int max_clients = 5;
   int server_fd, client_fd, err, opt_val;
   int client_socket[max_clients], max_sd, activity, new_socket, valread;
   struct sockaddr_in server;
   struct sockaddr client;
   char *buf;
   fd_set readfds;

   //Create socket....
   server_fd = socket(AF_INET, SOCK_STREAM, 0);
   if(server_fd < 0)
   {
     fprintf(stderr, "Could not create socket\n");
     exit(EXIT_FAILURE);
   }

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

   //Listen on port number....
   err = listen(server_fd, 128);
   if(err < 0)
   {
     fprintf(stderr, "Could not listen on socket");
     exit(EXIT_FAILURE);
   }
   printf("Server is listening on %d\n", port);

   //Initialise all client sockets
   for(int i = 0; i < max_clients; i++)
   {
     client_socket[i] = 0;
   }

   //Accept connection to port number....
   while(true)
   {
     buf = calloc(BUFFER_SIZE, sizeof(char));
     socklen_t client_len = sizeof(client);
     FD_ZERO(&readfds);
     FD_SET(server_fd, &readfds);
     max_sd = server_fd;

     for(int i = 0; i < max_clients; i++)
     {
       if(client_socket[i] > 0)
       {
         FD_SET(client_socket[i], &readfds);
       }
       if(client_socket[i] > max_sd)
       {
         max_sd = client_socket[i];
       }
     }

     activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

     if(FD_ISSET(server_fd, &readfds))
     {
       new_socket = accept(server_fd, (struct sockaddr *) &client, &client_len);
       printf("New connection, socket fd is %d, port: %d\n",
              new_socket, ntohs(server.sin_port));

       for(int i = 0; i < max_clients; i++)
       {
         if(client_socket[i] == 0)
         {
           printf("Adding client to list of sockets\n");
           client_socket[i] = new_socket;
           break;
         }
       }
     }

     for(int i = 0; i < max_clients; i++)
     {
       if(FD_ISSET(client_socket[i], &readfds))
       {
         read(client_socket[i], buf, BUFFER_SIZE);
         if(strcmp("QUIT", buf) == 0)
         {
           close(client_socket[i]);
           printf("Client %d connection closed\n", client_socket[i]);
           num_joined--;
           client_socket[i] = 0;
         }
         else
         {
           printf("client %d --> %s\n", client_socket[i], buf);
           parse_message(client_socket[i], buf);
         }
       }
     }

     if(num_joined == NUM_PLAYERS && game_started == 0)
     {
       setup_game();
     }

     if(game_started == 1 && waiting_moves == 0)
     {
       play_game_round();
     }

     free(buf);
   }

 }
