#include "main.h"
data_t _data;
pthread_mutex_t print_mutex;
volatile int total_jobs = 1;

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
  int port = PORT;
  system("mkdir tmp log > /dev/null");
  if(pthread_mutex_init(&print_mutex, NULL)) {
    printf("Can't initialize mutex\n");
    exit(1);
  }
  _data.cluster_list = NULL;
  /* Populate some cluster data */
  if(!list_insert(&_data.cluster_list, "192.168.139.135", port)) {
    printf("Failed to init\n");
    exit(1);
  }
  if(!list_insert(&_data.cluster_list, "192.168.139.134", port)) {
    printf("Failed to init\n");
    exit(1);
  }
  /* Launch server thread */
  if(0 != pthread_create(&_data.server_thread, NULL, server_listener, &port)) {
    printf("failed allocating a new pthread for the listener\n");
    exit(1);
  }
}

void start_repl() {
  while(1) {
    char input_buf[1024];
    pthread_mutex_lock(&print_mutex);
    printf("Start: ");
    fflush(NULL);
    pthread_mutex_unlock(&print_mutex);
    fgets(input_buf, 1024, stdin);
    if(0 == strncmp("grep", input_buf, 4)) {
      send_grep(input_buf);
    }
  }
}

void* remote_exec(void *dat) {
  cmd_info_t* info = (cmd_info_t*) dat;
  int sock_fd = conn_socket(info->addr, info->port);
  if(sock_fd == -1) {
    printf("Failed to connect with remote host %s:%d\n", info->addr, info->port);
    return NULL;
  }
  
  char buf[BUF_SIZE];
  sprintf(buf, "%d", GREP);
  send(sock_fd, buf, strlen(buf), NO_FLAGS);
  if(!wait_ack(sock_fd, GREP_ACK)) 
    exit(1);

  send(sock_fd, info->args, strlen(info->args), NO_FLAGS);
  pthread_mutex_lock(&print_mutex);
  printf("From %s:%d \n~~~~~~~~~~~~~~\n", info->addr, info->port);
  while(recv(sock_fd, buf, BUF_SIZE, NO_FLAGS) > 0) 
    printf("%s\n", buf);
  
  pthread_mutex_unlock(&print_mutex);
  free(info);
  fflush(NULL);
  total_jobs--;
  return NULL;
}

void send_grep(const char* args) {
  total_jobs = 2;
  if(args == NULL) return;
  /* TODO : connect socket ignores address argument */
  list_node_t *head = _data.cluster_list;
  while(head != NULL) {
    /* Free this in the thread */
    cmd_info_t *cmd = malloc(sizeof(cmd_info_t));
    if(cmd == NULL) {
      head = head->next;
      continue;
    }
    cmd->addr = strdup(head->addr);
    cmd->port = head->port;
    cmd->cmd = GREP;
    cmd->args = strdup(args+5);
    
    /* Spawn pthread to do the job */
    pthread_t thread;
    if(0 != pthread_create(&thread, NULL, remote_exec, cmd)) {
      printf("Failed to launch a grep job\n");
      exit(1);
    }
    head = head->next;
  }
  while(total_jobs > 0);
  return;
}


void* server_listener(void* listen_port) {
  char buf[BUF_SIZE];
  int socket_fd = 0, conn_fd =0;
  struct sockaddr_in server_addr;
  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if(socket_fd == -1) 
    err_handler("Failed to get socket file descriptor");

  memset(&server_addr, '0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons((uint16_t)PORT);
  pthread_mutex_lock(&print_mutex);
  printf("Listener started on port %d\n", PORT);
  pthread_mutex_unlock(&print_mutex);

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
    if(recv(conn_fd, buf, BUF_SIZE, NO_FLAGS) > 0) {
      if(atoi(buf) == GREP) {
        printf("GREP\n");
        char sendbuf[BUF_SIZE];
        /* Ack for Grep */
        sprintf(sendbuf, "%d", GREP_ACK);
        send(conn_fd, sendbuf, strlen(sendbuf), NO_FLAGS);
        /* Recieve args, execute */
        recv(conn_fd, buf, BUF_SIZE, NO_FLAGS);
        strip_newline(buf);
        sprintf(sendbuf, "grep %s log/*", buf);
        /* Send results to client */
        FILE* f = popen(sendbuf, "r");
        if(f > 0) {
          memset(buf, '\0', sizeof(buf));
          while(fgets(buf, sizeof(buf), f) != NULL) {
            send(conn_fd, buf, strlen(buf), NO_FLAGS);
            memset(buf, '\0', sizeof(buf));
          }
        }
      }
    }
    fflush(NULL);
    close(conn_fd);
  }

  return NULL;
}

