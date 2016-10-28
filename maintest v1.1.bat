@echo off
setlocal EnableDelayedExpansion

set "is_help="
call :GETOPTS %*

if defined is_help call :HELP & goto :eof

set "vdev.name=WEATHER"
set "vdev.address=98D3312016CA"
set "vdev.cod=575374"

set "unknownswitch=-k"
set "unknownvalue=makarena"
set "nvdev.address=98D3312016CG"
set "nvdev.cod=57537G"

echo name: %vdev.name%
echo address: %vdev.address%
echo cod: %vdev.cod%

echo execpath: %execpath%
echo execname: %execname%
echo.
:INIT
set "ret="
set /p ret=Init variable is ok? [y/n]
if not defined ret goto INIT
if !ret!==y goto SILENT_TEST
if !ret!==n goto :eof
goto INIT


::SILENT TESTS
:SILENT_TEST
set "ret="
set /p ret=For the moment will be preformed silent test (all of them) [ENTER/s=skip/q]
cls
if !ret!==s goto COMMON_TEST
if !ret!==q goto :eof
call :EXEC "REQUEST --> silent without other parameters" "-s" 33 0 & echo. & echo.
call :EXEC "REQUEST --> silent with name" "-s -n %vdev.name%" 0 0 & echo. & echo.
call :EXEC "REQUEST --> silent with address" "-s -a %vdev.address%" 0 0 & echo. & echo.
call :EXEC "REQUEST --> silent with COD" "-s -c %vdev.cod%" 0 0 & echo. & echo.
call :EXEC "REQUEST --> COD and silent" "-c %vdev.cod% -s" 0 0 & echo. & echo.
call :EXEC "REQUEST --> silent with name, address and cod" "-s -n %vdev.name% -a %vdev.address% -c %vdev.cod%" 0 0 & echo. & echo.
call :EXEC "REQUEST --> name, address and cod with silent" "-n %vdev.name% -a %vdev.address% -c %vdev.cod% -s" 0 0 & echo. & echo.
call :EXEC "REQUEST --> silent with help" "-s -h" 0 0 & echo. & echo.
call :EXEC "REQUEST --> help with silent" "-h -s" 0 0 & echo. & echo.
call :EXEC "REQUEST --> unknown switch with silent" "%unknownswitch% -s" 32 0 & echo. & echo.
call :EXEC "REQUEST --> unknown value with silent" "%unknownvalue% -s" 32 0 & echo. & echo.
call :EXEC "REQUEST --> silent with unknown switch" "-s %unknownswitch%" 32 0 & echo. & echo.
call :EXEC "REQUEST --> silent with unknown value" "-s %unknownvalue%" 32 0 & echo. & echo.
call :EXEC "REQUEST --> silent with switch n without value and valid address" "-s -n -a %vdev.address%" 32 0 & echo. & echo.
call :EXEC "REQUEST --> silent with switch a without address and valid name" "-s -a -n %vdev.name%" 32 0 & echo. & echo.
call :EXEC "REQUEST --> switch a without address and valid name with silent" "-a -n %vdev.name% -s" 32 0 & echo. & echo.
call :EXEC "REQUEST --> silent with switch a with not hex address" "-s -a %nvdev.address%" 32 0 & echo. & echo.
call :EXEC "REQUEST --> silent with switch a with not hex address and valid name" "-s -a %nvdev.address% -n %vdev.name%" 32 0 & echo. & echo.
call :EXEC "REQUEST --> switch a with not hex address and valid name in silent" "-a %nvdev.address% -n %vdev.name% -s" 32 0 & echo. & echo.
call :EXEC "REQUEST --> silent with valid COD and switch n without value" "-s -c %vdev.cod% -n" 32 0 & echo. & echo.
call :EXEC "REQUEST --> switch n without value and silent and valid address" "-n -s -a %vdev.address%" 32 0 & echo. & echo.
call :EXEC "REQUEST --> switch c without value and silent and valid name" "-c -s -n %vdev.name%" 32 0 & echo. & echo.
set "ret=" & set /p ret=Silent test was been performed. Check everything is fine to this point. [ENTER to follow]
set "ret=" & set /p ret=Repeat? [y/n]
if !ret!==y goto SILENT_TEST

::COMMON TEST
:COMMON_TEST
cls
set "ret=" & set /p ret=For the moment will be preformed common test (every individual) [ENTER to start or s=SKIP to ommit]
cls
if !ret!==s goto CHOOSEN_TEST

