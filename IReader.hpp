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
  std::string _name;
};

class TestReader : public IReader {
public:
  TestReader(std::vector<char> &data);
  std::vector<char> get();
  ~TestReader() = default;

private:
  std::vector<char> _data;
};

#endif
