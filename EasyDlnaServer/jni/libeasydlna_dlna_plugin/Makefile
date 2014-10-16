CC=g++
LIBFLAGS=-lupnp++

CFLAGS= `pkg-config --cflags libupnp` -I/thunderst/disk/dlna-relative/dlna/upnp++/ -D_FILE_OFFSET_BITS=64 

HEAD= CESDlnaPlugin.h  CESDMController.h CESDMRender.h CESDMServer.h CESDlnaDeviceHandler.h \
	  CESAVTransport.h CESConnectionManager.h CESContentDirectory.h CESRenderingControl.h
		
SOURCE=CESDlnaPlugin.cpp CESDMController.cpp CESDMRender.cpp CESDMServer.cpp CESDlnaDeviceHandler.cpp \
		CESAVTransport.cpp CESConnectionManager.cpp CESContentDirectory.cpp  CESRenderingControl.cpp
 
DLNA_HEAD= IDlnaAVTransport.h IDlnaConnectionManager.h  \
		IDlnaContentDirectory.h IDlnaRenderingControl.h IDlnaService.h IDMController.h \
		IDMRender.h IDMServer.h ESDlnaHelper.h



OBJ := CESDlnaPlugin.o  CESDMController.o CESDMRender.o CESDMServer.o CESDlnaDeviceHandler.o \
		CESAVTransport.o CESConnectionManager.o CESContentDirectory.o CESRenderingControl.o

OUT := libthunderst_dlna.so

${OUT}:${OBJ} ${SOURCE} ${HEAD}
	${CC} -shared -fPIC -Wl -o ${OUT} ${OBJ}  ${LIBFLAGS}
	ln -sf `pwd`/${OUT}   /usr/lib/${OUT} 
	
${OBJ}:${SOURCE} ${HEAD}
	${CC} ${CFLAGS} -c -g CESDlnaPlugin.cpp
	${CC} ${CFLAGS} -c -g CESDMController.cpp
	${CC} ${CFLAGS} -c -g CESDMRender.cpp
	${CC} ${CFLAGS} -c -g CESDMServer.cpp
	${CC} ${CFLAGS} -c -g CESDlnaDeviceHandler.cpp
	${CC} ${CFLAGS} -c -g CESAVTransport.cpp
	${CC} ${CFLAGS} -c -g CESConnectionManager.cpp
	${CC} ${CFLAGS} -c -g CESContentDirectory.cpp
	${CC} ${CFLAGS} -c -g CESRenderingControl.cpp



