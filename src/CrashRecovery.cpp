#include "stdafx.h"
#include <shlwapi.h>
#include <atlbase.h>
#include "CrashRecovery.h"
#include "Settings.h"

/*
GetModuleInfo
-------------
Returns the current module name and path.

In: (none)
out: Name, Path. */
bool CrashRecovery::GetModuleInfo(wstring &Name, wstring &Path)
{
    wchar_t ModulePath[MAX_PATH] = {0};
    ModulePath[0] = 0;

    if (GetModuleFileNameW(NULL, ModulePath, MAX_PATH) == 0) {
        LOG_ERROR("Failed to get module path.");
        return false;
    }

    wstring FullPath(ModulePath);
    // get the dir excluding exe name.
    PathRemoveFileSpecW(ModulePath);
    wstring ModDir(ModulePath);
    Name = FullPath.substr(ModDir.length() + 1);
    Path = ModulePath;
    return true;
}

/*
CrashMe
-------------
Crash the process. */
void CrashRecovery::CrashMe()
{
    LOG_INFO(L"\n\n!!!CRASH!!! Access Violation.\n\n");
    int* ptr = NULL;
    *ptr = 0;
}

/*
Init()
------
Initializes:
- Register app with WER for restart on crash.
- Set callback on system control events(e.g. Installer requesting shutdown for update).
- Register callback for recovery handling.
- Set the WER crash dump settings such as dump location, dump type, dump counts. */
bool CrashRecovery::Init()
{
    HRESULT hr = S_OK;
    bool status = false;

    // register for app restart on crash, hang, etc.
    hr = RegisterApplicationRestart(nullptr, RESTART_NO_PATCH | RESTART_NO_REBOOT);
    if (FAILED(hr))
    {
        LOG_INFO("RegisterApplicationRestart failed. Err: " << hr);
        goto cleanup;
    }

    // set ctrl event handler.
    if (!SetConsoleCtrlHandler(SystemCtrlCallback, TRUE))
    {
        LOG_INFO("SetConsoleCtrlHandler failed.");
        goto cleanup;
    }

    // Register callback for recovery handling. Typically crashdump/logs saving/uploading
    // shall be done here. Also notify WER that application recovery work is complete.
    hr = RegisterApplicationRecoveryCallback(RecoveryCallback, nullptr, Settings::CrashRecoveryIntervalInSec, 0);
    if (FAILED(hr))
    {
        LOG_INFO(L"RegisterApplicationRecoveryCallback failed. Err: " << hr);
        goto cleanup;
    }

    // Set the WER crash dump settings
    if (!SetCrashDumpSettings()) {

        LOG_ERROR("SetCrashDumpLocation failed.");
        goto cleanup;
    }

    status = true;

cleanup:
    return status;
}


