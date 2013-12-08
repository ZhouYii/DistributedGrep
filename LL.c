#ifndef CONST_C
#define CONST_C
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "consts.h"
#include "LL.h"
/*  Inserts a node into front of list.
 *  Returns the new head or false (null).
 */
list_node_t* list_insert(list_node_t** head, const char* addr, const int port) {
  list_node_t *node = malloc(sizeof(list_node_t));
  if(!node)
    return FALSE;

  node->addr = strdup(addr);
  node->port = port;
  node->next = *(head);
  node->prev = NULL;

  if(*(head) != NULL) {
    (*(head))->prev = node;
  }
  *(head) = node;
  return node;
}

/* Seeks for a node with a certain port value 
 * Returns null if not found */
list_node_t* list_port_seek(list_node_t* head, const int port) {
  if(head == NULL)
    return NULL;
  if(head->port == port)
    return head;
  return list_port_seek(head->next, port);
}

/* Seeks for a node with a certain address value 
 * Returns null if not found*/
list_node_t* list_addr_seek(list_node_t* head, char* addr) {
  if(head == NULL)
    return NULL;
  if(strcmp(addr, head->addr) == 0)
    return head;
  return list_addr_seek(head->next, addr);
}

void list_print(list_node_t* head) {
  if(head == NULL)
    return;
  printf("%s\n%d\n\n", head->addr, head->port);
  list_print(head->next);
}

#endif
