#include <bits/stdc++.h>

using namespace std;

class IHandler {
 public:
  virtual bool handle(void *obj) = 0;
};

class HandlerA : public IHandler {
 public:
  virtual bool handle(void *obj) override {
    // ...
    return false;
  }
};

class HandlerB : public IHandler {
 public:
  virtual bool handle(void *obj) override {
    // ...
    return false;
  }
};

class HandlerChain {
 public:
  void addHandler(IHandler &handler) { handlers.push_back(&handler); }

  void handle(void *obj) {
    for (auto &handler : handlers) {
      if (handler->handle(obj)) {
        break;
      }
    }
  }

 private:
  std::vector<IHandler *> handlers;
};