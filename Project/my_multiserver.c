#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int client_id = 0;
int num_players = 0;
int num_lives = 3;

typedef enum {INIT, MOV} client_messages;
typedef enum {EVEN, ODD, DOUB, CON} client_moves ;
typedef enum {WELCOME, START, PASS, FAIL, ELIM, VICT, REJECT, CANCEL} server_messages;

void send_message(int client_fd, server_messages to_send)
{
  char packet[14];
  //Construct packet to send to client....
  switch(to_send)
  {
    case WELCOME:
      sprintf(packet, "WELCOME,%d", client_fd);
      printf("WELCOME message sent\n");
      break;
    case REJECT:
      sprintf(packet, "REJECT");
      break;
    case START:
      sprintf(packet, "START,%d,%d", num_players, num_lives);
      break;
    case CANCEL:
      sprintf(packet, "CANCEL");
      break;
    case PASS:
      sprintf(packet, "%d,PASS", client_id);
      break;
    case FAIL:
      sprintf(packet, "%d,FAIL", client_id);
      break;
    case ELIM:
      sprintf(packet, "%d,ELIM", client_id);
      break;
    case VICT:
      sprintf(packet, "%d,VICT", client_id);
      break;
  }
  //Send packet....
  send(client_fd, packet, strlen(packet), 0);
}

void parse_message(int client_fd, char buf[])
{
  if(strcmp("INIT", buf) == 0)
  {
    send_message(client_fd, WELCOME);
  }
}

void play_game_round()
{

}

void setup_game()
{

}

void kill_game()
{

}

 int main(int argc, char *argv[])
 {
   int port = 4444;
   int true = 1;
   int max_clients = 10;
   int server_fd, client_fd, err, opt_val, client_socket[30], max_sd,
       activity, new_socket, addrlen, valread;
   struct sockaddr_in server, client;
   fd_set readfds;
   char *buffer;

   //initialise all client sockets to 0 so not checked
   for(int i = 0; i < max_clients; i++)
   {
     client_socket[i] = 0;
   }

   //Create master socket....
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

   addrlen = sizeof(server);

   //Accept connection to port number....
   while(true)
   {
     socklen_t client_len = sizeof(client);

     FD_ZERO(&readfds);
     FD_SET(server_fd, &readfds);
     max_sd = server_fd;

     //Add child sockets to set
     for(int i = 0; i < max_clients; i++)
     {
       client_fd = client_socket[i];
       if(client_fd > 0)
       {
         FD_SET(client_fd, &readfds);
       }
       if(client_fd > max_sd)
       {
         max_sd = client_fd;
       }
     }

     activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

     if(FD_ISSET(server_fd, &readfds))
     {
        new_socket = accept(server_fd, (struct sockaddr *)&server, (socklen_t*)&addrlen);
        if(new_socket < 0)
        {
          perror("accept");
          exit(EXIT_FAILURE);
        }

        printf("New connection, socket fd is %d, ip is: %s, port: %d\n",
               new_socket, inet_ntoa(server.sin_addr), ntohs(server.sin_port));

        send_message(new_socket, WELCOME);

        //Add new socket to array of sockets
        for(int i = 0; i < max_clients; i++)
        {
          if(client_socket[i] == 0)
          {
            client_socket[i] = new_socket;
            printf("Adding to list of sockets\n");
            break;
          }
        }

        for(int i = 0; i < max_clients; i++)
        {
          client_fd = client_socket[i];

          if((valread = read(client_fd, buffer, BUFFER_SIZE)) == 0)
          {
            //Somebody disconnected , get his details and print
            getpeername(client_fd, (struct sockaddr*)&server, (socklen_t*)&addrlen);
            printf("Host disconnected , ip %s , port %d \n" ,
            inet_ntoa(server.sin_addr) , ntohs(server.sin_port));
            //Close the socket and mark as 0 in list for reuse
            close(client_fd);
            client_socket[i] = 0;
          }
          else
          {
            parse_message(client_fd, buffer);
          }
        }

     }

   }

 }
