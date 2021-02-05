#ifndef __IREADER_H__
#define __IREADER_H__

#include <vector>
#include <string>

class IReader {
public:
  virtual ~IReader() {};
  virtual std::vector<char> get() = 0;
};

class FileReader : public IReader {
public:
  FileReader(std::string &fileName);
  std::vector<char> get();
  ~FileReader() = default;

private:
  std::string name;
};

class TestReader : public IReader {
  TestReader(std::string &fileName);
  std::vector<char> get();
  ~TestReader() = default;
};

#endif
