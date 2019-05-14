void send_message(int i, server_messages to_send)
{
  char packet[15];
  //Construct packet to send to client....
  switch(to_send)
  {
    case WELCOME:
      sprintf(packet, "WELCOME,%d", players.id[i]);
      printf("WELCOME message sent to client %d\n", players.fd[i]);
      break;
    case REJECT:
      sprintf(packet, "REJECT");
      printf("REJECT message sent to client %d\n", players.fd[i]);
      break;
    case START:
      sprintf(packet, "START,%d,%d", num_joined, players.lives[i]);
      printf("START message sent to client %d, player %d\n", players.fd[i], players.id[i]);
      break;
    case CANCEL:
      sprintf(packet, "CANCEL");
      break;
    case PASS:
      sprintf(packet, "%d,PASS", players.id[i]);
      break;
    case FAIL:
      sprintf(packet, "%d,FAIL", players.id[i]);
      break;
    case ELIM:
      sprintf(packet, "%d,ELIM", players.id[i]);
      break;
    case VICT:
      sprintf(packet, "%d,VICT", players.id[i]);
      break;
  }
  //Send packet....
  printf("Sending to %d: %s\n", players.fd[i], packet);
  send(players.fd[i], packet, strlen(packet), 0);
}

void parse_message(int i, char buffer[])
{
  printf("Buffer: %s\n", buffer);
  if(strcmp("INIT", buffer) == 0 && num_joined < NUM_PLAYERS)
  {
    players.id[i] = i + 100;
    players.lives[i] = NUM_LIVES;
    players.level[i] = 1;
    printf("Player %d initialised\n", players.id[i]);
    num_joined++;
    send_message(i, WELCOME);
  }
  else if(strcmp("INIT", buffer) == 0 && num_joined >= NUM_PLAYERS)
  {
    printf("Rejecting connection %d, too many players\n", players.fd[i]);
    send_message(i, REJECT);
    kill_user(i);
  }

  if(strstr(buffer, "MOV") && players_ready != NUM_PLAYERS)
  {
    if(strstr(buffer, "EVEN"))
    {
      players.move[i] = EVEN;
    }
    else if(strstr(buffer, "ODD"))
    {
      players.move[i] = ODD;
    }
    else if(strstr(buffer, "DOUB"))
    {
      players.move[i] = DOUB;
    }
    else if(strstr(buffer, "CON"))
    {
      players.move[i] = CON;
      players.move_var[i] = atoi(&buffer[8]);
    }
    printf("Moves received from %d\n", players.id[i]);
    players_ready++;
  }
}

void check_message(int i, char buffer[])
{
  // printf("check_message()\n");
  printf("client %d --> %s\n", players.fd[i], buffer);
  parse_message(i, buffer);
}
