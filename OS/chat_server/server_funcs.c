#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "blather.h"

// server_t: data pertaining to server operations
// typedef struct {
//   char server_name[MAXPATH];    // name of server which dictates file names for joining and logging
//   int join_fd;                  // file descriptor of join file/FIFO
//   int join_ready;               // flag indicating if a join is available
//   int n_clients;                // number of clients communicating with server
//   client_t client[MAXCLIENTS];  // array of clients populated up to n_clients
//   int time_sec;                 // ADVANCED: time in seconds since server started
//   int log_fd;                   // ADVANCED: file descriptor for log
//   sem_t *log_sem;               // ADVANCED: posix semaphore to control who_t section of log file
// } server_t

client_t *server_get_client(server_t *server, int idx) {
// Gets a pointer to the client_t struct at the given index.
  client_t* client = &server->client[idx];              // Pointer to client_t struct
  if(idx >= server->n_clients){
    perror("The index is beyond n_clients");
  }
  return client;
}


void server_start(server_t *server, char *server_name, int perms) {
// Initializes and starts the server with the given name.
  log_printf("BEGIN: server_start()\n");
  
  char join_fifo[MAXPATH];
  snprintf(join_fifo, MAXPATH, "%s.fifo", server_name);       // A join fifo called "server_name.fifo"
  remove(join_fifo);                                          // REMOVES any existing file of that name prior to creation
  perms = S_IRUSR | S_IWUSR;
  mkfifo(join_fifo, perms);                                   // CREATE join FIFO.

  int fd = open(join_fifo, O_RDWR);                           // OPEN the FIFO 
  if(fd == -1){
    perror("Error opening file");
  }
  server->join_fd = fd; 
  
  // INITIALIZE server
  server->join_ready = 0;
  server->n_clients = 0;
  int server_name_len = sizeof(server->server_name);
  snprintf(server->server_name, server_name_len, "%s", server_name);

  log_printf("END: server_start()\n");
}


void server_shutdown(server_t *server) {
// Shut down the server.
  log_printf("BEGIN: server_shutdown()\n");
  int ret = close(server->join_fd);          // CLOSE the join FIFO

  if(ret == -1){
    perror("Error closing fifo");
  }
  else{
    // UNLINK (remove) it so that no further clients can join.
    server->join_fd = -1;                    
    char join_fifo[MAXPATH];
    snprintf(join_fifo, (sizeof(join_fifo) + 5), "%s.fifo", server->server_name);
    remove(join_fifo);

    // Send a BL_SHUTDOWN message to all clients
    // and proceed to remove all clients in any order.
    mesg_t msg = {};
    msg.kind = BL_SHUTDOWN;
    server_broadcast(server, &msg);   
    server->n_clients = 0;       
  }
  log_printf("END: server_shutdown()\n");
}


int server_add_client(server_t *server, join_t *join) {
// Adds a client to the server according to the parameter join which should have 
// fileds such as name filed in.
  log_printf("BEGIN: server_add_client()\n");
  if (server->n_clients == MAXCLIENTS){               // The server as no space for clients
    log_printf("END: server_add_client()\n");
    return -1;
  }

  char to_server_fifo[MAXPATH];
  char to_client_fifo[MAXPATH];
  snprintf(to_server_fifo, (sizeof(to_server_fifo) + 5), "%s", join->to_server_fname);
  snprintf(to_client_fifo, (sizeof(to_client_fifo) + 5), "%s", join->to_client_fname);
  
  // CREATE new client
  client_t new;
  
  // File descriptors are opened for its to-server and to-client FIFOs
  new.to_server_fd = open(to_server_fifo, O_RDWR);
  new.to_client_fd = open(to_client_fifo, O_RDWR);

  // INITIALIZE new client
  strncpy(new.name, join->name, sizeof(join->name));
  strncpy(new.to_server_fname, join->to_server_fname, sizeof(join->to_server_fname));
  strncpy(new.to_client_fname, join->to_client_fname, sizeof(join->to_client_fname));
  new.data_ready = 0;

  // ADD new client to server 
  server->client[server->n_clients] = new;
  server->n_clients++;

  log_printf("END: server_add_client()\n");
  return 0;
}


int server_remove_client(server_t *server, int idx) {
// Remove the given client likely due to its having departed or disconnected.
  client_t *client = server_get_client(server, idx);
  
  // CLOSE fifos associated with the client
  close(client->to_client_fd);
  close(client->to_server_fd);

  // REMOVE them
  remove(client->to_client_fname);
  remove(client->to_server_fname);

  int n_clients = server->n_clients;
  // SHIFT the remaining clients to lower indices of the client[] preserving 
  // their order in the array.
  for(int i = idx; i < n_clients - 1; i++){
    server->client[i] = server->client[i+1];
  }

  // DECREASES n_clients
  server->n_clients -= 1;
  return 0;
}


int server_broadcast(server_t *server, mesg_t *mesg) {

// Send the given message to all clients connected to the server.
  for (int i = 0; i < server->n_clients; i++) { 
    // WRITE mesg to the file descriptors associated with them.
    write(server->client[i].to_client_fd, mesg, sizeof(mesg_t));
  }
  return 0;
}


