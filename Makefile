CC ?= cc
CFLAGS ?= -Wall -Wextra -std=c11
LDLIBS ?= -lm

TARGET := tetris
SOURCES := src/main.c src/tetris.c
OBJECTS := $(SOURCES:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

src/%.o: src/%.c src/tetris.h termbox2.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(OBJECTS) $(TARGET)
