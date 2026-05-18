#include "CsvParser.h"
#include <fstream>
#include <stdexcept>

namespace {

CsvParser::Row ParseLine(const std::string& line) {
  CsvParser::Row result;
  std::string field;

  for (char c : line) {
    if (c == ';') {
      result.push_back(field);
      field.clear();
    } else {
      field += c;
    }
  }

  result.push_back(field);
  return result;
}

}

CsvParser::Table CsvParser::Parse(const std::string& file_path) {
  std::ifstream file(file_path);
  if (!file.is_open())
    throw std::runtime_error("Не удалось открыть файл данных: " + file_path);

  Table result;
  std::string line;
  bool is_header = true;

  while (std::getline(file, line)) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line.empty()) continue;
    if (is_header) { is_header = false; continue; }
    result.push_back(ParseLine(line));
  }

  return result;
}
