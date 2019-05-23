/*-------------------------------------------------
*  CITS3002 Project 2019.
*  Author: Alexander Varano della Vergiliana
*  Student ID: 22268701
*--------------------------------------------------*/

static int const BUFFER_SIZE = 512;
static int const PORT = 4444;
static int const NUM_LIVES = 3;
//MAX_CLIENTS can be changed to suit. Tested up to 12. Possible for more.
enum {MAX_CLIENTS = 12};

//NUM_PLAYERS Modifiable at command prompt as program argument[1].
int NUM_PLAYERS = 1;

//Possible client moves and server messages
typedef enum {EVEN, ODD, DOUB, CON, NONE} client_moves ;
typedef enum {WELCOME, START, PASS, FAIL, ELIM, VICT, REJECT, CANCEL} server_messages;

/*Structure given to every connection/player. We +1 so we are able to handle the user
* that connects and sends an INIT (and the corresponding REJECT response from server)
* when we already have MAX_CLIENTS connected. */
typedef struct clients
{
  int fd[MAX_CLIENTS+1];
  int id[MAX_CLIENTS+1];
  int lives[MAX_CLIENTS+1];
  int level[MAX_CLIENTS+1];
  client_moves move[MAX_CLIENTS+1];
  int move_var[MAX_CLIENTS+1];
  time_t move_time[MAX_CLIENTS+1];
  int timed_out[MAX_CLIENTS+1];
  int in_lobby[MAX_CLIENTS+1];
  int in_game[MAX_CLIENTS+1];
  int pass[MAX_CLIENTS+1];
  int packets[MAX_CLIENTS+1];
} clients;

//My dice structure. One int for each dice.
typedef struct dice
{
  int first;
  int second;
} dice;

//Game variables
clients players;
dice gamedice;
fd_set readfds;
time_t lobbytime;
int max_sd, activity, num_clients;
int num_joined, players_ready, num_elim, to_lobby, lobby_timer;
