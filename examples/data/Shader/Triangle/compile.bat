@echo off
for /r %%i in (*.frag, *.vert) do "../glslangValidator.exe" -V "%%i" -o "%%i.spv"
pause

