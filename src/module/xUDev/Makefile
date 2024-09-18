INCLUDES = -I. -I/usr/local/include  -I../../inc \
	 -I/usr/local/boost_1_78/include/ -I/usr/include/python3.6m/ \
	 
SRC =			\
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
