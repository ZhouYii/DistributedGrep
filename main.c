#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include "LL.h"
#include "consts.h"

typedef struct ClusterData {
  list_node_t* cluster_list;
  pthread_t server_thread;
  /* More? */
} data_t;

data_t _data;

void* server_listener();
void init_structs();
void start_repl();

/* For now all nodes are homogenous */
int main(int argc, char** argv) {
  init_structs();
  start_repl();
  return 0;
}

/* Some info about initializing structs :
 *  Ideally, the node should initialize itself.
 *  Then it will initialize the other nodes in the network.
 *  This suggests that we know all the nodes in the network anyways. */
void init_structs() {
  _data.cluster_list = NULL;
  /* Populate some cluster data */
  for(int i = 0 ; i < 5 ; i++) 
    list_insert(&_data.cluster_list, "127.0.0.1", BASE_PORT+i);

  /* Launch server thread */
  if(0 != pthread_create(&_data.server_thread, NULL, server_listener, (void *)NULL)) {
    printf("failed allocating a new pthread for the listener");
    exit(1);
  }
}

void start_repl() {
  while(1) {
    char input_buf[1024];
    fgets(input_buf, 1024, stdin);
    printf("%s\n", input_buf);
  }
}

void* server_listener() {
}
