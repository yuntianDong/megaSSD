INCLUDES = -I. -I/usr/local/include  -I../../inc \
	 -I/usr/local/include/boost/include/ -I/usr/include/python3.6/ \


SRC =			\
	Wrapper.cpp	\
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CXX) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)