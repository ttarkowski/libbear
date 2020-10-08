#include <mutex>
#include <libbear/core/thread.h>

void
libbear::thread_pool::
acquire() {
  std::unique_lock<std::mutex> ul{m_};
  cv_.wait(ul, [this]() { return free_threads_ != 0; });
  --free_threads_;
}

void
libbear::thread_pool::
release() {
  std::unique_lock<std::mutex> ul{m_};
  ++free_threads_;
  cv_.notify_one();
}

