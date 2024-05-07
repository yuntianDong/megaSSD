INCLUDES = -I. -I/usr/local/include  -I../../inc \
	 -I/usr/local/boost/include -I/usr/local/python3.5/include/python3.5m \
	 
SRC =			\
	IBlkDevMngr.cpp	\
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
