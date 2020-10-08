#ifndef LIBBEAR_CORE_THREAD_H
#define LIBBEAR_CORE_THREAD_H

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <type_traits>

namespace libbear {

  class thread_pool {
  public:
    explicit thread_pool(std::size_t sz) : free_threads_{sz} {}

    template<typename T>
    std::future<T> async(const std::function<T()>& f) {
      return std::async([this, f]() {
                          acquire();
                          if constexpr (std::is_same_v<T, void>) {
                            f();
                            release();
                          } else {
                            const T res = f();
                            release();
                            return res;
                          }
                        });
    }

  private:
    void acquire();
    void release();

  private:
    std::mutex m_{};
    std::condition_variable cv_{};
    std::size_t free_threads_;
  };

} // namespace libbear

#endif // LIBBEAR_CORE_THREAD_H

