#include "../../core/FThread.h"
#include "../../core/X86ThreadContext.h"

namespace xtrainer {
  class LinuxThread: public FThread {
  public:
    LinuxThread(Process* parent, long tid);

    virtual FThreadContext* pause();
    virtual void resume();
    virtual FThreadContext* wait();

    FThreadContext* context;
  };
}
