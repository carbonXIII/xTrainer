#include "FThread.h"

namespace xtrainer {
  struct ABI {
    virtual FThreadContext* GetThreadContext(FThread* thread) = 0;
    virtual void UpdateThreadContext(FThread* thread, FThreadContext* context) = 0;
  };
}
