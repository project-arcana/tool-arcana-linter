#pragma once

#include <filesystem> // TODO cc::filesystem
#include <iostream>
#include <string> // TODO cc::string

namespace fs = std::filesystem;

namespace arclint
{
/// Calls func with all paths at or within the given one, returns false iff no regular files are found
template <class F>
bool iterate_regular_files(std::string const& root, F func)
{
    auto const path = fs::path(root);
    if (fs::is_directory(path))
    {
        for (auto const& p : fs::recursive_directory_iterator(path))
        {
            if (p.is_regular_file())
            {
                if (!func(p.path()))
                    break;
            }
        }

        return true;
    }
    else if (fs::is_regular_file(path))
    {
        func(path);
        return true;
    }
    return false;
}

/// Returns true if the file's extension indicates that it is a C++/C file
[[nodiscard]] inline bool has_cpp_file_extension(fs::path const& path)
{
    auto const cpp_file_exts = {".cpp", ".cxx", ".c", ".cc", ".hpp", ".h", ".hxx", ".hh", ".inl"};
    auto const ext = path.extension().string();

    for (auto const& cpp_ext : cpp_file_exts)
        if (ext.compare(cpp_ext) == 0)
            return true;


    return false;
}

/// Returns true if the file's extension indicates that it is a C++/C header
[[nodiscard]] inline bool has_cpp_header_extension(fs::path const& path)
{
    auto const cpp_file_exts = {".hpp", ".h", ".hxx", ".hh"};
    auto const ext = path.extension().string();

    for (auto const& cpp_ext : cpp_file_exts)
        if (ext.compare(cpp_ext) == 0)
            return true;


    return false;
}

/// Returns true if the file's extension is canonical (cc/hh)
[[nodiscard]] inline bool has_canonical_cpp_file_extension(fs::path const& path)
{
    auto const canonical_cpp_file_exts = {".cc", ".hh"};
    auto const ext = path.extension().string();

    for (auto const& cpp_ext : canonical_cpp_file_exts)
        if (ext.compare(cpp_ext) == 0)
            return true;

    return false;
}

}
