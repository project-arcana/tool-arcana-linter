#include "DNoexceptMember.hh"

#include <sstream>

void arclint::diag::DNoexceptMember::registerFile(const arclint::CodeFile& file)
{
    if (file.isHeader)
    {
        auto const funcs_missing_noexcept = file.getFunctionsMissingNoexcept();
        if (!funcs_missing_noexcept.empty())
        {
            std::stringstream ss;
            auto first = true;
            for (auto const& func : funcs_missing_noexcept)
            {
                if (!first)
                    ss << ", ";
                ss << func;
                if (first)
                    first = false;
            }

            addOffendingFile(file.getFileName(), ss.str(), int(funcs_missing_noexcept.size()));
        }
    }
}
