@echo off
python -m venv env
call env/Scripts/activate.bat
pip install GitPython
pip install beautifulsoup4
call env/Scripts/deactivate.bat
pause