#pragma once

#include "DiagnosticBase.hh"

namespace arclint::diag
{
struct DPragmaOnce final : public DiagnosticBase
{
public:
    void registerFile(CodeFile const& file) override
    {
        if (file.isHeader && !file.hasPragmaOnce)
            addOffendingFile(file.getFileName());
    }

    /// Returns the name of this diagnostic
    [[nodiscard]] char const* getName() const override { return "Headers missing pragma once"; }
};
}
