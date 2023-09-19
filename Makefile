CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -mwindows
INCLUDES = -IC:/msys64/mingw64/include/SDL2 -I./
LIBS = -LC:/msys64/mingw64/lib -lSDL2 -lSDL2main -lSDL2_ttf
SRCS = snake.c
OBJS = $(SRCS:.c=.o)
MAIN = snake
RM = rm -f

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS) $(MAIN)

all: $(MAIN)

.PHONY: all clean
