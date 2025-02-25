/*
 * Copyright (C) 2018-2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/os_interface/windows/sys_calls.h"

namespace NEO {

unsigned int getPid() {
    return GetCurrentProcessId();
}

bool isShutdownInProgress() {
    auto handle = GetModuleHandleA("ntdll.dll");

    if (!handle) {
        return true;
    }

    auto RtlDllShutdownInProgress = reinterpret_cast<BOOLEAN(WINAPI *)()>(GetProcAddress(handle, "RtlDllShutdownInProgress"));
    return RtlDllShutdownInProgress();
}

namespace SysCalls {

HANDLE createEvent(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName) {
    return CreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName);
}

BOOL closeHandle(HANDLE hObject) {
    return CloseHandle(hObject);
}

BOOL getSystemPowerStatus(LPSYSTEM_POWER_STATUS systemPowerStatusPtr) {
    return GetSystemPowerStatus(systemPowerStatusPtr);
}
BOOL getModuleHandle(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule) {
    return GetModuleHandleEx(dwFlags, lpModuleName, phModule);
}
DWORD getModuleFileName(HMODULE hModule, LPWSTR lpFilename, DWORD nSize) {
    return GetModuleFileName(hModule, lpFilename, nSize);
}
char *getenv(const char *variableName) {
    return ::getenv(variableName);
}
} // namespace SysCalls

} // namespace NEO
