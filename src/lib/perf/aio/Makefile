INCLUDES = -I/usr/local/include -I../../../inc/

SRC =			\
	AioPerfEngine.c	\
	
.SUFFIXES: .c

OBJ = $(SRC:.c=.o)

all: $(OBJ)

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) $(DFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(OUT)

clobber: clean
	rm -f $(OUT)
