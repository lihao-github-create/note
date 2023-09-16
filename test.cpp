#include <csignal>
#include <cstdlib>
#include <iostream>

class MyObject {
 public:
  MyObject() { std::cout << "MyObject constructed." << std::endl; }

  ~MyObject() { std::cout << "MyObject destructed." << std::endl; }
};

MyObject* obj = nullptr;

// 信号处理函数
void sigint_handler(int signum) {
  if (obj != nullptr) {
    delete obj;  // 调用析构函数
  }
  std::cout << "Received SIGINT signal (Ctrl+C)." << std::endl;
  std::exit(0);  // 确保退出程序
}

int main() {
  obj = new MyObject();

  obj->~MyObject();

  // 注册信号处理函数
  std::signal(SIGINT, sigint_handler);

  std::cout << "Waiting for SIGINT signal..." << std::endl;

  while (1) {
    // 什么也不做
  }

  return 0;
}
