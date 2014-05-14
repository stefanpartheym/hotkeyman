# ##############################################################################
# hotkeyman -- makefile
# ##############################################################################

TARGET  := hotkeyman
EXT     := 
SRC     := main.c hotkeyman.c hklist.c
OBJ      = $(SRC:%.c=%.o)
CFLAGS  := 
LDFLAGS := 
RM      := rm -f

ifeq ($(OS), Windows_NT)

# Windows specific settings
EXT     := .exe
CFLAGS  := $(CFLAGS) -D _PLAT_WNDS

else

# X11 specific settings
SRC		:= $(SRC) xhklib.c
LDFLAGS := $(LDFLAGS) -lX11

endif


# default target is debug
default: debug

# debug
debug: CFLAGS := -g -D DEBUG
debug: build

# release
ifeq ($(OS), Windows_NT)
release: LDFLAGS := $(LDFLAGS) -mwindows
endif
release: build

# build executable
build: $(OBJ)
	$(CC) -o $(TARGET)$(EXT) $(OBJ) $(LDFLAGS)

# build objects
%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(TARGET)$(EXT) $(OBJ)

.PHONY: clean
