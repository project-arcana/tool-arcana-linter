#include "clang_format.hh"

#include <cstdlib>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <clean-core/assert.hh>

bool arclint::run_clang_format(const char* filename, const char* binary)
{
    int pid;
    int status;

    if ((pid = ::fork()))
    {
        // wait for the child to exit
        ::waitpid(pid, &status, 0);
    }
    else
    {
        // this is the child process
        ::execl(binary, binary, "-i", "-style=file", filename, nullptr);
        // Only reached on errors
        std::exit(1);
    }

    return status == 0;
}

bool arclint::is_clang_format_available(const char* binary)
{
    int pid;
    int status;

    if ((pid = ::fork()))
    {
        // wait for the child to exit
        ::waitpid(pid, &status, 0);
    }
    else
    {
        // this is the child process
        ::execl(binary, binary, "--version", nullptr);
        // Only reached on errors
        std::exit(1);
    }

    return status == 0;
}
