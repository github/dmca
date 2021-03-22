@:: download cygwin setup.exe and perform an update
@:: mostly inspired by https://github.com/ocaml/opam/blob/master/appveyor.yml
appveyor DownloadFile https://cygwin.com/setup-x86_64.exe -FileName cygwinsetup.exe
cygwinsetup -q -R %CYG_ROOT% -s %CYG_MIRROR% -l %CYG_CACHE% >nul
exit /b %ERRORLEVEL%
