# Makefile compatible with GNU Make only.

TARGET = colorfilter

COMPILER = $(shell basename $(CC))
CFLAGS_gcc = -Wall -Wextra -maltivec -mabi=altivec -mcpu=native -std=c11
CFLAGS_cc = $(CFLAGS_gcc)
CFLAGS_xlc = -qaltivec -qarch=auto
CFLAGS = $(CFLAGS_$(COMPILER))

HDR = bmp.h
SRC = bmp.c main.c
OBJ = $(SRC:%.c=%.o)
DEP = $(SRC:%.c=%.d)

DISTFILES = $(HDR) $(SRC) Makefile
ARCHIVE = $(TARGET).tar.gz

all: $(TARGET)

ifeq ($(COMPILER),gcc)
-include $(DEP)
endif

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

run: $(TARGET)
	./$(TARGET) apples.bmp

archive: $(DISTFILES)
	tar cvzf $(ARCHIVE) $(DISTFILES)

clean:
	rm -f $(OBJ) $(DEP)
	rm -f $(TARGET)

distclean: clean
	rm -f $(ARCHIVE) output.bmp

.PHONY: all run archive clean distclean
