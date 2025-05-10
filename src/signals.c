#include <context.h>
#include <signals.h>

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int read_fd;
  int write_fd;
  volatile sig_atomic_t pending;
} SignalState;

static SignalState sig_state = {
    .read_fd = -1,
    .write_fd = -1,
    .pending = 0};  // FIXME: IDK what is heppening on this line!

static void write_in_pipe(int sig) {
  int saved_errno = errno;
  if (sig_state.write_fd != -1) {
    write(sig_state.write_fd, &sig, sizeof(sig));
  }
  errno = saved_errno;
}

// TODO: Update signals handlers
static void handler_sigint(int sig) {
  write_in_pipe(sig);
}

static void handler_sigtstp(int sig) {
  write_in_pipe(sig);
}

static void handler_sigterm(int sig) {
  (void)sig;
  const char msg[] = "\nReceived SIGTERM, exiting...\n";
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);
  exit(0);
}

static void handler_sighup(int sig) {
  (void)sig;
  const char msg[] = "\nReceived SIGHUP, exiting...\n";
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);
  exit(0);
}

static void handler_sigcont(int sig) {
  write_in_pipe(sig);
}

int signals_init(Context* ctx) {
  int pipefd[2];

  if (pipe(pipefd) == -1) {
    fprintf(stderr, "%s: pipe failed", ctx->argv[0]);
    return -1;
  }

  sig_state.read_fd = pipefd[0];
  sig_state.read_fd = pipefd[1];

  struct sigaction sa = {
      .sa_handler = SIG_IGN,
      .sa_flags = SA_RESTART,
  };
  sigemptyset(&sa.sa_mask);

  sigset_t block_mask, old_mask;
  sigfillset(&block_mask);
  sigprocmask(SIG_SETMASK, &block_mask, &old_mask);
  /*
   *
   *
   * Implement in this blok signals []
   *
   *
   */

  sigprocmask(SIG_SETMASK, &old_mask, NULL);

  return 0;
}

int signal_set_handler(int signum, signal_handler_t handler) {
  struct sigaction sa;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

  if (sigaction(signum, &sa, NULL) == -1) {
    fprintf(stderr, "signal_set_handler failed");  // FIXME add ctx
    return -1;
  }
  return 0;
}

void signals_process_pending(Context* ctx) {
  int sig;
  ssize_t n;

  if (sig_state.read_fd == -1)
    return;

  struct timeval tv = {.tv_sec = 0, .tv_usec = 0};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(sig_state.read_fd, &fds);

  while (select(sig_state.read_fd + 1, &fds, NULL, NULL, &tv) > 0) {
    n = read(sig_state.read_fd, &sig, sizeof(sig));
    if (n != sizeof(sig))
      continue;

    switch (sig) {
      case SIGINT:
        write(STDOUT_FILENO, "\n^C\n", 4);
        print_prompt(ctx);
        break;
      case SIGTSTP:
        write(STDOUT_FILENO, "\n[Suspended]\n", 13);
        break;
        // TODO: Add all signals
    }
  }
}

void signals_restore_defaults(void) {
  if (sig_state.read_fd != -1)
    close(sig_state.read_fd);
  if (sig_state.write_fd != -1)
    close(sig_state.write_fd);

  signal_set_handler(SIGINT, SIG_DFL);
  signal_set_handler(SIGTSTP, SIG_DFL);
  // FIXME: add all signals
}
