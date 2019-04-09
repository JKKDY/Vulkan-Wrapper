@echo off
call env/Scripts/activate.bat
start python -m setup_dependencies_examples.py
call env/Scripts/deactivate.bat