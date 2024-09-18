INCLUDES = -I. -I/usr/local/include  -I../../inc \
	 -I/usr/local/boost/include -I/usr/include/python3.6m/ \
	 
SRC =			\
	Convert.cpp	\
	IPCPwrMngr.cpp	\
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
