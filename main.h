#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include "LL.h"
#include "helper.h"
#include "consts.h"
typedef enum {GREP} cmd_t;
typedef enum {GREP_ACK, ACK=100} ack_t;

typedef struct ClusterData {
  list_node_t* cluster_list;
  pthread_t server_thread;
  /* More? */
} data_t;

typedef struct CmdInfo {
  char * addr;
  int port;
  cmd_t cmd;
  char* args;
} cmd_info_t;

void* server_listener(void* port);
void send_grep(const char* args);
void init_structs();
void start_repl();
void* remote_exec(void* dat);
