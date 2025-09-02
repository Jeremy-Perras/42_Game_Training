#pragma once

#include <string>
#include <vector>
namespace ve {

  class Parsing {
  public:
    Parsing(const std::string &filepatht);
    Parsing(const Parsing &src) = delete;
    Parsing &operator=(const Parsing &rhs) = delete;
    ~Parsing() {};

    void initParsing(const std::string &filepath);
    // Getter
    std::vector<std::string> getMap() { return map_; }
    float getXStart() const { return xStart_; }
    float getYStart() const { return yStart_; }
    int getLineSize() const { return lineSize_; }
    int getColoneSize() const { return coloneSize_; }

  private:
    std::vector<std::string> map_;
    float xStart_;
    float yStart_;
    int lineSize_;
    int coloneSize_;
  };
}  // namespace ve