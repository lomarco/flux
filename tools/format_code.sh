#!/usr/bin/env bash

set -euo pipefail

DIR="${1:-.}"
DIR="$(readlink -f "$DIR")"

RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
RESET="\033[0m"

FORMAT_STYLE="Chromium"

error() {
  printf "${RED}error${RESET}: %s\n" "$1" >&2
}

info() {
  printf "${GREEN}%s${RESET}\n" "$1"
}

warn() {
  printf "${YELLOW}warning${RESET}: %s\n" "$1"
}

# Check if clang-format is installed
if ! command -v clang-format >/dev/null 2>&1; then
  error "clang-format not found. Please install it to proceed."
  exit 1
fi

info "Starting formatting in directory: $DIR"
echo "----------------------------------------"

# Find files with .c or .h extensions
mapfile -t files < <(find "$DIR" \( -name "*.c" -o -name "*.h" \))

if [[ ${#files[@]} -eq 0 ]]; then
  info "No .c or .h files found in $DIR."
  exit 0
fi

info "Found ${#files[@]} file(s) to format."

# Format files with progress output and check if changed
for file in "${files[@]}"; do
  printf "Formatting: %s ... " "$file"

  tmp_before=$(mktemp)
  tmp_after=$(mktemp)

  cp "$file" "$tmp_before"

  if clang-format -style="$FORMAT_STYLE" "$file" > "$tmp_after"; then
    if ! cmp -s "$tmp_before" "$tmp_after"; then
      # File changed, replace original
      mv "$tmp_after" "$file"
      printf "${GREEN}done${RESET} ${YELLOW}[CH]${RESET}\n"
    else
      # File unchanged
      printf "${GREEN}done${RESET}\n"
      rm "$tmp_after"
    fi
  else
    printf "${RED}failed${RESET}\n"
    warn "Could not format file: $file"
    rm -f "$tmp_after"
  fi

  rm -f "$tmp_before"
done

echo "----------------------------------------"
info "Formatting complete."
