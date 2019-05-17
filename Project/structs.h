static int const BUFFER_SIZE = 1024;
static int const PORT = 4444;
static int const NUM_PLAYERS = 4;
static int const NUM_LIVES = 3;
enum {MAX_CLIENTS = 10};

typedef enum {INIT, MOV} client_messages;
typedef enum {EVEN, ODD, DOUB, CON, NONE} client_moves ;
typedef enum {WELCOME, START, PASS, FAIL, ELIM, VICT, REJECT, CANCEL} server_messages;

typedef struct clients
{
  int fd[MAX_CLIENTS];
  int id[MAX_CLIENTS];
  int lives[MAX_CLIENTS];
  int level[MAX_CLIENTS];
  client_moves move[MAX_CLIENTS];
  int move_var[MAX_CLIENTS];
  time_t move_time[MAX_CLIENTS];
  int timed_out[MAX_CLIENTS];
  int in_lobby[MAX_CLIENTS];
  int pass[MAX_CLIENTS];
} clients;

typedef struct dice
{
  int first;
  int second;
} dice;

clients players;
dice gamedice;
fd_set readfds;
int num_joined, players_ready, max_sd, activity, num_elim, to_lobby, num_clients;
