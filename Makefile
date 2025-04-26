CC             := clang
CFLAGS         := -Wall -Wextra -O2 -std=gnu17
PREFIX         := /usr
TARGET         := flux
INSTALL_DIR    := $(PREFIX)/bin/
SRC            := $(wildcard *.c)
MANPAGE        := flux.1
MAN_DIR        := $(PREFIX)/share/man/man1/

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $@ $(CFLAGS) $^

install: $(TARGET)
	install -D -m 755 $< $(INSTALL_DIR)

install_all: install
	install -D -m 644 $(MANPAGE) $(MAN_DIR)
	gzip -f $(MAN_DIR)/$(MANPAGE)

clean:
	rm -f $(TARGET)

uninstall:
	rm -f $(MAN_DIR)/$(MANPAGE).gz $(INSTALL_DIR)/$(TARGET)

check-man:
	if [ ! -f $(MAN_DIR)/$(MANPAGE).gz ]; then \
		echo -e "\033[31mError: $(MANPAGE).gz not found.\033[0m"; \
		exit 1; \
	fi
	if ! man $(TARGET) &> /dev/null; then \
		echo -e "\033[31mError: $(MANPAGE).gz cannot be formatted.\033[0m"; \
		exit 1; \
	fi
	echo -e "\033[32m$(MANPAGE).gz checked successfully.\033[0m"

.PHONY: all install install_all clean uninstall check-man
.SILENT: $(TARGET) install install_all clean uninstall check-man
