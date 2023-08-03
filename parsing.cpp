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

    unsigned long lineS = map_[0].substr(0, map_[0].find('\n')).size();
    lineSize_ = static_cast<int>(lineS);
    coloneSize_ = 0;

    std::string::size_type n = 0;

    for (auto c : map_[0]) {
      if (c == '\n') {
        coloneSize_++;
      }
    }

    int lineStart = static_cast<int>(lineS) / 2;
    int coloneStart = static_cast<int>(coloneSize_) / 2;

    xStart_ = (-0.05F * static_cast<float>(lineStart) - 0.05F * (static_cast<float>(lineStart) + 1))
              / 2;
    yStart_
        = (-0.05F * static_cast<float>(coloneStart) - 0.05F * (static_cast<float>(coloneStart) + 1))
          / 2;

    while ((n = map_[0].find('\n')) != std::string::npos) {
      map_[0].erase(n, 1);
    }

    n = 0;

    while ((n = map_[1].find('\n')) != std::string::npos) {
      map_[1].erase(n, 1);
    }

    file.close();
  }

}  // namespace ve