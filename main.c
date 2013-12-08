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

typedef struct ClusterData {
  list_node_t* cluster_list;
  pthread_t server_thread;
  /* More? */
} data_t;

data_t _data;

void* server_listener();
int conn_socket(const char* addr, const int port);
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
    if(!list_insert(&_data.cluster_list, "127.0.0.1", BASE_PORT+i)) {
      printf("Failed to init\n");
      exit(1);
    }
  /* Launch server thread */
  if(0 != pthread_create(&_data.server_thread, NULL, server_listener, (int *)5000)) {
    printf("failed allocating a new pthread for the listener");
    exit(1);
  }
}

void start_repl() {
  while(1) {
    int sock_fd;
    char input_buf[1024];
    fgets(input_buf, 1024, stdin);
    printf("%s\n", input_buf);
    sock_fd = conn_socket("127.0.0.1", 5000);
  }
}

int conn_socket(const char* addr, const int port) {
  int sock_fd = 0;
  struct sockaddr_in serv_addr;

  if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("Cannot create socket\n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  if(connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0) {
    printf("Failed to connect socket\n");
    return -1;
  }

  return sock_fd;
}

void* server_listener(void* listen_port) {
  //char buf[BUF_SIZE];
  int socket_fd = 0, conn_fd =0;
  struct sockaddr_in server_addr;
  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if(socket_fd == -1) 
    err_handler("Failed to get socket file descriptor");

  memset(&server_addr, '0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons((uint16_t)listen_port);
  printf("Listener started");

  if(bind(socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
    printf("Failed to bind port");
    return 0;
  }
  if(listen(socket_fd, 10) == -1) {
    printf("Failed to listen on port");
    return 0;
  }
  while(1) {
    conn_fd = accept(socket_fd, (struct sockaddr*) NULL, NULL);
    printf("ACCEPTED");
    close(conn_fd);
    /*while(recv(conn_fd, buf, BUF_SIZE, NO_FLAGS) > 0) 
        close(conn_fd);*/
  }

  return NULL;
}

