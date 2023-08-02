#pragma once

#include "game_object.hpp"
namespace ve {

  class Parsing {
  public:
    Parsing(const std::string &filepatht);
    Parsing(const Parsing &src) = delete;
    Parsing &operator=(const Parsing &rhs) = delete;
    ~Parsing(){};

    void initParsing(const std::string &filepath);
    // Getter
    std::vector<std::string> getMap() { return map_; }

  private:
    std::vector<std::string> map_;
  };
}  // namespace ve