::make_setup.bat
::
::use innosetup 6.x to create an installer
::
::Version  2022-11-29
::Author   Boulette 42

@echo off
setlocal

set x86_x64_=
if /i "%1" == "x86" (
  set x86_x64_=x86
) else if /i "%1" == "x64" (
  set x86_x64_=x64
)
if "%x86_x64_%" == "" (
  echo Aufruf mit
  echo   %~n0 x86^|x64
  exit /b 1
)

set iss_inc_file_=setup\install_mode.inc
set my_dir_=%~dp0
set dist_=%my_dir_%dist

if not exist "%QTDIR%\bin\Qt5Core.dll" (
  echo "QTDIR not set"
  exit /b 1
)

call :_SET_INNOSETUP_INSTALL_DiR
if errorlevel 1 (
  echo "innosetup not found"
  exit /b 1
)

if "%x86_x64_%" == "x86" (
  set petu_src_dir_=%mydir_%exe32\release
  set vc_src_dir_=%WINDIR%\SysWOW64
) else (
  set petu_src_dir_=%mydir_%exe\release
  set vc_src_dir_=%WINDIR%\System32
)

if not exist %petu_src_dir_%\petu.exe (
  echo "petu.exe not build"
  exit /b 1
)

set setup_prefix_=setup-petanque-turnier
rmdir /s /q %dist_% >NUL 2>NUL
del %my_dir_%installer\%setup_prefix_%-%x86_x64_%-*.exe >NUL 2>NUL

echo copying petu-files...
call :_COPY_FILE_TO_DIR %petu_src_dir_%\petu.exe %dist_%\exe
call :_COPY_FILE_TO_DIR %petu_src_dir_%\petu_de.qm %dist_%\exe
call :_COPY_FILE_TO_DIR %petu_src_dir_%\petu_en.qm %dist_%\exe
call :_COPY_FILE_TO_DIR %petu_src_dir_%\petu_fr.qm %dist_%\exe
call :_COPY_FILE_TO_DIR %mydir_%lizenzen.html %dist_%\exe
call :_COPY_FILE_TO_FILE %mydir_%petanque-turnier.dok %dist_%\exe\petanque-turnier.dok.txt

echo copying Qt-files...
call :_COPY_FILE_TO_DIR %QTDIR%\bin\Qt5Core.dll %dist_%\exe
call :_COPY_FILE_TO_DIR %QTDIR%\bin\Qt5Gui.dll %dist_%\exe
call :_COPY_FILE_TO_DIR %QTDIR%\bin\Qt5Widgets.dll %dist_%\exe
call :_COPY_FILE_TO_DIR %QTDIR%\bin\libGLESv2.dll %dist_%\exe
call :_COPY_FILE_TO_DIR %QTDIR%\plugins\platforms\qwindows.dll %dist_%\exe\platforms
call :_COPY_FILE_TO_DIR %QTDIR%\plugins\styles\qwindowsvistastyle.dll %dist_%\exe\styles
call :_COPY_FILE_TO_FILE %QTDIR%\translations\qtbase_de.qm %dist_%\exe\qt_de.qm
call :_COPY_FILE_TO_FILE %QTDIR%\translations\qtbase_en.qm %dist_%\exe\qt_en.qm
call :_COPY_FILE_TO_FILE %QTDIR%\translations\qtbase_fr.qm %dist_%\exe\qt_fr.qm

echo copying vc-runtime-files...
call :_COPY_FILE_TO_DIR "%vc_src_dir_%\msvcp140.dll" %dist_%\exe
call :_COPY_FILE_TO_DIR "%vc_src_dir_%\msvcp140_1.dll" %dist_%\exe
call :_COPY_FILE_TO_DIR "%vc_src_dir_%\vcruntime140.dll" %dist_%\exe
if exist "%vc_src_dir_%\vcruntime140_1.dll" (
  call :_COPY_FILE_TO_DIR "%vc_src_dir_%\vcruntime140_1.dll" %dist_%\exe
)

echo creating '%iss_inc_file_%'...
if "%x86_x64_%" == "x86" (
  set arg_=
) else (
  set arg_=x64
)
echo ArchitecturesInstallIn64BitMode=%arg_%> %iss_inc_file_%

echo compiling setup...
"%INNOSETUP_INSTALL_DIR%\IScc" setup\petu.iss -q

if errorlevel 1 exit /b 1

call :_RENAME_SETUP
echo '%setup_name_%.exe' built in directory '%my_dir_%installer'.

exit /b 0


:_SET_INNOSETUP_INSTALL_DiR
if exist "%INNOSETUP_INSTALL_DIR%\IScc.exe" exit /b 0
set INNOSETUP_INSTALL_DIR=%ProgramFiles(x86)%\Inno Setup 6
if exist "%INNOSETUP_INSTALL_DIR%\IScc.exe" exit /b 0
exit /b 1


:_RENAME_SETUP
pushd %my_dir_%installer
for %%i in (%setup_prefix_%-*.exe) do (
  call :_CHECK_SETUP_FILE "%%~i"
)
popd
exit /b 0


:_CHECK_SETUP_FILE filename
set tmp1_=%~n1
set tmp2_=%tmp1_:x86=x%
if not "%tmp2_%" == "%tmp1_%" exit /b 1
set tmp2_=%tmp1_:x64=x%
if not "%tmp2_%" == "%tmp1_%" exit /b 1
setlocal EnableDelayedExpansion
set tmp2_=!tmp1_:%setup_prefix_%-=-!
endlocal && set setup_name_=%setup_prefix_%-%x86_x64_%%tmp2_%
rename "%~1" %setup_name_%.exe
exit /b 0


:_COPY_FILE_TO_FILE from to
if not exist "%~1" (
  echo  ! Datei '%~1' nicht vorhanden
  exit /b 1
)
if not exist "%~dp2*" (
  mkdir "%~dp2" >NUL
)
copy "%~1" "%~2" 2>&1 >NUL
exit /b 0


:_COPY_FILE_TO_DIR from to
if not exist "%~1" (
  echo  ! Datei '%~1' nicht vorhanden
  exit /b 1
)
if not exist "%~2\*" (
  mkdir "%~2" >NUL
)
copy "%~1" "%~2" 2>&1 >NUL
exit /b 0

