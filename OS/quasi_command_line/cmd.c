#include "commando.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// cmd_t: struct to represent a running command/child process.
// typedef struct {
//   char   name[NAME_MAX+1];         // name of command like "ls" or "gcc"
//   char  *argv[ARG_MAX+1];          // argv for running child, NULL terminated
//   pid_t  pid;                      // PID of child
//   int    out_pipe[2];              // pipe for child output
//   int    finished;                 // 1 if child process finished, 0 otherwise
//   int    status;                   // return value of child, -1 if not finished
//   char   str_status[STATUS_LEN+1]; // describes child status such as RUN or EXIT(..)
//   void  *output;                   // saved output from child, NULL initially
//   int    output_size;              // number of bytes in output
// } cmd_t;

cmd_t *cmd_new(char *argv[]) {

  cmd_t *res = malloc(sizeof(cmd_t));         //Allocate memory for cmd_t

  int i = 0;
  for(i = 0; argv[i] != NULL; i++){           // Obtain elements
    if(i < ARG_MAX){
      res->argv[i] = strdup(argv[i]);         // Make string copies of argv strings
    }else{
      break;
    }
  }

  res->argv[i] = NULL;                       // Ensures res->argv[] ended with NULL
  strncpy(res->name, argv[0], NAME_MAX);     // Sets name field to argv[0]
  res->finished = 0;                         // Not finished
  snprintf(res->str_status, STATUS_LEN, "INIT");    //str_status = "INIT"

  // Initialize remianing fields
  res->pid = -1;
  res->out_pipe[0] = 0;
  res->out_pipe[1] = 0;
  res->status = -1;
  res->output = NULL;
  res->output_size = -1;

  return res;
}

void cmd_free(cmd_t *cmd) {

  for(int i = 0; cmd->argv[i] != NULL; i++){
    free(cmd->argv[i]);       // Deallocates strings in argv[]
  }
  if(cmd->output != NULL){
    free(cmd->output);        // Deallocates output buffer
  }

  free(cmd);                  // Dealloacates cmd
}

void cmd_start(cmd_t *cmd) {

  pipe(cmd->out_pipe);                           // Create a pipe
  snprintf(cmd->str_status, STATUS_LEN, "RUN");  // str_status = "RUN"
  pid_t child = fork();                          // Fork a process

  // CHILD
  if(child == 0){
    dup2(cmd->out_pipe[PWRITE], STDOUT_FILENO);  // Redirect standard output file to pipe
    close(cmd->out_pipe[PREAD]);                 // Read pipe closed
    execvp(cmd->name, cmd->argv);
    exit(-1);
  }else
  // PARENT
  {
    cmd->pid = child;                            // pid set to child PID.
    close(cmd->out_pipe[PWRITE]);                // Write pipe closed
  }
}

void cmd_update_state(cmd_t *cmd, int block) {

  if(cmd->finished == 1){                        // If finished, does nothing
    return;
  }

  int status;
  int wait_pid = waitpid(cmd->pid, &status, block);   // Wait for child process

  if (wait_pid == -1){
    perror("child resulted an error");
  }

  if(cmd->pid == wait_pid && WIFEXITED(status)){                           // Child has exited
    cmd->finished = 1;
    cmd->status = WEXITSTATUS(status);                                     // Exit status
    snprintf(cmd->str_status, STATUS_LEN, "EXIT(%d)", cmd->status);
    cmd_fetch_output(cmd);                                                 // Fill output buffer
    printf("@!!! %s[#%d]: EXIT(%d)\n", cmd->name, cmd->pid, cmd->status);  // Status update message
  }
}

char *read_all(int fd, int *nread) {

  int byte_count = 0;                       // Bytes read
  int size = 1;                             // Initial size
  char *res = malloc(size * sizeof(char));

  if(res == NULL){                          // Cannot allocate memory
    return NULL;
  }

  while(1){
    int read_bytes = read(fd, res + byte_count, size - byte_count);   // Read input form fd
    if(read_bytes <= 0){
      *nread = byte_count;                                            // Bytes read
      break;
    }

    byte_count += read_bytes;
    if(byte_count >= size){                       // Grow buffer (additional space needed)
      size *= 2;
      res = realloc(res, size * sizeof(char));    // Resizing
      if(res == NULL){                            // Cannot allocate memory
        return NULL;
      }
    }
  }
  res[byte_count] = '\0';                         // Return string is null-terminated

  return res;

}


void cmd_fetch_output(cmd_t *cmd) {

  if (cmd->finished == 0){
    printf("%s[#%d] not finished yet", cmd->name, cmd->pid);          // Error message
  }
  cmd->output = read_all(cmd->out_pipe[PREAD], &cmd->output_size);    // Retrieve output from cmd->out_pipe
  close(cmd->out_pipe[PREAD]);                                        // Close pipe
}


void cmd_print_output(cmd_t *cmd) {

  if (cmd->output == NULL){
    printf("%s[#%d] : output not ready\n", cmd->name, cmd->pid);
    return;
  }
  printf("%s", (char *) cmd->output);
}
