#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/parse_req.h"

void _set_state(struct parse_state *pstate, uint8_t next) {
  pstate->parse_state = next;
  pstate->last_cr = false;
  pstate->last_newline = false;
  pstate->parse_index = 0;
}

void parse_cycle(struct parse_state *pstate, uint8_t *buf,
                 uint_fast32_t bufsize) {

  for (int i = 0; i < bufsize; i += 1) {

    if (pstate->parse_state == 4) {
      if (pstate->parse_index + 1 >= HTTP_BODY_MAX_SIZE) {
        pstate->request->body[pstate->parse_index] = '\0';
        _set_state(pstate, 5);
      } else if (pstate->parse_index + 1 >= pstate->bodylen) {
        pstate->request->body[pstate->parse_index] = '\0';
        _set_state(pstate, 5);
      }
    }

    if (buf[i] == '\n' && pstate->last_cr) {
      if (pstate->parse_state <= 2) {

        // HTTP header
        if (pstate->parse_state == 0) {
          pstate->request->method[pstate->parse_index] = '\0';
        } else if (pstate->parse_state == 1) {
          pstate->request->path[pstate->parse_index] = '\0';
        }

        _set_state(pstate, 3);
        continue;
      }
      // Headers
      if (pstate->parse_state == 3 && pstate->last_newline) {
        if (pstate->bodylen == 0) {
          pstate->request->body[0] = '\0';
          _set_state(pstate, 5);
        } else {
          _set_state(pstate, 4);
        }
        continue;
      }
      pstate->last_newline = true;
    } else if (buf[i] != '\r') {
      pstate->last_newline = false;
    }

    if (buf[i] == '\r') {
      pstate->last_cr = true;
    } else {
      pstate->last_cr = false;
    }

    if (buf[i] == ' ') {
      if (pstate->parse_state == 0) {
        pstate->request->method[pstate->parse_index] = '\0';
        _set_state(pstate, 1);
        continue;
      }
      if (pstate->parse_state == 1) {
        pstate->request->path[pstate->parse_index] = '\0';
        _set_state(pstate, 2);
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
  char *mem =
      malloc(HTTP_METHOD_MAX_SIZE + HTTP_PATH_MAX_SIZE + HTTP_BODY_MAX_SIZE);
  if (mem == NULL) {
    return -1;
  }

  req->method = (char *)mem;
  req->path = mem + HTTP_METHOD_MAX_SIZE;
  req->body = mem + HTTP_METHOD_MAX_SIZE + HTTP_PATH_MAX_SIZE;

  printf("%p\n", req->method);
  printf("%p\n", req->path);
  printf("%p\n", req->body);

  return 0;
}

void initialize_parse_state(struct parse_state *pstate, struct http_req *req) {
  pstate->request = req;
  pstate->bodylen = 0;
  _set_state(pstate, 0);
}
