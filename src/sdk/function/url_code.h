#ifndef __URL_CODE_H__
#define __URL_CODE_H__
#include <string>

using std::string;

string UrlEncodeUtf8(const string& szToEncode);
string UrlDecodeUtf8(const string& szToDecode);

#endif