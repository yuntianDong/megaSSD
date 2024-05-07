INCLUDES = -I. -I/usr/local/include -I../../../inc -I/usr/local/boost/include

SRC =			\
	DNVMeUtil.cpp	\
	DNVMeIOCtrl.cpp	\
	DNVMeMetaBuf.cpp	\
	DNVMeRegister.cpp	\
	DNVMeQueue.cpp	\
	DNVMeQMngr.cpp	\
	DNVMeControl.cpp	\
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CC) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)