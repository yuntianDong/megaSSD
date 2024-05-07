INCLUDES = -I/usr/local/include -I/usr/local/boost/include -I../../inc/

SRC =			\
	JsonRPC2TaskMngr.cpp	\
	NanoJsonRPC.cpp	\
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CXX) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)
