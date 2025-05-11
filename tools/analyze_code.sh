#!/usr/bin/env bash

# set -euo pipefail

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

# Find clang-tidy config
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

# Init counters stats
count_ok=0
count_warnings=0
count_errors=0
count_issues=0

for file in "${files[@]}"; do
  echo
  echo "========== Analysis for file: $file =========="

  if [[ -n "$CLANG_TIDY_CONFIG" ]]; then
    output=$(clang-tidy --quiet --config-file="$CLANG_TIDY_CONFIG" "$file" -- -I"$DIR/include" 2>&1)
    retcode=$?
  else
    output=$(clang-tidy --quiet "$file" -- -I"$DIR/include" 2>&1)
    retcode=$?
  fi

  if [[ $retcode -eq 0 && -z "$output" ]]; then
    printf "%s: ${GREEN}done${RESET}\n" "$file"
    ((count_ok++))
    continue
  fi

  if echo "$output" | grep -q "error:"; then
    printf "%s: ${RED}error${RESET}\n" "$file"
    echo "----------------------------------------"
    echo "$output" | sed 's/^/    /'
    echo "----------------------------------------"
    ((count_errors++))
  elif echo "$output" | grep -q "warning:"; then
    printf "%s: ${YELLOW}warning${RESET}\n" "$file"
    echo "----------------------------------------"
    echo "$output" | sed 's/^/    /'
    echo "----------------------------------------"
    ((count_warnings++))
  else
    printf "%s: ${YELLOW}issues found${RESET}\n" "$file"
    echo "----------------------------------------"
    echo "$output" | sed 's/^/    /'
    echo "----------------------------------------"
    ((count_issues++))
  fi
done

echo "----------------------------------------"
info "clang-tidy analysis complete."
echo

# Print stats
echo -e "Summary:"
echo -e "  ${GREEN}OK files:${RESET}       $count_ok"
echo -e "  ${YELLOW}Files with warnings:${RESET} $count_warnings"
echo -e "  ${YELLOW}Files with issues:${RESET}   $count_issues"
echo -e "  ${RED}Files with errors:${RESET}    $count_errors"
