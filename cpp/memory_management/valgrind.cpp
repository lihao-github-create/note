#include <bits/stdc++.h>
using namespace std;

class Widgt {
  int x;
};

int main() {
  shared_ptr<Widgt[]> p1 = make_shared<Widgt[]>();
  std::shared_ptr<int> sp3 =
      make_shared<int>(new int[10](), [](int *p) { delete[] p; });

  std::unique_ptr<Widgt> pw = std::make_unique<Widgt>();
  std::unique_ptr<Widgt> pw2 = std::move(pw);

  return 0;
}
