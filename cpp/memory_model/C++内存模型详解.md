# C++内存模型详解

## 应用

### 双重检查锁定

```cpp
#include <iostream>
#include <mutex>
#include <atomic>

template <typename T>
class Singleton {
public:
    static T& getInstance() {
        if (!instance_.load(std::memory_order_acquire)) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!instance_.load(std::memory_order_relaxed)) {
                T* temp = new T;
                instance_.store(temp, std::memory_order_release);
            }
        }
        return *instance_.load(std::memory_order_relaxed);
    }

    // 防止复制构造函数和赋值运算符被调用
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    // 私有构造函数，防止外部实例化
    Singleton() {}

    static std::atomic<T*> instance_;
    static std::mutex mutex_;
};

template <typename T>
std::atomic<T*> Singleton<T>::instance_(nullptr);

template <typename T>
std::mutex Singleton<T>::mutex_;

```
