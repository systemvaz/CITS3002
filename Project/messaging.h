/*Packet formation to be sent to client. Then sent. */
void send_message(int i, server_messages to_send)
{
  char packet[14];
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

/*Packet from client after passing sanity checks. Perform appropriate actions
* if new user trying to initialise a connection. If game in process and client
* is a valid player, interpret move packets and log to player structure. */
void parse_message(int i, char buffer[])
{
  printf("Buffer: %s\n", buffer);
  if(strcmp("INIT", buffer) == 0 && num_clients <= MAX_CLIENTS)
  {
    printf("INIT recieved\n");
    //Setup player variables: init_sessions.h
    initialise_player(i);
    //Send WELCOME packet: messaging.h
    send_message(i, WELCOME);
  }
  else if(strcmp("INIT", buffer) == 0 && num_clients > MAX_CLIENTS)
  {
    printf("Rejecting connection %d, too many connections\n", players.fd[i]);
    //Send REJECT packet: messaging.h
    send_message(i, REJECT);
    //Kill connection: init_sessions.h
    kill_user(i);
  }

  if(strstr(buffer, "MOV") && players_ready != (NUM_PLAYERS - num_elim)
     && players.timed_out[i] != 1 && players.in_lobby[i] != 1)
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
      players.move_var[i] = atoi(&buffer[11]);
    }
    printf("Moves received from %d\n", players.id[i]);
    players_ready++;
  }
}

/*Packet recieved from client undergoes sanity and security checks. If OK, sent
* to parse_message function. */
void check_message(int i, char buffer[], int num_bytes)
{
  players.packets[i]++;
  printf("client %d --> %s\n", players.fd[i], buffer);

  /*Anti-flooding checks. If packet count exceeds threshold of 10, kill_user.
  * The packet count will be reset later after a game round. */
  if(players.packets[i] >= 10)
  {
    printf("Flooding detected. Killing user %d.\n", players.fd[i]);
    kill_user(i);
  }

  /*Anti-injection attempt checks. If packet contains illegal characters, kill user. */
  char illegal_chars[12] = {'!', '=', '"', '&', '%', '*', '+', '-', '/', '(', ')', ';'};
  for(int j = 0; j < 12; j++)
  {
    if(strchr(buffer, illegal_chars[j]) != NULL)
    {
      printf("Possible injection attack. Killing user %d\n", players.fd[i]);
      kill_user(i);
    }
  }

  /*Anti-cheating and illegal move checks if recieving a MOV packet.
  * Quite ruthless, kill user on detection of these */
  if(strstr(buffer, "MOV"))
  {
    /*If MOV packet, user supplied player ID number should match that recorded.
    * If it doesn't, may be player trying impersonate another so kill them */
    int id_check = atoi(&buffer[0]);
    if(id_check != players.id[i])
    {
      printf("Impersonation detected. Killing user: %d\n", players.id[i]);
      kill_user(i);
    }
    /*If player is trying to change their MOV mid game, kill them. */
    if(players.move[i] != NONE)
    {
      printf("Double move attempt detected. Killing user: %d\n", players.id[i]);
      kill_user(i);
    }
    /*If player tries to send both ODD and EVEN moves in the same packet, kill them */
    if(strstr(buffer, "EVEN"))
    {
      if(strstr(buffer, "ODD"))
      {
        printf("Cheat move detected. Trying ODD and EVEN. Killing user: %d\n", players.id[i]);
        kill_user(i);
      }
    }
    if(strstr(buffer, "ODD"))
    {
      if(strstr(buffer, "EVEN"))
      {
        printf("Cheat move detected. Trying ODD and EVEN. Killing user: %d\n", players.id[i]);
        kill_user(i);
      }
    }
    /* If player move is CON, make sure the number they provide is within range 1->6.
    * If not kill user. */
    if(strstr(buffer, "CON"))
    {
      if(atoi(&buffer[10]) < 1 || atoi(&buffer[10]) > 6)
      {
        printf("Illegal CON moves detected. Killing user: %d\n", players.id[i]);
        kill_user(i);
      }
    }
  }

  /* Finally if packet pass above checks and is no more than character array length 14
  * (includes null terminated bytes), then pass to parse_message function: messaging.h */
  if(num_bytes <= (sizeof(char)*14))
  {
    parse_message(i, buffer);
  }
}
