#pragma once
enum DumpType {
    CustomDump,
    MiniDump,
    FullDump
};

class Settings
{
public:
    Settings() {};
    ~Settings() {};

    static uint32_t WorkIntervalInSec;
    static uint32_t CrashRecoveryIntervalInSec;
    // // Number of crash dumps to preserve.
    static uint32_t CrashDumpCount;
    static enum DumpType dmpType;
    static wstring CrashDumpLocation;
};

