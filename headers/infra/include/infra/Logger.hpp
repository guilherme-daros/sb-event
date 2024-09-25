#include <fmt/color.h>
#include <filesystem>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <source_location>
#include <sstream>
#include <string_view>
#include <thread>
#include <unordered_map>

#include "infra/StringLiteral.hpp"

namespace logger {

namespace utils {
inline auto get_pos(const std::string_view path, const uint64_t line) -> std::string_view {
  constexpr wchar_t separator = std::filesystem::path::preferred_separator;

  auto offset = path.rfind(separator) != std::string::npos ? path.rfind(separator) : 0;

  auto path_len = path.length() - offset;

  std::string_view filename{path.substr((offset + 1), path.length())};
  return filename;
}

inline void Uptime() {
  static auto start = std::chrono::steady_clock::now();

  std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000));

  auto now = std::chrono::steady_clock::now();

  auto uptime_s = std::chrono::duration_cast<std::chrono::seconds>(now - start);
  auto uptime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

  std::cout << "[" << std::setw(5) << std::right << uptime_s.count() << "." << std::setw(3) << std::setfill('0')
            << (uptime_ms.count() % 1000) << "]" << std::setfill(' ') << std::endl;
}

}  // namespace utils

class Adaptor {
 public:
  static auto output_stream() -> std::ostream& {
    static auto& pStream = std::cout;
    return pStream;
  }
};

class Base {
 public:
  enum class Level { None, Error, Warning, Info, Debug };

 protected:
  static auto to_string(Level level) -> std::string& {
    static auto buffer = std::unordered_map<Level, std::string>{
        {Level::None, "None"}, {Level::Debug, "D"}, {Level::Info, "I"}, {Level::Warning, "W"}, {Level::Error, "E"}};
    return buffer[(level >= Level::None && level <= Level::Debug) ? level : static_cast<Level>(0)];
  }
  static auto to_color(Level level) -> std::string& {
    static auto buffer = std::unordered_map<Level, std::string>{{Level::None, "None"},
                                                                {Level::Debug, "\033[1;39;44m"},
                                                                {Level::Info, "\033[1;39;42m"},
                                                                {Level::Warning, "\033[1;39;43m"},
                                                                {Level::Error, "\033[1;39;41m"}};

    return buffer[(level >= Level::None && level <= Level::Debug) ? level : static_cast<Level>(0)];
  }

};  // namespace log

template <typename Adaptor, StringLiteral domain>
class Logger : public Base {
 public:
  Logger() {}
  ~Logger() {}

  class sLog {
   public:
    sLog(Level level, std::ostream& output, const std::thread::id id = std::this_thread::get_id(),
         const std::source_location file_src = std::source_location::current())
        : level_(level), output_(output) {
      file_number_ = utils::get_pos(file_src.file_name(), file_src.line());
      id_ = id;
    }

    // Currently we dont support function templates from https://en.cppreference.com/w/cpp/io/manip
    // except for std::endl, but I dont know why

    // This captures all data_types
    template <typename T>
    sLog& operator<<(const T& token) {
      os << token;
      return *this;
    }

    // This deals with std::endl
    void operator<<(std::ostream& (*func)(std::ostream&)) {
      constexpr auto default_light = std::string_view{"\033[0;39;49m"};
      std::ostringstream context;

      context << "[" << std::setw(6) << " ]";
      context << " ";
      context << "[" << id_ << "]";
      context << " ";
      context << std::setw(20) << std::right << domain.data;
      context << " ";
      context << to_color(level_) << " " << to_string(level_) << " " << default_light;
      context << " ";
      context << std::setw(20) << std::left << file_number_;
      context << " ";

      output_ << context.str() << os.str() << std::endl;
      os.str("");
      os.clear();
    }

    std::ostringstream os;
    std::ostream& output_;
    std::string_view file_number_;
    std::thread::id id_;
    Level level_;
  };

  struct sDebug : public sLog {
    sDebug(const std::thread::id id = std::this_thread::get_id(),
           const std::source_location file_src = std::source_location::current())
        : sLog(Level::Debug, Adaptor::output_stream(), id, file_src) {}
  };

  struct sInfo : public sLog {
    sInfo(const std::thread::id id = std::this_thread::get_id(),
          const std::source_location file_src = std::source_location::current())
        : sLog(Level::Info, Adaptor::output_stream(), id, file_src) {}
  };

  struct sWarning : public sLog {
    sWarning(const std::thread::id id = std::this_thread::get_id(),
             const std::source_location file_src = std::source_location::current())
        : sLog(Level::Warning, Adaptor::output_stream(), id, file_src) {}
  };

  struct sError : public sLog {
    sError(const std::thread::id id = std::this_thread::get_id(),
           const std::source_location file_src = std::source_location::current())
        : sLog(Level::Error, Adaptor::output_stream(), id, file_src) {}
  };

  std::ostream& output_;
  std::ostringstream os;
};

}  // namespace logger
