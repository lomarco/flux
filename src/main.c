#include <context.h>
#include <core/eval.h>
#include <env.h>
#include <signals.h>
#include <utils/debug.h>
#include <utils/tty.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
  Context* ctx =
      init_context(argc, argv);  // TODO: Implement in this line ALL_INIT()
  if (!ctx) {
    fprintf(stderr, "%s: Failed to initialize shell context", argv[0]);
    return EXIT_FAILURE;
  }

  DEBUG_PRINT("Flux shell initialized (pid=%d)\n", getpid());

  env_init(ctx);

  signals_init(ctx);

  if (disable_echoctl(ctx)) {
    return 1;
  }

  eval_loop(ctx);

  free_context(ctx);

  return EXIT_SUCCESS;
}
