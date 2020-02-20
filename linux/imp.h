#ifndef __XT_IMP_H
#define __XT_IMP_H

#include "../core/thread.h"
#include "../core/proc.h"
#include <sys/uio.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>

namespace xtrainer {
  struct linux_x64;

  struct RegSet {
    user_regs_struct regs;
    user_fpregs_struct fpregs;
  };

  template <>
  struct _Types<linux_x64> {
    typedef uint64_t Address;

    typedef struct {
      unsigned long pid;
    } CreateArgs;

    typedef struct {
      Address addr;
      size_t count;
      mutable void* buf;
    } ReadWriteArgs;

    typedef struct {} GetThreadsArgs;

    typedef struct {} StartDebugArgs;
    typedef struct { Address signal; } StopDebugArgs;

    typedef struct {
      RegSet& regs;
    } GetRegsArgs;

    typedef struct {
      bool set_regs;
      bool set_fpregs;
      RegSet& regs;
    } SetRegsArgs;
  };

  struct linux_x64 {
    struct _Process;
    struct _Thread: Thread<linux_x64> {
      friend struct _Process;

      int _start_debug(const StartDebugArgs& args) {
        int rc;

        rc = ptrace(PTRACE_ATTACH, tid, 0, 0);
        if(rc != 0) return rc;

        int status;
        int proc;
        while(true) {
          rc = waitpid(tid, &status, 0);
          if(rc == 0) continue;
          if(rc != tid) return rc;
          if(!WIFSTOPPED(tid)) return -2;
          return 0;
        }

        return -1; // unreachable
      }

      int _continue_debug(const StopDebugArgs& args) {
        int rc = ptrace(PTRACE_CONT, tid, 0, (void*)args.signal);

        if(rc != 0)
          return rc;
        return 0;
      }

      int _stop_debug(const StopDebugArgs& args) {
        int rc = ptrace(PTRACE_DETACH, tid, 0, (void*)args.signal);

        if(rc != 0)
          return rc;
        return 0;
      }

      int _get_regs(const GetRegsArgs& args) {
        int rc;
        rc = ptrace(PTRACE_GETREGS, tid, 0, &args.regs.regs);
        if(rc != 0) return rc;

        rc = ptrace(PTRACE_GETFPREGS, tid, 0, &args.regs.fpregs);
        if(rc != 0) return rc;

        return 0;
      }

      int _set_regs(const SetRegsArgs& args) {
        int rc;
        if(args.set_regs) {
          rc = ptrace(PTRACE_SETREGS, tid, 0, &args.regs.regs);
          if(rc != 0) return rc;
        }

        if(args.set_fpregs) {
          rc = ptrace(PTRACE_SETFPREGS, tid, 0, &args.regs.fpregs);
          if(rc != 0) return rc;
        }

        return 0;
      }

    protected:
      unsigned long tid;
      _Process* p; // parent process
    };

    struct _Process: Process<linux_x64> {
      using Base = Process<linux_x64>;

      static std::shared_ptr<Base> _create(const CreateArgs& args) {
        auto ret = std::make_shared<_Process>();
        ret->pid = args.pid;
        ret->thread.tid = args.pid;
        return ret;
      }

      int _read_bytes(const ReadWriteArgs& args) {
        iovec local, remote;
        local.iov_base = args.buf;
        local.iov_len = args.count;

        remote.iov_base = reinterpret_cast<void*>(args.addr);
        remote.iov_len = args.count;

        int64_t _read = process_vm_readv(pid,
                                         &local, 1,
                                         &remote, 1,
                                         0);

        if(_read != args.count)
          return errno;
        return 0;
      }

      int _write_bytes(const ReadWriteArgs& args) {
        iovec local, remote;
        local.iov_base = args.buf;
        local.iov_len = args.count;

        remote.iov_base = reinterpret_cast<void*>(args.addr);
        remote.iov_len = args.count;

        int64_t _read = process_vm_writev(pid,
                                         &local, 1,
                                         &remote, 1,
                                         0);

        if(_read != args.count)
          return errno;
        return 0;
      }

      std::vector<Thread<linux_x64>*> _get_threads(const GetThreadsArgs&) {
        return { &thread };
      }

    protected:
      unsigned long pid;

      // linux tid==pid, so there's just one
      // main thread per process
      // there should be a way to enumerate child
      // processes, but for now Thread <=> Process
      _Thread thread;
    };

  }; // linux_x64
}

#endif // __XT_IMP_H
