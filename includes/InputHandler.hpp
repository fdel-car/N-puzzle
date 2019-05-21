#pragma once

#include <iostream>
#include <regex>

const std::string errorFont = "\033[31;1m";
const std::string resetFont = "\033[0m";

class InputHandler {
 public:
  InputHandler(std::istream &is);
  virtual ~InputHandler(void);

  std::vector<u_char> firstGrid;

 private:
  int _lineCount;
  const std::regex _nbrRegex = std::regex("[0-9]+");

  InputHandler(void);
  InputHandler(InputHandler const &src);

  const std::string _errorString(const std::string &elem,
                                 const std::string &error) const;

  InputHandler &operator=(InputHandler const &rhs);
};
