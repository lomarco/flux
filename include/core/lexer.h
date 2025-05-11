#pragma once

#include <context.h>

// TODO: Write my own lexer insteat of strtok_t

char** lex_line(Context* ctx, char* line);

// TODO: remove this func
int count_args(char** tokens_arr);
