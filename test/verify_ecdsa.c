#if !TARGET_LPC11XX

#include "ecc.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int randfd;

void getRandomBytes(void *p_dest, unsigned p_size)
{
    if(read(randfd, p_dest, p_size) != (int)p_size)
    {
        printf("Failed to get random bytes.\n");
    }
}

int main()
{
    EccPoint l_public;
    uint32_t l_private[NUM_ECC_DIGITS];

    uint32_t l_hash[NUM_ECC_DIGITS];
    uint32_t l_random[NUM_ECC_DIGITS];
    
    uint32_t r[NUM_ECC_DIGITS];
    uint32_t s[NUM_ECC_DIGITS];
    
    int i;
    
    randfd = open("/dev/urandom", O_RDONLY);
    if(randfd == -1)
    {
        printf("No access to urandom\n");
        return -1;
    }
    
    printf("Testing 256 signatures\n");
    
    for(i=0; i<256; ++i)
    {
        printf(".");
        fflush(stdout);
        getRandomBytes((char *)l_private, NUM_ECC_DIGITS * sizeof(uint32_t));
        
        ecc_make_key(&l_public, l_private, l_private);
        
        getRandomBytes((char *)l_hash, NUM_ECC_DIGITS * sizeof(uint32_t));
        getRandomBytes((char *)l_random, NUM_ECC_DIGITS * sizeof(uint32_t));
        
        if(!ecdsa_sign(r, s, l_private, l_random, l_hash))
        {
            printf("ecdsa_sign() failed\n");
            continue;
        }
        
        if(!ecc_valid_public_key(&l_public))
        {
            printf("Not a valid public key!\n");
            continue;
        }
        
        if(!ecdsa_verify(&l_public, l_hash, r, s))
        {
            printf("ecdsa_verify() failed\n");
        }
    }
    printf("\n");
    
    return 0;
}

#endif /* !TARGET_LPC11XX */
