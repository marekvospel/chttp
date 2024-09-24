#include "../src/parse_req.c"

#include <criterion/criterion.h>
#include <stdbool.h>
#include <stdio.h>

void with_request(void (*fn)(struct parse_state *)) {
  struct http_req req;
  struct parse_state pstate;

  cr_assert_eq(initialize_request(&req), 0);
  initialize_parse_state(&pstate, &req);

  // Test
  (*fn)(&pstate);

  free(req.method);
  free(req.path);
  free(req.body);
}

void request_get(struct parse_state *pstate) {
  uint8_t buf[] = "GET /abc/def HTTP/1.0\nAccept: */*\nContent-Type: "
                  "application/text\n\nzkouska123\n\n";

  parse_cycle(pstate, (uint8_t *)&buf, sizeof(buf));

  printf("'%s'\n", pstate->request->method);
  printf("'%s'\n", pstate->request->path);
  printf("'%s'\n", pstate->request->body);

  cr_assert(strcmp(pstate->request->method, "GET") == 0);
  cr_assert(strcmp(pstate->request->path, "/abc/def") == 0);
  cr_assert(strcmp(pstate->request->body, "zkouska123") == 0);
}

void request_emptybody(struct parse_state *pstate) {
  uint8_t buf[] = "POST /index.html HTTP/1.0\nUser-Agent: "
                  "C22\nX-Forwarded-For: 127.0.0.1\nHost: vospel.cz\n\n\n\n";

  parse_cycle(pstate, (uint8_t *)&buf, sizeof(buf));

  printf("'%s'\n", pstate->request->method);
  printf("'%s'\n", pstate->request->path);
  printf("'%s'\n", pstate->request->body);

  cr_assert(strcmp(pstate->request->method, "POST") == 0);
  cr_assert(strcmp(pstate->request->path, "/index.html") == 0);
  cr_assert(strcmp(pstate->request->body, "") == 0);
}

void multi_iter(struct parse_state *pstate) {
  uint8_t buf[] =
      "GET /index.css HTTP/1.0\nA: B\nC: D\nE: false\nF: true\n\n\n\n";

  for (int i = 0; i < sizeof(buf); i++) {
    parse_cycle(pstate, (uint8_t *)&(buf[i]), 1);
  }

  printf("'%s'\n", pstate->request->method);
  printf("'%s'\n", pstate->request->path);
  printf("'%s'\n", pstate->request->body);

  cr_assert(strcmp(pstate->request->method, "GET") == 0);
  cr_assert(strcmp(pstate->request->path, "/index.css") == 0);
  cr_assert(strcmp(pstate->request->body, "") == 0);
}

Test(request, get) { with_request(&request_get); }
Test(request, empty) { with_request(&request_emptybody); }
Test(request, multiiter) { with_request(&multi_iter); }
