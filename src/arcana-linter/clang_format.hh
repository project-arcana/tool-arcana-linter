#pragma once

namespace arclint
{
[[nodiscard]] bool is_clang_format_available(char const* binary);
[[nodiscard]] bool run_clang_format(char const* filename, char const* binary = "/usr/bin/clang-format");
}
