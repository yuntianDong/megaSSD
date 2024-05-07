INCLUDES = -I. -I/usr/local/include  -I../../inc -I/usr/local/boost/include
	 
SRC =			\
	DUTInfo.cpp	\
	TaskDesc.cpp	\
	TaskMngr.cpp	\
	TaskReport.cpp	\
	
.SUFFIXES: .cpp

OBJ = $(SRC:.cpp=.o)

all: $(OBJ)

.cpp.o:
	$(CC) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)
