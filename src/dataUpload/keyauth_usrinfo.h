#ifndef __KEY_AUTH_H__
#define __KEY_AUTH_H__
#define APPID_LEN 16
const int AUTH_OK             = 0;
const int AUTH_INVALID_ENCODE = -1;
const int AUTH_ERROR_DECRYPT  = -2;
const int AUTH_INVALID_KEY    = -3;

int key_auth_init();
int key_auth(char* encoded);

#endif

