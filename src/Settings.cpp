#include "stdafx.h"
#include "Settings.h"

// How long should app perform its work? min: 60s.
uint32_t Settings::WorkIntervalInSec = 61;
// Duration required to recover from a crash.
uint32_t Settings::CrashRecoveryIntervalInSec = RECOVERY_DEFAULT_PING_INTERVAL;
// How many crashdumps to save? New crashes overwrite the oldest crashdumps.
uint32_t Settings::CrashDumpCount = 1;
// Crashdump type? Mini, full, custom?
enum DumpType Settings::dmpType = DumpType::MiniDump;
