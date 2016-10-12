#ifndef __FUNCIONT_PC_H__
#define __FUNCIONT_PC_H__

int get_mac(char * mac, int max_len);
void InitNetwork();
int get_client_info(char* info, int max_len, int last_error, char* service_info);

#endif