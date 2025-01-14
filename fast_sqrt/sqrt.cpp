#include <immintrin.h>

extern "C" void fastsqrt(const float* in,  float* out, unsigned count) {
    for (unsigned j = 0; j + 15 < count; j += 16) {
        _mm_prefetch((const char*)&in[j + 16], _MM_HINT_T0);

        __m512 n = _mm512_loadu_ps(&in[j]);
        __m512 two = _mm512_set1_ps(2.0f);

        // Initial guess
        __m512i i = _mm512_castps_si512(n);
        i = _mm512_srli_epi32(i, 1);
        i = _mm512_add_epi32(i, _mm512_set1_epi32(0x1FBB473E));
        __m512 y = _mm512_castsi512_ps(i);

        // Heron's method
        y = _mm512_div_ps(_mm512_add_ps(y, _mm512_div_ps(n, y)), two);

        // Newton's method;
        y = _mm512_sub_ps(y, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y, y), n), _mm512_mul_ps(y, two)));
        y = _mm512_sub_ps(y, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y, y), n), _mm512_mul_ps(y, two)));

        _mm512_storeu_ps(&out[j], y);
    }
}