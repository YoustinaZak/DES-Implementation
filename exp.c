#include <stdint.h>
#include <stdio.h>

int main() {
    int E[48] = {
        32, 1, 2, 3, 4, 5,
         4, 5, 6, 7, 8, 9,
         8, 9,10,11,12,13,
        12,13,14,15,16,17,
        16,17,18,19,20,21,
        20,21,22,23,24,25,
        24,25,26,27,28,29,
        28,29,30,31,32, 1
    };

    uint64_t LUT[4][256] = {0};

    for (int byte = 0; byte < 4; ++byte) {
        for (int val = 0; val < 256; ++val) {
            uint64_t result = 0;
            for (int bit = 0; bit < 8; ++bit) {
                int global_bit = byte * 8 + bit; // input bit index (0–31)
                for (int out = 0; out < 48; ++out) { // 48 output bits
                    if (E[out] - 1 == global_bit) {
                        int bit_val = (val >> (7 - bit)) & 1;
                        result |= ((uint64_t)bit_val << (47 - out));
                    }
                }
            }
            LUT[byte][val] = result;
        }
    }

    // Print
    printf("static const uint64_t DES_E_LUT[4][256] = {\n");
    for (int i = 0; i < 4; i++) {
        printf("  {\n");
        for (int j = 0; j < 256; j++) {
            printf("    0x%012llXULL%s", (unsigned long long)LUT[i][j],
                   (j == 255 ? "" : ","));
            if ((j + 1) % 4 == 0) printf("\n");
        }
        printf("  }%s\n", (i == 3 ? "" : ","));
    }
    printf("};\n");

    return 0;
}
