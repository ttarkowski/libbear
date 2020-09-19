#ifndef LIBBEAR_CORE_SYSTEM_H
#define LIBBEAR_CORE_SYSTEM_H

#include <string>
#include <tuple>

namespace libbear {

  std::tuple<std::string, std::string> execute(const std::string& command);

} // namespace libbear

#endif // LIBBEAR_CORE_SYSTEM_H

