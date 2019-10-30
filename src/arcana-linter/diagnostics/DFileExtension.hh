#pragma once

#include "DiagnosticBase.hh"

namespace arclint::diag
{
struct DFileExtension final : public DiagnosticBase
{
public:
    void registerFile(CodeFile const& file) override
    {
        if (!file.isExtensionCanonical)
            addOffendingFile(file.getFileName());
    }

    /// Returns the name of this diagnostic
    [[nodiscard]] char const* getName() const override { return "Noncanonical File Extension"; }
};
}
