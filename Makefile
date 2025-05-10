CC ?= gcc
ifeq ($(LLVM), 1)
    CC := clang
endif

TARGET       := flux
BUILD_DIR    := build
SRC_DIR      := src
INC_DIR      := include
MANPAGES_DIR := manpages

DESTDIR         ?=
PREFIX          ?= /usr
INSTALL_DIR     := $(DESTDIR)$(PREFIX)/bin
MAN_INSTALL_DIR := $(DESTDIR)$(PREFIX)/share/man/man1
MANPAGES        := $(wildcard $(MANPAGES_DIR)/*.1)

WARNINGS := -Wall -Wextra -pedantic \
            -Wshadow -Wformat=2 \
            -Wconversion -Wnull-dereference \
            -Wstack-protector -Wdouble-promotion

CFLAGS := -std=gnu23 $(WARNINGS) \
          -I$(INC_DIR) -MMD -MP \
          $(EXTRA_CFLAGS)

# TODO: Add LDFLAGS

DEBUG_CFLAGS   := -g3 -DDEBUG -O0 -fno-omit-frame-pointer
RELEASE_CFLAGS := -O3 -flto -DNDEBUG

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

RED   := \033[31m
GREEN := \033[32m
RESET := \033[0m

all: release

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: $(BUILD_DIR)/$(TARGET)

release: CFLAGS += $(RELEASE_CFLAGS)
release: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $@

install: $(BUILD_DIR)/$(TARGET)
	@mkdir -p $(INSTALL_DIR) $(MAN_INSTALL_DIR)
	install -m 755 $(BUILD_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET)
	install -m 644 $(MANPAGES_DIR)/flux.1 $(MAN_INSTALL_DIR)/flux.1
	gzip -f $(MAN_INSTALL_DIR)/flux.1

clean:
	rm -rf $(BUILD_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/$(TARGET) $(MAN_INSTALL_DIR)/flux.1.gz

check-man:
	@if [ ! -f $(MAN_INSTALL_DIR)/flux.1.gz ]; then \
		printf "$(RED)%s: error: flux.1.gz not found$(RESET)\n" "$@"; \
		exit 1; \
	fi
	@if ! man $(TARGET) > /dev/null 2>&1; then \
		printf "$(RED)%s: error: manpage cannot be formatted$(RESET)\n" "$@"; \
		exit 1; \
	fi
	@printf "$(GREEN)%s: manpage correctly installed$(RESET)\n" "$@"

-include $(DEPS)

.PHONY: all install clean uninstall check-man debug release
.SILENT: $(TARGET) install clean uninstall check-man $(OBJ) debug release $(BUILD_DIR)
