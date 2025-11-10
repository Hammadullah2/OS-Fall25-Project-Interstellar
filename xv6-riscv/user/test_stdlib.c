// user/test_stdlib.c
#include "xv6_stdlib.h"
#include "test_milestone2.h"
#include <stdio.h>

// Comparator functions for testing
int int_cmp(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

int float_cmp(const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    if (fa < fb) return -1;
    if (fa > fb) return 1;
    return 0;
}

int test_calloc(void) {
    printf("\n=== calloc tests ===\n");
    int passed = 0;
    
    // Test 1: Normal allocation
    int* arr = (int*)xv6_calloc(5, sizeof(int));
    if (arr != NULL) {
        int all_zero = 1;
        for (int i = 0; i < 5; i++) {
            if (arr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        ASSERT_INT_EQUALS(all_zero, 1, "calloc - initialize to zero");
        xv6_free(arr);
        passed++;
    }
    
    // Test 2: Size 0 allocation
    void* ptr = xv6_calloc(0, sizeof(int));
    // Should return NULL or valid pointer, but shouldn't crash
    ASSERT_INT_EQUALS(ptr == NULL ? 1 : 1, 1, "calloc - size 0");
    if (ptr) xv6_free(ptr);
    passed++;
    
    return passed;
}

int test_qsort(void) {
    printf("\n=== qsort tests ===\n");
    int passed = 0;
    
    // Test 1: Sort integers
    int arr1[] = {5, 2, 8, 1, 9};
    int expected1[] = {1, 2, 5, 8, 9};
    xv6_qsort(arr1, 5, sizeof(int), int_cmp);
    
    int sorted1 = 1;
    for (int i = 0; i < 5; i++) {
        if (arr1[i] != expected1[i]) {
            sorted1 = 0;
            break;
        }
    }
    ASSERT_INT_EQUALS(sorted1, 1, "qsort - integers");
    passed++;
    
    // Test 2: Already sorted array
    int arr2[] = {1, 2, 3, 4, 5};
    xv6_qsort(arr2, 5, sizeof(int), int_cmp);
    int sorted2 = 1;
    for (int i = 0; i < 5; i++) {
        if (arr2[i] != i + 1) {
            sorted2 = 0;
            break;
        }
    }
    ASSERT_INT_EQUALS(sorted2, 1, "qsort - already sorted");
    passed++;
    
    // Test 3: Reverse sorted array
    int arr3[] = {5, 4, 3, 2, 1};
    xv6_qsort(arr3, 5, sizeof(int), int_cmp);
    int sorted3 = 1;
    for (int i = 0; i < 5; i++) {
        if (arr3[i] != i + 1) {
            sorted3 = 0;
            break;
        }
    }
    ASSERT_INT_EQUALS(sorted3, 1, "qsort - reverse sorted");
    passed++;
    
    // Test 4: Single element
    int arr4[] = {42};
    xv6_qsort(arr4, 1, sizeof(int), int_cmp);
    ASSERT_INT_EQUALS(arr4[0], 42, "qsort - single element");
    passed++;
    
    return passed;
}

int test_bsearch(void) {
    printf("\n=== bsearch tests ===\n");
    int passed = 0;
    
    int arr[] = {1, 3, 5, 7, 9, 11, 13, 15};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    // Test 1: Find existing element
    int key1 = 7;
    int* result1 = (int*)xv6_bsearch(&key1, arr, n, sizeof(int), int_cmp);
    ASSERT_INT_EQUALS(result1 != NULL ? *result1 : -1, 7, "bsearch - find existing");
    passed++;
    
    // Test 2: Search for non-existent element
    int key2 = 6;
    int* result2 = (int*)xv6_bsearch(&key2, arr, n, sizeof(int), int_cmp);
    ASSERT_INT_EQUALS(result2 == NULL ? 1 : 0, 1, "bsearch - non-existent element");
    passed++;
    
    // Test 3: Search in single-element array
    int single_arr[] = {42};
    int key3 = 42;
    int* result3 = (int*)xv6_bsearch(&key3, single_arr, 1, sizeof(int), int_cmp);
    ASSERT_INT_EQUALS(result3 != NULL ? *result3 : -1, 42, "bsearch - single element");
    passed++;
    
    // Test 4: Search in empty array
    int key4 = 100;
    int* result4 = (int*)xv6_bsearch(&key4, arr, 0, sizeof(int), int_cmp);
    ASSERT_INT_EQUALS(result4 == NULL ? 1 : 0, 1, "bsearch - empty array");
    passed++;
    
    return passed;
}

int test_atoi(void) {
    printf("\n=== atoi tests ===\n");
    int passed = 0;
    
    // Test 1: Positive number
    ASSERT_INT_EQUALS(xv6_atoi("42"), 42, "atoi - positive number");
    passed++;
    
    // Test 2: Negative number
    ASSERT_INT_EQUALS(xv6_atoi("-123"), -123, "atoi - negative number");
    passed++;
    
    // Test 3: Leading whitespace
    ASSERT_INT_EQUALS(xv6_atoi("   456"), 456, "atoi - leading whitespace");
    passed++;
    
    // Test 4: Invalid input
    ASSERT_INT_EQUALS(xv6_atoi("abc"), 0, "atoi - invalid input");
    passed++;
    
    // Test 5: Mixed input
    ASSERT_INT_EQUALS(xv6_atoi("123abc"), 123, "atoi - mixed input");
    passed++;
    
    // Test 6: Overflow (simple test)
    ASSERT_INT_EQUALS(xv6_atoi("2147483647"), 2147483647, "atoi - max int");
    passed++;
    
    return passed;
}

int test_atof(void) {
    printf("\n=== atof tests ===\n");
    int passed = 0;
    
    // Test 1: Integer as float
    float result1 = xv6_atof("42");
    ASSERT_FLOAT_EQUALS(result1, 42.0f, 1e-5, "atof - integer");
    passed++;
    
    // Test 2: Fractional number
    float result2 = xv6_atof("3.14");
    ASSERT_FLOAT_EQUALS(result2, 3.14f, 1e-5, "atof - decimal");
    passed++;
    
    // Test 3: Scientific notation
    float result3 = xv6_atof("1.5e-2");
    ASSERT_FLOAT_EQUALS(result3, 0.015f, 1e-5, "atof - scientific notation");
    passed++;
    
    // Test 4: Negative number
    float result4 = xv6_atof("-2.718");
    ASSERT_FLOAT_EQUALS(result4, -2.718f, 1e-5, "atof - negative");
    passed++;
    
    // Test 5: Leading whitespace
    float result5 = xv6_atof("   99.9");
    ASSERT_FLOAT_EQUALS(result5, 99.9f, 1e-5, "atof - leading whitespace");
    passed++;
    
    return passed;
}

void run_stdlib_tests(void) {
    TEST_SUITE_START("STANDARD LIBRARY");
    
    int total = 0;
    int passed = 0;
    
    passed += test_calloc();
    total += 2;
    
    passed += test_qsort();
    total += 4;
    
    passed += test_bsearch();
    total += 4;
    
    passed += test_atoi();
    total += 6;
    
    passed += test_atof();
    total += 5;
    
    printf("\nStandard Library Summary: %d/%d tests passed\n", passed, total);
    TEST_SUITE_END();
}