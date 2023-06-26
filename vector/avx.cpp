#include <iostream>
#include <x86intrin.h>
#include <fstream>
#include <chrono>
 
 
#define AVX512_FUNCTION_SPECIFIC_ATTRIBUTE __attribute__((target("sse,sse2,sse3,ssse3,sse4,popcnt,avx,avx2,avx512f")))
#define AVX2_FUNCTION_SPECIFIC_ATTRIBUTE __attribute__((target("sse,sse2,sse3,ssse3,sse4,popcnt,avx,avx2")))
#define AVX_FUNCTION_SPECIFIC_ATTRIBUTE __attribute__((target("sse,sse2,sse3,ssse3,sse4,popcnt,avx"))
#define SSE42_FUNCTION_SPECIFIC_ATTRIBUTE __attribute__((target("sse,sse2,sse3,ssse3,sse4,popcnt")))
 
using namespace std;
 
struct StringView {
    const char* p;
    const size_t len;
};
 
StringView FileSize(const char* fileName) {
    ifstream ifstr(fileName);
    const auto b = ifstr.tellg();
    ifstr.seekg(0, ios::end);
    const auto e = ifstr.tellg();
    const size_t fileSize = e - b;
    ifstr.seekg(0, ios::beg);
    char *p = new char[fileSize];
    ifstr.read(p, fileSize);
    return {p, fileSize};
}
 
// Normal function
size_t count_c_normal(const StringView& str, const uint8_t c) {
    uint32_t num = 0;
    for (uint32_t i = 0; i < str.len; ++i) {
        if (c == *(str.p + i)) {
            ++num;
        }
    }
    return num;
}
 
// SIMD function
AVX512_FUNCTION_SPECIFIC_ATTRIBUTE size_t count_c_simd(const StringView& str, const uint8_t c) {
    __m128i ch = _mm_set1_epi8(c); // char ch[16] = { c, c, ..., c }
    size_t cnt = 0;
    uint32_t i = 0;
    for (; i < str.len; i+=16) {
        // char t[16] = { (str+i)[0], (str+i)[1], ... }
        __m128i t = _mm_loadu_si128((__m128i *)(str.p + i));
        __m128i res = _mm_cmpeq_epi8(t, ch);
 
        // res[16] = { 0xFF, 0x00, 0xFF ... }
        unsigned mask = _mm_movemask_epi8(res);
 
        // bits[16] = 0...1101
        cnt += __builtin_popcount(mask);
    }
 
    // free cnt .
    for (; i < str.len; ++i) {
        if (c == *(str.p + i))
        {
            ++cnt;
        }
    }
    return cnt;
}
 
// AVX function
AVX2_FUNCTION_SPECIFIC_ATTRIBUTE size_t count_c_avx256(const StringView& str, const uint8_t c) {
    __m256i ch = _mm256_set1_epi8(c); // char ch[16] = { c, c, ..., c }
    size_t cnt = 0;
    uint32_t i = 0;
    for (; i < str.len; i+=32) {
        // char t[16] = { (str+i)[0], (str+i)[1], ... }
        __m256i t = _mm256_loadu_si256((__m256i *)(str.p + i));
        __m256i res = _mm256_cmpeq_epi8(t, ch);
 
        // res[16] = { 0xFF, 0x00, 0xFF ... }
        unsigned mask = _mm256_movemask_epi8(res);
 
        // bits[16] = 0...1101
        cnt += __builtin_popcount(mask);
    }
 
    // free cnt .
    for (; i < str.len; ++i) {
        if (c == *(str.p + i))
        {
            ++cnt;
        }
    }
    return cnt;
}
 
int main() {
    const auto ret = FileSize("../test_file");
    size_t cnt1 = 0, cnt2 = 0, cnt3 = 0;
    const auto t1 = std::chrono::steady_clock::now();
    cnt1 = count_c_normal(ret, uint8_t('1'));
    const auto t2 = std::chrono::steady_clock::now();
    cnt2 = count_c_simd(ret, uint8_t('1'));
    const auto t3 = std::chrono::steady_clock::now();
    cnt3 = count_c_avx256(ret, uint8_t('1'));
    const auto t4 = std::chrono::steady_clock::now();
 
    std::cout << "cnt1: " << cnt1 << std::endl;
    std::cout << "cnt2: " << cnt2 << std::endl;
    std::cout << "cnt3: " << cnt3 << std::endl;
 
    std::cout << "-----------" << std::endl;
 
    const auto d1 = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
    const auto d2 = std::chrono::duration_cast<std::chrono::milliseconds>(t3-t2).count();
    const auto d3 = std::chrono::duration_cast<std::chrono::milliseconds>(t4-t3).count();
 
    std::cout << "NORMAL: " << d1 << std::endl;
    std::cout << "SIMD: " << d2 << std::endl;
    std::cout << "AVX: " << d3 << std::endl;
        
    return 0;
}