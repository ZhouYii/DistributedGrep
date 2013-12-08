typedef struct LLNode {
  char* addr;
  int port;
  struct LLNode* next;
  struct LLNode* prev;
} list_node_t;

list_node_t* list_insert(list_node_t** head, const char* addr, const int port);
list_node_t* list_port_seek(list_node_t* head, const int port);
list_node_t* list_addr_seek(list_node_t* head, char* addr);
void list_print(list_node_t* head);


