INCLUDES = -I. -I/usr/local/include -I../../inc -I/usr/local/boost/include

SRC =			\
	MsgProcessComm.cpp	\
	FileMappingSharedMem.cpp	\
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CC) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak

clobber: clean
	rm -f $(OUT)
