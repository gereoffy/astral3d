#include <signal.h>

//  signal(sig_num,sig_handler);  helyett:
void bsd_signal(int sig_num,void sig_handler(int)){
struct sigaction sa;
  sa.sa_handler=sig_handler;
  sa.sa_flags=SA_RESTART;
  sigemptyset(&sa.sa_mask);
//  sigaddset(&sa.sa_mask, SIG_TIME);
  sigaction(sig_num, &sa, NULL);
}

