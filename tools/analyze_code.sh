#!/usr/bin/env bash

set -euo pipefail

DIR="${1:-.}"
DIR="$(readlink -f "$DIR")"

RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
RESET="\033[0m"

error() {
  printf "${RED}error${RESET}: %s\n" "$1" >&2
}

info() {
  printf "${GREEN}%s${RESET}\n" "$1"
}

warn() {
  printf "${YELLOW}warning${RESET}: %s\n" "$1"
}

if ! command -v clang-tidy >/dev/null 2>&1; then
  error "clang-tidy not found. Please install it to proceed."
  exit 1
fi

# Find .clang-tidy conf
CLANG_TIDY_CONFIG=""
if [[ -f "$DIR/.clang-tidy" ]]; then
  CLANG_TIDY_CONFIG="$(readlink -f "$DIR/.clang-tidy")"
  info "Found clang-tidy config file: $CLANG_TIDY_CONFIG"
else
  warn "clang-tidy config file (.clang-tidy) not found in $DIR. Proceeding without explicit config file."
fi

info "Starting clang-tidy analysis in directory: $DIR"
echo "----------------------------------------"

mapfile -t files < <(find "$DIR" \( -name "*.c" -o -name "*.h" \))

if [[ ${#files[@]} -eq 0 ]]; then
  info "No .c or .h files found in $DIR."
  exit 0
fi

info "Found ${#files[@]} file(s) to analyze."

for file in "${files[@]}"; do
  printf "Analyzing: %s ... " "$file"
  if [[ -n "$CLANG_TIDY_CONFIG" ]]; then
    if clang-tidy --config-file="$CLANG_TIDY_CONFIG" "$file" -- -I"$DIR/include" >/dev/null 2>&1; then
      printf "${GREEN}done${RESET}\n"
    else
      printf "${YELLOW}issues found${RESET}\n"
      clang-tidy --config-file="$CLANG_TIDY_CONFIG" "$file" -- -I"$DIR/include"
    fi
  else
    if clang-tidy "$file" -- -I"$DIR/include" >/dev/null 2>&1; then
      printf "${GREEN}done${RESET}\n"
    else
      printf "${YELLOW}issues found${RESET}\n"
      clang-tidy "$file" -- -I"$DIR/include"
    fi
  fi
done

echo "----------------------------------------"
info "clang-tidy analysis complete."
