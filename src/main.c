#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>

#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include <emmintrin.h>
#include <immintrin.h>

typedef int32_t i32;
typedef i32 b32;
typedef uint32_t u32;
typedef uint64_t u64;
typedef double f64;

#define get_cycles() __rdtsc()

#define BENCHMARK(name) void name( f64 *results, i32 count )
typedef BENCHMARK(Benchmark);

typedef enum Flags {
    Flags_None = 0,
    Flags_No_Check = (1 << 0),
    Flags_ARM      = (1 << 1),
    Flags_x86      = (1 << 2),
} Flags;

#define BENCH_WARMUP_COUNT 10
#define BENCH_RESULT_COUNT 10

typedef struct Benchmark_Entry {
    const char *name;
    Benchmark *benchmark;
    u32 flags;
    f64 mean;
    f64 std_deviation;
    f64 skew;
    f64 kurtosis;
    f64 results[BENCH_RESULT_COUNT];
} Benchmark_Entry;

typedef __m128i u64x2;
typedef __m128d f64x2;
typedef __m256i u64x4;
typedef __m256d f64x4;

#include "rng.c"
#include "inverse_cdf.c"
#include "boxmuller.c"
#include "irwin_hall_distribution.c"
#include "svml.c"

BENCHMARK(bench_memset)
{
    memset(results, 0, count * sizeof(*results));
}

BENCHMARK(bench_rand_u64)
{
    Rng rng = new_rng();

    for (int i = 0; i < count; i++) {
        u64 u = rand_u64(&rng);
        results[i] = *(f64 *)&u; 
    }
}

BENCHMARK(bench_rand_u64x4)
{
    Rng_x4 rng = new_rng_x4();

    for (int i = 0; i < count; i += 4) {
        u64x4 *p = (u64x4 *) (results + i);
        u64x4 u = rand_u64x4(&rng);
        _mm256_store_si256(p, u);
    }
}

BENCHMARK(bench_rand_f64)
{
    Rng rng = new_rng();

    for (int i = 0; i < count; i++) {
        results[i] = rand_f64(&rng);
    }
}

BENCHMARK(bench_rand_f64x4)
{
    Rng_x4 rng = new_rng_x4();

    for (int i = 0; i < count; i += 4) {
        f64 *p = results + i;
        f64x4 u = rand_f64x4(&rng);
        _mm256_store_pd(p, u);
    }
}

Benchmark_Entry entries[] = {
    { "Memset",                    bench_memset,                    Flags_No_Check | Flags_ARM | Flags_x86 },
    { "Rand u64",                  bench_rand_u64,                  Flags_No_Check | Flags_ARM | Flags_x86 },
    { "Rand u64x4",                bench_rand_u64x4,                Flags_No_Check |             Flags_x86 },
    { "Rand f64",                  bench_rand_f64,                  Flags_No_Check | Flags_ARM | Flags_x86 },
    { "Rand f64x4",                bench_rand_f64x4,                Flags_No_Check |             Flags_x86 },
    { "Inverse Normal CDF",        bench_inverse_normal_cdf,                         Flags_ARM | Flags_x86 },
    { "Inverse Normal CDF 2",      bench_inverse_normal_cdf_2,                       Flags_ARM | Flags_x86 },
    { "Boxmuller",                 bench_boxmuller,                                  Flags_ARM | Flags_x86 },
    { "Irwin Hall (12 draws)",     irwin_hall_distribution_12,                       Flags_ARM | Flags_x86 },
    { "Irwin Hall (12 draws) AVX", irwin_hall_distribution_12_avx2,                              Flags_x86 },
    { "Irwin Hall (16 draws)",     irwin_hall_distribution_16,                       Flags_ARM | Flags_x86 },
    { "Intel SVML",                bench_svml,                                                   Flags_x86 },
    { "Intel SVML SSE",            bench_svml_sse,                                               Flags_x86 },
};

