#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>


/* struct */
#define STATUS_SUCCESS  0x01
#define STATUS_FAILURE  0x0

#define UPPER_RANDOM_ID 0x20
#define LOWER_RANDOM_ID 0x05

#define MAX_ELE_STACK   0x96

typedef struct snode_s{
  uint32_t  id;
  struct snode_s  *next;
}snode_t;

typedef struct stack_s{
  uint32_t   store[MAX_ELE_STACK];
  int16_t    top;
}stack_t;

typedef struct slist_s{
  snode_t   *head; // head of singly link list
  snode_t   *tail;
  uint32_t  total;
}slist_t;



typedef struct task_inst_s{
  slist_t   *slist;
  stack_t   *stack;
}task_inst_t;


/* function declaration */


uint32_t slist_add_node_at_head( slist_t *slist);
uint32_t slist_add_n_node_at_head(slist_t *slist, uint32_t n);
uint32_t slist_add_node_at_tail(slist_t *slist);
uint32_t slist_add_n_node_at_tail(slist_t *slist, uint32_t n);
uint32_t slist_delete_head_node(slist_t *slist);
uint32_t slist_delete_tail_node(slist_t *slist);
uint32_t slist_display_all(slist_t *slist);
uint32_t slist_find_and_delete_node(slist_t *slist, uint32_t id);
uint32_t slist_delete_all_node(slist_t *slist);
uint32_t slist_reverse(slist_t *slist);
snode_t* allocate_slist_node();

slist_t* create_slist();
task_inst_t* create_task_inst();
uint32_t  slist_test_api(task_inst_t *inst);


/* stack */
uint32_t insert_stack_entry(stack_t *stack);
uint32_t pop_stack_entry(stack_t *stack , uint32_t *poped_val);
uint32_t  display_stack(stack_t *stack);
uint32_t insert_n_entry_into_stack(stack_t *stack, uint32_t n);
uint32_t  delete_stack_inst_n_exit(task_inst_t *inst);
stack_t*  create_stack_inst(task_inst_t *inst);
uint32_t  test_stack_api(task_inst_t *inst);

uint32_t main()
{
  task_inst_t   *inst = NULL;
  slist_t       *slist = NULL;
  uint32_t  status = STATUS_FAILURE;

  inst = create_task_inst();
  if(inst == NULL) {
    // memory alloc failed
    return STATUS_FAILURE;
  }

  status = test_stack_api(inst);

  status =  slist_test_api(inst);

  return status;
}

uint32_t  slist_test_api(task_inst_t *inst)
{
  uint32_t  choice = 0;
  uint32_t  delete_id = 0;
  slist_t *slist = create_slist();
  if(slist == NULL) {
    // memory alloc failed
    return STATUS_FAILURE;
  }

  inst->slist = slist;

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
        slist_add_node_at_head(slist);
        break;
      case 2:
        slist_add_node_at_tail(slist);
        break;
      case 3:
        slist_delete_head_node(slist);
        break;
      case 4:
        slist_delete_tail_node(slist);
        break;
      case 5:
        printf("\n Enter ID of the node to be deleted:");
        scanf("%u",&delete_id);
        slist_find_and_delete_node(slist , delete_id);
        break;
      case 6:
        slist_display_all(slist);
        break;
      case 7:
        slist_add_n_node_at_head(slist,  10);
        break;
      case 8:
        slist_add_n_node_at_tail(slist,  10);
        break;
      case 9:
        slist_delete_all_node(slist);
        break;
      case 10: // delete n exit
        slist_delete_all_node(slist);
        return STATUS_SUCCESS;
      case 11:
        slist_reverse(slist);
        break;
    } /* switch */
  }/* while */

  return STATUS_SUCCESS;
}

uint32_t slist_display_all(slist_t *slist)
{
  snode_t *tmp = NULL;

  if(slist == NULL || slist->head == NULL || slist->tail == NULL) {
    printf("\n List is empty , nothing for display");
  }

  tmp = slist->head;
  printf("\n Content of list:");
  while(tmp != NULL) {
    printf("\t [%u], ", tmp->id);
    tmp = tmp->next;
  }
  printf("\n <---- END of list content ------");

  return STATUS_SUCCESS;
}

uint32_t slist_delete_head_node(slist_t *slist)
{
  snode_t *tmp = NULL;

  /* list is empty */
  if(slist == NULL || slist->head == NULL) {
    printf("\n Nothing to be deleted as slist is empty");
    return STATUS_SUCCESS;
  }

  /* only single node in the list */
  if(slist->head == slist->tail) {
    printf("\n Last node to be deleted");
    tmp = slist->head;
    slist->head = NULL;
    slist->tail = NULL;
  }
  else {
    tmp = slist->head;
    slist->head = slist->head->next;
  }


  printf("\\n Deleted Node with id =%u",tmp->id);

  free(tmp);

}


