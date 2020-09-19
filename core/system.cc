#include <future>
#include <string>
#include <tuple>
#include <boost/asio/io_context.hpp>
#include <boost/process/async.hpp>
#include <boost/process/child.hpp>
#include <boost/process/io.hpp>
#include <libbear/core/system.h>

std::tuple<std::string, std::string>
libbear::execute(const std::string& command) {
  boost::asio::io_context ioc{};
  std::future<std::string> out{};
  std::future<std::string> err{};
  boost::process::child c{command.c_str(),
    boost::process::std_in.close(),
    boost::process::std_out > out,
    boost::process::std_err > err,
    ioc};
  ioc.run();
  return std::tuple<std::string, std::string>{out.get(), err.get()};
}

