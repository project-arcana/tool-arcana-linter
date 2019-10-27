#include "clang_format.hh"

#include <sys/wait.h>
#include <unistd.h>

void arclint::run_clang_format(const char* filename, const char* binary)
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
    }
}