void server_check_sources(server_t *server) {
// Checks all sources of data for the server to determine if any are
// ready for reading.

  log_printf("BEGIN: server_check_sources()\n");
  // SETUP for polling for SERVER
  struct pollfd fds[server->n_clients + 1];             // Poll struct array.
  int n_clients = server->n_clients;

  // Initialize fields
  fds[n_clients].fd = server->join_fd;
  fds[n_clients].events = POLLIN;
  for (int i = 0; i < n_clients + 1; i++) {             
      fds[i].revents = 0;
  }

  // SETUP for polling for CLIENT
  for (int i = 0; i < n_clients; i++) {
    client_t* client = server_get_client(server, i);
    // Initialize fields
    fds[i].fd = client->to_server_fd;
    fds[i].events = POLLIN;
    fds[i].revents = 0;
  } 

  log_printf("poll()'ing to check %d input sources\n", server->n_clients + 1);
  errno = 0;
  int ret = poll(fds, n_clients + 1, -1);         // poll() call
  log_printf("poll() completed with return value %d\n", ret);
  if (ret == -1 && errno == EINTR)
  {
    log_printf("poll() interrupted by a signal\n");
    log_printf("END: server_check_sources()\n");
    return;
  }
  else if (ret == -1) {
    perror("poll()'ing failed");                  // Error for failed poll()
  }

  //  CHECK if SERVER ready for reading
  if (fds[n_clients].revents & POLLIN){
    server->join_ready = 1;
  }
  log_printf("join_ready = %d\n", server->join_ready);

  // CHECK if CLIENT sources are ready
  for (int i = 0; i < n_clients; i++) {
    client_t* client = server_get_client(server, i);
    if (fds[i].revents & POLLIN) {
      client->data_ready = 1;  
    }
    log_printf("client %d '%s' data_ready = %d\n", i, client->name, client->data_ready);
  }
  log_printf("END: server_check_sources()\n");
}


int server_join_ready(server_t *server) {
// Return the join_ready flag from the server which indicates whether
// a call to server_handle_join() is safe.
  return server->join_ready;
}


int server_handle_join(server_t *server) {
// Read a join request and add the new client to the server.
  log_printf("BEGIN: server_handle_join()\n");
  join_t join_request;      
  int ret = read(server->join_fd, &join_request, sizeof(join_t));   // Read join request
  if(ret == -1){
    log_printf("END: server_handle_join()\n");
    return -1;                                                      // Error reading join request
  }
  log_printf("join request for new client '%s'\n", join_request.name);
  int ret2 = server_add_client(server, &join_request);              // Add the new client to the server.
  server->join_ready = 0;                                           // Set the servers join_ready flag to 0.

  // Broadcast join message to all clients.
  mesg_t msg = {};
  msg.kind = BL_JOINED;
  strncpy(msg.name, join_request.name, sizeof(join_request.name));
  
  server_broadcast(server, &msg);
  log_printf("END: server_handle_join()\n");
  return ret2;
}


int server_client_ready(server_t *server, int idx) {
// Return the data_ready field of the given client which indicates
// whether the client has data ready to be read from it.
  client_t* client = server_get_client(server, idx);
  return client->data_ready;
}


int server_handle_client(server_t *server, int idx) {
// Process a message from the specified client.
  log_printf("BEGIN: server_handle_client()\n");
  check_fail(idx >= server->n_clients, -1, "Out of bounds");
  if(server_client_ready(server, idx)){
    client_t* client = server_get_client(server, idx);
    mesg_t msg = {};

    // READ a message from to_server_fd.
    int ret = read(client->to_server_fd, &msg, sizeof(mesg_t));
    if(ret == -1){
      return -1;                                  // Error reading
    }

    // Analyze the message kind.
    mesg_kind_t msg_type = msg.kind;

    if(msg_type == BL_DEPARTED){   
      log_printf("client %d '%s' DEPARTED\n", idx, server->client[idx].name);     
      server_remove_client(server, idx);          // Remove departed client
    }
    
    if(msg_type == BL_MESG){   
      log_printf("client %d '%s' MESSAGE '%s'\n", idx, server->client[idx].name, msg.body);
    }

    // Departure and Message types should be broadcast to all other clients.
    if(msg_type == BL_DEPARTED || msg_type == BL_MESG){
      
      server_broadcast(server, &msg);
    }

    // CLEAR the client's data_ready flag so it has value 0.
    client->data_ready = 0;
    log_printf("END: server_handle_client()\n");
    return 0;
  }
  log_printf("END: server_handle_client()\n");
  return 1;
}

// OPTIONAL
void server_tick(server_t *server) { 

}

void server_ping_clients(server_t *server) {

}


void server_remove_disconnected(server_t *server, int disconnect_secs) {

}

void server_write_who(server_t *server){

}


void server_log_message(server_t *server, mesg_t *mesg){

}

