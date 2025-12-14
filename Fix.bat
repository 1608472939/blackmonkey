@echo off
echo ========================================================
echo 正在强制生成 Visual Studio 项目文件 (By Command Line)...
echo ========================================================

:: 这里的路径是根据你刚才截图里的报错信息推断的
:: 如果你的引擎安装位置变了，请修改下面这一行
set UBT_PATH="D:\ue\UE_5.7\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"

:: 这是你的项目路径
set PROJECT_PATH="D:\ue\bm\bm.uproject"

:: 执行生成命令
%UBT_PATH% -projectfiles -project=%PROJECT_PATH% -game -rocket -progress

echo.
echo ========================================================
echo 如果上面显示 "Project files generated successfully"
echo 那么恭喜你，.sln 文件已经回来了！
echo ========================================================
pause