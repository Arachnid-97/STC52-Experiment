for /f "delims=" %%i in ('dir Objects\ /a-d /b /s') do (if not %%~xi==.hex (del /s /a /q "%%~si"))


del /q/s Listings\*.*

cd Project\RVC51(uv5)
del /q *.txt
del /q *.bak
del /q *.plg
del /q *.dep
del /q *.Administrator

exit