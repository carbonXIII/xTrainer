#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <type_traits>
#include "../tools/tools.h"

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
      return *reinterpret_cast<T*>(context + it->second);
    }

    void* get(){ return context; }
    void* update(void* context);

    bool setRegister(const std::string& registerName, void* data, int size);
    bool getRegister(const std::string& registerName, void* data, int size);

  private:

    char* context;
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
    int pause();
    int resume(bool debug = false, int32_t threadId = 0);

    address_t stackPush(address_t basePointer, Argument arg);
    address_t stackPop(address_t basePointer, size_t size, void* out = 0);

    template <typename _Iter>
    address_t stackPush(address_t basePointer, _Iter begin, _Iter end){
      for(_Iter it = begin; it != end; it++){
        basePointer = stackPush(basePointer, *it);
      }return basePointer;
    }

    CPUState getCPUState();
    void setCPUState(CPUState& state);

    long long waitForFatalDebugEvent();
    void run_explicit(address_t foreignAddr, Argument ret = Argument(), std::vector<Argument> args = std::vector<Argument>());

    template <typename First, typename... Args>
    typename std::enable_if<std::is_fundamental<First>::value || std::is_pointer<First>::value, void>::type
    initializeArgumentVector(std::vector<Argument>& argv, First& arg1, Args... rest);
    void initializeArgumentVector(std::vector<Argument>& argv);
    void freeArgumentVector(std::vector<Argument>& argv);

    template <typename Return, typename... Args>
    Return run(address_t foreignAddr, Args... args);

  private:
    Process* parent;

    long threadID;
    void* handle;
  };

  template <typename First, typename... Args>
  typename std::enable_if<std::is_fundamental<First>::value || std::is_pointer<First>::value, void>::type
  ForeignThread::initializeArgumentVector(std::vector<Argument>& argv, First& arg1, Args... rest){
    First* mem = (First*)new char[sizeof(First)];
    *mem = arg1;
    argv.emplace_back(mem, sizeof(First), std::is_floating_point<First>::value);

    initializeArgumentVector(argv, rest...);
  }

  template <typename Return, typename... Args>
  Return ForeignThread::run(address_t foreignAddr, Args... args){
    std::vector<Argument> argv;
    initializeArgumentVector(argv, args...);

    Return ret;
    Argument retArg(&ret, sizeof(Return), std::is_floating_point<Return>::value);

    run_explicit(foreignAddr, retArg, argv);

    //cleanup the mem we allocated
    freeArgumentVector(argv);

    return ret;
  }

}//namespace xtrainer
