INCLUDES = -I/usr/local/include -I/usr/local/include -I../../../inc -I/usr/local/spdk/include

SRC =			\
	StdSPDKEngine.cpp
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CC) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)
