#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define PORT 4444

typedef enum {EVEN, ODD, DOUB, CON, NONE} client_moves ;

int true = 1;

// char[14] check_reply(int length);
// {
//   i = 0;
//   while(i < length)
//   {
//
//   }
// }

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char reply[14];
    char buffer[1024] = {0};
    int lives, players, random;
    char player_id[4];
    char packet[15];
    char move[5];
    char convar[2];
    int selection;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock , "INIT", strlen("INIT"), 0);
    printf("INIT message sent\n");

    // strcpy(reply, check_reply(4));

    read(sock, buffer, 9);
    printf("%s\n", buffer);
    if(strstr(buffer, "WELCOME"))
    {
      memset(player_id, 0, sizeof(player_id));
      strcat(player_id, buffer + 8);
      memset(buffer, 0, sizeof(buffer));
      printf("Waiting for game to start...\n");
      int u = 1;
      while(u == 1)
      {
        memset(packet, 0, sizeof packet);
        memset(move, 0, sizeof move);
        read(sock, buffer, 9);
        printf("%s\n", buffer);
        if(strstr(buffer, "START"))
        {
          printf("Players %c, Lives: %c\n", buffer[6], buffer[8]);
          time_t t;
          //srand((unsigned) time(&t));
          //random = (rand() % 4)+1;
          //printf("rand: %d\n", random);
          printf("1:EVEN 2:ODD 3:DOUB 4:CON\n");
          scanf("%d", &selection);
          switch(selection)
          {
            case 1:
              strcpy(move, "EVEN");
              break;
            case 2:
              strcpy(move, "ODD");
              break;
            case 3:
              strcpy(move, "DOUB");
              break;
            case 4:
              strcpy(move, "CON");
              time_t t;
              srand((unsigned) time(&t));
              sprintf(convar, "%d", (rand() % 6)+1);
              break;
          }
          if(strstr(move, "CON"))
          {
            strcat(packet, player_id);
            strcat(packet, ",MOV,");
            strcat(packet, move);
            strcat(packet, ",");
            strcat(packet, convar);
          }
          else
          {
            strcat(packet, player_id);
            strcat(packet, ",MOV,");
            strcat(packet, move);
          }

          //scanf("%s", packet);
          printf("Packet about to send: %s\n", packet);
          // scanf("%d", &u);
          send(sock, packet, strlen(packet), 0);
          // send(sock, "100,EVEN", strlen("100,EVEN"), 0);
          printf("Move sent to server....\n");
        }
        else if(strstr(buffer, "ELIM") || strstr(buffer, "VICT") || strstr(buffer, "CANCEL"))
        {
          close(sock);
          return 0;
        }
        memset(buffer, 0, sizeof(buffer));
        read(sock, buffer, 6);
        printf("Buffer: %s\n", buffer);
        // scanf("%d", &u);
        // printf("%d\n", u);
      }

    }

    else if(strstr(buffer, "REJECT"))
    {
      printf("Rejected: Too many players on server.\n");
      send(sock, "QUIT", strlen("QUIT"), 0);
      return 0;
    }

    close(sock);
    return 0;
}
