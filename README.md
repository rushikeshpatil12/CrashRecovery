# CrashRecovery
=============
CrashRecovers demonstrates recovery mechanism from crashes/hangs using a simple do-nothing app. This app stays alive, recovers and restarts from a crash or a hang situation. 
In case of crashes/hangs, it creates and saves the application state(memory dump) that can be used for later troubleshooting.

Platforms:
==========
- Windows Vista & Windows server 2008.
- Windows 7 & Windows server 2008 R2.
- Windows 8 & Windows server 2012.
- Windows 8.1 & Windows server 2012 R2.
- Windows 10 & Windows server 2016.
- Both x86 and x64 are supported.

Design: 
=======
- CrashRecovery.exe designed to work on windows platforms.
- It handles the WOW64 redirections for 32-bit(esp. registry).
- CrashRecovery uses WER(Windows Error Reporting) APIs to get crash notification, recovery callbacks, system event callbacks, crashdump, etc.
- WER APIs requires app to be running for, at least, 60 seconds to avoid cyclic restart.
- Crashdump: Only the last crash dump is saved to preserve the diskspace.

Alternate design considerations:
================================
- Another approach is to create two instances of the executable. One instance will run the application while other monitors the first app.
- On crash monitor app can save the crash dump and restart the app if needed.  
- Platform independent solution could prefer this approach.
 
Usage:
======
- Run CrashRecovery.exe with admin command prompt(App uses registry & WER settings thus needs admin rights.)
- App keeps crashing every 61 seconds, collecting crashdump. 
- Press Ctr+C to exit.
- By default crashdump is created in the module directory with name similar to CrashRecovery.exe.11748.dmp.

Note: 
=====
- Settings such as minidump/fulldump are configurable in Settings class(Settings.h).
- Older versions of windows show the crash notification(UI) by default. If required this can be disabled with registry setting(HKEY_CURRENT_USER\Software\ Microsoft\Windows\Windows Error Reporting\DontShowUI = 1).

Testing:
========
Testing is done on windows 7 & windows 10 x64 and x86 platforms.

Assumption:
===========
1.Windows WER would be enabled and active for this application. 
- It's possible that WER(Windows error reporting) can be replaced with some other Error handling tool.

TODO: 
====
- Improvement in logging framework.
