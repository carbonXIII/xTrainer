#include "LinuxProcess.h"

#include <sys/uio.h>
#include <dirent.h>
#include <fstream>

namespace xtrainer {
  namespace lnx { // linux is a reserved word?
    size_t LinuxProcess::readBytes(address_t addr, void* buffer, size_t size) {
      size_t read = 0;
      int errorNum = 0;

      iovec local, remote;
      local.iov_base = buffer;
      local.iov_len = size;

      remote.iov_base = (void*)addr;
      remote.iov_len = size;


      long long _read = process_vm_readv(pid, &local, 1, &remote, 1, 0);

      if(_read == -1)
        errorNum = errno;
      //else
      read = _read;

      if(errorNum){
        LOG << "Error while reading process memory: " << errorNum << "\n";
        LOG << "Foreign Address: " << addr << "\n";
        LOG << "Expected byte count: " << size << "\n";
      }

      return read;
    }

    size_t LinuxProcess::writeBytes(address_t addr, const void* buffer, size_t size) {
      size_t written;
      int errorNum;

      iovec local, remote;

      //the local buffer shouldn't be edited, but we need it to be non const
      local.iov_base = const_cast<void*>(buffer);
      local.iov_len = size;

      remote.iov_base = (void*)addr;
      remote.iov_len = size;


      written = process_vm_writev(pid, &local, 1, &remote, 1, 0);

      if(written == -1)
        errorNum = errno;

      if(errorNum){
        LOG << "Error while writing process memory: " << errorNum << "\n";
        LOG << "Foreign Address: " << addr << "\n";
        LOG << "Expected byte count: " << size << "\n";
      }

      return written;
    }

    address_t LinuxProcess::allocateMemory(size_t length, MemAccess access) {
      // TODO: need to use FThread::run
      return 0;
    }

    bool LinuxProcess::freeMemory(address_t addr) {
      // TODO: need to use FThread::run
      return 0;
    }

    FPage fromRangeString(std::string range) {
      int b;
      for(int i = 0; i < range.size(); i++)if(range[i] == '-'){
          range[i] = 0;
          b = i + 1;
          break;
        }

      address_t startAddr = strtoull(range.c_str() + 0,0,16);
      address_t endAddr   = strtoull(range.c_str() + b,0,16);

      return FPage(startAddr, endAddr);
    }

    void LinuxProcess::updatePages() {
      pages.clear();
      char buf[64];
      sprintf(buf, "/proc/%ld/maps", pid);
      std::ifstream fin(buf);

      std::string waste;
      std::string range;
      while(fin >> range){
        pages.emplace_back(new FPage(fromRangeString(range)));
        getline(fin, waste);//horrible way to seek to the next line
      }

      fin.close();
    }

    size_t LinuxProcess::getPageCount() const {
      return pages.size();
    }

    FPage* LinuxProcess::getPage(int i) const {
      return pages[i].get();
    }

    void LinuxProcess::updateThreads() {
      threads.clear();
      char buf[64];
      sprintf(buf, "/proc/%ld/task", pid);

      // can't use c++17 filesystem,
      // but we're guaranteed to be on linux anyways
      // so we can settle for POSIX opendir, etc.
      DIR* dirp = opendir(buf);
      struct dirent * dp;
      while ((dp = readdir(dirp)) != NULL) {
        long tid = strtoul(dp->d_name, nullptr, 10);
        if(tid != 0)
          threads.emplace_back(new FThread(tid));
      }
      closedir(dirp);
    }

    size_t LinuxProcess::getThreadCount() const {
      return threads.size();
    }

    FThread* LinuxProcess::getThread(int i) const {
      return threads[i].get();
    }
  }
}
