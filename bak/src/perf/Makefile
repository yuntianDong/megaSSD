INCLUDES = -I/usr/local/include -I../../inc/ 

SRC =			\
	PerfTask.cpp	\
	PerfEngine.cpp	\
	PerfWorker.cpp	\
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CXX) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)
