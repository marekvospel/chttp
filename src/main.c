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

struct http_req {
  char *method;
  char *path;
  char *body;
};

#define HTTP_METHOD_MAX_SIZE 16
#define HTTP_PATH_MAX_SIZE 256
#define HTTP_BODY_MAX_SIZE 2048

void handle_client(int clientfd) {
  printf("Client: %i\n", clientfd);

  struct http_req req;

  if ((req.method = (char *)malloc(HTTP_METHOD_MAX_SIZE)) == NULL) {
    goto cleanup_client_fd;
  }
  if ((req.path = (char *)malloc(HTTP_PATH_MAX_SIZE)) == NULL) {
    goto cleanup_client_path;
  }
  if ((req.body = (char *)malloc(HTTP_BODY_MAX_SIZE)) == NULL) {
    goto cleanup_client_body;
  }

  // 0 = method
  // 1 = path
  // 2 = version
  // 3 = headers
  // 4 = body
  int_fast8_t parse_state = 0;
  uint8_t buf[10 + 1] = {0};
  int_fast32_t parse_index = 0;
  bool last_newline = false;

  while (parse_state < 5) {
    int readlen;
    memset(buf, 0, sizeof(buf));
    if ((readlen = read(clientfd, &buf, sizeof(buf) - 1)) < 0) {
      goto cleanup_client;
    } else if (readlen == 0) {
      goto cleanup_client; // ? idk, maybe error
    }

    buf[sizeof(buf) - 1] = '\0';

    for (int i = 0; i < strlen((char *)&buf); i += 1) {
      // State transitions
      if (buf[i] == '\n') {
        if (parse_state <= 2) {
          if (parse_state == 0) {
            req.method[parse_index] = '\0';
          } else if (parse_state == 1) {
            req.path[parse_index] = '\0';
          }

          parse_state = 3;
          parse_index = 0;
          last_newline = false;
          continue;
        }
        if (parse_state == 3 && last_newline) {
          last_newline = false;
          parse_index = 0;
          parse_state = 4;
          continue;
        }
        if (parse_state == 4 && last_newline) {
          req.body[parse_index] = '\0';

          goto reply;
        }
        last_newline = true;
      } else {
        last_newline = false;
      }

      if (buf[i] == ' ') {
        if (parse_state == 0) {
          req.method[parse_index] = '\0';
          parse_index = 0;
          parse_state = 1;
          continue;
        }
        if (parse_state == 1) {
          req.path[parse_index] = '\0';
          parse_index = 0;
          parse_state = 2;
          continue;
        }
      }
      if (parse_state == 0) {
        if (parse_index >= HTTP_METHOD_MAX_SIZE) {
          continue;
        }

        req.method[parse_index] = buf[i];
      } else if (parse_state == 1) {
        if (parse_index >= HTTP_PATH_MAX_SIZE) {
          continue;
        }

        req.path[parse_index] = buf[i];
      } else if (parse_state == 4) {
        if (parse_index >= HTTP_BODY_MAX_SIZE) {
          continue;
        }

        req.body[parse_index] = buf[i];
      }

      parse_index += 1;
    }

    // printf("Recv buffer \"%s\"\n", buf);
  }

  char msg[16];
reply:
  printf("Method: %s\n", req.method);
  printf("Path: %s\n", req.path);
  printf("Body: %s\n", req.body);

  strcpy((char *)&msg, "Hello world!\n");
  write(clientfd, msg, sizeof(msg));

cleanup_client:
cleanup_client_body:
  free(req.body);
cleanup_client_path:
  free(req.path);
cleanup_client_method:
  free(req.method);
cleanup_client_fd:
  close(clientfd);
  printf("Konec clienta");
}
