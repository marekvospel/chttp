#include <criterion/criterion.h>
#include <stdbool.h>

Test(misc, passing) { cr_assert_eq(true, true); }
