#include "DBlacklistedSTL.hh"

#include <sstream>

void arclint::diag::DBlacklistedSTL::registerFile(const arclint::CodeFile& file)
{
    if (file.isHeader)
    {
        auto const blacklisted_includes = file.getBlacklistedSTLIncludes();
        if (!blacklisted_includes.empty())
        {
            std::stringstream ss;
            auto first = true;
            for (auto const& header : blacklisted_includes)
            {
                if (!first)
                    ss << ", ";
                ss << "<" << header << ">";
                if (first)
                    first = false;
            }

            addOffendingFile(file.getFileName(), ss.str(), int(blacklisted_includes.size()));
        }
    }
}
