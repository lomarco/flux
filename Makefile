CC          := clang
CFLAGS      := -std=gnu17 -Wall -Wextra -O2 -pedantic  -Wshadow -Wformat=2 -Wconversion -MMD -MP
PREFIX      := /usr
DESTDIR     := 
TARGET      := flux
INSTALLDIR  := $(PREFIX)/bin
MANPAGE     := flux.1
MANDIR      := $(PREFIX)/share/man/man1

SRC         := $(wildcard *.c)
OBJ         := $(SRC:.c=.o)
DEP         := $(OBJ:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

-include $(DEP)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install:
	mkdir -p $(DESTDIR)$(INSTALLDIR) $(DESTDIR)$(MANDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(INSTALLDIR)/$(TARGET)
	install -m 644 $(MANPAGE) $(DESTDIR)$(MANDIR)/$(MANPAGE)
	gzip -f $(DESTDIR)$(MANDIR)/$(MANPAGE)

clean:
	rm -f $(TARGET) $(OBJ) $(DEP)

uninstall:
	rm -f $(DESTDIR)$(INSTALLDIR)/$(TARGET) $(DESTDIR)$(MANDIR)/$(MANPAGE).gz

check-man:
	if [ ! -f $(MANDIR)/$(MANPAGE).gz ]; then \
		echo -e "\033[31m$(MAKE): error: $(MANPAGE).gz not found.\033[0m"; \
		exit 1; \
	fi
	if ! man $(TARGET) &> /dev/null; then \
		echo -e "\033[31m$(MAKE): error: $(MANPAGE).gz cannot be formatted.\033[0m"; \
		exit 1; \
	fi
	echo -e "\033[32m$(MAKE): $(MANPAGE).gz instaled to $(MANDIR)\033[0m"

.PHONY: all install clean uninstall check-man
.SILENT: $(TARGET) install clean uninstall check-man
