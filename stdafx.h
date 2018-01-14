#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

// Target minimum OS version: Windows 7 and Windows Server 2008 R2
#define _WIN32_WINNT 0x0601
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <lm.h>

#pragma  comment(lib, "Netapi32.lib")
