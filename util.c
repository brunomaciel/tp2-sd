#include <openssl/evp.h>
#include <string.h>

const size_t SHA_DIGEST_LENGTH = 40;

/*
 * helper function to generate a random string
 */
 static char *rand_string(char *str, size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

unsigned int hash(const char *mode, const char* dataToHash, size_t dataSize, unsigned char* outHashed) {
    unsigned int md_len = -1;
    const EVP_MD *md = EVP_get_digestbyname(mode);
    if(NULL != md) {
        EVP_MD_CTX mdctx;
        EVP_MD_CTX_init(&mdctx);
        EVP_DigestInit_ex(&mdctx, md, NULL);
        EVP_DigestUpdate(&mdctx, dataToHash, dataSize);
        EVP_DigestFinal_ex(&mdctx, outHashed, &md_len);
        EVP_MD_CTX_cleanup(&mdctx);
    }
    return md_len;
}

/*
 * helper function to generate a random sha1 hash
 */
 unsigned char *getRandomSHA1() {
 	const size_t BUFFER_SIZE = 40;
 	char *buffer = (char*)malloc(sizeof(char) * BUFFER_SIZE);
 	rand_string(buffer, BUFFER_SIZE);
     unsigned char *outHash = (unsigned char*)malloc(sizeof(unsigned char) * SHA_DIGEST_LENGTH);
     hash("SHA1", buffer, strlen(buffer), outHash);
     return outHash;
 }

 void printHash(unsigned char* hash) {
    for (int i = 0; i < 20; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}
