INCLUDES = -I. -I/usr/local/include -I/usr/local/include  -I../../inc \
	 -I/usr/local/boost_1_78/include/ -I/usr/include/python3.6m/ -I/usr/local/spdk/include \
	 
SRC =			\
	IHostBufMngr.cpp	\
	Convert.cpp	\
	Wrapper.cpp	\
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CC) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)
