#APP_ABI := all
APP_ABI := armeabi  armeabi-v7a arm64-v8a mips x86 x86_64
#APP_ABI := mips64
APP_STL := stlport_static
APP_CFLAGS += -Wno-error=format-security
