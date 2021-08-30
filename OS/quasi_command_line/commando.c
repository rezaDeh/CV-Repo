#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include "commando.h"

int main(int argc, char*argv[]){
  setvbuf(stdout, NULL, _IONBF, 0); // Turn off output buffering
  int echo = 0;
  char *echo_command = getenv("COMMANDO_ECHO"); // get value for enviroment variable
  if((argc >= 2 && strncmp(argv[1],"--echo", 6) == 0 ) || echo_command != NULL) { // set command echoing on if either case is true

    echo = 1;
  }
  cmdcol_t *cmdcol = malloc(sizeof(cmdcol_t)); // allocate a comdcol_t structure
  *cmdcol->cmd = NULL;
  cmdcol->size = 0;
  while(1){ // infinite loop until no more user input or user enters "exit"
    printf("@> ");
    char input_str[MAX_LINE] = {0};
    fgets(input_str, MAX_LINE, stdin); // get a whole line of input from comand line
    if(input_str[0] == 0){ // check to see if no input entered
      printf("\nEnd of input");
      cmdcol_freeall(cmdcol); // free alocated cmds in cmdcol
      free(cmdcol); // free allocated cmdcol
      break;
    }
    else if (input_str[0] == '\n') { // deal with empty line of input(user just hit enter)
      printf("\n");
      continue; // continue to the end of loop if a enmpty line entered.
    }
    if(echo) printf("%s", input_str); // if echoing on print entered inout
    char *tokens[ARG_MAX];
    int ntok = 0;
    parse_into_tokens(input_str, tokens, &ntok); // parse tokens entered by user
    char token0[NAME_MAX];
    strcpy(token0, tokens[0]);
    if(strncmp(token0, "help", 4) == 0){ // built in command for printing the use of commando commands
      printf("COMMANDO COMMANDS\n");
      printf("help               : show this message\n");
      printf("exit               : exit the program\n");
      printf("list               : list all jobs that have been started giving information on each\n");
      printf("pause nanos secs   : pause for the given number of nanseconds and seconds\n");
      printf("output-for int     : print the output for given job number\n");
      printf("output-all         : print output for all jobs\n");
      printf("wait-for int       : wait until the given job number finishes\n");
      printf("wait-all           : wait for all jobs to finish\n");
      printf("command arg1 ...   : non-built-in is run as a job\n");
    }
    else if(strncmp(token0, "exit", 4) == 0){ // built in commando for exiting the loop/program
      cmdcol_freeall(cmdcol); // free alocated cmds in cmdcol
      free(cmdcol); // free allocated cmdcol
      break;
    }
    else if(strncmp(token0, "list", 4) == 0) cmdcol_print(cmdcol); // built in command to print list of running processes
    else if(strncmp(token0, "pause", 5) == 0){ // built in command to pause the program for enterd seconds and nanosectonds
      long nano_secs = atol(tokens[1]);
      int secs = atoi(tokens[2]);
      pause_for(nano_secs, secs);
    }
    else if(strncmp(token0, "output-for", 10) == 0){ // built in command to print the out put for the entered job number
      pid_t cmd_job_num = atoi(tokens[1]);
      printf("@<<< Output for %s[#%d] (%d bytes):\n",cmdcol->cmd[cmd_job_num]->name,cmdcol->cmd[cmd_job_num]->pid, cmdcol->cmd[cmd_job_num]->output_size);
      printf("----------------------------------------\n"); // print pid as follows #%d to ensure sequential pids: %1 %2 %3...
      cmd_print_output(cmdcol->cmd[cmd_job_num]);
      printf("----------------------------------------\n");
    }
    else if(strncmp(token0, "output-all", 10) == 0){ // built in command to print out put for all jobs
      for(int i = 0; i < cmdcol->size; i++){ // go through all cmds in cmdcol->cmd[]
        printf("@<<< Output for %s[#%d] (%d bytes):\n",cmdcol->cmd[i]->name,cmdcol->cmd[i]->pid, cmdcol->cmd[i]->output_size);
        printf("----------------------------------------\n");
        cmd_print_output(cmdcol->cmd[i]);
        printf("----------------------------------------\n");
      }
    }
    else if(strncmp(token0, "wait-for", 8) == 0){ // built in command to wiat for entered job number
      pid_t cmd_job_num = atoi(tokens[1]);
      cmd_update_state(cmdcol->cmd[cmd_job_num], DOBLOCK);
    }
    else if(strncmp(token0, "wait-all", 8) == 0){ // built in command to wait for all jobs
      for(int i = 0; i < cmdcol->size; i++){ // traverse the array of cmds in cmdcol
        cmd_update_state(cmdcol->cmd[i], DOBLOCK);
      }
    }
    else{ // if non of the built ins match, add a cmd_t struct to the cmdcol->cmds[]
      cmd_t *new_cmd = cmd_new(tokens); // create a new process with arguments goten from command line
      cmdcol_add(cmdcol, new_cmd); // add to the array of cmds
      cmd_start(new_cmd); // new job/process created with the appropriate information

    }
    cmdcol_update_state(cmdcol, NOBLOCK); // update status at the end of each loop
  }
  return 0;
}
