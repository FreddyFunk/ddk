#ifdef _WIN32
#include <Windows.h>
#include <stdio.h>
#endif

namespace DDK {
namespace Test {
namespace Windows {
#ifdef _WIN32
    bool canCreateWindowsSymlinks();
#endif
}
}
}