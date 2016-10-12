#pragma once
#ifndef __HTTP_DEF_H__
#define	__HTTP_DEF_H__

static const int		SERVICE_TIMEOUT_LIMIT =		100;
static const int		I_LIMITED_SIMPLE =			100;
static const int		I_LIMITED_HARD =			1000;

// http headers && value name
static const char*		H_SDKPARAM =				"P";

static const char*		H_AUDIO_VERSION =			"V";
static const char*		V_SDK_VERSION =				"2.1.19";
static const char*		GITTAG =					"GITTAG:1234567";

static const char*		H_SERVICE_TYPE =			"S";
static const int		V_SERVICE_ASR =				1;
static const int		V_SERVICE_TTS =				2;
static const int		V_SERVICE_SPR =				4;
static const int		V_SERVICE_VPR =				8;

static const int		N_HANDSHAKE_TO = 3500;
static const int		N_ACK_TO = 2500;
static const int		N_ONERESP_TO = 10000;
static const int		N_LAST_TO = 15000;
static const int		N_SLEEP_TIME = 20;
static const int		N_BEFORE_SEND_TIME = 10;

static const char*		H_SERVICE_REQ_CMD =			"C";
static const char*		V_LOGIN =					"[login]";
static const char*		V_LOGOUT =					"[logout]";
static const char*		V_LOGINOUT =				"0";
static const char*		V_SERVICE_REQUEST =			"1";
static const char*		V_SERVICE_PROBE =			"2";
static const char*		V_SERVICE_NORMALEND =		"3";

static const char*		H_REQ_NUM_CMD =				"N";

static const char*		H_CRYPT_VERSION =			"CRYP";
static const char*		V_CRYPT_VERSION =			"1";

static const char*		H_RANDOM_NUM =				"R";
static const char*		H_MD5 =						"MDFI";

static const char*		H_APPKEY =					"K";
static const char*		H_USER_ID =					"U";
static const char*		H_IMEI =					"I";
static const char*		H_UDID =					"UI";
static const char*		H_TOKEN =					"T";
static const char*		H_CLIENT_INFO =				"CI";
static const char*		H_SESSION_REQUEST_ID =		"SID";
static const char*		H_SERVICE_PARAM =			"SP";
static const char*		H_NLU_PARAMETER =			"TP";

static const char*		H_CONTENT_TYPE =			"CT";
static const char*		H_REQUEST_STATE =			"RS";
static const char*		H_REQUEST_STATE_ADD =		"STA";
static const char*		H_CONTENT_LENGTH =			"Content-Length";
static const char*		H_REQUEST_CONNECT =			"Connection";

static const char*		V_REQUEST_STATE_END =		"END";
static const char*		V_REQUEST_STATE_DOING =		"DOING";
static const char*		V_REQUEST_STATE_IDLE =		"IDLE";
static const char*		V_REQUEST_STATE_RELEASE =	"RELEASE";

static const char*		V_REQUEST_CONNECT_KEEP =	"Keep-Alive";
static const char*		V_REQUEST_CONNECT_CLOSE =	"Close";

static const char*		U_HTTP_LABEL =				"http://";
static const char*		U_HTTP_HOST =				"tr.hivoice.cn";
static const char*		U_HTTP_PORT =				"80";
static const char*		U_HTTP_LOGIO_URL_LAST =		"/trafficRouter/sa";
static const char*		U_HTTP_REQ_URL_LAST =		"/trafficRouter/cs";

static const char*		P_SEMICOLON =				";";
static const char*		P_PERIOD =					"=";
static const char*		P_OP_PAREN =				"[";
static const char*		P_CL_PAREN =				"]";

static const char*		V_FALSE =					"false";
static const char*		V_TRUE =					"true";



#endif // !__HTTP_DEF_H__
