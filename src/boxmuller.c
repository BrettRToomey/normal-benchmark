BENCHMARK(bench_boxmuller)
{
	Rng rng = new_rng();

	f64 tau = 3.14159265358979323846 * 2.0;

	for (int i = 0; i < count; i += 2) {
		f64 u = 1.0 - rand_f64(&rng);
		f64 v = rand_f64(&rng);
		f64 radius = sqrt(-2 * log(u));
		f64 theta = tau * v;

		results[i + 0] = radius * cos(theta);
		results[i + 1] = radius * sin(theta);
	}
}