/*
SetCrashDumpSettings()
----------------------
Update WER settings for crash dump.
- Crashdump Location: This is where WER will create crash dump.
- Dump Type: Specify Mini, Full, Custom dump.
- Dump Count: In case multiple crashes specify how many crashdumps to be saved. */
bool CrashRecovery::SetCrashDumpSettings()
{
    static const wstring WerLocalDump = L"SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps";
    bool status = false;

    wstring ExeName, ExePath;
    if (!GetModuleInfo(ExeName, ExePath))
    {
        LOG_ERROR("GetModuleInfo failed");
        return false;
    }

    // destructor will close the regKey & regSubKey.
    CRegKey regKey;
    CRegKey regSubKey;
    DWORD   dwValue = 0;

    if (ERROR_SUCCESS != regKey.Create(HKEY_LOCAL_MACHINE, WerLocalDump.c_str(), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_WOW64_64KEY))
    {
        LOG_ERROR("CRegKey::Open failed. ");
        return false;
    }

# if 0 // Setting could change. Until change detection is in place overwrite everytime.
    if (ERROR_SUCCESS == regSubKey.Open(regKey, ExeName.c_str(), KEY_READ | KEY_WOW64_64KEY))
    {
        LOG_INFO("Crash dump location already set. ");
        // SUCCESS...
        return true;;
    }
#endif

    // create the crash dump keys.
    if (ERROR_SUCCESS != regSubKey.Create(regKey, ExeName.c_str(), REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_WOW64_64KEY)) {
        LOG_ERROR("CRegKey::Create failed. ");
        return false;
    }

    // crash dump location.
    if (ERROR_SUCCESS != regSubKey.SetStringValue(L"DumpFolder", ExePath.c_str(), REG_SZ)) {
        LOG_ERROR("CRegKey::SetValue failed. ");
        return false;
    }

    // Mini-dump.
    if (ERROR_SUCCESS != regSubKey.SetDWORDValue(L"DumpType", (DWORD)Settings::dmpType)) {
        LOG_ERROR("CRegKey::SetValue(DumpType) failed. ");
        return false;
    }

    // Save only 1 crashdump. New crashes will overwrite the previous crash dump.
    if (ERROR_SUCCESS != regSubKey.SetDWORDValue(L"DumpCount", (DWORD)Settings::CrashDumpCount)) {
        LOG_ERROR("CRegKey::SetValue(DumpCount) failed. ");
        return false;
    }

    return true;
}

/*
RecoveryCallback()
------------------
Update WER settings for crash dump.
- Crashdump Location: This is where WER will create crash dump.
- Dump Type: Specify Mini, Full, Custom dump.
- Dump Count: In case multiple crashes specify how many crashdumps to be saved.
*/
DWORD CrashRecovery::RecoveryCallback(PVOID pContext)
{
    HRESULT hr = S_OK;
    BOOL bCanceled = FALSE;

    // Do recovery work.
    LOG_INFO("Recovery callback...");
    hr = RegisterApplicationRestart(nullptr, RESTART_NO_PATCH | RESTART_NO_REBOOT);
    if (FAILED(hr))
    {
        LOG_INFO("RegisterApplicationRestart failed with " << hr);
    }

    // Notify app recovery is in progress.
    hr = ApplicationRecoveryInProgress(&bCanceled);
    if (bCanceled)
    {
        LOG_INFO("Recovery was canceled by the user.");
        goto cleanup;
    }

    // Do more recovery work.

    uint32_t RecoveryTimeInSec = Settings::CrashRecoveryIntervalInSec / 1000;
    RecoveryTimeInSec--;

    cout << "Performing crash recovery. (" << std::setw(4) << std::setfill('0')
        << RecoveryTimeInSec << " sec)...";

    for (int32_t i = (int32_t )RecoveryTimeInSec; i >= 0; i--) {
        back_space(10);
        cout << std::setw(2) << std::setfill('0') << i << " sec)...";
        Sleep(1000);

    }
cleanup:

    // Save state of the crash/dumpfile, etc. if required.
    // ...

    LOG_INFO("\nRecovery finished.");
    LOG_INFO("Restarting the app");
    ApplicationRecoveryFinished((bCanceled) ? FALSE : TRUE);

    return 0;
}

/*
SystemCtrlCallback()
------------------
Callback routine to get notificaiton for system events.
- CTRL_C_EVENT: Ctrl+C by user or Installer tries to stop the app for update.
- CTRL_LOGOFF_EVENT:
- CTRL_SHUTDOWN_EVENT: */
BOOL CrashRecovery::SystemCtrlCallback(DWORD dwControlEvent)
{
    LOG_INFO(L"\nSystemCtrlCallback invoked. EventType: " << dwControlEvent);

    switch (dwControlEvent)
    {
    case CTRL_C_EVENT:
        // Application shutdown requested. System sends Ctr+C for installer(update) events.
        LOG_INFO("Shutdown requested.");
        LOG_INFO("Press Enter to exit...");
        g_shutdown = true; // notify other components of the app.
        getchar();
        ExitProcess(0);

    // other system events.
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    default:
        return FALSE;
    }

    return 0;
}
