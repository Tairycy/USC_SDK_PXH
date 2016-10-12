#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <openssl/blowfish.h>

#include "base32.h"
#include "crc16.h"
#include "keyauth_usrinfo.h"
#include "key"

const int MAX_AUTH_DATA_SIZE  = 64;

static int key_decode(char *decoded, char* encoded);

BF_KEY bfkey;

int key_auth_init() {

	BF_set_key(&bfkey, KEY_LEN, KEY);

	return 0;
}

int key_auth(char* encoded) {
	
	int enc_len = base32encode_get_length(KEY_LEN) - 1;
	
	if(strlen(encoded) != (unsigned)enc_len) {
		return AUTH_INVALID_ENCODE;
	}
	
	unsigned char decoded[MAX_AUTH_DATA_SIZE] = {0};
	if(key_decode((char*)decoded, encoded) != 0) {
		return AUTH_INVALID_ENCODE;
	}
	
	return AUTH_OK;
}

static int key_decode(char *decoded, char* encoded) {

	int len = strlen(encoded);
	
	char base32[MAX_AUTH_DATA_SIZE] = {0};
	strcpy(base32, encoded);
	char crc16_chr = base32[len-1];
	base32[len-1] = '=';
	
	if(base32validate(base32, len) < 0){
		return -1;
	}
	
	if(base32decode(decoded, base32, len) == 0) {
		return -1;
	}
	
	if(crc16_chr != crc16_base_char((unsigned char*)decoded, KEY_LEN)) {
		return -1;
	}
	
	return 0;
}



int CheckKeyValid(char* keyInput, int keyLen)
{
	
	if(keyInput==NULL)
		return -1;

	char* target=(char*)malloc(keyLen+1);
	memset(target, 0x00, keyLen+1);
	memcpy(target, keyInput, keyLen);


	key_auth_init();

	if(key_auth(target) != 0) {
		//printf("invalid key!\n");
		return -1;
	}else{
		//printf("OK!valid key!\n");
	}
	
	free(target);

	return 0;			
}

