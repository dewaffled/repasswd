#include "stdafx.h"
#include "userapi.h"
#include <array>

static const int NUM_PASSWORD_CHANGES = 25;
static const int PASSWORD_CHANGE_DELAY_MS = 1200;

int _tmain(int argc, TCHAR* argv[])
{
    std::array<TCHAR, UNLEN + 1> username;
    DWORD username_len = (DWORD)username.size();

    std::array<TCHAR, UNLEN + 1> domain;
    DWORD domain_len = (DWORD)domain.size();

    if (!GetCurrentUserAndDomain(username.data(), &username_len, domain.data(), &domain_len))
    {
        _tprintf(_T("Failed to detect user name\n"));
        exit(1);
    }

    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

    std::array<TCHAR, PWLEN + 1> password;
    DWORD password_len = (DWORD)password.size();

    _tprintf(_T("Enter current password for %s@%s:\n"), username.data(), domain.data());
    if (!ReadConsole(hStdin, password.data(), password_len - 1, &password_len, nullptr))
    {
        _tprintf(_T("Failed to read password\n"));
        exit(1);
    }
    SetConsoleMode(hStdin, mode);

    password[password_len] = 0;
    while (password_len > 0 && (password[password_len - 1] == '\n' || password[password_len - 1] == '\r'))
    {
        password[password_len - 1] = 0; // remove new-line characters
        --password_len;
    }

    std::array<TCHAR, PWLEN + 16> old_password;
    std::array<TCHAR, PWLEN + 16> new_password;

    _tcscpy_s(old_password.data(), old_password.size(), password.data());

    DWORD rc = 0;
    int i = 0;
    for (; i < NUM_PASSWORD_CHANGES; ++i)
    {
        _tprintf(_T("******-%d\n"), i);
        _sntprintf_s(new_password.data(), new_password.size(), _TRUNCATE, _T("%s-%d"), password.data(), i);
        DWORD rc = NetUserChangePassword(domain.data(), username.data(), old_password.data(), new_password.data());
        if (rc != 0)
        {
            break;
        }

        _tcscpy_s(old_password.data(), old_password.size(), new_password.data());
        Sleep(PASSWORD_CHANGE_DELAY_MS);
    }

    if (rc == 0)
    {
        rc = NetUserChangePassword(domain.data(), username.data(), old_password.data(), password.data());
    }

    if (rc != 0)
    {
        if (i > 0)
        {
            _tprintf(_T("Password change error: %d. Current password: %s\n"), rc, old_password.data());
        }
        else
        {
            _tprintf(_T("Password change error: %d."), rc);
        }
        getchar();
        exit(2);
    }

    _tprintf(_T("Password rearmed\n"));
    return 0;
}
