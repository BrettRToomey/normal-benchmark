// The RNG used is Xorshift128+

typedef struct Rng {
    u64 s0;
    u64 s1;
} Rng;

typedef struct __attribute__((aligned(16))) Rng_x2 {
    u64x2 s0;
    u64x2 s1;
} Rng_x2;

typedef struct __attribute__((aligned(32))) Rng_x4 {
    u64x4 s0;
    u64x4 s1;
} Rng_x4;

Rng new_rng( void )
{
    Rng result = {0};
    result.s0 = 0x462f64212d132fee;
    result.s1 = 0x42a5adbcc0b99488;
    return result;
}

Rng_x2 new_rng_x2( void )
{
    Rng_x2 result = {0};
    result.s0 = _mm_set_epi64x(0xdc8d15e35704ef07, 0x462f64212d132fee);
    result.s1 = _mm_set_epi64x(0xcdb9cfa193afd75e, 0x42a5adbcc0b99488);
    return result;
}

Rng_x4 new_rng_x4( void )
{
    Rng_x4 result = {0};
    result.s0 = _mm256_set_epi64x(0x439ba8681dd32623, 0x220f59b20dc77641, 0xdc8d15e35704ef07, 0x462f64212d132fee);
    result.s1 = _mm256_set_epi64x(0x5ad612f8f37e5165, 0x39c32e960221768a, 0xcdb9cfa193afd75e, 0x42a5adbcc0b99488);
    return result;
}

u64 rand_u64( Rng *state )
{
    u64 s0 = state->s0;
    u64 s1 = state->s1;

    u64 r = s0 + s1;

    s1 ^= s1 << 23;
    state->s1 = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);

    return r;
}

u64x2 rand_u64x2( Rng_x2 *state )
{
    u64x2 s0 = state->s0;
    u64x2 s1 = state->s1;

    u64x2 r = _mm_add_epi64(s0, s1);

    // s1 ^= s1 << 23;
    s1 = _mm_xor_si128(s1, _mm_slli_epi64(s1, 23));

    // s1 = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
    s1 = _mm_xor_si128(
        s1,
        _mm_xor_si128(
            s0,
            _mm_xor_si128(
                _mm_srli_epi64(s1, 18), 
                _mm_srli_epi64(s0, 5))));

    state->s1 = s1;

    return r;
}

u64x4 rand_u64x4( Rng_x4 *state )
{
    u64x4 s0 = state->s0;
    u64x4 s1 = state->s1;

    u64x4 r = _mm256_add_epi64(s0, s1);

    // s1 ^= s1 << 23;
    s1 = _mm256_xor_si256(s1, _mm256_slli_epi64(s1, 23));

    // s1 = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
    s1 = _mm256_xor_si256(
        s1,
        _mm256_xor_si256(
            s0,
            _mm256_xor_si256(
                _mm256_srli_epi64(s1, 18), 
                _mm256_srli_epi64(s0, 5))));

    state->s1 = s1;

    return r;
}

f64x2 rand_f64x2( Rng_x2 *state )
{
    u64x2 s0 = state->s0;
    u64x2 s1 = state->s1;

    u64x2 r = _mm_add_epi64(s0, s1);

    // s1 ^= s1 << 23;
    s1 = _mm_xor_si128(s1, _mm_slli_epi64(s1, 23));

    // s1 = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
    s1 = _mm_xor_si128(
        s1,
        _mm_xor_si128(
            s0,
            _mm_xor_si128(
                _mm_srli_epi64(s1, 18), 
                _mm_srli_epi64(s0, 5))));

    state->s1 = s1;

    f64x2 rf = _mm_or_si128(
        _mm_set1_epi64x(UINT64_C(0x3FF) << 52),
        _mm_srli_epi64(r, 12));

    rf = _mm_sub_pd((f64x2)rf, _mm_set1_pd(1.0));

    return rf;
}

f64x4 rand_f64x4( Rng_x4 *state )
{
    u64x4 s0 = state->s0;
    u64x4 s1 = state->s1;

    u64x4 r = _mm256_add_epi64(s0, s1);

    // s1 ^= s1 << 23;
    s1 = _mm256_xor_si256(s1, _mm256_slli_epi64(s1, 23));

    // s1 = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
    s1 = _mm256_xor_si256(
        s1,
        _mm256_xor_si256(
            s0,
            _mm256_xor_si256(
                _mm256_srli_epi64(s1, 18), 
                _mm256_srli_epi64(s0, 5))));

    state->s1 = s1;

    f64x4 rf = _mm256_or_si256(
        _mm256_set1_epi64x(UINT64_C(0x3FF) << 52),
        _mm256_srli_epi64(r, 12));

    rf = _mm256_sub_pd((f64x4)rf, _mm256_set1_pd(1.0));
    return rf;
}

f64 rand_f64( Rng *state )
{
    u64 s0 = state->s0;
    u64 s1 = state->s1;

    u64 r = s0 + s1;

    s1 ^= s1 << 23;
    state->s1 = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);

    const union { u64 i; f64 d; }
    u = { .i = UINT64_C(0x3FF) << 52 | r >> 12 };
    return u.d - 1.0;
}