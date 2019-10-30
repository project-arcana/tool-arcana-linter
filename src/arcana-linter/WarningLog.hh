#pragma once

#include <clean-core/defer.hh>
//#include <clean-core/vector.hh>

#include <vector>
#include <memory>

#include "CodeFile.hh"
#include "diagnostics/DiagnosticBase.hh"

namespace arclint
{
class WarningLog
{
public:
    void registerFile(CodeFile const& file);

    void printSummary(double time_spent_ms) const;

    template<class DiagT>
    void addDiagnostic()
    {
        mDiagnostics.emplace_back(std::make_unique<DiagT>());
    }

private:
    int mNumTranslationUnits = 0;
    int mNumHeaders = 0;
    int mNumParseErrors = 0;
    std::vector<std::unique_ptr<diag::DiagnosticBase>> mDiagnostics;
};

}
