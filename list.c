#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "list.h"

// struct proclist{
//   struct procnode *head;
//   int length;
// };
//
// struct procnode{
//   pid_t pid;
//   char *cmd;
//   struct procnode *next;
// };

/*
 * Return a pointer to a newly-heap-allocated struct proclist.
 * Don't forget to initialize the contents of the struct!
 */
struct proclist* proclist_new() {
  struct proclist *new = malloc(sizeof(struct proclist));
  new->length=0;
  new->head=NULL;
  return new;
}

/*
 * Completely free the memory consumed by a struct proclist,
 * including each node (and each cmd in each node, which should
 * also be heap-allocated), and the struct proclist itself.
 */
void proclist_free(struct proclist *head) {
  while (head->head!= NULL){
    struct procnode *temp = head->head;
    head->head = (head->head)->next;
    free(temp->cmd);
    free(temp);
  }
  free(head);
}

/*
 * Create a new heap-allocated struct procnode, initialize it
 * with the pid and cmd passed as parameters, and add it to the
 * struct proclist.
 *
 * The cmd string *must* be copied to a newly heap-allocated
 * location.  (Hint: use strdup.)
 */
void proclist_add(struct proclist* head, pid_t pid, char *cmd) {
  struct procnode *new = malloc(sizeof(struct procnode));
  head->length = head->length + 1;
  char *p = strdup(cmd);
  new->pid = pid;
  new->cmd = p;
  new->next = NULL;
  if (head->head == NULL){
    head->head = new;
    return;
  }
  struct procnode *temp = head->head;
  while(temp->next!=NULL){
    temp = temp->next;
  }
  temp->next = new;
}

/*
 * Search the struct proclist for a struct procnode with a particular
 * process id (pid).  Return NULL if the pid isn't anywhere on the proclist,
 * or a pointer to the struct procnode for that pid.
 */
struct procnode* proclist_find(struct proclist* head, pid_t pid) {
  struct procnode *temp = head->head;
  while(temp!=NULL){
    if (temp->pid == pid)
      return temp;
    temp = temp->next;
  }
  return NULL;
}

/*
 * Remove the procnode within the proclist that has a particular
 * pid.  If a procnode with the pid doesn't exist on the proclist, do
 * nothing.
 * When removing, don't forget to deallocate any heap memory used
 * by the struct procnode being removed (including the cmd, which should
 * have been heap-allocated).
 */
void proclist_remove(struct proclist *head, pid_t pid) {
  struct procnode * temp = NULL;
  struct procnode * curr;
  curr = head->head;
  if (head->head == NULL){
    return;
  }
  if ((head->head)->pid == pid){
    temp = head->head;
    head->head = (head->head)->next;
    free(temp->cmd);
    free(temp);
    head->length--;
    return;
  }
  while (curr->next!=NULL){
    if ((curr->next)->pid == pid){
      temp = curr->next;
      curr->next = (curr->next)->next;
      free (temp->cmd);
      free (temp);
      break;
    }
    curr =  curr->next;
  }
  head->length--;
}


/*
 * Print some representation of the (active) processes on the proclist.
 * For example, if no processes are running, you can print something like:
 *

 Processes currently active: none

 *
 * If there are any processes active, you output might look like:

 Processes currently active:
        [30]: /bin/sleep
        [29]: /bin/sleep
        [28]: /bin/sleep

 */
void proclist_print(struct proclist *head) {
  if (head->head == NULL){
    printf("Processes currently active: none\n");\
    return;
  }
  struct procnode * curr = head->head;
  printf("Process currently active:\n");
  while(curr!= NULL){
    printf("[%d]: %s\n",(int)curr->pid,curr->cmd);
    curr=curr->next;
  }
}