uint32_t slist_delete_tail_node(slist_t *slist)
{
  snode_t *tmp = NULL;

  /* list is empty */
  if(slist == NULL || slist->head == NULL) {
    printf("\n Nothing to be deleted as slist is empty");
    return STATUS_SUCCESS;
  }

  /* only single node in the list */
  if(slist->head == slist->tail) {
    printf("\n Last node to be deleted");
    tmp = slist->head;
    slist->head = NULL;
    slist->tail = NULL;
  }
  else {
    tmp = slist->head;
    while(tmp != NULL) {

      if(tmp->next != slist->tail) {
        tmp = tmp->next;
        printf("\\n Deleted Node with id =%u",tmp->id);
        free(tmp);
      }
      else {
        printf("\n Deleted Node with id =%u",slist->tail->id);
        free(slist->tail);
        slist->tail = tmp;
        slist->tail->next = NULL;
        break;
      }
    }/* while */
  }
  
  return STATUS_SUCCESS;
}


uint32_t slist_add_node_at_tail(slist_t *slist)
{
  snode_t *node = NULL;

  if(slist == NULL) {
    return STATUS_FAILURE;
  }

  node = allocate_slist_node();

  if(node == NULL) {
    return STATUS_FAILURE;
  }
    node->next = NULL;

  if(slist->head == NULL) {
    slist->head = node;
    slist->tail = node;
  }
  else {
    slist->tail->next = node;
    slist->tail = node;
  }

  slist->total++;

  return STATUS_SUCCESS ;

}

uint32_t slist_add_n_node_at_head(slist_t *slist, uint32_t n)
{
  uint32_t status = STATUS_FAILURE;

  for( uint32_t i = 0 ; i < n ; i++) {
    status = slist_add_node_at_head(slist);
    if(status == STATUS_FAILURE) {
      return STATUS_FAILURE;
    }
  }
  return STATUS_SUCCESS;
}


uint32_t slist_add_n_node_at_tail(slist_t *slist, uint32_t n)
{
  uint32_t status = STATUS_FAILURE;

  for( uint32_t i = 0 ; i < n ; i++) {
    status = slist_add_node_at_tail(slist);
    if(status == STATUS_FAILURE) {
      return STATUS_FAILURE;
    }
  }
  return STATUS_SUCCESS;
}


uint32_t slist_add_node_at_head(slist_t *slist)
{
  snode_t *node = NULL;

  if(slist == NULL) {
    return STATUS_FAILURE;
  }

  node = allocate_slist_node();

  if(node == NULL) {
    return STATUS_FAILURE;
  }

  if(slist->head == NULL) {
    node->next = NULL;
    slist->head = node;
    slist->tail = node;
  }
  else {
    node->next = slist->head;
    slist->head = node;
  }

  slist->total++;

  return STATUS_SUCCESS ;

}

