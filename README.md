# R STUDIO — Stage 1 skeleton (v0.1.0)

**Status: UNBUILT AND UNTESTED.** This source was written in a sandbox with no
Qt/compiler/Windows. It has not been compiled. Expect small build errors on first build.

## What exists
- Qt 6 main window, 18-section navigation, dark/light theme (persisted in SQLite)
- Logger that redacts RTMP/RTMPS stream keys and key/password/token values
- SQLite settings store
- Unit tests for log redaction
- Inno Setup installer script

## What does NOT exist yet
Capture, scenes, audio, encoding, recording, streaming, multistreaming,
automation, overlays, virtual camera, AI, analytics. Pages say so in the UI.

## Build (Windows 10/11 x64)
1. Install Visual Studio 2022 (Desktop C++), CMake 3.21+, Qt 6.5+ (MSVC 2022 64-bit), Inno Setup 6.
2. ```
   cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH=C:\Qt\6.x.x\msvc2022_64
   cmake --build build --config Release
   ctest --test-dir build -C Release
   ```
3. Package:
   ```
   mkdir dist
   copy build\Release\RStudio.exe dist\
   C:\Qt\6.x.x\msvc2022_64\bin\windeployqt.exe --release dist\RStudio.exe
   iscc installer\rstudio.iss
   ```
4. Test the installer on a clean Windows VM before sharing.

## Next: Stage 2 (scenes, sources, preview, recording) — see docs/STATUS.md

## Stage 2 (part 1) additions
- Needs `ffmpeg.exe` (Windows build from gyan.dev or BtbN). Put it next to RStudio.exe
  or pick it in Recording Studio. Hardware encoders: h264_nvenc / h264_amf / h264_qsv.
- Recording Studio: choose folder, encoder, FPS, bitrate -> Start/Stop. Output is MKV.
- Tests: `ctest --test-dir build -C Release` (logger redaction + recording arguments).

## Build without installing anything (GitHub Actions)
1. Create a free GitHub account and a new repository.
2. Upload the CONTENTS of this folder (CMakeLists.txt must be at the repository root).
3. Open the "Actions" tab -> "Build R STUDIO" -> wait for the run to finish.
4. Download the "RStudio-windows" artifact: RStudio.exe folder + installer.
If the run fails, open it, copy the red error text and send it for fixing.
