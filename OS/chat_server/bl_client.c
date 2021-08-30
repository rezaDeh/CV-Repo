#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "blather.h"

simpio_t simpio_actual;
simpio_t *simpio = &simpio_actual;

// global varianles to be used in workers
// no mutex required because workers will no manipulate their data
char client[MAXPATH];
char to_client_fifo[MAXPATH];
char to_server_fifo[MAXPATH];

pthread_t user_thread; // thread managing user input
pthread_t background_thread;

// Worker thread to manage user input
// no return val or prameters
void *user_worker(void *arg){
  int to_server_fd = open(to_server_fifo, O_RDWR);  // open to server fifo to send messeges
  if(to_server_fd == -1){
    perror("Error opening file");
    exit(1);
  }
  while(!simpio->end_of_input){
    simpio_reset(simpio);
    iprintf(simpio, ""); // print prompt
    while(!simpio->line_ready && !simpio->end_of_input){  // read until line is complete
      simpio_get_char(simpio);
    }
    if(simpio->line_ready){ // when there is a input ready
      mesg_t user_mesg = {}; // create a message struct and filll in fields appropietly
      user_mesg.kind = BL_MESG;
      strcpy(user_mesg.name, client);
      strcpy(user_mesg.body, simpio->buf);
      write(to_server_fd, &user_mesg, sizeof(mesg_t)); // send massge to server via the appropriate fifo
    }
  }
  mesg_t dep_mesg = {}; // if no more input left
  dep_mesg.kind = BL_DEPARTED; // create  depart messege  and send to fifo
  strcpy(dep_mesg.name, client);
  strcpy(dep_mesg.body, "");
  write(to_server_fd, &dep_mesg, sizeof(mesg_t));
  pthread_cancel(background_thread); // kill the background thread
  close(to_server_fd);
  return NULL;
}



// Worker thread to listen to the info from the server
// no return val or prameters
void *background_worker(void *arg){
  int to_client_fd = open(to_client_fifo, O_RDWR); // open to client fifo to receive messeges
  if(to_client_fd == 0){
    perror("Error opening file");
    exit(1);
  }
  while(1){ // as long a server is no shutt down check for messeges
    mesg_t server_mesg = {};
    read(to_client_fd, &server_mesg, sizeof(mesg_t));
    // depending on the messege kind act appropriately
    if(server_mesg.kind == BL_SHUTDOWN){
      iprintf(simpio, "!!! server is shutting down !!!\n");
      pthread_cancel(user_thread); // kill the background thread
      close(to_client_fd);
      return NULL; // return NULL when shutdown
    }
    if(server_mesg.kind == BL_MESG){ // print massge to the screen using iprintf
      iprintf(simpio, "[%s] : %s\n", server_mesg.name, server_mesg.body);
    }
    else if(server_mesg.kind == BL_JOINED) iprintf(simpio, "-- %s JOINED --\n", server_mesg.name);
    else if(server_mesg.kind == BL_DEPARTED) iprintf(simpio, "-- %s DEPARTED --\n", server_mesg.name);
  }
}



int main(int argc, char *argv[]){
  if(argc < 3){
    printf("usage: <server name> <client name>");
    return 0;
  }
  char server[MAXPATH];
  char server_fifo[MAXPATH];
  strcpy(server, argv[1]);
  strcpy(client, argv[2]);
  // slightly different to/from fifo name standard: to_clientname_from_servername || to_servername_from_clientname
  sprintf(to_server_fifo, "to_%s_from_%s.fifo", server, client);
  sprintf(to_client_fifo, "to_%s_from_%s.fifo", client, server);
  remove(to_server_fifo); // remove all existing versions of that fifo
  remove(to_client_fifo);
  mkfifo(to_server_fifo, DEFAULT_PERMS); // create new fifo with perms defiend in blather.h
  mkfifo(to_client_fifo, DEFAULT_PERMS);
  join_t join_request = {}; // crete a join struct to ask to join the sever
  strcpy(join_request.name, client);
  strcpy(join_request.to_server_fname, to_server_fifo); // set up fifos so server knows which ones to use for communication
  strcpy(join_request.to_client_fname, to_client_fifo);
  sprintf(server_fifo, "%s.fifo", server);
  int server_fd = open(server_fifo, O_RDWR); // open FIFO read/write in case server hasn't started
  if(server_fd == -1){
    perror("Error opening file");
    exit(1);
  }
  write(server_fd, &join_request, sizeof(join_t)); // send the join request to the server

  char prompt[MAXNAME];
  snprintf(prompt, MAXNAME, "%s>> ", client); // create a prompt string
  simpio_set_prompt(simpio, prompt);         // set the prompt
  simpio_reset(simpio);                      // initialize io
  simpio_noncanonical_terminal_mode();       // set the terminal into a compatible mode

  pthread_create(&user_thread, NULL, user_worker, NULL); // start user thread to read input
  pthread_create(&background_thread, NULL, background_worker, NULL); // start a thread to get messeges from server
  pthread_join(user_thread, NULL); // wait for threads to response
  pthread_join(background_thread, NULL);

  simpio_reset_terminal_mode();
  printf("\n");                 // newline just to make returning to the terminal prettier
  close(server_fd);


}

//   }
//   write(server_fd, &join_request, sizeof(join_t));
//
//   char prompt[MAXNAME];
//   snprintf(prompt, MAXNAME, "%s>> ", client); // create a prompt string
//   simpio_set_prompt(simpio, prompt);         // set the prompt
//   simpio_reset(simpio);                      // initialize io
//   simpio_noncanonical_terminal_mode();       // set the terminal into a compatible mode
//
//   arg_t *args = NULL;
//   strcpy(args->arg1, client);
//   strcpy(args->arg2, to_server_fifo);
//   pthread_create(&user_thread, NULL, user_worker, (void *) args);     // start user thread to read input
//   pthread_create(&background_thread, NULL, background_worker, (void *) to_client_fifo);
//   pthread_join(user_thread, NULL);
//   pthread_join(background_thread, NULL);
//
//   simpio_reset_terminal_mode();
//   printf("\n");                 // newline just to make returning to the terminal prettier
//
//
// }
