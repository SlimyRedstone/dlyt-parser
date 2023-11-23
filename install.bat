@echo off
cls
echo Creating folder...
mkdir "%USERPROFILE%/.dlyt"
echo Copying main file...
robocopy output %USERPROFILE%/.dlyt/ main.exe youtube_downloader.exe
cd %USERPROFILE%/.dlyt/
echo Renaming main file...
del youtube_downloader.exe /F
ren main.exe youtube_downloader.exe

@REM echo Applying register key...
@REM reg import ./ytdl_protocol_registers.reg
@REM reg add HKEY_CLASSES_ROOT\dlyt\shell\open\command /f /ve /d "\"%USERPROFILE%/.dlyt/youtube_downloader.exe\" \"-l %1\""
@REM reg add HKEY_CLASSES_ROOT\dlyt 

@REM [HKEY_CLASSES_ROOT\dlyt\shell\open\command]
@REM @="\"%USERPROFILE%\\.dlyt\\youtube_downloader.exe\" \"-l %1\""
@REM [HKEY_CLASSES_ROOT\dlyt]
@REM "URL Protocol"=""
@REM @="URL:dlyt Protocol"

pause