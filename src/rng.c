// The RNG used is Xorshift128+

typedef struct Rng {
    u64 s0;
    u64 s1;
} Rng;

typedef struct __attribute__((aligned(32))) Rng_x4 {
    u64x4 s0;
    u64x4 s1;
} Rng_x4;

Rng new_rng( void )
{
    Rng result = {0};
    result.s0 = 0xababab;
    result.s1 = 0xbababa;
    return result;
}

Rng_x4 new_rng_x4( void )
{
    Rng_x4 result = {0};
    result.s0 = _mm256_set1_epi64x(0xababab);
    result.s1 = _mm256_set1_epi64x(0xbababa);
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