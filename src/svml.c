BENCHMARK(bench_svml)
{
#ifdef __INTEL_COMPILER
	Rng_x4 rng = new_rng_x4();

	for (int i = 0; i < count; i += 4) {
		f64 *p = results + i;
		f64x4 u = rand_f64x4(&rng);
		f64x4 r = _mm256_cdfnorminv_pd(u);
        _mm256_store_pd(p, r);
	}
#endif
}

BENCHMARK(bench_svml_sse)
{
#ifdef __INTEL_COMPILER
	Rng_x2 rng = new_rng_x2();

	for (int i = 0; i < count; i += 2) {
		f64 *p = results + i;
		f64x2 u = rand_f64x2(&rng);
		f64x2 r = _mm_cdfnorminv_pd(u);
        _mm_store_pd(p, r);
	}
#endif
}