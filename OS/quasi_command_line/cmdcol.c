#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "commando.h"

// function to cmd to the array of cmds col->cmd[]
// print out error messege and return 1 if size+1 > MAX_CMDS
void cmdcol_add(cmdcol_t *col, cmd_t *cmd){
  if(col->size + 1 > MAX_CMDS){
    perror("adding will exceed the max # of commands supported");    // report error if size exceeded the max
    exit(1);                                                         // number of commands supported
  }
  col->cmd[col->size] = cmd;                                              // add cmd to array of cmds
  col->size += 1;                                                    // update size
}


// function to print elements in the col structure in a table
void cmdcol_print(cmdcol_t *col){
  printf("JOB  #PID      STAT   STR_STAT OUTB COMMAND\n");
  for(int i = 0; i < col->size; i++){                               // loop to print each cmd per one line
    printf("%-4d #%-8d %4d %10s %4d ", i, col->cmd[i]->pid, col->cmd[i]->status,
      col->cmd[i]->str_status, col->cmd[i]->output_size);          // print diferent fields of cmd structure
    for(int j = 0; j < ARG_MAX; j++){                              // loop to print col->argv[] seperated by a space
      if(col->cmd[i]->argv[j] == NULL) break;                      // break if reached end of argv input
      printf("%s ",col->cmd[i]->argv[j]);
    }
    printf("\n");
  }
}

// function to update each cmd in col by calling cmd_update_state
void cmdcol_update_state(cmdcol_t *col, int block){
  for(int i = 0; i < col->size; i++) cmd_update_state(col->cmd[i], block);
}

// fucntion to free each cmd in col by calling cmd_free()
void cmdcol_freeall(cmdcol_t *col){
  for(int i = 0; i < col->size; i++) cmd_free(col->cmd[i]);
}
