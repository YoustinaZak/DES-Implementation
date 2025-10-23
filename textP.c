#include "des_ip_lut.h"
#include "des_pc1_lut.h"
#include "des_pc2_lut.h"
#include "des_exp_lut.h"
#include "des_p_lut.h"
#include "des_fp_lut.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>

uint64_t init_permutation(uint64_t* input,uint64_t* output) {
    for (int byte = 0; byte < 8; ++byte) {
        uint8_t val = (*input >> ((7 - byte) * 8)) & 0xFF;
        *output |= DES_IP_LUT[byte][val];
    }
}

void pc1(uint64_t* key, uint64_t* output){
    for (int i = 0; i < 8; ++i)
    *output |= DES_PC1_LUT[i][(*key >> ((7 - i) * 8)) & 0xFF];
}

void pc2(uint64_t* key, uint64_t* output){
for (int i = 0; i < 7; ++i)
    *output |= DES_PC2_LUT[i][(*key >> ((6 - i) * 8)) & 0xFF];
}
void expand(uint32_t* R, uint64_t* output){
    for (int byte = 0; byte < 4; ++byte) {
        *output |= DES_E_LUT[byte][(*R >> ((3 - byte) * 8)) & 0xFF];
    }
}
void perm(uint32_t* input, uint32_t* output){
    for (int byte = 0; byte < 4; ++byte) {
        *output |= DES_P_LUT[byte][(*input >> ((3 - byte) * 8)) & 0xFF];
    }
}
void final_perm(uint64_t* input){
    uint64_t output = 0;
    for (int byte = 0; byte < 8; ++byte) {
        uint8_t val = (*input >> ((7 - byte) * 8)) & 0xFF;
        output |= DES_FP_LUT[byte][val];
    }
}
int main (){
    uint64_t plaintext = 0x133457799BBCDFF1;
    uint32_t in =0x12345678;
    uint32_t ciphertext=0;
    const int N = 10;
    clock_t start = clock();
    uint64_t x=0;
    uint64_t t;
    for (int i = 0; i < N; i++) {
        perm(&in, &ciphertext);
    }
       // t=0;
        //x=i;
    // Swap bits across the 32-bit halves
    /**
        t = ((x >> 4) ^ x) & 0x0F0F0F0F0F0F0F0FULL; x ^= t ^ (t << 4);
        t = ((x >> 16) ^ x) & 0x0000FFFF0000FFFFULL; x ^= t ^ (t << 16);
        t = ((x >> 2) ^ x) & 0x3333333333333333ULL; x ^= t ^ (t << 2);
        t = ((x >> 8) ^ x) & 0x00FF00FF00FF00FFULL; x ^= t ^ (t << 8);
        t = ((x >> 1) ^ x) & 0x5555555555555555ULL; x ^= t ^ (t << 1);
        x = (x << 32) | (x >> 32);
        */
    

    clock_t end = clock();

    double avg_time = ((double)(end - start) / CLOCKS_PER_SEC*1000.0);

    printf("Ciphertext: %016llX\n", (unsigned long long)ciphertext);
    //printf("Ciphertext: %016llX\n", (unsigned long long)t);
    printf("Average time: %.9f milliseconds per run\n", avg_time);
    
   // uint64_t key=0x133457799BBCDFF1ULL;
   /*
    uint64_t key2=0xF0CCAAF556678FULL;
    uint64_t result=0;
    const int N = 100000;
    clock_t start = clock();
    for (int i = 0; i < N; i++) {
        pc2(&key2,&result);
    }

    clock_t end = clock();

    double avg_time = ((double)(end - start) / CLOCKS_PER_SEC*1000.0) / N;
    */
    //printf("key56: %016llX\n", (unsigned long long)result);
    //printf("Average time: %.9f milliseconds per run\n", avg_time);
    return 0;
}