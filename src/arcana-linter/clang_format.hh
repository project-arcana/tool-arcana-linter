#pragma once

namespace arclint
{
[[nodiscard]] bool run_clang_format(char const* filename, char const* binary = "/usr/bin/clang-format");
}
