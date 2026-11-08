// T01-03: sequential vs stride(16) read of the same uint32_t[1,000,000].
#define _POSIX_C_SOURCE 200809L  // expose clock_gettime under -std=c11 on Linux
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000u
#define STRIDE 16u
#define SCANS 50      // full-array scans per timed sample
#define SAMPLES 31
#define WARMUP 5      // warm-up samples per kernel (discarded)

// noinline so the compiler cannot fold the two kernels together or hoist across calls.
__attribute__((noinline)) static uint64_t seq_scan(const uint32_t *data) {
    uint64_t sum = 0;
    for (size_t i = 0; i < N; i++) sum += data[i];
    return sum;
}

__attribute__((noinline)) static uint64_t stride_scan(const uint32_t *data) {
    uint64_t sum = 0;
    for (size_t offset = 0; offset < STRIDE; offset++)
        for (size_t i = offset; i < N; i += STRIDE) sum += data[i];
    return sum;
}

typedef uint64_t (*kernel_t)(const uint32_t *);

static double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Runs SCANS scans; returns elapsed seconds, accumulates checksum.
static double sample(kernel_t k, const uint32_t *data, uint64_t *checksum) {
    uint64_t total = 0;
    double t0 = now_sec();
    for (int r = 0; r < SCANS; r++) total += k(data);
    double t1 = now_sec();
    *checksum = total;
    return t1 - t0;
}

static int cmp_d(const void *a, const void *b) {
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}

int main(void) {
    uint32_t *data = malloc(N * sizeof *data);
    if (!data) return 1;
    uint64_t expected = 0;                       // independent reference sum
    for (size_t i = 0; i < N; i++) {
        data[i] = (uint32_t)(i * 2654435761u) ^ (uint32_t)(i >> 3);
        expected += data[i];
    }
    uint64_t expected_total = expected * SCANS;

    kernel_t k[2] = {seq_scan, stride_scan};
    const char *name[2] = {"A_sequential", "B_stride16"};
    double t[2][SAMPLES];
    uint64_t cs;

    for (int w = 0; w < WARMUP; w++)
        for (int j = 0; j < 2; j++) sample(k[j], data, &cs);

    int ok = 1;
    for (int s = 0; s < SAMPLES; s++) {
        for (int p = 0; p < 2; p++) {
            int j = (s % 2) ? 1 - p : p;         // alternate A,B / B,A
            t[j][s] = sample(k[j], data, &cs);
            if (cs != expected_total) { ok = 0; fprintf(stderr, "CHECKSUM MISMATCH %s\n", name[j]); }
        }
    }

    printf("N=%u scans/sample=%d samples=%d expected_scan_checksum=%llu\n",
           N, SCANS, SAMPLES, (unsigned long long)expected);
    printf("checksum_all_match=%s\n", ok ? "yes" : "NO");
    puts("# raw samples (ms per sample): index A B");
    for (int s = 0; s < SAMPLES; s++) printf("%d %.4f %.4f\n", s, t[0][s]*1e3, t[1][s]*1e3);
    double acc = (double)N * SCANS;
    for (int j = 0; j < 2; j++) {
        double v[SAMPLES];
        for (int s = 0; s < SAMPLES; s++) v[s] = t[j][s];
        qsort(v, SAMPLES, sizeof *v, cmp_d);
        double mean = 0; for (int s = 0; s < SAMPLES; s++) mean += v[s]; mean /= SAMPLES;
        double med = v[SAMPLES/2];
        printf("%s median=%.4f ms min=%.4f max=%.4f mean=%.4f  median ns/access=%.4f  (min %.4f, max %.4f)\n",
               name[j], med*1e3, v[0]*1e3, v[SAMPLES-1]*1e3, mean*1e3,
               med/acc*1e9, v[0]/acc*1e9, v[SAMPLES-1]/acc*1e9);
    }
    free(data);
    return ok ? 0 : 2;
}
