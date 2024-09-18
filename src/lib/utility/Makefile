INCLUDES = -I. -I/usr/local/include -I../../inc -I/usr/local/boost_1_78/include/

SRC =			\
	ByteHdlr.cpp	\
	Misc.cpp	\
	CRC.cpp		\
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CC) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak

clobber: clean
	rm -f $(OUT)
