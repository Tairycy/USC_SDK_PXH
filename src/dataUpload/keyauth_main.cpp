#include "check_usrinfo.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
  
        if(argc < 2) {
                printf("usage: keyauth KEY\n");
                return 1;
        }


        return CheckKeyValid(argv[1], strlen(argv[1]));
}

