#include "IReader.hpp"
#include <fstream>

FileReader::FileReader(std::string &fileName) : _name(fileName) {}

std::vector<char> FileReader::get() {
  std::ifstream file(_name, std::ios::binary | std::ios::in);
  if (file.good()) {
    std::vector<char> bytes((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

    return bytes;
  } else {
    throw std::runtime_error("File '" + _name + "' not found.");
  }
}

TestReader::TestReader(std::vector<char> &data) : _data(data) {}

std::vector<char> TestReader::get() {
  return _data;
}
