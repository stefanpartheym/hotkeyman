# ##############################################################################
# hotkeyman -- makefile
# ##############################################################################

TARGET	:= hotkeyman
EXT		:= exe
SRC		:= hotkeyman.c hklist.c
OBJ		:= $(SRC:%.c=%.o)
CFLAGS	:= 
LDFLAGS	:= 

# default target is debug
default: debug

# debug
debug: CFLAGS := -D DEBUG
debug: build

# release
release: build

# build executable
build: $(OBJ)
	$(CC) -o $(TARGET).$(EXT) $(OBJ) $(LDFLAGS)

# build objects
%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm $(TARGET).$(EXT) $(OBJ)

.PHONY: clean
