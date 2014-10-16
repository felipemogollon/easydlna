#this is for building easydlna on PC platform.

CC=g++
CFLAGS=-fPIC `pkg-config --cflags libupnp` -D_FILE_OFFSET_BITS=64 -DPLATFORM_64
LIBFLAGS=`pkg-config --libs libupnp` -ldl

HEAD=IUpnp.h CUpnpControl.h CUpnpDevice.h IUpnpUtil.h UpnpCommand.h IUpnpDefine.h
SOURCE=IUpnp.cpp CUpnpControl.cpp CUpnpDevice.cpp IUpnpUtil.cpp

DLNA_HEAD=IDlnaFrame.h
DLNA_SOURCE=IDlnaFrame.cpp

OBJ := IUpnp.o CUpnpControl.o CUpnpDevice.o IUpnpUtil.o 
DLNA_OBJ:=IDlnaFrame.o

OUT := libupnp++.so
testout:=testmain.out	
all:${OUT} ${testout}
	cd `pwd`/plugin/ && ${MAKE}
	cd `pwd`/pydlna/ && ${MAKE}
	make test



${OUT}:${OBJ} ${DLNA_OBJ}
	${CC}  -shared  -Wl -o ${OUT} ${OBJ} ${DLNA_OBJ}  ${LIBFLAGS}
	ln -sf `pwd`/${OUT}   /usr/lib/${OUT}
	cp  -f `pwd`/${OUT}   `pwd`/bin/pydlna/
	
${OBJ}:${SOURCE} ${HEAD}
	${CC} ${CFLAGS} -c -g IUpnp.cpp
	${CC} ${CFLAGS} -c -g CUpnpControl.cpp
	${CC} ${CFLAGS} -c -g CUpnpDevice.cpp
	${CC} ${CFLAGS} -c -g IUpnpUtil.cpp

${DLNA_OBJ}:${DLNA_HEAD} ${DLNA_SOURCE}
	${CC} ${CFLAGS} -c -g IDlnaFrame.cpp

clean:
	rm -r *.so *.o *.out
	rm -f /usr/lib/${OUT}
	cd `pwd`/plugin/ && make clean
	cd `pwd`/pydlna/ && make clean
	rm -f `pwd`/bin/pydlna/*.so  `pwd`/bin/pydlna/*.out


TESTSOURCE=test/testmain.cpp
test:${TESTSOURCE}
	${CC} ${CFLAGS} -g -D_FILE_OFFSET_BITS=64 -rdynamic -lupnp++ -o testmain.out ${TESTSOURCE}


