INCLUDES = -I/usr/local/include -I../../inc/ -I/usr/local/spdk/include -I/usr/local/include/boost/include/

SRC =			\
	DeviceHandler.cpp	\
	DeviceMngr.cpp
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CXX) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)
