SRC	:= main.cpp mainw.cpp lava_thread.cpp lava_widget.cpp
SRCC	:= FindDoctor.c
HDR	:= mainw.h lava_widget.h
TRG	:= lava_qt2

MOC_SRC	:= $(HDR:%.h=moc_%.cpp)
SRC	+= $(MOC_SRC)

TOP	?= $(CURDIR)/../build/qt-2.3.10-mt
INC	+= -I$(TOP)/include -I$(TOP)/src/3rdparty/freetype/include
LIB	+= -L$(TOP)/lib -Wl,-rpath,/opt/qt-2.3.10/lib -lqte-mt
DEF	+= -D_REENTRANT -DQWS -DQT_THREAD_SUPPORT
CFLAGS	:= -Os -pipe -D_FILE_OFFSET_BITS=64
FLAGS	?= -Os -pipe -fno-rtti -D_FILE_OFFSET_BITS=64
CROSS	?= mipsel-linux-
MOC	?= ../bin/moc

.PHONY: all
all: $(TRG)

.PHONY: send
send: $(TRG)
	-cat $(TRG) | ncat np1000 1234 --send-only

$(TRG): $(SRC:%.cpp=%.o) $(SRCC:%.c=%.o)
	$(CROSS)g++ -s -o $@ $^ $(LIB) $(FLAGS)

%.o: %.cpp
	$(CROSS)g++ -o $@ -c $(INC) $^ $(DEF) $(FLAGS)

%.o: %.c
	$(CROSS)gcc -o $@ -c $(INC) $^ $(DEF) $(CFLAGS)

moc_%.cpp: %.h
	$(MOC) -o $@ $<

.PHONY: clean
clean:
	rm -f $(TRG) $(SRC:%.cpp=%.o) $(SRCC:%.c=%.o) $(MOC_SRC)

.PHONY: gdb
gdb: $(TRG)
	mipsel-linux-gdb $(TRG) \
		-ex 'target extended-remote np1000:1234' \
		-ex 'set sysroot $(TOP)/rootfs'
