#include <stdio.h>
#include <stdint.h>

int main(void) {
    // DES Final Permutation (Inverse of Initial Permutation)
    const int FP[64] = {
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9, 49, 17, 57, 25
    };

    uint64_t LUT[8][256] = {0};

    // Build lookup tables
    for (int byte = 0; byte < 8; ++byte) {
        for (int val = 0; val < 256; ++val) {
            uint64_t result = 0;
            for (int bit = 0; bit < 8; ++bit) {
                int global_bit = byte * 8 + bit; // input bit index (0–63)
                for (int out = 0; out < 64; ++out) {
                    if (FP[out] - 1 == global_bit) {
                        int bit_val = (val >> (7 - bit)) & 1;
                        result |= ((uint64_t)bit_val << (63 - out));
                    }
                }
            }
            LUT[byte][val] = result;
        }
    }

    // Print the complete lookup table in C array format
    printf("static const uint64_t DES_FP_LUT[8][256] = {\n");
    for (int i = 0; i < 8; i++) {
        printf("  {\n");
        for (int j = 0; j < 256; j++) {
            printf("    0x%016llXULL%s", (unsigned long long)LUT[i][j],
                   (j == 255 ? "" : ","));
            if ((j + 1) % 4 == 0) printf("\n");
        }
        printf("  }%s\n", (i == 7 ? "" : ","));
    }
    printf("};\n");

    return 0;
}
