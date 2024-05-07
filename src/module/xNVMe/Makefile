INCLUDES = -I. -I/usr/local/include  -I../../inc \
	 -I/usr/local/include/boost/include/ -I/usr/include/python3.6/ -I/usr/local/spdk/include  \

SRC =			\
	../xBuffer/IHostBufMngr.cpp \
	../xPattern/IPatternMngr.cpp \
	../../lib/context/Context.cpp \
	INVMeDevice.cpp	\
	Convert.cpp	\
	Wrapper.cpp	\
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CXX) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)
