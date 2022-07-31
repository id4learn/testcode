#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>


/* struct */
#define STATUS_SUCCESS  0x01
#define STATUS_FAILURE  0x0

#define UPPER_RANDOM_ID 0x20
#define LOWER_RANDOM_ID 0x05


typedef struct dnode_s{
  uint32_t  id;
  struct dnode_s  *prev;
  struct dnode_s  *next;
}dnode_t;

typedef struct dlist_s{
  dnode_t   *head; // head of singly link list
  dnode_t   *tail;
  uint32_t  total;
}dlist_t;

typedef struct task_inst_s{
  dlist_t   *dlist;
}task_inst_t;


/* function declaration */

uint32_t dlist_add_node_at_head( dlist_t *dlist);
uint32_t dlist_add_n_node_at_head(dlist_t *dlist, uint32_t n);
uint32_t dlist_add_node_at_tail(dlist_t *dlist);
uint32_t dlist_add_n_node_at_tail(dlist_t *dlist, uint32_t n);
uint32_t dlist_delete_head_node(dlist_t *dlist);
uint32_t dlist_delete_tail_node(dlist_t *dlist);
uint32_t dlist_display_all(dlist_t *dlist);
uint32_t dlist_find_and_delete_node(dlist_t *dlist, uint32_t id);
uint32_t dlist_delete_all_node(dlist_t *dlist);
uint32_t dlist_reverse(dlist_t *dlist);
dnode_t* allocate_dlist_node();

dlist_t* create_dlist();
task_inst_t* create_task_inst();
uint32_t  dlist_test_api(task_inst_t *inst);


uint32_t main()
{
  task_inst_t   *inst = NULL;
  dlist_t       *dlist = NULL;
  uint32_t  status = STATUS_FAILURE;

  inst = create_task_inst();
  if(inst == NULL) {
    // memory alloc failed
    return STATUS_FAILURE;
  }
  status =  dlist_test_api(inst);

  return status;
}

uint32_t  dlist_test_api(task_inst_t *inst)
{
  uint32_t  choice = 0;
  uint32_t  delete_id = 0;
  dlist_t *dlist = create_dlist();
  if(dlist == NULL) {
    // memory alloc failed
    return STATUS_FAILURE;
  }

  inst->dlist = dlist;

  while(1) {
    printf("\n 1 Add entry at head");
    printf("\n 2 Add entry at tail");
    printf("\n 3  Delete Head node");
    printf("\n 4  Delete tail node");
    printf("\n 5  find and delete a node ");
    printf("\n 6  Display content of list ");
    printf("\n 7 Add 10 nodes at head");
    printf("\n 8 Add 10 nodes at tail");
    printf("\n 9  Delete complete list");
    printf("\n 10  Delete all nodes & exit ");
    printf("\n 11 Reverse list elements ");
    
    printf("\n Enter your choice now:");
    scanf("%u",&choice);

    switch(choice){
      case 1:
        dlist_add_node_at_head(dlist);
        break;
      case 2:
        dlist_add_node_at_tail(dlist);
        break;
      case 3:
        dlist_delete_head_node(dlist);
        break;
      case 4:
        dlist_delete_tail_node(dlist);
        break;
      case 5:
        printf("\n Enter ID of the node to be deleted:");
        scanf("%u",&delete_id);
        dlist_find_and_delete_node(dlist , delete_id);
        break;
      case 6:
        dlist_display_all(dlist);
        break;
      case 7:
        dlist_add_n_node_at_head(dlist,  10);
        break;
      case 8:
        dlist_add_n_node_at_tail(dlist,  10);
        break;
      case 9:
        dlist_delete_all_node(dlist);
        break;
      case 10: // delete n exit
        dlist_delete_all_node(dlist);
        return STATUS_SUCCESS;
      case 11:
        dlist_reverse(dlist);
        break;
    } /* switch */
  }/* while */

  return STATUS_SUCCESS;
}

