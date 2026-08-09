/*
 * server.h
 *
 * Created on: Aug 9, 2026
 * Author: zhovk@github.com
 */

#ifndef FLUSS_SERVER_H
#define FLUSS_SERVER_H

#include <netinet/in.h>
#include <sys/types.h>

typedef struct {
  int socketfd;
  int port;
  int max_threads;
  struct sockaddr_in address;
} fluss_server_t;

fluss_server_t *fluss_server_create(int port);
void fluss_server_listen(fluss_server_t *instance);
void fluss_server_free(fluss_server_t *instance);

#endif /* FLUSS_SERVER_H */