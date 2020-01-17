#ifndef __XT_CORE_THREAD_H
#define __XT_CORE_THREAD_H

#include "core.h"

namespace xtrainer {
  template <typename imp>
  struct Thread {
    using Derived = typename imp::_Thread;
    using Address = typename _Types<imp>::Address;

    Derived* get_imp()
    { return reinterpret_cast<Derived*>(this); }

    using StartDebugArgs = typename _Types<imp>::StartDebugArgs;
    using StopDebugArgs = typename _Types<imp>::StopDebugArgs;
    DECLARE(int,start_debug,StartDebugArgs);
    DECLARE(int,continue_debug,StopDebugArgs);
    DECLARE(int,stop_debug,StopDebugArgs);

    using GetRegsArgs = typename _Types<imp>::GetRegsArgs;
    using SetRegsArgs = typename _Types<imp>::SetRegsArgs;
    DECLARE(int,get_regs,GetRegsArgs);
    DECLARE(int,set_regs,SetRegsArgs);
  };
}

#endif // __XT_CORE_THREAD_H
