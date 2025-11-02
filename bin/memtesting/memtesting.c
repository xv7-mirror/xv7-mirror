#include "types.h"
#include "stat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHECK(condition, msg)                                                  \
    if (!(condition)) {                                                        \
        printf(stdout, "  [FAILED] %s\n", msg);                                \
        return 0;                                                              \
    } else {                                                                   \
        printf(stdout, "  [PASSED] %s\n", msg);                                \
    }

void fill_mem(char* ptr, size_t size, char pattern)
{
    for (size_t i = 0; i < size; i++) {
        ptr[i] = pattern;
    }
}

int check_mem(char* ptr, size_t size, char pattern)
{
    for (size_t i = 0; i < size; i++) {
        if (ptr[i] != pattern) {
            return 0;
        }
    }
    return 1;
}

int test_malloc_free()
{
    printf(stdout, "Testing malloc() and free()\n");
    char* p1 = malloc(100);
    CHECK(p1 != NULL, "malloc(100) returned a non-NULL pointer.");

    fill_mem(p1, 100, 0xAA);
    CHECK(check_mem(p1, 100, 0xAA), "Wrote and verified pattern 0xAA in p1.");

    char* p2 = malloc(200);
    CHECK(p2 != NULL, "malloc(200) returned a non-NULL pointer.");

    fill_mem(p2, 200, 0xBB);
    CHECK(check_mem(p2, 200, 0xBB), "Wrote and verified pattern 0xBB in p2.");

    free(p1);
    printf(stdout, "  free(p1) called.\n");

    char* p3 = malloc(50);
    CHECK(p3 != NULL, "malloc(50) (reusing p1's block) returned non-NULL.");

    fill_mem(p3, 50, 0xCC);
    CHECK(check_mem(p3, 50, 0xCC), "Wrote and verified pattern 0xCC in p3.");

    CHECK(check_mem(p2, 200, 0xBB),
        "Integrity check: p2's data is not corrupted.");

    free(p2);
    free(p3);
    printf(stdout, "  free(p2) and free(p3) called.\n");

    printf(stdout, "Passed malloc() and free() tests\n\n");
    return 1;
}

int test_calloc()
{
    printf(stdout, "Testing calloc()\n");
    size_t count = 100;
    size_t elem_size = sizeof(int);
    int* p1 = calloc(count, elem_size);
    CHECK(p1 != NULL, "calloc(100, sizeof(int)) returned non-NULL.");

    int is_zeroed = 1;
    for (size_t i = 0; i < count; i++) {
        if (p1[i] != 0) {
            is_zeroed = 0;
            break;
        }
    }
    CHECK(is_zeroed, "Memory allocated by calloc is zero-initialized (0x00).");

    free(p1);
    printf(stdout, "  free(p1) called.\n");

    printf(stdout, "Passed calloc() test\n\n");
    return 1;
}

int test_realloc()
{
    printf(stdout, "Testing realloc()\n");

    char* r1 = realloc(NULL, 100);
    CHECK(r1 != NULL, "realloc(NULL, 100) (as malloc) returned non-NULL.");
    fill_mem(r1, 100, 0xDD);
    CHECK(check_mem(r1, 100, 0xDD), "Wrote and verified pattern 0xDD in r1.");

    char* r2 = realloc(r1, 200);
    CHECK(r2 != NULL, "realloc(r1, 200) (grow block) returned non-NULL.");

    CHECK(check_mem(r2, 100, 0xDD),
        "Data from r1 is preserved in r2 after grow.");

    fill_mem(r2 + 100, 100, 0xEE);
    CHECK(check_mem(r2 + 100, 100, 0xEE),
        "Wrote and verified pattern 0xEE in new part of r2.");

    char* r3 = realloc(r2, 50);
    CHECK(r3 != NULL, "realloc(r2, 50) (shrink block) returned non-NULL.");

    CHECK(check_mem(r3, 50, 0xDD),
        "Data from r2 is preserved in r3 after shrink.");

    void* r4 = realloc(r3, 0);
    CHECK(r4 == NULL, "realloc(r3, 0) (as free) returned NULL.");

    char* r5 = malloc(40);
    CHECK(r5 != NULL, "malloc(40) after realloc-free succeeded.");
    free(r5);

    printf(stdout, "Passed realloc() test\n\n");
    return 1;
}

int main(int argc, char* argv[])
{
    int all_passed = 1;

    if (!test_malloc_free()) {
        all_passed = 0;
    }

    if (!test_calloc()) {
        all_passed = 0;
    }

    if (!test_realloc()) {
        all_passed = 0;
    }

    if (all_passed) {
        printf(stdout, "Memory tests passed!\n");
        return 0;
    } else {
        printf(stdout, "Some tests have failed.\n");
        return -1;
    }
}
