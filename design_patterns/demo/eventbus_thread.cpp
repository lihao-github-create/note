#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

// 事件类型
using EventType = int;

class EventBus {
 public:
  EventBus() : running_(true) {
    workerThread_ = std::thread([this]() { this->run(); });
  }

  ~EventBus() {
    running_ = false;
    condition_.notify_all();
    workerThread_.join();
  }

  // 注册事件处理器，使用模板函数重载处理不同类型的事件
  template <typename EventData>
  void subscribe(EventType eventType,
                 void (*handler)(const std::shared_ptr<EventData>&)) {
    std::lock_guard<std::mutex> lock(mutex_);
    eventHandlers_[eventType].push_back(
        [handler](const std::shared_ptr<void>& data) {
          if (data) {
            handler(std::static_pointer_cast<EventData>(data));
          }
        });
  }

  // 发布事件，使用模板函数处理不同类型的事件数据
  template <typename EventData>
  void publish(EventType eventType, const EventData& eventData) {
    std::lock_guard<std::mutex> lock(mutex_);
    eventQueue_.push(
        std::make_pair(eventType, std::make_shared<EventData>(eventData)));
    condition_.notify_one();
  }

 private:
  void run() {
    while (running_) {
      std::unique_lock<std::mutex> lock(mutex_);
      condition_.wait(lock,
                      [this] { return !eventQueue_.empty() || !running_; });

      while (!eventQueue_.empty()) {
        auto event = eventQueue_.front();
        eventQueue_.pop();
        lock.unlock();

        // 处理事件，使用函数指针调用
        if (eventHandlers_.find(event.first) != eventHandlers_.end()) {
          for (const auto& handler : eventHandlers_[event.first]) {
            handler(event.second);
          }
        }

        lock.lock();
      }
    }
  }

 private:
  std::thread workerThread_;
  bool running_;
  std::mutex mutex_;
  std::condition_variable condition_;
  std::unordered_map<
      EventType, std::vector<std::function<void(const std::shared_ptr<void>&)>>>
      eventHandlers_;
  std::queue<std::pair<EventType, std::shared_ptr<void>>> eventQueue_;
};

// 示例使用
int main() {
  EventBus eventBus;

  // 订阅事件，使用模板函数重载处理不同类型的事件
  eventBus.subscribe<int>(1, [](const std::shared_ptr<int>& data) {
    if (data) {
      std::cout << "Received event of int type with data: " << *data
                << std::endl;
    }
  });

  eventBus.subscribe<std::string>(
      2, [](const std::shared_ptr<std::string>& data) {
        if (data) {
          std::cout << "Received event of string type with data: " << *data
                    << std::endl;
        }
      });

  // 发布事件，使用模板函数处理不同类型的事件数据
  int intValue = 42;
  eventBus.publish(1, intValue);

  std::string stringValue = "Hello, EventBus!";
  eventBus.publish(2, stringValue);

  // // 等待事件处理
  // std::this_thread::sleep_for(std::chrono::seconds(1));

  return 0;
}
