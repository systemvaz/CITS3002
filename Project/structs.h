static int const BUFFER_SIZE = 1024;
static int const PORT = 4444;
static int const NUM_PLAYERS = 1;
static int const MAX_CLIENTS = 5;
static int const NUM_LIVES = 3;

typedef enum {INIT, MOV} client_messages;
typedef enum {EVEN, ODD, DOUB, CON, NONE} client_moves ;
typedef enum {WELCOME, START, PASS, FAIL, ELIM, VICT, REJECT, CANCEL} server_messages;

typedef struct clients
{
  int fd[5];
  int id[5];
  int lives[5];
  int level[5];
  client_moves move[5];
  int move_var[5];
  int pass[5];
} clients;

typedef struct dice
{
  int first;
  int second;
} dice;

clients players;
dice gamedice;
fd_set readfds;
int num_joined, players_ready, max_sd, activity;
