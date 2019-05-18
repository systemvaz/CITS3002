/*Initialise player structure variables for a new game.
* Then send START packet via: messaging.h */
int setup_game()
{
  for(int i = 0; i < MAX_CLIENTS; i++)
  {
    if(players.id[i] != 0 && players.in_lobby[i] != 1)
    {
      players.move_time[i] = time(NULL);
      players.move[i] = NONE;
      players.move_var[i] = 0;
      send_message(i, START);
    }
  }
  time_t t;
  srand((unsigned) time(&t));
  gamedice.first = rand() % 7 + 1;
  gamedice.second = rand() % 7 + 1;
  printf("Dice1: %d, Dice2: %d\n", gamedice.first, gamedice.second);
  printf("Waiting for moves from players....\n");
  return 1;
}

/*Check lobby for all connected players. Initialise them to player the next game.
* Set num_joined++ to tell server the player has joined the game */
void check_lobby()
{
  for(int i = 0; i < MAX_CLIENTS; i++)
  {
    if(players.in_lobby[i] == 1)
    {
      if(num_joined != NUM_PLAYERS)
      {
        players.in_lobby[i] = 0;
        num_joined++;
      }
    }
  }
}

/*For all users in a game who haven't made a move, compare current time to the
* time move was requested of user. If 30secs then flag user as timing out.
* Set players_ready++ to allow game logic to continue. */
void check_timeouts()
{
  for(int i = 0; i < MAX_CLIENTS; i++)
  {
    if(players.id[i] != 0 && players.move[i] == NONE && players.in_lobby[i] != 1)
    {
      if(difftime(time(NULL), players.move_time[i]) >= 30)
      {
        //Player has timed out on making moves. User will get a FAIL packet after round
        printf("Player %d has timed out on making a move\n", players.id[i]);
        players.timed_out[i] = 1;
        players_ready++;
      }
    }
  }
}

/*called after game round to see if only one player left. If so, we have a
* victor and VICT packet is sent, via messaging.h, then kill_user via: init_sessions.h
* When there is a victory or all players eliminated, to_lobby is set to 0 to
* so new players can join the next game */
void check_victory()
{
  printf("Checking victory. numjoined: %d\n", num_joined);
  if(num_joined == 1)
  {
    printf("Victory detected. Finding victor....\n");
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
      printf("Active player: %d\n", players.id[i]);
      if(players.id[i] != 0 && players.in_lobby[i] != 1)
      {
        printf("Sending VICT to user: %d\n", players.id[i]);
        send_message(i, VICT);
        kill_user(i);
        num_elim = 0;
        to_lobby = 0;
      }
    }
  }
  else if(num_joined < 1)
  {
    //All players lost. Reset.
    printf("All players eliminated, resetting....\n");
    num_elim = 0;
    to_lobby = 0;
  }
}

/*Game results are tallied, player lives are adjusted accordingly. Kill users
* that have depleated all their lives. Send appropriate packets via: messaging.h */
void tally_results()
{
  printf("tally_results()....\n");
  for(int i = 0; i < MAX_CLIENTS; i++)
  {
    if(players.id[i] != 0 && players.in_lobby[i] != 1)
    {
      if(players.pass[i] == 1)
      {
        players.level[i]++;
        send_message(i, PASS);
      }
      else if(players.pass[i] == 0)
      {
        players.lives[i]--;
        if(players.lives[i] == 0)
        {
          send_message(i, ELIM);
          kill_user(i);
        }
        else
        {
          players.level[i]++;
          send_message(i, FAIL);
        }
      }
    }
    //Reset any move timeout flags now that round has finished
    players.timed_out[i] = 0;
  }
}

/*Player the game. Generate random numbers for each dice. First we process what
* the rolled numbers mean in terms of even or odd, doubles. Then for each user
* check to see if their move resulted in a pass or fail. */
void play_round()
{
  //Check result for each players_ready
  printf("All player moves received. player_round()....\n");
  int sum = gamedice.first + gamedice.second;
  client_moves evenodd;
  int doubles;
  if(sum % 2 == 0)
  {
    evenodd = EVEN;
  }
  else
  {
    evenodd = ODD;
  }
  if(gamedice.first == gamedice.second)
  {
    doubles = 1;
  }
  else
  {
    doubles = 0;
  }

  for(int i = 0; i < MAX_CLIENTS; i++)
  {
    if(players.id[i] != 0 && players.in_lobby[i] != 1)
    {
      if(players.move[i] == EVEN || players.move[i] == ODD)
      {
        if(players.move[i] != evenodd)
        {
          players.pass[i] = 0;
        }
        else
        {
          players.pass[i] = 1;
        }
      }
      else if(players.move[i] == DOUB)
      {
        if(doubles == 1)
        {
          players.pass[i] = 1;
        }
        else
        {
          players.pass[i] = 0;
        }
      }
      else if(players.move[i] == CON)
      {
        if(players.move_var[i] == gamedice.first || players.move_var[i] == gamedice.second)
        {
          players.pass[i] = 1;
        }
        else
        {
          players.pass[i] = 0;
        }
      }

      if(players.timed_out[i] == 1)
      {
        players.pass[i] = 0;
      }
    }
  }
}
