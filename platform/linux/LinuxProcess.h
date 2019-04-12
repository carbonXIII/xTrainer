#include "../../core/Process.h"

#include <memory>

namespace xtrainer {
  namespace lnx {
    struct LinuxProcess: public Process {
      LinuxProcess(unsigned long pid, ABI* abi);

      virtual size_t readBytes(address_t addr, void* buffer, size_t size);
      virtual size_t writeBytes(address_t addr, const void* buffer, size_t size);
      virtual address_t allocateMemory(size_t length, MemAccess access);
      virtual bool freeMemory(address_t addr);

      virtual void updatePages();
      virtual size_t getPageCount() const;
      virtual FPage* getPage(int i) const;

      virtual void updateThreads();
      virtual size_t getThreadCount() const;
      virtual FThread* getThread(int i) const;

      virtual ABI* getABI();

      virtual ~LinuxProcess() {}

    private:
      mutable std::vector<std::unique_ptr<FPage>> pages;
      mutable std::vector<std::unique_ptr<FThread>> threads;

      ABI* abi;
    };
  }
}
