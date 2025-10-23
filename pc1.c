#include <stdio.h>
#include <stdint.h>

int main() {
    // DES Permuted Choice 1 (PC-1)
    int PC1[56] = {
        57, 49, 41, 33, 25, 17, 9,
         1, 58, 50, 42, 34, 26, 18,
        10,  2, 59, 51, 43, 35, 27,
        19, 11,  3, 60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15,
         7, 62, 54, 46, 38, 30, 22,
        14,  6, 61, 53, 45, 37, 29,
        21, 13,  5, 28, 20, 12,  4
    };

    uint64_t LUT[8][256] = {0};

    // Build lookup tables
    for (int byte = 0; byte < 8; ++byte) {
        for (int val = 0; val < 256; ++val) {
            uint64_t result = 0;
            for (int bit = 0; bit < 8; ++bit) {
                int global_bit = byte * 8 + bit; // bit index in 64-bit input
                for (int out = 0; out < 56; ++out) {
                    if (PC1[out] - 1 == global_bit) {
                        int bit_val = (val >> (7 - bit)) & 1;
                        result |= ((uint64_t)bit_val << (55 - out));
                    }
                }
            }
            LUT[byte][val] = result;
        }
    }

    // Print the complete lookup table
    printf("static const uint64_t DES_PC1_LUT[8][256] = {\n");
    for (int i = 0; i < 8; i++) {
        printf("  {\n");
        for (int j = 0; j < 256; j++) {
            printf("    0x%014llXULL%s", (unsigned long long)LUT[i][j],
                   (j == 255 ? "" : ","));
            if ((j + 1) % 4 == 0) printf("\n");
        }
        printf("  }%s\n", (i == 7 ? "" : ","));
    }
    printf("};\n");

    return 0;
}
