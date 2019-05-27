#include "InputHandler.hpp"
#include <sys/stat.h>
#include "Puzzle.hpp"

static const std::string errorFont = "\033[31;1m";
static const std::string resetFont = "\033[0m";

static void ltrim(std::string &str) {
  str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
              return !std::isspace(ch);
            }));
}
static void rtrim(std::string &str) {
  str.erase(std::find_if(str.rbegin(), str.rend(),
                         [](int ch) { return !std::isspace(ch); })
                .base(),
            str.end());
}
static void trim(std::string &str) {
  ltrim(str);
  rtrim(str);
}

static std::string removeExtraSpaces(const std::string &input) {
  std::string output;
  std::unique_copy(input.begin(), input.end(),
                   std::back_insert_iterator<std::string>(output),
                   [](char a, char b) { return isspace(a) && isspace(b); });
  return output;
}

InputHandler::InputHandler(int argc, char **argv) {
  try {
    for (int i = 1; i < argc; i++) {
      if (strlen(argv[i]) == 2 && argv[i][0] == '-') {
        // TODO: Only set the heuristic once, otherwise throw an error
        auto it = Node::hMap.find(argv[i]);
        if (it != Node::hMap.end()) {
          Node::currHeuristic = it->second;
        }
      } else if (!_fileOpened) {
        if (i < argc - 1)
          throw std::runtime_error("too many arguments passed to the program.");
        struct stat st;
        _ifs.open(argv[i], std::ios::in);
        if (_ifs.good()) {
          stat(argv[i], &st);
          if (st.st_mode & S_IFDIR) {
            _ifs.close();
            throw std::invalid_argument("Error: " + std::string(argv[i]) +
                                        " is a directory.");
          }
          _fileOpened = true;
        } else {
          throw std::invalid_argument("Error: " + std::string(strerror(errno)) +
                                      " (" + argv[i] + ").");
        }
      }
    }
  } catch (const std::runtime_error &err) {
    _invalidInput = true;
    std::cerr << "Error: " << err.what() << std::endl;
    printUsage();
  }
}

void InputHandler::printUsage(void) const {
  std::cerr << "./n_puzzle [-m|-l] [file]" << std::endl;
  std::cerr << "./n_puzzle [-h]" << std::endl;
}

void InputHandler::parseTiles(void) {
  if (_invalidInput) return;
  std::string line, tile;
  bool firstLineParsed = false;
  size_t spacePos, searchStart = 0;
  int x = 0, y = 0;
  _lineCount = 0;
  std::istream &is = (!_fileOpened) ? std::cin : _ifs;

  while (std::getline(is, line)) {
    _lineCount++;
    // Remove everything after the hash
    size_t hash = line.find('#');
    line = line.substr(0, hash);
    trim(line);
    line = removeExtraSpaces(line);

    if (line.empty()) continue;
    if (!firstLineParsed) {
      if (std::regex_match(line, _nbrRegex)) {
        Puzzle::N = std::stoi(line);
        Puzzle::totalSize = Puzzle::N * Puzzle::N;
        Puzzle::nbrLength = std::to_string(Puzzle::totalSize).length();
        startGrid.resize(Puzzle::totalSize, 0);
      } else
        throw std::invalid_argument(
            _errorString(line, " is not a valid integer value."));
      firstLineParsed = true;
    } else {
      for (x = 0; x < Puzzle::N; x++) {
        spacePos = line.find(' ', searchStart);
        if (spacePos == std::string::npos && x < Puzzle::N - 1)
          throw std::logic_error(
              _errorString(line, ", missing one (or more) value(s)."));
        else if (spacePos != std::string::npos && x == Puzzle::N - 1)
          throw std::logic_error(
              _errorString(line, ", too many values inside this row."));
        tile = line.substr(searchStart, spacePos - searchStart);
        if (std::regex_match(tile, _nbrRegex)) {
          int value = std::stoi(tile);
          if (std::numeric_limits<u_char>::min() > value ||
              std::numeric_limits<u_char>::max() < value) {
            throw std::out_of_range("stouc: out of range");
          }
          startGrid[x + y * Puzzle::N] = static_cast<u_char>(value);
        } else
          throw std::invalid_argument(
              _errorString(tile, " is not a valid integer value."));
        searchStart = spacePos + 1;
      }
      y++;
    }
  }
  if (_fileOpened) _ifs.close();
}

InputHandler::~InputHandler(void) {}

const std::string InputHandler::_errorString(const std::string &elem,
                                             const std::string &error) const {
  return ("line " + std::to_string(_lineCount) + ": " + errorFont + elem +
          resetFont + error);
}
