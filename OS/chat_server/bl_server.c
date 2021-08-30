#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "blather.h"

int no_sig = 1; // global variable used for loop control/signals

// Function run when a SIGINT is sent to the program
void handle_SIGINT(int sig_num) {
  no_sig = 0;
}
// Function run when a SIGTERM is sent to the program
void handle_SIGTERM(int sig_num) {
  no_sig = 0;
}


int main(int argc, char *argv[]){
  if(argc < 2){
    printf("usage: <server name>");
    return 0;
  }
  // Set handling functions for programs
  struct sigaction my_sa = {}; // portable signal handling setup with sigaction()

  my_sa.sa_handler = handle_SIGTERM; // run function handle_SIGTERM
  sigaction(SIGTERM, &my_sa, NULL);  // register SIGTERM with given action

  my_sa.sa_handler = handle_SIGINT;  // run function handle_SIGINT
  sigaction(SIGINT,  &my_sa, NULL);  // register SIGINT with given action

  char server_name[MAXPATH];
  strcpy(server_name, argv[1]);
  server_t server = {}; // create a server struct with empty fields
  server_start(&server, server_name, DEFAULT_PERMS); // start the server and set its fields
  while(no_sig){ // server alive unti; signal received
    server_check_sources(&server); // check to see what clients have data ready
    if(server_join_ready(&server)) server_handle_join(&server); // if client wants to join handle
    for(int i = 0; i < server.n_clients && no_sig; i++){ // for each client
      if(server_client_ready(&server, i)) server_handle_client(&server, i); // if there is messges handle them
    }
  }
  server_shutdown(&server); // server recieved SIGINT || SIGTERM shut down gracefully
  return 0;

}
