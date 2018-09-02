#include <vector>
#include <memory>
#include <functional>

#include "trainer.h"

namespace xtrainer{

  struct Hook{
    static Hook makeHook(Process* p, void* target, void* hook);

    void* trampoline();
    void unhook();
    void rehook();

  protected:
    Hook(Process* p, void* target, void* hook, std::vector<char> displaced);

  private:
    void* target;
    void* hook;
    std::vector<char> displaced;
    std::shared_ptr<void> _trampoline;
    Process* p;

    static std::vector<char> assembleJump(size_t target, size_t base);
  };

}
