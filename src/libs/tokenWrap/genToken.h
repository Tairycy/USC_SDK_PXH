#ifndef __GEN_TOKEN_H__
#define __GEN_TOKEN_H__

typedef enum TOKEN_ELEMENT{
    VERSION = 0,
    SHARE_SERECT,
    RANDOM_NUMBER,
    USR_INFO,
    RESV,
    TIME,
    APP_KEY,
    USR_ID,
    IMEI,
    CRC,
    TOTAL
};

/*---------------------------------------------------------------------------------------------------
 *|version|secrect|random|usr info|recv|time|app key len|app key|usr id len|usr id|imei len|imei|crc|
 *---------------------------------------------------------------------------------------------------
 *|   2   |   8   |  4   |   4    | 4  | 8  |     1     |  var  |    1     | var  |   1    |var | 4 |
 *---------------------------------------------------------------------------------------------------
 */

const int VERSIONLEN = 2;
const int SSLEN = 8;
const int RANDLEN = 4;
const int USERINFOLEN = 4;
const int RESVLEN = 4;
const int TTLEN = 8;
const int APP_KEY_LEN_LEN = 1;
const int USR_ID_LEN_LEN = 1;
const int IMEI_LEN_LEN = 1;
const int CRCLEN = 4;

const int G_MIN_TOKEN_LENGTH = 41;
const int POSRESV = 16;


#ifdef __cplusplus
extern "C"{
#endif

const char* genToken(char version, const char* appKey, const char* usrId, const char* imei, unsigned int* len);
const char* genTokenMorePara(char version, const char* appKey, const char* usrID, const char* imei, unsigned int* len);
const char* genTokenUsingTimestamp(const char* origToken, unsigned int* len, unsigned long timeStamp);

#ifdef __cplusplus
}
#endif

#endif
