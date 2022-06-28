; Inno Setup Script Version 6.2
;
; Version  2022-06-28
; Autor    Boulette42

#include "../src/version.h"

[Languages]
Name: de; MessagesFile: "compiler:Languages\german.isl"

[Setup]
AppName={#MY_PRODUCT_NAME}
AppVerName={#MY_PRODUCT_NAME} {#MY_VERSION_STRING}
AppPublisher={#MY_COMPANY_NAME}
AppVersion={#MY_MAJOR_VERSION}.{#MY_MINOR_VERSION}.{#MY_MAJOR_SUB_VERSION}.{#MY_MINOR_SUB_VERSION}
WizardStyle=modern
DefaultDirName={autopf}\{#MY_PRODUCT_NAME}
DefaultGroupName={#MY_PRODUCT_NAME}
UninstallDisplayIcon={app}\petu.exe
PrivilegesRequired=admin
CreateUninstallRegKey=yes
Compression=lzma2
SolidCompression=yes
; Output
OutputDir=..\installer
OutputBaseFilename=setup-petanque-turnier-{#MY_VERSION_STRING}

[Files]
Source: "..\dist\exe\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs;

[Icons]
Name: "{group}\{#MY_PRODUCT_NAME}"; Filename: "{app}\petu.exe"

[Run]
Filename: "{app}\petu.exe"; Description: "Programm starten"; Flags: postinstall
