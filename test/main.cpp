#include <iostream>

#include "core/core.h"
#include "linux/imp.h"

using namespace std;

template <typename T>
T input(const char* msg, bool hex = 0) {
  cout << msg;
  T ret;
  if(hex) cin >> std::hex >> ret;
  else cin >> ret;
  return ret;
}

void die(int line) {
  cout << "died @" << line << endl;
  exit(1);
}

int main() {
  int rc;

  auto pid = input<unsigned long>("pid: ");
  auto proc = xtrainer::Process<xtrainer::linux_x64>::create({pid});

  auto demo_read_write =
    [&]() {
      auto addr = input<unsigned long>("address: ", 1);
      int thing = 0;
      rc = proc->read_bytes({addr, 4, &thing});
      if(rc != 0) die(__LINE__);

      cout << thing << endl;

      thing = 0;
      rc = proc->write_bytes({addr, 4, &thing});
      if(rc != 0) die(__LINE__);
    };
  demo_read_write();

  auto demo_thread_pause =
    [&]() {
      auto threads = proc->get_threads();
      if(!threads.size()) die(__LINE__);

      { char temp; cin >> temp; }
      threads[0]->start_debug();

      { char temp; cin >> temp; }
      threads[0]->continue_debug({0});
    };
  demo_thread_pause();

	return 0;
}
