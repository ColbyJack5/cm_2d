@echo off
set "WSHELL=C:/msys2/opt/wonderful/wonderful_shell.cmd"

:: Run bash and execute a command string (-c)
:: -l ensures a login shell (loads your .bashrc / toolkit profiles)
call "%WSHELL%" -c "cd C:/devkitPro/nflib/template && make && pause && start template.nds"
pause