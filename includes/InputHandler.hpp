#pragma once

#include <sys/stat.h>
#include <iostream>
#include <regex>

struct invalid_usage : public std::logic_error {
 public:
  invalid_usage(const std::string &msg) : std::logic_error(msg) {}
  invalid_usage(const char *msg) : std::logic_error(msg) {}
};

class InputHandler {
 public:
  InputHandler(int argc, char **argv);
  virtual ~InputHandler(void);

  std::vector<u_char> startGrid;

  void parseTiles(void);

 private:
  int _lineCount;
  bool _hSelected = false;
  bool _goalSelected = false;
  bool _algoSelected = false;
  bool _fileOpened = false;
  bool _readyToParse = true;
  const std::regex _nbrRegex = std::regex("[0-9]+");
  std::ifstream _ifs;

  InputHandler(void);
  InputHandler(InputHandler const &src);

  void _parseFlags(const std::string &flags);
  void _showHelp(void) const;
  void _printUsage(void) const;
  void _openFileStream(const std::string &fileName);
  const std::string _errorString(const std::string &elem,
                                 const std::string &error) const;

  InputHandler &operator=(InputHandler const &rhs);
};
