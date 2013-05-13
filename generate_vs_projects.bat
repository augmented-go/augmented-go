cd build
cmake -G "Visual Studio 11" .. && python ../modify_vs_projects.py
REM cmake -G "Visual Studio 11" -T "v120_CTP_Nov2012" ..
cd ..