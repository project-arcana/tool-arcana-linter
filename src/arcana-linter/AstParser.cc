#include "AstParser.hh"

arclint::AstParser::AstParser(const cxxopts::ParseResult& options)
{
    if (options.count("verbose"))
        mConfig.write_preprocessed(true);

    if (options.count("fast_preprocessing"))
        mConfig.fast_preprocessing(true);

    if (options.count("remove_comments_in_macro"))
        mConfig.remove_comments_in_macro(true);

    if (options.count("include_directory"))
        for (auto& include : options["include_directory"].as<std::vector<std::string>>())
            mConfig.add_include_dir(include);
    if (options.count("macro_definition"))
        for (auto& macro : options["macro_definition"].as<std::vector<std::string>>())
        {
            auto equal = macro.find('=');
            auto name = macro.substr(0, equal);
            if (equal == std::string::npos)
                mConfig.define_macro(std::move(name), "");
            else
            {
                auto def = macro.substr(equal + 1u);
                mConfig.define_macro(std::move(name), std::move(def));
            }
        }

    if (options.count("macro_undefinition"))
        for (auto& name : options["macro_undefinition"].as<std::vector<std::string>>())
            mConfig.undefine_macro(name);
    if (options.count("feature"))
        for (auto& name : options["feature"].as<std::vector<std::string>>())
            mConfig.enable_feature(name);

    // the compile_flags are generic flags
    cppast::compile_flags flags;
    if (options.count("gnu_extensions"))
        flags |= cppast::compile_flag::gnu_extensions;
    if (options.count("msvc_extensions"))
        flags |= cppast::compile_flag::ms_extensions;
    if (options.count("msvc_compatibility"))
        flags |= cppast::compile_flag::ms_compatibility;

    mConfig.set_flags(cppast::cpp_standard::cpp_1z);

    if (options.count("verbose"))
        mLogger.set_verbose(true);
}

std::unique_ptr<cppast::cpp_file> arclint::AstParser::parseFile(const std::string& filename) const
{
    cppast::cpp_entity_index idx;
    cppast::libclang_parser parser(type_safe::ref(mLogger));
    return parser.parse(idx, filename, mConfig);
}