void emit_entry_results( FILE *file, Benchmark_Entry *entry )
{
    f64 sum = 0;
    f64 min = DBL_MAX;
    f64 max = -DBL_MAX;

    for (int i = 0; i < BENCH_RESULT_COUNT; i++) {
        f64 cycles = entry->results[i];
        if (cycles < min) min = cycles;
        if (cycles > max) max = cycles;
        sum += cycles;
    }

    f64 average = sum / BENCH_RESULT_COUNT;
    f64 variance = 0;
    
    for (int i = 0; i < BENCH_RESULT_COUNT; i++) {
        f64 delta = entry->results[i] - average;
        f64 deviation = delta * delta;
        variance += deviation;
    }

    variance /= BENCH_RESULT_COUNT;

    f64 std_deviation = sqrt(variance);

    fprintf(file,
        "%s,%.2lf,%.2lf,%.2lf,%.2lf,%.6lf,%.6lf,%.6lf,%.6lf\n",
        entry->name, average, min, max, std_deviation,
        entry->mean, entry->std_deviation, entry->skew, entry->kurtosis);
}

void emit_benchmark_results( FILE *file )
{
    fprintf(file, "algorithm,cycles,min,max,cycles_stddev,mean,stddev,skew,kurtosis\n");

    i32 count = sizeof(entries) / sizeof(entries[0]);
    for (i32 i = 0; i < count; i++) {
        Benchmark_Entry *entry = entries + i;
        emit_entry_results(file, entry);
    }
}

void check_distribution( Benchmark_Entry *entry, f64 *samples, i32 sample_count )
{
    f64 sum = 0;
    f64 min = DBL_MAX;
    f64 max = -DBL_MAX;

    for (i32 i = 0; i < sample_count; i++) {
        if (samples[i] < min) min = samples[i];
        if (samples[i] < max) max = samples[i];
        sum += samples[i];
    }

    f64 mean = sum / sample_count;

    f64 variance = 0;
    for (i32 i = 0; i < sample_count; i++) {
        f64 delta = samples[i] - mean;
        variance += delta * delta;
    }

    f64 std_deviation = sqrt(variance / sample_count);

    f64 skew = 0;
    for (i32 i = 0; i < sample_count; i++) {
        f64 term = (samples[i] - mean) / std_deviation;
        skew += term * term * term;
    }

    skew /= sample_count;

    f64 kurtosis = 0;
    for (i32 i = 0; i < sample_count; i++) {
        f64 delta = samples[i] - mean;
        kurtosis += (delta * delta) * (delta * delta);
    }

    kurtosis = (kurtosis / sample_count) / 
        ((std_deviation * std_deviation) * (std_deviation * std_deviation)) - 3.0;

    entry->mean = mean;
    entry->std_deviation = std_deviation;
    entry->skew = skew;
    entry->kurtosis = kurtosis;
}

void run_benchmarks( f64 *samples, i32 sample_count )
{
    i32 count = sizeof(entries) / sizeof(entries[0]);
    for (i32 i = 0; i < count; i++) {
        Benchmark_Entry *entry = entries + i;

        for (int j = 0; j < BENCH_WARMUP_COUNT; j++) {
            entry->benchmark(samples, sample_count);
        }
        
        for (int j = 0; j < BENCH_RESULT_COUNT; j++) {
            u64 cycles_start = get_cycles();
            entry->benchmark(samples, sample_count);
            u64 cycles_end = get_cycles();

            f64 cycles = cycles_end - cycles_start;
            f64 cycles_per_sample = cycles / (f64)(sample_count);

            entry->results[j] = cycles_per_sample;
        }

        if ((entry->flags & Flags_No_Check) == 0) {
            entry->benchmark(samples, sample_count);
            check_distribution(entry, samples, sample_count);
            
            char path[256];
            snprintf(path, sizeof(path), "report/%s.csv", entry->name);

            FILE *f = fopen(path, "w");
            if (f) {
               fprintf(f, "value\n");
                for (int i = 0; i < sample_count; i++) {
                    fprintf(f, "%.15lf\n", samples[i]);
                }
                fclose(f); 
            }
        }
    }

    emit_benchmark_results(stdout);
}

#include <assert.h>

int main( void )
{
    i32 sample_count = 1e6;
    f64 *sample_buffer = aligned_alloc(32, sample_count * sizeof(*sample_buffer));

    run_benchmarks(sample_buffer, sample_count);

    return 0;
}