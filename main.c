#include <stdio.h>
#include <stdint.h>

const int IP[64] = { //initial permutation
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};
const int FP[64] = { //final permutation
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9,  49, 17, 57, 25
};
const int E[48] = { //Expansion
    32, 1,  2,  3,  4,  5,
    4,  5,  6,  7,  8,  9,
    8,  9,  10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
};
const int P[32] = { //permutation function
    16, 7, 20, 21,
    29, 12, 28, 17,
    1,  15, 23, 26,
    5,  18, 31, 10,
    2,  8,  24, 14,
    32, 27, 3,  9,
    19, 13, 30, 6,
    22, 11, 4,  25
};
const int PC1[56] = { //permuted choice 1
    57, 49, 41, 33, 25, 17, 9,
    1,  58, 50, 42, 34, 26, 18,
    10, 2,  59, 51, 43, 35, 27,
    19, 11, 3,  60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7,  62, 54, 46, 38, 30, 22,
    14, 6,  61, 53, 45, 37, 29,
    21, 13, 5,  28, 20, 12, 4
};
const int PC2[48] = { //permuted choice 2
    14, 17, 11, 24, 1,  5,
    3,  28, 15, 6,  21, 10,
    23, 19, 12, 4,  26, 8,
    16, 7,  27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};
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
enum {
    encrypt,
    decrypt
} op;
//data processing blocks
uint64_t  InitialPermutation(uint64_t block)
{

}
uint64_t FinalPermutation(uint64_t block)
{

}
uint64_t Swap32(uint64_t block)
{
    uint64_t left = (block >> 32) & 0xFFFFFFFFULL;
    uint64_t right = block & 0xFFFFFFFFULL;
    return (right << 32) | left;
}


//Key schedule blocks
uint64_t PermutationChoice1(uint64_t key)
{

}
uint64_t PermutationChoice2(uint64_t block)
{

}


uint64_t LeftCircularShift(uint64_t key,int roundNumber)
{

}


//Function blocks

uint64_t ExpansionPermutation(uint32_t halfblock)
{

}

uint32_t SBoxSubstitution(uint64_t block)
{

}

uint32_t Permutation(uint32_t block)
{

}
//big blocks
uint32_t FeistelFunction(uint32_t halfblock,uint64_t key)
{
    uint64_t temp=ExpansionPermutation(halfblock);
    temp=temp^key;//xor
    uint32_t out =SBoxSubstitution(temp);
    out=Permutation(out);
    return out;

}


uint64_t Round(uint64_t block,uint64_t key)
{
    uint32_t inleft= (block >>32) & 0xFFFFFFFF;
    uint32_t inright= block & 0xFFFFFFFF;

    uint32_t outright= inleft ^ FeistelFunction(inright,key);
    uint32_t outleft= inright;
    uint64_t out= ((uint64_t)outleft <<32) | (uint64_t)outright;

    return out;


}


//Main Blocks
void KeySchedule(uint64_t key,uint64_t  subkeys[16])
{

    uint64_t tempKey= PermutationChoice1(key);


    for (int i=0;i<16;i++)
    {
        //left shifts logic here

        tempKey= LeftCircularShift(tempKey,i+1);


        subkeys[i]= PermutationChoice2(tempKey);
    }

}
uint64_t DES_Encrypt(uint64_t block,uint64_t subkeys[16])
{
    block= InitialPermutation(block);

    for (int i=0;i<16;i++)
    {
        block= Round(block,subkeys[i]);
    }

    block= Swap32(block);

    block= FinalPermutation(block);

    return block;
}

uint64_t DES_Decrypt(uint64_t block,uint64_t subkeys[16])
{
    block= InitialPermutation(block);

    for (int i=15;i>=0;i--)
    {
        block= Round(block,subkeys[i]);
    }

    block= Swap32(block);

    block= FinalPermutation(block);

    return block;
}




int main(int argc ,char **argv){
    FILE *fkey, *fplain, *fcipher;
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
    printf("Key read: %016llx\n", key);
    uint64_t subkeys[16];
    KeySchedule(key,subkeys);
    //key cycle

    uint64_t block;

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
    fclose(fcipher);
    return 0;
}