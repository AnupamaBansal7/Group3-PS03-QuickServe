@echo off
if not exist quickserve.exe (
    echo quickserve.exe not found. Building first...
    call build.bat
)
quickserve.exe %*
