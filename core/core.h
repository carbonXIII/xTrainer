#ifndef __XT_CORE_H
#define __XT_CORE_H

#define DECLARE_STATIC(RET,FUNC,ARG)            \
  static RET FUNC(const ARG& args = ARG())      \
  { return Derived::_##FUNC(args); }
#define DECLARE(RET,FUNC,ARG)            \
  RET FUNC(const ARG& args = ARG())      \
  { return get_imp()->_##FUNC(args); }

namespace xtrainer {

  template <typename imp>
  struct _Types {
    typedef int Address;
    typedef int CreateArgs;
    typedef int ReadWriteArgs;
    typedef int GetThreadsArgs;

    typedef int StartDebugArgs;
    typedef int StopDebugArgs;
    typedef int GetRegsArgs;
    typedef int SetRegsArgs;
  };

}

#endif // __XT_CORE_H
