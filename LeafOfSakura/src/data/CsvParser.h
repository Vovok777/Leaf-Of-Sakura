#pragma once
#include <string>
#include <vector>

namespace CsvParser {
  using Row   = std::vector<std::string>;
  using Table = std::vector<Row>;

  Table Parse(const std::string& file_path);
}
