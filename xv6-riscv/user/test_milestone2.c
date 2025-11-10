// user/test_milestone2.c
#include "test_milestone2.h"

// Global test counters
int TEST_PASS_COUNT = 0;
int TEST_FAIL_COUNT = 0;

void print_final_summary(void) {
    printf("\n");
    printf("=================================\n");
    printf("    MILESTONE 2 TEST SUMMARY\n");
    printf("=================================\n");
    printf("TOTAL PASSED: %d\n", TEST_PASS_COUNT);
    printf("TOTAL FAILED: %d\n", TEST_FAIL_COUNT);
    printf("=================================\n");
    
    if (TEST_FAIL_COUNT == 0) {
        printf("ALL TESTS PASSED! ✅\n");
    } else {
        printf("SOME TESTS FAILED! ❌\n");
    }
    printf("=================================\n");
}

int main(int argc, char *argv[]) {
    printf("Starting Milestone 2 Comprehensive Tests...\n");
    
    // Reset global counters
    TEST_PASS_COUNT = 0;
    TEST_FAIL_COUNT = 0;
    
    // Run all test suites
    run_math_tests();
    run_string_tests(); 
    run_stdlib_tests();
    
    // Print final summary
    print_final_summary();
    
    exit(0);
}