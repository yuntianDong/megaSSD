INCLUDES = -I/usr/local/include -I/usr/local/include -I/usr/local/boost_1_78/include/ -I../../inc/ -I/usr/local/spdk/include

SRC =			\
	PatternBitmap.cpp	\
	PatternRecorder.cpp	\
	PatternMngr.cpp	\
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CXX) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)
