#!/bin/bash
g++ -Wall -O2 -fpic -shared -o libEncodeContent.so EncodeContent.cpp base32.o crc16.o keyauth_usrinfo.o -lssl -I. -I./include -I./hiredis -I/usr/java/jdk1.6.0_34/include/ -I/usr/java/jdk1.6.0_34/include/linux
