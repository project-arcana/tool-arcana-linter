#pragma once

#include "DiagnosticBase.hh"

namespace arclint::diag
{
struct DBlacklistedSTL final : public DiagnosticBase
{
public:
    void registerFile(CodeFile const& file) override;

    /// Returns the name of this diagnostic
    [[nodiscard]] char const* getName() const override { return "Blacklisted STL headers"; }
};
}
