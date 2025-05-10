#pragma once

#include <context.h>

typedef void (*signal_handler_t)(int);

int signals_init(Context* ctx);

int signal_set_handler(int signum, signal_handler_t handler);

void signals_process_pending(Context* ctx);

void signals_restore_defaults(void);
