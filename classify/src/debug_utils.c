// debug_utils.c
// Debug Utilities
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2015, All rights reserved.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

static void hwp_sigaction(int signum, siginfo_t* siginfo,
    void* context) {
  fprintf(stderr, "Signal Catched (No: %d, Code: %d)\n",
      siginfo->si_signo, siginfo->si_code);
  fprintf(stderr, "Type any key to attach to GDB\n");
  getchar();
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(2);
  }
  else if (pid) {
    char s[13];
    sprintf(s, "%d", pid);
    execlp("gdb", "gdb", "-p", s, NULL);
  } else {
    setpgid(0, getpid());
    raise(SIGSTOP);
  }
}

void debug_init(void) {
  struct sigaction act;
  memset(&act, '\0', sizeof(act));
  act.sa_flags = SA_SIGINFO;
  act.sa_sigaction = hwp_sigaction;

  if (sigaction(SIGABRT, &act, NULL) < 0
      || sigaction(SIGSEGV, &act, NULL) < 0) {  
    perror ("sigaction");  
    exit(2);  
  }  
}

