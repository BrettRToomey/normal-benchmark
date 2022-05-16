BENCHMARK(irwin_hall_distribution_12)
{
    Rng rng = new_rng();

    for (int i = 0; i < count; i++) {
        f64 sum = rand_f64(&rng);

        sum += rand_f64(&rng);
        sum += rand_f64(&rng);
        sum += rand_f64(&rng);
        sum += rand_f64(&rng);
        sum += rand_f64(&rng);
        sum += rand_f64(&rng);
        sum += rand_f64(&rng);
        sum += rand_f64(&rng);
        sum += rand_f64(&rng);
        sum += rand_f64(&rng);
        sum += rand_f64(&rng);

        results[i] = sum - 6;
    }
}

BENCHMARK(irwin_hall_distribution_12_avx2)
{
    Rng_x4 rng = new_rng_x4();

    f64x4 six = _mm256_set1_pd(6.0);

    for (int i = 0; i < count; i += 4) {
        f64x4 sum = rand_f64x4(&rng);

        for (int j = 1; j < 12; j++) {
            sum = _mm256_add_pd(sum, rand_f64x4(&rng));
        }

        f64 *p = results + i;
        _mm256_store_pd(p, _mm256_sub_pd(sum, six));
    }

}

BENCHMARK(irwin_hall_distribution_16)
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