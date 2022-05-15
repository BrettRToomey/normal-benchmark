// Beasley-Springer-Moro algorithm for inverse CDF
// in Glasserman (2003) "Monte Carlo Methods in Financial Engineering", p 67,
// works with Sobol generator.
BENCHMARK(bench_inverse_normal_cdf)
{
    Rng rng = new_rng();

    for (i32 i = 0; i < count; i++) {
        // FIXME(BRETT): this needs to be normalized
        f64 u = rand_f64(&rng);

        b32 sup = u > 0.5;
        f64 up = sup ? 1.0 - u : u;

        f64 a[] = { 2.50662823884, -18.61500062529, 41.39119773534, -25.44106049637 };
        f64 b[] = { -8.47351093090, 23.08336743743, -21.06224101826, 3.13082909833 };
        f64 c[] = {
            0.3374754822726147, 0.9761690190917186, 0.1607979714918209, 0.0276438810333863,
            0.0038405729373609, 0.0003951896511919, 0.0000321767881768, 0.0000002888167364,
            0.0000003960315187
        };

        f64 x = up - 0.5;
        f64 r;

        if ( fabs(x) < 0.42 )
        {
            r = x*x;
            r = x*(((a[3]*r + a[2])*r + a[1])*r + a[0])
                / ((((b[3]*r + b[2])*r + b[1])*r + b[0])*r + 1.0);

            results[count] = sup ? -r: r;
            continue;
        }

        r = up;
        r = log( -log(r) );
        r = c[0] + r*(c[1] + r*(c[2] + r*(c[3] + r*(c[4] + r*(c[5] + r*(c[6] + r*(c[7] + r*c[8])))))));

        results[count] = sup? r: -r;
    }
}