# Getting Started on Windows
Taken from [Shawn Hymel guide](https://shawnhymel.com/2096/how-to-set-up-raspberry-pi-pico-c-c-toolchain-on-windows-with-vs-code/#Directory_Setup)
1. Create the folder using the following command in command prompt:
```
md C:\VSARM C:\VSARM\armcc C:\VSARM\lib C:\VSARM\mingw C:\VSARM\sdk
```
2. Install latest win32 [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).  The install location *MUST* be `C:\VSARM\armcc\<arm toolchain version>` where `<arm toolchain version>` is the the version number (already populated as the deepest subfolder in the path).  Make sure "Add path to environment variable" is selected at the end of the install. 

3. Install [MinGW-w64](https://www.mingw-w64.org/downloads/#mingw-builds) (click Sourceforge to download the installer for Mingw-w64). Use default settings except make the install folder `C:\VSARM\mingw`.  After the install, run the following in a command prompt:
```
echo mingw32-make %* > C:\VSARM\mingw\mingw32\bin\make.bat
```
4. Install [CMake](https://cmake.org/download/) msi file for x64.  Select "Add CMake to the system PATH for all users".  Use default path for CMake since it's system wide.

5. Install [Python 3.9](https://www.python.org/downloads/). Use recommended settings and add to PATH.  At the end, ensure MAX_PATH is disabled.  If not asked, do it manually (regedit, `Computer\HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\FileSystem`, set `LongPathsEnabled=1` as hex value.)

6.  Install [git](https://git-scm.com/download/win)

7. Add `C:\VSARM\mingw\mingw32\bin' to PATH system variable.  

8. ~~Add PICO_SDK_PATH=C:\VSARM\sdk\pico\pico-sdk as user variable.~~  We'll do this in the project and use the local git dir in vscode

9. From project folder in this git directory, run the following (can be in vscode terminal):
```
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cd blink
make
```

10. In VSCode, choose GCC x.x.x arm-none-eabi as the kit.  


# OLD (should use mingw instead)
## Getting Started
1. Install Raspberry Pi Pico SDK pre-reqs from the [RP2040 Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) (Chapter 1 for Linux, Chapter 9 for Win, MacOS).  For Windows, I used the [Bootstrap Script](https://github.com/ndabas/pico-setup-windows) 
2. At least for windows, run this in the VS 2019 terminal:
```
mkdir build
cd build
cmake .. -G "NMake Makefiles"
nmake
```

## Building and Modifying the App
I use VSCode for my IDE in Windows as described in the [RP2040 Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf).  
1. Open the `Visual Studio 2019 Developer Command Prompt` and type "code" to open the IDE with the toolchain properly configured.