uint32_t dlist_display_all(dlist_t *dlist)
{
  dnode_t *tmp = NULL;

  if(dlist == NULL || dlist->head == NULL || dlist->tail == NULL) {
    printf("\n List is empty , nothing for display");
  }

  tmp = dlist->head;
  printf("\n Content of list:");
  while(tmp != NULL) {
    printf("\t [%u], ", tmp->id);
    tmp = tmp->next;
  }
  printf("\n <---- END of list content ------");

  return STATUS_SUCCESS;
}

uint32_t dlist_delete_head_node(dlist_t *dlist)
{
  dnode_t *tmp = NULL;

  /* list is empty */
  if(dlist == NULL || dlist->head == NULL) {
    printf("\n Nothing to be deleted as dlist is empty");
    return STATUS_SUCCESS;
  }

  /* only single node in the list */
  if(dlist->head == dlist->tail) {
    printf("\n Last node to be deleted");
    tmp = dlist->head;
    dlist->head = NULL;
    dlist->tail = NULL;
  }
  else {
    tmp = dlist->head;

    dlist->head = dlist->head->next;
    dlist->head->prev = NULL;
  }


  printf("\\n Deleted Node with id =%u",tmp->id);
  free(tmp);

}


uint32_t dlist_delete_tail_node(dlist_t *dlist)
{
  dnode_t *tmp = NULL;

  /* list is empty */
  if(dlist == NULL || dlist->head == NULL) {
    printf("\n Nothing to be deleted as dlist is empty");
    return STATUS_SUCCESS;
  }

  /* only single node in the list */
  if(dlist->head == dlist->tail) {
    printf("\n Last node to be deleted");
    tmp = dlist->head;
    dlist->head = NULL;
    dlist->tail = NULL;
  }
  else {
    tmp = dlist->tail;

    dlist->tail = dlist->tail->prev;
    dlist->tail->next = NULL;
  }
  printf("\\n Deleted Node with id =%u",tmp->id);
  free(tmp);
  
  return STATUS_SUCCESS;
}


uint32_t dlist_add_node_at_tail(dlist_t *dlist)
{
  dnode_t *node = NULL;

  if(dlist == NULL) {
    return STATUS_FAILURE;
  }

  node = allocate_dlist_node();

  if(node == NULL) {
    return STATUS_FAILURE;
  }

  if( dlist->tail == NULL) {
    dlist->head = node;
    dlist->tail = node;
    dlist->head->prev = NULL;
    dlist->tail->next = NULL;
  }
  else {
    node->next = NULL;
    node->prev = dlist->tail;
    dlist->tail->next = node;
    dlist->tail = node;
  }

  dlist->total++;

  return STATUS_SUCCESS ;

}

uint32_t dlist_add_n_node_at_head(dlist_t *dlist, uint32_t n)
{
  uint32_t status = STATUS_FAILURE;

  for( uint32_t i = 0 ; i < n ; i++) {
    status = dlist_add_node_at_head(dlist);
    if(status == STATUS_FAILURE) {
      return STATUS_FAILURE;
    }
  }
  return STATUS_SUCCESS;
}


uint32_t dlist_add_n_node_at_tail(dlist_t *dlist, uint32_t n)
{
  uint32_t status = STATUS_FAILURE;

  for( uint32_t i = 0 ; i < n ; i++) {
    status = dlist_add_node_at_tail(dlist);
    if(status == STATUS_FAILURE) {
      return STATUS_FAILURE;
    }
  }
  return STATUS_SUCCESS;
}


uint32_t dlist_add_node_at_head(dlist_t *dlist)
{
  dnode_t *node = NULL;

  if(dlist == NULL) {
    return STATUS_FAILURE;
  }

  node = allocate_dlist_node();

  if(node == NULL) {
    return STATUS_FAILURE;
  }

  if( dlist->head == NULL) {
    dlist->head = node;
    dlist->tail = node;
    dlist->head->prev = NULL;
    dlist->tail->next = NULL;
  }
  else {
    node->next = dlist->head;
    dlist->head->prev = node;
    node->prev = NULL;
    dlist->head = node;
  }


  dlist->total++;

  return STATUS_SUCCESS ;

}

