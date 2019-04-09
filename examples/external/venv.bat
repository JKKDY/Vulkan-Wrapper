@echo off
python -m venv env
call env/Scripts/activate.bat
pip install GitPython
call env/Scripts/deactivate.bat
pause