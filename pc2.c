#include <stdio.h>
#include <stdint.h>

int main() {
    // DES Permuted Choice 2 (PC-2)
    int PC2[48] = {
        14, 17, 11, 24,  1,  5,
         3, 28, 15,  6, 21, 10,
        23, 19, 12,  4, 26,  8,
        16,  7, 27, 20, 13,  2,
        41, 52, 31, 37, 47, 55,
        30, 40, 51, 45, 33, 48,
        44, 49, 39, 56, 34, 53,
        46, 42, 50, 36, 29, 32
    };

    uint64_t LUT[8][256] = {0};

    // Build lookup tables
    for (int byte = 0; byte < 8; ++byte) {
        for (int val = 0; val < 256; ++val) {
            uint64_t result = 0;
            for (int bit = 0; bit < 8; ++bit) {
                int global_bit = byte * 8 + bit; // position in the 56-bit input
                for (int out = 0; out < 48; ++out) {
                    if (PC2[out] - 1 == global_bit) {
                        int bit_val = (val >> (7 - bit)) & 1;
                        result |= ((uint64_t)bit_val << (47 - out));
                    }
                }
            }
            LUT[byte][val] = result;
        }
    }

    // Print the lookup table
    printf("static const uint64_t DES_PC2_LUT[8][256] = {\n");
    for (int i = 0; i < 8; i++) {
        printf("  {\n");
        for (int j = 0; j < 256; j++) {
            printf("    0x%012llXULL%s", (unsigned long long)LUT[i][j],
                   (j == 255 ? "" : ","));
            if ((j + 1) % 4 == 0) printf("\n");
        }
        printf("  }%s\n", (i == 7 ? "" : ","));
    }
    printf("};\n");

    return 0;
}
