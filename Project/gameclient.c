#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define PORT 4444

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
    int lives, players;

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

    read(sock, buffer, 11);
    printf("%s\n", buffer);
    if(strstr(buffer, "WELCOME"))
    {
      memset(buffer, 0, sizeof(buffer));
      printf("Waiting for game to start...\n");
      read(sock, buffer, 14);
      printf("%s\n", buffer);
    }
    else if(strstr(buffer, "REJECT"))
    {
      printf("Rejected: Too many players on server.\n");
      send(sock, "QUIT", strlen("QUIT"), 0);
      return 0;
    }

    if(strstr(buffer, "START"))
    {
      printf("Players %c, Lives: %c\n", buffer[6], buffer[8]);

    }

    sleep(10);
    send(sock, "QUIT", strlen("QUIT"), 0);
    return 0;
}