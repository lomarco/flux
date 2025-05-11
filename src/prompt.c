#include <prompt.h>

#include <context.h>
#include <stdio.h>
#include <unistd.h>

void print_prompt(Context* ctx) {
  char prompt[256];
  int len;

  len = snprintf(prompt, sizeof(prompt), "[%d] %s", get_exit_code(ctx), PROMPT);

  if (len > 0) {
    write(STDOUT_FILENO, prompt, (size_t)len);
  }
}
