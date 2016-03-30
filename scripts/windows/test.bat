@echo off
setlocal


set BIN_DIR=bin\windows
set INCLUDE_DIR=include
set OBJ_DIR=obj\windows
set SRC_DIR=src


:: locate toolchain
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
set OBJS=
set LINK=


goto:main


:mkdir_for_file
	setlocal
	set dir=%~dp1
	if not exist "%dir%" mkdir "%dir%"
	endlocal
exit /b 0


:: compile_c <path/to/file/in/SRC_DIR>
:compile_c
	call:mkdir_for_file "%OBJ_DIR%\%1.obj"
	set OBJS=%OBJS% "%OBJ_DIR%\%1.obj"
	cl /nologo /I%INCLUDE_DIR% /c "%SRC_DIR%\%1" /Fo"%OBJ_DIR%\%1.obj"
	if %errorlevel% neq 0 goto:error
exit /b %errorlevel%


:: link <path/to/app/in/BIN_DIR>
:link
	call:mkdir_for_file "%1"
	echo.%1
	link /nologo /OUT:"%1" %OBJS%
exit /b %errorlevel%


:main


set APP=%BIN_DIR%\main.exe
call:compile_c tests.c
call:link %APP%
%APP%

:error