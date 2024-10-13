#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/parse_req.h"

#define HTTP_PORT 3000
#define HTTP_ADDR "0.0.0.0"

void handle_client(int clientfd);

int main() {
#ifdef DEV_MODE
  printf("DEV MODE\n");
  setbuf(stdout, NULL);
#endif
  int32_t ret = 0;

  int32_t serverfd;
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(HTTP_ADDR);
  addr.sin_port = htons(HTTP_PORT);

  if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    ret = errno;
    goto cleanup_server;
  }
  // Cant wait TIME_WAIT before restarting while debugging :D
  if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){1},
                 sizeof(int)) < 0) {
    ret = errno;
    goto cleanup_server;
  }

  if (bind(serverfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) < 0) {
    ret = errno;
    printf("Could not bind port %i\n", HTTP_PORT);
    goto cleanup_server;
  }

  if (listen(serverfd, 1) < 0) {
    ret = errno;
    printf("Could not listen for a connection\n");
    goto cleanup_server;
  }

  printf("Listening on %i\n", HTTP_PORT);

  while (true) {
    int clientfd;
    if ((clientfd = accept(serverfd, NULL, NULL)) < 0) {
      ret = errno;
      goto cleanup_server;
    }

    handle_client(clientfd);
  }

cleanup_server:
  close(serverfd);
  fflush(stdout);

  return ret;
}

void handle_client(int clientfd) {
  printf("Client: %i\n", clientfd);

  ParseState pstate;
  HttpReq req;

  if (initialize_request(&req) < 0) {
    goto cleanup_client_fd;
  }

  initialize_parse_state(&pstate, &req);

  uint8_t buf[1024] = {0};

  // Memory allocated, time to parse

  while (pstate.parse_state < 5) {
    int readlen;
    memset(buf, 0, sizeof(buf));
    if ((readlen = read(clientfd, &buf, sizeof(buf))) < 0) {
      goto cleanup_client;
    } else if (readlen == 0) {
      // TODO: possibly already parsed
      /*if (pstate.parse_state >= 4) {
        pstate.request->body[pstate.parse_index] = '\0';
        break;
      }*/
      printf("What happened? %i", pstate.parse_state);
      goto cleanup_client;
    }
    printf("'%s'\n", buf);
    parse_cycle(&pstate, (uint8_t *)&buf, readlen);
  }

  char msg[103];
reply:
  printf("Method: %s\n", req.method);
  printf("Path: %s\n", req.path);
  printf("Body: %s\n", req.body);

  strcpy((char *)&msg,
         "HTTP/1.0 200 OK\r\nServer: cHTTP (Vospel)\r\nContent-Type: "
         "text/html\r\nContent-Length: 11\r\n\r\nHello World!");
  int _ = write(clientfd, msg, sizeof(msg));

cleanup_client:
  free(req.method);
cleanup_client_fd:
  close(clientfd);
  printf("Konec clienta");
}
