TARGET = endianchecker

CFLAGS = -Wall -Wextra -std=c11

SRC = endianchecker.c
OBJ = $(SRC:%.c=%.o)
DEP = $(SRC:%.c=%.d)

DISTFILES = $(SRC) Makefile
ARCHIVE = $(TARGET).tar.gz

all: $(TARGET)

-include $(DEP)

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

run: $(TARGET)
	./$(TARGET)

archive: $(DISTFILES)
	tar cvzf $(ARCHIVE) $(DISTFILES)

clean:
	rm -f $(OBJ) $(DEP)
	rm -f $(TARGET)

distclean: clean
	rm -f $(ARCHIVE)

.PHONY: all run archive clean distclean
