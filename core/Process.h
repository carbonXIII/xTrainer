#include "../tools/tools.h"
#include "FThread.h"

#include <map>

namespace xtrainer {
  enum MemAccess {RO,/*Read Only*/
                  RW,/*Read Write*/
                  RE,/*Read and Exceute*/
                  RWE};/*Read, Write, Execute*/

  struct FPage {
    FPage(address_t startAddr, address_t endAddr)
      :startAddr(startAddr), endAddr(endAddr) {}

    size_t size() const { return (size_t)endAddr - (size_t)startAddr; }

    address_t startAddr;
    address_t endAddr;
  };

  class Process {
  public:
    Process(unsigned long pid, address_t base_address)
      : pid(pid), base_address(base_address){}

    virtual size_t readBytes(address_t addr, void* buffer, size_t size) = 0;
    virtual size_t writeBytes(address_t addr, const void* buffer, size_t size) = 0;
    virtual address_t allocateMemory(size_t length, MemAccess access) = 0;
    virtual bool freeMemory(address_t addr) = 0;

    address_t uploadString(std::string& s, MemAccess access, bool null_term = true) {
      address_t addr = allocateMemory(s.size() + null_term, access);

      writeBytes(addr, (const void*)s.data(), s.size());
      if(null_term) {
        char nul = '\0';
        writeBytes(addr + s.size(), &nul, 1);
      }

      return addr;
    }

    // Query functions that take PageQuery objects to find appropriate Pages
    virtual void updatePages() = 0;
    virtual size_t getPageCount() const = 0;
    virtual FPage* getPage(int i) const = 0;

    virtual void updateThreads() = 0;
    virtual size_t getThreadCount() const = 0;
    virtual FThread* getThread(int i) const = 0;

    unsigned long getPID() const { return pid; }
    address_t getBaseAddress() const { return base_address; }

    // void startDebugging();
    // void stopDebugging();

    virtual ~Process() {}

    template <typename T, typename... Args>
    static Process* getOrCreate(unsigned long pid, Args... args);

  protected:
    unsigned long pid;
    address_t base_address;
  };

  struct ProcessPool {
    std::map<unsigned long, Process*> m;

    Process* find(unsigned long pid) {
      if(m.count(pid))
        return m[pid];
      return nullptr;
    }

    Process* insert(unsigned long pid, Process* p) {
      return (m[pid] = p);
    }

    static ProcessPool& get() {
      static ProcessPool main;
      return main;
    }
  };

  template <typename T, typename... Args>
  Process* Process::getOrCreate(unsigned long pid, Args... args) {
    ProcessPool& pp = ProcessPool::get();

    Process* ret;
    if((ret = pp.find(pid)))
      return ret;

    return pp.insert(pid, new T(pid, args...));
  }
}
