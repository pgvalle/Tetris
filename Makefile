CC ?= cc
CFLAGS ?= -Wall -Wextra -std=c11 -Iinclude
LDLIBS ?= -lm

TARGET := tetris
OBJDIR := obj
SOURCES := src/ext.c src/main.c src/bg.c src/point.c src/tetromino.c
OBJECTS := $(SOURCES:src/%.c=$(OBJDIR)/%.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@


$(OBJDIR)/%.o: src/%.c src/*.h include/*.h
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) -r $(OBJDIR) $(TARGET)
