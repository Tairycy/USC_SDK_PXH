#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "crypt.h"


CLCrypt::CLCrypt():m_version(1){

    switch(m_version){
    case 1:
        m_key = "!@#$^&*()";
		m_keyLen = strlen(m_key);
        break;
    default:
        printf("!!!!!Impossible!!!!!!\n");
    }

    m_buffer = NULL;
}

CLCrypt::CLCrypt(char *key):m_version(1){
    if (NULL == key||strlen(key) == 0){
        CLCrypt();
        return ;
    }
    m_key = key;
	m_keyLen = strlen(m_key);
    m_buffer = NULL;
}

CLCrypt::CLCrypt(int version){
    if (version > 1){
        CLCrypt();
        return ;
    }
    m_version = version;
    switch(m_version){
    case 1:
        m_key = "!@#$^&*()";
		m_keyLen = strlen(m_key);
        break;
    default:
        printf("!!!!!Impossible!!!!!!\n");
    }
    m_buffer = NULL;
}

CLCrypt::CLCrypt(int version, char *key){

    if (version > 1){
        if (NULL == key||strlen(key) == 0){
            CLCrypt();
            return ;
        }
        CLCrypt(key);
        return ;
    }
    
    m_version = version;

    if (NULL == key||strlen(key) == 0){
        CLCrypt(m_version);
        return ;
    }

    m_key = key;
	m_keyLen = strlen(m_key);
    m_buffer = NULL;
}

CLCrypt::~CLCrypt(){
    if (NULL != m_buffer){
        free(m_buffer);
        m_buffer = NULL;
    }
    
    m_version = 1;
}

bool CLCrypt::isNeedCrypt(char* key)
{
	if (strlen(key) > 3)
		return false;
	return true;
}

int CLCrypt::setKey(char *key){
    if (NULL == key||strlen(key) == 0){
        return -1;
    }

    m_key = key;

    return 0;
}

int CLCrypt::setVersion(int version){
	if (version > 1 || version < 0){
        return -1;
    }

    m_version = version;
    return 0;
}

int CLCrypt::getVersion(void){
    return m_version;
}

char *CLCrypt::getKey(void){
    return m_key;
}

void CLCrypt::encrypt_v1(char *plaintext ){
    unsigned int length = strlen(plaintext);

    if (NULL != m_buffer){
        free(m_buffer);
        m_buffer = NULL;
    }

    m_buffer = (char *)malloc(length+1);

    for(unsigned int i = 0;i<length;i++){

		if (plaintext[i] >= '0' && plaintext[i] <= '9')
			m_buffer[i] = 'q' + plaintext[i] - '0';

		else if (plaintext[i] >= 'A' && plaintext[i] <= 'J')
			m_buffer[i] = 'D' + plaintext[i] - 'A';

		else if (plaintext[i] >= 'K' && plaintext[i] <= 'Z')
			m_buffer[i] = 'a' + plaintext[i] - 'K';

		else if (plaintext[i] >= 'a' && plaintext[i] <= 'm')
			m_buffer[i] = 'N' + plaintext[i] - 'a';

		else if (plaintext[i] >= 'n' && plaintext[i] <= 'w')
			m_buffer[i] = '0' + plaintext[i] - 'n';

		else if (plaintext[i] >= 'x' && plaintext[i] <= 'z')
			m_buffer[i] = 'A' + plaintext[i] - 'x';

		else
			m_buffer[i] = plaintext[i];
    }
    m_buffer[length] = '\0';
}

void CLCrypt::decrypt_v1(char *ciphertext ){
    unsigned int length = strlen(ciphertext);

    if (NULL != m_buffer){
        free(m_buffer);
        m_buffer = NULL;
    }

    m_buffer = (char *)malloc(length+1);

    for(unsigned int i = 0;i<length;i++){

		if (ciphertext[i] >= 'q' && ciphertext[i] <= 'z')
			m_buffer[i] = '0' + ciphertext[i] - 'q';

		else if (ciphertext[i] >= 'D' && ciphertext[i] <= 'M')
			m_buffer[i] = 'A' + ciphertext[i] - 'D';

		else if (ciphertext[i] >= 'a' && ciphertext[i] <= 'p')
			m_buffer[i] = 'K' + ciphertext[i] - 'a';

		else if (ciphertext[i] >= 'N' && ciphertext[i] <= 'Z')
			m_buffer[i] = 'a' + ciphertext[i] - 'N';

		else if (ciphertext[i] >= '0' && ciphertext[i] <= '9')
			m_buffer[i] = 'n' + ciphertext[i] - '0';

		else if (ciphertext[i] >= 'A' && ciphertext[i] <= 'C')
			m_buffer[i] = 'x' + ciphertext[i] - 'A';

		else
			m_buffer[i] = ciphertext[i];
    }
    m_buffer[length] = '\0';
}

char *CLCrypt::encrypt(char *plaintext ){
    if (NULL == plaintext){
        return NULL;
    }

    char *output = NULL;
    
    switch(m_version){
	case 0:
        encrypt_v0(plaintext);
        break;
    case 1:
        encrypt_v1(plaintext);
        break;
    default:
        printf("!!!!!!no crypt version!!!!!!\n");
    }

    return m_buffer;
}
char *CLCrypt::decrypt(char *ciphertext ){
    if (NULL == ciphertext){
        return NULL;
    }

    char *output = NULL;
    
    switch(m_version){
	case 0:
        decrypt_v0(ciphertext);
        break;
    case 1:
        decrypt_v1(ciphertext);
        break;
    default:
        printf("!!!!!!no crypt version!!!!!!\n");
    }

    return m_buffer;
}

void CLCrypt::clear(void)
{
	if (m_buffer != NULL)
		free(m_buffer);
	m_buffer = NULL;
	m_version = 1;
}

void CLCrypt::encrypt_v0(char *plaintext)
{
    unsigned int length = strlen(plaintext);

    if (NULL != m_buffer){
        free(m_buffer);
        m_buffer = NULL;
    }
    m_buffer = (char *)malloc(length+1);
	memcpy(m_buffer, plaintext, length);
    m_buffer[length] = '\0';	
}

void CLCrypt::decrypt_v0(char *ciphertext)
{
    unsigned int length = strlen(ciphertext);

    if (NULL != m_buffer){
        free(m_buffer);
        m_buffer = NULL;
    }
    m_buffer = (char *)malloc(length+1);
	memcpy(m_buffer, ciphertext, length);
    m_buffer[length] = '\0';
}