dnode_t* allocate_dlist_node()
{
  dnode_t *node = NULL;

  node = (dnode_t *) malloc(sizeof(dnode_t));
  if(node != NULL) {
    memset((void *)node, 0, sizeof(dnode_t));
  }

  /* Generate random number beween UPPER_RANDOM_ID & LOWER_RANDOM_ID */
  node->id = (rand()% (UPPER_RANDOM_ID - LOWER_RANDOM_ID +1)) + LOWER_RANDOM_ID;
  printf("\n Random number generated = %u", node->id);


#if 0
  printf("\n Enter id of node to be added::");
  scanf("%u",&(node->id));
#endif

  return node;
}

dlist_t* create_dlist()
{
  dlist_t *dlist = NULL;

  dlist = (dlist_t *) malloc(sizeof(dlist_t));
  if(dlist != NULL) {
    memset((void *)dlist, 0, sizeof(dlist_t));
  }

  return dlist;
}

task_inst_t* create_task_inst()
{
  task_inst_t *inst = NULL;

  inst = (task_inst_t *) malloc(sizeof(task_inst_t));
  if(inst != NULL) {
    memset((void *)inst, 0, sizeof(task_inst_t));
  }

  return inst;
}

uint32_t dlist_find_and_delete_node(dlist_t *dlist, uint32_t id)
{
  uint32_t status = STATUS_FAILURE;
  dnode_t *tmp = NULL;
  dnode_t *prev = NULL;

  if(dlist == NULL || dlist->head == NULL || dlist->tail == NULL) {
    printf("\n List is empty , nothing for display");
    return status;
  }

  tmp = dlist->head;

  if(dlist->head->id == id) {
    dlist_delete_head_node(dlist);
    return STATUS_SUCCESS;
  }

  if(dlist->tail->id == id) {
    dlist_delete_tail_node(dlist);
    return STATUS_SUCCESS;
  }


  /* delete middle node */
  while(tmp != NULL) {
    if(tmp->id == id) {

      tmp->prev->next = tmp->next;
      tmp->next->prev = tmp->prev;

      printf("\\n Deleted Node with id =%u",tmp->id);
      free(tmp);

      break;
    }
    else {
      tmp = tmp->next;
    }
  }

  return STATUS_FAILURE;
}


uint32_t dlist_delete_all_node(dlist_t *dlist)
{
  uint32_t status = STATUS_FAILURE;
  dnode_t *tmp = NULL;

  if(dlist == NULL || dlist->head == NULL || dlist->tail == NULL) {
    printf("\n List is empty , nothing to delete");
  }

  tmp = dlist->head;
  while(tmp != NULL) {
  
    dlist->head = dlist->head->next;
    free(tmp);
    tmp = dlist->head;
  }

  dlist->head = NULL;
  dlist->tail = NULL;

  return STATUS_FAILURE;
}

uint32_t dlist_reverse(dlist_t *dlist)
{
  uint32_t status = STATUS_FAILURE;
  dnode_t *prev = NULL;
  dnode_t *curr = NULL;
  dnode_t *next = NULL;

  if(dlist == NULL || dlist->head == NULL || dlist->tail == NULL) {
    printf("\n List is empty , nothing to reverse");
  }

  if(dlist-> head == dlist->tail) {
    printf("\n noting to be done for reversal as only single element exist");
    return STATUS_SUCCESS;
  }
  dlist->tail = dlist->head;

  curr = dlist->head;

  while(curr != NULL) {

    next = curr->next;
    curr->next = prev;
    prev = curr;

    curr = next;
  }
  dlist->head = prev;

  printf("\n Reversed list content ......");
  dlist_display_all(dlist);

  return STATUS_SUCCESS;

}


