override CFLAGS += -Wall -O2 -I ./inc -I.

CXXFLAGS	= $(CFLAGS) -fpic

SRCS            = keyauth_usrinfo.cpp base32.cpp crc16.cpp
 
OBJS            = $(SRCS:%.cpp=%.o)

%.o:    %.cpp
	g++  $(CXXFLAGS) -o $@ -c $< -g

TARGET:	libCheckUsrinfo.so keyauth

#################################################################
libCheckUsrinfo.so:    base32.o crc16.o keyauth_usrinfo.o
	g++ -shared -o $@ $^ -lssl

#keyauth_main.o:     keyauth_main.cpp
#	g++ -I. -o keyauth_main.o keyauth_main.cpp 
keyauth:	keyauth_main.cpp
	g++ -o $@ $^ -I. -L. -lCheckUsrinfo
##################################################################

clean:
	rm -rf $(OBJS) 
	rm -rf $(TARGET)

test:
	env LD_LIBRARY_PATH=. ./keyauth Foy23uemdimqjakmc25twkl5bwcywzncxaf2szaR

