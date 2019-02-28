#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include "list.h"


char getprompt(char* command){ //prompts the user for input

  printf("prompt> ");
  fflush(stdout);
  memset(command,0,1024);
  if (fgets(command,1024,stdin)==NULL){
    return 1;}
  return 0;
}

int isstatus(char *cmd){ //checks if the user entered "status"
  if (strcmp(cmd, "status") == 0){
    return 1;
  }
  return 0;
}

int isexit(char *cmd){  //checks if the user entered "exit"
  if (strcmp(cmd, "exit") == 0){
    return 1;
  }
  return 0;
}

int iskill(char *cmd){  //checks if the user entered "kill"
  if (strcmp(cmd, "kill") == 0){
    return 1;
  }
  return 0;
}

char** tokenify(const char *s, const char *sep) {
    char *word = NULL;

    // make a copy of the input string, because strtok
    // likes to mangle strings.
    char *s1 = strdup(s);

    // find out exactly how many tokens we have
    int words = 0;
    for (word = strtok(s1, sep);
         word;
         word = strtok(NULL, sep)) words++ ;
    free(s1);

    s1 = strdup(s);

    // allocate the array of char *'s, with one additional
    char **array = (char **)malloc(sizeof(char *)*(words+1));
    int i = 0;
    for (word = strtok(s1, sep);
         word;
         word = strtok(NULL, sep)) {
        array[i] = strdup(word);
        i++;
    }
    array[i] = NULL;
    free(s1);
    return array;
}

void free_tokens(char **tokens) {
    int i = 0;
    while (tokens[i] != NULL) {
        free(tokens[i]); // free each string
        i++;
    }
    free(tokens); // then free the array
}

void run_fork(char **curr, int check, struct proclist *plist){ //creates the child process and executes the command
    int *status= NULL;
    pid_t rc = fork();
    if (rc < 0){
      free_tokens(curr);
      printf("fork failed\n");
      exit(-1);
    }
    if(rc==0){
      if (execv(curr[0],curr)<0){
        printf("execv failed\n");
        free_tokens(curr);
        exit(-1);
      }
    }
    if (rc > 0){
      if (check==1){
        proclist_add(plist, rc, curr[0]);
        waitpid(rc, status, WNOHANG);
      }
      else{
        waitpid(rc, NULL, 0);}
      free_tokens(curr);
    }
  }

int isbackground(char* command){ //checks if the user entered a '&' to run a process in the background
  for (int index = strlen(command)-1; index >= 0; index--){
    if (command[index]!=' '){
      if (command[index]=='&'){
        command[index]='\0';
        return 1;
      }
      else{
        return 0;
      }
    }
  }
  return 0;
}

int iterateprocesses(char *process, struct proclist *plist){
  int check=isbackground(process); //check whether the command is running in background or not
  char** curr = tokenify(process," ");
    if (isstatus(curr[0])){
      proclist_print(plist);
      free_tokens(curr);
    }
    else if (isexit(curr[0])){  //checks if exit is called and exits loop if so
      free_tokens(curr);
      return 1;

    }
    else if(iskill(curr[0])){
      struct procnode *rem = proclist_find(plist, atoi(curr[1]));
      if (rem != NULL){
        kill(rem->pid, SIGKILL);
        free_tokens(curr);
      }
      else{
        printf("process does not exist\n");
        free_tokens(curr);
      }
    }
    else{
      run_fork(curr, check, plist);
    }
    return 0;
}

void scanlist(struct proclist *plist){
  int *status = NULL;
  struct procnode *temp = plist->head;
  while(temp!=NULL){
    int check = waitpid(temp->pid, status , WNOHANG);
    if (check>0){
      struct procnode *later = temp->next;
      proclist_remove(plist, temp->pid);
      temp=later;
    }
    else if (check==0){
      temp=temp->next;
    }
    else if (check<0){
      printf("waitpid error");
      temp=temp->next;
    }
  }
}


int main(int argc, char **argv) {
    struct proclist *plist = proclist_new(); // you'll need this eventually
    int close = 0;
    while(true) {
      scanlist(plist);
      char command[1024];
      if (getprompt(command)){  break;  } //checks if the commmand is EOF
      else{
        command[strlen(command)-1]='\0';
        char **processes = tokenify(command, ";"); //tokenifies the user input into processes
        for (int i = 0; processes[i]!=NULL; i++){  //iterates through all the processes given
          close = iterateprocesses(processes[i], plist);
          if (close == 1){  break;  }
        }
        free_tokens(processes);
        if (close == 1){  break;  }  //when exit is called it will stop the shell
      }
    }
    proclist_free(plist); // clean up the process list
    return EXIT_SUCCESS;
}
