#include "user_types.h"
#include "user/user.h"
#include "xv6_stdlib.h"

// CALLOC 
void* xv6_calloc(int num, int size) {
    if (num <= 0 || size <= 0)
        return 0;

    unsigned long total = (unsigned long)num * (unsigned long)size;
    if (total == 0 || total > 0x7fffffff)
        return 0;

    void* ptr = malloc((int)total);
    if (!ptr) return 0;

    unsigned char* p = (unsigned char*)ptr;
    for (unsigned long i = 0; i < total; i++)
        p[i] = 0;

    return ptr;
}

// BSEARCH 
void* xv6_bsearch(const void* key, const void* base, int num, int size,
                  int (*compare)(const void*, const void*)) {
    if (!key || !base || num <= 0 || size <= 0 || !compare)
        return 0;

    const unsigned char* arr = (const unsigned char*)base;
    int left = 0, right = num - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        const unsigned char* mid_ptr = arr + mid * size;
        int cmp = compare(key, mid_ptr);
        if (cmp == 0) return (void*)mid_ptr;
        if (cmp < 0) right = mid - 1;
        else left = mid + 1;
    }
    return 0;
}

// QSORT 
static void swap_elements(unsigned char* a, unsigned char* b, int size) {
    if (a == b) return;
    for (int i = 0; i < size; i++) {
        unsigned char tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

static void insertion_sort(unsigned char* arr, int low, int high, int size,
                           int (*compare)(const void*, const void*)) {
    for (int i = low + 1; i <= high; i++) {
        int j = i;
        while (j > low) {
            unsigned char* cur = arr + j * size;
            unsigned char* prev = arr + (j - 1) * size;
            if (compare(prev, cur) <= 0) break;
            swap_elements(prev, cur, size);
            j--;
        }
    }
}

static int partition(unsigned char* arr, int low, int high, int size,
                     int (*compare)(const void*, const void*)) {
    unsigned char* pivot = arr + high * size;
    int i = low;
    for (int j = low; j < high; j++) {
        if (compare(arr + j * size, pivot) < 0) {
            swap_elements(arr + i * size, arr + j * size, size);
            i++;
        }
    }
    swap_elements(arr + i * size, arr + high * size, size);
    return i;
}

static void quicksort_recursive(unsigned char* arr, int low, int high, int size,
                                int (*compare)(const void*, const void*)) {
    while (low < high) {
        if (high - low <= 8) {
            insertion_sort(arr, low, high, size, compare);
            return;
        }

        int p = partition(arr, low, high, size, compare);
        if (p - low < high - p) {
            quicksort_recursive(arr, low, p - 1, size, compare);
            low = p + 1;
        } else {
            quicksort_recursive(arr, p + 1, high, size, compare);
            high = p - 1;
        }
    }
}

void xv6_qsort(void* base, int num, int size,
               int (*compare)(const void*, const void*)) {
    if (!base || num <= 1 || size <= 0 || !compare) return;
    quicksort_recursive((unsigned char*)base, 0, num - 1, size, compare);
}

// ATOI 
int xv6_atoi(const char* str) {
    if (!str) return 0;

    int i = 0, sign = 1, result = 0;
    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') i++;

    if (str[i] == '-') { sign = -1; i++; }
    else if (str[i] == '+') { i++; }

    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return sign * result;
}

// ATOF
float xv6_atof(const char* str) {
    if (!str) return 0.0f;

    int i = 0, sign = 1, int_part = 0, frac_part = 0, frac_count = 0;

    while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') i++;

    if (str[i] == '-') { sign = -1; i++; }
    else if (str[i] == '+') { i++; }

    while (str[i] >= '0' && str[i] <= '9') { int_part = int_part * 10 + (str[i] - '0'); i++; }

    if (str[i] == '.') {
        i++;
        while (str[i] >= '0' && str[i] <= '9' && frac_count < 6) {
            frac_part = frac_part * 10 + (str[i] - '0');
            frac_count++; i++;
        }
        while (str[i] >= '0' && str[i] <= '9') i++;
    }

    float result = (float)int_part;
    if (frac_count > 0) {
        float divisor = 1.0f;
        for (int j = 0; j < frac_count; j++) divisor *= 10.0f;
        result += (float)frac_part / divisor;
    }

    if (str[i] == 'e' || str[i] == 'E') {
        i++;
        int exp_sign = 1, exponent = 0;
        if (str[i] == '-') { exp_sign = -1; i++; }
        else if (str[i] == '+') i++;
        while (str[i] >= '0' && str[i] <= '9' && exponent < 30) {
            exponent = exponent * 10 + (str[i] - '0'); i++;
        }
        for (int j = 0; j < exponent; j++) {
            if (exp_sign > 0) result *= 10.0f;
            else result /= 10.0f;
        }
    }

    return sign * result;
}
