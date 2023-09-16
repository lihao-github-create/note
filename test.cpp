#include <iostream>
#include <thread>

void function1() { std::cout << "Function 1 is running\n"; }

void function2() { std::cout << "Function 2 is running\n"; }

int main() {
  std::thread myThread;  // 创建线程并指定入口函数为 function1

  myThread = std::thread(function2);  // 更改线程的入口函数为 function2
  myThread.join();                    // 等待线程结束

  return 0;
}
