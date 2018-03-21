#include "control.h"
#include "../tools/tools.h"
#include "trainer.h"

using namespace std;

namespace xtrainer{

std::map<std::string, size_t> REGISTER_TABLE;

extern void InitRegisterTable(){
  if(REGISTER_TABLE.size())return;

  ADD_REGISTER(REGISTER_TABLE, rax,   CONTEXT, Rax);
  ADD_REGISTER(REGISTER_TABLE, rcx,   CONTEXT, Rcx);
  ADD_REGISTER(REGISTER_TABLE, rdx,   CONTEXT, Rdx);
  ADD_REGISTER(REGISTER_TABLE, rbx,   CONTEXT, Rbx);
  ADD_REGISTER(REGISTER_TABLE, rsp,   CONTEXT, Rsp);
  ADD_REGISTER(REGISTER_TABLE, rbp,   CONTEXT, Rbp);
  ADD_REGISTER(REGISTER_TABLE, rsi,   CONTEXT, Rsi);
  ADD_REGISTER(REGISTER_TABLE, rdi,   CONTEXT, Rdi);
  ADD_REGISTER(REGISTER_TABLE, r8,    CONTEXT, R8);
  ADD_REGISTER(REGISTER_TABLE, r9,    CONTEXT, R9);
  ADD_REGISTER(REGISTER_TABLE, r10,   CONTEXT, R10);
  ADD_REGISTER(REGISTER_TABLE, r11,   CONTEXT, R11);
  ADD_REGISTER(REGISTER_TABLE, r12,   CONTEXT, R12);
  ADD_REGISTER(REGISTER_TABLE, r13,   CONTEXT, R13);
  ADD_REGISTER(REGISTER_TABLE, r14,   CONTEXT, R14);
  ADD_REGISTER(REGISTER_TABLE, r15,   CONTEXT, R15);
  ADD_REGISTER(REGISTER_TABLE, rip,   CONTEXT, Rip);
  ADD_REGISTER(REGISTER_TABLE, xmm0,  CONTEXT, Xmm0);
  ADD_REGISTER(REGISTER_TABLE, xmm1,  CONTEXT, Xmm1);
  ADD_REGISTER(REGISTER_TABLE, xmm2,  CONTEXT, Xmm2);
  ADD_REGISTER(REGISTER_TABLE, xmm3,  CONTEXT, Xmm3);
  ADD_REGISTER(REGISTER_TABLE, xmm4,  CONTEXT, Xmm4);
  ADD_REGISTER(REGISTER_TABLE, xmm5,  CONTEXT, Xmm5);
  ADD_REGISTER(REGISTER_TABLE, xmm6,  CONTEXT, Xmm6);
  ADD_REGISTER(REGISTER_TABLE, xmm7,  CONTEXT, Xmm7);
  ADD_REGISTER(REGISTER_TABLE, xmm8,  CONTEXT, Xmm8);
  ADD_REGISTER(REGISTER_TABLE, xmm9,  CONTEXT, Xmm9);
  ADD_REGISTER(REGISTER_TABLE, xmm10, CONTEXT, Xmm10);
  ADD_REGISTER(REGISTER_TABLE, xmm11, CONTEXT, Xmm11);
  ADD_REGISTER(REGISTER_TABLE, xmm12, CONTEXT, Xmm12);
  ADD_REGISTER(REGISTER_TABLE, xmm13, CONTEXT, Xmm13);
  ADD_REGISTER(REGISTER_TABLE, xmm14, CONTEXT, Xmm14);
  ADD_REGISTER(REGISTER_TABLE, xmm15, CONTEXT, Xmm15);
}


CPUState::CPUState(size_t size, void* context): size(size) {
  InitRegisterTable();

  this->context = new char[size];

  if(!context)return;

  memcpy(this->context, context, size);
}

//move constructor
CPUState::CPUState(CPUState&& other){
  InitRegisterTable();
    
  this->context = other.context;
  other.context = 0;
  this->size = other.size;
}

//copy constructor
CPUState::CPUState(const CPUState& other){
  InitRegisterTable();

  this->size = other.size;
  this->context = new char[size];
  memcpy(this->context, other.context, size);
}

CPUState& CPUState::operator=(CPUState&& other){
  this->context = other.context;
  other.context = 0;
  this->size = other.size;
}

CPUState::~CPUState(){
  if(context)delete [] context;
}

void* CPUState::update(void* context) {
  memcpy(this->context, context, size);
}

bool CPUState::setRegister(const string& registerName, void* data, int size){
  map<string, size_t>::iterator it = REGISTER_TABLE.find(registerName);

  if(it == REGISTER_TABLE.end())return false;//register not in table

  memcpy(context + it->second, data, size);
}

bool CPUState::getRegister(const string& registerName, void* data, int size){
  map<string, size_t>::iterator it = REGISTER_TABLE.find(registerName);

  if(it == REGISTER_TABLE.end())return false;//register not in table

  memcpy(data, context + it->second, size);
}

ForeignThread::ForeignThread(Process* p, long unsigned int (*callback)(void*), void* args): parent(p){
  handle = CreateRemoteThread(p->getInternalHandle(), 0, 0, callback, args, threadID, 0);
}

ForeignThread::ForeignThread(Process* p, long threadID): parent(p), threadID(threadID){
  handle = OpenThread(THREAD_ALL_ACCESS, false, threadID);
}

void ForeignThread::join(){
  WaitForSingleObject(handle, INFINITE);
}

void ForeignThread::pause(){
  SuspendThread(handle);
}

void ForeignThread::resume(bool debug){
  if(debug)ContinueDebugEvent(parent->getPID(), threadID, DBG_CONTINUE);
  ResumeThread(handle);
}

void* ForeignThread::stackPush(void* basePointer, Argument arg){
  basePointer -= arg.size;
  parent->writeBytes(basePointer, arg.addr, arg.size);
  return basePointer;
}

//Pop the given number of bytes off the stack, storing them in out (if not NULL)
void* ForeignThread::stackPop(void* basePointer, size_t size, void* out){
  if(out)parent->readBytes(basePointer, out, size);
  return basePointer + size;
}

CPUState ForeignThread::getCPUState(){
  CPUState ret(sizeof(CONTEXT));

  static_cast<CONTEXT*>(ret.get())->ContextFlags = CONTEXT_ALL;
  if(!GetThreadContext(handle, static_cast<CONTEXT*>(ret.get()))){
    LOG << "Failed to retrieve thread context.\n";
    LOG << "Error code: " << GetLastError() << "\n";
  }

  return ret;
}

void ForeignThread::setCPUState(CPUState& state){
  if(!SetThreadContext(handle, (CONTEXT*)state.get())){
    LOG << "Failed to set thread context.\n";
    LOG << "Error code: " << GetLastError() << "\n";
  }
}

long long ForeignThread::waitForFatalDebugEvent(){
  DEBUG_EVENT dbgEvent;

  while(true){
    WaitForDebugEvent(&dbgEvent, INFINITE);

    if(dbgEvent.dwProcessId == (size_t)parent->getPID() && dbgEvent.dwThreadId == threadID
       && (dbgEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT
           || dbgEvent.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT
           || dbgEvent.dwDebugEventCode == EXIT_THREAD_DEBUG_EVENT))
      return dbgEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT ? dbgEvent.u.Exception.ExceptionRecord.ExceptionCode : -dbgEvent.dwDebugEventCode;

    ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_CONTINUE);
  }
}

