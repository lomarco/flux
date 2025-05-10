#include <context.h>
// #include <utils/error_utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Context* init_context(int argc, char* argv[]) {
  int i, j;

  Context* ctx = (Context*)malloc(sizeof(Context));
  if (!ctx) {
    fprintf(stderr, "%s: error allocating Context\n", argv[0]);
    return NULL;
  }

  ctx->env_size = 0;
  ctx->last_exit_code = 0;
  ctx->argc = argc;

  ctx->argv = (char**)malloc((size_t)argc * sizeof(char*));
  if (!ctx->argv) {
    fprintf(stderr, "%s: error allocated argv\n", argv[0]);
    free(ctx);
    return NULL;
  }
  for (i = 0; i < argc; ++i) {
    ctx->argv[i] = strdup(argv[i]);
    if (!ctx->argv[i]) {
      fprintf(stderr, "%s: error duplicating argv[%d]\n", argv[0], i);
      for (j = 0; j < i; ++j)
        free(ctx->argv[j]);
      free(ctx->argv);
      free(ctx);
      return NULL;
    }
  }

  ctx->argv[argc] = NULL;
  return ctx;
}

void free_context(Context* ctx) {
  int i;
  size_t j;

  if (!ctx)
    return;
  for (i = 0; i < ctx->argc; ++i)
    free(ctx->argv[i]);
  free(ctx->argv);

  for (j = 0; j < ctx->env_size; ++j)
    free(ctx->env_vars[j]);
  free(ctx->env_vars);
  free(ctx);
}
