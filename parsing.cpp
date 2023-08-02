#include "parsing.hpp"

namespace ve {
  Parsing::Parsing(const std ::string &filepath) { initParsing(filepath); }

  void Parsing::initParsing(const std::string &filepath) {
    std::ifstream file(filepath);
    std::string line;
    int i = 0;

    map_.resize(3);

    if (!file.is_open()) {
      throw std::runtime_error(std::string{"failed to open file:"} + filepath);
    }
    while (file) {
      std::getline(file, line);
      if (line.empty()) {
        i++;
      } else {
        map_[i].append(line);
        map_[i].append("\n");
      }
    }
    file.close();
  }

}  // namespace ve