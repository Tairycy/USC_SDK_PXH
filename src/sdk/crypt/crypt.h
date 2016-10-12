#ifndef __CRYPT_H__
#define __CRYPT_H__

#ifdef __cplusplus
extern "C"
{
#endif

class CLCrypt{

public:
    CLCrypt();
    CLCrypt(char *key);
    CLCrypt(int version);
    CLCrypt(int version, char *key);
    ~CLCrypt();

	bool isNeedCrypt(char* key);
    int setKey(char *key);
    int setVersion(int version);

    int getVersion(void);
    char *getKey(void);
    char *encrypt(char *plaintext);
	char *decrypt(char *ciphertext);

	void clear(void);

private:
    void encrypt_v0(char *plaintext);
    void decrypt_v0(char *ciphertext);
    void encrypt_v1(char *plaintext);
    void decrypt_v1(char *ciphertext);

private:
    char *m_buffer;
    char *m_key;
	int  m_keyLen;
    int m_version;
};

#ifdef __cplusplus
}
#endif

#endif//__CRYPT_H__

