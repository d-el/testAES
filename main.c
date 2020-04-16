#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <aes.h>

const char *const _binary_key_txt_start;  // Declare external linked object

uint8_t char2int(char input){
	if(input >= '0' && input <= '9')
		return input - '0';
	if(input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	if(input >= 'a' && input <= 'f')
		return input - 'a' + 10;
}

int main(int argc, char *argv[]){
	const char *hexString = (char*)&_binary_key_txt_start;
    uint8_t key[16] = {};
    for(size_t i = 0; i < sizeof(key); i++){
    	key[i] = char2int(hexString[i*2]) << 4 | char2int(hexString[i*2 + 1]);
    }
    uint8_t iv[16] = {};
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);

    FILE *fd = fopen(argv[1], "r");
    if(fd == NULL){
    	printf("error open file\n");
    	return 1;
    }

    uint8_t buffer[1024] = {};
    fread(buffer, 1, sizeof(buffer), fd);
    AES_CBC_decrypt_buffer(&ctx, buffer, sizeof(buffer));
    const uint32_t signatureRef = 0xC0DE1234;
    uint32_t signature = __builtin_bswap32(*(uint32_t*)&buffer[0]);
    uint32_t size = __builtin_bswap32(*(uint32_t*)&buffer[4]);
    if(signatureRef != signature){
    	printf("bad signature\n");
    	return 1;
    }

    size_t writeSize = sizeof(buffer) - 8 < size ? sizeof(buffer) - 8 : size;
    write(1, buffer + 8, writeSize);
    size_t writed = writeSize;
    while(fread(buffer, 1, 1024, fd)){
    	AES_CBC_decrypt_buffer(&ctx, buffer, 1024);
    	writeSize = sizeof(buffer) < size - writed ? sizeof(buffer) : size - writed;
    	write(1, buffer, writeSize);
    	writed += writeSize;
    }
    return 0;
}
