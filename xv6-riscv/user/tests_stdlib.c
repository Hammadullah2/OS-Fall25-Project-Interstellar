#include "user_types.h"
#include "user/user.h"
#include "xv6_stdlib.h"

int total_tests = 0;
int passed_tests = 0;

void print_test_header(char* test_name) {
    printf("\n Testing:  %s \n", test_name);
}

void assert_test(int condition, char* test_description) {
    total_tests++;
    if (condition) {
        printf("PASS: %s\n", test_description);
        passed_tests++;
    } else {
        printf("FAIL: %s\n", test_description);
    }
}

// CALLOC TESTS 
void test_calloc_basic() {
    print_test_header("calloc - Basic Allocation");
    
    int* arr = (int*)xv6_calloc(5, sizeof(int));
    assert_test(arr != 0, "calloc returns non-null pointer");
    
    int all_zero = 1;
    for (int i = 0; i < 5; i++) {
        if (arr[i] != 0) all_zero = 0;
    }
    assert_test(all_zero, "calloc zeros all allocated memory");
    
    free(arr);
}

void test_calloc_size_zero() {
    print_test_header("calloc - Size Zero");
    
    void* ptr = xv6_calloc(0, sizeof(int));
    assert_test(ptr == 0, "calloc(0, size) returns null");
}

// QSORT TESTS 
int int_compare(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

int float_compare(const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    if (fa < fb) return -1;
    if (fa > fb) return 1;
    return 0;
}

int is_sorted_int(int* arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) return 0;
    }
    return 1;
}

int is_sorted_float(float* arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) return 0;
    }
    return 1;
}

void test_qsort_integers() {
    print_test_header("qsort - Integer Array");

    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 45, 50};
    xv6_qsort(arr, 10, sizeof(int), int_compare);
    assert_test(is_sorted_int(arr, 10), "qsort sorts integer array");
}

void test_qsort_floats() {
    print_test_header("qsort - Float Array");

    float arr[] = {3.14f, -1.0f, 2.71f, 0.0f, 42.0f};
    xv6_qsort(arr, 5, sizeof(float), float_compare);
    assert_test(is_sorted_float(arr, 5), "qsort sorts float array with comparator");
}

void test_qsort_already_sorted() {
    print_test_header("qsort - Already Sorted");

    int arr[] = {1, 2, 3, 4, 5};
    xv6_qsort(arr, 5, sizeof(int), int_compare);
    assert_test(is_sorted_int(arr, 5), "qsort leaves sorted array unchanged");
}

void test_qsort_reverse_sorted() {
    print_test_header("qsort - Reverse Sorted");

    int arr[] = {50, 40, 30, 20, 10};
    xv6_qsort(arr, 5, sizeof(int), int_compare);
    assert_test(is_sorted_int(arr, 5), "qsort sorts reverse order array");
}

void test_qsort_single_element() {
    print_test_header("qsort - Single Element");

    int arr[] = {42};
    xv6_qsort(arr, 1, sizeof(int), int_compare);
    assert_test(arr[0] == 42, "qsort handles single element array");
}

// BSEARCH TESTS 
void test_bsearch_existing() {
    print_test_header("bsearch - Existing Element");

    int arr[] = {2, 4, 6, 8, 10, 12};
    int key = 8;
    int* result = (int*)xv6_bsearch(&key, arr, 6, sizeof(int), int_compare);
    assert_test(result != 0 && *result == 8, "bsearch locates existing element");
}

void test_bsearch_missing() {
    print_test_header("bsearch - Non-Existent Element");

    int arr[] = {1, 3, 5, 7, 9};
    int key = 6;
    int* result = (int*)xv6_bsearch(&key, arr, 5, sizeof(int), int_compare);
    assert_test(result == 0, "bsearch returns null when element absent");
}

void test_bsearch_single() {
    print_test_header("bsearch - Single Element Array");

    int arr[] = {42};
    int key = 42;
    int* result = (int*)xv6_bsearch(&key, arr, 1, sizeof(int), int_compare);
    assert_test(result != 0 && *result == 42, "bsearch works with single element array");
}

