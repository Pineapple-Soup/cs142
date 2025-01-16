#include <immintrin.h>

extern "C" void fastsqrt(const float* in,  float* out, unsigned count) {
    _mm_prefetch((const char*)&in[0], _MM_HINT_T0);
    __m512 two = _mm512_set1_ps(2.0f);
    for (unsigned j = 0; j + 63 < count; j += 64) {
        __m512 n0 = _mm512_loadu_ps(&in[j]);
        __m512 n1 = _mm512_loadu_ps(&in[j+16]);
        __m512 n2 = _mm512_loadu_ps(&in[j+32]);
        __m512 n3 = _mm512_loadu_ps(&in[j+48]);

        // Initial guess
        __m512i i0 = _mm512_castps_si512(n0);
        i0 = _mm512_srli_epi32(i0, 1);
        i0 = _mm512_add_epi32(i0, _mm512_set1_epi32(0x1FBD1DF5));
        __m512 y0 = _mm512_castsi512_ps(i0);
        __m512i i1 = _mm512_castps_si512(n1);
        i1 = _mm512_srli_epi32(i1, 1);
        i1 = _mm512_add_epi32(i1, _mm512_set1_epi32(0x1FBD1DF5));
        __m512 y1 = _mm512_castsi512_ps(i1);
        __m512i i2 = _mm512_castps_si512(n2);
        i2 = _mm512_srli_epi32(i2, 1);
        i2 = _mm512_add_epi32(i2, _mm512_set1_epi32(0x1FBD1DF5));
        __m512 y2 = _mm512_castsi512_ps(i2);
        __m512i i3 = _mm512_castps_si512(n3);
        i3 = _mm512_srli_epi32(i3, 1);
        i3 = _mm512_add_epi32(i3, _mm512_set1_epi32(0x1FBD1DF5));
        __m512 y3 = _mm512_castsi512_ps(i3);

        // Heron's method
        y0 = _mm512_div_ps(_mm512_add_ps(y0, _mm512_div_ps(n0, y0)), two);
        y1 = _mm512_div_ps(_mm512_add_ps(y1, _mm512_div_ps(n1, y1)), two);
        y2 = _mm512_div_ps(_mm512_add_ps(y2, _mm512_div_ps(n2, y2)), two);        
        y3 = _mm512_div_ps(_mm512_add_ps(y3, _mm512_div_ps(n3, y3)), two);

        // Newton's method
        y0 = _mm512_sub_ps(y0, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y0, y0), n0), _mm512_mul_ps(y0, two)));
        y1 = _mm512_sub_ps(y1, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y1, y1), n1), _mm512_mul_ps(y1, two)));
        y2 = _mm512_sub_ps(y2, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y2, y2), n2), _mm512_mul_ps(y2, two)));
        y3 = _mm512_sub_ps(y3, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y3, y3), n3), _mm512_mul_ps(y3, two)));
        
        y0 = _mm512_sub_ps(y0, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y0, y0), n0), _mm512_mul_ps(y0, two)));
        y1 = _mm512_sub_ps(y1, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y1, y1), n1), _mm512_mul_ps(y1, two)));
        y2 = _mm512_sub_ps(y2, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y2, y2), n2), _mm512_mul_ps(y2, two)));
        y3 = _mm512_sub_ps(y3, _mm512_div_ps(_mm512_sub_ps(_mm512_mul_ps(y3, y3), n3), _mm512_mul_ps(y3, two)));

        _mm512_storeu_ps(&out[j], y0);
        _mm512_storeu_ps(&out[j+16], y1);
        _mm512_storeu_ps(&out[j+32], y2);
        _mm512_storeu_ps(&out[j+48], y3);

        _mm_prefetch((const char*)&in[j + 64], _MM_HINT_T0);
    }
}