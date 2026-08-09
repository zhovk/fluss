/*
 * server.c
 *
 * Created on: Aug 9, 2026
 * Author: zhovk@github.com
 */

#include <arpa/inet.h>
#include <fluss/server.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static void initialize_server_socket(fluss_server_t *instance);
static void handle_client_connection(struct in_addr sin_addr,
                                     in_port_t sin_port);
static void handle_client_buffer(int connfd);

fluss_server_t *fluss_server_create(int port) {
  fluss_server_t *instance = malloc(sizeof(fluss_server_t));

  if (instance == NULL) {
    perror("Allocation memory error for server instance.");
    exit(EXIT_FAILURE);
  }

  instance->port = port;
  instance->max_threads = 5;

  printf("Starting initialization of the server..\n");
  initialize_server_socket(instance);
  printf("Server was succesfully created on port *%d*.\n", port);

  return instance;
}

static void initialize_server_socket(fluss_server_t *instance) {
  instance->socketfd = socket(AF_INET, SOCK_STREAM, 0);

  if (instance->socketfd == -1) {
    perror("Opening socket went wrong.");
    exit(EXIT_FAILURE);
  }

  printf("\t* [1/3] Socket opened succesfully.\n");

  int opt = 1;
  if (setsockopt(instance->socketfd, SOL_SOCKET, SO_REUSEADDR, &opt,
                 sizeof(opt)) < 0) {
    perror("Configuring socket for reuse went wrong.");
    exit(EXIT_FAILURE);
  }

  printf("\t* [2/3] Socket configured succesfully.\n");

  memset(&instance->address, 0, sizeof(instance->address));
  instance->address.sin_family = AF_INET;
  instance->address.sin_addr.s_addr = htonl(INADDR_ANY);
  instance->address.sin_port = htons(instance->port);

  if (bind(instance->socketfd, (struct sockaddr *)&instance->address,
           sizeof(instance->address)) != 0) {
    perror("Socket attempt to bind was failur.");
    exit(EXIT_FAILURE);
  }

  printf("\t* [3/3] Socket binded succesfully.\n");
}

static void handle_client_buffer(int connfd) {
  char buffer[1024];
  ssize_t bytes_read = read(connfd, buffer, sizeof(buffer));

  if (bytes_read == 0) {
    printf("Client disconnected.\n");
  } else if (bytes_read < 0) {
    perror("Read error");
  }

  close(connfd);
}

static void handle_client_connection(struct in_addr sin_addr,
                                     in_port_t sin_port) {
  char client_ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(sin_addr), client_ip, sizeof(client_ip));
  int client_port = ntohs(sin_port);

  printf("New client connected from %s:%d.\n", client_ip, client_port);
}

void fluss_server_listen(fluss_server_t *instance) {
  if (listen(instance->socketfd, 1024) != 0) {
    perror("Server fail to start listening.");
    exit(EXIT_FAILURE);
  }

  printf("Server listening ...\n");

  while (1) {
    struct sockaddr_in cli;
    socklen_t len = sizeof(cli);

    int connfd = accept(instance->socketfd, (struct sockaddr *)&cli, &len);

    if (connfd < 0) {
      perror("Server doesn't accept connection.");
      exit(EXIT_FAILURE);
    }

    handle_client_connection(cli.sin_addr, cli.sin_port);
    handle_client_buffer(connfd);
  }
}
void fluss_server_free(fluss_server_t *instance) {
  if (instance == NULL) {
    return;
  }

  if (instance->socketfd >= 0) {
    close(instance->socketfd);
  }

  free(instance);
}