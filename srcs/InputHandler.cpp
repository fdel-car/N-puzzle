#include "InputHandler.hpp"
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
    bool hSelected = false;
    bool goalSelected = false;
    for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-h") == 0) {
        if (argc != 2)
          std::cout << "Warning: --help flag should be given alone."
                    << std::endl;
        _showHelp();
        _readyToParse = false;
        break;
      }
      if (strlen(argv[i]) == 2 && argv[i][0] == '-') {
        if (_fileOpened)
          throw invalid_usage("flag(s) must be given before the file.");
        auto it = Node::hMap.find(argv[i]);
        if (it != Node::hMap.end()) {
          if (hSelected)
            throw invalid_usage("only one heuristic can be selected.");
          Node::currHeuristic = it->second;
          hSelected = true;
        } else if (argv[i][1] == 's' || argv[i][1] == 'c') {
          if (goalSelected)
            throw invalid_usage("only one goal pattern can be used.");
          Puzzle::useSnailSolution = argv[i][1] == 's';
          goalSelected = true;
        } else {
          throw invalid_usage("invalid flag \"" + std::string(argv[i]) +
                              "\" given.");
        }
      } else {
        if (_fileOpened) throw invalid_usage("only one file can be supplied.");
        if (i == argc - 1) _openFileStream(argv[i]);
        _fileOpened = true;
      }
    }
  } catch (const invalid_usage &err) {
    _readyToParse = false;
    std::cerr << "Error: " << err.what() << std::endl;
    _printUsage();
  }
}

void InputHandler::_showHelp(void) const {
  std::cout << "Available heuristics:" << std::endl;
  std::cout << std::setw(30) << std::left << "  -m, --manhattan-distance";
  std::cout << "Use Manhattan distance, also called taxicab metric."
            << std::endl;
  std::cout << std::setw(30) << std::left << "  -l, --linear-conflict";
  std::cout << "Use linear conflict along with Manhattan distance (default)."
            << std::endl;
  std::cout << std::endl;
  std::cout << "Goal patterns:" << std::endl;
  std::cout << std::setw(30) << std::left << "  -s, --snail";
  std::cout << "Set the snail solution as the final grid state (default)."
            << std::endl;
  std::cout << std::setw(30) << std::left << "  -c, --classic";
  std::cout << "Use the classic solution, an ordered grid with a trailing 0."
            << std::endl;
}

void InputHandler::_printUsage(void) const {
  std::cerr << "./n_puzzle [-m|-l] [file]" << std::endl;
  std::cerr << "./n_puzzle [-h]" << std::endl;
}

void InputHandler::_openFileStream(const std::string &fileName) {
  struct stat st;
  _ifs.open(fileName, std::ios::in);
  if (_ifs.good()) {
    stat(fileName.c_str(), &st);
    if (st.st_mode & S_IFDIR) {
      _ifs.close();
      throw std::invalid_argument('"' + fileName + "\" is a directory.");
    }
  } else {
    std::string error(strerror(errno));
    error[0] = tolower(error[0]);
    throw std::invalid_argument(error + " (" + fileName + ").");
  }
}

void InputHandler::parseTiles(void) {
  if (!_readyToParse) return;
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
