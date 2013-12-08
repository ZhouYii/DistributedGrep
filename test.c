#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "LL.h"

void testLL();

int main(void) {
  testLL();
  return 0;
}

void testLL() {
  printf("Testing LL structure\n");
  list_node_t *head = NULL;
  list_insert(&head, "127.0.0.1", 500);
  list_insert(&head, "127.0.0.2", 400);

  list_node_t *one = list_addr_seek(head, "127.0.0.1");
  list_node_t *two = list_port_seek(head, 400);
  assert(one != NULL);
  assert(two != NULL);
  assert(one->port == 500);
  assert(0 == strcmp(two->addr, "127.0.0.2"));
}
