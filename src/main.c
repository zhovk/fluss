/*
 * main.c
 *
 * Created on: Aug 9, 2026
 * Author: zhovk@github.com
 */

#include "fluss/server.h"
#include <fluss/fluss.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

  fluss_server_t *server = fluss_server_create(8000);
  fluss_server_listen(server);
  fluss_server_free(server);

  return EXIT_SUCCESS;
}
