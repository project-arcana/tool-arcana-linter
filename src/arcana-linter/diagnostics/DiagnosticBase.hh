#pragma once

#include <string_view>
#include <vector>

#include <arcana-linter/CodeFile.hh>

namespace arclint::diag
{
class DiagnosticBase
{
public:
    virtual ~DiagnosticBase() = default;

    virtual void registerFile(CodeFile const& file) = 0;

    /// Returns the name of this diagnostic
    [[nodiscard]] virtual char const* getName() const = 0;

public:
    [[nodiscard]] int getNumOffenses() const { return mNumOffenses; }
    [[nodiscard]] bool isEmpty() const { return mNumOffenses == 0; }
    void printSummary() const;

protected:
    void addOffendingFile(std::string_view path, std::string_view reason = "", int offense_count = 1);

private:
    std::vector<std::string> mOffendingFiles;
    int mNumOffenses = 0;
};

}
