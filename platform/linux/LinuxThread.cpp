#include "LinuxThread.h"

#include "../../core/Process.h"
#include "../../core/ABI.h"

#include <sys/ptrace.h>
#include <sys/wait.h>

namespace xtrainer {
  LinuxThread::LinuxThread(Process* parent, long tid)
    : FThread(parent, tid) {}

  FThreadContext* LinuxThread::pause() {
    ptrace(PTRACE_ATTACH, tid, nullptr, nullptr);

    // TODO: handle waitpid returning a different result
    waitpid(tid, 0, 0);

    return (context = parent->getABI()->GetThreadContext(this));
  }

  void LinuxThread::resume() {
    parent->getABI()->UpdateThreadContext(this, context);
    ptrace(PTRACE_DETACH, tid, nullptr, nullptr);

    // TODO: handle waitpid returning a different result
    waitpid(tid, 0, 0);
  }

  FThreadContext* LinuxThread::wait() {
    // TODO: add signal information into the thread context
    // before returning it
    waitpid(tid, 0, 0);
    return (context = parent->getABI()->GetThreadContext(this));
  }
}
