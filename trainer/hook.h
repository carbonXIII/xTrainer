#include <vector>
#include <memory>
#include <functional>

#include "trainer.h"

namespace xtrainer{

  struct Hook{
    static Hook makeHook(Process* p, address_t target, address_t hook);

    address_t trampoline();
    void unhook();
    void rehook();

  protected:
    Hook(Process* p, address_t target, address_t hook, std::vector<char> displaced);

  private:
    address_t target;
    address_t hook;
    std::vector<char> displaced;
    std::shared_ptr<void> _trampoline;
    Process* p;

    static std::vector<char> assembleJump(address_t target, address_t base);
  };

}
