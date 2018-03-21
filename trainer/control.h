#include <windows.h>
#include <string>
#include <map>
#include <vector>

namespace xtrainer{

struct Argument{
  void* addr;
  size_t size;
  bool fp = false;

  Argument(): addr(), size(), fp() {}
  Argument(void* addr, size_t size, bool fp = false): addr(addr), size(size), fp(fp) {}
};

#define ADD_REGISTER(table, register_name, context_type, field_name) table[#register_name] = offsetof(context_type, field_name)

extern std::map<std::string, size_t> REGISTER_TABLE;
extern void InitRegisterTable();

class CPUState{
public:
  CPUState(size_t size, void* context = 0);
  CPUState(CPUState&& other);
  CPUState(const CPUState& other);

  CPUState& operator=(CPUState&& other);

  ~CPUState();

  template<typename T>
  T& r(std::string registerName){
    std::map<std::string, size_t>::iterator it = REGISTER_TABLE.find(registerName);
    return *static_cast<T*>(context + it->second);
  }

  void* get(){ return context; }
  void* update(void* context);

  bool setRegister(const std::string& registerName, void* data, int size);
  bool getRegister(const std::string& registerName, void* data, int size);

private:

  void* context;
  size_t size;
};

class Process;
class ForeignThread{
public:
  ForeignThread(Process* p, long unsigned int (*callback)(void*), void* args);
  ForeignThread(Process* p, long threadID);

  void* getInternalHandle() { return handle; }
  long getID() { return threadID; }

  void join();
  void pause();
  void resume(bool debug = false);

  void* stackPush(void* basePointer, Argument arg);
  void* stackPop(void* basePointer, size_t size, void* out = 0);

  template <typename _Iter>
  void* stackPush(void* basePointer, _Iter begin, _Iter end){
    for(_Iter it = begin; it != end; it++){
      basePointer = stackPush(basePointer, *it);
    }return basePointer;
  }
 
  CPUState getCPUState();
  void setCPUState(CPUState& state);

  long long waitForFatalDebugEvent();
  void runFunction(void* foreignAddr, Argument ret = Argument(), std::vector<Argument> args = std::vector<Argument>());

private:
  Process* parent;

  long threadID;
  void* handle;
};

}//namespace xtrainer
