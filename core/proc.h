#ifndef __XT_CORE_PROCESS_H
#define __XT_CORE_PROCESS_H

#include "core.h"
#include "thread.h"

#include <vector>
#include <memory>

namespace xtrainer {
  template <typename imp>
  struct Process {
    using Derived = typename imp::_Process;
    using Address = typename _Types<imp>::Address;

    Derived* get_imp()
    { return reinterpret_cast<Derived*>(this); }

    using CreateArgs = typename _Types<imp>::CreateArgs;
    DECLARE_STATIC(std::shared_ptr<Process>,create,CreateArgs)

    using ReadWriteArgs = typename _Types<imp>::ReadWriteArgs;
    DECLARE(int,read_bytes,ReadWriteArgs)
    DECLARE(int,write_bytes,ReadWriteArgs)

    using GetThreadsArgs = typename _Types<imp>::GetThreadsArgs;
    DECLARE(std::vector<Thread<imp>*>,get_threads,GetThreadsArgs);

  protected:
    void* handle;
  };
}

#endif // __XT_CORE_PROCESS_H
