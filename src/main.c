#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

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
	Flags_ARM  = (1 << 0),
	Flags_x86  = (1 << 1),
} Flags;

#define BENCH_RESULT_COUNT 10

typedef struct Benchmark_Entry {
	const char *name;
	Benchmark *benchmark;
	u32 flags;
	f64 results[BENCH_RESULT_COUNT];
} Benchmark_Entry;

typedef __m256i u64x4;

#include "rng.c"
#include "inverse_cdf.c"
#include "boxmuller.c"

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
		u64x4 *p = (u64x4 *) results + i;
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

Benchmark_Entry entries[] = {
	{ "Memset",             bench_memset,             Flags_ARM | Flags_x86 },
	{ "Rand u64",           bench_rand_u64,           Flags_ARM | Flags_x86 },
	{ "Rand u64x4",         bench_rand_u64x4,                     Flags_x86 },
	{ "Rand f64",           bench_rand_f64,           Flags_ARM | Flags_x86 },
	{ "Inverse Normal CDF", bench_inverse_normal_cdf, Flags_ARM | Flags_x86 },
	{ "Boxmuller",          bench_boxmuller,          Flags_ARM | Flags_x86 },
};

void run_benchmarks( f64 *samples, i32 sample_count )
{
	i32 count = sizeof(entries) / sizeof(entries[0]);
	for (i32 i = 0; i < count; i++) {
		Benchmark_Entry *entry = entries + i;

		f64 sum = 0.0;
		for (int j = 0; j < BENCH_RESULT_COUNT; j++) {
			u64 cycles_start = get_cycles();
			entry->benchmark(samples, sample_count);
			u64 cycles_end = get_cycles();

			f64 cycles = cycles_end - cycles_start;
			f64 cycles_per_sample = cycles / (f64)(sample_count);

			entry->results[j] = cycles_per_sample;
			sum += cycles_per_sample;
		}

		u64 cpu_clock_speed = 3200000000; /* M1 clock speed is 3,2Ghz */
		f64 avg_cycles_per_sample = sum / BENCH_RESULT_COUNT;
		f64 samples_per_second = cpu_clock_speed / avg_cycles_per_sample;
		f64 bytes_per_second = samples_per_second * sizeof(*samples);

		printf("%s:\n"
			   	"  Average cycles per sample: %.2lf, %.2lf/bps\n",
				entry->name, avg_cycles_per_sample, bytes_per_second);
	}
}

int main( void )
{
	i32 sample_count = 1e6;
	f64 *sample_buffer = aligned_alloc(32, sample_count * sizeof(*sample_buffer));
	run_benchmarks(sample_buffer, sample_count);
	return 0;
}