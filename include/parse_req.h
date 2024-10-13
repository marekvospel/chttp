#include <stdbool.h>
#include <stdint.h>

typedef struct {
  char *method;
  char *path;
  char *body;
} HttpReq;

typedef struct {
  HttpReq *request;
  uint_fast32_t parse_index;
  //*
  // 0 = method
  // 1 = path
  // 2 = version
  // 3 = headers
  // 4 = body
  //*/
  uint_fast8_t parse_state;
  // True if last line was CRLF, not reset if CRLFCR found
  bool last_newline;
  // Only true if latest character is actually a \r
  bool last_cr;
  uint_fast32_t bodylen;
} ParseState;

#define HTTP_METHOD_MAX_SIZE 16
#define HTTP_PATH_MAX_SIZE 256
#define HTTP_BODY_MAX_SIZE 2048

int initialize_request(HttpReq *req);
void initialize_parse_state(ParseState *pstate, HttpReq *req);
void parse_cycle(ParseState *pstate, uint8_t *buf, uint_fast32_t bufsize);
