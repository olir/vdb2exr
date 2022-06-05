@echo off

if "%~1" equ ":main" (
  shift /1
  goto main
)
cmd /d /c "%~f0" :main %*
popd
exit /b

:main
echo Main actions go here
pushd 3RDPARTY
..\x64\Release\vdb2exr.exe ..\Testdata\wdas_cloud_sixteenth.vdb ..\Testdata\wdas_cloud_sixteenth.exr
..\x64\Release\vdb2exr.exe ..\Testdata\wdas_cloud_eighth.vdb ..\Testdata\wdas_cloud_eighth.exr
..\x64\Release\vdb2exr.exe ..\Testdata\wdas_cloud_quarter.vdb ..\Testdata\wdas_cloud_quarter.exr
exit /b
