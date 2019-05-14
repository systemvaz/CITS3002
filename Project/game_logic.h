int setup_game()
{
  for(int i = 0; i < NUM_PLAYERS; i++)
  {
    send_message(i, START);
  }
  time_t t;
  srand((unsigned) time(&t));
  // gamedice.first = rand() % 7 + 1;
  // gamedice.second = rand() % 7 + 1;
  gamedice.first = 2;
  gamedice.second = 1;
  printf("Dice1: %d, Dice2: %d\n", gamedice.first, gamedice.second);
  printf("Waiting for moves from players....\n");
  return 1;
}

void check_victory()
{
  printf("Checking victory. numjoined: %d\n", num_joined);
  if(num_joined == 1)
  {
    printf("Victory detected. Finding victor....\n");
    for(int i = 0; i < NUM_PLAYERS; i++)
    {
      printf("Active player: %d\n", players.id[i]);
      if(players.id[i] != 0)
      {
        printf("Sending VICT to user: %d\n", players.id[i]);
        send_message(i, VICT);
        kill_user(i);
      }
    }
  }
}

void tally_results()
{
  printf("tally_results()....\n");
  for(int i = 0; i < NUM_PLAYERS; i++)
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
}

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

  for(int i = 0; i < NUM_PLAYERS; i++)
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
  }
}