snode_t* allocate_slist_node()
{
  snode_t *node = NULL;

  node = (snode_t *) malloc(sizeof(snode_t));
  if(node != NULL) {
    memset((void *)node, 0, sizeof(snode_t));
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

slist_t* create_slist()
{
  slist_t *slist = NULL;

  slist = (slist_t *) malloc(sizeof(slist_t));
  if(slist != NULL) {
    memset((void *)slist, 0, sizeof(slist_t));
  }

  return slist;
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

uint32_t slist_find_and_delete_node(slist_t *slist, uint32_t id)
{
  uint32_t status = STATUS_FAILURE;
  snode_t *tmp = NULL;
  snode_t *prev = NULL;

  if(slist == NULL || slist->head == NULL || slist->tail == NULL) {
    printf("\n List is empty , nothing for display");
  }

  tmp = slist->head;

  if(slist->head->id == id) {
    slist_delete_head_node(slist);
    return STATUS_SUCCESS;
  }

  if(slist->tail->id == id) {
    slist_delete_tail_node(slist);
    return STATUS_SUCCESS;
  }


  /* delete middle node */
  while(tmp != NULL) {
    prev = tmp;
    tmp = tmp->next;
    if(tmp != NULL) {
      if (tmp->id != id) {
        continue;
      } 
      else { //found
        printf("\n Node to be delete found");
        
        prev->next = prev->next->next;
        free(tmp);
        break;
      }/* found */
    }/*  tmp Not NULL */
  }

  return STATUS_FAILURE;
}


uint32_t slist_delete_all_node(slist_t *slist)
{
  uint32_t status = STATUS_FAILURE;
  snode_t *tmp = NULL;

  if(slist == NULL || slist->head == NULL || slist->tail == NULL) {
    printf("\n List is empty , nothing to delete");
  }

  tmp = slist->head;
  while(tmp != NULL) {
  
    slist->head = slist->head->next;
    free(tmp);
    tmp = slist->head;
  }

  slist->head = NULL;
  slist->tail = NULL;

  return STATUS_FAILURE;
}

uint32_t slist_reverse(slist_t *slist)
{
  uint32_t status = STATUS_FAILURE;
  snode_t *prev = NULL;
  snode_t *curr = NULL;
  snode_t *next = NULL;

  if(slist == NULL || slist->head == NULL || slist->tail == NULL) {
    printf("\n List is empty , nothing to reverse");
  }

  if(slist-> head == slist->tail) {
    printf("\n noting to be done for reversal as only single element exist");
    return STATUS_SUCCESS;
  }
  slist->tail = slist->head;

  curr = slist->head;

  while(curr != NULL) {

    next = curr->next;
    curr->next = prev;
    prev = curr;

    curr = next;
  }
  slist->head = prev;

  printf("\n Reversed list content ......");
  slist_display_all(slist);

  return STATUS_SUCCESS;

}


stack_t*  create_stack_inst(task_inst_t *inst)
{

  stack_t   *stack  = NULL;

  if(inst == NULL) 
    return STATUS_FAILURE;

  stack = (stack_t *) malloc(sizeof(stack_t));

  if(stack == NULL)  {
    printf("\n Malloc failed for stack creation ");
    return NULL;
  }

  memset((void *)stack, 0 , sizeof(stack_t));
  stack->top = -1;

  return stack;
}

uint32_t  delete_stack_inst_n_exit(task_inst_t *inst)
{
  stack_t *stack = NULL;

  if(inst == NULL) 
    return STATUS_SUCCESS;


  if(inst->stack != NULL) {
    free (inst->stack);
    inst->stack = NULL;
    printf("\n Stack instance freed");
  }

  return STATUS_SUCCESS;

}


uint32_t insert_n_entry_into_stack(stack_t *stack, uint32_t n)
{
  uint32_t status = STATUS_FAILURE;

  for( uint32_t i = 0 ; i < n ; i++) {
    status = insert_stack_entry(stack);
    if(status == STATUS_FAILURE) {
      return STATUS_FAILURE;
    }
  }
  return STATUS_SUCCESS;
}


uint32_t insert_stack_entry(stack_t *stack)
{
  uint32_t  value = 0;

  if(stack == NULL) {
    printf("\n Stack ptr is NULL, Please allocate ");
    return STATUS_FAILURE;
  }

  if( stack->top >= MAX_ELE_STACK) {
    printf("\n Stack is full");
    return STATUS_FAILURE;
  }

  value = (rand()% (UPPER_RANDOM_ID - LOWER_RANDOM_ID +1)) + LOWER_RANDOM_ID;
  printf("\n Random number generated = %u", value);

  stack->store[++(stack->top)] = value;

  return STATUS_SUCCESS;
}

uint32_t pop_stack_entry(stack_t *stack , uint32_t *poped_val)
{

  if(stack == NULL || stack->top < 0  ) {
    printf("\n Stack is empty");
    return STATUS_FAILURE;
  }
  else {
    *poped_val = stack->store[stack->top--];

    printf("\n Popped value [%u] from stack", *poped_val);
  }

  return STATUS_SUCCESS;

}

uint32_t  display_stack(stack_t *stack)
{
  int16_t i = 0;
  if(stack == NULL || stack->top < 0) {
    return STATUS_FAILURE;
  }

  printf("\n [ Stack Entries: ");
  for (i = stack->top ; i >=0 ;  i--) {
    printf("\t [%u]",stack->store[i]); 
  }
  printf("\t ]");

  return STATUS_SUCCESS;

}

uint32_t  test_stack_api(task_inst_t *inst)
{
  uint32_t  choice = 0;
  uint32_t  delete_id = 0;
  uint32_t value = 0;
  uint32_t  status = STATUS_FAILURE;
  stack_t *stack = create_stack_inst(inst);
  if(stack == NULL) {
    // memory alloc failed
    return STATUS_FAILURE;
  }

  inst->stack = stack;

  while(1) {
    printf("\n 1 Add entry into stack");
    printf("\n 2 pop entry from stack");
    printf("\n 3 Display content of stack ");
    printf("\n 4 Add 10 nodes stack");
    printf("\n 5 pop 10 nodes from stack");
    printf("\n 6 Delete stack ");
    
    printf("\n Enter your choice now:");
    scanf("%u",&choice);

    switch(choice){
      case 1:
        insert_stack_entry(stack);
        break;
      case 2:
        status = pop_stack_entry(stack, &value);
        if(status == STATUS_SUCCESS) {
          printf("\n Popped value:[%u]",value);
        }
        break;
      case 3:
        display_stack(stack);
        break;
      case 4:
        insert_n_entry_into_stack(stack, 10);
        break;
        /*
      case 5:
        pop_n_entry(stack);
        break; */
      case 6:
        delete_stack_inst_n_exit(inst);
        return STATUS_SUCCESS;
    } /* switch */
  }/* while */

  return STATUS_SUCCESS;
}

