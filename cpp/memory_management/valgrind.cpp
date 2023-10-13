#include <string>

class Singleton {
 public:
  static Singleton &getInstance() {
    static Singleton instance;
    return instance;
  }

 private:
  Singleton() {}
  ~Singleton() {}
};
int main() {
  Singleton::getInstance();
  return 0;
}
