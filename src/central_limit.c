BENCHMARK(bench_central_limit_4)
{
	f64 inv = 1.0 / sqrt(4 / 12.0);

	Rng rng = new_rng();

	for (int i = 0; i < count; i++) {
		f64 sum = rand_f64(&rng);
		
		for (int j = 1; j < 4; j++) {
			sum += rand_f64(&rng);
		}

		results[i] = (sum - 4 / 2.0) * inv;
	}
}

BENCHMARK(bench_central_limit_8)
{
	f64 inv = 1.0 / sqrt(8 / 12.0);

	Rng rng = new_rng();

	for (int i = 0; i < count; i++) {
		f64 sum = rand_f64(&rng);
		
		for (int j = 1; j < 8; j++) {
			sum += rand_f64(&rng);
		}

		results[i] = (sum - 8 / 2.0) * inv;
	}
}

BENCHMARK(bench_central_limit_16)
{
	f64 inv = 1.0 / sqrt(16 / 12.0);

	Rng rng = new_rng();

	for (int i = 0; i < count; i++) {
		f64 sum = rand_f64(&rng);
		
		for (int j = 1; j < 16; j++) {
			sum += rand_f64(&rng);
		}

		results[i] = (sum - 16 / 2.0) * inv;
	}
}