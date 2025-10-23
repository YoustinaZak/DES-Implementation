#include <stdio.h>
#include <stdint.h>

int main() {
    // DES P-box permutation
    int P[32] = {
        16,  7, 20, 21,
        29, 12, 28, 17,
         1, 15, 23, 26,
         5, 18, 31, 10,
         2,  8, 24, 14,
        32, 27,  3,  9,
        19, 13, 30,  6,
        22, 11,  4, 25
    };

    uint32_t LUT[4][256] = {0};

    // Build the lookup table
    for (int byte = 0; byte < 4; ++byte) {
        for (int val = 0; val < 256; ++val) {
            uint32_t result = 0;
            for (int bit = 0; bit < 8; ++bit) {
                int global_bit = byte * 8 + bit;
                for (int out = 0; out < 32; ++out) {
                    if (P[out] - 1 == global_bit) {
                        int bit_val = (val >> (7 - bit)) & 1;
                        result |= (bit_val << (31 - out));
                    }
                }
            }
            LUT[byte][val] = result;
        }
    }

    // Print the full lookup table in C format
    printf("static const uint32_t DES_P_LUT[4][256] = {\n");
    for (int i = 0; i < 4; ++i) {
        printf("  {\n");
        for (int j = 0; j < 256; ++j) {
            printf("    0x%08XULL%s", LUT[i][j], (j == 255 ? "" : ","));
            if ((j + 1) % 4 == 0) printf("\n");
        }
        printf("  }%s\n", (i == 3 ? "" : ","));
    }
    printf("};\n");

    return 0;
}
