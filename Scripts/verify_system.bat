@echo off
REM verify_system.bat
REM 用户友好的包装脚本：可以双击运行

chcp 65001 >nul
echo 正在启动系统验证...
echo.

REM 使用 Bypass 执行策略运行 PowerShell 脚本
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0verify_system.ps1" %*

echo.
echo 按任意键关闭窗口...
pause >nul
