@echo off
call env/Scripts/activate.bat
start python -m setup_dependencies.py
call env/Scripts/deactivate.bat