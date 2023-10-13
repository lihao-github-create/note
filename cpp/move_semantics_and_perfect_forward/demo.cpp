#include <bits/stdc++.h>
using namespace std;

struct M2 {  // bad: incomplete set of copy/move/destructor operations
 public:
  M2(){};
  M2(M2 &&) = default;
  // ...
  // ... no copy or move operations ...
  ~M2() { delete[] rep; }

 private:
  pair<int, int> *rep;  // zero-terminated set of pairs
};

void use() {
  M2 x;
  M2 y;
  // ...
  x = y;  // the default assignment
          // ...
}
