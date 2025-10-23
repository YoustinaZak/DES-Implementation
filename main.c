#include <stdio.h>
#include <stdint.h>
#include "des_ip_lut.h"
#include "des_pc1_lut.h"
#include "des_pc2_lut.h"
#include "des_exp_lut.h"
#include "des_p_lut.h"
#include "des_fp_lut.h"

//#define ROTL(x, n, bits) ( ((x) << (n)) | ((x) >> ((bits) - (n))) ) //left circular shift
#define SWAP32(block) ( (((block) & 0xFFFFFFFFULL) << 32) | (((block) >> 32) & 0xFFFFFFFFULL) )
#define DES_LEFT_CIRCULAR_SHIFT28(value, shift) ( (( ( value & 0x0FFFFFFF ) << shift ) | ( ( value& 0x0FFFFFFF ) >> (28 - shift) )) & 0x0FFFFFFF )

const int SBOX[8][4][16] = {
    // S1
    {
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    },
    // S2
    {
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    // S3
    {
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    // S4
    {
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    },
    // S5
    {
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    },
    // S6
    {
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    },
    // S7
    {
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    },
    // S8
    {
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
    }
};
static const int DES_SHIFT_SCHEDULE[16] = {
    1, 1, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 2, 2, 1
};
enum {
    encrypt,
    decrypt
} op;
void Left_Circular_Shift(uint64_t* key56, int round) {
    // Extract 28-bit halves
    uint32_t C = (*key56 >> 28) & 0x0FFFFFFF;
    uint32_t D = *key56 & 0x0FFFFFFF;

    // Get the shift amount for this round
    int shift = DES_SHIFT_SCHEDULE[round - 1];
    C = DES_LEFT_CIRCULAR_SHIFT28(C, shift);
    D = DES_LEFT_CIRCULAR_SHIFT28(D, shift);

    // Combine back into a 56-bit key
    uint64_t combined = (((uint64_t)C) << 28) | (uint64_t)D;
    *key56= combined & 0x00FFFFFFFFFFFFFFULL; // ensure 56 bits only
}
//data processing blocks
uint64_t  InitialPermutation(uint64_t* block, uint64_t* output)
{
    for (int byte = 0; byte < 8; ++byte) {
        uint8_t val = (*block >> ((7 - byte) * 8)) & 0xFF;
        *output |= DES_IP_LUT[byte][val];
    }
}
uint64_t FinalPermutation(uint64_t* block, uint64_t* output)
{
    for (int byte = 0; byte < 8; ++byte) {
        *output |= DES_FP_LUT[byte][(*block >> ((7 - byte) * 8)) & 0xFF];
    }
}
uint64_t Swap32(uint64_t block)
{
    uint64_t left = (block >> 32) & 0xFFFFFFFFULL;
    uint64_t right = block & 0xFFFFFFFFULL;
    return (right << 32) | left;
}

//Key schedule blocks
void PermutationChoice1(uint64_t* key)
{
    uint64_t output=0;
    for (int i = 0; i < 8; ++i)
    output |= DES_PC1_LUT[i][(*key >> ((7 - i) * 8)) & 0xFF];
    *key = output;
}
uint64_t PermutationChoice2(uint64_t key)
{
    uint64_t output=0;
    for (int i = 0; i < 7; ++i)
    output |= DES_PC2_LUT[i][(key >> ((6 - i) * 8)) & 0xFF];
    return output;
}
void ExpansionPermutation(uint32_t* halfblock, uint64_t* output)
{
    for (int byte = 0; byte < 4; ++byte) {
        *output |= DES_P_LUT[byte][(*halfblock >> ((3 - byte) * 8)) & 0xFF];
    }
}

uint32_t SBoxSubstitution(uint64_t block)
{

}

void Permutation(uint32_t* block, uint32_t* output)
{
    for (int byte = 0; byte < 4; ++byte) {
        *output |= DES_P_LUT[byte][(*block >> ((3 - byte) * 8)) & 0xFF];
    }
}
//big blocks
/*uint32_t FeistelFunction(uint32_t halfblock,uint64_t key)
{
    uint64_t temp=ExpansionPermutation(halfblock);
    temp=temp^key;//xor
    uint32_t out =SBoxSubstitution(temp);
    out=Permutation(out);
    return out;

}*/

/*uint64_t Round(uint64_t block,uint64_t key)
{
    uint32_t inleft= (block >>32) & 0xFFFFFFFF;
    uint32_t inright= block & 0xFFFFFFFF;

    uint32_t outright= inleft ^ FeistelFunction(inright,key);
    uint32_t outleft= inright;
    uint64_t out= ((uint64_t)outleft <<32) | (uint64_t)outright;

    return out;


}*/

//Main Blocks
void KeySchedule(uint64_t* key,uint64_t  subkeys[16])
{
    PermutationChoice1(key);
    printf("Key after PC1: %016llX \n",*key);
    for (int i=0;i<16;i++)
    {
        Left_Circular_Shift(key,i);
        subkeys[i]= PermutationChoice2(*key);
    }

}
/*uint64_t DES_Encrypt(uint64_t* block,uint64_t subkeys[16])
{
    uint64_t ip_block=0;
    InitialPermutation(block, &ip_block);

    for (int i=0;i<16;i++)
    {
        block= Round(block,subkeys[i]);
    }

    block= Swap32(block);

    block= FinalPermutation(block);

    return block;
}*/

/*uint64_t DES_Decrypt(uint64_t block,uint64_t subkeys[16])
{
    block= InitialPermutation(block);

    for (int i=15;i>=0;i--)
    {
        block= Round(block,subkeys[i]);
    }

    block= Swap32(block);

    block= FinalPermutation(block);

    return block;
}*/

int main(int argc ,char **argv){
    /*FILE *fkey, *fplain, *fcipher;
    if (argv[1][0]== 'e') {
        // encrypt
        op=0;
        if (argc != 5) {
            fprintf(stderr, "Usage: %s key plaintext ciphertext\n", argv[0]);
            return 1;
        }
        fprintf(stderr,"Encryption\n");
        fkey = fopen(argv[2], "rb");
        fplain = fopen(argv[3], "rb");
        fcipher = fopen(argv[4], "wb");
    } else if (argv[1][0]== 'd'){
        // decrypt
        op=1;
        if (argc != 5) {
            fprintf(stderr, "Usage: %s keyfile ciphertext plaintextfile\n", argv[0]);
            return 1;
        }

        fkey = fopen(argv[2], "rb");
        fcipher = fopen(argv[3], "rb");
        fplain = fopen(argv[4], "wb");
    }
    

    if (!fkey || !fplain || !fcipher) { //check if any of them don't open
        perror("File open error");
        return 1;
    }

    // Read key
    uint64_t key = 0;
    if (fread(&key, sizeof(uint64_t), 1, fkey) != 1) {
        fprintf(stderr, "Error reading key file\n");
        fclose(fkey);
        return 1;
    }
    fclose(fkey);
    printf("Key read: %016llx\n", key);*/
    uint64_t key = 0x133457799BBCDFF1;
    uint64_t subkeys[16]={0};
    KeySchedule(&key,subkeys);
    //key cycle
    for(int i=0; i<16;i++){
        printf("Key for round {%d}= {%016llX} \n",i+1,subkeys[i]);
    }
    /*uint64_t block;

    // Read plaintext 8 bytes at a time
    while (fread(&block, sizeof(uint64_t), 1, op? fcipher: fplain) == 1) {
        //Preform the whole DES on that block
        //fwrite(&encrypted, sizeof(uint64_t), 1, fcipher);
        printf("Block read: %016llx\n", block);
        if(op==0)// encrypt path
        {
            uint64_t encrypted= DES_Encrypt(block,subkeys);
            fwrite(&encrypted, sizeof(uint64_t), 1, fcipher);
            printf("Block encrypted: %016llx\n", encrypted);
        }
        else // decrypt path
        {
            uint64_t decrypted= DES_Decrypt(block,subkeys);
            fwrite(&decrypted, sizeof(uint64_t), 1, fplain);
            printf("Block decrypted: %016llx\n", decrypted);

        }
    }

    fclose(fplain);
    fclose(fcipher); */
    return 0;
}