call :EXEC "REQUEST --> unknown switch without value" "%unknownswitch%" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> unknown value" "%unknownvalue%" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> switch n without value" "-n" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> switch c without value" "-c" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> switch a without value with valid name" "-a -n %vdev.name%" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> valid address and switch n without value" "-a %vdev.address% -n" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> not valid hex address" "-a %nvdev.address%" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> not valid hex COD" "-c %nvdev.cod%" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> verbose and switch n without value" "-v -n" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> verbose and switch c without value" "-v -c" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> switch c without value with verbose" "-c -v" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> verbose with switch a without value and valid name" "-v -a -n %vdev.name%" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> verbose with valid address and switch n without value" "-v -a %vdev.address% -n" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> valid address with switch n without value and verbose" "-a %vdev.address% -n -v" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> verbose and not valid hex address" "-v -a %nvdev.address%" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> not valid hex COD and verbose" "-c %nvdev.cod% -v" 32 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> valid address and COD" "-a %vdev.address% -c %vdev.cod%" 0 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> empty callpath" "" 0 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> verbose and valid name" "-v -n %vdev.name%" 0 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> verbose and valid name, address and COD" "-v -n %vdev.name% -a %vdev.address% -c %vdev.cod%" 0 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> verbose empty callpath" "-v" 0 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> help" "-h" 0 1 & echo. & echo. & set "ret=" & set /p ret=[ENTER/q] & if !ret!==q goto :eof
call :EXEC "REQUEST --> help and verbose" "-h -v" 0 1 & echo. & echo.
set "ret=" & set /p ret=Common test was been performed [ENTER to follow]
set "ret=" & set /p ret=Repeat? [y/n]
if !ret!==y goto COMMON_TEST

::CHOOSEN
:CHOOSEN_TEST
cls
set "ret=" & set /p ret=For the moment will be preformed choosen test (every individual) [ENTER to start or s=SKIP to ommit]
cls
if !ret!==s goto SERVICE_TEST
call :EXEC "REQUEST --> choose number in range" "" 0 1 & echo. & echo.
call :EXEC "REQUEST --> choose nothing" "" 81 1 & echo. & echo.
call :EXEC "REQUEST --> choose sth other than e, end or number" "" 82 1 & echo. & echo.
call :EXEC "REQUEST --> choose number out range" "" 83 1 & echo. & echo.
call :EXEC "REQUEST --> choose e" "" 0 1 & echo. & echo.
call :EXEC "REQUEST --> choose end" "" 0 1 & echo. & echo.
call :EXEC "REQUEST --> choose valid" "-v" 0 1 & echo. & echo.
set "ret=" & set /p ret=Choosen test was been performed [ENTER to follow]
set "ret=" & set /p ret=Repeat? [y/n]
if !ret!==y goto CHOOSEN_TEST

::SERVICE
:SERVICE_TEST
cls
set "ret=" & set /p ret=For the moment will be preformed service test (all of them) [ENTER to start or s=SKIP to ommit]
if !ret!==s goto NO_DEVICE_TEST
set "ret=" & set /p ret=Switch OFF service and type ENTER!
cls
call :EXEC "REQUEST --> silent and valid name" "-s -n %vdev.name%" 67 0 & echo. & echo.
call :EXEC "REQUEST --> verbose" "-v" 67 1 & echo. & echo.

call :EXEC "REQUEST --> empty callpath" "" 67 1 & echo. & echo.
set "ret=" & set /p ret=Service test was been performed [ENTER to follow]
set "ret=" & set /p ret=Repeat? [y/n]
if !ret!==y goto SERVICE_TEST

::NO DEVICE
:NO_DEVICE_TEST
cls
set "ret=" & set /p ret=For the moment will be preformed no device test (all of them) [ENTER to start or s=SKIP to ommit]
if !ret!==s goto MAX_DEVICE_TEST
set "ret=" & set /p ret=Switch ON service and type ENTER!
set "ret=" & set /p ret=Switch OFF all devices in range and type ENTER!
set "ret=" & set /p ret=Switch OFF all paired devices and type ENTER!
call :EXEC "REQUEST --> empty callpath" "" 67 1 & echo. & echo.
call :EXEC "REQUEST --> verbose" "-v" 67 1 & echo. & echo.
call :EXEC "REQUEST --> valid name, address and COD" "-n %vdev.name% -a %vdev.address% -c %vdev.cod%" 67 1 & echo. & echo.
call :EXEC "REQUEST --> valid name, address and COD with verbose" "-n %vdev.name% -a %vdev.address% -c %vdev.cod% -v" 67 1 & echo. & echo.
call :EXEC "REQUEST --> valid name, address and COD with silent" "-n %vdev.name% -a %vdev.address% -c %vdev.cod% -s" 67 0 & echo. & echo.
set "ret=" & set /p ret=No device test was been performed [ENTER to follow]
set "ret=" & set /p ret=Repeat? [y/n]
if !ret!==y goto NO_DEVICE_TEST

