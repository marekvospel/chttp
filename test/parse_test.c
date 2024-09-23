#include "../src/parse_req.c"

#include <criterion/criterion.h>
#include <stdbool.h>
#include <stdio.h>

Test(simple, test) {

  struct http_req req;
  struct parse_state pstate;

  cr_assert_eq(initialize_request(&req), 0);
  initialize_parse_state(&pstate, &req);

  uint8_t buf[] = "GET /abc/def HTTP/1.0\nAccept: */*\nContent-Type: "
                  "application/text\n\nzkouska123\n\n";

  parse_cycle(&pstate, (uint8_t *)&buf, sizeof(buf));

  printf("'%s' %i\n", req.method, strcmp(req.method, "GET"));
  printf("'%s' %i\n", req.path, strcmp(req.path, "/abc/def"));
  printf("'%s' %i\n", req.body, strcmp(req.body, "zkouska123"));

  cr_assert(strcmp(req.method, "GET") == 0);
  cr_assert(strcmp(req.path, "/abc/def") == 0);
  // TODO: wrong value, body shouldn't have the last newline
  cr_assert(strcmp(req.body, "zkouska123\n") == 0);

  free(req.method);
  free(req.path);
  free(req.body);
}