void ForeignThread::runFunction(void* foreignAddr, Argument ret, vector<Argument> args){
  parent->startDebugging();
  pause();

  CPUState backup = getCPUState();
  CPUState custom = backup;

  //set some things so that when the thread is resumed it will run the function, then SIGSEGV

  //push 0 onto the stack as the return address to cause SIGSEGV
  size_t death = 0;
  custom.r<void*>("rsp") = stackPush(custom.r<void*>("rsp"), Argument((void*)&death, sizeof(void*)));

  int k[] = {0,0};//the number of int/fp args we have had so far

  const string argRegs[2][4] = {{"rcx", "rdx", "r8", "r9"}, {"xmm0", "xmm1", "xmm2", "xmm3"}};

  for(int i = 0; i < args.size(); i++){
    if(k[args[i].fp] < 4)custom.setRegister(argRegs[args[i].fp][k[args[i].fp]], args[i].addr, args[i].size);
    else custom.r<void*>("rsp") = stackPush(custom.r<void*>("rsp"), args[i]);
    k[args[i].fp]++;
  }

  custom.r<void*>("rip") = foreignAddr;

  setCPUState(custom);
  resume();

  //catch the SIGSEGV and retrieve the return value of our function
  long long errCode = waitForFatalDebugEvent();

  if(errCode != EXCEPTION_ACCESS_VIOLATION){
    LOG << "Thread died before completing function.\n";
    LOG << "Error code: " << errCode << '\n';
  }

  custom = getCPUState();
  if(ret.addr && ret.size)custom.getRegister("rax",ret.addr,ret.size);

  //reset the thread using the original context (including the stack)
  setCPUState(backup);
  resume(true);
  parent->stopDebugging();
}

}//namespace xtrainer
