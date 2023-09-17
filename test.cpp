#include <cassert>
#include <climits>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_set>

using namespace std;

/** 申明整个生命周期 **/
class LifeCycle {
 public:
  // 纯虚函数
  virtual void init(std::string config) = 0;
  virtual void startUp() = 0;

  virtual void shutDown() = 0;
};

/** 模板类，一般对模板参数类型typename和class认为是一样的，并构建多两个纯虚函数
https://blog.csdn.net/function_dou/article/details/84644963 **/
template <typename T>
class Component : public LifeCycle {
 public:
  virtual std::string getName() = 0;

  virtual void execute(T t) = 0;
};

/** 需要把之前的模板一同继承 **/
template <typename T, typename R>
class AbstractComponent : public Component<T> {
 private:
  std::unordered_set<shared_ptr<Component<R>>> down_stream;

 protected:
  const std::unordered_set<shared_ptr<Component<R>>> &getDownStream() {
    return down_stream;
  }

 protected:
  virtual R doExecute(T t) = 0;

 public:
  void addDownStream(shared_ptr<Component<R>> component) {
    down_stream.insert(component);  //将函数指针存入数组中
  }

  void init(std::string config) override {}

  void startUp() override {
    for (auto &&obj : this->getDownStream()) {
      obj->startUp();
    }
    cout << "------------------ " + this->getName() +
                " is starting ----------------------"
         << endl;
  }

  void shutDown() override {
    auto downStreams = this->getDownStream();
    for (auto &&obj : downStreams) {
      obj->shutDown();
    }
    cout << "------------------ " + this->getName() +
                " is starting ----------------------"
         << endl;
  }

  void execute(T t) override {
    R r = doExecute(t);
    cout << this->getName() + "\treceive\t" << typeid(t).name() << "\t" << t
         << "\treturn\t" << typeid(r).name() << "\t" << r << endl;
    if constexpr (is_same_v<R, void>) {
      return;
    }
    for (auto &&obj : getDownStream()) {  //读取数据
      obj->execute(r);                    //一层层查找
    }
  }
};

/**从这向下才是pipeline的操作，上面的是数据的生命周期**/

class printSink : public AbstractComponent<string, int> {
 public:
  string getName() override { return "printSink"; }

 protected:
  int doExecute(string t) override { return INT_MIN; }
};

class intStringChannel : public AbstractComponent<int, string> {
 public:
  string getName() override { return "intStringChannel"; }

  void startUp() override {}

 protected:
  string doExecute(int t) override { return to_string(t + 100); }
};

class IntSource : public AbstractComponent<int, int> {
 private:
  int val = 0;

 public:
  void init(std::string config) override {
    cout << "--------- " + getName() + " init --------- ";
    val = 1;
  }

  string getName() override { return "Int Source"; }

  void startUp() override {
    this->execute(val);  //处理数据
  }

 protected:
  int doExecute(int) override { return val + 1; }
};

template <typename R, typename T>
class pipeline : public LifeCycle {
 private:
  shared_ptr<AbstractComponent<R, T>> source;

 public:
  void setSource(shared_ptr<AbstractComponent<R, T>> component) {
    source = component;  //传入类
  }

  void init(std::string config) override {}

  void startUp() override {
    assert(source.get() != nullptr);
    source->startUp();  //完成类函数的处理
  }

  void shutDown() override { source->shutDown(); }
};

int main() {
  pipeline<int, int> p;
  // source
  auto is = make_shared<IntSource>();

  // channel
  auto isc = make_shared<intStringChannel>();

  // sink
  auto ps = make_shared<printSink>();

  is->addDownStream(isc);
  isc->addDownStream(ps);

  // 设置 source
  p.setSource(is);

  // 启动
  p.startUp();
}