CC = gcc
LLVM ?= 0
ifeq ($(LLVM), 1)
	CC := clang
endif

CFLAGS = -std=gnu23 -Wall -Wextra -pedantic -Wshadow \
         -Wformat=2 -Wconversion -MMD -MP $(EXTRA_CFLAGS)
DEBUG_CFLAGS   = -g -DDEBUG -O0
RELEASE_CFLAGS = -O2


DESTDIR     ?=
PREFIX      ?= /usr
TARGET      := flux
BUILD_DIR   := bin
INSTALL_DIR := $(DESTDIR)$(PREFIX)/bin

MANPAGE         := flux.1
MANPAGE_DIR     := manpages
MAN_INSTALL_DIR := $(DESTDIR)$(PREFIX)/share/man/man1


SRC := $(wildcard *.c)
OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC))
DEP := $(OBJ:.o=.d)

RED   := \033[31m
GREEN := \033[32m
RESET := \033[0m

all: release

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: $(BUILD_DIR)/$(TARGET)

release: CFLAGS += $(RELEASE_CFLAGS)
release: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/$(TARGET): $(OBJ) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

-include $(DEP)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

install: $(BUILD_DIR)/$(TARGET)
	mkdir -p $(INSTALL_DIR) $(MAN_INSTALL_DIR)
	install -m 755 $(BUILD_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET)
	install -m 644 $(MANPAGE_DIR)/$(MANPAGE) $(MAN_INSTALL_DIR)/$(MANPAGE)
	gzip -f $(MAN_INSTALL_DIR)/$(MANPAGE)

clean:
	rm -rf $(OBJ) $(DEP) $(BUILD_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/$(TARGET) $(MAN_INSTALL_DIR)/$(MANPAGE).gz

check-man:
	if [ ! -f $(MAN_INSTALL_DIR)/$(MANPAGE).gz ]; then \
		printf "$(RED)%s: error: %s.gz not found$(RESET)\n" "$@" "$(MANPAGE)"; \
		exit 1; \
	fi
	if ! man $(TARGET) > /dev/null 2>&1; then \
		printf "$(RED)%s: error: %s.gz cannot be formatted$(RESET)\n" "$@" "$(MANPAGE)"; \
		exit 1; \
	fi
	printf "$(GREEN)%s: %s.gz installed to %s$(RESET)\n" "$@" "$(MANPAGE)" "$(MAN_INSTALL_DIR)"

.PHONY: all install clean uninstall check-man debug release
.SILENT: $(TARGET) install clean uninstall check-man $(OBJ) debug release $(BUILD_DIR)
