#include "../../core/ABI.h"
#include "LinuxThread.h"

#include <sys/ptrace.h>
#include <sys/user.h>

namespace xtrainer {
  namespace lnx {
    enum class Arch { X86, AMD64 };

    struct LinuxABI {
      LinuxABI(Arch arch)
        :arch(arch) {}

      virtual FThreadContext* GetThreadContext(FThread* thread) {
        int tid = thread->getTid();
      }

    private:
      Arch arch;
    };
  }
}
