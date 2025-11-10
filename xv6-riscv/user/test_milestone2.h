// user/test_milestone2.h
#ifndef _TEST_MILESTONE2_H_
#define _TEST_MILESTONE2_H_

#include "types.h"
#include "user.h"

// ===== TESTING MACROS =====
extern int TEST_PASS_COUNT;
extern int TEST_FAIL_COUNT;

#define ASSERT_INT_EQUALS(actual, expected, test_name) \
    do { \
        int a = (actual); \
        int e = (expected); \
        if (a == e) { \
            printf("PASS: %s\n", test_name); \
            TEST_PASS_COUNT++; \
        } else { \
            printf("FAIL: %s (Expected %d, Got %d)\n", test_name, e, a); \
            TEST_FAIL_COUNT++; \
        } \
    } while (0)

#define ASSERT_FLOAT_EQUALS(actual, expected, tolerance, test_name) \
    do { \
        float a = (actual); \
        float e = (expected); \
        float diff = (a > e) ? a - e : e - a; \
        if (diff <= tolerance) { \
            printf("PASS: %s\n", test_name); \
            TEST_PASS_COUNT++; \
        } else { \
            printf("FAIL: %s (Expected %.6f, Got %.6f, Diff %.6f)\n", test_name, e, a, diff); \
            TEST_FAIL_COUNT++; \
        } \
    } while (0)

#define ASSERT_STR_EQUALS(actual, expected, test_name) \
    do { \
        char* a = (actual); \
        char* e = (expected); \
        if (strcmp(a, e) == 0) { \
            printf("PASS: %s\n", test_name); \
            TEST_PASS_COUNT++; \
        } else { \
            printf("FAIL: %s (Expected '%s', Got '%s')\n", test_name, e, a); \
            TEST_FAIL_COUNT++; \
        } \
    } while (0)

#define TEST_SUITE_START(suite_name) \
    printf("\n=== RUNNING TEST SUITE: %s ===\n", suite_name);

#define TEST_SUITE_END() \
    printf("=== PASSED: %d | FAILED: %d ===\n\n", TEST_PASS_COUNT, TEST_FAIL_COUNT);

// Function prototypes
void run_math_tests(void);
void run_string_tests(void);
void run_stdlib_tests(void);
void print_final_summary(void);

#endif // _TEST_MILESTONE2_H_