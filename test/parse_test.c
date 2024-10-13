#include "../src/parse_req.c"

#include <criterion/criterion.h>
#include <stdbool.h>
#include <stdio.h>

void with_request(void (*fn)(ParseState *)) {
  HttpReq req;
  ParseState pstate;

  cr_assert_eq(initialize_request(&req), 0);
  initialize_parse_state(&pstate, &req);

  // Test
  (*fn)(&pstate);

  free(req.method);
}

void request_get(ParseState *pstate) {
  uint8_t buf[] = "GET /abc/def HTTP/1.0\r\nAccept: */*\r\nContent-Type: "
                  "application/text\r\n\r\nzkouska123";

  parse_cycle(pstate, (uint8_t *)&buf, sizeof(buf));

  printf("'%s'\n", pstate->request->method);
  printf("'%s'\n", pstate->request->path);
  printf("'%s'\n", pstate->request->body);

  cr_assert_eq(pstate->parse_state, 5);

  cr_assert(strcmp(pstate->request->method, "GET") == 0);
  cr_assert(strcmp(pstate->request->path, "/abc/def") == 0);
  // Currently not working, as Content-Length is not parsed :D
  // cr_assert(strcmp(pstate->request->body, "zkouska123") == 0);
}

void request_emptybody(ParseState *pstate) {
  uint8_t buf[] =
      "POST /index.html HTTP/1.0\r\nUser-Agent: "
      "C22\r\nX-Forwarded-For: 127.0.0.1\r\nHost: vospel.cz\r\n\r\n";

  parse_cycle(pstate, (uint8_t *)&buf, sizeof(buf));

  printf("'%s'\n", pstate->request->method);
  printf("'%s'\n", pstate->request->path);
  printf("'%s'\n", pstate->request->body);

  cr_assert_eq(pstate->parse_state, 5);

  cr_assert(strcmp(pstate->request->method, "POST") == 0);
  cr_assert(strcmp(pstate->request->path, "/index.html") == 0);
  cr_assert(strcmp(pstate->request->body, "") == 0);
}

void multi_iter(ParseState *pstate) {
  uint8_t buf[] = "GET /index.css HTTP/1.0\r\nA: B\r\nC: D\r\nE: false\r\nF: "
                  "true\r\n\r\n";

  for (int i = 0; i < sizeof(buf); i++) {
    parse_cycle(pstate, (uint8_t *)&(buf[i]), 1);
  }

  printf("'%s'\n", pstate->request->method);
  printf("'%s'\n", pstate->request->path);
  printf("'%s'\n", pstate->request->body);

  cr_assert_eq(pstate->parse_state, 5);

  cr_assert(strcmp(pstate->request->method, "GET") == 0);
  cr_assert(strcmp(pstate->request->path, "/index.css") == 0);
  cr_assert(strcmp(pstate->request->body, "") == 0);
}

Test(request, get) { with_request(&request_get); }
Test(request, empty) { with_request(&request_emptybody); }
Test(request, multiiter) { with_request(&multi_iter); }
