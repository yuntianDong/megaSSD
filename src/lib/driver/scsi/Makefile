LDFLAGS=-lm
DFLAGS =-DDEBUG
LIBS = -L../ -L/usr/local/lib -lm
INCLUDES = -I. -I../../ -I/usr/local/include -I../../host

SRC =			\
	ScsiCommand.cpp	\
	ScsiResponse.cpp	\
	ScsiEngine.cpp
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CC) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak

clobber: clean
	rm -f $(OUT)
