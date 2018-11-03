// RecoveryApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CrashRecovery.h"
#include "Settings.h"


bool g_shutdown = false;

/* Check if the app is running with admin privilege. */
bool IsAppElevated() {
    bool isElevation = false;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            isElevation = (0 != Elevation.TokenIsElevated);
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return isElevation;
}


void DoSomeWork() {

    // Minimum 60s. WER requires app to run for, at least, 60 seconds to avoid cyclic restart of the app.
    uint32_t work_interval = (Settings::WorkIntervalInSec > (uint32_t)60) ?  Settings::WorkIntervalInSec : 61;

    // assume WorkIntervalInSec < 100;

    cout << "Performing some work. (" << std::setw(4) << std::setfill('0') << work_interval << " sec)...";
    // WER requires minimum of 60 seconds of app runtime. This is to avoid cyclic restart of the app.
    for (int32_t i = (int32_t)work_interval -1; i >= 0; i--) {
        if (!g_shutdown)
        {
            back_space(10);
            cout << std::setw(2) << std::setfill('0') << i << " sec)...";
        }
        Sleep(1000);
    }

}
int main()
{
    // App uses registry & WER settings thus needs admin rights.
    if (!IsAppElevated())
    {
        LOG_INFO("Admin privileges required. Run as admin again.");
        return 0;
    }

    LOG_INFO(L"-------------------------------------------------------");
    LOG_INFO(L"Welcome to crash recovery Demo. (Press Ctr+C to exit).");
    LOG_INFO(L"-------------------------------------------------------");
    if (!CrashRecovery::Init())
    {
        LOG_ERROR("Init Failed. Exiting");
        return -1;
    }

    DoSomeWork();

    CrashRecovery::CrashMe();

    return 0;
}

