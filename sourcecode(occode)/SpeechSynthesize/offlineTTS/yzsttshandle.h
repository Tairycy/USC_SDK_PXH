#ifndef _YZSTTS_HANDLE_H_
#define _YZSTTS_HANDLE_H_


#if defined MAKING_LIB
#define DLL_PUBLIC
#define DLL_LOCAL
#else
#if defined _WIN32 || defined __CYGWIN__
#ifdef MAKING_DLL
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__((dllexport))
#else
#define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__((dllimport))
#else
#define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define DLL_LOCAL
#else
#if __GNUC__ >= 4
#define DLL_PUBLIC __attribute__ ((visibility("default")))
#define DLL_LOCAL  __attribute__ ((visibility("hidden")))
#else
#define DLL_PUBLIC
#define DLL_LOCAL
#endif
#endif
#endif

#define VOICE_SCALE 0
#define VOICE_SPEED1 1
#define VOICE_SPEED2 2
#define VOICE_PITCH1 3
#define VOICE_PITCH2 4
#define SAMPLE_RATE 5
#define FIELD_OPTION 6

#define ALARM 0
#define WEATHER 1

#ifdef __cplusplus
extern "C" {
#endif

DLL_PUBLIC void* yzstts_createbase(const char* path);
DLL_PUBLIC void yzstts_releasebase(void* basehandle);

DLL_PUBLIC void* yzstts_create(void* basehandle, const char* path);
DLL_PUBLIC void yzstts_release(void* handle);

DLL_PUBLIC void* yzstts_create_singleton(const char* path, const char* param);
DLL_PUBLIC void yzstts_release_singleton(void* handle);


DLL_PUBLIC bool yzstts_add_field(void* handle, int field);
DLL_PUBLIC void yzstts_clear_fields(void* handle);

DLL_PUBLIC int yzstts_set_option(void* handle, int id, const char* opt);
DLL_PUBLIC const char* yzstts_get_option(void* handle, int id);
DLL_PUBLIC int yzstts_clear_option(void* handle);

DLL_PUBLIC int yzstts_predict_wavlenms(void* handle);



DLL_PUBLIC int yzstts_set_text(void* handle, const char* text);
DLL_PUBLIC unsigned int yzstts_generate_wave(void* handle, short* wavbuf, unsigned int maxSampleNum);

DLL_PUBLIC int yzstts_generate_params(void* handle, const char* text);
typedef void(*yzstts_callback)(void* param, short* wave_buffer, int nsamples);
DLL_PUBLIC int yzstts_generate_params_callback(void* handle, const char* text, yzstts_callback cbf, void* cbf_param);
//DLL_PUBLIC short* yzstts_generate_wave(void* handle, int frame_begin, int frame_end);
DLL_PUBLIC int yzstts_sample_number(void* handle);
DLL_PUBLIC int yzstts_frame_number(void* handle);
DLL_PUBLIC int yzstts_callback_frame_number(void* handle);


DLL_PUBLIC void yzstts_cancel(void* handle);
DLL_PUBLIC void yzstts_getversion();


#ifdef __cplusplus
}
#endif

#endif
