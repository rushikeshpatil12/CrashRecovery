#pragma once

#define LOG_ERROR(_error_info) \
   wcout << _error_info << " Errorcode: " << GetLastError() << endl;

#define LOG_INFO(_info) \
   wcout << _info << endl;

#define back_space(n__)\
            do { \
                int bspace_count = n__;\
                while (bspace_count--) {\
                    cout << '\b';\
                }\
            } while (0);

extern bool g_shutdown;

class CrashRecovery
{
    static bool CrashRecovery::GetModuleInfo(wstring &Name, wstring &Path);
public:
    static void CrashMe();
    CrashRecovery() {};
    ~CrashRecovery() {};
    static bool Init();
    static bool SetCrashDumpSettings();
    static DWORD WINAPI RecoveryCallback(PVOID pContext);
    static BOOL WINAPI SystemCtrlCallback(DWORD dwControlEvent);
};

