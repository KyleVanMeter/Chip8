#include "IReader.hpp"
#include <fstream>

FileReader::FileReader(std::string &fileName) : name(fileName) {}

std::vector<char> FileReader::get() {
  std::ifstream file(name, std::ios::binary | std::ios::in);
  if (file.good()) {
    std::vector<char> bytes((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

    return bytes;
  } else {
    throw std::runtime_error("File '" + name + "' not found.");
  }
}
