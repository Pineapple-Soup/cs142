#include <immintrin.h>

extern "C" void fastsqrt(const float* in,  float* out, unsigned count) {
    for (unsigned j = 0; j + 15 < count; j += 16) {
        __m512 n = _mm512_loadu_ps(&in[j]);

        // Initial guess
        __m512i i = _mm512_castps_si512(n);
        i = _mm512_srli_epi32(i, 1);
        i = _mm512_add_epi32(i, _mm512_set1_epi32(0x1FBB473E));
        __m512 y = _mm512_castsi512_ps(i);

        // Heron's method
        y = _mm512_div_ps(_mm512_add_ps(y, _mm512_div_ps(n, y)), _mm512_set1_ps(2.0f));

        // Newton's method
        y = _mm512_sub_ps(y, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y, y), n), _mm512_mul_ps(y, _mm512_set1_ps(2.0f))));
        y = _mm512_sub_ps(y, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y, y), n), _mm512_mul_ps(y, _mm512_set1_ps(2.0f))));
 
        // Halley's Method
        // __m512 y2 = _mm512_mul_ps(y, y);
        // __m512 y2_n = _mm512_sub_ps(y2, n);
        // __m512 numerator = _mm512_mul_ps(_mm512_mul_ps(y2_n, y), _mm512_set1_ps(4.0f));
        // __m512 denominator = _mm512_sub_ps(_mm512_mul_ps(y2, _mm512_set1_ps(8.0f)), _mm512_mul_ps(y2_n, _mm512_set1_ps(2.0f)));
        // y = _mm512_sub_ps(y, _mm512_div_ps(numerator, denominator));

        _mm512_storeu_ps(&out[j], y);
    }
}