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

# Check if clang-tidy is installed
if ! command -v clang-tidy >/dev/null 2>&1; then
  error "clang-tidy not found. Please install it to proceed."
  exit 1
fi

# Path to compile flags file (optional)
COMPILE_FLAGS_FILE="$DIR/compile_flags.txt"

# Read compile flags if file exists
EXTRA_ARGS=()
if [[ -f "$COMPILE_FLAGS_FILE" ]]; then
  while IFS= read -r line || [[ -n "$line" ]]; do
    # Skip empty lines and comments
    [[ -z "$line" || "$line" =~ ^# ]] && continue
    EXTRA_ARGS+=("-extra-arg=$line")
  done < "$COMPILE_FLAGS_FILE"
fi

info "Starting clang-tidy analysis in directory: $DIR"
echo "----------------------------------------"

# Find .c and .h files recursively
mapfile -t files < <(find "$DIR" \( -name "*.c" -o -name "*.h" \))

if [[ ${#files[@]} -eq 0 ]]; then
  info "No .c or .h files found in $DIR."
  exit 0
fi

info "Found ${#files[@]} file(s) to analyze."

for file in "${files[@]}"; do
  printf "Analyzing: %s ... " "$file"
  if clang-tidy "$file" "${EXTRA_ARGS[@]}" -- -I"$DIR/include" >/dev/null 2>&1; then
    printf "${GREEN}done${RESET}\n"
  else
    printf "${YELLOW}issues found${RESET}\n"
    # Run clang-tidy again without redirect to show output
    clang-tidy "$file" "${EXTRA_ARGS[@]}" -- -I"$DIR/include"
  fi
done

echo "----------------------------------------"
info "clang-tidy analysis complete."


# TODO: Add compile_commands.sh
