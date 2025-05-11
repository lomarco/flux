#pragma once

#include <context.h>

void env_init(Context* ctx);
void env_add(Context* ctx, char* key_value);
char* env_get(Context* ctx, char* key);

// int env_set(Context* ctx, char* key_value);  // TODO: Implement this
