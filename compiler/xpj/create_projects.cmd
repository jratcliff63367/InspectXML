@echo off

set XPJ="xpj4.exe"

%XPJ% -v 1 -t VC14 -p WIN32 -x InspectXML.xpj

cd ..
cd vc14win32

goto cleanExit

:pauseExit
pause

:cleanExit

