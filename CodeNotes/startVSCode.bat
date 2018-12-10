@ echo off
REM open VSCode with Cygwin so we don't have to deal with all the env nonsense (also use a nifty trick to replace the backslashes with forward slashes)
C:\cygwin64\bin\bash --login -c "code '%cd:\=/%' >file"
EXIT