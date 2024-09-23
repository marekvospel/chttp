#include <stdbool.h>
#include <stdint.h>

struct parse_state {
  struct http_req *request;
  uint_fast32_t parse_index;
  //*
  // 0 = method
  // 1 = path
  // 2 = version
  // 3 = headers
  // 4 = body
  //*/
  uint_fast8_t parse_state;
  bool last_newline;
};

struct http_req {
  char *method;
  char *path;
  char *body;
};

#define HTTP_METHOD_MAX_SIZE 16
#define HTTP_PATH_MAX_SIZE 256
#define HTTP_BODY_MAX_SIZE 2048

int initialize_request(struct http_req *req);
void initialize_parse_state(struct parse_state *pstate, struct http_req *req);
void parse_cycle(struct parse_state *pstate, uint8_t *buf,
                 uint_fast32_t bufsize);
