#include "xv6_math.h"
#include <stdio.h>
#include <math.h>
#include <float.h>

// Include the generated test vectors
#include "math_test_vectors.c"

#define EPSILON 1e-5f
#define fabsf_local(x) ((x) < 0 ? -(x) : (x))

static void test_results(const char *name, float max_err, float avg_err, int pass, int total) {
    printf("%s: max_error=%.8e avg_error=%.8e pass=%d/%d\n",
           name, max_err, avg_err, pass, total);
}

int main(void) {
    printf("=== xv6 Math Function Tests ===\n");

    // sqrtf test
    {
        float max_err = 0, avg_err = 0;
        int pass = 0;
        for (int i=0; i<NUM_SQRT; i++) {
            float out = xv6_sqrtf(sqrt_inputs[i]);
            float exp = sqrt_expected[i];
            float err;
            if (isnan(exp) && isnan(out)) err = 0;
            else err = fabsf_local(out - exp);
            avg_err += err;
            if (err < EPSILON || (isnan(exp) && isnan(out))) pass++;
            if (err > max_err) max_err = err;
        }
        avg_err /= NUM_SQRT;
        test_results("sqrtf", max_err, avg_err, pass, NUM_SQRT);
    }

    // expf test
    {
        float max_err = 0, avg_err = 0;
        int pass = 0;
        for (int i=0; i<NUM_EXP; i++) {
            float out = xv6_expf(exp_inputs[i]);
            float exp = exp_expected[i];
            float err;
            if (isnan(exp) && isnan(out)) err = 0;
            else err = fabsf_local(out - exp);
            avg_err += err;
            if (err < EPSILON || (isnan(exp) && isnan(out))) pass++;
            if (err > max_err) max_err = err;
        }
        avg_err /= NUM_EXP;
        test_results("expf", max_err, avg_err, pass, NUM_EXP);
    }

    // powf test
    {
        float max_err = 0, avg_err = 0;
        int pass = 0;
        for (int i=0; i<NUM_POW; i++) {
            float out = xv6_powf(pow_x[i], pow_y[i]);
            float exp = pow_expected[i];
            float err;
            if (isnan(exp) && isnan(out)) err = 0;
            else err = fabsf_local(out - exp);
            avg_err += err;
            if (err < EPSILON || (isnan(exp) && isnan(out))) pass++;
            if (err > max_err) max_err = err;
        }
        avg_err /= NUM_POW;
        test_results("powf", max_err, avg_err, pass, NUM_POW);
    }

    // sinf test
    {
        float max_err = 0, avg_err = 0;
        int pass = 0;
        for (int i=0; i<NUM_TRIG; i++) {
            float out = xv6_sinf(sin_inputs[i]);
            float exp = sin_expected[i];
            float err = fabsf_local(out - exp);
            avg_err += err;
            if (err < EPSILON) pass++;
            if (err > max_err) max_err = err;
        }
        avg_err /= NUM_TRIG;
        test_results("sinf", max_err, avg_err, pass, NUM_TRIG);
    }

    // cosf test
    {
        float max_err = 0, avg_err = 0;
        int pass = 0;
        for (int i=0; i<NUM_TRIG; i++) {
            float out = xv6_cosf(cos_inputs[i]);
            float exp = cos_expected[i];
            float err = fabsf_local(out - exp);
            avg_err += err;
            if (err < EPSILON) pass++;
            if (err > max_err) max_err = err;
        }
        avg_err /= NUM_TRIG;
        test_results("cosf", max_err, avg_err, pass, NUM_TRIG);
    }

    // fabsf test
    {
        float max_err = 0, avg_err = 0;
        int pass = 0;
        for (int i=0; i<NUM_FABS; i++) {
            float out = xv6_fabsf(fabs_inputs[i]);
            float exp = fabs_expected[i];
            float err = fabsf_local(out - exp);
            avg_err += err;
            if (err < EPSILON) pass++;
            if (err > max_err) max_err = err;
        }
        avg_err /= NUM_FABS;
        test_results("fabsf", max_err, avg_err, pass, NUM_FABS);
    }

    printf("=== Tests completed ===\n");
    return 0;
}
