namespace xtrainer {
  // TODO: virtualize
  class FThread {
  public:
    FThread(long tid)
      : tid(tid) {}

    long getTid() { return tid; }

  protected:
    long tid;
  };
}
