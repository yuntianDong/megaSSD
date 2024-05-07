INCLUDES = -I/usr/local/include -I../../inc/ -I/usr/local/include/boost/include/ -I/usr/local/spdk/include

SRC =			\
	Buffers.cpp	\
	HostBuffer.cpp	\
	BufferPool.cpp	\
	BufferMngr.cpp	\
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CXX) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)
