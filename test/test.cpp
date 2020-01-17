#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;

int main() {
  int takema = 69;

  cout << getpid() << endl;
  cout << &takema << endl;
  while(takema) {
    std::this_thread::yield();
  }

  auto spin = []() {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    int z = 0;
    int b = 0;
    while(true) {
      if(b == 0) {
        cout << "="; cout.flush();
        if(++z == 10) b = 1;
      } else {
        cout << "\b"; cout.flush();
        if(--z == 0) b = 0;
      }
    }
  };

  spin();

  return 0;
}
