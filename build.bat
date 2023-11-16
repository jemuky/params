@echo off

zig build -freference-trace -Dtarget=native-native-msvc
if %ERRORLEVEL% neq 0 (
    @echo build failed
    exit /b
)

@REM zig-out\bin\params.exe

echo.
