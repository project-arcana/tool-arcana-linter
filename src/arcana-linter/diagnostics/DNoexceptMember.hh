#pragma once

#include "DiagnosticBase.hh"

namespace arclint::diag
{
struct DNoexceptMember final : public DiagnosticBase
{
public:
    void registerFile(CodeFile const& file) override;

    /// Returns the name of this diagnostic
    [[nodiscard]] char const* getName() const override { return "Functions missing noexcept specifier"; }
};
}
