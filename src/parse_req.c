#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/parse_req.h"

void parse_cycle(struct parse_state *pstate, uint8_t *buf,
                 uint_fast32_t bufsize) {
  for (int i = 0; i < bufsize; i += 1) {

    if (buf[i] == '\n') {
      if (pstate->parse_state <= 2) {
        if (pstate->parse_state == 0) {
          pstate->request->method[pstate->parse_index] = '\0';
        } else if (pstate->parse_state == 1) {
          pstate->request->path[pstate->parse_index] = '\0';
        }

        pstate->parse_state = 3;
        pstate->parse_index = 0;
        pstate->last_newline = false;
        continue;
      }
      if (pstate->parse_state == 3 && pstate->last_newline) {
        pstate->last_newline = false;
        pstate->parse_index = 0;
        pstate->parse_state = 4;
        continue;
      }
      if (pstate->parse_state == 4 && pstate->last_newline) {
        pstate->request->body[pstate->parse_index] = '\0';

        pstate->parse_state = 5;
      }
      pstate->last_newline = true;
    } else {
      pstate->last_newline = false;
    }

    if (buf[i] == ' ') {
      if (pstate->parse_state == 0) {
        pstate->request->method[pstate->parse_index] = '\0';
        pstate->parse_index = 0;
        pstate->parse_state = 1;
        continue;
      }
      if (pstate->parse_state == 1) {
        pstate->request->path[pstate->parse_index] = '\0';
        pstate->parse_index = 0;
        pstate->parse_state = 2;
        continue;
      }
    }
    if (pstate->parse_state == 0) {
      if (pstate->parse_index >= HTTP_METHOD_MAX_SIZE) {
        continue;
      }

      pstate->request->method[pstate->parse_index] = buf[i];
    } else if (pstate->parse_state == 1) {
      if (pstate->parse_index >= HTTP_PATH_MAX_SIZE) {
        continue;
      }

      pstate->request->path[pstate->parse_index] = buf[i];
    } else if (pstate->parse_state == 4) {
      if (pstate->parse_index >= HTTP_BODY_MAX_SIZE) {
        continue;
      }

      pstate->request->body[pstate->parse_index] = buf[i];
    }

    pstate->parse_index += 1;
  }
}

int initialize_request(struct http_req *req) {

  if ((req->method = (char *)malloc(HTTP_METHOD_MAX_SIZE)) == NULL) {
    return -1;
  }
  if ((req->path = (char *)malloc(HTTP_PATH_MAX_SIZE)) == NULL) {
    free(req->method);
    return -1;
  }
  if ((req->body = (char *)malloc(HTTP_BODY_MAX_SIZE)) == NULL) {
    free(req->method);
    free(req->path);
    return -1;
  }

  return 0;
}

void initialize_parse_state(struct parse_state *pstate, struct http_req *req) {
  pstate->request = req;
  pstate->parse_state = 0;
  pstate->parse_index = 0;
  pstate->last_newline = false;
}
