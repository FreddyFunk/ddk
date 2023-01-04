#include "windows_helper.hpp"

namespace DDK {
namespace Test {
namespace Windows {
#ifdef _WIN32
        bool canCheckWindowsPermissions(){
            HANDLE token;
            if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
            {
                // The calling process has sufficient privileges to query the token
                CloseHandle(token);
                return true;
            }
            else
            {
                // The calling process does not have sufficient privileges to query the token
                CloseHandle(token);
                return false;
            }
        }
        bool canCreateWindowsSymlinks(){
            if (!canCheckWindowsPermissions())
            {
                return false;
            }
            HANDLE token;
            if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
            {
                LUID luid;
                if (LookupPrivilegeValue(nullptr, SE_CREATE_SYMBOLIC_LINK_NAME, &luid))
                {
                    TOKEN_PRIVILEGES tp;
                    tp.PrivilegeCount = 1;
                    tp.Privileges[0].Luid = luid;
                    tp.Privileges[0].Attributes = 0;

                    DWORD size;
                    if (AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), nullptr, &size))
                    {
                        CloseHandle(token);
                        return true;
                    }
                    else
                    {
                        CloseHandle(token);
                        return false;
                    }
                }
            }
            return false;
        }
#endif
}
}
}