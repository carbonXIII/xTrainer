#ifndef FTHREAD_H
#define FTHREAD_H

#include "../tools/tools.h"

#include <memory>

namespace xtrainer {
  // CPU state: (architecture specific)
  struct FThreadContext {
    virtual FThreadContext* clone() = 0;
    virtual FThreadContext* restore(FThreadContext* split) = 0;

    virtual address_t stackPointer() = 0;
    virtual address_t stackPointer(address_t addr) = 0;

    virtual address_t stackPush(size_t sz, const void* buffer) = 0;
    virtual address_t stackPop(size_t sz, void* out_optional) = 0;

    virtual address_t instructionPointer() = 0;
    virtual address_t instructionPointer(address_t addr) = 0;
  };

  struct Argument {
    void* addr;
    size_t size;
    bool fp = false;

    Argument(): addr(), size(), fp() {}
    Argument(void* addr, size_t size, bool fp = false): addr(addr), size(size), fp(fp) {}
  };

  struct FFunctionContext {
    FFunctionContext(address_t target): target(target) {}

    struct Options {
      size_t stack_size = 0;
    };

    Options& options() { return _options; }

    template <typename Ret, typename... Args>
    static FFunctionContext create(address_t target, Args... args) {
      FFunctionContext ret(target);
      ret.ret.size = new Argument(new char[sizeof(Ret)], sizeof(Ret), std::is_floating_point<Ret>::value);
      ret.initializeArgumentVector(args...);
      return ret;
    }

    address_t getTargetAddress();

    template <typename T>
    T getResult();

    ~FFunctionContext() {
      freeArgumentVector();
    }

  private:
    Options _options;
    address_t target;

    template <typename First, typename... Args>
    typename std::enable_if<std::is_fundamental<First>::value || std::is_pointer<First>::value, void>::type
    initializeArgumentVector(First& arg1, Args... rest);
    void initializeArgumentVector() {}
    void freeArgumentVector();

    template <typename T>
    void initResult();

    std::vector<Argument> args;

    // TODO: use smart pointers to manage the memory here
    Argument ret;
  };

  // calling convention (fastcall, __stdcall, etc)
  // might only be valid on some subset of architectures / platforms
  struct CallingConvention {
    virtual void SetupCallContext(FThreadContext* context,
                               const FFunctionContext& func_context) const = 0;
    virtual void RetrieveReturnResult(FThreadContext* context,
                                      FFunctionContext& func_context) const = 0;
  };

  class Process;
  class FThread {
  public:
    FThread(Process* parent, long tid);

    long getTid() { return tid; }

    // interrupt and return the current thread context
    virtual FThreadContext* pause() = 0;

    // update the thread context with any modifications
    // then resume
    virtual void resume() = 0;

    // wait for the next signal from the child process
    // then pause and return the thread context
    virtual FThreadContext* wait() = 0;

    // pause the foreign thread's execution and run a function of our own,
    // using the provided calling convention to handle setting up the function context
    // the result of the func_context will be populated by calling_conv.retrieveReturnResult()
    virtual void run(const CallingConvention& calling_conv, FFunctionContext& func_context);

  protected:
    Process* parent;
    long tid;
  };

  template <typename First, typename... Args>
  typename std::enable_if<std::is_fundamental<First>::value || std::is_pointer<First>::value, void>::type
  FFunctionContext::initializeArgumentVector(First& arg1, Args... rest) {
    First* mem = (First*)new char[sizeof(First)];
    *mem = arg1;
    args.emplace_back(mem, sizeof(First), std::is_floating_point<First>::value);

    initializeArgumentVector(rest...);
  }

  template <typename T>
  T FFunctionContext::getResult() {
    return *reinterpret_cast<T*>(ret.addr);
  }
}

#endif // FTHREAD_H
