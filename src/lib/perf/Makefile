INCLUDES = -I/usr/local/include -I../../inc/

SRC =			\
	PerfContext.c	\
	PerfUtility.c	\
	PerfParam.c	\
	PerfTask.c	\
	PerfQueue.c	\
	PerfStat.c	\
	PerfEngine.c	\
	
.SUFFIXES: .c

OBJ = $(SRC:.c=.o)

all: $(OBJ)

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)
