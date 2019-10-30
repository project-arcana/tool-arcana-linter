#include "CodeFile.hh"

#include <fstream>
#include <iostream>

#include <cppast/cpp_entity_kind.hpp>

#include <clean-core/assert.hh>

#include "file_util.hh"


#include <cppast/code_generator.hpp>         // for generate_code()
#include <cppast/cpp_entity_kind.hpp>        // for the cpp_entity_kind definition
#include <cppast/cpp_forward_declarable.hpp> // for is_definition()
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_namespace.hpp> // for cpp_namespace
#include <cppast/visitor.hpp>       // for visit()

namespace
{
[[nodiscard]] bool is_move_ctor_or_assign_op(cppast::cpp_function_base const& e)
{
    auto const is_ctor = e.kind() == cppast::cpp_entity_kind::constructor_t;
    auto const is_mem_func = e.kind() == cppast::cpp_entity_kind::member_function_t;

    if (!is_ctor && !is_mem_func)
        return false;

    int num_params = 0;
    bool is_first_param_rval_ref = false;
    for (auto const& param : e.parameters())
    {
        if (num_params == 0)
        {
            if (param.type().kind() == cppast::cpp_type_kind::reference_t
                && static_cast<cppast::cpp_reference_type const&>(param.type()).reference_kind() == cppast::cpp_ref_rvalue)
            {
                // technically this could be any type, TODO: check if the type is right
                is_first_param_rval_ref = true;
            }
        }

        ++num_params;
    }

    if (is_mem_func)
    {
        auto const& memf_ref = static_cast<cppast::cpp_member_function const&>(e);

        if (memf_ref.name() != "operator=")
            return false;
    }

    return num_params == 1 && is_first_param_rval_ref;
}

[[nodiscard]] bool is_binary_comparison_op(cppast::cpp_function_base const& e)
{
    if (e.kind() == cppast::cpp_entity_kind::member_function_t)
    {
        auto const& memf_ref = static_cast<cppast::cpp_member_function const&>(e);

        if (memf_ref.name() == "operator==" || memf_ref.name() == "operator!=")
            return true;
    }

    return false;
}

[[nodiscard]] bool is_missing_noexcept(cppast::cpp_entity const& e)
{
    if (cppast::is_function(e.kind()))
    {
        auto const& func_base_ref = static_cast<cppast::cpp_function_base const&>(e);
        auto const is_move_ctor_or_assign = is_move_ctor_or_assign_op(func_base_ref);
        auto const is_comp_operator = is_binary_comparison_op(func_base_ref);

        if (is_move_ctor_or_assign || is_comp_operator)
        {
            auto const noexcept_cond = func_base_ref.noexcept_condition();
            if (!noexcept_cond.has_value())
            {
                return true;
            }
        }
    }

    return false;
}
}

namespace
{
std::vector<std::string> whitelisted_stl_headers
    = {"utility", "type_traits", "atomic", "cstddef", "cstring", "cstdlib", "cstdint", "initializer_list"};
}

arclint::CodeFile::CodeFile(const fs::path& path) : mPath(path) { CC_ASSERT(has_cpp_file_extension(path)); }


void arclint::CodeFile::initialize(const arclint::AstParser& parser)
{
    CC_ASSERT(mInitialized == false);

    isHeader = has_cpp_header_extension(mPath);
    isExtensionCanonical = has_canonical_cpp_file_extension(mPath);

    try
    {
        mFileData = parser.parseFile(mPath.string());
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "Error parsing " << mPath.c_str() << '\n';
        std::cerr << "  " << e.what() << '\n';
    }

    if (mFileData)
    {
        // recursively visit file and all children
        cppast::visit(*mFileData, [&](const cppast::cpp_entity& e, cppast::visitor_info) {
            if (e.kind() == cppast::cpp_entity_kind::file_t || cppast::is_templated(e) || cppast::is_friended(e))
                // no need to do anything for a file,
                // templated and friended entities are just proxies, so skip those as well
                // return true to continue visit for children
                return true;

            if (e.kind() == cppast::cpp_entity_kind::include_directive_t)
            {
                mIncludedHeaders.push_back(e.name());
            }

            if (is_missing_noexcept(e))
            {
                mFunctionsMissingNoexcept.push_back(e.name());
            }

            return true;
        });

        std::ifstream in_file_raw(mPath.string());
        if (in_file_raw.good())
        {
            std::string first_line;
            std::getline(in_file_raw, first_line);
            hasPragmaOnce = first_line.rfind("#pragma once", 0) == 0;

            if (!hasPragmaOnce)
            {
                //                std::cout << "This line is not pragma once: " << first_line << std::endl;
            }
        }
        else
        {
            throw std::runtime_error("Failed to re-open file");
        }
    }

    mInitialized = true;
}

std::vector<std::string> arclint::CodeFile::getBlacklistedSTLIncludes() const
{
    std::vector<std::string> res;
    res.reserve(4);

    for (auto const& include : mIncludedHeaders)
    {
        if (include.find('.') == std::string::npos)
        {
            // include has no '.', thus likely an STL header
            if (std::find(whitelisted_stl_headers.begin(), whitelisted_stl_headers.end(), include) == whitelisted_stl_headers.end())
                // header not on the whitelist
                res.push_back(include);
        }
    }

    return res;
}
