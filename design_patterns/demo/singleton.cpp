#include <bits/stdc++.h>

using namespace std;

class Base {
 public:
  static Base& getInstance() {
    static Base object;
    return object;
  }

 private:
  Base() { cout << "base()" << endl; }
};

class Base2 {
 public:
  static Base2& getInstance() { return object; }

 private:
  Base2() { cout << "base2()" << endl; }
  static Base2 object;
};

Base2 Base2::object;

int main() {
  // 观察构造函数何时被调用
  cout << "hello" << endl;
  Base& b = Base::getInstance();
  Base2& b2 = Base2::getInstance();
  return 0;
}