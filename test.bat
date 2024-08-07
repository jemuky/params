@echo off

xmake -y
if %ERRORLEVEL% neq 0 (
    @echo build failed
    exit /b
)

build\windows\x64\release\params_test.exe --exist=false

echo.
