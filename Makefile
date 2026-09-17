CC ?= cc
CFLAGS ?= -Wall -Wextra -std=c11
LDLIBS ?= -lm

TARGET := tetris
OBJDIR := obj
SOURCES := src/main.c src/tetris.c
OBJECTS := $(SOURCES:src/%.c=$(OBJDIR)/%.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@


$(OBJDIR)/%.o: src/%.c src/tetris.h termbox2.h
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) -r $(OBJDIR) $(TARGET)