::MAX DEVICE
:MAX_DEVICE_TEST
cls
set "ret=" & set /p ret=For the moment will be preformed max devices test (all of them) [ENTER to start or s=SKIP to ommit]
if !ret!==s goto NO_REQUESTED_DEVICE_TEST
set "ret=" & set /p ret=Switch ON all your devices, set MAX to small number and type ENTER!
call :EXEC "REQUEST --> empty callpath" "" 0 1 & echo. & echo.
call :EXEC "REQUEST --> verbose" "-v" 0 1 & echo. & echo.
call :EXEC "REQUEST --> valid name, address and COD" "-n %vdev.name% -a %vdev.address% -c %vdev.cod%" 0 1 & echo. & echo.
call :EXEC "REQUEST --> valid name, address and COD with verbose" "-n %vdev.name% -a %vdev.address% -c %vdev.cod% -v" 0 1 & echo. & echo.
call :EXEC "REQUEST --> valid name, address and COD with silent" "-n %vdev.name% -a %vdev.address% -c %vdev.cod% -s" 0 0 & echo. & echo.
set "ret=" & set /p ret=Max device test was been performed [ENTER to follow]
set "ret=" & set /p ret=Repeat? [y/n]
if !ret!==y goto MAX_DEVICE_TEST

::NO REQUESTED DEVICE
:NO_REQUESTED_DEVICE_TEST
cls
set "ret=" & set /p ret=For the moment will be preformed no requested devices test (all of them) [ENTER to start or s=SKIP to ommit]
if !ret!==s goto REQUESTED_DEVICE_DIFFERENCE_TEST
set "ret=" & set /p ret=Switch OFF your requested device, unpair this device and type ENTER!
call :EXEC "REQUEST --> valid name, address and COD" "-n %vdev.name% -a %vdev.address% -c %vdev.cod%" 66 1 & echo. & echo.
call :EXEC "REQUEST --> valid name, address and COD with verbose" "-n %vdev.name% -a %vdev.address% -c %vdev.cod% -v" 66 1 & echo. & echo.
call :EXEC "REQUEST --> valid name, address and COD with silent" "-n %vdev.name% -a %vdev.address% -c %vdev.cod% -s" 66 0 & echo. & echo.
set "ret=" & set /p ret=no requested devices test was been performed [ENTER to follow]
set "ret=" & set /p ret=Repeat? [y/n]
if !ret!==y goto NO_REQUESTED_DEVICE_TEST

::REQUESTED DEVICE ARE DIFFERENCE
:REQUESTED_DEVICE_DIFFERENCE_TEST
cls
set "ret=" & set /p ret=For the moment will be preformed requested devices difference test (all of them) [ENTER to start or s=SKIP to ommit]
if !ret!==s goto :eof
set "ret=" & set /p ret=Switch ON your requested device and type ENTER!
call :EXEC "REQUEST --> difference name, address and COD" "-n AMARENA -a %vdev.address% -c %vdev.cod%" 0 1 & echo. & echo.
call :EXEC "REQUEST --> valid name, address+1 and COD with verbose" "-n %vdev.name% -a 98D3312016CB -c %vdev.cod% -v" 0 1 & echo. & echo.
call :EXEC "REQUEST --> valid name, address and COD+1 with silent" "-n %vdev.name% -a %vdev.address% -c 575375 -s" 0 0 & echo. & echo.
set "ret=" & set /p ret=requested devices difference test was been performed [ENTER to follow]
set "ret=" & set /p ret=Repeat? [y/n]
if !ret!==y goto REQUESTED_DEVICE_DIFFERENCE_TEST

exit /B

:GETOPTS
 if /I "%~1" equ "-p" (set execpath=%2 & shift)
 if /I "%~1" equ "-n" (set execname=%2 & shift)
 if /I "%~1" equ "-h" (set is_help=1)
 shift
if not (%1)==() goto GETOPTS
exit /B 0

:EXEC
::%1-message %2-call params %3-expected return code %4-is clear stdout (0/1)
set "logfilename=output.tmp"
set "param=%~2"
echo %1
echo.
echo START: start "maintest" /D %execpath% /B /W %execname%%param%
echo ==================================================
if %4==0 (
  >%logfilename% start "maintest" /D %execpath% /B /W %execname%%param%
) else (
  start "maintest" /D %execpath% /B /W %execname%%param% 
)
echo ==================================================
echo STOP
echo.
if !errorlevel! equ %3 (echo "RESPONSE --> ERRORLEVEL is OK: %3") else (echo "RESPONSE --> ERRORLEVEL is NOT OK: get !errorlevel! opposite expected %3")
if %4==0 (
  for /f %%A in ("%logfilename%") do set "logfilesize=%%~zA" 
  if !logfilesize! == 0 (
    echo "RESPONSE --> STDOUT is CLEAR"
  ) else (
    echo "RESPONSE --> STDOUT is NOT EMPTY"
    echo ..................................................
    type %logfilename%
    echo ..................................................
  ) 
)
type nul > %logfilename%

exit /B

:HELP
echo.
echo Console receiver TEST BATCH SCIRPT by Dawid Karminski v1.1 10/2016
echo.
echo "maintest v1.1.bat" [ -p ^<execfilepath^> ^| -n ^<execfilename^> ^| -h ]
echo.
exit /B