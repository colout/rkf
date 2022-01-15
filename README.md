# Getting Started on Windows
Taken from [Shawn Hymel guide](https://shawnhymel.com/2096/how-to-set-up-raspberry-pi-pico-c-c-toolchain-on-windows-with-vs-code/#Directory_Setup)
1. Create the folder using the following command in command prompt:
```
md C:\VSARM C:\VSARM\armcc C:\VSARM\lib C:\VSARM\mingw C:\VSARM\openocd
```
2. Install latest win32 [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).  The install location *MUST* be `C:\VSARM\armcc\<arm toolchain version>` where `<arm toolchain version>` is the the version number (already populated as the deepest subfolder in the path).  Make sure "Add path to environment variable" is selected at the end of the install. 

3. Install [MinGW-w64](https://www.mingw-w64.org/downloads/#mingw-builds) (click Sourceforge to download the installer for Mingw-w64). Use default settings except make the install folder `C:\VSARM\mingw`.  After the install, run the following in a command prompt:
```
echo mingw32-make %* > C:\VSARM\mingw\mingw32\bin\make.bat
```
4. Install [CMake](https://cmake.org/download/) msi file for x64.  Select "Add CMake to the system PATH for all users".  Use default path for CMake since it's system wide.

5. Install [Python 3.9](https://www.python.org/downloads/). Use recommended settings and add to PATH.  At the end, ensure MAX_PATH is disabled.  If not asked, do it manually (regedit, `Computer\HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\FileSystem`, set `LongPathsEnabled=1` as hex value.)

6.  Install [git](https://git-scm.com/download/win)

7. Install openocd for deployments.  Download zipped [Windows 64 bit builds](https://github.com/xpack-dev-tools/openocd-xpack/releases).  Extract to C:\VSARM\openocd,

8. Add `C:\VSARM\mingw\mingw32\bin` and `C:\VSARM\openocd\bin` to PATH system variable.  

9. Clone repo, open folder in VSCode, then do the following (can be in vscode terminal):
```
git submodule update --init --recursive
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
make
```

10. In VSCode, install the extensions and choose GCC x.x.x arm-none-eabi as the kit.  
```
code -–install-extension marus25.cortex-debug
code -–install-extension ms-vscode.cmake-tools
code -–install-extension ms-vscode.cpptools
```

# Serial
1. Leader read mode with pullup.  Follower write mode pulled down.

2.  Leader waits for follower to go to read mode with pullup.  When ready, follower gopes to read mode with pull up.

3.  Leader executes send sync.

4.  Leader returns to float.  Follower returns to ground