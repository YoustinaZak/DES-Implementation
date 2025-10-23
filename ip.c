#include <stdio.h>
#include <stdint.h>

int main() {
    // DES Initial Permutation (IP)
    int IP[64] = {
        58, 50, 42, 34, 26, 18, 10,  2,
        60, 52, 44, 36, 28, 20, 12,  4,
        62, 54, 46, 38, 30, 22, 14,  6,
        64, 56, 48, 40, 32, 24, 16,  8,
        57, 49, 41, 33, 25, 17,  9,  1,
        59, 51, 43, 35, 27, 19, 11,  3,
        61, 53, 45, 37, 29, 21, 13,  5,
        63, 55, 47, 39, 31, 23, 15,  7
    };

    uint64_t LUT[8][256] = {0};

    // Build lookup tables
    for (int byte = 0; byte < 8; ++byte) {
        for (int val = 0; val < 256; ++val) {
            uint64_t result = 0;
            for (int bit = 0; bit < 8; ++bit) {
                int global_bit = byte * 8 + bit;
                for (int out = 0; out < 64; ++out) {
                    if (IP[out] - 1 == global_bit) {
                        int bit_val = (val >> (7 - bit)) & 1;
                        result |= ((uint64_t)bit_val << (63 - out));
                    }
                }
            }
            LUT[byte][val] = result;
        }
    }

    // Print the complete lookup table in C array format
    printf("static const uint64_t DES_IP_LUT[8][256] = {\n");
    for (int i = 0; i < 8; i++) {
        printf("  {\n");
        for (int j = 0; j < 256; j++) {
            printf("    0x%016llXULL%s", (unsigned long long)LUT[i][j],
                   (j == 255 ? "" : ","));
            if ((j + 1) % 4 == 0) printf("\n"); // group 4 per line for readability
        }
        printf("  }%s\n", (i == 7 ? "" : ","));
    }
    printf("};\n");

    return 0;
}
