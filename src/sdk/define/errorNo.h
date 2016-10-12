#ifndef __ERROR_NO_H__
#define __ERROR_NO_H__


static const int ALL_ERROR_START = -91000;
static const int ALL_ERROR_END = -92000;

// -910XX

// Initialize failed.
static const int INIT_ERROR							=		-91000;
// No initialize before you put a request.
static const int NO_INIT_ERROR						=		-91001;
// Request initialize failed
static const int REQ_INIT_ERROR						=		-91002;
// Network connection error.
static const int NETWORK_TIMEOUT_ERROR				=		-91003;
// Request error, cann't get token from server.
static const int HTTP_REQ_ERROR						=		-91004;
// Opus decode or encode error.
static const int DECODE_ERROR						=		-91005;
// No start before get result.
static const int NO_LOGIN_ERROR						=		-91006;
// No connect internet
static const int NO_CONNECT_NETWORK_ERROR			=		-91007;
// Socket error
static const int GET_RESPONSE_ERROR					=		-91008;
static const int SEND_REQUEST_ERROR					=		-91009;
static const int GET_NO_RESPONSE_ERROR				=		-91010;

static const int SOCKET_ERROR = -90001;
static const int CONNECT_ERROR = -90002;
static const int CONNECT_SELECT_ERROR = -90003;
static const int CONNECT_SELECT_TIMEOUT = -90004;
static const int SEND_ERROR = -90005;
static const int SEND_SELECT_ERROR = -90006;
static const int SEND_SELECT_TIMEOUT = -90007;
static const int RECV_ERROR = -90008;
static const int RECV_SELECT_ERROR = -90009;
static const int RECV_SELECT_TIMEOUT = -90010;
static const int SET_SOCKET_OPTION_ERROR = -90011;
static const int SET_FCNTL_ERROR = -90012;
static const int GET_HOST_NAME_ERROR = -90013;

static const int OTHER_ERROR = -90020;

// -911XX normal operation error

// Put text is null.
static const int TEXT_NULL_ERROR					=		-91131;
// Put text too long.
static const int TEXT_TOO_LONG_ERROR				=		-91132;
// No service!
static const int SET_SERVICE_ERROR					=		-91133;
// Get infomation error.
static const int GET_INFO_ERROR						=		-91134;
// No support format error.
static const int NO_SUPPORT_FORMAT_ERROR			=		-91135;
// No support codec error
static const int NO_SUPPORT_CODEC_ERROR				=		-91136;
// Set ip/port error.
static const int SER_IP_ADDRESS_ERROR				=		-91137;
// Handle error.
static const int NO_HANDLE_ERROR					=		-91138;
// 
static const int SET_PROPERTY_ERROR					=		-91139;
// No data error.
static const int NO_DARA_ERROR						=		-91140;
// Get token error.
static const int GET_TOKEN_ERROR					=		-91141;
// No set option ID error.
static const int OPTION_ID_ERROR					=		-91151;
// Option parameter error.
static const int OPTION_PARAM_ERROR					=		-91152;
static const int SET_CRYPT_VERSION_ERROR			=		-91153;
static const int CHECK_MD5_ERROR					=		-91154;
static const int MAX_SPEECH_TIMEOUT					=		-91155;
static const int COMPRESS_PCM_ERROR					=		-91156;
static const int INVALID_INPUT_DATA					=		-91157;
static const int NO_SECRET_ERROR = -91158;
static const int VAD_TIMEOUT = -91159;
static const int TIME_LIMIT_ERROR = -91160;

// -91700 http request error
static const int HTTP_REQ_ERROR_START				=		-91700;
static const int HTTP_REQ_ERROR_END					=		-91899;


#endif

