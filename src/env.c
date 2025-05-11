#include <context.h>
#include <env.h>

#include <stdlib.h>
#include <string.h>

extern char** environ;

void env_init(Context* ctx) {  // TODO: Implement hash table
  char** env;

  for (env = environ; *env; env++) {
    env_add(ctx, *env);
  }
}

void env_add(Context* ctx, char* key_value) {  // TODO: Add find dup
  ctx->env_vars =
      (char**)realloc(ctx->env_vars, (ctx->env_size + 2) * sizeof(char*));
  ctx->env_vars[ctx->env_size++] = strdup(key_value);
  ctx->env_vars[ctx->env_size] = NULL;
}

char* env_get(Context* ctx, char* key) {
  size_t key_len = strlen(key);
  size_t i;
  for (i = 0; i < ctx->env_size; ++i) {
    if (strncmp(ctx->env_vars[i], key, key_len) == 0 &&
        ctx->env_vars[i][key_len] == '=')
      return ctx->env_vars[i] + key_len + 1;
  }
  return NULL;
}

// int env_set(Context* ctx, char* key_value) {  // TODO: Implement this
//   ;                                           // TODO: Implement this
// }