void test_bsearch_empty() {
    print_test_header("bsearch - Empty Array");

    int key = 10;
    int dummy = 0;
    int* result = (int*)xv6_bsearch(&key, &dummy, 0, sizeof(int), int_compare);
    assert_test(result == 0, "bsearch returns null on empty array");
}

// ATOI TESTS 
void test_atoi_positive() {
    print_test_header("atoi - Positive Numbers");
    
    assert_test(xv6_atoi("123") == 123, "atoi converts '123' to 123");
}

void test_atoi_negative() {
    print_test_header("atoi - Negative Numbers");
    
    assert_test(xv6_atoi("-456") == -456, "atoi converts '-456' to -456");
}

void test_atoi_whitespace() {
    print_test_header("atoi - Leading Whitespace");
    
    assert_test(xv6_atoi("   789") == 789, "atoi skips leading whitespace");
}

void test_atoi_invalid() {
    print_test_header("atoi - Invalid Input");
    
    assert_test(xv6_atoi("abc") == 0, "atoi returns 0 for invalid input");
}

void test_atoi_overflow() {
    print_test_header("atoi - Overflow Input");
    
    int ov1 = xv6_atoi("2147483648");   // INT_MAX + 1
    int ov2 = xv6_atoi("-2147483649");  // INT_MIN - 1
    assert_test(ov1 != 0, "atoi handles positive overflow input");
    assert_test(ov2 != 0, "atoi handles negative overflow input");
}

// ATOF TESTS 
int float_equal(float a, float b, float tolerance) {
    float diff = a - b;
    if (diff < 0) diff = -diff;
    return diff < tolerance;
}

void test_atof_integer() {
    print_test_header("atof - Integer as Float");
    
    assert_test(float_equal(xv6_atof("123"), 123.0f, 0.01f), "atof converts integer string");
}

void test_atof_fractional() {
    print_test_header("atof - Fractional Number");
    
    assert_test(float_equal(xv6_atof("0.789"), 0.789f, 0.01f), "atof converts fractional number");
}

void test_atof_scientific() {
    print_test_header("atof - Scientific Notation");
    
    assert_test(float_equal(xv6_atof("1.5e-3"), 0.0015f, 0.0001f), "atof converts scientific notation");
}

void test_atof_negative() {
    print_test_header("atof - Negative Number");
    
    assert_test(float_equal(xv6_atof("-123.45"), -123.45f, 0.01f), "atof converts negative number");
}

void test_atof_leading_whitespace() {
    print_test_header("atof - Leading Whitespace");
    
    assert_test(float_equal(xv6_atof("   3.14"), 3.14f, 0.01f), "atof skips leading whitespace");
}

int main() {
    printf("   TASK 3C: STANDARD LIBRARY FUNCTIONS\n");
    printf("      Selected Test Suite\n");
    
    test_calloc_basic();
    test_calloc_size_zero();
    
    test_qsort_integers();
    test_qsort_floats();
    test_qsort_already_sorted();
    test_qsort_reverse_sorted();
    test_qsort_single_element();
    
    test_bsearch_existing();
    test_bsearch_missing();
    test_bsearch_single();
    test_bsearch_empty();
    
    test_atoi_positive();
    test_atoi_negative();
    test_atoi_whitespace();
    test_atoi_invalid();
    test_atoi_overflow();
    
    test_atof_integer();
    test_atof_fractional();
    test_atof_scientific();
    test_atof_negative();
    test_atof_leading_whitespace();
    
    printf(" TEST SUMMARY:  \n");
    printf("Total Tests:  %d\n", total_tests);
    printf("Passed:       %d\n", passed_tests);
    printf("Failed:       %d\n", total_tests - passed_tests);
    printf("Success Rate: %.1f%%\n", 
           (total_tests > 0) ? (100.0f * passed_tests / total_tests) : 0);
    
    exit(0);
}