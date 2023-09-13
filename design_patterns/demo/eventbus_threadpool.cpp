#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

// 事件类型
using EventType = int;

// 事件数据类型
using EventData = int;

// 事件处理器类型
using EventHandler = std::function<void(EventData)>;

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  ThreadPool(size_t numThreads) : running_(true) {
    for (size_t i = 0; i < numThreads; ++i) {
      threads_.emplace_back([this] { this->workerThread(); });
    }
  }

  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(queueMutex_);
      running_ = false;
      condition_.notify_all();
    }

    for (auto& thread : threads_) {
      thread.join();
    }
  }

  template <typename Function, typename... Args>
  void enqueue(Function&& func, Args&&... args) {
    {
      std::unique_lock<std::mutex> lock(queueMutex_);
      if (!running_) {
        return;  // 如果线程池已经停止，不再接受新任务
      }
      tasks_.emplace(
          std::bind(std::forward<Function>(func), std::forward<Args>(args)...));
    }
    condition_.notify_one();
  }

 private:
  void workerThread() {
    while (running_) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(queueMutex_);
        condition_.wait(lock, [this] { return !running_ || !tasks_.empty(); });
        if (!running_ && tasks_.empty()) {
          return;
        }
        task = std::move(tasks_.front());
        tasks_.pop();
      }
      task();
    }
  }

 private:
  std::vector<std::thread> threads_;
  std::queue<std::function<void()>> tasks_;
  std::mutex queueMutex_;
  std::condition_variable condition_;
  bool running_;
};

#include <iostream>
#include <memory>
#include <string>

// 事件类型
using EventType = int;

class EventBus {
 public:
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

  // 处理事件
  void processEvents() {
    while (true) {
      std::pair<EventType, std::shared_ptr<void>> event;
      {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return !eventQueue_.empty(); });
        event = eventQueue_.front();
        eventQueue_.pop();
      }
      if (eventHandlers_.find(event.first) != eventHandlers_.end()) {
        for (const auto& handler : eventHandlers_[event.first]) {
          handler(event.second);
        }
      }
    }
  }

 private:
  std::mutex mutex_;
  std::condition_variable condition_;
  std::unordered_map<
      EventType, std::vector<std::function<void(const std::shared_ptr<void>&)>>>
      eventHandlers_;
  std::queue<std::pair<EventType, std::shared_ptr<void>>> eventQueue_;
};

int main() {
  const int numThreads = 4;  // 设置线程池中线程的数量
  ThreadPool threadPool(numThreads);

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
  for (int i = 0; i < 10; ++i) {
    int intValue = i;
    eventBus.publish(1, intValue);

    std::string stringValue = "Event " + std::to_string(i);
    eventBus.publish(2, stringValue);
  }

  // 启动线程池来处理事件
  threadPool.enqueue([&eventBus] { eventBus.processEvents(); });

  // 等待事件处理
  std::this_thread::sleep_for(std::chrono::seconds(1));

  return 0;
}
