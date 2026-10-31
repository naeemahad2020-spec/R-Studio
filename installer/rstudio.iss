; Inno Setup script. Build with: iscc installer\rstudio.iss
; Run windeployqt on RStudio.exe into dist\ first (see README).
#define AppVer "0.1.0"
[Setup]
AppName=R STUDIO
AppVersion={#AppVer}
DefaultDirName={autopf}\R STUDIO
DefaultGroupName=R STUDIO
OutputBaseFilename=RStudio-Setup-{#AppVer}
OutputDir=..\release
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
Compression=lzma2
SolidCompression=yes
UninstallDisplayIcon={app}\RStudio.exe

[Files]
Source: "..\dist\*"; DestDir: "{app}"; Flags: recursesubdirs ignoreversion

[Icons]
Name: "{group}\R STUDIO"; Filename: "{app}\RStudio.exe"
Name: "{autodesktop}\R STUDIO"; Filename: "{app}\RStudio.exe"

; User settings live in %APPDATA% and are intentionally NOT removed on uninstall.
