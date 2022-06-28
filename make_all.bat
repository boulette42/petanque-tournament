::make_all.bat
::
::build petu.exe and installer
::
::Version  2021-03-28
::Author   Boulette42

@echo off
setlocal

call :_CHECK_VC_VERSION
if errorlevel 1 (
  echo VS2019/VS2022 not found: call 'vcvars32.bat x86^|x64'
  exit /b 1
)

set x86_x64_=%VSCMD_ARG_TGT_ARCH%
if "%x86_x64_%" == "amd64" (
  set x86_x64_=x64
) else (
  if not "%x86_x64_%" == "x86" (
    if not "%x86_x64_%" == "x64" (
      echo unknown target architecture '%x86_x64_%': call 'vcvars32.bat x86^|x64'
      exit /b 1
    )
  )
)

call :_FIND_QTDIR
if not exist "%QTDIR%\bin\qmake.exe" ( 
  echo Qt not found: install Qt or set environment variable 'QTDIR'
  exit /b 1
)

call :_FIND_INNOSETUP_INSTALL_DiR
if errorlevel 1 (
  echo "innosetup not found: install inno setup or set environment variable 'INNOSETUP_INSTALL_DIR'"
  exit /b 1
)

set errlog_="%~dp0error.log"

echo creating makefile...
del Makefile >NUL 2>NUL
del Makefile.Release >NUL 2>NUL
del Makefile.Debug >NUL 2>NUL
del .qmake.stash >NUL 2>NUL
qmake petu.pro  2>&1 >%errlog_%
if errorlevel 1 (
  echo qmake failed: see '%errlog_%'
  echo did you set 'QTDIR' according to x86^|x64?
  exit /b 1
)

echo nmaking binaries...
nmake -nologo /A Release >>%errlog_% 2>&1
if errorlevel 1 (
  echo build failed: see '%errlog_%'
  exit /b 1
)

echo building setup...
call make_setup.bat %x86_x64_%

exit /b %errorlevel%


:_CHECK_VC_VERSION
if not defined VS160COMNTOOLS (
  if not defined VS170COMNTOOLS (
    exit /b 1
  )
)
exit /b 0


:_FIND_QTDIR
if not "%QTDIR%" == "" exit /b 0
for /f "tokens=1,2,3,4 delims=;" %%a in ("c;d;e;f") do (
  call :_CHECK_QTDIR %%a
  if errorlevel 1 call :_CHECK_QTDIR %%b
  if errorlevel 1 call :_CHECK_QTDIR %%c
  if errorlevel 1 call :_CHECK_QTDIR %%d
  if errorlevel 1 exit /b 1
)
set PATH=%path%;%QTDIR%\bin
exit /b 0


:_CHECK_QTDIR drive
if exist %1:\Qt\5.15.2\* (
  if "%x86_x64_%" == "x86" (
    echo setting QTDIR to '%1:\Qt\5.15.2\msvc2019'
    set QTDIR=%1:\Qt\5.15.2\msvc2019
  ) else (
    set QTDIR=%1:\Qt\5.15.2\msvc2019_64
    echo setting QTDIR to '%1:\Qt\5.15.2\msvc2019_64'
  )
  exit /b 0
)
exit /b 1


:_FIND_INNOSETUP_INSTALL_DiR
if exist "%INNOSETUP_INSTALL_DIR%\IScc.exe" exit /b 0
set INNOSETUP_INSTALL_DIR=%ProgramFiles(x86)%\Inno Setup 6
if exist "%INNOSETUP_INSTALL_DIR%\IScc.exe" exit /b 0
exit /b 1

