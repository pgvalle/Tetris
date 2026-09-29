CC ?= cc
CFLAGS ?= -Wall -Wextra -std=c11 -Iinclude
CPPFLAGS ?= -MMD -MP
LDLIBS ?= -lm

TARGET := tetris
OBJDIR := obj
SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:src/%.c=$(OBJDIR)/%.o)
DEPS := $(OBJECTS:.o=.d)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@


$(OBJDIR)/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

-include $(DEPS)

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) -r $(OBJDIR) $(TARGET)
