#include "InputHandler.hpp"
#include "Puzzle.hpp"

static const std::string errorFont = "\033[31;1m";
static const std::string warningFont = "\033[33m";
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
      if (!_readyToParse) break;
      std::string currArg(argv[i]);
      if (currArg[0] == '-') {
        // TODO: Handle full name flag like '--help'
        _parseFlags(currArg.substr(1));
      } else {
        if (_fileOpened) throw invalid_usage("only one file can be supplied.");
        _openFileStream(currArg);
      }
    }
  } catch (const invalid_usage &err) {
    _readyToParse = false;
    std::cerr << "Error: " << err.what() << std::endl;
    _printUsage();
  }
}

void InputHandler::_parseFlags(const std::string &flags) {
  for (auto flag : flags) {
    if (flag == 'h') {
      _showHelp();
      _readyToParse = false;
      continue;
      // std::cout << warningFont << "Warning" << resetFont
      // << ": --help flag should be given alone." << std::endl;
    }
    auto itHMap = Node::hMap.find(flag);
    if (itHMap != Node::hMap.end()) {
      if (_hSelected)
        throw invalid_usage("only one heuristic can be selected.");
      Node::currHeuristic = itHMap->second;
      _hSelected = true;
      continue;
    }
    if (Puzzle::updateGoalPattern(flag)) {
      if (_goalSelected)
        throw invalid_usage("only one goal pattern can be selected.");
      _goalSelected = true;
      continue;
    }
    auto itAMap = Puzzle::algoMap.find(flag);
    if (itAMap != Puzzle::algoMap.end()) {
      if (_algoSelected)
        throw invalid_usage("only one type of algorithm can be selected.");
      if (itAMap->second == Puzzle::UniformCost && _hSelected)
        std::cout << warningFont << "Warning" << resetFont
                  << ": heuristic choice ignored with --uniform-cost."
                  << std::endl;
      Puzzle::currAlgorithm = itAMap->second;
      _algoSelected = true;
      continue;
    }
    std::string dashedFlag("-");
    dashedFlag += flag;
    throw invalid_usage("invalid flag \"" + dashedFlag + "\" given.");
  }
}

void InputHandler::_showHelp(void) const {
  _printUsage();
  std::cout << std::endl;
  // TODO: Dynamically build the help based on static flags data
  std::cout << "Goal patterns:" << std::endl;
  std::cout << std::setw(30) << std::left << "  -s, --snail";
  std::cout << "Set the snail solution as the final grid state (default)."
            << std::endl;
  std::cout << std::setw(30) << std::left << "  -c, --classic";
  std::cout << "Use the classic solution, an ordered grid with a trailing 0."
            << std::endl;

  std::cout << std::endl;
  std::cout << "Available heuristics:" << std::endl;
  std::cout << std::setw(30) << std::left << "  -H, --hamming-distance";
  std::cout << "Use Hamming distance, corresponds to the number of misplaced "
               "tiles, slowest heuristic."
            << std::endl;
  std::cout << std::setw(30) << std::left << "  -m, --manhattan-distance";
  std::cout << "Use Manhattan distance, also called taxicab metric."
            << std::endl;
  std::cout << std::setw(30) << std::left << "  -l, --linear-conflict";
  std::cout << "Use linear conflict along with Manhattan distance, fastest "
               "heuristic (default)."
            << std::endl;

  std::cout << std::endl;
  std::cout << "Type of algorithms:" << std::endl;
  std::cout << std::setw(30) << std::left << "  -A, --a-star";
  std::cout << "Both the expected and the past cost are considered during the "
               "search (default)."
            << std::endl;
  std::cout << std::setw(30) << std::left << "  -g, --greedy";
  std::cout << "The past cost will be ignored, only considering expected cost, "
               "fastest algorithm but the result won't be optimal."
            << std::endl;
  std::cout << std::setw(30) << std::left << "  -u, --uniform-cost";
  std::cout << "All states will have the same expected cost (discard heuristic "
               "choice), so all nodes will be considered, slowest algorithm."
            << std::endl;

  std::cout << std::endl;
  std::cout << std::setw(30) << std::left << "  -h, --help";
  std::cout << "Show this help list." << std::endl;
}

void InputHandler::_printUsage(void) const {
  std::cerr
      << "Usage: ./n_puzzle [-h] [-H | -m | -l] [-s | -c] [-a | -g | -u] [file]"
      << std::endl;
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
  _fileOpened = true;
}

void InputHandler::_normalizeLine(std::string &line) {
  if (line.empty()) return;
  // Remove everything after the hash
  size_t hash = line.find('#');
  line = line.substr(0, hash);
  trim(line);
  line = removeExtraSpaces(line);
}

int InputHandler::_tryToParseInt(const std::string &str) {
  if (std::regex_match(str, _nbrRegex))
    return std::stoi(str);
  else
    throw std::invalid_argument(
        _errorString(str, " is not a valid integer value."));
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
    _normalizeLine(line);
    if (line.empty()) continue;

    if (!firstLineParsed) {
      Puzzle::N = _tryToParseInt(line);
      Puzzle::totalSize = Puzzle::N * Puzzle::N;
      Puzzle::nbrLength = std::to_string(Puzzle::totalSize).length();
      startGrid.resize(Puzzle::totalSize, 0);
      firstLineParsed = true;
    } else {
      if (y >= Puzzle::N)
        throw std::logic_error(
            "too many lines given (use '#' if you want to add some "
            "comments).");
      for (x = 0; x < Puzzle::N; x++) {
        spacePos = line.find(' ', searchStart);
        tile = line.substr(searchStart, spacePos - searchStart);
        int value = _tryToParseInt(tile);
        if (spacePos == std::string::npos && x < Puzzle::N - 1)
          throw std::logic_error(
              _errorString(line, ", missing one (or more) value(s)."));
        else if (spacePos != std::string::npos && x == Puzzle::N - 1)
          throw std::logic_error(
              _errorString(line, ", too many values inside this row."));
        if (std::numeric_limits<u_char>::min() > value ||
            std::numeric_limits<u_char>::max() < value) {
          throw std::out_of_range("stouc: out of range");
        }
        if (value >= Puzzle::totalSize)
          throw std::logic_error(_errorString(
              tile, " is bigger than the max tile value for this puzzle."));
        startGrid[x + y * Puzzle::N] = static_cast<u_char>(value);
        searchStart = spacePos + 1;
      }
      y++;
    }
  }
  if (_fileOpened) _ifs.close();
  _verifyGrid();
}

void InputHandler::_verifyGrid(void) {
  std::vector<bool> bitset(Puzzle::totalSize, true);
  for (auto tile : startGrid) {
    int idx = static_cast<int>(tile);
    if (bitset[idx])
      bitset[idx] = false;
    else
      throw std::logic_error("duplicated values inside the grid.");
  }
}

InputHandler::~InputHandler(void) {}

const std::string InputHandler::_errorString(const std::string &elem,
                                             const std::string &error) const {
  return ("line " + std::to_string(_lineCount) + ": " + errorFont + elem +
          resetFont + error);
}
