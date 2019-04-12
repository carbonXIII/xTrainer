#include "FThread.h"

#include "../core/Process.h"

namespace xtrainer {
  FThread::FThread(Process* parent, long tid)
    : parent(parent), tid(tid) {}

  void FThread::run(const CallingConvention& calling_conv, FFunctionContext& func_context) {
    auto* context = pause();
    auto* backup = context->clone();

    if(func_context.options().stack_size) {
      context->stackPointer(parent->allocateMemory(func_context.options().stack_size, MemAccess::RW));
    }
    calling_conv.SetupCallContext(context, func_context);

    // TODO: do error handling if the context exited for an unexpected, (i.e not SIGSEGV)
    resume(), context = wait();
    calling_conv.RetrieveReturnResult(context, func_context);

    context->restore(backup);
    resume();
  }

  void FFunctionContext::freeArgumentVector() {
    for(auto it = args.begin(); it != args.end(); it++)
      delete [] ((char*)it->addr);
  }
}
