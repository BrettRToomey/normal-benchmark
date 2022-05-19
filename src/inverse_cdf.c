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

            results[i] = sup ? -r: r;
            continue;
        }

        r = up;
        r = log( -log(r) );
        r = c[0] + r*(c[1] + r*(c[2] + r*(c[3] + r*(c[4] + r*(c[5] + r*(c[6] + r*(c[7] + r*c[8])))))));

        results[i] = sup? r: -r;
    }
}

BENCHMARK(bench_inverse_normal_cdf_2)
{
    f64 a1 = -39.69683028665376;
    f64 a2 = 220.9460984245205;
    f64 a3 = -275.9285104469687;
    f64 a4 = 138.3577518672690;
    f64 a5 = -30.66479806614716;
    f64 a6 = 2.506628277459239;

    f64 b1 = -54.47609879822406;
    f64 b2 = 161.5858368580409;
    f64 b3 = -155.6989798598866;
    f64 b4 = 66.80131188771972;
    f64 b5 = -13.28068155288572;

    f64 c1 = -0.007784894002430293;
    f64 c2 = -0.3223964580411365;
    f64 c3 = -2.400758277161838;
    f64 c4 = -2.549732539343734;
    f64 c5 = 4.374664141464968;
    f64 c6 = 2.938163982698783;

    f64 d1 = 0.007784695709041462;
    f64 d2 = 0.3224671290700398;
    f64 d3 = 2.445134137142996;
    f64 d4 = 3.754408661907416;

    Rng rng = new_rng();

    for (int i = 0; i < count; i++) {
        f64 u = rand_f64(&rng);

        if (u < 0.02425) {
            f64 q = sqrt(-2*log(u));
            results[i] = -(((((c1*q+c2)*q+c3)*q+c4)*q+c5)*q+c6) /
                ((((d1*q+d2)*q+d3)*q+d4)*q+1);
        } else if (u > 0.97575) {
            f64 q = sqrt(-2*log(1-u));
            results[i] = (((((c1*q+c2)*q+c3)*q+c4)*q+c5)*q+c6) /
                ((((d1*q+d2)*q+d3)*q+d4)*q+1);
        } else {
            f64 q = u - 0.5;
            f64 r = q * q;
            results[i] = (((((a1*r+a2)*r+a3)*r+a4)*r+a5)*r+a6)*q / 
                (((((b1*r+b2)*r+b3)*r+b4)*r+b5)*r+1);
        }
    }
}