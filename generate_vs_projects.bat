@echo off
SET BUILDFOLDER=build

IF NOT EXIST %BUILDFOLDER%\ (
mkdir %BUILDFOLDER% && echo INFO: %BUILDFOLDER% created
)

cd build
cmake -G "Visual Studio 11" ..
cd ..