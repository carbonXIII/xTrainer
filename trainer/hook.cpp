#include "hook.h"

const unsigned char JUMP_REL_OP = 0xe9;

namespace xtrainer{

  Hook Hook::makeHook(Process* p, void* target, void* hook){
    std::vector<char> jump = assembleJump((size_t)hook, (size_t)target);
    std::vector<char> displaced(jump.size());
    p->readBytes(target, displaced.data(), displaced.size());
    p->writeBytes(target, jump.data(), jump.size());
    return Hook(p, target, hook, displaced);
  }

  void* Hook::trampoline(){
    if(!_trampoline){
      _trampoline = std::shared_ptr<void>(
                                         p->allocateMemory(displaced.size()*2, MemAccess::RWE),
                                         [this](void* mem) { p->freeMemory(mem); }
                                         );
      p->writeBytes(_trampoline.get(), displaced.data(), displaced.size());
      std::vector<char> jump = assembleJump((size_t)target + displaced.size(), (size_t)_trampoline.get() + displaced.size());
      p->writeBytes(_trampoline.get() + displaced.size(), jump.data(), jump.size());
    }return _trampoline.get();
  }

  void Hook::unhook(){
    p->writeBytes(target, displaced.data(), displaced.size());
  }

  void Hook::rehook(){
    std::vector<char> jump = assembleJump((size_t)hook, (size_t)target);
    p->writeBytes(target, jump.data(), jump.size());
  }

  Hook::Hook(Process* p, void* target, void* hook, std::vector<char> displaced):
    p(p), target(target), hook(hook), displaced(displaced) {}

  std::vector<char> Hook::assembleJump(size_t target, size_t base){
    std::vector<char> ret(5);
    ret[0] = JUMP_REL_OP;
    *reinterpret_cast<uint32_t*>(&ret[1]) = target - base - ret.size();
    return ret;
  }

}
