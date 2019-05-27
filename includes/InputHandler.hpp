#pragma once

#include <fstream>
#include <iostream>
#include <regex>

class InputHandler {
 public:
  InputHandler(int argc, char **argv);
  virtual ~InputHandler(void);

  std::vector<u_char> startGrid;

  void parseTiles(void);

 private:
  int _lineCount;
  bool _fileOpened = false;
  bool _invalidInput = false;
  const std::regex _nbrRegex = std::regex("[0-9]+");
  std::ifstream _ifs;

  InputHandler(void);
  InputHandler(InputHandler const &src);

  void printUsage(void) const;
  const std::string _errorString(const std::string &elem,
                                 const std::string &error) const;

  InputHandler &operator=(InputHandler const &rhs);
};
